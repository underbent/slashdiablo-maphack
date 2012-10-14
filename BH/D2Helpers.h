#pragma once
#include <Windows.h>
#include "D2Structs.h"

#define INVALID_PARTY_ID 0xFFFF

bool IsValidMonster(UnitAny *pUnit);
int GetRelation(UnitAny* unit);
RosterUnit* FindPlayerRoster(DWORD unitId);