#include "Gamefilter.h"
#include "../../D2Ptrs.h"
#include "../../D2Intercepts.h"
#include "../../D2Stubs.h"
#include <sstream>
#include "../../BH.h"

using namespace std;

list<GameListEntry*> Gamefilter::gameList;
Control* Gamefilter::filterBox;
int Gamefilter::refreshTime;

Patch* createGameBox = new Patch(Call, D2MULTI, { 0x149EF, 0xAD8F }, (int)D2MULTI_CreateGameBox_Interception, 5);
Patch* destoryGameList = new Patch(Call, D2MULTI, { 0x11DC3, 0x8413 }, (int)Gamefilter::DestroyGamelist, 5);
Patch* listRefresh = new Patch(Call, D2MULTI, { 0xDF4E, 0x121EE }, (int)D2MULTI_GameListRefresh_Interception, 5);

void Gamefilter::OnLoad() {
	if (!D2CLIENT_GetPlayerUnit()) {
		createGameBox->Install();
		destoryGameList->Install();
		listRefresh->Install();
	}
	refreshTime = BH::config->ReadInt("GameListRefresh", 1500);
}

void Gamefilter::OnUnload() {
	createGameBox->Remove();
	destoryGameList->Remove();
	listRefresh->Remove();
}

void Gamefilter::OnGameJoin(const string& name, const string& pass, int diff) {
	createGameBox->Remove();
	destoryGameList->Remove();
	listRefresh->Remove();
}

void Gamefilter::OnGameExit() {
	createGameBox->Install();
	destoryGameList->Install();
	listRefresh->Install();
}

void Gamefilter::OnRealmPacketRecv(BYTE* pPacket, bool* blockPacket) {
		if(pPacket[0] == 0x05 && filterBox)
	{		
		wstring wFilter(filterBox->wText);
		string sFilter(wFilter.begin(), wFilter.end());

		GameListEntry* pEntry = new GameListEntry;

		pEntry->wRequestId = *(WORD*)&pPacket[1];
		pEntry->dwIndex = *(DWORD*)&pPacket[3];
		pEntry->bPlayers = pPacket[7];
		pEntry->dwStatus = *(DWORD*)&pPacket[8];
		pEntry->sGameName = reinterpret_cast<CHAR*>(pPacket + 12);
		pEntry->sGameDesc = reinterpret_cast<CHAR*>(pPacket + 13 + pEntry->sGameName.length());

		if(pEntry->bPlayers == 0)
		{
			delete pEntry;
			*blockPacket = true;
			return;
		}

		if(!gameList.empty())
		{
			for(list<GameListEntry*>::iterator ListEntry = gameList.begin(); ListEntry != gameList.end(); ListEntry++)
				if(!(*ListEntry)->sGameName.compare(pEntry->sGameName))
				{
					delete pEntry;
					*blockPacket = true;
					return;
				}
		}

		string sGameName;

		for(unsigned int i = 0; i < sFilter.length(); i++)
			sFilter[i] = ::toupper(sFilter[i]);

		for(unsigned int i = 0; i < pEntry->sGameName.length(); i++)
			sGameName += ::toupper(pEntry->sGameName[i]);

		if(wFilter.empty() || strstr(sGameName.c_str(), sFilter.c_str()))
		{
			ControlText* pText = new ControlText;
			memset(pText, NULL, sizeof(ControlText));

			pText->dwColor = 4;
			pText->wText = new wchar_t[64];
			pText->wText2 = new wchar_t[12];

			memset(pText->wText, NULL, 64);
			memset(pText->wText2, NULL, 12);

			wstring wGameName(pEntry->sGameName.begin(), pEntry->sGameName.end());

			wcscpy_s(pText->wText, 64, wGameName.c_str());
			pText->wText2[0] = pEntry->bPlayers+0x30;

			if((*p_D2MULTI_GameListControl)->pFirstText)
			{
				(*p_D2MULTI_GameListControl)->pLastText->pNext = pText;
				(*p_D2MULTI_GameListControl)->pLastText = pText;
			} else {
				(*p_D2MULTI_GameListControl)->pFirstText = pText;	
				(*p_D2MULTI_GameListControl)->pLastText = pText;
				(*p_D2MULTI_GameListControl)->pSelectedText = pText;
			}

			(*p_D2MULTI_GameListControl)->dwSelectEnd+=1;
			(*p_D2MULTI_GameListControl)->pChildControl->dwScrollEntries = (*p_D2MULTI_GameListControl)->dwSelectEnd > 10 ? (*p_D2MULTI_GameListControl)->dwSelectEnd - 10 : 0;			
		}

		gameList.push_back(pEntry);

		return;
	}

	return;
}

BOOL __stdcall Gamefilter::Filterbox_InputHandler(Control* pControl, DWORD dwLength, CHAR* pChar)
{
	wstring wInput(pControl->wText);

	string sFilter(wInput.begin(), wInput.end());

	if(dwLength > 0)
		sFilter+=*pChar;

	for(unsigned int i = 0; i < sFilter.length(); i++)
		sFilter[i] = ::toupper(sFilter[i]);

	Gamefilter::BuildGameList(sFilter);

	return TRUE;
}

BOOL __stdcall Gamefilter::Filterbox_ReturnHandler(wchar_t* wText)
{

	return TRUE;
}

