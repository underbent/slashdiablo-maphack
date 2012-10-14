#pragma once

#include "../../Hook.h"

namespace Drawing {
	class Keyhook : public Hook {
		private:
			unsigned int* key;//Pointer to the current key
			std::string name;//Name of the hotkey
			unsigned int timeout;//Timeout to change hotkey if clicked
		public:
			//Two Hook Initializations; one for basic hooks, one for grouped hooks.
			Keyhook(HookVisibility visibility, unsigned int x, unsigned int y, unsigned int* key, std::string hotkeyName);
			Keyhook(HookGroup* group, unsigned int x, unsigned int y, unsigned int* key, std::string hotkeyName);

			std::string GetName() { return name; };
			void SetName(std::string newName) { Lock(); name = newName; Unlock(); };

			unsigned int GetKey() { return *key; };
			void SetKey(unsigned int* newKey) { Lock(); key = newKey; Unlock(); };

			bool OnLeftClick(bool up, unsigned int x, unsigned int y);
			void OnDraw();
			bool OnKey(bool up, BYTE key, LPARAM lParam);

			unsigned int GetXSize();
			unsigned int GetYSize();
	};
};