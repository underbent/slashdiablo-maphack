/* Maphack Module
 *
 */
#include "../../D2Ptrs.h"
#include "../../D2Helpers.h"
#include "../../D2Stubs.h"
#include "../../D2Intercepts.h"
#include "Maphack.h"
#include "../../BH.h"
#include "../../Drawing.h"
#include "../Item/ItemDisplay.h"
#include "../../AsyncDrawBuffer.h"

using namespace Drawing;
Patch* weatherPatch = new Patch(Jump, D2COMMON, 0x6CC56, (int)Weather_Interception, 5);
Patch* lightingPatch = new Patch(Call, D2CLIENT, 0xA9A37, (int)Lighting_Interception, 6);
Patch* infraPatch = new Patch(Call, D2CLIENT, 0x66623, (int)Infravision_Interception, 7);
Patch* shakePatch = new Patch(Call, D2CLIENT, 0x442A2,(int)Shake_Interception, 5);

DrawDirective automapDraw(true, 5);

Maphack::Maphack() : Module("Maphack") {
	revealType = MaphackRevealAct;
	ResetRevealed();
	ReadConfig();
}

void Maphack::ReadConfig() {
	revealType = (MaphackReveal)BH::config->ReadInt("RevealMode", 0);
	monsterResistanceThreshold = BH::config->ReadInt("Show Monster Resistance", 100);

	Config automap(BH::config->ReadAssoc("Missile Color"));
	automapColors["Player Missile"] = automap.ReadInt("Player", 0x97);
	automapColors["Neutral Missile"] = automap.ReadInt("Neutral", 0x0A);
	automapColors["Partied Missile"] = automap.ReadInt("Partied", 0x84);
	automapColors["Hostile Missile"] = automap.ReadInt("Hostile", 0x5B);

	TextColorMap["�c0"] = 0x20;  // white
	TextColorMap["�c1"] = 0x0A;  // red
	TextColorMap["�c2"] = 0x84;  // green
	TextColorMap["�c3"] = 0x97;  // blue
	TextColorMap["�c4"] = 0x0D;  // gold
	TextColorMap["�c5"] = 0xD0;  // gray
	TextColorMap["�c6"] = 0x00;  // black
	TextColorMap["�c7"] = 0x5A;  // tan
	TextColorMap["�c8"] = 0x60;  // orange
	TextColorMap["�c9"] = 0x0C;  // yellow
	TextColorMap["�c;"] = 0x9B;  // purple

	map<string, string> MonsterColors = BH::config->ReadAssoc("Monster Color");
	for (auto it = MonsterColors.cbegin(); it != MonsterColors.cend(); ) {
		// If the key is a number, it means a monster we've assigned a specific color
		int monsterId = -1;
		stringstream ss((*it).first);
		if ((ss >> monsterId).fail()) {
			++it;
		} else {
			int monsterColor = StringToNumber((*it).second);
			automapMonsterColors[monsterId] = monsterColor;
			MonsterColors.erase(it++);
		}
	}

	Config monster(MonsterColors);
	automapColors["Normal Monster"] = monster.ReadInt("Normal", 0x5B);
	automapColors["Minion Monster"] = monster.ReadInt("Minion", 0x60);
	automapColors["Champion Monster"] = monster.ReadInt("Champion", 0x91);
	automapColors["Boss Monster"] = monster.ReadInt("Boss", 0x84);

	Toggles["Auto Reveal"] = BH::config->ReadToggle("Reveal Map", "None", true);
	Toggles["Show Monsters"] = BH::config->ReadToggle("Show Monsters", "None", true);
	Toggles["Show Missiles"] = BH::config->ReadToggle("Show Missiles", "None", true);
	Toggles["Show Chests"] = BH::config->ReadToggle("Show Chests", "None", true);
	Toggles["Force Light Radius"] = BH::config->ReadToggle("Force Light Radius", "None", true);
	Toggles["Remove Weather"] = BH::config->ReadToggle("Remove Weather", "None", true);
	Toggles["Infravision"] = BH::config->ReadToggle("Infravision", "None", true);
	Toggles["Remove Shake"] = BH::config->ReadToggle("Remove Shake", "None", false);
	Toggles["Display Level Names"] = BH::config->ReadToggle("Display Level Names", "None", true);

	automapDraw.maxGhost = BH::config->ReadInt("Minimap Max Ghost", 5);
}

