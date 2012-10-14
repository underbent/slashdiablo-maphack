#pragma once
#include "../Module.h"
#include "../../Config.h"
#include <Windows.h>
#include <list>
#include "Bayes.h"

class SpamFilter : public Module {
	private:
		std::string log, url;
		Bayes bayes;

		map<std::string, Toggle> Toggles;

		void DoUpdate();

	public:
		SpamFilter() : Module("Spam Filter") {};

		void OnLoad();
		void OnUnload();

		void OnChatMsg(const char*, const char*, bool, bool*);
		void OnUserInput(const wchar_t*, bool, bool*);
};
