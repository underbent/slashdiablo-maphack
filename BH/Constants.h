#pragma once

enum TextColor {
	Disabled=-1,
	White=0,
	Red,
	Green,
	Blue,
	Gold,
	Grey,
	Black,
	Tan,
	Orange,
	Yellow,
	DarkGreen,
	Purple,
	Silver=15
};

//////////////////////////////////////////////////////////////////////
// Attack Types
//////////////////////////////////////////////////////////////////////
#define ATTACKTYPE_SHIFTLEFT		0xE5
#define ATTACKTYPE_UNITLEFT			0xC5
#define ATTACKTYPE_SHIFTRIGHT		0x66
#define ATTACKTYPE_RIGHT			0x46

///////////////////////////////////////////////////
// Game UI Flags
///////////////////////////////////////////////////
#define	UI_GAME					0x00
#define UI_INVENTORY			0x01
#define UI_CHARACTER			0x02
#define UI_MINISKILL			0x03
#define UI_SKILLTREE			0x04
#define UI_CHAT_CONSOLE			0x05
#define UI_UNK					0x06   //auto disabled
#define UI_UNK_EX				0x07   //auto disabled
#define UI_NPCMENU				0x08	
#define UI_DIALOG				0x08
#define UI_ESCMENU_MAIN			0x09
#define UI_AUTOMAP				0x0A
#define UI_HOTKEY_CONFIG		0x0B
#define UI_NPCSHOP				0x0C
#define UI_ALTDOWN				0x0D
#define UI_GROUND_ITEMS			0x0D
#define UI_ANVIL				0x0E
#define UI_QUEST				0x0F
#define UI_BARKSCROLL			0x10
#define UI_QUEST_LOG			0x11
#define UI_STATUS_AREA			0x12
#define UI_ESCMENU_EX			0x13
#define UI_WPMENU				0x14 //Waypoints Menu
#define UI_MINIPANEL			0x15
#define UI_PARTY				0x16
#define UI_TRADE				0x17
#define UI_MESSAGE_LOG			0x18
#define UI_STASH				0x19
#define UI_CUBE					0x1A
#define UI_UNUSED				0x1B
#define UI_INVENTORY_EX			0x1C
#define UI_INVENTORY_EXX		0x1D
#define UI_UNUSED_EX			0x1E
#define UI_BELT					0x1F
#define UI_UNUSED_EXX			0x20
#define UI_HELP_MENU			0x21
#define UI_HELP_BUTTON			0x22
#define UI_ESCMENU				0x23 //unused??
#define UI_MERC					0x24
#define UI_SCROLL				0x25

///////////////////////////////////////////////////
// Control types
///////////////////////////////////////////////////
#define CONTROL_EDITBOX			0x01
#define CONTROL_IMAGE			0x02
#define CONTROL_UNUSED			0x03
#define CONTROL_TEXTBOX			0x04
#define CONTROL_SCROLLBAR		0x05
#define CONTROL_BUTTON			0x06
#define CONTROL_LIST			0x07
#define CONTROL_TIMER			0x08
#define CONTROL_SMACK			0x09
#define CONTROL_PROGRESSBAR		0x0a
#define CONTROL_POPUP			0x0b
#define CONTROL_ACCOUNTLIST		0x0c

//////////////////////////////////////////////////////////////////////
// PVP Flags
//////////////////////////////////////////////////////////////////////
#define PVP_INVITED_YOU			0x01 // Invited you, but you may not have accepted
#define PVP_INVITED_BY_YOU		0x02 // Invited by you, but he may not have accepted
#define PVP_ALLIED				0x04 // Allied
#define PVP_HOSTILED_YOU		0x08 // Hostiled you, but you may not have hostiled him
#define PVP_HOSTILED_BY_YOU		0x10 // Hostiled by you, but he may not have hostiled you

//////////////////////////////////////////////////////////////////////////
// Player Type Flags
//////////////////////////////////////////////////////////////////////////
#define PLAYER_TYPE_HARDCORE	4
#define PLAYER_TYPE_DEAD		8
#define PLAYER_TYPE_EXPANSION	32
#define PLAYER_TYPE_LADDER		64

