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
	int width = 220;
	int height = 400;

	vector<pair<string, string>> stats = BH::config->ReadMapList("Stat Screen");
	for (unsigned int i = 0; i < stats.size(); i++) {
		std::transform(stats[i].first.begin(), stats[i].first.end(), stats[i].first.begin(), ::tolower);
		if (StatMap.count(stats[i].first) > 0) {
			StatProperties *sp = StatMap[stats[i].first];
			DisplayedStat *customStat = new DisplayedStat();
			customStat->name = stats[i].first;
			customStat->useValue = false;
			std::transform(customStat->name.begin(), customStat->name.end(), customStat->name.begin(), ::tolower);
			if (sp->saveParamBits > 0) {
				int num = -1;
				stringstream ss(Trim(stats[i].second));
				if ((ss >> num).fail() || num < 0) {
					continue;
				}
				customStat->useValue = true;
				customStat->value = num;
			}
			customStats.push_back(customStat);
		}
	}
	if (customStats.size() > 0) {
		height += (customStats.size() * 16) + 8;
	}

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
		Texthook::Draw(115, (y += 8), Center, 1, Silver, "Character Stats");
		Texthook::Draw(15, (y += 24), None, 6, Gold, "�c4Name:�c0 %s", unit->pPlayerData->szName);
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Level:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LEVEL, 0));
		y += 8;

		BnetData* pData = (*p_D2LAUNCH_BnData);
		int xPacMultiplier = pData->nCharFlags & PLAYER_TYPE_EXPANSION ? 2 : 1;
		int resPenalty[3] = { RES_PENALTY_CLS_NORM, RES_PENALTY_CLS_NM, RES_PENALTY_CLS_HELL };
		int penalty = resPenalty[D2CLIENT_GetDifficulty()] * xPacMultiplier;
		int fMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXFIRERESIST, 0) + 75;
		int cMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXCOLDRESIST, 0) + 75;
		int lMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXLIGHTNINGRESIST, 0) + 75;
		int pMax = (int)D2COMMON_GetUnitStat(unit, STAT_MAXPOISONRESIST, 0) + 75;
		Texthook::Draw(15, (y += 16), None, 6, Red, "�c4Fire Resist:�c1 %d �c0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_FIRERESIST, 0) + penalty, fMax);
		Texthook::Draw(15, (y += 16), None, 6, Blue, "�c4Cold Resist:�c3 %d �c0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_COLDRESIST, 0) + penalty, cMax);
		Texthook::Draw(15, (y += 16), None, 6, Yellow, "�c4Lightning Resist:�c9 %d �c0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGRESIST, 0) + penalty, lMax);
		Texthook::Draw(15, (y += 16), None, 6, Green, "�c4Poison Resist:�c2 %d �c0/ %d", (int)D2COMMON_GetUnitStat(unit, STAT_POISONRESIST, 0) + penalty, pMax);
		y += 8;

		int fAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_FIREABSORB, 0);
		int fAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_FIREABSORBPERCENT, 0);
		int cAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_COLDABSORB, 0);
		int cAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_COLDABSORBPERCENT, 0);
		int lAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGABSORB, 0);
		int lAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_LIGHTNINGABSORBPERCENT, 0);
		int mAbsorb = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICABSORB, 0);
		int mAbsorbPct = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICABSORBPERCENT, 0);
		Texthook::Draw(15, (y += 16), None, 6, Red, "�c4Absorption: �c1%d�c0/�c1%d%c �c3%d�c0/�c3%d%c �c9%d�c0/�c9%d%c �c8%d�c0/�c8%d%c", fAbsorb, fAbsorbPct, '%', cAbsorb, cAbsorbPct, '%', lAbsorb, lAbsorbPct, '%', mAbsorb, mAbsorbPct, '%');

		int dmgReduction = (int)D2COMMON_GetUnitStat(unit, STAT_DMGREDUCTION, 0);
		int dmgReductionPct = (int)D2COMMON_GetUnitStat(unit, STAT_DMGREDUCTIONPCT, 0);
		int magReduction = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICDMGREDUCTION, 0);
		int magReductionPct = (int)D2COMMON_GetUnitStat(unit, STAT_MAGICDMGREDUCTIONPCT, 0);
		Texthook::Draw(15, (y += 16), None, 6, Tan, "�c4Damage Reduction: �c7%d�c0/�c7%d%c �c8%d�c0/�c8%d%c", dmgReduction, dmgReductionPct, '%', magReduction, magReductionPct, '%');
		y += 8;

		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Faster Cast Rate:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERCAST, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Faster Block Rate:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERBLOCK, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Faster Hit Recovery:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERHITRECOVERY, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Faster Run/Walk:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_FASTERRUNWALK, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Increased Attack Speed:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_IAS, 0));
		y += 8;

		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Crushing Blow:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_CRUSHINGBLOW, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Life Leech:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_LIFELEECH, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Mana Leech:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MANALEECH, 0));
		y += 8;

		int cowKingKilled = D2COMMON_GetQuestFlag(D2CLIENT_GetQuestInfo(), 4, 10);
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Magic Find:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_MAGICFIND, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Gold Find:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDFIND, 0));
		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Stash Gold:�c0 %d", (int)D2COMMON_GetUnitStat(unit, STAT_GOLDBANK, 0));

		Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4Cow King:�c0 %s", cowKingKilled ? "killed" : "alive");

		if (customStats.size() > 0) {
			y += 8;
			for (unsigned int i = 0; i < customStats.size(); i++) {
				int secondary = customStats[i]->useValue ? customStats[i]->value : 0;
				int stat = (int)D2COMMON_GetUnitStat(unit, STAT_NUMBER(customStats[i]->name), secondary);
				Texthook::Draw(15, (y += 16), None, 6, Gold, "�c4%s:�c0 %d", customStats[i]->name.c_str(), stat);
			}
		}
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
