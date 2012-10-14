#include "Framehook.h"
#include "../../../Common.h"
#include "../../../D2Ptrs.h"

using namespace Drawing;

/* Basic Hook Initializer
 *		Used for just drawing basic framees on screen.
 */
Framehook::Framehook(HookVisibility visiblity, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) :
Hook(visiblity, x, y) {
	//Set the extra variables
	SetXSize(xSize);
	SetYSize(ySize);
	SetColor(0);
	SetTransparency(BTFull);
}

/* Group Hook Initializer
 *		Used in conjuction with other basic hooks to create an advanced hook.
 */
Framehook::Framehook(HookGroup* group, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) :
Hook(group, x, y) {
	//Set the extra variables
	SetXSize(xSize);
	SetYSize(ySize);
	SetColor(0);
	SetTransparency(BTFull);
}

/* GetColor()
 *	Returns the color of the framehook.
 */
unsigned int Framehook::GetColor() {
	return color;
}

/* SetColor(unsigned int color)
 *	Sets the color of the framehook.
 */
void Framehook::SetColor(unsigned int newColor) {
	if (newColor < 0 || newColor > 255)
		return;
	Lock();
	color = newColor;
	Unlock();
}

/* GetXSize()
 *	Returns the width of the frame hook.
 */
unsigned int Framehook::GetXSize() {
	return xSize;
}

/* SetXSize(unsigned int newX)
 *	Sets the new width of the framehook.
 */
void Framehook::SetXSize(unsigned int newX) {
	Lock();
	xSize = newX;
	Unlock();
}

/* GetYSize()
 *	Returns the height of the frame hook.
 */
unsigned int Framehook::GetYSize() {
	return ySize;
}

/* SetYSize(unsigned int ySize)
 *	Sets the height of the frame hook.
 */
void Framehook::SetYSize(unsigned int newY) {
	Lock();
	ySize = newY;
	Unlock();
}

/* GetTransparency()
 *	Returns the transparency of the frame hook.
 */
BoxTrans Framehook::GetTransparency() {
	return transparency;
}

/* SetTransparency(BoxTrans trans)
 *	Sets the transparency of the frame hook.
 */
void Framehook::SetTransparency(BoxTrans trans) {
	Lock();
	transparency = trans;
	Unlock();
}

DWORD __declspec(naked) _fastcall Framehook::DrawRectStub(RECT *pRect) {
	__asm
	{
		mov eax, ecx
		jmp D2CLIENT_DrawRectFrame
	}
}

/* OnDraw()
 *	Draws the rectangle
 */
void Framehook::OnDraw() {
	if (!IsActive())
		return;

	Lock();
	RECT pRect  = {GetX(), GetY(), GetX() + GetXSize(), GetY() + GetYSize()};
	D2GFX_DrawRectangle(GetX(), GetY(), GetX() + GetXSize(), GetY() + GetYSize(), GetColor(), GetTransparency());
	Framehook::DrawRectStub(&pRect);
	Unlock();
}

/* OnLeftClick(bool up, unsigned int x, unsigned int y)
 *	Check if the Frame hook has been clicked on.
 */
bool Framehook::OnLeftClick(bool up, unsigned int x, unsigned int y) {
	if (InRange(x,y) && GetLeftClickHandler()) {
		bool block = false;
		Lock();
		block = GetLeftClickHandler()(up, this, GetLeftClickVoid());
		Unlock();
		return block;
	}
	return false;
}

/* OnRightClick(bool up, unsigned int x, unsigned int y)
 *	Check if the Frame hook has been clicked on.
 */
bool Framehook::OnRightClick(bool up, unsigned int x, unsigned int y) {
	if (InRange(x,y) && GetRightClickHandler()) {
		bool block = false;
		Lock();
		block = GetRightClickHandler()(up, this, GetRightClickVoid());
		Unlock();
		return block;
	}
	return false;
}

bool Framehook::Draw(unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize, unsigned int color, BoxTrans trans) {
	RECT pRect  = {x, y, x + xSize, y + ySize};
	D2GFX_DrawRectangle(x, y, x + xSize, y + ySize, color, trans);
	Framehook::DrawRectStub(&pRect);
	return true;
}