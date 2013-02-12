#pragma once
#include "../../Constants.h"
#include "../../D2Ptrs.h"
#include "../../Config.h"
#include "../../BH.h"

// Flags available for each entry in ItemLookup
#define ITEM_GROUP_HELM					0x00000001
#define ITEM_GROUP_ARMOR				0x00000002
#define ITEM_GROUP_SHIELD				0x00000004
#define ITEM_GROUP_GLOVES				0x00000008
#define ITEM_GROUP_BOOTS				0x00000010
#define ITEM_GROUP_BELT					0x00000020
#define ITEM_GROUP_DRUID_PELT			0x00000040
#define ITEM_GROUP_BARBARIAN_HELM		0x00000080
#define ITEM_GROUP_PALADIN_SHIELD		0x00000100
#define ITEM_GROUP_NECROMANCER_SHIELD	0x00000200
#define ITEM_GROUP_AXE					0x00000400
#define ITEM_GROUP_MACE					0x00000800
#define ITEM_GROUP_SWORD				0x00001000
#define ITEM_GROUP_DAGGER				0x00002000
#define ITEM_GROUP_THROWING				0x00004000
#define ITEM_GROUP_JAVELIN				0x00008000
#define ITEM_GROUP_SPEAR				0x00010000
#define ITEM_GROUP_POLEARM				0x00020000
#define ITEM_GROUP_BOW					0x00040000
#define ITEM_GROUP_CROSSBOW				0x00080000
#define ITEM_GROUP_STAFF				0x00100000
#define ITEM_GROUP_WAND					0x00200000
#define ITEM_GROUP_SCEPTER				0x00400000
#define ITEM_GROUP_ASSASSIN_KATAR		0x00800000
#define ITEM_GROUP_SORCERESS_ORB		0x01000000
#define ITEM_GROUP_AMAZON_WEAPON		0x02000000
#define ITEM_GROUP_NORMAL				0x04000000
#define ITEM_GROUP_EXCEPTIONAL			0x08000000
#define ITEM_GROUP_ELITE				0x10000000
#define ITEM_GROUP_UNUSED1				0x20000000
#define ITEM_GROUP_UNUSED2				0x40000000
#define ITEM_GROUP_COMMON				0x80000000

extern unsigned int ItemLookup[36][36][36];
extern unsigned int RuleCacheIndex;

unsigned int GetItemCodeIndex(char codeChar);

class Condition
{
public:
	Condition(bool n) : negate(n) {}
	virtual ~Condition() {}

	static const string tokenDelims;
	static void BuildConditions(vector<Condition*> &conditions, string token);

	bool Match(UnitAny *item, char *itemCode);
	virtual int InsertLookup() = 0;
protected:
	bool negate;
private:
	virtual bool MatchInternal(UnitAny *item, char *itemCode) { return false; }
};

class ItemCodeCondition : public Condition
{
public:
	ItemCodeCondition(bool negate, const char *code) : Condition(negate) {
		targetCode[0] = code[0];
		targetCode[1] = code[1];
		targetCode[2] = code[2];
		targetCode[3] = 0;
	};
	int InsertLookup() {
		unsigned int itemVal = ItemLookup[GetItemCodeIndex(targetCode[0])][GetItemCodeIndex(targetCode[1])][GetItemCodeIndex(targetCode[2])];
		if (itemVal == 0) {
			ItemLookup[GetItemCodeIndex(targetCode[0])][GetItemCodeIndex(targetCode[1])][GetItemCodeIndex(targetCode[2])] = RuleCacheIndex | ITEM_GROUP_COMMON;
			return RuleCacheIndex++;
		}
		return -1;
	}
private:
	char targetCode[4];
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class FlagsCondition : public Condition
{
public:
	FlagsCondition(bool negate, unsigned int flg) : Condition(negate), flag(flg) {};
	int InsertLookup() { return -1; }
private:
	unsigned int flag;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class QualityCondition : public Condition
{
public:
	QualityCondition(bool negate, unsigned int qual) : Condition(negate), quality(qual) {};
	int InsertLookup() { return -1; }
private:
	unsigned int quality;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class NonMagicalCondition : public Condition
{
public:
	NonMagicalCondition(bool negate) : Condition(negate) {};
	int InsertLookup() { return -1; }
private:
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class GemCondition : public Condition
{
public:
	GemCondition(bool negate, BYTE op, BYTE gem) : Condition(negate), gemNumber(gem), operation(op) {};
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE gemNumber;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class RuneCondition : public Condition
{
public:
	RuneCondition(bool negate, BYTE op, BYTE rune) : Condition(negate), runeNumber(rune), operation(op) {};
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE runeNumber;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class ItemLevelCondition : public Condition
{
public:
	ItemLevelCondition(bool negate, BYTE op, BYTE ilvl) : Condition(negate), itemLevel(ilvl), operation(op) {};
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE itemLevel;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class ItemGroupCondition : public Condition
{
public:
	ItemGroupCondition(bool negate, unsigned int group) : Condition(negate), itemGroup(group) {};
	int InsertLookup() { return -1; }
private:
	unsigned int itemGroup;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class EDCondition : public Condition
{
public:
	EDCondition(bool negate, BYTE op, unsigned int target) : Condition(negate), operation(op), targetED(target) {};
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	unsigned int targetED;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class ItemStatCondition : public Condition
{
public:
	ItemStatCondition(bool negate, unsigned int stat, unsigned int stat2, BYTE op, unsigned int target)
		: Condition(negate), itemStat(stat), itemStat2(stat2), operation(op), targetStat(target) {};
	int InsertLookup() { return -1; }
private:
	unsigned int itemStat;
	unsigned int itemStat2;
	BYTE operation;
	unsigned int targetStat;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

class ResistAllCondition : public Condition
{
public:
	ResistAllCondition(bool negate, BYTE op, unsigned int target)
		: Condition(negate), operation(op), targetStat(target) {};
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	unsigned int targetStat;
	bool MatchInternal(UnitAny *item, char *itemCode);
};

struct ActionReplace {
	string key;
	string value;
};

struct Action {
	bool stopProcessing;
	string name;
	string colorOnMap;
	Action() : colorOnMap(""), stopProcessing(true), name("") {}
};

struct Rule {
	vector<Condition*> conditions;
	Action action;
};

extern vector<Rule*> RuleList;
extern vector<Rule*> MapRuleList;
extern vector<Rule*> RuleCache;

void CreateItemTable();
void InitializeItemRules();
void BuildAction(string *str, Action *act);
BYTE GetOperation(string *op);
inline bool IntegerCompare(unsigned int Lvalue, int operation, unsigned int Rvalue);
void GetItemName(UnitAny *item, string &name, char *itemCode);
void SubstituteNameVariables(UnitAny *item, string &name, Action *action);
