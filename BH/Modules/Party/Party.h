#pragma once
#include "../Module.h"
#include "../../Config.h"
#include "../../Drawing.h"

#define PARTY_NOT_IN_PARTY 0x00
#define PARTY_IN_PARTY 0x01
#define PARTY_INVITED_YOU 0x02
#define PARTY_INVITED_BY_YOU 0x04

class Party : public Module {
	private:
		map<std::string, Toggle> Toggles;
		map<std::string, bool> LootingPermission;
		void CheckParty();
		int c;
	public:
		Party() : Module("Party") {};
		void OnLoad();
		void OnUnload();
		void OnLoop();
		void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
		void OnGameExit();
};