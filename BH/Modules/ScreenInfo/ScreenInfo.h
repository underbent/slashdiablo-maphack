#pragma once
#include "../../D2Structs.h"
#include "../Module.h"
#include "../../Config.h"
#include "../../Drawing.h"


struct AutomapReplace {
	std::string key;
	std::string value;
};

class ScreenInfo : public Module {
	private:
		std::vector<std::string> automapInfo;
		Drawing::Texthook* bhText;
		DWORD gameTimer;

		int packetRequests;
		ULONGLONG warningTicks;
		ULONGLONG packetTicks;
		bool MephistoBlocked;
		bool DiabloBlocked;
		bool BaalBlocked;
		bool ReceivedQuestPacket;
	public:
		ScreenInfo() :
			Module("Screen Info"), warningTicks(BHGetTickCount()), packetRequests(0),
			MephistoBlocked(false), DiabloBlocked(false), BaalBlocked(false), ReceivedQuestPacket(false) {};

		void OnLoad();
		void OnGameJoin(const string& name, const string& pass, int diff);
		void OnGameExit();

		void OnRightClick(bool up, int x, int y, bool* block);
		void OnDraw();
		void OnAutomapDraw();
		void OnGamePacketRecv(BYTE* packet, bool *block);
};