void Maphack::ResetRevealed() {
	revealedGame = false;
	for (int act = 0; act < 6; act++)
		revealedAct[act] = false;
	for (int level = 0; level < 255; level++)
		revealedLevel[level] = false;
}

void Maphack::ResetPatches() {
	//Lighting Patch
	if (Toggles["Force Light Radius"].state)
		lightingPatch->Install();
	else
		lightingPatch->Remove();

	//Weather Patch
	if (Toggles["Remove Weather"].state)
		weatherPatch->Install();
	else
		weatherPatch->Remove();

	//Infravision Patch
	if (Toggles["Infravision"].state)
		infraPatch->Install();
	else
		infraPatch->Remove();
		//GameShake Patch
	if (Toggles["Remove Shake"].state)
		shakePatch->Install();
	else
		shakePatch->Remove();

}

void Maphack::OnLoad() {
	/*ResetRevealed();
	ReadConfig();
	ResetPatches();*/

	settingsTab = new UITab("Maphack", BH::settingsUI);

	new Texthook(settingsTab, 80, 3, "Toggles");
	unsigned int Y = 0;
	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Auto Reveal"].state, "Auto Reveal");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Auto Reveal"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Show Monsters"].state, "Show Monsters");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Show Monsters"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Show Missiles"].state, "Show Missiles");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Show Missiles"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Show Chests"].state, "Show Chests");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Show Chests"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Force Light Radius"].state, "Light Radius");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Force Light Radius"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Remove Weather"].state, "Remove Weather");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Remove Weather"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Infravision"].state, "Infravision");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Infravision"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Remove Shake"].state, "Remove Shake");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Remove Shake"].toggle, "");

	new Checkhook(settingsTab, 4, (Y += 15), &Toggles["Display Level Names"].state, "Level Names");
	new Keyhook(settingsTab, 130, (Y + 2), &Toggles["Display Level Names"].toggle, "");

	new Texthook(settingsTab, 215, 3, "Missile Colors");

	new Colorhook(settingsTab, 210, 17, &automapColors["Player Missile"], "Player");
	new Colorhook(settingsTab, 210, 32, &automapColors["Neutral Missile"], "Neutral");
	new Colorhook(settingsTab, 210, 47, &automapColors["Partied Missile"], "Partied");
	new Colorhook(settingsTab, 210, 62, &automapColors["Hostile Missile"], "Hostile");

	new Texthook(settingsTab, 215, 77, "Monster Colors");

	new Colorhook(settingsTab, 210, 92, &automapColors["Normal Monster"], "Normal");
	new Colorhook(settingsTab, 210, 107, &automapColors["Minion Monster"], "Minion");
	new Colorhook(settingsTab, 210, 122, &automapColors["Champion Monster"], "Champion");
	new Colorhook(settingsTab, 210, 137, &automapColors["Boss Monster"], "Boss");

	new Texthook(settingsTab, 3, (Y += 15), "Reveal Type:");

	vector<string> options;
	options.push_back("Game");
	options.push_back("Act");
	options.push_back("Level");
	new Combohook(settingsTab, 100, Y, 70, &revealType, options);

}

void Maphack::OnKey(bool up, BYTE key, LPARAM lParam, bool* block) {
	for (map<string,Toggle>::iterator it = Toggles.begin(); it != Toggles.end(); it++) {
		if (key == (*it).second.toggle) {
			*block = true;
			if (up) {
				(*it).second.state = !(*it).second.state;
				ResetPatches();
			}
			return;
		}
	}
	return;
}

void Maphack::OnUnload() {
	lightingPatch->Remove();
	weatherPatch->Remove();
	infraPatch->Remove();
	shakePatch->Remove();
}

