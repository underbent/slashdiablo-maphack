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

std::map<std::string, int> UnknownItemCodes;
vector<Rule*> RuleList;
vector<Rule*> MapRuleList;
vector<Rule*> IgnoreRuleList;
BYTE LastConditionType;

TrueCondition *trueCondition = new TrueCondition();
FalseCondition *falseCondition = new FalseCondition();

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

bool IsGem(ItemAttributes *attrs) {
	return (attrs->flags2 & ITEM_GROUP_GEM) > 0;
}

BYTE GetGemLevel(ItemAttributes *attrs) {
	if (attrs->flags2 & ITEM_GROUP_CHIPPED) {
		return 1;
	} else if (attrs->flags2 & ITEM_GROUP_FLAWED) {
		return 2;
	} else if (attrs->flags2 & ITEM_GROUP_REGULAR) {
		return 3;
	} else if (attrs->flags2 & ITEM_GROUP_FLAWLESS) {
		return 4;
	} else if (attrs->flags2 & ITEM_GROUP_PERFECT) {
		return 5;
	}
	return 0;
}

char *GetGemLevelString(BYTE level) {
	return GemLevels[level];
}

BYTE GetGemType(ItemAttributes *attrs) {
	if (attrs->flags2 & ITEM_GROUP_AMETHYST) {
		return 1;
	} else if (attrs->flags2 & ITEM_GROUP_DIAMOND) {
		return 2;
	} else if (attrs->flags2 & ITEM_GROUP_EMERALD) {
		return 3;
	} else if (attrs->flags2 & ITEM_GROUP_RUBY) {
		return 4;
	} else if (attrs->flags2 & ITEM_GROUP_SAPPHIRE) {
		return 5;
	} else if (attrs->flags2 & ITEM_GROUP_TOPAZ) {
		return 6;
	} else if (attrs->flags2 & ITEM_GROUP_SKULL) {
		return 7;
	}
	return 0;
}

char *GetGemTypeString(BYTE type) {
	return GemTypes[type];
}

bool IsRune(ItemAttributes *attrs) {
	return (attrs->flags2 & ITEM_GROUP_RUNE) > 0;
}

BYTE RuneNumberFromItemCode(char *code){
	return (BYTE)(((code[1] - '0') * 10) + code[2] - '0');
}

void GetItemName(UnitItemInfo *uInfo, string &name) {
	for (vector<Rule*>::iterator it = RuleList.begin(); it != RuleList.end(); it++) {
		if ((*it)->Evaluate(uInfo, NULL)) {
			SubstituteNameVariables(uInfo, name, &(*it)->action);
			if ((*it)->action.stopProcessing) {
				break;
			}
		}
	}
}

void SubstituteNameVariables(UnitItemInfo *uInfo, string &name, Action *action) {
	char origName[128], sockets[4], code[4], ilvl[4], alvl[4], runename[16] = "", runenum[4] = "0";
	char gemtype[16] = "", gemlevel[16] = "", sellValue[16] = "";

	UnitAny *item = uInfo->item;
	ItemText *txt = D2COMMON_GetItemText(item->dwTxtFileNo);
	char *szCode = txt->szCode;
	code[0] = szCode[0];
	code[1] = szCode[1];
	code[2] = szCode[2];
	code[3] = '\0';
	sprintf_s(sockets, "%d", D2COMMON_GetUnitStat(item, STAT_SOCKETS, 0));
	sprintf_s(ilvl, "%d", item->pItemData->dwItemLevel);
	sprintf_s(alvl, "%d", GetAffixLevel((BYTE)item->pItemData->dwItemLevel, (BYTE)uInfo->attrs->qualityLevel, uInfo->attrs->flags, code));
	sprintf_s(origName, "%s", name.c_str());

	UnitAny* pUnit = D2CLIENT_GetPlayerUnit();
	if (pUnit && txt->fQuest == 0) {
		sprintf_s(sellValue, "%d", D2COMMON_GetItemPrice(pUnit, item, D2CLIENT_GetDifficulty(), (DWORD)D2CLIENT_GetQuestInfo(), 0x201, 1));
	}

	if (IsRune(uInfo->attrs)) {
		sprintf_s(runenum, "%d", RuneNumberFromItemCode(code));
		sprintf_s(runename, name.substr(0, name.find(' ')).c_str());
	} else if (IsGem(uInfo->attrs)) {
		sprintf_s(gemlevel, "%s", GetGemLevelString(GetGemLevel(uInfo->attrs)));
		sprintf_s(gemtype, "%s", GetGemTypeString(GetGemType(uInfo->attrs)));
	}
	ActionReplace replacements[] = {
		{"NAME", origName},
		{"SOCKETS", sockets},
		{"RUNENUM", runenum},
		{"RUNENAME", runename},
		{"GEMLEVEL", gemlevel},
		{"GEMTYPE", gemtype},
		{"ILVL", ilvl},
		{"ALVL", alvl},
		{"CODE", code},
		{"PRICE", sellValue},
		COLOR_REPLACEMENTS
	};
	name.assign(action->name);
	for (int n = 0; n < sizeof(replacements) / sizeof(replacements[0]); n++) {
		if (name.find("%" + replacements[n].key + "%") == string::npos)
			continue;
		name.replace(name.find("%" + replacements[n].key + "%"), replacements[n].key.length() + 2, replacements[n].value);
	}
}

