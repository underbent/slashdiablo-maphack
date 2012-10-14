#pragma once
#include <Windows.h>
#include "D2Structs.h"

bool IsValidMonster(UnitAny *pUnit);
int GetRelation(UnitAny* unit);
RosterUnit* FindPlayerRoster(DWORD unitId);