void Maphack::OnLoop() {
	//// Remove or install patchs based on state.
	ResetPatches();

	// Get the player unit for area information.
	UnitAny* unit = D2CLIENT_GetPlayerUnit();
	if (!unit || !Toggles["Auto Reveal"].state)
		return;
	
	// Reveal the automap based on configuration.
	switch((MaphackReveal)revealType) {
		case MaphackRevealGame:
			RevealGame();
		break;
		case MaphackRevealAct:
			RevealAct(unit->pAct->dwAct + 1);
		break;
		case MaphackRevealLevel:
			RevealLevel(unit->pPath->pRoom1->pRoom2->pLevel);
		break;
	}
}

bool IsObjectChest(ObjectTxt *obj)
{
	//ObjectTxt *obj = D2COMMON_GetObjectTxt(objno);
	return (obj->nSelectable0 && (
		(obj->nOperateFn == 1) || //bed, undef grave, casket, sarc
		(obj->nOperateFn == 3) || //basket, urn, rockpile, trapped soul
		(obj->nOperateFn == 4) || //chest, corpse, wooden chest, buriel chest, skull and rocks, dead barb
		(obj->nOperateFn == 5) || //barrel
		(obj->nOperateFn == 7) || //exploding barrel
		(obj->nOperateFn == 14) || //loose bolder etc....*
		(obj->nOperateFn == 19) || //armor stand
		(obj->nOperateFn == 20) || //weapon rack
		(obj->nOperateFn == 33) || //writ
		(obj->nOperateFn == 48) || //trapped soul
		(obj->nOperateFn == 51) || //stash
		(obj->nOperateFn == 68)    //evil urn
		));
}

BYTE nChestClosedColour = 0x09;
BYTE nChestLockedColour = 0x09;

