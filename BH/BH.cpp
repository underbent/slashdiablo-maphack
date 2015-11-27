#define _DEFINE_PTRS
#include "BH.h"
#include <Shlwapi.h>
#include <psapi.h>
#include "D2Ptrs.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"
#include "Modules.h"
#include "MPQReader.h"
#include "MPQInit.h"
#include "TableReader.h"
#include "Task.h"

string BH::path;
HINSTANCE BH::instance;
ModuleManager* BH::moduleManager;
Config* BH::config;
Drawing::UI* BH::settingsUI;
Drawing::StatsDisplay* BH::statsDisplay;
bool BH::initialized;
bool BH::cGuardLoaded;
WNDPROC BH::OldWNDPROC;
map<string, Toggle>* BH::MiscToggles;
map<string, Toggle>* BH::MiscToggles2;

Patch* patches[] = {
	new Patch(Call, D2CLIENT, 0x44230, (int)GameLoop_Interception, 7),

	new Patch(Jump, D2CLIENT, 0xC3DB4, (int)GameDraw_Interception, 6),
	new Patch(Jump, D2CLIENT, 0x626C9, (int)GameAutomapDraw_Interception, 5),

	new Patch(Call, BNCLIENT, 0xEABC, (int)ChatPacketRecv_Interception, 12),
	new Patch(Call, D2MCPCLIENT, 0x69D7, (int)RealmPacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0xACE61, (int)GamePacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0x70B75, (int)GameInput_Interception, 5),
	new Patch(Call, D2MULTI, 0xD753, (int)ChannelInput_Interception, 5),
	new Patch(Call, D2MULTI, 0x10781, (int)ChannelWhisper_Interception, 5),
	new Patch(Jump, D2MULTI, 0x108A0, (int)ChannelChat_Interception, 6),
	new Patch(Jump, D2MULTI, 0x107A0, (int)ChannelEmote_Interception, 6),
	new Patch(NOP, D2CLIENT, 0x3CB7C, 0, 9),
};

Patch* BH::oogDraw = new Patch(Call, D2WIN, 0x18911, (int)OOGDraw_Interception, 5);

unsigned int index = 0;
bool BH::Startup(HINSTANCE instance, VOID* reserved) {

	BH::instance = instance;
	if (reserved != NULL) {
		cGuardModule* pModule = (cGuardModule*)reserved;
		if (!pModule)
			return FALSE;
		path.assign(pModule->szPath);
		cGuardLoaded = true;
	}
	else {
		char szPath[MAX_PATH];
		GetModuleFileName(BH::instance, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		path.assign(szPath);
		path += "\\";
		cGuardLoaded = false;
	}


	initialized = false;
	Initialize();
	return true;
}

DWORD WINAPI LoadMPQData(VOID* lpvoid){
	char szFileName[1024];
	std::string patchPath;
	UINT ret = GetModuleFileName(NULL, szFileName, 1024);
	patchPath.assign(szFileName);
	size_t start_pos = patchPath.rfind("\\");
	if (start_pos != std::string::npos) {
		start_pos++;
		if (start_pos < patchPath.size()){
			patchPath.replace(start_pos, patchPath.size() - start_pos, "Patch_D2.mpq");
		}
	}

	ReadMPQFiles(patchPath);
	InitializeMPQData();
	Tables::initTables();

	return 0;
}

void BH::Initialize()
{
	moduleManager = new ModuleManager();
	config = new Config("BH.cfg");
	config->Parse();

	if (D2GFX_GetHwnd()) {
		BH::OldWNDPROC = (WNDPROC)GetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC);
		SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)GameWindowEvent);
	}

	settingsUI = new Drawing::UI("Settings", 350, 200);

	Task::InitializeThreadPool(2);

	// Read the MPQ Data asynchronously
	//CreateThread(0, 0, LoadMPQData, 0, 0, 0);
	Task::Enqueue([]() -> void {
		LoadMPQData(NULL);
	});

	
	new ScreenInfo();
	new Gamefilter();
	new Bnet();
	new Item();
	new SpamFilter();
	new AutoTele();
	new Party();
	new ItemMover();
	new StashExport();
	new Maphack();

	moduleManager->LoadModules();

	statsDisplay = new Drawing::StatsDisplay("Stats");

	MiscToggles = ((AutoTele*)moduleManager->Get("autotele"))->GetToggles();
	MiscToggles2 = ((Item*)moduleManager->Get("item"))->GetToggles();

	// Injection would occasionally deadlock (I only ever saw it when using Tabbed Diablo
	// but theoretically it could happen during regular injection):
	// Worker thread in DllMain->LoadModules->AutoTele::OnLoad->UITab->SetCurrentTab->Lock()
	// Main thread in GameDraw->UI::OnDraw->D2WIN_SetTextSize->GetDllOffset->GetModuleHandle()
	// GetModuleHandle can invoke the loader lock which causes the deadlock, so delay patch
	// installation until after all startup initialization is done.
	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		patches[n]->Install();
	}

	if (!D2CLIENT_GetPlayerUnit())
		oogDraw->Install();

	// GameThread can potentially run oogDraw->Install, so create the thread after all
	// loading/installation finishes.
	CreateThread(0, 0, GameThread, 0, 0, 0);

	initialized = true;
}

bool BH::Shutdown() {
	if (initialized){
		moduleManager->UnloadModules();

		delete moduleManager;
		delete settingsUI;
		delete statsDisplay;

		SetWindowLong(D2GFX_GetHwnd(), GWL_WNDPROC, (LONG)BH::OldWNDPROC);
		for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
			delete patches[n];
		}

		oogDraw->Remove();
		delete config;
	}
	
	return true;
}