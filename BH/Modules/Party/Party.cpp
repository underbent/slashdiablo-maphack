#include "Party.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"
#include "../../D2Stubs.h"
#include "../../D2Helpers.h"

using namespace std;
using namespace Drawing;
Drawing::Hook* PartyHook;

void Party::OnLoad() {
	Toggles["Enabled"] = BH::config->ReadToggle("Party Enabled", "None", true);
	c = 0;

	PartyHook = new Drawing::Checkhook(Drawing::InGame, 100, 100, &Toggles["Enabled"].state, "Autoparty Enabled");
	PartyHook->SetActive(0);
}

void Party::OnUnload() {
	
}

void Party::OnLoop() {
	if(Toggles["Enabled"].state)
		CheckParty();
	if(D2CLIENT_GetUIState(0x16) && PartyHook->IsActive() == 0)
		PartyHook->SetActive(1);
	else if(D2CLIENT_GetUIState(0x16) == 0 && PartyHook->IsActive())
		PartyHook->SetActive(0);
}

void Party::CheckParty() {
	
	if(c == 0) {
		UnitAny* Me = *p_D2CLIENT_PlayerUnit;
		RosterUnit* MyRoster = FindPlayerRoster(Me->dwUnitId);

		for(RosterUnit* Party = (*p_D2CLIENT_PlayerUnitList);Party;Party = Party->pNext) {
			if(!_stricmp(Party->szName, MyRoster->szName))
				continue;
			if(!Party->wLevel || !Party) {
				c++;
				return;
			}
			if(Party->wPartyId == 0xFFFF || Party->wPartyId != MyRoster->wPartyId) {
				if(Party->dwPartyFlags & 0x2) {
					D2CLIENT_ClickParty(Party, 2);
					c++;
					return;
				}
				if(Party->wPartyId == 0xFFFF) {
					if(Party->dwPartyFlags & 0x4)
						continue;
					D2CLIENT_ClickParty(Party, 2);
					c++;
					return;
				}
			}
		}
	}
	c++;
	if(c>=25)
		c = 0;
}

void Party::OnKey(bool up, BYTE key, LPARAM lParam, bool* block)  {
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