BYTE GetAffixLevel(BYTE ilvl, BYTE qlvl, unsigned int flags, char *code) {
	BYTE mlvl = 0;
	if ((code[0] == 'r' && code[1] == 'i' && code[2] == 'n') ||
		(code[0] == 'a' && code[1] == 'm' && code[2] == 'u') ||
		(code[0] == 'j' && code[1] == 'e' && code[2] == 'w') ||
		(code[0] == 'c' && code[1] == 'm' && code[2] >= '1' && code[2] <= '3')) {
		qlvl = ilvl;
	} else if (code[0] == 'c' && code[1] == 'i' && code[2] == '0') {
		mlvl = 3;
	} else if (code[0] == 'c' && code[1] == 'i' && code[2] == '1') {
		mlvl = 8;
	} else if (code[0] == 'c' && code[1] == 'i' && code[2] == '2') {
		mlvl = 13;
	} else if (code[0] == 'c' && code[1] == 'i' && code[2] == '3') {
		mlvl = 18;
	} else if (flags & ITEM_GROUP_WAND || flags & ITEM_GROUP_STAFF || flags & ITEM_GROUP_SORCERESS_ORB) {
		mlvl = 1;
	}
	if (ilvl > 99) {
		ilvl = 99;
	}
	if (qlvl > ilvl) {
		ilvl = qlvl;
	}
	if (mlvl > 0) {
		return ilvl + mlvl > 99 ? 99 : ilvl + mlvl;
	}
	return ((ilvl) < (99 - ((qlvl)/2)) ? (ilvl) - ((qlvl)/2) : (ilvl) * 2 - 99);
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

namespace ItemDisplay {
	bool item_display_initialized = false;
	void InitializeItemRules() {
		if (item_display_initialized) return;
		if (!IsInitialized()){
			return;
		}

		item_display_initialized = true;
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

			RuleList.push_back(r);
			if (r->action.colorOnMap.length() > 0) {
				MapRuleList.push_back(r);
			}
			else if (r->action.name.length() == 0) {
				IgnoreRuleList.push_back(r);
			}
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
	vector<Condition*> endConditions;
	int i;

	// Since we don't have a real parser, things will break if [!()] appear in
	// the middle of a token (e.g. "(X AND Y)(A AND B)")

	// Look for syntax characters at the beginning of the token
	for (i = 0; i < (int)token.length(); i++) {
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
	token.erase(0, i);

	// Look for syntax characters at the end of the token
	for (i = token.length() - 1; i >= 0; i--) {
		if (token[i] == '!') {
			endConditions.insert(endConditions.begin(), new NegationOperator());
		} else if (token[i] == '(') {
			endConditions.insert(endConditions.begin(), new LeftParen());
		} else if (token[i] == ')') {
			endConditions.insert(endConditions.begin(), new RightParen());
		} else {
			break;
		}
	}
	if (i < (int)(token.length() - 1)) {
		token.erase(i + 1, string::npos);
	}

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
	} else if (key.compare(0, 2, "RW") == 0) {
		Condition::AddOperand(conditions, new FlagsCondition(ITEM_RUNEWORD));
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
	} else if (key.compare(0, 4, "ALVL") == 0) {
		Condition::AddOperand(conditions, new AffixLevelCondition(operation, value));
	} else if (key.compare(0, 4, "RUNE") == 0) {
		Condition::AddOperand(conditions, new RuneCondition(operation, value));
	} else if (key.compare(0, 4, "GOLD") == 0) {
		Condition::AddOperand(conditions, new GoldCondition(operation, value));
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
	} else if (key.compare(0, 3, "IAS") == 0) {
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_IAS, 0, operation, value));
	} else if (key.compare(0, 4, "LIFE") == 0) {
		// For unknown reasons, the game's internal HP stat is 256 for every 1 displayed on item
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_MAXHP, 0, operation, value * 256));
	} else if (key.compare(0, 5, "ARMOR") == 0) {
		Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_ALLARMOR));
	} else if (key.compare(0, 2, "EQ") == 0 && keylen == 3) {
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
		} else if (key[2] == '7') {
			Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_CIRCLET));
		}
	} else if (key.compare(0, 2, "CL") == 0 && keylen == 3) {
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
	} else if (key.compare(0, 6, "WEAPON") == 0) {
		Condition::AddOperand(conditions, new ItemGroupCondition(ITEM_GROUP_ALLWEAPON));
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
		if ((ss >> num).fail() || num < 0 || num > (int)STAT_MAX) {
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
	} else if (key.compare(0, 5, "ALLSK") == 0) {
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_ALLSKILLS, 0, operation, value));
	} else if (key.compare(0, 5, "TABSK") == 0) {
		int num = -1;
		stringstream ss(key.substr(5));
		if ((ss >> num).fail() || num < 0 || num > SKILLTAB_MAX) {
			return;
		}
		Condition::AddOperand(conditions, new ItemStatCondition(STAT_SKILLTAB, num, operation, value));
	} else if (key.compare(0, 4, "STAT") == 0) {
		int num = -1;
		stringstream ss(key.substr(4));
		if ((ss >> num).fail() || num < 0 || num > (int)STAT_MAX) {
			return;
		}
		Condition::AddOperand(conditions, new ItemStatCondition(num, 0, operation, value));
	}

	for (vector<Condition*>::iterator it = endConditions.begin(); it != endConditions.end(); it++) {
		Condition::AddNonOperand(conditions, (*it));
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

bool Condition::Evaluate(UnitItemInfo *uInfo, ItemInfo *info, Condition *arg1, Condition *arg2) {
	// Arguments will vary based on where we're called from.
	// We will have either *info set (if called on reception of packet 0c9c, in which case
	// the normal item structures won't have been set up yet), or *uInfo otherwise.
	if (info) {
		return EvaluateInternalFromPacket(info, arg1, arg2);
	}
	return EvaluateInternal(uInfo, arg1, arg2);
}

bool TrueCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return true;
}
bool TrueCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return true;
}