//////////////////////////////////////////////////////////////////////////
// Resistance Penalties
//////////////////////////////////////////////////////////////////////////
#define RES_PENALTY_CLS_NORM		0
#define RES_PENALTY_CLS_NM			-20
#define RES_PENALTY_CLS_HELL		-50
#define RES_PENALTY_XPAC_NORM		(2*RES_PENALTY_CLS_NORM)
#define RES_PENALTY_XPAC_NM			(2*RES_PENALTY_CLS_NM)
#define RES_PENALTY_XPAC_HELL		(2*RES_PENALTY_CLS_HELL)

//////////////////////////////////////////////////////////////////////////
// Trade Flags
//////////////////////////////////////////////////////////////////////////
#define TRADE_OPEN				3
#define TRADE_ACCEPTED			5
#define TRADE_ACCEPT			7

///////////////////////////////////////////////////
// Mercenary Class ID's
///////////////////////////////////////////////////
#define MERC_A1		0x010f
#define MERC_A2		0x0152
#define MERC_A3		0x0167
#define MERC_A5		0x0231

///////////////////////////////////////////////////
// Unit Stat ID Definition (Partial)
///////////////////////////////////////////////////
#define STAT_STRENGTH				0		// str
#define	STAT_ENERGY					1		// energy
#define	STAT_DEXTERITY				2		// dexterity
#define	STAT_VITALITY				3		// vitality
#define STAT_STATPOINTSLEFT			4
#define	STAT_HP						6		// life
#define	STAT_MAXHP					7		// max life
#define	STAT_MANA					8		// mana
#define	STAT_MAXMANA				9		// max mana
#define	STAT_STAMINA				10		// stamina
#define	STAT_MAXSTAMINA				11		// max stamina
#define	STAT_LEVEL					12		// level
#define STAT_EXP					13		// experience
#define	STAT_GOLD					14		// gold
#define	STAT_GOLDBANK				15		// stash gold
#define	STAT_TOBLOCK				20		// to block
#define	STAT_DAMAGEREDUCTION		36		// damage reduction
#define	STAT_MAGICDAMAGEREDUCTION	35		// magic damage reduction
#define	STAT_MAGICRESIST			37		// magic resist
#define	STAT_MAXMAGICRESIST			38		// max magic resist
#define	STAT_FIRERESIST				39		// fire resist
#define	STAT_MAXFIRERESIST			40		// max fire resist
#define	STAT_LIGHTNINGRESIST		41		// lightning resist
#define	STAT_MAXLIGHTNINGRESIST		42		// max lightning resist
#define	STAT_COLDRESIST				43		// cold resist
#define	STAT_MAXCOLDRESIST			44		// max cold resist
#define	STAT_POISONRESIST			45		// poison resist
#define	STAT_MAXPOISONRESIST		46		// max poison resist
#define STAT_LIFELEECH				60		// Life Leech
#define STAT_MANALEECH				62		// Mana Leech
#define	STAT_VELOCITYPERCENT		67		// effective run/walk
#define STAT_AMMOQUANTITY			70		// ammo quantity(arrow/bolt/throwing)
#define STAT_DURABILITY				72		// item durability
#define STAT_MAXDURABILITY			73		// max item durability
#define STAT_GOLDFIND				79		// Gold find (GF)
#define	STAT_MAGICFIND				80		// magic find (MF)
#define	STAT_IAS					93		// IAS
#define	STAT_FASTERRUNWALK			96		// faster run/walk
#define	STAT_FASTERHITRECOVERY		99		// faster hit recovery
#define	STAT_FASTERBLOCK			102		// faster block rate
#define	STAT_FASTERCAST				105		// faster cast rate
#define STAT_POISONLENGTHREDUCTION	110		// Poison length reduction
#define STAT_OPENWOUNDS				135		// Open Wounds
#define	STAT_CRUSHINGBLOW			136		// crushing blow
#define	STAT_DEADLYSTRIKE			141		// deadly strike
#define	STAT_FIREABSORBPERCENT		142		// fire absorb %
#define	STAT_FIREABSORB				143		// fire absorb
#define	STAT_LIGHTNINGABSORBPERCENT	144		// lightning absorb %
#define	STAT_LIGHTNINGABSORB		145		// lightning absorb
#define	STAT_MAGICABSORBPERCENT		146		// magic absorb %
#define	STAT_COLDABSORBPERCENT		148		// cold absorb %
#define	STAT_COLDABSORB				149		// cold absorb
#define STAT_SLOW					150		// slow %

