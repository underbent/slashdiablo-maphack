#include "Hook.h"
#include "Advanced/Colorhook/Colorhook.h"
#include "../D2Ptrs.h"

using namespace Drawing;
using namespace std;

std::list<Hook*> Hook::Hooks;

/* Basic Hook Initializer
 *		Used for just drawing basic things on screen.
 */
Hook::Hook(HookVisibility visibility, unsigned int x, unsigned int y) : 
visibility(visibility), x(x), y(y), z(1), active(true), alignment(None), group(false), left(false), right(false), leftVoid(false), rightVoid(false) {
	InitializeCriticalSection(&crit);
	Hooks.push_back(this);
}

/* Group Hook Initializer
 *		Used in conjuction with other basic hooks to create an advanced hook.
 */
Hook::Hook(HookGroup *group, unsigned int x, unsigned int y) :
visibility(Group), x(x), y(y), z(1), active(true), alignment(None), group(group), left(false), right(false), leftVoid(false), rightVoid(false) {
	InitializeCriticalSection(&crit);
	Hooks.push_back(this);
	group->Hooks.push_back(this);
}

/* Lock()
 *	Locks Critical Section so we can do work.
 */
void Hook::Lock() {
	EnterCriticalSection(&crit);
}

/* Unlock()
 *	Unlocks Critical Section so others can do work.
 */
void Hook::Unlock() {
	LeaveCriticalSection(&crit);
}

/* GetX()
 *	Returns the x screen position that the hook will drawn at.
 */
unsigned int Hook::GetX() {
	unsigned int retX = (GetVisibility() == Group) ? group->GetX() + x : x;

	//If needed, convert x to automap before continuing.
	if (visibility == Automap) {
		POINT ptPos;
		ScreenToAutomap(&ptPos, x,y);
		retX = ptPos.x;
	}

	if (alignment & Center) {
		//Determine the center of the group 
		if (GetVisibility() == Group)
			return group->GetX() + (group->GetXSize() / 2);
		//Center hook on the given x.
		return retX - (GetXSize() / 2);
	}

	if (alignment & Right) {
		if (GetVisibility() == Group)
			return (group->GetX() + group->GetY()) - GetXSize();
		return retX - GetXSize();
	}
	return retX;
}

/* SetBaseX(unsigned int xPos)
 *	Sets the base x position, used for alignments 
 *  and/or offset from group position if necessary.
 */
void Hook::SetBaseX(unsigned int xPos) {
	Lock();
	x = xPos;
	Unlock();
}

/* GetBaseX()
 *	Returns the base x psotion, can be altered
 *	by alignment and groups if available.
 */
unsigned int Hook::GetBaseX() {
	return x;
}

/* GetY()
 *	Returns the x screen position that the hook will drawn at.
 */
unsigned int Hook::GetY() {
	unsigned int retY = (GetVisibility() == Group) ? group->GetY() + y : y;

	//If needed, convert x to automap before continuing.
	if (visibility == Automap) {
		POINT ptPos;
		ScreenToAutomap(&ptPos, x,y);
		retY = ptPos.y;
	}	
	
	if (alignment & Top)
		retY -= GetYSize();

	return retY;
}

/* SetBaseY(unsigned int yPos)
 *	Sets the base y position, used for alignments 
 *  and/or offset from group position if necessary.
 */
void Hook::SetBaseY(unsigned int yPos) {
	Lock();
	y = yPos;
	Unlock();
}

/* GetBaseY()
 *	Returns the base y position, can be altered
 *	by alignment and groups if available.
 */
unsigned int Hook::GetBaseY() {
	return y;
}

/* GetZOrder()
 *	Returns when the hook will be drawn compared 
 *	to other hooks.
 */
int Hook::GetZOrder() {
	return z;
}

/* SetZOrder(int zPos)
 *	Sets when the hook will be drawn compared to the other hooks.
 */
void Hook::SetZOrder(int zPos) {
	Lock();
	z = zPos;
	Unlock();
}

/* GetVisibility()
 *	Returns when the hook will be visible
 */
HookVisibility Hook::GetVisibility() {
	return visibility;
}

/* SetVisibility(HookVisibility newVisibility)
 *	Sets when the hook will be visible
 */
void Hook::SetVisibility(HookVisibility newVisibility) {
	Lock();
	visibility = newVisibility;
	Unlock();
}

/* IsActive()
 *	Returns if we are drawing the hook currently.
 */
bool Hook::IsActive() {
	return (group ? group->IsActive() : active);
}

/* SetActive(bool newActive)
 *	Sets if we should be drawing the hook.
 */
void Hook::SetActive(bool newActive) {
	Lock();
	active = newActive;
	Unlock();
}

/* GetAlignment()
 *	Returns how we are going to align the hook
 */
int Hook::GetAlignment() {
	return alignment;
}

/* SetAlignment(int newAlign)
 *	Sets how we are to align the hook.
 */
void Hook::SetAlignment(int newAlign) {
	Lock();
	alignment = newAlign;
	Unlock();
}

/* GetGroup()
 *	Returns the hook's group.
 */
HookGroup* Hook::GetGroup() {
	return group;
}

/* SetGroup(HookGroup* newGroup)
 *	Sets the hook's group.
 */
void Hook::SetGroup(HookGroup* newGroup) {
	Lock();
	group = newGroup;
	Unlock();
}

/* GetLeftClickHandler()
 *	Returns the handler for left clicking
 */
