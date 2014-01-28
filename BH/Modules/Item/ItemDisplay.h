#pragma once
#include "../../Constants.h"
#include "../../D2Ptrs.h"
#include "../../Config.h"
#include "../../BH.h"

#define EXCEPTION_INVALID_STAT			1
#define EXCEPTION_INVALID_OPERATION		2
#define EXCEPTION_INVALID_OPERATOR		3
#define EXCEPTION_INVALID_FLAG			4
#define EXCEPTION_INVALID_ITEM_TYPE		5
#define EXCEPTION_INVALID_GOLD_TYPE		6

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
#define ITEM_GROUP_ALLARMOR				0x20000000
#define ITEM_GROUP_ALLWEAPON			0x40000000
#define ITEM_GROUP_CIRCLET				0x80000000


// Item properties from incoming packets
struct ItemPropertyBits {
	string name;
	unsigned int saveBits;
	unsigned int saveParamBits;
	unsigned int saveAdd;
};

// Static item attributes
struct ItemAttributes {
	string name;
	char code[4];
	string category;
	BYTE width;
	BYTE height;
	BYTE stackable;
	BYTE useable;
	BYTE throwable;
	BYTE itemLevel;		// 1=normal, 2=exceptional, 3=elite
	BYTE unusedFlags;
	unsigned int flags;
	BYTE qualityLevel;
};

// Properties that can appear on an item from incoming packets
struct ItemProperty {
	unsigned int stat;
	long value;

	unsigned int minimum;
	unsigned int maximum;
	unsigned int length;

	unsigned int level;
	unsigned int characterClass;
	unsigned int skill;
	unsigned int tab;

	unsigned int monster;

	unsigned int charges;
	unsigned int maximumCharges;

	unsigned int skillChance;

	unsigned int perLevel;
};

// Collection of item data from the internal UnitAny structure
struct UnitItemInfo {
	UnitAny *item;
	char itemCode[4];
	ItemAttributes *attrs;
};

// Item data obtained from an incoming 0x9c packet
struct ItemInfo {
	ItemAttributes *attrs;
	char code[4];
	//std::string packet;
	std::string name;
	std::string earName;
	std::string personalizedName;
	unsigned int id;
	unsigned int x;
	unsigned int y;
	unsigned int amount;
	unsigned int prefix;
	unsigned int suffix;
	unsigned int setCode;
	unsigned int uniqueCode;
	unsigned int runewordId;
	unsigned int defense;
	unsigned int action;
	unsigned int category;
	unsigned int version;
	unsigned int directory;
	unsigned int container;
	unsigned int earLevel;
	unsigned int width;
	unsigned int height;
	unsigned int quality;
	unsigned int graphic;
	unsigned int color;
	unsigned int superiority;
	unsigned int runewordParameter;
	unsigned int maxDurability;
	unsigned int durability;
	BYTE usedSockets;
	BYTE level;
	BYTE earClass;
	BYTE sockets;
	bool equipped;
	bool inSocket;
	bool identified;
	bool switchedIn;
	bool switchedOut;
	bool broken;
	bool potion;
	bool hasSockets;
	bool inStore;
	bool notInSocket;
	bool ear;
	bool startItem;
	bool simpleItem;
	bool ethereal;
	bool personalized;
	bool gambling;
	bool runeword;
	bool ground;
	bool unspecifiedDirectory;
	bool isGold;
	bool hasGraphic;
	bool hasColor;
	bool isArmor;
	bool isWeapon;
	bool indestructible;
	std::vector<unsigned long> prefixes;
	std::vector<unsigned long> suffixes;
	std::vector<ItemProperty> properties;
	bool operator<(ItemInfo const & other) const;
};

ItemAttributes ItemAttributeList[];
ItemPropertyBits ItemPropertyBitsList[];
extern std::map<std::string, ItemAttributes*> ItemAttributeMap;

enum ConditionType {
	CT_None,
	CT_LeftParen,
	CT_RightParen,
	CT_NegationOperator,
	CT_BinaryOperator,
	CT_Operand
};

class Condition
{
public:
	Condition() {}
	virtual ~Condition() {}

	static const string tokenDelims;
	static void BuildConditions(vector<Condition*> &conditions, string token);
	static void ProcessConditions(vector<Condition*> &rawConditions, vector<Condition*> &processedConditions);
	static void AddOperand(vector<Condition*> &conditions, Condition *cond);
	static void AddNonOperand(vector<Condition*> &conditions, Condition *cond);

	bool Evaluate(UnitItemInfo *uInfo, ItemInfo *info, Condition *arg1, Condition *arg2);

	BYTE conditionType;
private:
	virtual bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) { return false; }
	virtual bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) { return false; }
};