///////////////////////////////////////////////////
// NodePages
///////////////////////////////////////////////////
#define NODEPAGE_STORAGE	1
#define NODEPAGE_BELTSLOTS	2
#define NODEPAGE_EQUIP		3

///////////////////////////////////////////////////
// Body Locations
///////////////////////////////////////////////////
enum BodyLocation {
	EQUIP_NONE = 0,			// Not equipped
	EQUIP_HEAD,				// Head
	EQUIP_AMULET,			// Amulet
	EQUIP_BODY,				// Body armor
	EQUIP_RIGHT_PRIMARY,	// Right primary slot
	EQUIP_LEFT_PRIMARY,		// Left primary slot
	EQUIP_RIGHT_RING,		// Right ring
	EQUIP_LEFT_RING,		// Left ring
	EQUIP_BELT,				// Belt
	EQUIP_FEET,				// Feet
	EQUIP_GLOVES,			// Gloves
	EQUIP_RIGHT_SECONDARY,	// Right secondary slot
	EQUIP_LEFT_SECONDARY,	// Left secondary slot
};

///////////////////////////////////////////////////
// Storage Buffers
///////////////////////////////////////////////////
enum StorageBuffer {
	STORAGE_INVENTORY	= 0x00,
	STORAGE_EQUIP		= 0x01,
	STORAGE_BELT		= 0x02,
	STORAGE_CUBE		= 0x03,
	STORAGE_STASH		= 0x04,
	STORAGE_NULL		= 0xFF
};

///////////////////////////////////////////////////
// Item Quality
///////////////////////////////////////////////////
#define	ITEM_QUALITY_INFERIOR			0x01
#define	ITEM_QUALITY_NORMAL				0x02
#define	ITEM_QUALITY_SUPERIOR			0x03
#define	ITEM_QUALITY_MAGIC				0x04
#define	ITEM_QUALITY_SET				0x05
#define	ITEM_QUALITY_RARE				0x06
#define	ITEM_QUALITY_UNIQUE				0x07
#define	ITEM_QUALITY_CRAFT				0x08

///////////////////////////////////////////////////
// Player Mode Definition
///////////////////////////////////////////////////
enum PlayerMode {
	PLAYER_MODE_DEATH = 0,				// death
	PLAYER_MODE_STAND_OUTTOWN,			// standing outside town
	PLAYER_MODE_WALK_OUTTOWN,			// walking outside town
	PLAYER_MODE_RUN,					// running
	PLAYER_MODE_BEING_HIT,				// being hit
	PLAYER_MODE_STAND_INTOWN,			// standing inside town
	PLAYER_MODE_WALK_INTOWN,			// walking outside town
	PLAYER_MODE_ATTACK1,				// attacking 1
	PLAYER_MODE_ATTACK2,				// attacking 2
	PLAYER_MODE_BLOCK,					// blocking
	PLAYER_MODE_CAST,					// casting spell
	PLAYER_MODE_THROW,					// throwing
	PLAYER_MODE_KICK,					// kicking (assassin)
	PLAYER_MODE_USESKILL1,				// using skill 1
	PLAYER_MODE_USESKILL2,				// using skill 2
	PLAYER_MODE_USESKILL3,				// using skill 3
	PLAYER_MODE_USESKILL4,				// using skill 4
	PLAYER_MODE_DEAD,					// dead
	PLAYER_MODE_SEQUENCE,				// sequence
	PLAYER_MODE_BEING_KNOCKBACK			// being knocked back
};

