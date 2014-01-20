#include "Item.h"
#include "../../D2Ptrs.h"
#include "../../BH.h"
#include "../../D2Stubs.h"
#include "ItemDisplay.h"

map<std::string, Toggle> Item::Toggles;
UnitAny* Item::viewingUnit;

Patch* itemNamePatch = new Patch(Call, D2CLIENT, 0x92366, (int)ItemName_Interception, 6);
Patch* viewInvPatch1 = new Patch(Call, D2CLIENT, 0x953E2, (int)ViewInventoryPatch1_ASM, 6);
Patch* viewInvPatch2 = new Patch(Call, D2CLIENT, 0x94AB4, (int)ViewInventoryPatch2_ASM, 6);
Patch* viewInvPatch3 = new Patch(Call, D2CLIENT, 0x93A6F, (int)ViewInventoryPatch3_ASM, 5);

using namespace Drawing;

void Item::OnLoad() {
	Toggles["Show Ethereal"] = BH::config->ReadToggle("Show Ethereal", "None", true);
	Toggles["Show Sockets"] = BH::config->ReadToggle("Show Sockets", "None", true);
	Toggles["Show iLvl"] = BH::config->ReadToggle("Show iLvl", "None", true);
	Toggles["Show Rune Numbers"] = BH::config->ReadToggle("Show Rune Numbers", "None", true);
	Toggles["Alt Item Style"] = BH::config->ReadToggle("Alt Item Style", "None", true);
	Toggles["Color Mod"] = BH::config->ReadToggle("Color Mod", "None", false);
	Toggles["Shorten Item Names"] = BH::config->ReadToggle("Shorten Item Names", "None", false);
	Toggles["Advanced Item Display"] = BH::config->ReadToggle("Advanced Item Display", "None", false);

	CreateItemTable();
	if (Toggles["Advanced Item Display"].state) {
		InitializeItemRules();
	}

	showPlayer = BH::config->ReadKey("Show Players Gear", "VK_0");

	viewInvPatch1->Install();
	viewInvPatch2->Install();
	viewInvPatch3->Install();

	if (Toggles["Show Ethereal"].state || Toggles["Show Sockets"].state || Toggles["Show iLvl"].state || Toggles["Color Mod"].state ||
		Toggles["Show Rune Numbers"].state || Toggles["Alt Item Style"].state || Toggles["Shorten Item Names"].state || Toggles["Advanced Item Display"].state)
		itemNamePatch->Install();

	settingsTab = new UITab("Item", BH::settingsUI);

	new Checkhook(settingsTab, 4, 15, &Toggles["Show Ethereal"].state, "Show Ethereal");
	new Keyhook(settingsTab, 200, 17, &Toggles["Show Ethereal"].toggle, "");

	new Checkhook(settingsTab, 4, 30, &Toggles["Show Sockets"].state, "Show Sockets");
	new Keyhook(settingsTab, 200, 32, &Toggles["Show Sockets"].toggle, "");

	new Checkhook(settingsTab, 4, 45, &Toggles["Show iLvl"].state, "Show iLvl");
	new Keyhook(settingsTab, 200, 47, &Toggles["Show iLvl"].toggle, "");

	new Checkhook(settingsTab, 4, 60, &Toggles["Show Rune Numbers"].state, "Show Rune #");
	new Keyhook(settingsTab, 200, 62, &Toggles["Show Rune Numbers"].toggle, "");

	new Checkhook(settingsTab, 4, 75, &Toggles["Alt Item Style"].state, "Alt Style");
	new Keyhook(settingsTab, 200, 77, &Toggles["Alt Item Style"].toggle, "");

	new Checkhook(settingsTab, 4, 90, &Toggles["Color Mod"].state, "Color Mod");
	new Keyhook(settingsTab, 200, 92, &Toggles["Color Mod"].toggle, "");

	new Checkhook(settingsTab, 4, 105, &Toggles["Shorten Item Names"].state, "Shorten Names");
	new Keyhook(settingsTab, 200, 107, &Toggles["Shorten Item Names"].toggle, "");

	new Checkhook(settingsTab, 4, 120, &Toggles["Advanced Item Display"].state, "Advanced Item Display");
	new Keyhook(settingsTab, 200, 122, &Toggles["Advanced Item Display"].toggle, "");

	new Keyhook(settingsTab, 4, 137, &showPlayer, "Show Players Gear");
}

void Item::OnUnload() {
	itemNamePatch->Remove();
	viewInvPatch1->Remove();
	viewInvPatch2->Remove();
	viewInvPatch3->Remove();
}

