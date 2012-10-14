#include "Linehook.h"
#include "../../../Common.h"
#include "../../../D2Ptrs.h"

using namespace Drawing;

/* Basic Hook Initializer
 *		Used for just drawing basic lines of screen.
 */
Linehook::Linehook(HookVisibility visiblity, unsigned int x, unsigned int y, unsigned int x2, unsigned int y2) :
Hook(visiblity, x, y) {
	//Set the extra variables
	SetX2(x2);
	SetY2(y2);
	SetColor(0);
}

/* Group Hook Initializer
 *		Used in conjuction with other basic hooks to create an advanced hook.
 */
Linehook::Linehook(HookGroup* group, unsigned int x, unsigned int y, unsigned int x2, unsigned int y2) :
Hook(group, x, y) {
	//Set the extra variables
	SetX2(x2);
	SetY2(y2);
	SetColor(0);
}

/* GetColor()
 *	Returns the color of the linehook.
 */
unsigned int Linehook::GetColor() {
	return color;
}

/* SetColor(unsigned int color)
 *	Sets the color of the linehook.
 */
void Linehook::SetColor(unsigned int newColor) {
	if (newColor < 0 || newColor > 255)
		return;
	Lock();
	color = newColor;
	Unlock();
}

/* GetX2()
 *	Returns the X2 of the line hook.
 */
unsigned int Linehook::GetX2() {
	return x2;
}

/* SetX2(unsigned int newX)
 *	Sets the new X2 of the linehook.
 */
void Linehook::SetX2(unsigned int newX) {
	Lock();
	x2 = newX;
	Unlock();
}

/* GetY2()
 *	Returns the Y2 of the line hook.
 */
unsigned int Linehook::GetY2() {
	return y2;
}

/* SetY2(unsigned int y2)
 *	Sets the Y2 of the line hook.
 */
void Linehook::SetY2(unsigned int newY) {
	Lock();
	y2 = newY;
	Unlock();
}

/* Get(X/Y)Size()
 *	Used by click handlers, fuck clicking lines.
 */
unsigned int Linehook::GetXSize() { return 0; };
unsigned int Linehook::GetYSize() { return 0; };


/* OnDraw()
 *	Draws the line
 */
void Linehook::OnDraw() {
	if (!IsActive())
		return;

	Lock();
	D2GFX_DrawLine(GetX(), GetY(), GetX2(), GetY2(), GetColor(), -1);
	Unlock();
}

bool Linehook::Draw(unsigned int x, unsigned int y, unsigned int x2, unsigned int y2, unsigned int color) {
	D2GFX_DrawLine(x,y,x2,y2,color,-1);
	return true;
}