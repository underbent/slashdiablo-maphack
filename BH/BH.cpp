#define _DEFINE_PTRS
#include "BH.h"
#include <Shlwapi.h>
#include "D2Ptrs.h"
#include "D2Intercepts.h"
#include "D2Handlers.h"
#include "Modules.h"

string BH::path;
HINSTANCE BH::instance;
ModuleManager* BH::moduleManager;
Config* BH::config;
Drawing::UI* BH::settingsUI;
bool BH::cGuardLoaded;
WNDPROC BH::OldWNDPROC;

Patch* patches[] = {
	new Patch(Call, D2CLIENT, 0x44230, (int)GameLoop_Interception, 7),

	new Patch(Jump, D2CLIENT, 0xC3DB4,	(int)GameDraw_Interception, 6),
	new Patch(Jump, D2CLIENT, 0x626C9, (int)GameAutomapDraw_Interception, 5),

	new Patch(Call, BNCLIENT, 0xEABC, (int)ChatPacketRecv_Interception, 12),
	new Patch(Call, D2MCPCLIENT, 0x69D7, (int)RealmPacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0xACE61, (int)GamePacketRecv_Interception, 5),
	new Patch(Call, D2CLIENT, 0x70B75, (int)GameInput_Interception, 5),
	new Patch(Call, D2MULTI, 0xD753, (int)ChannelInput_Interception, 5),
	new Patch(Call, D2MULTI, 0x10781, (int)ChannelWhisper_Interception, 5),
	new Patch(Jump, D2MULTI, 0x108A0, (int)ChannelChat_Interception, 6),
	new Patch(Jump, D2MULTI, 0x107A0, (int)ChannelEmote_Interception, 6),

};

Patch* BH::oogDraw = new Patch(Call, D2WIN, 0x18911, (int)OOGDraw_Interception, 5);

unsigned int index = 0;
bool BH::Startup(HINSTANCE instance, VOID* reserved) {

	BH::instance = instance;
	if (reserved != NULL) {
		cGuardModule* pModule = (cGuardModule*)reserved;
		if(!pModule)
			return FALSE;
		path.assign(pModule->szPath);
		cGuardLoaded = true;
	} else {
		char szPath[MAX_PATH];
		GetModuleFileName(BH::instance, szPath, MAX_PATH);
		PathRemoveFileSpec(szPath);
		path.assign(szPath);
		path += "\\";
		cGuardLoaded = false;
	}

	moduleManager = new ModuleManager();
	config = new Config("BH.cfg");
	config->Parse();

	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		patches[n]->Install();
	}

	if (!D2CLIENT_GetPlayerUnit())
		oogDraw->Install();

	if(D2GFX_GetHwnd()) {
		BH::OldWNDPROC = (WNDPROC)GetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC);
		SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)GameWindowEvent);
	}

	settingsUI = new Drawing::UI("Settings", 350, 200);

	new Maphack();
	new ScreenInfo();
	new Gamefilter();
	new Bnet();
	new Item();
	new SpamFilter();
	new AutoTele();
	new Party();

	CreateThread(0,0,GameThread, 0,0,0);

	moduleManager->LoadModules();

	return true;
}

bool BH::Shutdown() {

	moduleManager->UnloadModules();

	delete moduleManager;
	delete settingsUI;

	SetWindowLong(D2GFX_GetHwnd(),GWL_WNDPROC,(LONG)BH::OldWNDPROC);
	for (int n = 0; n < (sizeof(patches) / sizeof(Patch*)); n++) {
		delete patches[n];
	}

	oogDraw->Remove();

	delete config;

	return true;
}