VOID Gamefilter::CreateGamelist(VOID)
{
	if(!gameList.empty())
	{
		for(list<GameListEntry*>::iterator ListEntry = gameList.begin(); ListEntry != gameList.end(); ListEntry++)
			delete (*ListEntry);

		gameList.clear();
	}

	filterBox = D2WIN_CreateEditBox(599, 185, 145, 41, 7, NULL, NULL, Gamefilter::Filterbox_ReturnHandler, NULL, NULL, (ControlPreferences*)p_D2MULTI_EditboxPreferences);
	filterBox->pCellFile = D2CLIENT_LoadUiImage("DATA\\GLOBAL\\UI\\FrontEnd\\textbox2");
	D2WIN_SetEditBoxCallback(filterBox, Gamefilter::Filterbox_InputHandler);
	filterBox->dwMaxLength = 0x12;
	filterBox->Initialize(filterBox);
}

VOID __stdcall Gamefilter::DestroyGamelist(Control* pControl)
{
	if(pControl)
	{
		if(filterBox)
		{
			if(!gameList.empty())
			{
				for(list<GameListEntry*>::iterator ListEntry = gameList.begin(); ListEntry != gameList.end(); ListEntry++)
					delete (*ListEntry);

				gameList.clear();
			}

			D2WIN_DestroyEditBox(filterBox);

			filterBox = NULL;
		}
	}
	
	D2WIN_DestroyControl(pControl);
}

void Gamefilter::OnOOGDraw() {
	// filterBox is instantiated in the create game box handler, so we can't
	// draw the join game screen until we have it
	if((*p_D2MULTI_GameListControl) && filterBox) {
		wstringstream wFilterStream;
		wstring wFilterString = L"Games: ";
		wstring wFilter = filterBox->wText;

		wFilterStream << (int)(*p_D2MULTI_GameListControl)->dwSelectEnd;
		wFilterString += wFilterStream.str().c_str();
		wFilterString += L"/";
		wFilterStream.str(L"");
		wFilterStream << (int)gameList.size();
		wFilterString += wFilterStream.str().c_str();

		DWORD dwOldSize = D2WIN_SetTextSize(1);
		D2WIN_DrawText(L"Filter", 549, 170, 4, -1);
		D2WIN_DrawText(wFilterString.c_str(), 424, 190, 4, -1);
		D2WIN_SetTextSize(dwOldSize);
	}
}

void Gamefilter::BuildGameList(string sFilter)
{
	if(!gameList.empty() && (*p_D2MULTI_GameListControl))
	{
		ControlText* pText = (*p_D2MULTI_GameListControl)->pFirstText;

		(*p_D2MULTI_GameListControl)->pFirstText = NULL;
		(*p_D2MULTI_GameListControl)->pLastText = NULL;
		(*p_D2MULTI_GameListControl)->pSelectedText = NULL;
		(*p_D2MULTI_GameListControl)->dwSelectEnd = 0;
		(*p_D2MULTI_GameListControl)->dwSelectStart = 0;
		(*p_D2MULTI_GameListControl)->dwMaxLength = 0;

		(*p_D2MULTI_GameListControl)->pChildControl->dwMaxLength = 0;
		(*p_D2MULTI_GameListControl)->pChildControl->dwScrollPosition = 0;
		(*p_D2MULTI_GameListControl)->pChildControl->dwScrollEntries = 0;

		while(pText)
		{
			ControlText* pNext = pText->pNext;
			
			delete[] pText->wText;
			delete[] pText->wText2;
			delete pText;

			pText = pNext;
		}

		for(list<GameListEntry*>::iterator ListEntry = gameList.begin(); ListEntry != gameList.end(); ListEntry++)
		{
			string sGameName((*ListEntry)->sGameName.c_str());

			for(UINT i = 0; i < sGameName.length(); i++)
				sGameName[i] = ::toupper(sGameName[i]);

			if(strstr(sGameName.c_str(), sFilter.c_str()))
			{

				ControlText* pText = new ControlText;
				memset(pText, NULL, sizeof(ControlText));

				pText->dwColor = 4;
				pText->wText = new wchar_t[64];
				pText->wText2 = new wchar_t[12];

				memset(pText->wText, NULL, 64);
				memset(pText->wText2, NULL, 12);

				wstring wGameName((*ListEntry)->sGameName.begin(), (*ListEntry)->sGameName.end());

				wcscpy_s(pText->wText, 64, wGameName.c_str());
				pText->wText2[0] = (*ListEntry)->bPlayers+0x30;

				if((*p_D2MULTI_GameListControl)->pFirstText)
				{
					(*p_D2MULTI_GameListControl)->pLastText->pNext = pText;
					(*p_D2MULTI_GameListControl)->pLastText = pText;
				} else {
					(*p_D2MULTI_GameListControl)->pFirstText = pText;	
					(*p_D2MULTI_GameListControl)->pLastText = pText;
					(*p_D2MULTI_GameListControl)->pSelectedText = pText;
				}

				(*p_D2MULTI_GameListControl)->dwSelectEnd+=1;
				(*p_D2MULTI_GameListControl)->pChildControl->dwScrollEntries = (*p_D2MULTI_GameListControl)->dwSelectEnd > 10 ? (*p_D2MULTI_GameListControl)->dwSelectEnd - 10 : 0;
			}
		}
	}
}

VOID __declspec(naked) D2MULTI_CreateGameBox_Interception(VOID)
{
	__asm
	{
		PUSHAD
		CALL Gamefilter::CreateGamelist
		POPAD
		MOV EDX, 0x10
		RETN
	}
}

VOID __declspec(naked) D2MULTI_GameListRefresh_Interception(VOID)
{
	__asm
	{
		CMP EAX, Gamefilter::refreshTime
		RETN
	}
}