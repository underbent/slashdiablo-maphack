#include "Boxhook.h"
#include "../../../Common.h"
#include "../../../D2Ptrs.h"

using namespace Drawing;

/* Basic Hook Initializer
 *		Used for just drawing basic boxes on screen.
 */
Boxhook::Boxhook(HookVisibility visiblity, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) :
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
Boxhook::Boxhook(HookGroup* group, unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize) :
Hook(group, x, y) {
	//Set the extra variables
	SetXSize(xSize);
	SetYSize(ySize);
	SetColor(0);
	SetTransparency(BTFull);
}

/* GetColor()
 *	Returns the color of the boxhook.
 */
unsigned int Boxhook::GetColor() {
	return color;
}

/* SetColor(unsigned int color)
 *	Sets the color of the boxhook.
 */
void Boxhook::SetColor(unsigned int newColor) {
	if (newColor < 0 || newColor > 255)
		return;
	Lock();
	color = newColor;
	Unlock();
}

/* GetXSize()
 *	Returns the width of the box hook.
 */
unsigned int Boxhook::GetXSize() {
	return xSize;
}

/* SetXSize(unsigned int newX)
 *	Sets the new width of the boxhook.
 */
void Boxhook::SetXSize(unsigned int newX) {
	Lock();
	xSize = newX;
	Unlock();
}

/* GetYSize()
 *	Returns the height of the box hook.
 */
unsigned int Boxhook::GetYSize() {
	return ySize;
}

/* SetYSize(unsigned int ySize)
 *	Sets the height of the box hook.
 */
void Boxhook::SetYSize(unsigned int newY) {
	Lock();
	ySize = newY;
	Unlock();
}

/* GetTransparency()
 *	Returns the transparency of the box hook.
 */
BoxTrans Boxhook::GetTransparency() {
	return transparency;
}

/* SetTransparency(BoxTrans trans)
 *	Sets the transparency of the box hook.
 */
void Boxhook::SetTransparency(BoxTrans trans) {
	Lock();
	transparency = trans;
	Unlock();
}

/* OnDraw()
 *	Draws the rectangle
 */
void Boxhook::OnDraw() {
	if (!IsActive())
		return;

	Lock();
	D2GFX_DrawRectangle(GetX(), GetY(), GetX() + GetXSize(), GetY() + GetYSize(), GetColor(), GetTransparency());
	Unlock();
}

/* OnLeftClick(bool up, unsigned int x, unsigned int y)
 *	Check if the Box hook has been clicked on.
 */
bool Boxhook::OnLeftClick(bool up, unsigned int x, unsigned int y) {
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
 *	Check if the Box hook has been clicked on.
 */
bool Boxhook::OnRightClick(bool up, unsigned int x, unsigned int y) {
	if (InRange(x,y) && GetRightClickHandler()) {
		bool block = false;
		Lock();
		block = GetRightClickHandler()(up, this, GetRightClickVoid());
		Unlock();
		return block;
	}
	return false;
}


bool Boxhook::Draw(unsigned int x, unsigned int y, unsigned int xSize, unsigned int ySize, unsigned int color, BoxTrans trans) {
	D2GFX_DrawRectangle(x, y, x + xSize, y + ySize, color, trans);
	return true;
}