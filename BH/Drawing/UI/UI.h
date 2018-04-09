#pragma once

#include <Windows.h>
#include <string>
#include <list>
#include "../Hook.h"

namespace Drawing {
	class UI;
	class UITab;

	#define TITLE_BAR_HEIGHT 15
	#define MINIMIZED_Y_POS 585
	#define MINIMIZED_X_POS 234

	class UI : public HookGroup {
		private:
			static std::list<UI*> UIs;
			static std::list<UI*> Minimized;
			unsigned int x, y, xSize, ySize, zOrder;//Position and Size and Order
			unsigned int minimizedX, minimizedY;//Position when minimized
			bool active, minimized, dragged;//If UI is active or minimized or dragged
			unsigned int dragX, dragY;//Position where we grabbed it.
			unsigned int startX, startY;//Position where we grabbed it.
			std::string name;//Name of the UI
			UITab* currentTab;//Current tab open at the time.
			CRITICAL_SECTION crit;//Critical section
		public:
			std::list<UITab*> Tabs;

			UI(std::string name, unsigned int xSize, unsigned int ySize);
			~UI();

			void Lock() { EnterCriticalSection(&crit); };
			void Unlock() { LeaveCriticalSection(&crit); };

			unsigned int GetX() { return x; };
			unsigned int GetY() { return y; };
			unsigned int GetXSize() { return xSize; };
			unsigned int GetYSize() { return ySize; };
			unsigned int GetMinimizedX() { return minimizedX; };
			unsigned int GetMinimizedY() { return minimizedY; };
			bool IsActive() { return active; };
			bool IsMinimized() { return minimized; };
			bool IsDragged() { return dragged; };
			std::string GetName() { return name; };
			unsigned int GetZOrder() { return zOrder; };

			void SetX(unsigned int newX);
			void SetY(unsigned int newY);
			void SetXSize(unsigned int newXSize);
			void SetYSize(unsigned int newYSize);
			void SetMinimizedX(unsigned int newX);
			void SetMinimizedY(unsigned int newY);
			void SetActive(bool newState) { Lock(); active = newState; Unlock(); };
			void SetMinimized(bool newState);
			void SetName(std::string newName) { Lock(); name = newName;  Unlock(); };
			void SetDragged(bool state);
			void SetZOrder(unsigned int newZ) { Lock(); zOrder = newZ; Unlock(); };

			UITab* GetActiveTab() { if (!currentTab) { currentTab = (*Tabs.begin()); } return currentTab; };
			void SetCurrentTab(UITab* tab) { Lock(); currentTab = tab; Unlock(); };

			void OnDraw();
			static void Draw();

			static void Sort(UI* zero);

			bool OnLeftClick(bool up, unsigned int mouseX, unsigned int mouseY);
			static bool LeftClick(bool up, unsigned int mouseX, unsigned int mouseY);

			bool OnRightClick(bool up, unsigned int mouseX, unsigned int mouseY);
			static bool RightClick(bool up, unsigned int mouseX, unsigned int mouseY);

			bool InWindow(unsigned int xPos, unsigned int yPos) { return xPos >= x && xPos <= x + xSize && yPos >= y && yPos <= y + ySize; };
			bool InTitle(unsigned int xPos, unsigned int yPos) { return xPos >= x && xPos <= x + xSize && yPos >= y && yPos <= y + TITLE_BAR_HEIGHT; };
			static bool InPos(unsigned int xPos, unsigned int yPos, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) { return xPos >= x && xPos <= x + xSize && yPos >= y && yPos <= y + ySize; };
	};
};
