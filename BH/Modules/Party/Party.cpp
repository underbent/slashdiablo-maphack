#include "Party.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"
#include "../../D2Stubs.h"
#include "../../D2Helpers.h"

using namespace std;
using namespace Drawing;
Drawing::Hook* PartyHook;
Drawing::Hook* LootHook;

void Party::OnLoad() {
	Toggles["Enabled"] = BH::config->ReadToggle("Party Enabled", "None", true);
	Toggles["LootEnabled"] = BH::config->ReadToggle("Looting Enabled", "None", true);
	c = 0;

	PartyHook = new Drawing::Checkhook(Drawing::InGame, 100, 100, &Toggles["Enabled"].state, "Autoparty Enabled");
	PartyHook->SetActive(0);
	LootHook = new Drawing::Checkhook(Drawing::InGame, 240, 100, &Toggles["LootEnabled"].state, "Autoloot Enabled");
	LootHook->SetActive(0);
}

void Party::OnUnload() {
	
}

void Party::OnLoop() {
	if(Toggles["Enabled"].state || Toggles["LootEnabled"].state)
		CheckParty();
	if(D2CLIENT_GetUIState(0x16) && PartyHook->IsActive() == 0) {
		PartyHook->SetBaseX(*p_D2CLIENT_PanelOffsetX + 20);
		PartyHook->SetActive(1);
	}
	else if(D2CLIENT_GetUIState(0x16) == 0 && PartyHook->IsActive())
		PartyHook->SetActive(0);

	BnetData* pData = (*p_D2LAUNCH_BnData);
	if(!pData)
		return;
	if(D2CLIENT_GetUIState(0x16) && LootHook->IsActive() == 0 && pData->nCharFlags & PLAYER_TYPE_HARDCORE) {
		LootHook->SetBaseX(*p_D2CLIENT_PanelOffsetX + 160);
		LootHook->SetActive(1);
	}
	else if(D2CLIENT_GetUIState(0x16) == 0 && LootHook->IsActive())
		LootHook->SetActive(0);
}

void Party::CheckParty() {
	if(c == 0) {
		bool valid = true;
		std::map<std::string, bool> CurrentParty;
		UnitAny* Me = *p_D2CLIENT_PlayerUnit;
		RosterUnit* MyRoster = FindPlayerRoster(Me->dwUnitId);
		BnetData* pData = (*p_D2LAUNCH_BnData);

		for(RosterUnit* Party = (*p_D2CLIENT_PlayerUnitList);Party;Party = Party->pNext) {
			if(!_stricmp(Party->szName, MyRoster->szName))
				continue;
			if(!Party->wLevel || !Party) {
				c++;
				return;
			}
			if ((Party->wPartyId == INVALID_PARTY_ID && Party->dwPartyFlags & PARTY_IN_PARTY) ||
				(Party->wPartyId != INVALID_PARTY_ID && Party->dwPartyFlags & PARTY_NOT_IN_PARTY)) {
				// Avoid crashing when multiple players in a game have auto-party enabled
				// (there seems to be a brief window in which the party data can be invalid)
				valid = false;
				continue;
			}
			if (pData && pData->nCharFlags & PLAYER_TYPE_HARDCORE) {
				CurrentParty[Party->szName] = true;
				if (Toggles["LootEnabled"].state) {
					string s(Party->szName);
					if (LootingPermission.find(s) == LootingPermission.end()) {
						BYTE PacketData[7] = {0x5d,1,1,0,0,0,0};
						*reinterpret_cast<int*>(PacketData + 3) = Party->dwUnitId;
						D2NET_SendPacket(7, 1, PacketData);
						LootingPermission[s] = true;
					}
				}
			}
			if ((Party->wPartyId == INVALID_PARTY_ID || Party->wPartyId != MyRoster->wPartyId) && Toggles["Enabled"].state) {
				if(Party->dwPartyFlags & PARTY_INVITED_YOU) {
					D2CLIENT_ClickParty(Party, 2);
					c++;
					return;
				}
				if(Party->wPartyId == INVALID_PARTY_ID) {
					if(Party->dwPartyFlags & PARTY_INVITED_BY_YOU)
						continue;
					D2CLIENT_ClickParty(Party, 2);
					c++;
					return;
				}
			}
		}

		if (valid) {
			for (auto it = LootingPermission.cbegin(); it != LootingPermission.cend(); ) {
				if (CurrentParty.find((*it).first) == CurrentParty.end()) {
					LootingPermission.erase(it++);
				} else {
					++it;
				}
			}
		}
	}
	c++;
	if(c>=25)
		c = 0;
}

void Party::OnKey(bool up, BYTE key, LPARAM lParam, bool* block)  {
	if(!D2CLIENT_GetPlayerUnit())
		return;

	if (D2CLIENT_GetUIState(UI_INVENTORY) || D2CLIENT_GetUIState(UI_STASH) || D2CLIENT_GetUIState(UI_CUBE)) {
		// Avoid toggling state when pressing number keys to drop/withdraw/deposit gold.
		// There is no UI state for the gold dialogs, so we have to disable whenever
		// stash/inventory are open.
		return;
	}

	for (map<string,Toggle>::iterator it = Toggles.begin(); it != Toggles.end(); it++) {
		if (key == (*it).second.toggle) {
			*block = true;
			if (up) {
				(*it).second.state = !(*it).second.state;
			}
			return;
		}
	}
}

void Party::OnGameExit() {
	LootingPermission.clear();
}
