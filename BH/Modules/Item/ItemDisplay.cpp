#include "ItemDisplay.h"

// All colors here must also be defined in TextColorMap
#define COLOR_REPLACEMENTS	\
	{"WHITE", "ÿc0"},		\
	{"RED", "ÿc1"},			\
	{"GREEN", "ÿc2"},		\
	{"BLUE", "ÿc3"},		\
	{"GOLD", "ÿc4"},		\
	{"GRAY", "ÿc5"},		\
	{"BLACK", "ÿc6"},		\
	{"TAN", "ÿc7"},			\
	{"ORANGE", "ÿc8"},		\
	{"YELLOW", "ÿc9"},		\
	{"PURPLE", "ÿc;"}

enum Operation {
	EQUAL,
	GREATER_THAN,
	LESS_THAN,
	NONE
};

unsigned int RuleCacheIndex = 0;
unsigned int ItemLookup[36][36][36] = {};
vector<Rule*> RuleList;
vector<Rule*> MapRuleList;
vector<Rule*> RuleCache;
BYTE LastConditionType;

char* GemLevels[] = {
	"NONE",
	"Chipped",
	"Flawed",
	"Normal",
	"Flawless",
	"Perfect"
};

char* GemTypes[] = {
	"NONE",
	"Amethyst",
	"Diamond",
	"Emerald",
	"Ruby",
	"Sapphire",
	"Topaz",
	"Skull"
};

bool IsGem(BYTE nType) {
	return (nType >= 96 && nType <= 102);
}

BYTE GetGemLevel(char *itemCode) {
	BYTE c1 = itemCode[1];
	BYTE c2 = itemCode[2];
	BYTE gLevel = 1;
	if (c1 == 'f' || c2 == 'f') {
		gLevel = 2;
	} else if (c1 == 's' || c2 == 'u') {
		gLevel = 3;
	} else if (c1 == 'l' || c1 == 'z' || c2 == 'l') {
		gLevel = 4;
	} else if (c1 == 'p' || c2 == 'z') {
		gLevel = 5;
	}
	return gLevel;
}

char *GetGemLevelString(BYTE level) {
	return GemLevels[level];
}

BYTE GetGemType(BYTE nType) {
	return nType - 95;
}

char *GetGemTypeString(BYTE type) {
	return GemTypes[type];
}

bool IsRune(BYTE nType) {
	return (nType == 74);
}

void GetItemName(UnitAny *item, string &name, char *itemCode) {
	unsigned int itemVal = ItemLookup[GetItemCodeIndex(itemCode[0])][GetItemCodeIndex(itemCode[1])][GetItemCodeIndex(itemCode[2])];
	if (itemVal & ITEM_GROUP_COMMON) {
		unsigned int idx = itemVal - ITEM_GROUP_COMMON;
		SubstituteNameVariables(item, name, &RuleCache[idx]->action);
	} else {
		for (vector<Rule*>::iterator it = RuleList.begin(); it != RuleList.end(); it++) {
			if ((*it)->Evaluate(item, itemCode)) {
				SubstituteNameVariables(item, name, &(*it)->action);
				if ((*it)->action.stopProcessing) {
					break;
				}
			}
		}
	}
}

void SubstituteNameVariables(UnitAny *item, string &name, Action *action) {
	char origName[128], sockets[4], ilvl[4], runename[16] = "", runenum[4] = "0";
	char gemtype[16] = "", gemlevel[16] = "";
	sprintf_s(sockets, "%d", D2COMMON_GetUnitStat(item, STAT_SOCKETS, 0));
	sprintf_s(ilvl, "%d", item->pItemData->dwItemLevel);
	sprintf_s(origName, "%s", name.c_str());
	BYTE nType = D2COMMON_GetItemText(item->dwTxtFileNo)->nType;
	if (IsRune(nType)) {
		sprintf_s(runenum, "%d", item->dwTxtFileNo - 609);
		sprintf_s(runename, name.substr(0, name.find(' ')).c_str());
	} else if (IsGem(nType)) {
		char* code = D2COMMON_GetItemText(item->dwTxtFileNo)->szCode;
		sprintf_s(gemlevel, "%s", GetGemLevelString(GetGemLevel(code)));
		sprintf_s(gemtype, "%s", GetGemTypeString(GetGemType(nType)));
	}
	ActionReplace replacements[] = {
		{"NAME", origName},
		{"SOCKETS", sockets},
		{"RUNENUM", runenum},
		{"RUNENAME", runename},
		{"GEMLEVEL", gemlevel},
		{"GEMTYPE", gemtype},
		{"ILVL", ilvl},
		COLOR_REPLACEMENTS
	};
	name.assign(action->name);
	for (int n = 0; n < sizeof(replacements) / sizeof(replacements[0]); n++) {
		if (name.find("%" + replacements[n].key + "%") == string::npos)
			continue;
		if (replacements[n].value.length() == 0)
			name.assign("");
		else
			name.assign(name.replace(name.find("%" + replacements[n].key + "%"), replacements[n].key.length() + 2, replacements[n].value));
	}
}

BYTE GetOperation(string *op) {
	if (op->length() < 1) {
		return NONE;
	} else if ((*op)[0] == '=') {
		return EQUAL;
	} else if ((*op)[0] == '<') {
		return LESS_THAN;
	} else if ((*op)[0] == '>') {
		return GREATER_THAN;
	}
	return NONE;
}

unsigned int GetItemCodeIndex(char codeChar) {
	// Characters '0'-'9' map to 0-9, and a-z map to 10-35
	return codeChar - (codeChar < 90 ? 48 : 87);
}

bool IntegerCompare(unsigned int Lvalue, BYTE operation, unsigned int Rvalue) {
	switch (operation) {
	case EQUAL:
		return Lvalue == Rvalue;
	case GREATER_THAN:
		return Lvalue > Rvalue;
	case LESS_THAN:
		return Lvalue < Rvalue;
	default:
		return false;
	}
}

void InitializeItemRules() {
	bool foundFirstComplexRule = false;
	vector<pair<string, string>> rules = BH::config->ReadMapList("ItemDisplay");
	for (unsigned int i = 0; i < rules.size(); i++) {
		string buf;
		stringstream ss(rules[i].first);
		vector<string> tokens;
		while (ss >> buf) {
			tokens.push_back(buf);
		}

		LastConditionType = CT_None;
		Rule *r = new Rule();
		vector<Condition*> RawConditions;
		for (vector<string>::iterator tok = tokens.begin(); tok < tokens.end(); tok++) {
			Condition::BuildConditions(RawConditions, (*tok));
		}
		Condition::ProcessConditions(RawConditions, r->conditions);
		BuildAction(&(rules[i].second), &(r->action));

		if (r->conditions.size() == 1 && !foundFirstComplexRule) {
			int idx = r->conditions[0]->InsertLookup();
			if (idx >= 0) {
				RuleCache.push_back(r);
				continue;
			}
		}
		foundFirstComplexRule = true;

		RuleList.push_back(r);
		if (r->action.colorOnMap.length() > 0) {
			MapRuleList.push_back(r);
		}
	}
}