bool FalseCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return false;
}
bool FalseCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return false;
}

bool NegationOperator::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return !arg1->Evaluate(uInfo, NULL, arg1, arg2);
}
bool NegationOperator::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return !arg1->Evaluate(NULL, info, arg1, arg2);
}

bool LeftParen::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return false;
}
bool LeftParen::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return false;
}

bool RightParen::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return false;
}
bool RightParen::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return false;
}

bool AndOperator::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(uInfo, NULL, NULL, NULL) && arg2->Evaluate(uInfo, NULL, NULL, NULL);
}
bool AndOperator::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(NULL, info, NULL, NULL) && arg2->Evaluate(NULL, info, NULL, NULL);
}

bool OrOperator::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(uInfo, NULL, NULL, NULL) || arg2->Evaluate(uInfo, NULL, NULL, NULL);
}
bool OrOperator::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return arg1->Evaluate(NULL, info, NULL, NULL) || arg2->Evaluate(NULL, info, NULL, NULL);
}

bool ItemCodeCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return (targetCode[0] == uInfo->itemCode[0] && targetCode[1] == uInfo->itemCode[1] && targetCode[2] == uInfo->itemCode[2]);
}
bool ItemCodeCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return (targetCode[0] == info->code[0] && targetCode[1] == info->code[1] && targetCode[2] == info->code[2]);
}

bool FlagsCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return ((uInfo->item->pItemData->dwFlags & flag) > 0);
}
bool FlagsCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	switch (flag) {
	case ITEM_ETHEREAL:
		return info->ethereal;
	case ITEM_IDENTIFIED:
		return info->identified;
	case ITEM_RUNEWORD:
		return info->runeword;
	}
	return false;
}

bool QualityCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return (uInfo->item->pItemData->dwQuality == quality);
}
bool QualityCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return (info->quality == quality);
}