///////////////////////////////////////////////////
// NPC Mode Definition
///////////////////////////////////////////////////
enum NpcMode {
	NPC_MODE_DEATH = 0,			// NPC death
	NPC_MODE_STAND,				// NPC standing still
	NPC_MODE_WALK,				// NPC walking
	NPC_MODE_BEING_HIT,			// NPC getting hit
	NPC_MODE_ATTACK1,			// NPC attacking 1
	NPC_MODE_ATTACK2,			// NPC attacking 2
	NPC_MODE_BLOCK,				// NPC blocking
	NPC_MODE_CAST,				// NPC casting spell skill
	NPC_MODE_USESKILL1,			// NPC using skill 1
	NPC_MODE_USESKILL2,			// NPC using skill 2
	NPC_MODE_USESKILL3,			// NPC using skill 3
	NPC_MODE_USESKILL4,			// NPC using skill 4
	NPC_MODE_DEAD,				// NPC dead
	NPC_MODE_BEING_KNOCKBACK,	// NPC being knocked back
	NPC_MODE_SEQUENCE,			// NPC sequence
	NPC_MODE_RUN				// NPC running
};

///////////////////////////////////////////////////
// Object Mode Definition
///////////////////////////////////////////////////
enum ObjectMode {
	OBJ_MODE_IDLE = 0,			// Object idle
	OBJ_MODE_OPERATING,			// Object operating
	OBJ_MODE_OPENED,			// Object opened
	OBJ_MODE_SPECIAL1,			// Object special 1
	OBJ_MODE_SPECIAL2,			// Object special 2
	OBJ_MODE_SPECIAL3,			// Object special 3
	OBJ_MODE_SPECIAL4,			// Object special 4
	OBJ_MODE_SPECIAL5			// Object special 5
};

///////////////////////////////////////////////////
// Item Mode Definition
///////////////////////////////////////////////////
enum ItemMode {
	ITEM_MODE_INV_STASH_CUBE_STORE = 0,		// Item inven stash cube store
	ITEM_MODE_EQUIPPED,						// Item equipped self or merc
	ITEM_MODE_IN_BELT,						// Item in belt
	ITEM_MODE_ON_GROUND,					// Item on ground
	ITEM_MODE_ON_CURSOR,					// Item on cursor
	ITEM_MODE_BEING_DROPPED,				// Item being dropped
	ITEM_MODE_SOCKETED_IN_ITEM				// Item socketed in item
};

///////////////////////////////////////////////////
// Unit Types
///////////////////////////////////////////////////
enum Type {
	UNIT_PLAYER = 0,
	UNIT_MONSTER,
	UNIT_OBJECT,
	UNIT_MISSLE,
	UNIT_ITEM,
	UNIT_TILE,
	EXIT,
	XY
};

///////////////////////////////////////////////////
// Spell Stats
///////////////////////////////////////////////////
#define DSI_PASSIVE			0x00000001 // Spell is passive
#define DSI_RIGHTONLY		0x00000002 // Spell must be on right side, like paladin auras
#define DSI_TARGETABLE		0x00000004 // Spell cannot target, like Nova
#define DSI_NEEDCORPSE		0x00000008 // Spell requires a corpse, like Revive
#define DSI_ENABLEINTOWN	0x00000010 // Spell is enabled in town
#define DSI_PHYSICAL		0x00000020 // Spell causes physical damage
#define DSI_MAGICAL			0x00000040 // Spell causes magical damage
#define DSI_FIRE			0x00000080 // Spell causes fire damage
#define DSI_COLD			0x00000100 // Spell causes cold damage
#define DSI_LIGHTNING		0x00000200 // Spell causes lightning damage
#define DSI_POISON			0x00000400 // Spell causes poison damage
#define DSI_KNOCKBACK		0x00000800 // Spell knocks target(s) back
#define DSI_STUN			0x00001000 // Spell stuns target(s)
#define DSI_AFFECTGROUP		0x00002000 // Spell affects multuple targets
#define DSI_MELEE			0x00004000 // Spell only affects target(s) within melee range
#define DSI_SUMMON			0x00008000 // Spell summons minion(s), like Valkyrie
#define DSI_PARTYCAST		0x00010000 // Spell castable on other party members
#define DSI_GUIDED			0x00020000 // Spell is guided to enemies

