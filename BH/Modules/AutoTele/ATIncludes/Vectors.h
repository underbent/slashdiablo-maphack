#include "../../Constants.h"
// there is no line to some bosses unless they are close enough when a character joins the map

Vector vVector[] = {
	{0}, {0}, {0}, {0},										//0x0
	{0}, {0}, {0}, {0},										//0x01

	{EXIT, MAP_A1_COLD_PLAINS}, 
	{EXIT, MAP_A1_DEN_OF_EVIL}, 
	{0}, 
	{EXIT, MAP_A1_ROGUE_ENCAMPMENT},						//0x02

	{EXIT, MAP_A1_STONY_FIELD},								//0x3
  {EXIT_MULTI, MAP_A1_BURIAL_GROUNDS, MAP_A1_CAVE_LEVEL_1},
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_BLOOD_MOOR},

	{EXIT, MAP_A1_UNDERGROUND_PASSAGE_LEVEL_1},
	{UNIT_OBJECT, 22},										//0x4
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_COLD_PLAINS},

	{EXIT, MAP_A1_BLACK_MARSH},								//0x5
	{UNIT_OBJECT, 30},										
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_UNDERGROUND_PASSAGE_LEVEL_1},

	{EXIT, MAP_A1_TAMOE_HIGHLAND},							//0x6
  {EXIT_MULTI, MAP_A1_FORGOTTEN_TOWER, MAP_A1_HOLE_LEVEL_1},
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_DARK_WOOD},

	{EXIT, MAP_A1_MONASTERY_GATE},							//0x7
	{EXIT, MAP_A1_PIT_LEVEL_1},
	{0},
	{EXIT, MAP_A1_BLACK_MARSH},

	{0},													//0x08
	{0}, 
	{0}, 
	{EXIT, MAP_A1_BLOOD_MOOR},								

	{EXIT, MAP_A1_CAVE_LEVEL_2},							//0x9
	{UNIT_MONSTER, 736},									//coldcrow
	{0},
	{EXIT, MAP_A1_COLD_PLAINS},

	{EXIT, MAP_A1_DARK_WOOD},								//0xa
	{EXIT, MAP_A1_UNDERGROUND_PASSAGE_LEVEL_2},
	{UNIT_OBJECT, 157},
	{EXIT, MAP_A1_STONY_FIELD},

	{EXIT, MAP_A1_HOLE_LEVEL_2},							//0xb
  {0},
	{0},
	{EXIT, MAP_A1_BLACK_MARSH},

	{EXIT, MAP_A1_PIT_LEVEL_2},								//0xc
  {0},
	{0},
	{EXIT, MAP_A1_TAMOE_HIGHLAND},

	{UNIT_OBJECT, 397},										//0xd
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_CAVE_LEVEL_1},

  {0},															//0xe
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_UNDERGROUND_PASSAGE_LEVEL_1},


  {0},															//0xf
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_HOLE_LEVEL_1},

  {0},															//0x10
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_PIT_LEVEL_1},

  {0},															//0x11
	{0},
	{0},
	{EXIT, MAP_A1_COLD_PLAINS},

  {0},															//0x12
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_BURIAL_GROUNDS},

  {0},															//0x13
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A1_BURIAL_GROUNDS},

	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_1},					//0x14
  {0},
	{0},
	{EXIT, MAP_A1_BLACK_MARSH},

	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_2},					//0x15
  {0},
	{0},
	{EXIT, MAP_A1_FORGOTTEN_TOWER},


	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_3},					//0x16
  {0},
	{0},
	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_1},


	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_4},					//0x17
  {0},
	{0},
	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_2},


	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_5},					//0x18
  {0},
	{0},
	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_3},


  {0},															//0x19
  {0},
	{0},
	{EXIT, MAP_A1_TOWER_CELLAR_LEVEL_4},

	{EXIT, MAP_A1_OUTER_CLOISTER},							//0x1a
  {0},
	{0},
	{XY, 15141, 5091, MAP_A1_TAMOE_HIGHLAND},


	{EXIT, MAP_A1_BARRACKS},								//0x1b
  {0},
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_MONASTERY_GATE},


	{EXIT, MAP_A1_JAIL_LEVEL_1},							//0x1c
	{UNIT_OBJECT, 108},
	{0},
	{EXIT, MAP_A1_OUTER_CLOISTER},


	{EXIT, MAP_A1_JAIL_LEVEL_2},							//0x1d
  {0},
	{UNIT_OBJECT, 157},
	{EXIT, MAP_A1_BARRACKS},


	{EXIT, MAP_A1_JAIL_LEVEL_3},							//0x1e
  {0},
	{0},
	{EXIT, MAP_A1_JAIL_LEVEL_1},


	{EXIT, MAP_A1_INNER_CLOISTER},							//0x1f
  {0},
	{0},
	{EXIT, MAP_A1_JAIL_LEVEL_2},


	{EXIT, MAP_A1_CATHEDRAL},								//0x20
  {0},
	{UNIT_OBJECT, 119},
	{EXIT, MAP_A1_JAIL_LEVEL_3},


	{EXIT, MAP_A1_CATACOMBS_LEVEL_1},						//0x21
  {0},
	{0},
	{EXIT, MAP_A1_INNER_CLOISTER},


	{EXIT, MAP_A1_CATACOMBS_LEVEL_2},						//0x22
  {0},
	{0},
	{EXIT, MAP_A1_CATHEDRAL},


	{EXIT, MAP_A1_CATACOMBS_LEVEL_3},						//0x23
  {0},
	{UNIT_OBJECT, 157},
	{EXIT, MAP_A1_CATACOMBS_LEVEL_1},


	{EXIT, MAP_A1_CATACOMBS_LEVEL_4},						//0x24
  {0},
	{0},
	{EXIT, MAP_A1_CATACOMBS_LEVEL_2},


	{XY, 22533, 9556},										//0x25
  {0},
	{0},
	{EXIT, MAP_A1_CATACOMBS_LEVEL_3},

	{0}, {0}, {0}, {0},										//0x26 -> tristram
	{0}, {0}, {0}, {0},										//0x27 -> cow lvl


	////////////////////////////////////////////////////////////////////
	// Act 2
	////////////////////////////////////////////////////////////////////

	{0}, {0}, {0}, {0},										//0x28 -> lut gholein

	{EXIT, MAP_A2_DRY_HILLS},								//0x29
  {0},
	{0},
	{EXIT, MAP_A2_LUT_GHOLEIN},

	{EXIT, MAP_A2_FAR_OASIS},								//0x2a
	{EXIT, MAP_A2_HALLS_OF_THE_DEAD_LEVEL_1},
	{UNIT_OBJECT, 156},
	{EXIT, MAP_A2_ROCKY_WASTE},

	{EXIT, MAP_A2_LOST_CITY},								//0x2b
	{EXIT, MAP_A2_MAGGOT_LAIR_LEVEL_1},
	{UNIT_OBJECT, 156},
	{EXIT, MAP_A2_DRY_HILLS},

	{EXIT, MAP_A2_VALLEY_OF_SNAKES},						//0x2c
	{EXIT, MAP_A2_ANCIENT_TUNNELS},
	{UNIT_OBJECT, 156},
	{EXIT, MAP_A2_FAR_OASIS},

	{EXIT, MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_1},				//0x2d
  {0},
	{0},
	{EXIT, MAP_A2_LOST_CITY},

	{0},													//0x2e -> canyon of the magi
	{UNIT_OBJECT, 402},	// hack to make tomb path 'next' colored
	{UNIT_OBJECT, 402},										
	{UNIT_OBJECT, 402},


	{EXIT, MAP_A2_SEWERS_LEVEL_2},							//0x2f
  {0},
	{0},
	{EXIT, MAP_A2_LUT_GHOLEIN},

	{EXIT, MAP_A2_SEWERS_LEVEL_3},							//0x30
  {0},
	{UNIT_OBJECT, 323},
	{EXIT, MAP_A2_SEWERS_LEVEL_1},

	{UNIT_OBJECT, 355},										//0x31
	{0},
	{0},
	{EXIT, MAP_A2_SEWERS_LEVEL_2},

	{UNIT_TILE, 29},										//0x32
  {0},
	{0},
	{EXIT, MAP_A2_LUT_GHOLEIN},

	{EXIT, MAP_A2_PALACE_CELLAR_LEVEL_1},					//0x33
  {0},
	{0},
	{EXIT, MAP_A2_HAREM_LEVEL_1},

	{EXIT, MAP_A2_PALACE_CELLAR_LEVEL_2},					//0x34
  {0},
	{UNIT_OBJECT, 288},
	{EXIT, MAP_A2_HAREM_LEVEL_2},

	{EXIT, MAP_A2_PALACE_CELLAR_LEVEL_3},					//0x35
  {0},
	{0},
	{EXIT, MAP_A2_PALACE_CELLAR_LEVEL_1},

	{UNIT_OBJECT, 298},										//0x36
  {0},
	{0},
	{EXIT, MAP_A2_PALACE_CELLAR_LEVEL_2},

	{EXIT, MAP_A2_STONY_TOMB_LEVEL_2},						//0x37 -> stony tomb
  {0},
	{0},
	{EXIT, MAP_A2_ROCKY_WASTE},

	{EXIT, MAP_A2_HALLS_OF_THE_DEAD_LEVEL_2},				//0x38
  {0},
	{0},
	{EXIT, MAP_A2_DRY_HILLS},

	{EXIT, MAP_A2_HALLS_OF_THE_DEAD_LEVEL_3},				//0x39
  {0},
	{UNIT_OBJECT, 156},
	{EXIT, MAP_A2_HALLS_OF_THE_DEAD_LEVEL_1},

	{EXIT, MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_2},				//0x3a
  {0},
	{0},
	{EXIT, MAP_A2_VALLEY_OF_SNAKES},

  {0},														//0x3b -> stony tomb lvl 2
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A2_STONY_TOMB_LEVEL_1},

  {0},														//0x3c
	{UNIT_OBJECT, 354},
	{0},
	{EXIT, MAP_A2_HALLS_OF_THE_DEAD_LEVEL_2},

  {0},														//0x3d
	{UNIT_OBJECT, 149},
	{0},
	{EXIT, MAP_A2_CLAW_VIPER_TEMPLE_LEVEL_1},

	{EXIT, MAP_A2_MAGGOT_LAIR_LEVEL_2},						//0x3e
  {0},
	{0},
	{EXIT, MAP_A2_FAR_OASIS},

	{EXIT, MAP_A2_MAGGOT_LAIR_LEVEL_3},						//0x3f
  {0},
	{0},
	{EXIT, MAP_A2_MAGGOT_LAIR_LEVEL_1},

  {0},														//0x40
	{UNIT_OBJECT, 356},
	{0},
	{EXIT, MAP_A2_MAGGOT_LAIR_LEVEL_2},

  {0},														//0x41 ancient tunnels
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A2_LOST_CITY},

	{UNIT_OBJECT, 152},										//0x42
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x43
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x44
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x45
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x46
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x47
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},


	{UNIT_OBJECT, 152},										//0x48										
  {0},
	{0},
	{EXIT, MAP_A2_CANYON_OF_THE_MAGI},

	{0}, {0}, {0}, {0},										//0x49 -> tal chambre

	{UNIT_OBJECT, 357},										//0x4a
  {0},
	{UNIT_OBJECT, 402},
	{UNIT_OBJECT, 298},

	////////////////////////////////////////////////////////////////////
	// Act 3
	////////////////////////////////////////////////////////////////////

	{0}, {0}, {0}, {0},										//0x4b -> kurast docks


	{EXIT_MULTI, MAP_A3_GREAT_MARSH, MAP_A3_FLAYER_JUNGLE},								//0x4c
	{EXIT, MAP_A3_SPIDER_CAVERN},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_KURAST_DOCKS},


	{EXIT, MAP_A3_FLAYER_JUNGLE},							//0x4d
  {0},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_SPIDER_FOREST},


	{EXIT, MAP_A3_LOWER_KURAST},							//0x4e
	{UNIT_OBJECT, 252},
	{UNIT_OBJECT, 237},
	{EXIT_MULTI, MAP_A3_GREAT_MARSH, MAP_A3_SPIDER_FOREST},


	{EXIT, MAP_A3_KURAST_BAZAAR},							//0x4f
  {0},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_FLAYER_JUNGLE},


	{EXIT, MAP_A3_UPPER_KURAST},							//0x50
	{EXIT, MAP_A3_RUINED_TEMPLE},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_LOWER_KURAST},


	{EXIT, MAP_A3_KURAST_CAUSEWAY},							//0x51
  {0},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_KURAST_BAZAAR},


	{EXIT, MAP_A3_TRAVINCAL},								//0x52
  {0},
	{0},
	{EXIT, MAP_A3_UPPER_KURAST},


	{EXIT, MAP_A3_DURANCE_OF_HATE_LEVEL_1},					//0x53
  {0},
	{UNIT_OBJECT, 237},
	{EXIT, MAP_A3_KURAST_CAUSEWAY},

  {0},																					//0x54 -> arachnid lair
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A3_SPIDER_FOREST},

  {0},																					//0x55 -> spider cavern
	{UNIT_OBJECT, 407},
	{0},
	{EXIT, MAP_A3_SPIDER_FOREST},

	{EXIT, MAP_A3_SWAMPY_PIT_LEVEL_2},						//0x56 -> swampy pit lvl 1
  {0},
	{0},
	{EXIT, MAP_A3_FLAYER_JUNGLE},

	{EXIT, MAP_A3_SWAMPY_PIT_LEVEL_3},						//0x57 -> swampy pit lvl 2
  {0},
	{0},
	{EXIT, MAP_A3_SWAMPY_PIT_LEVEL_1},

	{EXIT, MAP_A3_FLAYER_DUNGEON_LEVEL_2},					//0x58 -> flayer dungeon lvl 1
  {0},
	{0},
	{EXIT, MAP_A3_FLAYER_JUNGLE},

	{EXIT, MAP_A3_FLAYER_DUNGEON_LEVEL_3},					//0x59 -> flayer dungeon lvl 2
  {0},
	{0},
	{EXIT, MAP_A3_FLAYER_DUNGEON_LEVEL_1},

  {0},														//0x5a -> swampy pit lvl 3
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A3_SWAMPY_PIT_LEVEL_2},

  {0},																						//0x5b -> flayer dungeon lvl 3
	{UNIT_OBJECT, 406},
	{0},
	{EXIT, MAP_A3_FLAYER_DUNGEON_LEVEL_2},

	{EXIT, MAP_A3_SEWERS_LEVEL_2},							//0x5c -> sewer lvl 1
  {0},
	{0},
	{0},

  {0},																				//0x5d -> sewer lvl 2
	{UNIT_OBJECT, 405},
	{0},
	{EXIT, MAP_A3_SEWERS_LEVEL_1},

  {0},																				//0x5e -> ruined temple
	{UNIT_OBJECT, 193},
	{0},
	{EXIT, MAP_A3_KURAST_BAZAAR},

	{0}, {0}, {0}, {0},										//0x5f -> disused fane
	{0}, {0}, {0}, {0},										//0x60 -> forgotten reliquary
	{0}, {0}, {0}, {0},										//0x61 -> forgotton temple
	{0}, {0}, {0}, {0},										//0x62 -> ruined fane
	{0}, {0}, {0}, {0},										//0x63 -> disused reliquary

	{EXIT, MAP_A3_DURANCE_OF_HATE_LEVEL_2},					//0x64
  {0},
	{0},
	{EXIT, MAP_A3_TRAVINCAL},

	{EXIT, MAP_A3_DURANCE_OF_HATE_LEVEL_3},					//0x65
  {0},
	{UNIT_OBJECT, 324},
	{EXIT, MAP_A3_DURANCE_OF_HATE_LEVEL_1},

	{XY, 17591, 8069},										//0x66
  {0},
	{0},
	{EXIT, MAP_A3_DURANCE_OF_HATE_LEVEL_2},


	////////////////////////////////////////////////////////////////////
	// Act 4
	////////////////////////////////////////////////////////////////////

	{0}, {0}, {0}, {0},										//0x67 pandemonium fortress


	{EXIT, MAP_A4_PLAINS_OF_DESPAIR},						//0x68
  {0},
	{0},
	{EXIT, MAP_A4_THE_PANDEMONIUM_FORTRESS},


	{EXIT, MAP_A4_CITY_OF_THE_DAMNED},						//0x69
	{UNIT_MONSTER, 256},									//izual
	{UNIT_OBJECT, 238},
	{EXIT, MAP_A4_OUTER_STEPPES},


	{EXIT, MAP_A4_RIVER_OF_FLAME},							//0x6a
  {0},
	{UNIT_OBJECT, 238},
	{EXIT, MAP_A4_PLAINS_OF_DESPAIR},


	{UNIT_OBJECT, 255, NULL, MAP_A4_THE_CHAOS_SANCTUARY},	//0x6b
	{UNIT_OBJECT, 376},
	{UNIT_OBJECT, 238},
	{EXIT, MAP_A4_CITY_OF_THE_DAMNED},


	{UNIT_OBJECT, 392, 1337},								//0x6c
	{UNIT_OBJECT, 255},
	{0},
	{EXIT, MAP_A4_RIVER_OF_FLAME},


	////////////////////////////////////////////////////////////////////
	// Act 5
	////////////////////////////////////////////////////////////////////

	{0}, {0}, {0}, {0},										//0x6d

	{EXIT, MAP_A5_FRIGID_HIGHLANDS},						//0x6e
	{UNIT_MONSTER, 479},									//shenk
	{0},
	{EXIT, MAP_A5_HARROGATH},

	{EXIT, MAP_A5_ARREAT_PLATEAU},							//0x6f
	{UNIT_OBJECT, 60},                          // red portal
	{UNIT_OBJECT, 496},
	{EXIT, MAP_A5_THE_BLOODY_FOOTHILLS},

	{EXIT, MAP_A5_CRYSTALLINE_PASSAGE},						//0x70
	{UNIT_OBJECT, 60},                          // red portal
	{UNIT_OBJECT, 496},
	{EXIT, MAP_A5_FRIGID_HIGHLANDS},

	{EXIT, MAP_A5_GLACIAL_TRAIL},							//0x71
	{EXIT, MAP_A5_FROZEN_RIVER},
	{UNIT_OBJECT, 511},
	{EXIT, MAP_A5_ARREAT_PLATEAU},

  {0},																			//0x72
	{UNIT_OBJECT, 460},
	{0},
	{EXIT, MAP_A5_CRYSTALLINE_PASSAGE},

	{EXIT, MAP_A5_FROZEN_TUNDRA},							//0x73
	{EXIT, MAP_A5_DRIFTER_CAVERN},
	{UNIT_OBJECT, 511},
	{EXIT, MAP_A5_CRYSTALLINE_PASSAGE},

  {0},																			//0x74 -> drifter cavern
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A5_GLACIAL_TRAIL},

	{EXIT, MAP_A5_THE_ANCIENTS_WAY},						//0x75
	{UNIT_OBJECT, 60},                          // red portal
	{UNIT_OBJECT, 496},
	{EXIT, MAP_A5_GLACIAL_TRAIL},

	{EXIT, MAP_A5_ARREAT_SUMMIT},							//0x76
	{EXIT, MAP_A5_ICY_CELLAR},
	{UNIT_OBJECT, 511},
	{EXIT, MAP_A5_FROZEN_TUNDRA},

  {0},																			//0x77 -> icy cellar
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A5_THE_ANCIENTS_WAY},

	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_1},					//0x78
  {0},
	{0},
	{EXIT, MAP_A5_THE_ANCIENTS_WAY},

	{EXIT, MAP_A5_HALLS_OF_ANGUISH},						//0x79
  {0},
	{0},
	{0},

	{EXIT, MAP_A5_HALLS_OF_PAIN},							//0x7a
  {0},
	{0},
	{EXIT, MAP_A5_NIHLATHAKS_TEMPLE},

	{EXIT, MAP_A5_HALLS_OF_VAUGHT},							//0x7b
  {0},
	{UNIT_OBJECT, 496},
	{EXIT, MAP_A5_HALLS_OF_ANGUISH},

	{UNIT_OBJECT, 462},										//0x7c
  {0},
	{0},
	{EXIT, MAP_A5_HALLS_OF_PAIN},

  {0},																	//0x7d -> abaddon
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A5_FRIGID_HIGHLANDS},

  {0},																	//0x7e -> pit of acheron
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A5_ARREAT_PLATEAU},

	{0},																	//0x7f -> infernal pit
	{UNIT_OBJECT, 397},
	{0},
	{EXIT, MAP_A5_FROZEN_TUNDRA},

	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_2},					//0x80
  {0},
	{0},
	{EXIT, MAP_A5_ARREAT_PLATEAU},

	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_3},					//0x81
  {0},
	{UNIT_OBJECT, 494},
	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_1},

	{EXIT, MAP_A5_THRONE_OF_DESTRUCTION},					//0x82
  {0},
	{0},
	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_2},

  {0},																					//0x83
	{XY, 15114, 5069},
	{0},
	{EXIT, MAP_A5_WORLDSTONE_KEEP_LEVEL_3},

	{0}, {0}, {0}, {0},									//0x84 -> worldstone keep

	{0},													//0x85
	{UNIT_MONSTER, 707},					//lilith
	{0},
	{0},

	{0},													//0x86 -> forgotton sands
	{UNIT_MONSTER, 708},					//uber duriel
	{0},
	{0},

	{0},													//0x87
	{UNIT_MONSTER, 706},					//uber izual
	{0},
	{0},

	{0},													//0x88 uber trist
	{0},
	{0},
	{0}
};