OnClick Hook::GetLeftClickHandler() {
	return left;
}

/* GetLeftClickVoid()
 *	Returns left click void.
 */  
void* Hook::GetLeftClickVoid() {
	return leftVoid;
}

/* SetLeftCallback(OnClick leftHandler, void* voidVar)
 *	Sets the left callback function
 */
void Hook::SetLeftCallback(OnClick leftHandler, void *voidVar) {
	Lock();
	left = leftHandler;
	leftVoid = voidVar;
	Unlock();
}


/* GetRightClickHandler()
 *	Returns the handler for right clicking
 */
OnClick Hook::GetRightClickHandler() {
	return right;
}

/* GetRightClickVoid()
 *	Returns right click void.
 */  
void* Hook::GetRightClickVoid() {
	return rightVoid;
}

/* SetRightCallback(OnClick rightHandler, void* voidVar)
 *	Sets the right callback function
 */
void Hook::SetRightCallback(OnClick rightHandler, void *voidVar) {
	Lock();
	right = rightHandler;
	rightVoid = voidVar;
	Unlock();
}

/* InRange(unsigned int x, unsigned int y)
 *	Returns if the x/y are within the draw's area.
 */
bool Hook::InRange(unsigned int x, unsigned int y) {
	return (IsActive() &&
		x >= GetX() &&
		y >= GetY() &&
		x <= GetX() + GetXSize() &&
		y <= GetY() + GetYSize());
}

/* Hook::GetScreenHeight()
 *	Returns the height of the screen.
 */
unsigned int Hook::GetScreenHeight() {
	return *p_D2CLIENT_ScreenSizeY;
}

/* Hook::GetScreenWidth()
 *	Returns the width of the screen.
 */
unsigned int Hook::GetScreenWidth() {
	return *p_D2CLIENT_ScreenSizeX;
}

/* Hook::ScreenToAutomap(int x, int y)
 *	Returns converted coordinates from screen to automap.
 */
void Hook::ScreenToAutomap(POINT* ptPos, int x, int y) {
	x *= 32; y *= 32;
	ptPos->x = ((x - y)/2/(*(INT*)p_D2CLIENT_Divisor))-(*p_D2CLIENT_Offset).x+8; 
	ptPos->y = ((x + y)/4/(*(INT*)p_D2CLIENT_Divisor))-(*p_D2CLIENT_Offset).y-8; 
	if(D2CLIENT_GetAutomapSize()) { 
		--ptPos->x; 
		ptPos->y += 5; 
	}
}

/* Hook::AutomapToScreen(int x, int y)
 *	Returns converted coordinates from automap to screen.
 */
void Hook::AutomapToScreen(POINT* ptPos, int x, int y) {
	ptPos->x = x; ptPos->y = y;
	D2COMMON_MapToAbsScreen(&ptPos->x, &ptPos->y);
}

/* ZSort
 *	Compares hook Z Orders
 */
bool ZSort (Hook* one, Hook* two) {
	return one->GetZOrder() < two->GetZOrder();
}

/* Hook::Draw(HookVisibility type)
 *	Called by Handlers to draw appropirate hooks
 */
void Hook::Draw(HookVisibility type) {
	if (type != Drawing::OutOfGame) {
		// The Z order of the hooks doesn't get changed while OOG, so there's
		// no need to repeatedly sort them
		Hooks.sort(ZSort);
	}
	for (HookIterator it = Hooks.begin(); it!=Hooks.end(); ++it)
		if ((*it)->GetVisibility() == type || (*it)->GetVisibility() == Perm)
			(*it)->OnDraw();
	if (Colorhook::current) {
		Colorhook::current->OnDraw();
		return;
	}
}

/* Hook::LeftClick(bool up, unsigned int x, unsigned int y)
 *	Calls the Left Click handlers and blocks click if needed.
 */
bool Hook::LeftClick(bool up, unsigned int x, unsigned int y) {
	Hooks.sort(ZSort);
	bool block = false;
	if (Colorhook::current) {
		Colorhook::current->OnLeftClick(up, x, y);
		return true;
	}
	for (list<Hook*>::iterator it = Hooks.begin(); it!=Hooks.end(); ++it)
		if ((*it)->IsActive())
			if ((*it)->OnLeftClick(up, x, y))
				block = true;
	return block;
}

/* Hook::RightClick(bool up, unsigned int x, unsigned int y)
 *	Calls the Right Click handlers and blocks click if needed.
 */
bool Hook::RightClick(bool up, unsigned int x, unsigned int y) {
	Hooks.sort(ZSort);
	bool block = false;
	if (Colorhook::current) {
		Colorhook::current->OnRightClick(up, x, y);
		return true;
	}
	for (HookIterator it = Hooks.begin(); it!=Hooks.end(); ++it)
		if ((*it)->IsActive())
			if ((*it)->OnRightClick(up, x, y))
				block = true;
	return block;
}

/* Hook::KeyClick(bool bUp, BYTE bKey, LPARAM lParam)
 *	Calls the Key Click handlers and blocks click if needed.
 */
bool Hook::KeyClick(bool bUp, BYTE bKey, LPARAM lParam) {
	Hooks.sort(ZSort);
	bool block = false;
	for (HookIterator it = Hooks.begin(); it!=Hooks.end(); ++it)
		if ((*it)->IsActive())
			if ((*it)->OnKey(bUp, bKey, lParam))
				block = true;
	return block;
}
