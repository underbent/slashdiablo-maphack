#include "UITab.h"
#include "../Basic/Texthook/Texthook.h"
#include "../Basic/Framehook/Framehook.h"
#include "../../D2Ptrs.h"

using namespace Drawing;

UITab::~UITab() {
	ui->Lock();
	// Remove all hooks associated to the tab.
	for (auto it = Hooks.begin(); it != Hooks.end(); it++){
		Hook* h = *it;
		delete h;
	}
	Hooks.clear();
	
	// Remove tab from list.
	ui->Tabs.remove(this);

}

unsigned int UITab::GetTabPos() { 
	unsigned int n = 0; 
	for(std::list<UITab*>::iterator it = ui->Tabs.begin(); it != ui->Tabs.end(); it++, n++)
		if ((*it) == this)
			return n;
	return 0;
}

void UITab::OnDraw() {
	bool isHovering = IsHovering((*p_D2CLIENT_MouseX), (*p_D2CLIENT_MouseY));

	if (IsActive())
		for(std::list<Hook*>::iterator it = Hooks.begin(); it != Hooks.end(); it++)
			(*it)->OnDraw();
	else
		Framehook::Draw(GetTabX(), GetTabY(), GetTabSize(), TAB_HEIGHT, 0, (ui->IsActive()?BTNormal:BTHighlight));
	Texthook::Draw(GetTabX() + (GetTabSize() / 2), GetTabY() + 2, Center, 0, IsActive()?Grey:isHovering?Tan:Gold, name);
}