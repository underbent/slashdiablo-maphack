#pragma once
#include <vector>
#include "../../Hook.h"

namespace Drawing {
	class Combohook : public Hook {
		private:
			std::vector<std::string> options;
			unsigned int xSize;
			unsigned int font;
			unsigned int* currentIndex;
			bool active;
		public:
			Combohook(HookVisibility visibility, unsigned int x, unsigned int y, unsigned int xSize, unsigned int* currentIndex, std::vector<std::string> options);
			Combohook(HookGroup* group, unsigned int x, unsigned int y, unsigned int xSize, unsigned int* currentIndex, std::vector<std::string> options);

			std::vector<std::string> GetOptions() { return options; };
			unsigned int NewOption(std::string opt) { Lock(); options.push_back(opt); Unlock(); return options.size() - 1; };
			unsigned int GetSelectedIndex() { return *currentIndex; };
			void SetSelectedIndex(unsigned int index) { if (index > options.size()) { return; } Lock(); *currentIndex = index; Unlock(); };

			unsigned int GetFont() { return font; };
			void SetFont(unsigned int newFont) { Lock(); font = newFont; Unlock(); };

			unsigned int GetXSize() { return xSize; };
			void SetXSize(unsigned int size) { Lock(); xSize = size; Unlock(); };

			unsigned int GetYSize() { unsigned int height[] = {10,11,18,24,10,13,7,13,10,12,8,8,7,12}; return height[GetFont()]; };

			bool OnLeftClick(bool up, unsigned int x, unsigned int y);
			void OnDraw();

			bool InHook(unsigned int nx, unsigned int ny) { return nx >= GetX() && ny >= GetY() && nx <= GetX() + GetXSize() + 5 && ny <= GetY() + GetYSize() + 3; };
	};
};