void Item::OnLoop() {
	if (!D2CLIENT_GetUIState(0x01))
		viewingUnit = NULL;

	if (viewingUnit && viewingUnit->dwUnitId) {
		if (!viewingUnit->pInventory){
			viewingUnit = NULL;
			D2CLIENT_SetUIVar(0x01, 1, 0);			
		} else if (!D2CLIENT_FindServerSideUnit(viewingUnit->dwUnitId, viewingUnit->dwType)) {
			viewingUnit = NULL;
			D2CLIENT_SetUIVar(0x01, 1, 0);
		}
	}
}

void Item::OnKey(bool up, BYTE key, LPARAM lParam, bool* block) {
	if (key == showPlayer) {
		*block = true;
		if (up)
			return;
		UnitAny* selectedUnit = D2CLIENT_GetSelectedUnit();
		if (selectedUnit && selectedUnit->dwMode != 0 && selectedUnit->dwMode != 17 && selectedUnit->dwType == 0) {
			viewingUnit = selectedUnit;
			if (!D2CLIENT_GetUIState(0x01))
				D2CLIENT_SetUIVar(0x01, 0, 0);
			return;
		}
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

void Item::OnLeftClick(bool up, int x, int y, bool* block) {
	if (up)
		return;
	if (D2CLIENT_GetUIState(0x01) && viewingUnit != NULL && x >= 400)
		*block = true;
}

void __fastcall Item::ItemNamePatch(wchar_t *name, UnitAny *item)
{
	char* szName = UnicodeToAnsi(name);
	string itemName = szName;
	char* code = D2COMMON_GetItemText(item->dwTxtFileNo)->szCode;

	if (Toggles["Advanced Item Display"].state) {
		UnitItemInfo uInfo;
		uInfo.itemCode[0] = code[0]; uInfo.itemCode[1] = code[1]; uInfo.itemCode[2] = code[2]; uInfo.itemCode[3] = 0;
		uInfo.item = item;
		if (ItemAttributeMap.find(uInfo.itemCode) != ItemAttributeMap.end()) {
			uInfo.attrs = ItemAttributeMap[uInfo.itemCode];
			GetItemName(&uInfo, itemName);
		} else {
			PrintText(1, "Unknown item code name: %c%c%c\n", uInfo.itemCode[0], uInfo.itemCode[1], uInfo.itemCode[2]);
		}
	} else {
		OrigGetItemName(item, itemName, code);
	}

	// Some common color codes for text strings (see TextColor enum):
	// ÿc; (purple)
	// ÿc0 (white)
	// ÿc1 (red)
	// ÿc2 (green)
	// ÿc3 (blue)
	// ÿc4 (gold)
	// ÿc5 (gray)
	// ÿc6 (black)
	// ÿc7 (tan)
	// ÿc8 (orange)
	// ÿc9 (yellow)

	/* Test code to display item codes */
	//string test3 = test_code;
	//itemName += " {" + test3 + "}";

	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, itemName.c_str(), itemName.length(), name, itemName.length());
	name[itemName.length()] = 0;  // null-terminate the string since MultiByteToWideChar doesn't
	delete[] szName;
}

void Item::OrigGetItemName(UnitAny *item, string &itemName, char *code)
{
	bool displayItemLevel = Toggles["Show iLvl"].state;
	if (Toggles["Shorten Item Names"].state)
	{
		// We will also strip ilvls from these items
		if (code[0] == 't' && code[1] == 's' && code[2] == 'c')  // town portal scroll
		{
			itemName = "ÿc2**ÿc0TP";
			displayItemLevel = false;
		}
		else if (code[0] == 'i' && code[1] == 's' && code[2] == 'c')  // identify scroll
		{
			itemName = "ÿc2**ÿc0ID";
			displayItemLevel = false;
		}
		else if (code[0] == 'v' && code[1] == 'p' && code[2] == 's')  // stamina potion
		{
			itemName = "Stam";
			displayItemLevel = false;
		}
		else if (code[0] == 'y' && code[1] == 'p' && code[2] == 's')  // antidote potion
		{
			itemName = "Anti";
			displayItemLevel = false;
		}
		else if (code[0] == 'w' && code[1] == 'm' && code[2] == 's')  // thawing potion
		{
			itemName = "Thaw";
			displayItemLevel = false;
		}
		else if (code[0] == 'g' && code[1] == 'p' && code[2] == 's')  // rancid gas potion
		{
			itemName = "Ranc";
			displayItemLevel = false;
		}
		else if (code[0] == 'o' && code[1] == 'p' && code[2] == 's')  // oil potion
		{
			itemName = "Oil";
			displayItemLevel = false;
		}
		else if (code[0] == 'g' && code[1] == 'p' && code[2] == 'm')  // choking gas potion
		{
			itemName = "Chok";
			displayItemLevel = false;
		}
		else if (code[0] == 'o' && code[1] == 'p' && code[2] == 'm')  // exploding potion
		{
			itemName = "Expl";
			displayItemLevel = false;
		}
		else if (code[0] == 'g' && code[1] == 'p' && code[2] == 'l')  // strangling gas potion
		{
			itemName = "Stra";
			displayItemLevel = false;
		}
		else if (code[0] == 'o' && code[1] == 'p' && code[2] == 'l')  // fulminating potion
		{
			itemName = "Fulm";
			displayItemLevel = false;
		}
		else if (code[0] == 'h' && code[1] == 'p')  // healing potions
		{
			if (code[2] == '1')
			{
				itemName = "ÿc1**ÿc0Min Heal";
				displayItemLevel = false;
			}
			else if (code[2] == '2')
			{
				itemName = "ÿc1**ÿc0Lt Heal";
				displayItemLevel = false;
			}
			else if (code[2] == '3')
			{
				itemName = "ÿc1**ÿc0Heal";
				displayItemLevel = false;
			}
			else if (code[2] == '4')
			{
				itemName = "ÿc1**ÿc0Gt Heal";
				displayItemLevel = false;
			}
			else if (code[2] == '5')
			{
				itemName = "ÿc1**ÿc0Sup Heal";
				displayItemLevel = false;
			}
		}
		else if (code[0] == 'm' && code[1] == 'p')  // mana potions
		{
			if (code[2] == '1')
			{
				itemName = "ÿc3**ÿc0Min Mana";
				displayItemLevel = false;
			}
			else if (code[2] == '2')
			{
				itemName = "ÿc3**ÿc0Lt Mana";
				displayItemLevel = false;
			}
			else if (code[2] == '3')
			{
				itemName = "ÿc3**ÿc0Mana";
				displayItemLevel = false;
			}
			else if (code[2] == '4')
			{
				itemName = "ÿc3**ÿc0Gt Mana";
				displayItemLevel = false;
			}
			else if (code[2] == '5')
			{
				itemName = "ÿc3**ÿc0Sup Mana";
				displayItemLevel = false;
			}
		}
		else if (code[0] == 'r' && code[1] == 'v')  // rejuv potions
		{
			if (code[2] == 's')
			{
				itemName = "ÿc;**ÿc0Rejuv";
				displayItemLevel = false;
			}
			else if (code[2] == 'l')
			{
				itemName = "ÿc;**ÿc0Full";
				displayItemLevel = false;
			}
		}
		else if (code[1] == 'q' && code[2] == 'v')
		{
			if (code[0] == 'a')  // arrows
			{
				displayItemLevel = false;
			}
			else if (code[0] == 'c')  // bolts
			{
				displayItemLevel = false;
			}
		}
		else if (code[0] == 'k' && code[1] == 'e' && code[2] == 'y')  // key
		{
			displayItemLevel = false;
		}
	}

	/*Suffix Color Mod*/
	if( Toggles["Color Mod"].state )
	{
		/*Essences*/
		if( code[0] == 't' && code[1] == 'e' && code[2] == 's' )
		{
			itemName = itemName + " (Andariel/Duriel)";
		}
		if( code[0] == 'c' && code[1] == 'e' && code[2] == 'h' )
		{
			itemName = itemName + " (Mephtisto)";
		}
		if( code[0] == 'b' && code[1] == 'e' && code[2] == 't' )
		{
			itemName = itemName + " (Diablo)";
		}
		if( code[0] == 'f' && code[1] == 'e' && code[2] == 'd' )
		{
			itemName = itemName + " (Baal)";
		}
	}

	if( Toggles["Alt Item Style"].state )
	{
		if (Toggles["Show Rune Numbers"].state && D2COMMON_GetItemText(item->dwTxtFileNo)->nType == 74)
		{
			itemName = to_string(item->dwTxtFileNo - 609) + " - " + itemName;
		}
		else
		{
			if (Toggles["Show Sockets"].state)
			{
				int sockets = D2COMMON_GetUnitStat(item, STAT_SOCKETS, 0);
				if (sockets > 0)
				{
					itemName += "(" + to_string(sockets) + ")";
				}
			}

			if (Toggles["Show Ethereal"].state && item->pItemData->dwFlags & ITEM_ETHEREAL)
			{
				itemName = "Eth " + itemName;
			}
	
			/*show iLvl unless it is equal to 1*/
			if (displayItemLevel && item->pItemData->dwItemLevel != 1)
			{
				itemName += " L" + to_string(item->pItemData->dwItemLevel);
			}
		}
	}
	else
	{
		if (Toggles["Show Sockets"].state) {
			int sockets = D2COMMON_GetUnitStat(item, STAT_SOCKETS, 0);
			if (sockets > 0)
				itemName += "(" + to_string(sockets) + ")";
		}
		if (Toggles["Show Ethereal"].state && item->pItemData->dwFlags & ITEM_ETHEREAL)
			itemName += "(Eth)";

		if (displayItemLevel)
			itemName += "(L" + to_string(item->pItemData->dwItemLevel) + ")";

		if (Toggles["Show Rune Numbers"].state && D2COMMON_GetItemText(item->dwTxtFileNo)->nType == 74)
			itemName = "[" + to_string(item->dwTxtFileNo - 609) + "]" + itemName;
	}

	/*Affix (Colors) Color Mod*/
	if( Toggles["Color Mod"].state )
	{
		///*Flawless Gems*/
		//if( (code[0] == 'g' && code[1] == 'l'					) ||
		//	(code[0] == 's' && code[1] == 'k' && code[2] == 'l' ) )
		//{
		//	itemName = "ÿc:" + itemName;
		//}
		///*Perfect Gems*/
		//if( (code[0] == 'g' && code[1] == 'p'                   ) ||
		//	(code[0] == 's' && code[1] == 'k' && code[2] == 'p' ) )
		//{
		//	itemName = "ÿc<" + itemName;
		//}
		/*Ethereal*/
		if( item->pItemData->dwFlags & 0x400000 )
		{
			/*Turn ethereal elite armors (and paladin shields) purple*/
			if( (code[0] == 'u'                                    ) ||
				(code[0] == 'p' && code[1] == 'a' && code[2] >= 'b') )
			{
				itemName = "ÿc;" + itemName;
			}
		}
		/*Runes*/
		if( code[0] == 'r' )
		{
			if( code[1] == '0' )
			{
				itemName = "ÿc0" + itemName;
			}
			else if( code[1] == '1' )
			{
				if( code[2] <= '6')
				{
					itemName = "ÿc4" + itemName;
				}
				else
				{
					itemName = "ÿc8" + itemName;
				}
			}
			else if( code[1] == '2' )
			{
				if( code[2] <= '2' )
				{
					itemName = "ÿc8" + itemName;
				}
				else
				{
					itemName = "ÿc1" + itemName;
				}
			}
			else if( code[1] == '3' )
			{
				itemName = "ÿc1" + itemName;
			}
		}
	}
}

UnitAny* Item::GetViewUnit ()
{
	UnitAny* view = (viewingUnit) ? viewingUnit : D2CLIENT_GetPlayerUnit();
	if (view->dwUnitId == D2CLIENT_GetPlayerUnit()->dwUnitId)
		return D2CLIENT_GetPlayerUnit();

	Drawing::Texthook::Draw(560, 300, Drawing::Center, 0, White, "%s", viewingUnit->pPlayerData->szName);
	return viewingUnit;
}

void __declspec(naked) ItemName_Interception()
{
	__asm {
		mov ecx, edi
		mov edx, ebx
		call Item::ItemNamePatch
		mov al, [ebp+0x12a]
		ret
	}
}

void __declspec(naked) ViewInventoryPatch1_ASM()
{
	__asm {
		push eax;
		call Item::GetViewUnit;
		mov esi, eax;
		pop eax;
		ret;
	}
}
void __declspec(naked) ViewInventoryPatch2_ASM()
{
	__asm {
		push eax;
		call Item::GetViewUnit;
		mov ebx, eax;
		pop eax;
		ret;
	}
}
void __declspec(naked) ViewInventoryPatch3_ASM()
{
	__asm
	{
		push eax;
		push ebx;
		call Item::GetViewUnit;

		mov ebx, [edi];
		cmp ebx, 1;
		je OldCode;

		mov edi, eax;

		OldCode:
		pop ebx;
		pop eax;
		test eax, eax;
		mov ecx, dword ptr [edi + 0x60];

		ret;
	}
}