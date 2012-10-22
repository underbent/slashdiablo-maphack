#include "StatsDisplay.h"
#include "../Basic/Texthook/Texthook.h"
#include "../Basic/Framehook/Framehook.h"
#include "../Basic/Boxhook/Boxhook.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"

using namespace Drawing;

StatsDisplay *StatsDisplay::display;

StatsDisplay::StatsDisplay(std::string name) {
	int xPos = 10;
	int yPos = 10;
	int width = 200;
	int height = 400;

	InitializeCriticalSection(&crit);
	SetX(xPos);
	SetY(yPos);
	SetXSize(width);
	SetYSize(height);
	SetName(name);
	SetActive(true);
	SetMinimized(true);

	statsKey = BH::config->ReadKey("Character Stats", "VK_8");
	display = this;
}

StatsDisplay::~StatsDisplay() {
	Lock();
	// Remove all hooks associated with the display
	while (Hooks.size() > 0) {
		delete (*Hooks.begin());
	}
	Unlock();
	DeleteCriticalSection(&crit);
}

void StatsDisplay::SetX(unsigned int newX) {
	if (newX >= 0 && newX <= Hook::GetScreenWidth()) {
		Lock();
		x = newX;
		Unlock();
	}
}

void StatsDisplay::SetY(unsigned int newY) {
	if (newY >= 0 && newY <= Hook::GetScreenHeight()) {
		Lock();
		y = newY;
		Unlock();
	}
}

void StatsDisplay::SetXSize(unsigned int newXSize) {
	if (newXSize >= 0 && newXSize <= (Hook::GetScreenHeight() - GetX())) {
		Lock();
		xSize = newXSize;
		Unlock();
	}
}

void StatsDisplay::SetYSize(unsigned int newYSize) {
	if (newYSize >= 0 && newYSize <= (Hook::GetScreenHeight() - GetX())) {
		Lock();
		ySize = newYSize;
		Unlock();
	}
}

bool StatsDisplay::InRange(unsigned int x, unsigned int y) {
	return IsActive() &&
		x >= GetX() && y >= GetY() &&
		x <= GetX() + GetXSize() && y <= GetY() + GetYSize();
}

void StatsDisplay::Draw() {
	display->Lock();
	display->OnDraw();
	display->Unlock();
}

void StatsDisplay::OnDraw() {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return;

	if (!IsMinimized()) {
		for(std::list<Hook*>::iterator it = Hooks.begin(); it != Hooks.end(); it++)
			(*it)->OnDraw();

		int y = GetY();
		RECT pRect;
		pRect.left = GetX();
		pRect.top = y;
		pRect.right = x + GetXSize();
		pRect.bottom = y + GetYSize();

		Drawing::Boxhook::Draw(GetX(),GetY(), GetXSize(), GetYSize(), White, Drawing::BTBlack);
		Drawing::Framehook::DrawRectStub(&pRect);
		Texthook::Draw(105, (y += 8), Center, 1, Silver, "Character Stats");
		Texthook::Draw(15, (y += 24), None, 6, Gold, "ÿc4Name:ÿc0 %s", unit->pPlayerData->szName);
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Level:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LEVEL, 0));
		y += 8;
		int fMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXFIRERESIST, 0) + 75;
		int cMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXCOLDRESIST, 0) + 75;
		int lMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXLIGHTNINGRESIST, 0) + 75;
		int pMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXPOISONRESIST, 0) + 75;
		int mMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXMAGICRESIST, 0) + 75;
		Texthook::Draw(15, (y += 16), None, 6, Red, "ÿc1Fire Resist:ÿc0 %d / %d", (int)D2COMMON_GetUnitStat(unit, STAT_FIRERESIST, 0), fMax);
		Texthook::Draw(15, (y += 16), None, 6, Blue, "ÿc3Cold Resist:ÿc0 %d / %d", (int)D2COMMON_GetUnitStat(unit, STAT_COLDRESIST, 0), cMax);
		Texthook::Draw(15, (y += 16), None, 6, Yellow, "ÿc9Lightning Resist:ÿc0 %d / %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGRESIST, 0), lMax);
		Texthook::Draw(15, (y += 16), None, 6, Green, "ÿc2Poison Resist:ÿc0 %d / %d", (int)D2COMMON_GetUnitStat(unit, STAT_POISONRESIST, 0), pMax);
		Texthook::Draw(15, (y += 16), None, 6, Orange, "ÿc8Magic Resist:ÿc0 %d / %d", (int)D2COMMON_GetUnitStat(unit, STAT_MAGICRESIST, 0), mMax);
		y += 8;
		int fAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_FIREABSORBPERCENT, 0);
		int cAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_COLDABSORBPERCENT, 0);
		int lAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGABSORBPERCENT, 0);
		int mAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICABSORBPERCENT, 0);
		Texthook::Draw(15, (y += 16), None, 6, Red, "ÿc4Absorption: ÿc1%dÿc0 / ÿc3%dÿc0 / ÿc9%dÿc0 / ÿc8%d", fAbsorb, cAbsorb, lAbsorb, mAbsorb);
		Texthook::Draw(15, (y += 16), None, 6, Tan, "ÿc4Damage Reduction:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_DAMAGEREDUCTION, 0));
		y += 8;
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Faster Cast Rate:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Faster Block Rate:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Faster Hit Recovery:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Faster Run/Walk:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERRUNWALK, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Increased Attack Speed:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_IAS, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Life Leech:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIFELEECH, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Mana Leech:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MANALEECH, 0));
		y += 8;
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Magic Find:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MAGICFIND, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Gold Find:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDFIND, 0));
		y += 8;
		Texthook::Draw(15, (y += 16), None, 6, Gold, "ÿc4Stash Gold:ÿc0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDBANK, 0));
	}
}

bool StatsDisplay::KeyClick(bool bUp, BYTE bKey, LPARAM lParam) {
	display->Lock();
	bool block = display->OnKey(bUp, bKey, lParam);
	display->Unlock();
	return block;
}

bool StatsDisplay::OnKey(bool up, BYTE kkey, LPARAM lParam) {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return false;

	if (IsMinimized()) {
		if (!up && kkey == statsKey) {
			SetMinimized(false);
			return true;
		}
	} else {
		if (!up && (kkey == statsKey || kkey == VK_ESCAPE)) {
			SetMinimized(true);
			return true;
		}
	}
	return false;
}

bool StatsDisplay::Click(bool up, unsigned int mouseX, unsigned int mouseY) {
	display->Lock();
	bool block = display->OnClick(up, mouseX, mouseY);
	display->Unlock();
	return block;
}

bool StatsDisplay::OnClick(bool up, unsigned int x, unsigned int y) {
	UnitAny *unit = D2CLIENT_GetPlayerUnit();
	if (!unit)
		return false;

	if (!IsMinimized() && InRange(x, y)) {
		SetMinimized(true);
		return true;
	}
	return false;
}