void BuildAction(string *str, Action *act) {
	act->name = string(str->c_str());

	size_t map = act->name.find("%MAP%");
	if (map != string::npos) {
		string mapColor = "ÿc0";
		size_t lastColorPos = 0;
		ActionReplace colors[] = {
			COLOR_REPLACEMENTS
		};
		for (int n = 0; n < sizeof(colors) / sizeof(colors[0]); n++) {
			size_t pos = act->name.find("%" + colors[n].key + "%");
			if (pos != string::npos && pos < map && pos >= lastColorPos) {
				mapColor = colors[n].value;
				lastColorPos = pos;
			}
		}

		act->name.replace(map, 5, "");
		act->colorOnMap = mapColor;
	}
	size_t done = act->name.find("%CONTINUE%");
	if (done != string::npos) {
		act->name.replace(done, 10, "");
		act->stopProcessing = false;
	}
}

const string Condition::tokenDelims = "<=>";

// Implements the shunting-yard algorithm to evaluate condition expressions
// Returns a vector of conditions in Reverse Polish Notation
void Condition::ProcessConditions(vector<Condition*> &inputConditions, vector<Condition*> &processedConditions) {
	vector<Condition*> conditionStack;
	unsigned int size = inputConditions.size();
	for (unsigned int c = 0; c < size; c++) {
		Condition *input = inputConditions[c];
		if (input->conditionType == CT_Operand) {
			processedConditions.push_back(input);
		} else if (input->conditionType == CT_BinaryOperator || input->conditionType == CT_NegationOperator) {
			bool go = true;
			while (go) {
				if (conditionStack.size() > 0) {
					Condition *stack = conditionStack.back();
					if ((stack->conditionType == CT_NegationOperator || stack->conditionType == CT_BinaryOperator) &&
						input->conditionType == CT_BinaryOperator) {
						conditionStack.pop_back();
						processedConditions.push_back(stack);
					} else {
						go = false;
					}
				} else {
					go = false;
				}
			}
			conditionStack.push_back(input);
		} else if (input->conditionType == CT_LeftParen) {
			conditionStack.push_back(input);
		} else if (input->conditionType == CT_RightParen) {
			bool foundLeftParen = false;
			while (conditionStack.size() > 0 && !foundLeftParen) {
				Condition *stack = conditionStack.back();
				conditionStack.pop_back();
				if (stack->conditionType == CT_LeftParen) {
					foundLeftParen = true;
					break;
				} else {
					processedConditions.push_back(stack);
				}
			}
			if (!foundLeftParen) {
				// TODO: find a way to report error
				return;
			}
		}
	}
	while (conditionStack.size() > 0) {
		Condition *next = conditionStack.back();
		conditionStack.pop_back();
		if (next->conditionType == CT_LeftParen || next->conditionType == CT_RightParen) {
			// TODO: find a way to report error
			break;
		} else {
			processedConditions.push_back(next);
		}
	}
}

void Condition::BuildConditions(vector<Condition*> &conditions, string token) {
	size_t delPos = token.find_first_of(tokenDelims);
	string key;
	string delim = "";
	int value = 0;
	if (delPos != string::npos) {
		key = Trim(token.substr(0, delPos));
		delim = token.substr(delPos, 1);
		string valueStr = Trim(token.substr(delPos + 1));
		if (valueStr.length() > 0) {
			stringstream ss(valueStr);
			if ((ss >> value).fail()) {
				return;  // TODO: returning errors
			}
		}
	} else {
		key = token;
	}
	BYTE operation = GetOperation(&delim);

	int i;
	for (i = 0; i < (int)key.length(); i++) {
		if (key[i] == '!') {
			Condition::AddNonOperand(conditions, new NegationOperator());
		} else if (key[i] == '(') {
			Condition::AddNonOperand(conditions, new LeftParen());
		} else if (key[i] == ')') {
			Condition::AddNonOperand(conditions, new RightParen());
		} else {
			break;
		}
	}
	key.erase(0, i);

	unsigned int keylen = key.length();
	if (key.compare(0, 3, "AND") == 0) {
		Condition::AddNonOperand(conditions, new AndOperator());
	} else if (key.compare(0, 2, "OR") == 0) {
		Condition::AddNonOperand(conditions, new OrOperator());
	} else if (keylen >= 3 && !(isupper(key[0]) || isupper(key[1]) || isupper(key[2]))) {
		Condition::AddOperand(conditions, new ItemCodeCondition(key.substr(0, 3).c_str()));
	} else if (key.compare(0, 3, "ETH") == 0) {
		Condition::AddOperand(conditions, new FlagsCondition(ITEM_ETHEREAL));
	} else if (key.compare(0, 4, "SOCK") == 0) {
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_SOCKETS, 0, operation, value));
	} else if (key.compare(0, 3, "SET") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_SET));
	} else if (key.compare(0, 3, "MAG") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_MAGIC));
	} else if (key.compare(0, 4, "RARE") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_RARE));
	} else if (key.compare(0, 3, "UNI") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_UNIQUE));
	} else if (key.compare(0, 4, "NMAG") == 0) {
		Condition::AddOperand(conditions, new NonMagicalCondition());
	} else if (key.compare(0, 3, "SUP") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_SUPERIOR));
	} else if (key.compare(0, 3, "INF") == 0) {
		Condition::AddOperand(conditions, new QualityCondition(ITEM_QUALITY_INFERIOR));
	} else if (key.compare(0, 4, "NORM") == 0) {
		Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_NORMAL));
	} else if (key.compare(0, 3, "EXC") == 0) {
		Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_EXCEPTIONAL));
	} else if (key.compare(0, 3, "ELT") == 0) {
		Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_ELITE));
	} else if (key.compare(0, 2, "ID") == 0) {
		Condition::AddOperand(conditions, new FlagsCondition(ITEM_IDENTIFIED));
	} else if (key.compare(0, 4, "ILVL") == 0) {
		Condition::AddOperand(conditions, new ItemLevelCondition(operation, value));
	} else if (key.compare(0, 4, "RUNE") == 0) {
		Condition::AddOperand(conditions, new RuneCondition(operation, value));
	} else if (key.compare(0, 7, "GEMTYPE") == 0) {
		Condition::AddOperand(conditions, new GemTypeCondition(operation, value));
	} else if (key.compare(0, 3, "GEM") == 0) {
		Condition::AddOperand(conditions, new GemLevelCondition(operation, value));
	} else if (key.compare(0, 2, "ED") == 0) {
		Condition::AddOperand(conditions, new EDCondition(operation, value));
	} else if (key.compare(0, 3, "DEF") == 0) {
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_DEFENSE, 0, operation, value));
	} else if (key.compare(0, 3, "RES") == 0) {
		Condition::AddOperand(conditions, new ResistAllCondition(operation, value));
	} else if (key.compare(0, 2, "EQ") == 0 && keylen >= 3) {
		if (key[2] == '1') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_HELM));
		} else if (key[2] == '2') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_ARMOR));
		} else if (key[2] == '3') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_SHIELD));
		} else if (key[2] == '4') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_GLOVES));
		} else if (key[2] == '5') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_BOOTS));
		} else if (key[2] == '6') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_BELT));
		}
	} else if (key.compare(0, 2, "CL") == 0 && keylen >= 3) {
		if (key[2] == '1') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_DRUID_PELT));
		} else if (key[2] == '2') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_BARBARIAN_HELM));
		} else if (key[2] == '3') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_PALADIN_SHIELD));
		} else if (key[2] == '4') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_NECROMANCER_SHIELD));
		} else if (key[2] == '5') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_ASSASSIN_KATAR));
		} else if (key[2] == '6') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_SORCERESS_ORB));
		} else if (key[2] == '7') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_AMAZON_WEAPON));
		}
	} else if (key.compare(0, 2, "WP") == 0) {
		if (keylen >= 3) {
			if (key[2] == '1') {
				if (keylen >= 4 && key[3] == '0') {
					Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_CROSSBOW));
				} else if (keylen >= 4 && key[3] == '1') {
					Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_STAFF));
				} else if (keylen >= 4 && key[3] == '2') {
					Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_WAND));
				} else if (keylen >= 4 && key[3] == '3') {
					Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_SCEPTER));
				} else {
					Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_AXE));
				}
			} else if (key[2] == '2') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_MACE));
			} else if (key[2] == '3') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_SWORD));
			} else if (key[2] == '4') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_DAGGER));
			} else if (key[2] == '5') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_THROWING));
			} else if (key[2] == '6') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_JAVELIN));
			} else if (key[2] == '7') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_SPEAR));
			} else if (key[2] == '8') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_POLEARM));
			} else if (key[2] == '9') {
				Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_BOW));
			}
		}
	} else if (key.compare(0, 2, "SK") == 0) {
		int num = -1;
		stringstream ss(key.substr(2));
		if ((ss >> num).fail() || num < 0 || num > STAT_MAX) {
			return;
		}
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_SINGLESKILL, num, operation, value));
	} else if (key.compare(0, 4, "CLSK") == 0) {
		int num = -1;
		stringstream ss(key.substr(4));
		if ((ss >> num).fail() || num < 0 || num >= CLASS_NA) {
			return;
		}
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_CLASSSKILLS, num, operation, value));
	} else if (key.compare(0, 4, "STAT") == 0) {
		int num = -1;
		stringstream ss(key.substr(4));
		if ((ss >> num).fail() || num < 0 || num > STAT_MAX) {
			return;
		}
		Condition::AddOperand(conditions, new ItemStatCondition(num, 0, operation, value));
	}

	for (i = token.length() - 1; i >= 0; i--) {
		if (token[i] == '!') {
			Condition::AddNonOperand(conditions, new NegationOperator());
		} else if (token[i] == '(') {
			Condition::AddNonOperand(conditions, new LeftParen());
		} else if (token[i] == ')') {
			Condition::AddNonOperand(conditions, new RightParen());
		} else {
			break;
		}
	}
}