///////////////////////////////////////////////////
// Character Classes
///////////////////////////////////////////////////
enum {
	CLASS_AMA = 0,
	CLASS_SOR,
	CLASS_NEC,
	CLASS_PAL,
	CLASS_BAR,
	CLASS_DRU,
	CLASS_ASN,
	CLASS_NA
};

///////////////////////////////////////////////////
// Map Definition
///////////////////////////////////////////////////
#define MAP_UNKNOWN								0x00

///////////////////////////////////////////////////
// Act 1 Maps
///////////////////////////////////////////////////
#define MAP_A1_ROGUE_ENCAMPMENT					0x01
#define MAP_A1_BLOOD_MOOR						0x02
#define MAP_A1_COLD_PLAINS						0x03
#define MAP_A1_STONY_FIELD						0x04
#define MAP_A1_DARK_WOOD						0x05
#define MAP_A1_BLACK_MARSH						0x06
#define MAP_A1_TAMOE_HIGHLAND					0x07
#define MAP_A1_DEN_OF_EVIL						0x08
#define MAP_A1_CAVE_LEVEL_1						0x09 // Cave in Cold plains
#define MAP_A1_UNDERGROUND_PASSAGE_LEVEL_1		0x0a
#define MAP_A1_HOLE_LEVEL_1						0x0b
#define MAP_A1_PIT_LEVEL_1						0x0c
#define MAP_A1_CAVE_LEVEL_2						0x0d // Cave in Cold plains
#define MAP_A1_UNDERGROUND_PASSAGE_LEVEL_2		0x0e
#define MAP_A1_HOLE_LEVEL_2						0x0f
#define MAP_A1_PIT_LEVEL_2						0x10
#define MAP_A1_BURIAL_GROUNDS					0x11
#define MAP_A1_CRYPT							0x12
#define MAP_A1_MAUSOLEUM						0x13
#define MAP_A1_FORGOTTEN_TOWER					0x14
#define MAP_A1_TOWER_CELLAR_LEVEL_1				0x15
#define MAP_A1_TOWER_CELLAR_LEVEL_2				0x16
#define MAP_A1_TOWER_CELLAR_LEVEL_3				0x17
#define MAP_A1_TOWER_CELLAR_LEVEL_4				0x18
#define MAP_A1_TOWER_CELLAR_LEVEL_5				0x19
#define MAP_A1_MONASTERY_GATE					0x1a
#define MAP_A1_OUTER_CLOISTER					0x1b
#define MAP_A1_BARRACKS							0x1c
#define MAP_A1_JAIL_LEVEL_1						0x1d
#define MAP_A1_JAIL_LEVEL_2						0x1e
#define MAP_A1_JAIL_LEVEL_3						0x1f
#define MAP_A1_INNER_CLOISTER					0x20
#define MAP_A1_CATHEDRAL						0x21
#define MAP_A1_CATACOMBS_LEVEL_1				0x22
#define MAP_A1_CATACOMBS_LEVEL_2				0x23
#define MAP_A1_CATACOMBS_LEVEL_3				0x24
#define MAP_A1_CATACOMBS_LEVEL_4				0x25
#define MAP_A1_TRISTRAM							0x26
#define MAP_A1_THE_SECRET_COW_LEVEL				0x27

