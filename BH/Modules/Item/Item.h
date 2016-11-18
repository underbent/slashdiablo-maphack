#pragma once
#include "../Module.h"
#include "../../Constants.h"
#include "../../Config.h"
#include "../../Drawing.h"

struct UnitAny;

class Item : public Module {
	private:
		static map<std::string, Toggle> Toggles;
		unsigned int showPlayer;
		static UnitAny* viewingUnit;
		Drawing::UITab* settingsTab;
	public:

		Item() : Module("Item") {};

		void OnLoad();
		void OnUnload();

		void LoadConfig();
		void DrawSettings();

		void OnLoop();
		void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
		void OnLeftClick(bool up, int x, int y, bool* block);
		std::map<string, Toggle>* GetToggles() { return &Toggles; }

		static void __fastcall ItemNamePatch(wchar_t *name, UnitAny *item);
		static void OrigGetItemName(UnitAny *item, string &itemName, char *code);
		static UnitAny* GetViewUnit();
};

void ItemName_Interception();
void ViewInventoryPatch1_ASM();
void ViewInventoryPatch2_ASM();
void ViewInventoryPatch3_ASM();