// Insert extra AND operators to stay backwards compatible with old config
// that implicitly ANDed all conditions
void Condition::AddOperand(vector<Condition*> &conditions, Condition *cond) {
	if (LastConditionType == CT_Operand || LastConditionType == CT_RightParen) {
		conditions.push_back(new AndOperator());
	}
	conditions.push_back(cond);
	LastConditionType = CT_Operand;
}

void Condition::AddNonOperand(vector<Condition*> &conditions, Condition *cond) {
	if ((cond->conditionType == CT_NegationOperator || cond->conditionType == CT_LeftParen) &&
		(LastConditionType == CT_Operand || LastConditionType == CT_RightParen)) {
		conditions.push_back(new AndOperator());
	}
	conditions.push_back(cond);
	LastConditionType = cond->conditionType;
}

bool Condition::Evaluate(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return EvaluateInternal(item, itemCode, arg1, arg2);
}

bool TrueCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return true;
}

bool FalseCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return false;
}

bool NegationOperator::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return !arg1->Evaluate(item, itemCode, arg1, arg2);
}

bool LeftParen::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return false;
}

bool RightParen::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return false;
}

bool AndOperator::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(item, itemCode, NULL, NULL) && arg2->Evaluate(item, itemCode, NULL, NULL);
}

bool OrOperator::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(item, itemCode, NULL, NULL) || arg2->Evaluate(item, itemCode, NULL, NULL);
}

bool ItemCodeCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return (targetCode[0] == itemCode[0] && targetCode[1] == itemCode[1] && targetCode[2] == itemCode[2]);
}

bool FlagsCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return ((item->pItemData->dwFlags & flag) > 0);
}

bool QualityCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return (item->pItemData->dwQuality == quality);
}

bool NonMagicalCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return (item->pItemData->dwQuality == ITEM_QUALITY_INFERIOR ||
			item->pItemData->dwQuality == ITEM_QUALITY_NORMAL ||
			item->pItemData->dwQuality == ITEM_QUALITY_SUPERIOR);
}

bool GemLevelCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	BYTE nType = D2COMMON_GetItemText(item->dwTxtFileNo)->nType;
	if (IsGem(nType)) {
		return IntegerCompare(GetGemLevel(itemCode), operation, gemLevel);
	}
	return false;
}

bool GemTypeCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	BYTE nType = D2COMMON_GetItemText(item->dwTxtFileNo)->nType;
	if (IsGem(nType)) {
		return IntegerCompare(GetGemType(nType), operation, gemType);
	}
	return false;
}

bool RuneCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	if (IsRune(D2COMMON_GetItemText(item->dwTxtFileNo)->nType)) {
		return IntegerCompare(item->dwTxtFileNo - 609, operation, runeNumber);
	}
	return false;
}

bool ItemLevelCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return IntegerCompare(item->pItemData->dwItemLevel, operation, itemLevel);
}

bool ItemGroupCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	unsigned int itemVal = ItemLookup[GetItemCodeIndex(itemCode[0])][GetItemCodeIndex(itemCode[1])][GetItemCodeIndex(itemCode[2])];
	return ((itemVal & itemGroup) > 0);
}