///////////////////////////////////////////////////
// Act 2 Maps
///////////////////////////////////////////////////
#define MAP_A2_LUT_GHOLEIN						0x28
#define MAP_A2_ROCKY_WASTE						0x29
#define MAP_A2_DRY_HILLS						0x2a
#define MAP_A2_FAR_OASIS						0x2b
#define MAP_A2_LOST_CITY						0x2c
#define MAP_A2_VALLEY_OF_SNAKES					0x2d
#define MAP_A2_CANYON_OF_THE_MAGI				0x2e
#define MAP_A2_SEWERS_LEVEL_1					0x2f
#define MAP_A2_SEWERS_LEVEL_2					0x30
#define MAP_A2_SEWERS_LEVEL_3					0x31
#define MAP_A2_HAREM_LEVEL_1					0x32
#define MAP_A2_HAREM_LEVEL_2					0x33
#define MAP_A2_PALACE_CELLAR_LEVEL_1			0x34
#define MAP_A2_PALACE_CELLAR_LEVEL_2			0x35
#define MAP_A2_PALACE_CELLAR_LEVEL_3			0x36
#define MAP_A2_STONY_TOMB_LEVEL_1				0x37
#define MAP_A2_HALLS_OF_THE_DEAD_LEVEL_1		0x38
#define MAP_A2_HALLS_OF_THE_DEAD_LEVEL_2		0x39
#define MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_1		0x3a
#define MAP_A2_STONY_TOMB_LEVEL_2				0x3b
#define MAP_A2_HALLS_OF_THE_DEAD_LEVEL_3		0x3c
#define MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_2		0x3d
#define MAP_A2_MAGGOT_LAIR_LEVEL_1				0x3e
#define MAP_A2_MAGGOT_LAIR_LEVEL_2				0x3f
#define MAP_A2_MAGGOT_LAIR_LEVEL_3				0x40
#define MAP_A2_ANCIENT_TUNNELS					0x41
#define MAP_A2_TAL_RASHAS_TOMB_1				0x42
#define MAP_A2_TAL_RASHAS_TOMB_2				0x43
#define MAP_A2_TAL_RASHAS_TOMB_3				0x44
#define MAP_A2_TAL_RASHAS_TOMB_4				0x45
#define MAP_A2_TAL_RASHAS_TOMB_5				0x46
#define MAP_A2_TAL_RASHAS_TOMB_6				0x47
#define MAP_A2_TAL_RASHAS_TOMB_7				0x48
#define MAP_A2_TAL_RASHAS_CHAMBER				0x49
#define MAP_A2_ARCANE_SANCTUARY					0x4a

///////////////////////////////////////////////////
// Act 3 Maps
///////////////////////////////////////////////////
#define MAP_A3_KURAST_DOCKS						0x4b
#define MAP_A3_SPIDER_FOREST					0x4c
#define MAP_A3_GREAT_MARSH						0x4d
#define MAP_A3_FLAYER_JUNGLE					0x4e
#define MAP_A3_LOWER_KURAST						0x4f
#define MAP_A3_KURAST_BAZAAR					0x50
#define MAP_A3_UPPER_KURAST						0x51
#define MAP_A3_KURAST_CAUSEWAY					0x52
#define MAP_A3_TRAVINCAL						0x53
#define MAP_A3_ARCHNID_LAIR						0x54
#define MAP_A3_SPIDER_CAVERN					0x55
#define MAP_A3_SWAMPY_PIT_LEVEL_1				0x56
#define MAP_A3_SWAMPY_PIT_LEVEL_2				0x57
#define MAP_A3_FLAYER_DUNGEON_LEVEL_1			0x58
#define MAP_A3_FLAYER_DUNGEON_LEVEL_2			0x59
#define MAP_A3_SWAMPY_PIT_LEVEL_3				0x5a
#define MAP_A3_FLAYER_DUNGEON_LEVEL_3			0x5b
#define MAP_A3_SEWERS_LEVEL_1					0x5c
#define MAP_A3_SEWERS_LEVEL_2					0x5d
#define MAP_A3_RUINED_TEMPLE					0x5e
#define MAP_A3_DISUSED_FANE						0x5f
#define MAP_A3_FORGOTTEN_RELIQUARY				0x60
#define MAP_A3_FORGOTTEN_TEMPLE					0x61
#define MAP_A3_RUINED_FANE						0x62
#define MAP_A3_DISUSED_RELIQUARY				0x63
#define MAP_A3_DURANCE_OF_HATE_LEVEL_1			0x64
#define MAP_A3_DURANCE_OF_HATE_LEVEL_2			0x65
#define MAP_A3_DURANCE_OF_HATE_LEVEL_3			0x66