Act* lastAct = NULL;
void Maphack::OnAutomapDraw() {
	UnitAny* player = D2CLIENT_GetPlayerUnit();
	
	if (!player || !player->pAct || player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo == 0)
		return;

	if (lastAct != player->pAct){
		lastAct = player->pAct;
		automapDraw.forceUpdate();
	}

	if (!IsInitialized()){
		Drawing::Texthook::Draw(10, 70, Drawing::None, 12, Gold, "Loading MPQ Data...");
	}
	
	automapDraw.draw([=](AsyncDrawBuffer &automapBuffer) -> void {
		for (Room1* room1 = player->pAct->pRoom1; room1; room1 = room1->pRoomNext) {
			for (UnitAny* unit = room1->pUnitFirst; unit; unit = unit->pListNext) {
				//POINT automapLoc;
				DWORD xPos, yPos;

				// Draw monster on automap
				if (unit->dwType == UNIT_MONSTER && IsValidMonster(unit) && Toggles["Show Monsters"].state) {
					int color = automapColors["Normal Monster"];
					if (unit->pMonsterData->fBoss)
						color = automapColors["Boss Monster"];
					if (unit->pMonsterData->fChamp)
						color = automapColors["Champion Monster"];
					if (unit->pMonsterData->fMinion)
						color = automapColors["Minion Monster"];

					// User can override colors of non-boss monsters
					if (automapMonsterColors.find(unit->dwTxtFileNo) != automapMonsterColors.end() && !unit->pMonsterData->fBoss) {
						color = automapMonsterColors[unit->dwTxtFileNo];
					}

					//Determine immunities
					string szImmunities[] = { "�c7i", "�c8i", "�c1i", "�c9i", "�c3i", "�c2i" };
					string szResistances[] = { "�c7r", "�c8r", "�c1r", "�c9r", "�c3r", "�c2r" };
					DWORD dwImmunities[] = {
						STAT_DMGREDUCTIONPCT,
						STAT_MAGICDMGREDUCTIONPCT,
						STAT_FIRERESIST,
						STAT_LIGHTNINGRESIST,
						STAT_COLDRESIST,
						STAT_POISONRESIST
					};
					string immunityText;
					for (int n = 0; n < 6; n++) {
						int nImm = D2COMMON_GetUnitStat(unit, dwImmunities[n], 0);
						if (nImm >= 100) {
							immunityText += szImmunities[n];
						}
						else if (nImm >= monsterResistanceThreshold) {
							immunityText += szResistances[n];
						}
					}

					xPos = unit->pPath->xPos;
					yPos = unit->pPath->yPos;
					automapBuffer.push([immunityText, color, xPos, yPos]()->void{
						POINT automapLoc;
						Drawing::Hook::ScreenToAutomap(&automapLoc, xPos, yPos);
						if (immunityText.length() > 0)
							Drawing::Texthook::Draw(automapLoc.x, automapLoc.y - 8, Drawing::Center, 6, White, immunityText);
						Drawing::Crosshook::Draw(automapLoc.x, automapLoc.y, color);
					});
				}
				else if (unit->dwType == UNIT_MISSILE && Toggles["Show Missiles"].state) {
					int color = 255;
					switch (GetRelation(unit)) {
					case 0:
						continue;
						break;
					case 1://Me
						color = automapColors["Player Missile"];
						break;
					case 2://Neutral
						color = automapColors["Neutral Missile"];
						break;
					case 3://Partied
						color = automapColors["Partied Missile"];
						break;
					case 4://Hostile
						color = automapColors["Hostile Missile"];
						break;
					}

					xPos = unit->pPath->xPos;
					yPos = unit->pPath->yPos;					
					automapBuffer.push([color, unit, xPos, yPos]()->void{
						POINT automapLoc;
						Drawing::Hook::ScreenToAutomap(&automapLoc, xPos, yPos);
						Drawing::Boxhook::Draw(automapLoc.x - 1, automapLoc.y - 1, 2, 2, color, Drawing::BTHighlight);
					});
				}
				else if (unit->dwType == UNIT_ITEM) {
					UnitItemInfo uInfo;
					uInfo.item = unit;
					uInfo.itemCode[0] = D2COMMON_GetItemText(unit->dwTxtFileNo)->szCode[0];
					uInfo.itemCode[1] = D2COMMON_GetItemText(unit->dwTxtFileNo)->szCode[1];
					uInfo.itemCode[2] = D2COMMON_GetItemText(unit->dwTxtFileNo)->szCode[2];
					uInfo.itemCode[3] = 0;
					if (ItemAttributeMap.find(uInfo.itemCode) != ItemAttributeMap.end()) {
						uInfo.attrs = ItemAttributeMap[uInfo.itemCode];
						for (vector<Rule*>::iterator it = MapRuleList.begin(); it != MapRuleList.end(); it++) {
							if ((*it)->Evaluate(&uInfo, NULL)) {
								auto color = TextColorMap[(*it)->action.colorOnMap];
								
								xPos = unit->pItemPath->dwPosX;
								yPos = unit->pItemPath->dwPosY;
								automapBuffer.push([color, unit, xPos, yPos]()->void{
									POINT automapLoc;
									Drawing::Hook::ScreenToAutomap(&automapLoc, xPos, yPos);
									Drawing::Boxhook::Draw(automapLoc.x - 4, automapLoc.y - 4, 8, 8, color, Drawing::BTHighlight);
								});
								break;
							}
						}
					}
					else {
						HandleUnknownItemCode(uInfo.itemCode, "on map");
					}
				}
				else if (unit->dwType == UNIT_OBJECT && !unit->dwMode /* Not opened */ && Toggles["Show Chests"].state && IsObjectChest(unit->pObjectData->pTxt)) {
					xPos = unit->pObjectPath->dwPosX;
					yPos = unit->pObjectPath->dwPosY;
					automapBuffer.push([xPos, yPos]()->void{
						POINT automapLoc;
						Drawing::Hook::ScreenToAutomap(&automapLoc, xPos, yPos);
						Drawing::Boxhook::Draw(automapLoc.x - 1, automapLoc.y - 1, 2, 2, 255, Drawing::BTHighlight);
					});
				}
			}
		}

		if (!Toggles["Display Level Names"].state)
			return;
		for (list<LevelList*>::iterator it = automapLevels.begin(); it != automapLevels.end(); it++) {
			if (player->pAct->dwAct == (*it)->act) {
				string tombStar = ((*it)->levelId == player->pAct->pMisc->dwStaffTombLevel) ? "�c2*" : "�c4";
				POINT unitLoc;
				Hook::ScreenToAutomap(&unitLoc, (*it)->x, (*it)->y);
				char* name = UnicodeToAnsi(D2CLIENT_GetLevelName((*it)->levelId));
				std::string nameStr = name;
				delete[] name;

				automapBuffer.push([nameStr, tombStar, unitLoc]()->void{
					Texthook::Draw(unitLoc.x, unitLoc.y - 15, Center, 6, Gold, "%s%s", nameStr.c_str(), tombStar.c_str());
				});
			}
		}
	});
}

