#pragma once
#include "../../D2Ptrs.h"
#include "../Module.h"
#include "../../Drawing.h"
#include "../../Config.h"
#include "ATIncludes\ArrayEx.h"

typedef struct Vector_t
{
	unsigned int dwType;
	unsigned int Id;
	unsigned int Id2;
	unsigned int Area;
} Vector;

class AutoTele : public Module {
	private:
		std::map<string, Toggle> Toggles;
		unsigned int NextKey, OtherKey, WPKey, PrevKey;
		unsigned int Colors[5];
		Drawing::UITab* settingsTab;

		int Try;
		POINT End;
		DWORD _timer, _timer2, _InteractTimer;
		bool SetInteract, SetTele, CastTele, TeleActive, DoInteract;
		DWORD InteractType, InteractId;
		Room2* InteractRoom;
		DWORD LastArea;
		POINT Vectors[4];
		CArrayEx <POINT, POINT> TPath;

		//functions
		DWORD GetPlayerArea();
		void ManageTele(Vector T);
		int MakePath(int x, int y, DWORD Areas[], DWORD count, bool MoveThrough);
		POINT FindPresetLocation(DWORD dwType, DWORD dwTxtFileNo, DWORD Area);
		bool GetSkill(WORD wSkillId);
		bool SetSkill(WORD wSkillId, bool Left);
		void PrintText(DWORD Color, char *szText, ...);
		bool CastOnMap(WORD x, WORD y, bool Left);
		bool Interact(DWORD UnitId, DWORD UnitType);
		DWORD GetUnitByXY(DWORD X, DWORD Y, Room2* pRoom);
		void GetVectors();
				
	public:
		AutoTele() : Module("AutoTele") {};
		void OnLoad();
		void OnLoop();
		void OnAutomapDraw();
		void OnKey(bool up, BYTE key, LPARAM lParam, bool* block);
		void OnGamePacketRecv(BYTE* packet, bool* block);
		
		static Level* GetLevel(Act* pAct, int level);
		static DWORD GetDistanceSquared(DWORD x1, DWORD y1, DWORD x2, DWORD y2);
};


enum TeleType {
	Next = 0,
	Other,
	WP,
	Prev
};

enum {	PLAYER_MODE_DEATH = 0,				// death
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
PLAYER_MODE_BEING_KNOCKBACK };		// being knocked back

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

enum Type {
	UNIT_PLAYER  =	0,
	UNIT_MONSTER,
	UNIT_OBJECT,
	UNIT_MISSILE,
	UNIT_ITEM,
	UNIT_TILE,
	EXIT,
	XY
};
