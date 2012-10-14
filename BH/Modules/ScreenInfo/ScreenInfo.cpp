#include "Screeninfo.h"
#include "../../BH.h"
#include "../../D2Ptrs.h"
#include "../../D2Stubs.h"
#include <time.h>

using namespace Drawing;

void ScreenInfo::OnLoad() {
	automapInfo = BH::config->ReadArray("AutomapInfo");
	bhText = new Texthook(Perm, 790, 6, "ÿc4BH v0.1.1 (SlashDiablo Branch)");
	bhText->SetAlignment(Right);
	if (BH::cGuardLoaded) {
		Texthook* cGuardText = new Texthook(Perm, 790, 23, "ÿc4cGuard Loaded");
		cGuardText->SetAlignment(Right);
	}
	gameTimer = GetTickCount();
}

void ScreenInfo::OnGameJoin(const string& name, const string& pass, int diff) {
	gameTimer = GetTickCount();
}

void ScreenInfo::OnAutomapDraw() {
	GameStructInfo* pInfo = (*p_D2CLIENT_GameInfo);
	BnetData* pData = (*p_D2LAUNCH_BnData);
	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	char* szDiff[3] = {"Normal", "Nightmare", "Hell"};
	if (!pInfo || !pData || !pUnit)
		return;
	int y = 6+(BH::cGuardLoaded?16:0);

	char gameTime[20];
	int nTime = ((GetTickCount() - gameTimer) / 1000);
	sprintf_s(gameTime, 20, "%.2d:%.2d:%.2d", nTime/3600, (nTime/60)%60, nTime%60);

	time_t tTime;
	time(&tTime);
	CHAR szTime[128] = "";
	struct tm time;
	localtime_s(&time, &tTime);
	strftime(szTime, sizeof(szTime), "%I:%M:%S %p", &time);

	AutomapReplace automap[] = {
		{"GAMENAME", pData->szGameName},
		{"GAMEPASS", pData->szGamePass},
		{"GAMEIP", pData->szGameIP},
		{"GAMEDIFF", szDiff[D2CLIENT_GetDifficulty()]},
		{"ACCOUNTNAME", pData->szAccountName},
		{"CHARNAME", pUnit->pPlayerData->szName},
		{"LEVEL", UnicodeToAnsi(D2CLIENT_GetLevelName(pUnit->pPath->pRoom1->pRoom2->pLevel->dwLevelNo))},
		{"GAMETIME", gameTime},
		{"REALTIME", szTime}
	};

	for (vector<string>::iterator it = automapInfo.begin(); it < automapInfo.end(); it++) {
		string key;
		key.assign(*it);
		for (int n = 0; n < sizeof(automap) / sizeof(automap[0]); n++) {
			if (key.find("%" + automap[n].key + "%") == string::npos)
				continue;
			if (automap[n].value.length() == 0)
				key = "";
			else
				key = key.replace(key.find("%" + automap[n].key + "%"), automap[n].key.length() + 2, automap[n].value);
		}
		if (key.length() > 0)
			Texthook::Draw(790, (y+=16), Right,0,Gold,"%s", key.c_str());
	}
}