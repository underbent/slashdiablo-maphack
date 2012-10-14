#pragma once
#include "../Hook.h"
#include "UI.h"

namespace Drawing {
	#define TAB_HEIGHT 13

	class UITab : public HookGroup {
		private:
			std::string name;
			UI* ui;
		public:
			UITab(std::string name, UI* nui) : name(name), ui(nui) {ui->Tabs.push_back(this); if (ui->Tabs.size() == 1) { ui->SetCurrentTab(this); }};
			~UITab();

			unsigned int GetX() { return ui->GetX(); };
			unsigned int GetY() { return ui->GetY() + TITLE_BAR_HEIGHT + TAB_HEIGHT; };
			unsigned int GetXSize() { return ui->GetXSize(); };
			unsigned int GetYSize() { return ui->GetYSize() - TITLE_BAR_HEIGHT - TAB_HEIGHT; };

			unsigned int GetTabPos();
			unsigned int GetTabSize() { return (ui->GetXSize() / ui->Tabs.size()); };
			unsigned int GetTabX() { return ui->GetX() + GetTabPos() * GetTabSize(); };
			unsigned int GetTabY() { return ui->GetY() + TITLE_BAR_HEIGHT; };


			bool IsActive() { return ui->GetActiveTab() == this && !ui->IsMinimized(); };

			bool IsHovering(unsigned int x, unsigned int y) { return x >= GetTabX() && y >= GetTabY() && x <= (GetTabX() + GetTabSize()) && y <= (GetTabY() + TAB_HEIGHT); };

			void OnDraw();
	};
};