void Maphack::OnGameJoin(const string& name, const string& pass, int diff) {
	ResetRevealed();
	automapLevels.clear();
}

void Squelch(DWORD Id, BYTE button) {
	LPBYTE aPacket = new BYTE[7];	//create packet
	*(BYTE*)&aPacket[0] = 0x5d;	
	*(BYTE*)&aPacket[1] = button;	
	*(BYTE*)&aPacket[2] = 1;	
	*(DWORD*)&aPacket[3] = Id;
	D2NET_SendPacket(7, 0, aPacket);

	delete [] aPacket;	//clearing up data

	return;
}

void Maphack::OnGamePacketRecv(BYTE *packet, bool *block) {
	switch (packet[0]) {

	case 0x9c: {
		INT64 icode   = 0;
        char code[5]  = "";
        BYTE mode     = packet[1];
        DWORD gid     = *(DWORD*)&packet[4];
        BYTE dest     = ((packet[13] & 0x1C) >> 2);

        switch(dest)
        {
                case 0: 
                case 2:
                        icode = *(INT64 *)(packet+15)>>0x04;
                        break;
                case 3:
                case 4:
                case 6:
                        if(!((mode == 0 || mode == 2) && dest == 3))
                        {
                                if(mode != 0xF && mode != 1 && mode != 12)
                                        icode = *(INT64 *)(packet+17) >> 0x1C;
                                else
                                        icode = *(INT64 *)(packet+15) >> 0x04;
                        } 
                        else  
                                icode = *(INT64 *)(packet+17) >> 0x05;
                        break;
                default:
                        break;
        }

        memcpy(code, &icode, 4);
        if(code[3] == ' ') code[3] = '\0';

        //PrintText(1, "%s", code);

		//if(mode == 0x0 || mode == 0x2 || mode == 0x3) {
		//	BYTE ear = packet[10] & 0x01;
		//	if(ear) *block = true;
		//}
		break;
		}

	case 0xa8:
	case 0xa7: {
			//if(packet[1] == 0x0) {
			//	if(packet[6+(packet[0]-0xa7)] == 100) {
			//		UnitAny* pUnit = D2CLIENT_FindServerSideUnit(*(DWORD*)&packet[2], 0);
			//		if(pUnit)
			//			PrintText(1, "Alert: �c4Player �c2%s �c4drank a �c1Health �c4potion!", pUnit->pPlayerData->szName);
			//	} else if (packet[6+(packet[0]-0xa7)] == 105) {
			//		UnitAny* pUnit = D2CLIENT_FindServerSideUnit(*(DWORD*)&packet[2], 0);
			//		if(pUnit)
			//			if(pUnit->dwTxtFileNo == 1)
			//				if(D2COMMON_GetUnitState(pUnit, 30))
			//					PrintText(1, "Alert: �c4ES Sorc �c2%s �c4drank a �c3Mana �c4Potion!", pUnit->pPlayerData->szName);
			//	} else if (packet[6+(packet[0]-0xa7)] == 102) {//remove portal delay
			//		*block = true;
			//	}
			//}
			break;			   
		}
	case 0x94: {
			BYTE Count = packet[1];
			DWORD Id = *(DWORD*)&packet[2];
			for(DWORD i = 0;i < Count;i++) {
				BaseSkill S;
				S.Skill = *(WORD*)&packet[6+(3*i)];
				S.Level = *(BYTE*)&packet[8+(3*i)];
				Skills[Id].push_back(S);
			}
			//for(vector<BaseSkill>::iterator it = Skills[Id].begin();  it != Skills[Id].end(); it++)
			//	PrintText(1, "Skill %d, Level %d", it->Skill, it->Level);
			break;
		}
	case 0x5b: {	//36   Player In Game      5b [WORD Packet Length] [DWORD Player Id] [BYTE Char Type] [NULLSTRING[16] Char Name] [WORD Char Lvl] [WORD Party Id] 00 00 00 00 00 00 00 00
			WORD lvl = *(WORD*)&packet[24];
			DWORD Id = *(DWORD*)&packet[3];
			char* name = (char*)&packet[8];
			UnitAny* Me = D2CLIENT_GetPlayerUnit();
			if(!Me)
				return;
			else if (!strcmp(name, Me->pPlayerData->szName))
				return;
			//if(lvl < 9)
			//	Squelch(Id, 3);
		}			//2 = mute, 3 = squelch, 4 = hostile
	}
}