class TrueCondition : public Condition
{
public:
	TrueCondition() { conditionType = CT_Operand; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class FalseCondition : public Condition
{
public:
	FalseCondition() { conditionType = CT_Operand; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class NegationOperator : public Condition
{
public:
	NegationOperator() { conditionType = CT_NegationOperator; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class LeftParen : public Condition
{
public:
	LeftParen() { conditionType = CT_LeftParen; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class RightParen : public Condition
{
public:
	RightParen() { conditionType = CT_RightParen; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class AndOperator : public Condition
{
public:
	AndOperator() { conditionType = CT_BinaryOperator; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class OrOperator : public Condition
{
public:
	OrOperator() { conditionType = CT_BinaryOperator; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class ItemCodeCondition : public Condition
{
public:
	ItemCodeCondition(const char *code) {
		targetCode[0] = code[0];
		targetCode[1] = code[1];
		targetCode[2] = code[2];
		targetCode[3] = 0;
		conditionType = CT_Operand;
	};
private:
	char targetCode[4];
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class FlagsCondition : public Condition
{
public:
	FlagsCondition(unsigned int flg) : flag(flg) { conditionType = CT_Operand; };
private:
	unsigned int flag;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class QualityCondition : public Condition
{
public:
	QualityCondition(unsigned int qual) : quality(qual) { conditionType = CT_Operand; };
private:
	unsigned int quality;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class NonMagicalCondition : public Condition
{
public:
	NonMagicalCondition() { conditionType = CT_Operand; };
private:
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class GemLevelCondition : public Condition
{
public:
	GemLevelCondition(BYTE op, BYTE gem) : gemLevel(gem), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	BYTE gemLevel;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class GemTypeCondition : public Condition
{
public:
	GemTypeCondition(BYTE op, BYTE gType) : gemType(gType), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	BYTE gemType;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class RuneCondition : public Condition
{
public:
	RuneCondition(BYTE op, BYTE rune) : runeNumber(rune), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	BYTE runeNumber;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class GoldCondition : public Condition
{
public:
	GoldCondition(BYTE op, unsigned int amt) : goldAmount(amt), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	unsigned int goldAmount;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class ItemLevelCondition : public Condition
{
public:
	ItemLevelCondition(BYTE op, BYTE ilvl) : itemLevel(ilvl), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	BYTE itemLevel;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class AffixLevelCondition : public Condition
{
public:
	AffixLevelCondition(BYTE op, BYTE alvl) : affixLevel(alvl), operation(op) { conditionType = CT_Operand; };
private:
	BYTE operation;
	BYTE affixLevel;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class ItemGroupCondition : public Condition
{
public:
	ItemGroupCondition(unsigned int group) : itemGroup(group) { conditionType = CT_Operand; };
private:
	unsigned int itemGroup;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class EDCondition : public Condition
{
public:
	EDCondition(BYTE op, unsigned int target) : operation(op), targetED(target) { conditionType = CT_Operand; };
private:
	BYTE operation;
	unsigned int targetED;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
	bool EvaluateED(unsigned int flags);
};

class ItemStatCondition : public Condition
{
public:
	ItemStatCondition(unsigned int stat, unsigned int stat2, BYTE op, unsigned int target)
		: itemStat(stat), itemStat2(stat2), operation(op), targetStat(target) { conditionType = CT_Operand; };
private:
	unsigned int itemStat;
	unsigned int itemStat2;
	BYTE operation;
	unsigned int targetStat;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

class ResistAllCondition : public Condition
{
public:
	ResistAllCondition(BYTE op, unsigned int target) : operation(op), targetStat(target) { conditionType = CT_Operand; };
private:
	BYTE operation;
	unsigned int targetStat;
	bool EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2);
	bool EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2);
};

extern TrueCondition *trueCondition;
extern FalseCondition *falseCondition;

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

	// Evaluate conditions which are in Reverse Polish Notation
	bool Evaluate(UnitItemInfo *uInfo, ItemInfo *info) {
		if (conditions.size() == 0) {
			return true;  // a rule with no conditions always matches
		}

		bool retval;
		try {
			vector<Condition*> conditionStack;
			for (unsigned int i = 0; i < conditions.size(); i++) {
				Condition *input = conditions[i];
				if (input->conditionType == CT_Operand) {
					conditionStack.push_back(input);
				} else if (input->conditionType == CT_BinaryOperator || input->conditionType == CT_NegationOperator) {
					Condition *arg1 = NULL, *arg2 = NULL;
					if (conditionStack.size() < 1) {
						return false;
					}
					arg1 = conditionStack.back();
					conditionStack.pop_back();
					if (input->conditionType == CT_BinaryOperator) {
						if (conditionStack.size() < 1) {
							return false;
						}
						arg2 = conditionStack.back();
						conditionStack.pop_back();
					}
					if (input->Evaluate(uInfo, info, arg1, arg2)) {
						conditionStack.push_back(trueCondition);
					} else {
						conditionStack.push_back(falseCondition);
					}
				}
			}
			if (conditionStack.size() == 1) {
				retval = conditionStack[0]->Evaluate(uInfo, info, NULL, NULL);
			} else {
				retval = false;
			}
		} catch (...) {
			retval = false;
		}
		return retval;
	}
};

extern vector<Rule*> RuleList;
extern vector<Rule*> MapRuleList;
extern vector<Rule*> IgnoreRuleList;

void CreateItemTable();
void InitializeItemRules();
void BuildAction(string *str, Action *act);
BYTE GetOperation(string *op);
inline bool IntegerCompare(unsigned int Lvalue, int operation, unsigned int Rvalue);
void GetItemName(UnitItemInfo *uInfo, string &name);
void SubstituteNameVariables(UnitItemInfo *uInfo, string &name, Action *action);
int GetDefense(ItemInfo *item);
BYTE GetAffixLevel(BYTE ilvl, BYTE qlvl, unsigned int flags, char *code);
