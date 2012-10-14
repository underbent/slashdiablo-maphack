#include "D2Ptrs.h"
#include "D2Stubs.h"
#include "Common.h"

RosterUnit* FindPlayerRoster(DWORD unitId) {
	for (RosterUnit* roster = (*p_D2CLIENT_PlayerUnitList); roster; roster = roster->pNext) {
		if (roster->dwUnitId == unitId)
			return roster;
	}
	return NULL;
}

int GetRelation(UnitAny* unit) {
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	RosterUnit* roster;
	RosterUnit* playerRoster = FindPlayerRoster(player->dwUnitId);

	//Neutral = 2, Partied = 3, You = 1, Hostile = 4
	if (!unit || !player)
		return 2;

	switch(unit->dwType) {
		case 0://Player

			// Check if we are the unit.
			if (unit->dwUnitId == player->dwUnitId)
				return 1;//You
			
			// Check if we are in a party with unit.
			roster = FindPlayerRoster(unit->dwUnitId);
			if (playerRoster && roster && playerRoster->wPartyId == roster->wPartyId && roster->wPartyId != 0xFFFF)
				return 3;//Partied
			
			// Check if unit is hostile towards us
			if (TestPvpFlag_STUB(unit->dwUnitId, player->dwUnitId, 8))
				return 4;

			return 2;
		break;
		case 1://Monster or NPC or Summon
		case 3://Missile

			if (unit->dwOwnerId < 0 || unit->dwOwnerType < 0 || unit->dwOwnerType > 5)
				return 4;

			if(unit->dwType == 1 && unit->dwOwnerType == 1)
				return 4;
			
			// Find the owner of the unit.
			UnitAny* owner = D2CLIENT_FindServerSideUnit(unit->dwOwnerId, unit->dwOwnerType);
			if (!owner || owner->dwType != unit->dwOwnerType || owner->dwUnitId != unit->dwOwnerId)
				return 4;

			return GetRelation(owner);
		break;
	}

	return 2;//Neutral
}

bool IsValidMonster(UnitAny *pUnit)
{
	//Have we even been given a unit?
	if (!pUnit)
		return false;

	//Check to see if unit is dead.
	if ((pUnit->dwMode == 0) || (pUnit->dwMode == 12))
		return false;

	if ((pUnit->dwTxtFileNo == 608) && (pUnit->dwMode == 8))
		return false;
	
	if ((pUnit->dwTxtFileNo == 68) && (pUnit->dwMode == 14))// Ignore burrowing maggots
		return false;

	//Hide the water creepers
	if ((pUnit->dwTxtFileNo == 258 || (pUnit->dwTxtFileNo == 261)) && (pUnit->dwMode == 14))
		return false;

	DWORD badMonIds[] = {227, 283, 326, 327, 328, 329, 330, 410, 411, 412, 413, 414, 415, 416, 366, 406,
						 351, 352, 353, 266, 408, 516, 517, 518, 519, 522, 523, 543, 543, 545};

	for (DWORD n = 0; n < 30; n++)
	{
		if (pUnit->dwTxtFileNo == badMonIds[n])
			return false;
	}

	if (D2COMMON_GetUnitStat(pUnit, 172, 0) == 2) 
		return false;

	wchar_t* name = D2CLIENT_GetUnitName(pUnit);
	char* tmp = UnicodeToAnsi(name);

	if ((strcmp(tmp,"an evil force") == 0) || (strcmp(tmp, "dummy") == 0) || (strcmp(tmp, "Maggot") == 0)) {
		delete[] tmp;
		return false;
	}
	delete[] tmp;

	return true;
}