bool NonMagicalCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return (uInfo->item->pItemData->dwQuality == ITEM_QUALITY_INFERIOR ||
			uInfo->item->pItemData->dwQuality == ITEM_QUALITY_NORMAL ||
			uInfo->item->pItemData->dwQuality == ITEM_QUALITY_SUPERIOR);
}
bool NonMagicalCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return (info->quality == ITEM_QUALITY_INFERIOR ||
			info->quality == ITEM_QUALITY_NORMAL ||
			info->quality == ITEM_QUALITY_SUPERIOR);
}

bool GemLevelCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	if (IsGem(uInfo->attrs)) {
		return IntegerCompare(GetGemLevel(uInfo->attrs), operation, gemLevel);
	}
	return false;
}
bool GemLevelCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	if (IsGem(info->attrs)) {
		return IntegerCompare(GetGemLevel(info->attrs), operation, gemLevel);
	}
	return false;
}

bool GemTypeCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	if (IsGem(uInfo->attrs)) {
		return IntegerCompare(GetGemType(uInfo->attrs), operation, gemType);
	}
	return false;
}
bool GemTypeCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	if (IsGem(info->attrs)) {
		return IntegerCompare(GetGemType(info->attrs), operation, gemType);
	}
	return false;
}

bool RuneCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	if (IsRune(uInfo->attrs)) {
		return IntegerCompare(RuneNumberFromItemCode(uInfo->itemCode), operation, runeNumber);
	}
	return false;
}
bool RuneCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	if (IsRune(info->attrs)) {
		return IntegerCompare(RuneNumberFromItemCode(info->code), operation, runeNumber);
	}
	return false;
}

bool GoldCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return false; // can only evaluate this from packet data
}
bool GoldCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	if (info->code[0] == 'g' && info->code[1] == 'l' && info->code[2] == 'd') {
		return IntegerCompare(info->amount, operation, goldAmount);
	}
	return false;
}

bool ItemLevelCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return IntegerCompare(uInfo->item->pItemData->dwItemLevel, operation, itemLevel);
}
bool ItemLevelCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return IntegerCompare(info->level, operation, itemLevel);
}

bool AffixLevelCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	BYTE qlvl = uInfo->attrs->qualityLevel;
	BYTE alvl = GetAffixLevel((BYTE)uInfo->item->pItemData->dwItemLevel, (BYTE)uInfo->attrs->qualityLevel, uInfo->attrs->flags, uInfo->itemCode);
	return IntegerCompare(alvl, operation, affixLevel);
}
bool AffixLevelCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	int qlvl = info->attrs->qualityLevel;
	BYTE alvl = GetAffixLevel(info->level, info->attrs->qualityLevel, info->attrs->flags, info->code);
	return IntegerCompare(alvl, operation, affixLevel);
}

bool ItemGroupCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return ((uInfo->attrs->flags & itemGroup) > 0);
}
bool ItemGroupCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	return ((info->attrs->flags & itemGroup) > 0);
}

bool EDCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	// Either enhanced defense or enhanced damage depending on item type
	WORD stat;
	if (uInfo->attrs->flags & ITEM_GROUP_ALLARMOR) {
		stat = STAT_ENHANCEDDEFENSE;
	} else {
		// Normal %ED will have the same value for STAT_ENHANCEDMAXIMUMDAMAGE and STAT_ENHANCEDMINIMUMDAMAGE
		stat = STAT_ENHANCEDMAXIMUMDAMAGE;
	}

	// Pulled from JSUnit.cpp in d2bs
	DWORD value = 0;
	Stat aStatList[256] = { NULL };
	StatList* pStatList = D2COMMON_GetStatList(uInfo->item, NULL, 0x40);
	if (pStatList) {
		DWORD dwStats = D2COMMON_CopyStatList(pStatList, (Stat*)aStatList, 256);
		for (UINT i = 0; i < dwStats; i++) {
			if (aStatList[i].wStatIndex == stat && aStatList[i].wSubIndex == 0) {
				value += aStatList[i].dwStatValue;
			}
		}
	}
	return IntegerCompare(value, operation, targetED);
}
bool EDCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	// Either enhanced defense or enhanced damage depending on item type
	WORD stat;
	if (info->attrs->flags & ITEM_GROUP_ALLARMOR) {
		stat = STAT_ENHANCEDDEFENSE;
	} else {
		// Normal %ED will have the same value for STAT_ENHANCEDMAXIMUMDAMAGE and STAT_ENHANCEDMINIMUMDAMAGE
		stat = STAT_ENHANCEDMAXIMUMDAMAGE;
	}

	DWORD value = 0;
	for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
		if (prop->stat == stat) {
			value += prop->value;
		}
	}
	return IntegerCompare(value, operation, targetED);
}