bool EDCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	// Either enhanced defense or enhanced damage depending on item type
	unsigned int itemVal = ItemLookup[GetItemCodeIndex(itemCode[0])][GetItemCodeIndex(itemCode[1])][GetItemCodeIndex(itemCode[2])];
	unsigned int armorMask =
		ITEM_GROUP_HELM |
		ITEM_GROUP_ARMOR |
		ITEM_GROUP_SHIELD |
		ITEM_GROUP_GLOVES |
		ITEM_GROUP_BOOTS |
		ITEM_GROUP_BELT |
		ITEM_GROUP_DRUID_PELT |
		ITEM_GROUP_BARBARIAN_HELM |
		ITEM_GROUP_PALADIN_SHIELD |
		ITEM_GROUP_NECROMANCER_SHIELD;

	WORD stat;
	if (itemVal & armorMask) {
		stat = STAT_ENHANCEDDEFENSE;
	} else {
		// Normal %ED will have the same value for STAT_ENHANCEDMAXIMUMDAMAGE and STAT_ENHANCEDMINIMUMDAMAGE
		stat = STAT_ENHANCEDMAXIMUMDAMAGE;
	}

	// Pulled from JSUnit.cpp in d2bs
	Stat aStatList[256] = { NULL };
	StatList* pStatList = D2COMMON_GetStatList(item, NULL, 0x40);
	if (pStatList) {
		DWORD dwStats = D2COMMON_CopyStatList(pStatList, (Stat*)aStatList, 256);
		for (UINT i = 0; i < dwStats; i++) {
			if (aStatList[i].wStatIndex == stat && aStatList[i].wSubIndex == 0) {
				return IntegerCompare(aStatList[i].dwStatValue, operation, targetED);
			}
		}
	}
	return false;
}

bool ItemStatCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	return IntegerCompare(D2COMMON_GetUnitStat(item, itemStat, itemStat2), operation, targetStat);
}

bool ResistAllCondition::EvaluateInternal(UnitAny *item, char *itemCode, Condition *arg1, Condition *arg2) {
	int fRes = D2COMMON_GetUnitStat(item, STAT_FIRERESIST, 0);
	int lRes = D2COMMON_GetUnitStat(item, STAT_LIGHTNINGRESIST, 0);
	int cRes = D2COMMON_GetUnitStat(item, STAT_COLDRESIST, 0);
	int pRes = D2COMMON_GetUnitStat(item, STAT_POISONRESIST, 0);
	return (IntegerCompare(fRes, operation, targetStat) &&
			IntegerCompare(lRes, operation, targetStat) &&
			IntegerCompare(cRes, operation, targetStat) &&
			IntegerCompare(pRes, operation, targetStat));
}

