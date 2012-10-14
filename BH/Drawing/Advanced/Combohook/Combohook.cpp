#include "Combohook.h"
#include "../../Basic/Framehook/Framehook.h"
#include "../../Basic/Texthook/Texthook.h"
#include "../../../D2Ptrs.h"

using namespace Drawing;
using namespace std;

Combohook::Combohook(HookVisibility visibility, unsigned int x, unsigned int y, unsigned int xSize, unsigned int* index, std::vector<std::string> opts)
	: Hook(visibility, x, y) {
		SetXSize(xSize);
		currentIndex = index;
		options = opts;
		SetFont(0);
		active = false;
}

Combohook::Combohook(HookGroup* group, unsigned int x, unsigned int y, unsigned int xSize, unsigned int* index, std::vector<std::string> opts)
	: Hook(group, x, y) {
		SetXSize(xSize);
		currentIndex = index;
		options = opts;
		SetFont(0);
		active = false;
}

bool Combohook::OnLeftClick(bool up, unsigned int x, unsigned int y) {
	// Check if we clicked on a inactive combo box.
	if (InHook(x, y) && !active) {
		if (up)
			active = true;

		return true;
	}
	if (active && x >= GetX() && y >= GetY() && x <= GetX() + GetXSize() && y <= GetY() + ((options.size() + 1) * (GetYSize() + 4))) {
		int n = 0;
		for (vector<string>::iterator it = options.begin(); it < options.end(); it++,n++) {
			unsigned int textY = (GetY() + GetYSize() + 4 + (n * (GetYSize() + 4)));
			bool hovering = x >= GetX() && y >= textY && x <= GetX() + GetXSize() && y <= textY + GetYSize() + 4;
			if (hovering && up) {
				SetSelectedIndex(n);
				active = false;
				return true;
			}
		}
		if (up)
			active = false;
		return true;
	}
	active = false;
	return false;
}

void Combohook::OnDraw() {
	Framehook::Draw(GetX(), GetY(), GetXSize(), GetYSize() + 4, 0, BTNormal);
	Texthook::Draw(GetX() + 5, GetY() + 3, 0, GetFont(), Gold, options.at(GetSelectedIndex()));
	//Framehook::Draw(GetX() + GetXSize() - 16, GetY(), 8, GetYSize() + 4, 0, BTNormal);
	Texthook::Draw(GetX() + GetXSize() - 8, GetY() + 3, 0, GetFont(), InHook((*p_D2CLIENT_MouseX), (*p_D2CLIENT_MouseY))||active?Tan:Gold, "v");

	if (active) {
		Framehook::Draw(GetX(), GetY() + GetYSize() + 4, GetXSize(), (options.size() + 1) * GetYSize() + 4, 0, BTNormal);
		unsigned int mouseX = (*p_D2CLIENT_MouseX);
		unsigned int mouseY = (*p_D2CLIENT_MouseY);
		unsigned int n = 0;
		for (vector<string>::iterator it = options.begin(); it < options.end(); it++,n++) {
			unsigned int textY = (GetY() + GetYSize() + 4 + (n * (GetYSize() + 4)));
			bool hovering = mouseX >= GetX() && mouseY >= textY && mouseX <= GetX() + GetXSize() && mouseY <= textY + GetYSize() + 4;
			Texthook::Draw(GetX() + 5, textY + 2, 0, GetFont(), hovering?Tan:Gold, *it);
		}
	}
}