bool ItemStatCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	return IntegerCompare(D2COMMON_GetUnitStat(uInfo->item, itemStat, itemStat2), operation, targetStat);
}
bool ItemStatCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	int num = 0;
	switch (itemStat) {
	case STAT_SOCKETS:
		return IntegerCompare(info->sockets, operation, targetStat);
	case STAT_DEFENSE:
		return IntegerCompare(GetDefense(info), operation, targetStat);
	case STAT_SINGLESKILL:
		for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
			if (prop->stat == STAT_SINGLESKILL && prop->skill == itemStat2) {
				num += prop->value;
			}
		}
		return IntegerCompare(num, operation, targetStat);
	case STAT_CLASSSKILLS:
		for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
			if (prop->stat == STAT_CLASSSKILLS && prop->characterClass == itemStat2) {
				num += prop->value;
			}
		}
		return IntegerCompare(num, operation, targetStat);
	case STAT_SKILLTAB:
		for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
			if (prop->stat == STAT_SKILLTAB && (prop->characterClass * 8 + prop->tab) == itemStat2) {
				num += prop->value;
			}
		}
		return IntegerCompare(num, operation, targetStat);
	default:
		for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
			if (prop->stat == itemStat) {
				num += prop->value;
			}
		}
		return IntegerCompare(num, operation, targetStat);
	}
	return false;
}

bool ResistAllCondition::EvaluateInternal(UnitItemInfo *uInfo, Condition *arg1, Condition *arg2) {
	int fRes = D2COMMON_GetUnitStat(uInfo->item, STAT_FIRERESIST, 0);
	int lRes = D2COMMON_GetUnitStat(uInfo->item, STAT_LIGHTNINGRESIST, 0);
	int cRes = D2COMMON_GetUnitStat(uInfo->item, STAT_COLDRESIST, 0);
	int pRes = D2COMMON_GetUnitStat(uInfo->item, STAT_POISONRESIST, 0);
	return (IntegerCompare(fRes, operation, targetStat) &&
			IntegerCompare(lRes, operation, targetStat) &&
			IntegerCompare(cRes, operation, targetStat) &&
			IntegerCompare(pRes, operation, targetStat));
}
bool ResistAllCondition::EvaluateInternalFromPacket(ItemInfo *info, Condition *arg1, Condition *arg2) {
	int fRes = 0, lRes = 0, cRes = 0, pRes = 0;
	for (vector<ItemProperty>::iterator prop = info->properties.begin(); prop < info->properties.end(); prop++) {
		if (prop->stat == STAT_FIRERESIST) {
			fRes += prop->value;
		} else if (prop->stat == STAT_LIGHTNINGRESIST) {
			lRes += prop->value;
		} else if (prop->stat == STAT_COLDRESIST) {
			cRes += prop->value;
		} else if (prop->stat == STAT_POISONRESIST) {
			pRes += prop->value;
		}
	}
	return (IntegerCompare(fRes, operation, targetStat) &&
			IntegerCompare(lRes, operation, targetStat) &&
			IntegerCompare(cRes, operation, targetStat) &&
			IntegerCompare(pRes, operation, targetStat));
}

int GetDefense(ItemInfo *item) {
	int def = item->defense;
	for (vector<ItemProperty>::iterator prop = item->properties.begin(); prop < item->properties.end(); prop++) {
		if (prop->stat == STAT_ENHANCEDDEFENSE) {
			def *= (prop->value + 100);
			def /= 100;
		}
	}
	return def;
}

void HandleUnknownItemCode(char *code, char *tag) {
	// If the MPQ files arent loaded yet then this is expected
	if (!IsInitialized()){
		return;
	}

	// Avoid spamming endlessly
	if (UnknownItemCodes.size() > 10 || (*BH::MiscToggles2)["Allow Unknown Items"].state) {
		return;
	}
	if (UnknownItemCodes.find(code) == UnknownItemCodes.end()) {
		PrintText(1, "Unknown item code %s: %c%c%c\n", tag, code[0], code[1], code[2]);
		UnknownItemCodes[code] = 1;
	}
}

StatProperties *GetStatProperties(unsigned int stat) {
	return AllStatList.at(stat);
}