///////////////////////////////////////////////////
// Act 4 Maps
///////////////////////////////////////////////////
#define MAP_A4_THE_PANDEMONIUM_FORTRESS			0x67
#define MAP_A4_OUTER_STEPPES					0x68
#define MAP_A4_PLAINS_OF_DESPAIR				0x69
#define MAP_A4_CITY_OF_THE_DAMNED				0x6a
#define MAP_A4_RIVER_OF_FLAME					0x6b
#define MAP_A4_THE_CHAOS_SANCTUARY				0x6c

///////////////////////////////////////////////////
// Act 5 Maps
///////////////////////////////////////////////////
#define MAP_A5_HARROGATH						0x6d
#define MAP_A5_THE_BLOODY_FOOTHILLS				0x6e
#define MAP_A5_FRIGID_HIGHLANDS					0x6f
#define MAP_A5_ARREAT_PLATEAU					0x70
#define MAP_A5_CRYSTALLINE_PASSAGE				0x71
#define MAP_A5_FROZEN_RIVER						0x72
#define MAP_A5_GLACIAL_TRAIL					0x73
#define MAP_A5_DRIFTER_CAVERN					0x74
#define MAP_A5_FROZEN_TUNDRA					0x75
#define MAP_A5_THE_ANCIENTS_WAY					0x76
#define MAP_A5_ICY_CELLAR						0x77
#define MAP_A5_ARREAT_SUMMIT					0x78
#define MAP_A5_NIHLATHAKS_TEMPLE				0x79
#define MAP_A5_HALLS_OF_ANGUISH					0x7a
#define MAP_A5_HALLS_OF_PAIN					0x7b
#define MAP_A5_HALLS_OF_VAUGHT					0x7c
#define MAP_A5_ABADDON							0x7d
#define MAP_A5_PIT_OF_ACHERON					0x7e
#define MAP_A5_INFERNAL_PIT						0x7f
#define MAP_A5_WORLDSTONE_KEEP_LEVEL_1			0x80
#define MAP_A5_WORLDSTONE_KEEP_LEVEL_2			0x81
#define MAP_A5_WORLDSTONE_KEEP_LEVEL_3			0x82
#define MAP_A5_THRONE_OF_DESTRUCTION			0x83
#define MAP_A5_WORLDSTONE_KEEP					0x84
#define MAP_A5_MATRONS_DEN						0x85
#define MAP_A5_FORGOTTEN_SANDS					0x86
#define MAP_A5_FURNACE_OF_PAIN					0x87
#define MAP_A5_TRISTRAM							0x88

///////////////////////////////////////////////////
// Item Attributes (From D2jsp scripting document)
///////////////////////////////////////////////////
#define ITEM_IDENTIFIED			0x00000010 // Identified
#define ITEM_SWITCHIN			0x00000040 // Switched in(activated)
#define ITEM_SWITCHOUT			0x00000080 // Switched out(deactivated)
#define ITEM_BROKEN				0x00000100 // Broken(0 durability)
#define ITEM_HASSOCKETS			0x00000800 // Has sockets
#define ITEM_INSTORE			0x00002000 // In npc store or gamble screen
#define ITEM_ISEAR				0x00010000 // Player's ear
#define ITEM_STARTITEM			0x00020000 // Start item(1 selling/repair value)
#define ITEM_COMPACTSAVE		0x00200000 
#define ITEM_ETHEREAL			0x00400000 // Ethreal
#define ITEM_PERSONALIZED		0x01000000 // Personalized
#define ITEM_RUNEWORD			0x04000000 // Runeword

///////////////////////////////////////////////////
// Item Stats
///////////////////////////////////////////////////
#define ITEMSTAT_SOCKETS		0xC2
#define AFFECT_JUST_PORTALED	102