void Maphack::RevealGame() {
	// Check if we have already revealed the game.
	if (revealedGame)
		return;

	// Iterate every act and reveal it.
	for (int act = 1; act <= ((*p_D2CLIENT_ExpCharFlag) ? 5 : 4); act++) {
		RevealAct(act);
	}

	revealedGame = true;
}

void Maphack::RevealAct(int act) {
	// Make sure we are given a valid act
	if (act < 1 || act > 5)
		return;

	// Check if the act is already revealed
	if (revealedAct[act])
		return;

	UnitAny* player = D2CLIENT_GetPlayerUnit();
	if (!player || !player->pAct)
		return;

	// Initalize the act incase it is isn't the act we are in.
	int actIds[6] = {1, 40, 75, 103, 109, 137};
	Act* pAct = D2COMMON_LoadAct(act - 1, player->pAct->dwMapSeed, *p_D2CLIENT_ExpCharFlag, 0, D2CLIENT_GetDifficulty(), NULL, actIds[act - 1], D2CLIENT_LoadAct_1, D2CLIENT_LoadAct_2);
	if (!pAct || !pAct->pMisc)
		return;

	// Iterate every level for the given act.
	for (int level = actIds[act - 1]; level < actIds[act]; level++) {
		Level* pLevel = GetLevel(pAct, level);
		if (!pLevel)
			continue;
		if (!pLevel->pRoom2First)
			D2COMMON_InitLevel(pLevel);
		RevealLevel(pLevel);
	}

	InitLayer(player->pPath->pRoom1->pRoom2->pLevel->dwLevelNo);
	D2COMMON_UnloadAct(pAct);
	revealedAct[act] = true;
}

void Maphack::RevealLevel(Level* level) {
	// Basic sanity checks to ensure valid level
	if (!level || level->dwLevelNo < 0 || level->dwLevelNo > 255)
		return;

	// Check if the level has been previous revealed.
	if (revealedLevel[level->dwLevelNo])
		return;

	InitLayer(level->dwLevelNo);

	// Iterate every room in the level.
	for(Room2* room = level->pRoom2First; room; room = room->pRoom2Next) {
		bool roomData = false;

		//Add Room1 Data if it is not already there.
		if (!room->pRoom1) {
			D2COMMON_AddRoomData(level->pMisc->pAct, level->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1);
			roomData = true;
		}

		//Make sure we have Room1
		if (!room->pRoom1)
			continue;

		//Reveal the room
		D2CLIENT_RevealAutomapRoom(room->pRoom1, TRUE, *p_D2CLIENT_AutomapLayer);

		//Reveal the presets
		RevealRoom(room);

		//Remove Data if Added
		if (roomData)
			D2COMMON_RemoveRoomData(level->pMisc->pAct, level->dwLevelNo, room->dwPosX, room->dwPosY, room->pRoom1);
	}

	revealedLevel[level->dwLevelNo] = true;
}

