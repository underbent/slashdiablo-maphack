#pragma once
#include "../../D2Structs.h"
#include "../Module.h"
#include "../../Config.h"
#include "../../Drawing.h"
#include <deque>


struct AutomapReplace {
	std::string key;
	std::string value;
};

struct StateCode {
	std::string name;
	unsigned int value;
};

struct StateWarning {
	std::string name;
	ULONGLONG startTicks;
	StateWarning(string n, ULONGLONG ticks) : name(n), startTicks(ticks) {}
};

class ScreenInfo : public Module {
	private:
		std::vector<std::string> automapInfo;
		std::map<DWORD, string> SkillWarningMap;
		std::deque<StateWarning*> CurrentWarnings;
		Drawing::Texthook* bhText;
		DWORD gameTimer;

		int packetRequests;
		ULONGLONG warningTicks;
		ULONGLONG packetTicks;
		bool MephistoBlocked;
		bool DiabloBlocked;
		bool BaalBlocked;
		bool ReceivedQuestPacket;
		DWORD startExperience;
		int startLevel;

		void ScreenInfo::drawExperienceInfo();
	public:
		static map<std::string, Toggle> Toggles;

		ScreenInfo() :
			Module("Screen Info"), warningTicks(BHGetTickCount()), packetRequests(0),
			MephistoBlocked(false), DiabloBlocked(false), BaalBlocked(false), ReceivedQuestPacket(false),
			startExperience(0), startLevel(0) {};

		void OnLoad();
		void OnGameJoin(const string& name, const string& pass, int diff);
		void OnGameExit();

		void OnRightClick(bool up, int x, int y, bool* block);
		void OnDraw();
		void OnAutomapDraw();
		void OnGamePacketRecv(BYTE* packet, bool *block);
};

StateCode GetStateCode(unsigned int nKey);
StateCode GetStateCode(const char* name);
long long ExpByLevel[];