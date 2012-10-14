#pragma once
#include "../Module.h"
#include <Windows.h>
#include <list>

struct GameListEntry
{
	WORD wRequestId;
	DWORD dwIndex;
	BYTE bPlayers;
	DWORD dwStatus;
	std::string sGameName;
	std::string sGameDesc;
};
struct Control;

class Gamefilter : public Module {
	public:
		static std::list<GameListEntry*> gameList;
		static Control* filterBox;
		static int refreshTime;
	
		Gamefilter() : Module("Gamefilter") {};

		void OnLoad();
		void OnUnload();

		void OnGameJoin(const string& name, const string& pass, int diff);
		void OnGameExit();

		void OnRealmPacketRecv(BYTE* pPacket, bool* blockPacket);
		void OnOOGDraw();

		static void CreateGamelist();
		static void __stdcall DestroyGamelist(Control* pControl);
		static void BuildGameList(std::string sFilter);
		static BOOL __stdcall Filterbox_InputHandler(Control* pControl, DWORD dwLength, CHAR* pChar);
		static BOOL __stdcall Filterbox_ReturnHandler(wchar_t* wText);
};

VOID D2MULTI_CreateGameBox_Interception(VOID);
VOID D2MULTI_GameListRefresh_Interception(VOID);