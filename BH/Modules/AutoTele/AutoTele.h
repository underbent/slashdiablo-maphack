#pragma once
#include "../../Constants.h"
#include "../../D2Ptrs.h"
#include "../Module.h"
#include "../../Drawing.h"
#include "../../Config.h"
#include "ATIncludes\ArrayEx.h"

typedef struct Vector_t
{
	unsigned int dwType;
	unsigned int Id;
	unsigned int Id2;
	unsigned int Area;
} Vector;

class AutoTele : public Module {
	private:
		std::map<string, Toggle> Toggles;
		unsigned int NextKey, OtherKey, WPKey, PrevKey;
		unsigned int Colors[6];
		Drawing::UITab* settingsTab;

		int Try;
		POINT End;
		DWORD _timer, _timer2, _InteractTimer;
		bool SetInteract, SetTele, CastTele, TeleActive, DoInteract;
		DWORD InteractType, InteractId;
		Room2* InteractRoom;
		DWORD LastArea;
		POINT Vectors[5];
		CArrayEx <POINT, POINT> TPath;
		HANDLE LoadHandle;

		//functions
		DWORD GetPlayerArea();
		void ManageTele(Vector T);
		int MakePath(int x, int y, DWORD Areas[], DWORD count, bool MoveThrough);
		POINT FindPresetLocation(DWORD dwType, DWORD dwTxtFileNo, DWORD Area);
		bool GetSkill(WORD wSkillId);
		bool SetSkill(WORD wSkillId, bool Left);
		void PrintText(DWORD Color, char *szText, ...);
		bool CastOnMap(WORD x, WORD y, bool Left);
		bool Interact(DWORD UnitId, DWORD UnitType);
		DWORD GetUnitByXY(DWORD X, DWORD Y, Room2* pRoom);
		bool WaitingForMapData();

	public:
		AutoTele() : Module("AutoTele"), LoadHandle(NULL) {};
		void OnLoad();
		void LoadConfig();
		void OnLoop();
		void OnAutomapDraw();
		void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
		void OnGamePacketRecv(BYTE* packet, bool* block);

		std::map<string, Toggle>* GetToggles() { return &Toggles; }
		void GetVectors();

		static Level* GetLevel(Act* pAct, int level);
		static DWORD GetDistanceSquared(DWORD x1, DWORD y1, DWORD x2, DWORD y2);
};

enum TeleType {
	Next = 0,
	Other,
	WP,
	Prev
};

DWORD WINAPI LoadNewArea(VOID* lpvoid);
