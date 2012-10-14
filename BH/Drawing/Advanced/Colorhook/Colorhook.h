#pragma once

#include "../../Hook.h"

namespace Drawing {
	class Colorhook;

	class Colorhook : public Hook {
		private:
			std::string text;//Text to have linked
			unsigned int* currentColor;//Color that we will be changing
			unsigned int curColor;
		public:
			static Colorhook* current;//Pointer to the current colorhook

			//Two Hook Initializations; one for basic hooks, one for grouped hooks.
			Colorhook(HookVisibility visibility, unsigned int x, unsigned int y, unsigned int* color, std::string formatString, ...);
			Colorhook(HookGroup* group, unsigned int x, unsigned int y, unsigned int* color, std::string formatString, ...);

			std::string GetText() { return text; };
			void SetText(std::string newText);

			unsigned int GetColor() { return *currentColor; };
			void SetColor(unsigned int newColor);

			bool OnLeftClick(bool up, unsigned int x, unsigned int y);
			bool OnRightClick(bool up, unsigned int x, unsigned int y);
			void OnDraw();

			unsigned int GetXSize();
			unsigned int GetYSize();
	};
};