void CreateItemTable() {
	// This lookup table was created for all normal/exceptional/elite items
	// using the data in http://bhfiles.com/files/Diablo%20II/ItemCodes.html.
	// It is rather memory-inefficient, as only about 500 of the 36*36*36
	// entries are used -- we could definitely be smarter about encoding the
	// indices to minimize the dead space.

	// We also overload this lookup table for certain common items that don't
	// overlap with armor/weapons, such as potions and scrolls.  For these
	// items, the ITEM_GROUP_COMMON bit will be set to distinguish them.

	// Normal items
	ItemLookup[12][10][25] = 67108865;  //cap
	ItemLookup[28][20][25] = 67108865;  //skp
	ItemLookup[17][21][22] = 67108865;  //hlm
	ItemLookup[15][17][21] = 67108865;  //fhl
	ItemLookup[16][17][22] = 67108865;  //ghm
	ItemLookup[12][27][23] = 67108865;  //crn
	ItemLookup[22][28][20] = 67108865;  //msk
	ItemLookup[11][17][22] = 67108865;  //bhm
	ItemLookup[26][30][18] = 67108866;  //qui
	ItemLookup[21][14][10] = 67108866;  //lea
	ItemLookup[17][21][10] = 67108866;  //hla
	ItemLookup[28][29][30] = 67108866;  //stu
	ItemLookup[27][23][16] = 67108866;  //rng
	ItemLookup[28][12][21] = 67108866;  //scl
	ItemLookup[12][17][23] = 67108866;  //chn
	ItemLookup[11][27][28] = 67108866;  //brs
	ItemLookup[28][25][21] = 67108866;  //spl
	ItemLookup[25][21][29] = 67108866;  //plt
	ItemLookup[15][21][13] = 67108866;  //fld
	ItemLookup[16][29][17] = 67108866;  //gth
	ItemLookup[15][30][21] = 67108866;  //ful
	ItemLookup[10][10][27] = 67108866;  //aar
	ItemLookup[21][29][25] = 67108866;  //ltp
	ItemLookup[11][30][12] = 67108868;  //buc
	ItemLookup[28][22][21] = 67108868;  //sml
	ItemLookup[21][27][16] = 67108868;  //lrg
	ItemLookup[20][18][29] = 67108868;  //kit
	ItemLookup[29][24][32] = 67108868;  //tow
	ItemLookup[16][29][28] = 67108868;  //gts
	ItemLookup[11][28][17] = 67108868;  //bsh
	ItemLookup[28][25][20] = 67108868;  //spk
	ItemLookup[21][16][21] = 67108872;  //lgl
	ItemLookup[31][16][21] = 67108872;  //vgl
	ItemLookup[22][16][21] = 67108872;  //mgl
	ItemLookup[29][16][21] = 67108872;  //tgl
	ItemLookup[17][16][21] = 67108872;  //hgl
	ItemLookup[21][11][29] = 67108880;  //lbt
	ItemLookup[31][11][29] = 67108880;  //vbt
	ItemLookup[22][11][29] = 67108880;  //mbt
	ItemLookup[29][11][29] = 67108880;  //tbt
	ItemLookup[17][11][29] = 67108880;  //hbt
	ItemLookup[21][11][21] = 67108896;  //lbl
	ItemLookup[31][11][21] = 67108896;  //vbl
	ItemLookup[22][11][21] = 67108896;  //mbl
	ItemLookup[29][11][21] = 67108896;  //tbl
	ItemLookup[17][11][21] = 67108896;  //hbl
	ItemLookup[13][27][1] = 67108928;  //dr1
	ItemLookup[13][27][2] = 67108928;  //dr2
	ItemLookup[13][27][3] = 67108928;  //dr3
	ItemLookup[13][27][4] = 67108928;  //dr4
	ItemLookup[13][27][5] = 67108928;  //dr5
	ItemLookup[11][10][1] = 67108992;  //ba1
	ItemLookup[11][10][2] = 67108992;  //ba2
	ItemLookup[11][10][3] = 67108992;  //ba3
	ItemLookup[11][10][4] = 67108992;  //ba4
	ItemLookup[11][10][5] = 67108992;  //ba5
	ItemLookup[25][10][1] = 67109120;  //pa1
	ItemLookup[25][10][2] = 67109120;  //pa2
	ItemLookup[25][10][3] = 67109120;  //pa3
	ItemLookup[25][10][4] = 67109120;  //pa4
	ItemLookup[25][10][5] = 67109120;  //pa5
	ItemLookup[23][14][1] = 67109376;  //ne1
	ItemLookup[23][14][2] = 67109376;  //ne2
	ItemLookup[23][14][3] = 67109376;  //ne3
	ItemLookup[23][14][4] = 67109376;  //ne4
	ItemLookup[23][14][5] = 67109376;  //ne5
	ItemLookup[17][10][33] = 67109888;  //hax
	ItemLookup[10][33][14] = 67109888;  //axe
	ItemLookup[2][10][33] = 67109888;  //2ax
	ItemLookup[22][25][18] = 67109888;  //mpi
	ItemLookup[32][10][33] = 67109888;  //wax
	ItemLookup[21][10][33] = 67109888;  //lax
	ItemLookup[11][10][33] = 67109888;  //bax
	ItemLookup[11][29][33] = 67109888;  //btx
	ItemLookup[16][10][33] = 67109888;  //gax
	ItemLookup[16][18][33] = 67109888;  //gix
	ItemLookup[12][21][11] = 67110912;  //clb
	ItemLookup[28][25][12] = 67110912;  //spc
	ItemLookup[22][10][12] = 67110912;  //mac
	ItemLookup[22][28][29] = 67110912;  //mst
	ItemLookup[15][21][10] = 67110912;  //fla
	ItemLookup[32][17][22] = 67110912;  //whm
	ItemLookup[22][10][30] = 67110912;  //mau
	ItemLookup[16][22][10] = 67110912;  //gma
	ItemLookup[28][28][13] = 67112960;  //ssd
	ItemLookup[28][12][22] = 67112960;  //scm
	ItemLookup[28][11][27] = 67112960;  //sbr
	ItemLookup[15][21][12] = 67112960;  //flc
	ItemLookup[12][27][28] = 67112960;  //crs
	ItemLookup[11][28][13] = 67112960;  //bsd
	ItemLookup[21][28][13] = 67112960;  //lsd
	ItemLookup[32][28][13] = 67112960;  //wsd
	ItemLookup[2][17][28] = 67112960;  //2hs
	ItemLookup[12][21][22] = 67112960;  //clm
	ItemLookup[16][18][28] = 67112960;  //gis
	ItemLookup[11][28][32] = 67112960;  //bsw
	ItemLookup[15][21][11] = 67112960;  //flb
	ItemLookup[16][28][13] = 67112960;  //gsd
	ItemLookup[13][16][27] = 67117056;  //dgr
	ItemLookup[13][18][27] = 67117056;  //dir
	ItemLookup[20][27][18] = 67117056;  //kri
	ItemLookup[11][21][13] = 67117056;  //bld
	ItemLookup[29][20][15] = 67125248;  //tkf
	ItemLookup[29][10][33] = 67125248;  //tax
	ItemLookup[11][20][15] = 67125248;  //bkf
	ItemLookup[11][10][21] = 67125248;  //bal
	ItemLookup[19][10][31] = 67141632;  //jav
	ItemLookup[25][18][21] = 67141632;  //pil
	ItemLookup[28][28][25] = 67141632;  //ssp
	ItemLookup[16][21][31] = 67141632;  //glv
	ItemLookup[29][28][25] = 67141632;  //tsp
	ItemLookup[28][25][27] = 67174400;  //spr
	ItemLookup[29][27][18] = 67174400;  //tri
	ItemLookup[11][27][23] = 67174400;  //brn
	ItemLookup[28][25][29] = 67174400;  //spt
	ItemLookup[25][18][20] = 67174400;  //pik
	ItemLookup[11][10][27] = 67239936;  //bar
	ItemLookup[31][24][30] = 67239936;  //vou
	ItemLookup[28][12][34] = 67239936;  //scy
	ItemLookup[25][10][33] = 67239936;  //pax
	ItemLookup[17][10][21] = 67239936;  //hal
	ItemLookup[32][28][12] = 67239936;  //wsc
	ItemLookup[28][11][32] = 67371008;  //sbw
	ItemLookup[17][11][32] = 67371008;  //hbw
	ItemLookup[21][11][32] = 67371008;  //lbw
	ItemLookup[12][11][32] = 67371008;  //cbw
	ItemLookup[28][11][11] = 67371008;  //sbb
	ItemLookup[21][11][11] = 67371008;  //lbb
	ItemLookup[28][32][11] = 67371008;  //swb
	ItemLookup[21][32][11] = 67371008;  //lwb
	ItemLookup[21][33][11] = 67633152;  //lxb
	ItemLookup[22][33][11] = 67633152;  //mxb
	ItemLookup[17][33][11] = 67633152;  //hxb
	ItemLookup[27][33][11] = 67633152;  //rxb
	ItemLookup[28][28][29] = 68157440;  //sst
	ItemLookup[21][28][29] = 68157440;  //lst
	ItemLookup[16][28][29] = 68157440;  //gst
	ItemLookup[12][28][29] = 68157440;  //cst (gnarled staff is actually cst not gst in my tests)
	ItemLookup[11][28][29] = 68157440;  //bst
	ItemLookup[32][28][29] = 68157440;  //wst
	ItemLookup[32][23][13] = 69206016;  //wnd
	ItemLookup[34][32][23] = 69206016;  //ywn
	ItemLookup[11][32][23] = 69206016;  //bwn
	ItemLookup[16][32][23] = 69206016;  //gwn
	ItemLookup[28][12][25] = 71303168;  //scp
	ItemLookup[16][28][12] = 71303168;  //gsc
	ItemLookup[32][28][25] = 71303168;  //wsp
	ItemLookup[20][29][27] = 75497472;  //ktr
	ItemLookup[32][27][11] = 75497472;  //wrb
	ItemLookup[10][33][15] = 75497472;  //axf
	ItemLookup[12][14][28] = 75497472;  //ces
	ItemLookup[12][21][32] = 75497472;  //clw
	ItemLookup[11][29][21] = 75497472;  //btl
	ItemLookup[28][20][27] = 75497472;  //skr
	ItemLookup[24][11][1] = 83886080;  //ob1
	ItemLookup[24][11][2] = 83886080;  //ob2
	ItemLookup[24][11][3] = 83886080;  //ob3
	ItemLookup[24][11][4] = 83886080;  //ob4
	ItemLookup[24][11][5] = 83886080;  //ob5
	ItemLookup[10][22][1] = 100663296;  //am1
	ItemLookup[10][22][2] = 100663296;  //am2
	ItemLookup[10][22][3] = 100663296;  //am3
	ItemLookup[10][22][4] = 100663296;  //am4
	ItemLookup[10][22][5] = 100663296;  //am5

	// Exceptional items
	ItemLookup[33][10][25] = 134217729;  //xap
	ItemLookup[33][20][25] = 134217729;  //xkp
	ItemLookup[33][21][22] = 134217729;  //xlm
	ItemLookup[33][17][21] = 134217729;  //xhl
	ItemLookup[33][17][22] = 134217729;  //xhm
	ItemLookup[33][27][23] = 134217729;  //xrn
	ItemLookup[33][28][20] = 134217729;  //xsk
	ItemLookup[33][17][9] = 134217729;  //xh9
	ItemLookup[33][30][18] = 134217730;  //xui
	ItemLookup[33][14][10] = 134217730;  //xea
	ItemLookup[33][21][10] = 134217730;  //xla
	ItemLookup[33][29][30] = 134217730;  //xtu
	ItemLookup[33][23][16] = 134217730;  //xng
	ItemLookup[33][12][21] = 134217730;  //xcl
	ItemLookup[33][17][23] = 134217730;  //xhn
	ItemLookup[33][27][28] = 134217730;  //xrs
	ItemLookup[33][25][21] = 134217730;  //xpl
	ItemLookup[33][21][29] = 134217730;  //xlt
	ItemLookup[33][21][13] = 134217730;  //xld
	ItemLookup[33][29][17] = 134217730;  //xth
	ItemLookup[33][30][21] = 134217730;  //xul
	ItemLookup[33][10][27] = 134217730;  //xar
	ItemLookup[33][29][25] = 134217730;  //xtp
	ItemLookup[33][30][12] = 134217732;  //xuc
	ItemLookup[33][22][21] = 134217732;  //xml
	ItemLookup[33][27][16] = 134217732;  //xrg
	ItemLookup[33][18][29] = 134217732;  //xit
	ItemLookup[33][24][32] = 134217732;  //xow
	ItemLookup[33][29][28] = 134217732;  //xts
	ItemLookup[33][28][17] = 134217732;  //xsh
	ItemLookup[33][25][20] = 134217732;  //xpk
	ItemLookup[33][21][16] = 134217736;  //xlg
	ItemLookup[33][31][16] = 134217736;  //xvg
	ItemLookup[33][22][16] = 134217736;  //xmg
	ItemLookup[33][29][16] = 134217736;  //xtg
	ItemLookup[33][17][16] = 134217736;  //xhg
	ItemLookup[33][21][11] = 134217744;  //xlb
	ItemLookup[33][31][11] = 134217744;  //xvb
	ItemLookup[33][22][11] = 134217744;  //xmb
	ItemLookup[33][29][11] = 134217744;  //xtb
	ItemLookup[33][17][11] = 134217744;  //xhb
	ItemLookup[35][21][11] = 134217760;  //zlb
	ItemLookup[35][31][11] = 134217760;  //zvb
	ItemLookup[35][22][11] = 134217760;  //zmb
	ItemLookup[35][29][11] = 134217760;  //ztb
	ItemLookup[35][17][11] = 134217760;  //zhb
	ItemLookup[13][27][6] = 134217792;  //dr6
	ItemLookup[13][27][7] = 134217792;  //dr7
	ItemLookup[13][27][8] = 134217792;  //dr8
	ItemLookup[13][27][9] = 134217792;  //dr9
	ItemLookup[13][27][10] = 134217792;  //dra
	ItemLookup[11][10][6] = 134217856;  //ba6
	ItemLookup[11][10][7] = 134217856;  //ba7
	ItemLookup[11][10][8] = 134217856;  //ba8
	ItemLookup[11][10][9] = 134217856;  //ba9
	ItemLookup[11][10][10] = 134217856;  //baa
	ItemLookup[25][10][6] = 134217984;  //pa6
	ItemLookup[25][10][7] = 134217984;  //pa7
	ItemLookup[25][10][8] = 134217984;  //pa8
	ItemLookup[25][10][9] = 134217984;  //pa9
	ItemLookup[25][10][10] = 134217984;  //paa
	ItemLookup[23][14][6] = 134218240;  //ne6
	ItemLookup[23][14][7] = 134218240;  //ne7
	ItemLookup[23][14][8] = 134218240;  //ne8
	ItemLookup[23][14][9] = 134218240;  //ne9
	ItemLookup[23][14][10] = 134218240;  //nea
	ItemLookup[9][17][10] = 134218752;  //9ha
	ItemLookup[9][10][33] = 134218752;  //9ax
	ItemLookup[9][2][10] = 134218752;  //92a
	ItemLookup[9][22][25] = 134218752;  //9mp
	ItemLookup[9][32][10] = 134218752;  //9wa
	ItemLookup[9][21][10] = 134218752;  //9la
	ItemLookup[9][11][10] = 134218752;  //9ba
	ItemLookup[9][11][29] = 134218752;  //9bt
	ItemLookup[9][16][10] = 134218752;  //9ga
	ItemLookup[9][16][18] = 134218752;  //9gi
	ItemLookup[9][12][21] = 134219776;  //9cl
	ItemLookup[9][28][25] = 134219776;  //9sp
	ItemLookup[9][22][10] = 134219776;  //9ma
	ItemLookup[9][22][29] = 134219776;  //9mt
	ItemLookup[9][15][21] = 134219776;  //9fl
	ItemLookup[9][32][17] = 134219776;  //9wh
	ItemLookup[9][22][9] = 134219776;  //9m9
	ItemLookup[9][16][22] = 134219776;  //9gm
	ItemLookup[9][28][28] = 134221824;  //9ss
	ItemLookup[9][28][22] = 134221824;  //9sm
	ItemLookup[9][28][11] = 134221824;  //9sb
	ItemLookup[9][15][12] = 134221824;  //9fc
	ItemLookup[9][12][27] = 134221824;  //9cr
	ItemLookup[9][11][28] = 134221824;  //9bs
	ItemLookup[9][21][28] = 134221824;  //9ls
	ItemLookup[9][32][13] = 134221824;  //9wd
	ItemLookup[9][2][17] = 134221824;  //92h
	ItemLookup[9][12][22] = 134221824;  //9cm
	ItemLookup[9][16][28] = 134221824;  //9gs
	ItemLookup[9][11][9] = 134221824;  //9b9
	ItemLookup[9][15][11] = 134221824;  //9fb
	ItemLookup[9][16][13] = 134221824;  //9gd
	ItemLookup[9][13][16] = 134225920;  //9dg
	ItemLookup[9][13][18] = 134225920;  //9di
	ItemLookup[9][20][27] = 134225920;  //9kr
	ItemLookup[9][11][21] = 134225920;  //9bl
	ItemLookup[9][29][20] = 134234112;  //9tk
	ItemLookup[9][29][10] = 134234112;  //9ta
	ItemLookup[9][11][20] = 134234112;  //9bk
	ItemLookup[9][11][8] = 134234112;  //9b8
	ItemLookup[9][19][10] = 134250496;  //9ja
	ItemLookup[9][25][18] = 134250496;  //9pi
	ItemLookup[9][28][9] = 134250496;  //9s9
	ItemLookup[9][16][21] = 134250496;  //9gl
	ItemLookup[9][29][28] = 134250496;  //9ts
	ItemLookup[9][28][27] = 134283264;  //9sr
	ItemLookup[9][29][27] = 134283264;  //9tr
	ItemLookup[9][11][27] = 134283264;  //9br
	ItemLookup[9][28][29] = 134283264;  //9st
	ItemLookup[9][25][9] = 134283264;  //9p9
	ItemLookup[9][11][7] = 134348800;  //9b7
	ItemLookup[9][31][24] = 134348800;  //9vo
	ItemLookup[9][28][8] = 134348800;  //9s8
	ItemLookup[9][25][10] = 134348800;  //9pa
	ItemLookup[9][17][9] = 134348800;  //9h9
	ItemLookup[9][32][12] = 134348800;  //9wc
	ItemLookup[8][28][11] = 134479872;  //8sb
	ItemLookup[8][17][11] = 134479872;  //8hb
	ItemLookup[8][21][11] = 134479872;  //8lb
	ItemLookup[8][12][11] = 134479872;  //8cb
	ItemLookup[8][28][8] = 134479872;  //8s8
	ItemLookup[8][21][8] = 134479872;  //8l8
	ItemLookup[8][28][32] = 134479872;  //8sw
	ItemLookup[8][21][32] = 134479872;  //8lw
	ItemLookup[8][21][33] = 134742016;  //8lx
	ItemLookup[8][22][33] = 134742016;  //8mx
	ItemLookup[8][17][33] = 134742016;  //8hx
	ItemLookup[8][27][33] = 134742016;  //8rx
	ItemLookup[8][28][28] = 135266304;  //8ss
	ItemLookup[8][21][28] = 135266304;  //8ls
	ItemLookup[8][12][28] = 135266304;  //8cs
	ItemLookup[8][11][28] = 135266304;  //8bs
	ItemLookup[8][32][28] = 135266304;  //8ws
	ItemLookup[9][32][23] = 136314880;  //9wn
	ItemLookup[9][34][32] = 136314880;  //9yw
	ItemLookup[9][11][32] = 136314880;  //9bw
	ItemLookup[9][16][32] = 136314880;  //9gw
	ItemLookup[9][28][12] = 138412032;  //9sc
	ItemLookup[9][26][28] = 138412032;  //9qs
	ItemLookup[9][32][28] = 138412032;  //9ws
	ItemLookup[9][10][27] = 142606336;  //9ar
	ItemLookup[9][32][11] = 142606336;  //9wb
	ItemLookup[9][33][15] = 142606336;  //9xf
	ItemLookup[9][12][28] = 142606336;  //9cs
	ItemLookup[9][21][32] = 142606336;  //9lw
	ItemLookup[9][17][32] = 142606336;  //9hw
	ItemLookup[9][26][27] = 142606336;  //9qr
	ItemLookup[24][11][6] = 150994944;  //ob6
	ItemLookup[24][11][7] = 150994944;  //ob7
	ItemLookup[24][11][8] = 150994944;  //ob8
	ItemLookup[24][11][9] = 150994944;  //ob9
	ItemLookup[24][11][10] = 150994944;  //oba
	ItemLookup[10][22][6] = 167772160;  //am6
	ItemLookup[10][22][7] = 167772160;  //am7
	ItemLookup[10][22][8] = 167772160;  //am8
	ItemLookup[10][22][9] = 167772160;  //am9
	ItemLookup[10][22][10] = 167772160;  //ama

	// Elite items
	ItemLookup[30][10][25] = 268435457;  //uap
	ItemLookup[30][20][25] = 268435457;  //ukp
	ItemLookup[30][21][22] = 268435457;  //ulm
	ItemLookup[30][17][21] = 268435457;  //uhl
	ItemLookup[30][17][22] = 268435457;  //uhm
	ItemLookup[30][27][23] = 268435457;  //urn
	ItemLookup[30][28][20] = 268435457;  //usk
	ItemLookup[30][17][9] = 268435457;  //uh9
	ItemLookup[30][30][18] = 268435458;  //uui
	ItemLookup[30][14][10] = 268435458;  //uea
	ItemLookup[30][21][10] = 268435458;  //ula
	ItemLookup[30][29][30] = 268435458;  //utu
	ItemLookup[30][23][16] = 268435458;  //ung
	ItemLookup[30][12][21] = 268435458;  //ucl
	ItemLookup[30][17][23] = 268435458;  //uhn
	ItemLookup[30][27][28] = 268435458;  //urs
	ItemLookup[30][25][21] = 268435458;  //upl
	ItemLookup[30][21][29] = 268435458;  //ult
	ItemLookup[30][21][13] = 268435458;  //uld
	ItemLookup[30][29][17] = 268435458;  //uth
	ItemLookup[30][30][21] = 268435458;  //uul
	ItemLookup[30][10][27] = 268435458;  //uar
	ItemLookup[30][29][25] = 268435458;  //utp
	ItemLookup[30][30][12] = 268435460;  //uuc
	ItemLookup[30][22][21] = 268435460;  //uml
	ItemLookup[30][27][16] = 268435460;  //urg
	ItemLookup[30][18][29] = 268435460;  //uit
	ItemLookup[30][24][32] = 268435460;  //uow
	ItemLookup[30][29][28] = 268435460;  //uts
	ItemLookup[30][28][17] = 268435460;  //ush
	ItemLookup[30][25][20] = 268435460;  //upk
	ItemLookup[30][21][16] = 268435464;  //ulg
	ItemLookup[30][31][16] = 268435464;  //uvg
	ItemLookup[30][22][16] = 268435464;  //umg
	ItemLookup[30][29][16] = 268435464;  //utg
	ItemLookup[30][17][16] = 268435464;  //uhg
	ItemLookup[30][21][11] = 268435472;  //ulb
	ItemLookup[30][31][11] = 268435472;  //uvb
	ItemLookup[30][22][11] = 268435472;  //umb
	ItemLookup[30][29][11] = 268435472;  //utb
	ItemLookup[30][17][11] = 268435472;  //uhb
	ItemLookup[30][21][12] = 268435488;  //ulc
	ItemLookup[30][31][12] = 268435488;  //uvc
	ItemLookup[30][22][12] = 268435488;  //umc
	ItemLookup[30][29][12] = 268435488;  //utc
	ItemLookup[30][17][12] = 268435488;  //uhc
	ItemLookup[13][27][11] = 268435520;  //drb
	ItemLookup[13][27][12] = 268435520;  //drc
	ItemLookup[13][27][13] = 268435520;  //drd
	ItemLookup[13][27][14] = 268435520;  //dre
	ItemLookup[13][27][15] = 268435520;  //drf
	ItemLookup[11][10][11] = 268435584;  //bab
	ItemLookup[11][10][12] = 268435584;  //bac
	ItemLookup[11][10][13] = 268435584;  //bad
	ItemLookup[11][10][14] = 268435584;  //bae
	ItemLookup[11][10][15] = 268435584;  //baf
	ItemLookup[25][10][11] = 268435712;  //pab
	ItemLookup[25][10][12] = 268435712;  //pac
	ItemLookup[25][10][13] = 268435712;  //pad
	ItemLookup[25][10][14] = 268435712;  //pae
	ItemLookup[25][10][15] = 268435712;  //paf
	ItemLookup[23][14][11] = 268435968;  //neb
	ItemLookup[23][14][12] = 268435968;  //nec
	ItemLookup[23][14][13] = 268435968;  //ned
	ItemLookup[23][14][14] = 268435968;  //nee
	ItemLookup[23][14][15] = 268435968;  //nef
	ItemLookup[7][17][10] = 268436480;  //7ha
	ItemLookup[7][10][33] = 268436480;  //7ax
	ItemLookup[7][2][10] = 268436480;  //72a
	ItemLookup[7][22][25] = 268436480;  //7mp
	ItemLookup[7][32][10] = 268436480;  //7wa
	ItemLookup[7][21][10] = 268436480;  //7la
	ItemLookup[7][11][10] = 268436480;  //7ba
	ItemLookup[7][11][29] = 268436480;  //7bt
	ItemLookup[7][16][10] = 268436480;  //7ga
	ItemLookup[7][16][18] = 268436480;  //7gi
	ItemLookup[7][12][21] = 268437504;  //7cl
	ItemLookup[7][28][25] = 268437504;  //7sp
	ItemLookup[7][22][10] = 268437504;  //7ma
	ItemLookup[7][22][15] = 268437504;  //7mf
	ItemLookup[7][15][21] = 268437504;  //7fl
	ItemLookup[7][32][17] = 268437504;  //7wh
	ItemLookup[7][22][7] = 268437504;  //7m7
	ItemLookup[7][16][22] = 268437504;  //7gm
	ItemLookup[7][28][28] = 268439552;  //7ss
	ItemLookup[7][28][22] = 268439552;  //7sm
	ItemLookup[7][28][11] = 268439552;  //7sb
	ItemLookup[7][15][12] = 268439552;  //7fc
	ItemLookup[7][12][27] = 268439552;  //7cr
	ItemLookup[7][11][28] = 268439552;  //7bs
	ItemLookup[7][21][28] = 268439552;  //7ls
	ItemLookup[7][32][13] = 268439552;  //7wd
	ItemLookup[7][2][17] = 268439552;  //72h
	ItemLookup[7][12][22] = 268439552;  //7cm
	ItemLookup[7][16][28] = 268439552;  //7gs
	ItemLookup[7][11][7] = 268439552;  //7b7
	ItemLookup[7][15][11] = 268439552;  //7fb
	ItemLookup[7][16][13] = 268439552;  //7gd
	ItemLookup[7][13][16] = 268443648;  //7dg
	ItemLookup[7][13][18] = 268443648;  //7di
	ItemLookup[7][20][27] = 268443648;  //7kr
	ItemLookup[7][11][21] = 268443648;  //7bl
	ItemLookup[7][29][20] = 268451840;  //7tk
	ItemLookup[7][29][10] = 268451840;  //7ta
	ItemLookup[7][11][20] = 268451840;  //7bk
	ItemLookup[7][11][8] = 268451840;  //7b8
	ItemLookup[7][19][10] = 268468224;  //7ja
	ItemLookup[7][25][18] = 268468224;  //7pi
	ItemLookup[7][28][7] = 268468224;  //7s7
	ItemLookup[7][16][21] = 268468224;  //7gl
	ItemLookup[7][29][28] = 268468224;  //7ts
	ItemLookup[7][28][27] = 268500992;  //7sr
	ItemLookup[7][29][27] = 268500992;  //7tr
	ItemLookup[7][11][27] = 268500992;  //7br
	ItemLookup[7][28][29] = 268500992;  //7st
	ItemLookup[7][25][7] = 268500992;  //7p7
	ItemLookup[7][24][7] = 268566528;  //7o7
	ItemLookup[7][31][24] = 268566528;  //7vo
	ItemLookup[7][28][8] = 268566528;  //7s8
	ItemLookup[7][25][10] = 268566528;  //7pa
	ItemLookup[7][17][7] = 268566528;  //7h7
	ItemLookup[7][32][12] = 268566528;  //7wc
	ItemLookup[6][28][11] = 268697600;  //6sb
	ItemLookup[6][17][11] = 268697600;  //6hb
	ItemLookup[6][21][11] = 268697600;  //6lb
	ItemLookup[6][12][11] = 268697600;  //6cb
	ItemLookup[6][28][7] = 268697600;  //6s7
	ItemLookup[6][21][7] = 268697600;  //6l7
	ItemLookup[6][28][32] = 268697600;  //6sw
	ItemLookup[6][21][32] = 268697600;  //6lw
	ItemLookup[6][21][33] = 268959744;  //6lx
	ItemLookup[6][22][33] = 268959744;  //6mx
	ItemLookup[6][17][33] = 268959744;  //6hx
	ItemLookup[6][27][33] = 268959744;  //6rx
	ItemLookup[6][28][28] = 269484032;  //6ss
	ItemLookup[6][21][28] = 269484032;  //6ls
	ItemLookup[6][12][28] = 269484032;  //6cs
	ItemLookup[6][11][28] = 269484032;  //6bs
	ItemLookup[6][32][28] = 269484032;  //6ws
	ItemLookup[7][32][23] = 270532608;  //7wn
	ItemLookup[7][34][32] = 270532608;  //7yw
	ItemLookup[7][11][32] = 270532608;  //7bw
	ItemLookup[7][16][32] = 270532608;  //7gw
	ItemLookup[7][28][12] = 272629760;  //7sc
	ItemLookup[7][26][28] = 272629760;  //7qs
	ItemLookup[7][32][28] = 272629760;  //7ws
	ItemLookup[7][10][27] = 276824064;  //7ar
	ItemLookup[7][32][11] = 276824064;  //7wb
	ItemLookup[7][33][15] = 276824064;  //7xf
	ItemLookup[7][12][28] = 276824064;  //7cs
	ItemLookup[7][21][32] = 276824064;  //7lw
	ItemLookup[7][17][32] = 276824064;  //7hw
	ItemLookup[7][26][27] = 276824064;  //7qr
	ItemLookup[24][11][11] = 285212672;  //obb
	ItemLookup[24][11][12] = 285212672;  //obc
	ItemLookup[24][11][13] = 285212672;  //obd
	ItemLookup[24][11][14] = 285212672;  //obe
	ItemLookup[24][11][15] = 285212672;  //obf
	ItemLookup[10][22][11] = 301989888;  //amb
	ItemLookup[10][22][12] = 301989888;  //amc
	ItemLookup[10][22][13] = 301989888;  //amd
	ItemLookup[10][22][14] = 301989888;  //ame
	ItemLookup[10][22][15] = 301989888;  //amf
}
