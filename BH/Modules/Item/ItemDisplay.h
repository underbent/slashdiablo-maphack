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

enum ConditionType {
	CT_None,
	CT_LeftParen,
	CT_RightParen,
	CT_NegationOperator,
	CT_BinaryOperator,
	CT_Operand
};

extern unsigned int ItemLookup[36][36][36];
extern unsigned int RuleCacheIndex;

unsigned int GetItemCodeIndex(char codeChar);

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

	bool Evaluate(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
	virtual int InsertLookup() = 0;

	BYTE conditionType;
private:
	virtual bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) { return false; }
};

class TrueCondition : public Condition
{
public:
	TrueCondition() { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class FalseCondition : public Condition
{
public:
	FalseCondition() { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class NegationOperator : public Condition
{
public:
	NegationOperator() { conditionType = CT_NegationOperator; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class LeftParen : public Condition
{
public:
	LeftParen() { conditionType = CT_LeftParen; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class RightParen : public Condition
{
public:
	RightParen() { conditionType = CT_RightParen; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class AndOperator : public Condition
{
public:
	AndOperator() { conditionType = CT_BinaryOperator; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class OrOperator : public Condition
{
public:
	OrOperator() { conditionType = CT_BinaryOperator; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
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
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class FlagsCondition : public Condition
{
public:
	FlagsCondition(unsigned int flg) : flag(flg) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	unsigned int flag;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class QualityCondition : public Condition
{
public:
	QualityCondition(unsigned int qual) : quality(qual) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	unsigned int quality;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class NonMagicalCondition : public Condition
{
public:
	NonMagicalCondition() { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class GemCondition : public Condition
{
public:
	GemCondition(BYTE op, BYTE gem) : gemNumber(gem), operation(op) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE gemNumber;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class RuneCondition : public Condition
{
public:
	RuneCondition(BYTE op, BYTE rune) : runeNumber(rune), operation(op) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE runeNumber;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class ItemLevelCondition : public Condition
{
public:
	ItemLevelCondition(BYTE op, BYTE ilvl) : itemLevel(ilvl), operation(op) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	BYTE itemLevel;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class ItemGroupCondition : public Condition
{
public:
	ItemGroupCondition(unsigned int group) : itemGroup(group) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	unsigned int itemGroup;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class EDCondition : public Condition
{
public:
	EDCondition(BYTE op, unsigned int target) : operation(op), targetED(target) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	unsigned int targetED;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class ItemStatCondition : public Condition
{
public:
	ItemStatCondition(unsigned int stat, unsigned int stat2, BYTE op, unsigned int target)
		: itemStat(stat), itemStat2(stat2), operation(op), targetStat(target) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	unsigned int itemStat;
	unsigned int itemStat2;
	BYTE operation;
	unsigned int targetStat;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
};

class ResistAllCondition : public Condition
{
public:
	ResistAllCondition(BYTE op, unsigned int target) : operation(op), targetStat(target) { conditionType = CT_Operand; };
	int InsertLookup() { return -1; }
private:
	BYTE operation;
	unsigned int targetStat;
	bool EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2);
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

	// Evaluate conditions which are in Reverse Polish Notation
	bool Evaluate(UnitAny *item, char *itemCode) {
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
				if (input->Evaluate(item, itemCode, arg1, arg2)) {
					conditionStack.push_back(new TrueCondition());
				} else {
					conditionStack.push_back(new FalseCondition());
				}
			}
		}
		if (conditionStack.size() == 1) {
			return conditionStack[0]->Evaluate(item, itemCode, NULL, NULL);
		} else {
			// TODO: find a way to report error
			return false;
		}
	}
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
