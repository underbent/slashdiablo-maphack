#pragma once

#include <Windows.h>
#include <string>
#include <list>
#include "../Hook.h"

namespace Drawing {
	class StatsDisplay;

	class StatsDisplay : public HookGroup {
		private:
			static StatsDisplay *display;
			std::string name;
			unsigned int x, y, xSize, ySize, zOrder;
			unsigned int statsKey;
			bool active, minimized;
			CRITICAL_SECTION crit;
		public:
			StatsDisplay(std::string name);
			~StatsDisplay();

			void Lock() { EnterCriticalSection(&crit); };
			void Unlock() { LeaveCriticalSection(&crit); };

			unsigned int GetX() { return x; };
			unsigned int GetY() { return y; };
			unsigned int GetXSize() { return xSize; };
			unsigned int GetYSize() { return ySize; };
			std::string GetName() { return name; };
			unsigned int GetZOrder() { return zOrder; };
			bool IsActive() { return active; };
			bool IsMinimized() { return minimized; };

			bool InRange(unsigned int x, unsigned int y);

			void SetX(unsigned int newX);
			void SetY(unsigned int newY);
			void SetXSize(unsigned int newXSize);
			void SetYSize(unsigned int newYSize);
			void SetName(std::string newName) { Lock(); name = newName;  Unlock(); };
			void SetZOrder(unsigned int newZ) { Lock(); zOrder = newZ; Unlock(); };
			void SetActive(bool newState) { Lock(); active = newState; Unlock(); };
			void SetMinimized(bool newState) { Lock(); minimized = newState; Unlock(); };

			void OnDraw();
			static void Draw();

			bool OnLeftClick(bool up, unsigned int mouseX, unsigned int mouseY);
			static bool LeftClick(bool up, unsigned int mouseX, unsigned int mouseY);

			bool OnRightClick(bool up, unsigned int mouseX, unsigned int mouseY);
			static bool RightClick(bool up, unsigned int mouseX, unsigned int mouseY);

			bool OnKey(bool up, BYTE key, LPARAM lParam);
			static bool KeyClick(bool bUp, BYTE bKey, LPARAM lParam);

			bool InWindow(unsigned int xPos, unsigned int yPos) { return xPos >= x && xPos <= x + xSize && yPos >= y && yPos <= y + ySize; };
			static bool InPos(unsigned int xPos, unsigned int yPos, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) {
				return xPos >= x && xPos <= x + xSize && yPos >= y && yPos <= y + ySize;
			};
	};
};