void Maphack::RevealRoom(Room2* room) {
	//Grabs all the preset units in room.
	for (PresetUnit* preset = room->pPreset; preset; preset = preset->pPresetNext)
	{
		int cellNo = -1;
		
		// Special NPC Check
		if (preset->dwType == 1)
		{
			// Izual Check
			if (preset->dwTxtFileNo == 256)
				cellNo = 300;
			// Hephasto Check
			if (preset->dwTxtFileNo == 745)
				cellNo = 745;
		// Special Object Check
		} else if (preset->dwType == 2) {
			// Uber Chest in Lower Kurast Check
			if (preset->dwTxtFileNo == 580 && room->pLevel->dwLevelNo == 79)		
				cellNo = 9;

			// Countess Chest Check
			if (preset->dwTxtFileNo == 371) 
				cellNo = 301;
			// Act 2 Orifice Check
			else if (preset->dwTxtFileNo == 152) 
				cellNo = 300;
			// Frozen Anya Check
			else if (preset->dwTxtFileNo == 460) 
				cellNo = 1468; 
			// Canyon / Arcane Waypoint Check
			if ((preset->dwTxtFileNo == 402) && (room->pLevel->dwLevelNo == 46))
				cellNo = 0;
			// Hell Forge Check
			if (preset->dwTxtFileNo == 376)
				cellNo = 376;

			// If it isn't special, check for a preset.
			if (cellNo == -1 && preset->dwTxtFileNo <= 572) {
				ObjectTxt *obj = D2COMMON_GetObjectTxt(preset->dwTxtFileNo);
				if (obj)
					cellNo = obj->nAutoMap;//Set the cell number then.
			}
		} else if (preset->dwType == 5) {
			LevelList* level = new LevelList;
			for (RoomTile* tile = room->pRoomTiles; tile; tile = tile->pNext) {
				if (*(tile->nNum) == preset->dwTxtFileNo) {
					level->levelId = tile->pRoom2->pLevel->dwLevelNo;
					break;
				}
			}
			level->x = (preset->dwPosX + (room->dwPosX * 5));
			level->y = (preset->dwPosY + (room->dwPosY * 5));
			level->act = room->pLevel->pMisc->pAct->dwAct;
			automapLevels.push_back(level);
		}

		//Draw the cell if wanted.
		if ((cellNo > 0) && (cellNo < 1258))
		{
			AutomapCell* cell = D2CLIENT_NewAutomapCell();

			cell->nCellNo = cellNo;
			int x = (preset->dwPosX + (room->dwPosX * 5));
			int y = (preset->dwPosY + (room->dwPosY * 5));
			cell->xPixel = (((x - y) * 16) / 10) + 1;
			cell->yPixel = (((y + x) * 8) / 10) - 3;

			D2CLIENT_AddAutomapCell(cell, &((*p_D2CLIENT_AutomapLayer)->pObjects));
		}

	}
	return;
}

AutomapLayer* Maphack::InitLayer(int level) {
	//Get the layer for the level.
	AutomapLayer2 *layer = D2COMMON_GetLayer(level);

	//Insure we have found the Layer.
	if (!layer)
		return false;

	//Initalize the layer!
	return (AutomapLayer*)D2CLIENT_InitAutomapLayer(layer->nLayerNo);
}

Level* Maphack::GetLevel(Act* pAct, int level)
{
	//Insure that the shit we are getting is good.
	if (level < 0 || !pAct)
		return NULL;

	//Loop all the levels in this act
	
	for(Level* pLevel = pAct->pMisc->pLevelFirst; pLevel; pLevel = pLevel->pNextLevel)
	{
		//Check if we have reached a bad level.
		if (!pLevel)
			break;

		//If we have found the level, return it!
		if (pLevel->dwLevelNo == level && pLevel->dwPosX > 0)
			return pLevel;
	}
	//Default old-way of finding level.
	return D2COMMON_GetLevel(pAct->pMisc, level);
}

void __declspec(naked) Weather_Interception()
{
	__asm {
		je rainold
		xor al,al
rainold:
		ret 0x04
	}
}

BOOL __fastcall InfravisionPatch(UnitAny *unit)
{
	return false;
}

void __declspec(naked) Lighting_Interception()
{
	__asm {
		je lightold
		mov eax,0xff
		mov byte ptr [esp+4+0], al
		mov byte ptr [esp+4+1], al
		mov byte ptr [esp+4+2], al
		add dword ptr [esp], 0x72;
		ret
		lightold:
		push esi
		call D2COMMON_GetLevelIdFromRoom_I;
		ret
	}
}

void __declspec(naked) Infravision_Interception()
{
	__asm {
		mov ecx, esi
		call InfravisionPatch
		add dword ptr [esp], 0x72
		ret
	}
}

VOID __stdcall Shake_Interception(LPDWORD lpX, LPDWORD lpY)
{

	*p_D2CLIENT_xShake = 0;
	*p_D2CLIENT_yShake = 0;

}