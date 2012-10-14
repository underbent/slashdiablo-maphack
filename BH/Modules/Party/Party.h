#pragma once
#include "../Module.h"
#include "../../Config.h"
#include "../../Drawing.h"

class Party : public Module {
	private:
		map<std::string, Toggle> Toggles;
		void CheckParty();
		int c;

	public:
		Party() : Module("Party") {};
		void OnLoad();
		void OnUnload();
		void OnLoop();
		void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
};