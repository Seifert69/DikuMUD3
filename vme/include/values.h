/*
filename    values
password    guildmaster
changedby   Darg
EmailAdd    marco2@cyberzone.net
request     header
version     2
END HEADER*/

#include  "vme.h"
#ifndef _MUD_VALUES_H
#define _MUD_VALUES_H
/* *********************************************************************** *
 * File   : values.h                                  Part of Valhalla MUD *
 * Version: 2                                                           *
 * Author : YOU.                                                           *
 *                                                                         *
 * Purpose: Definitions for Admin definable aspects of the game.                  *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Published.                                                     *
 *                                                                         *
 * Copyright (C) 1994 - 1996 by Valhalla (This work is published).         *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This work is copyrighted. No part of this work may be copied,           *
 * reproduced, translated or reduced to any medium without the prior       *
 * written consent of Valhalla.                                            *
 * *********************************************************************** */

 #define IS_CORPSE(ITEM) \
   ((ITEM.nameidx=="corpse") and (ITEM.zoneidx=="death"))


 #define IS_HEAD(ITEM) \
   ((ITEM.nameidx=="head") and (ITEM.zoneidx=="death"))


#define TYPE_SOCIAL 0x0001
#define TYPE_SKILL 0x0002
/*
	*	Pc Races:
	*
	*	when changing the Pc races make sure you make  adjustments to races.def
	*	and to abilities.def as necessary.
	*
*/

#define ITEM_SKIN   26
#define ITEM_BOARD   27

#define POLICE_ACADEMY    "$Police Member"
#define POLICE_BANNED     "$Police Banned"

#define GUILD_UDG_FIGHTER     "Udgaard Fighter"
#define GUILD_UDG_FIGHTER_ATL "jones@fightersguild","lancelot@fightersguild","robin@fightersguild" //  Abi, Wpn, Ski

#define GUILD_KNIGHT      "Midgaard Dark Knight"
#define GUILD_KNIGHT_ATL  "matthew@knight", "aramand@knight", "hilary@knight", "soroth@knight" //  Abi, Wpn, Spl, Ski

#define GUILD_PALADIN     "Midgaard Paladin"
#define GUILD_PALADIN_ATL  "joan@paladin_guild", "edmund@paladin_guild", "alex@paladin_guild", "corbin@paladin_guild" //  Abi, Wpn, Spl, Ski

#define GUILD_NYM_RANGER  "Nymgaard Ranger"
#define GUILD_RANGER_ATL  "mountain_man@ranger", "justifier@ranger", "guardian@ranger", "pathfinder@ranger" //  Abi, Wpn, Spl, Ski

#define GUILD_UDG_THIEF   "Udgaard Thief"
#define GUILD_THIEF_ATL   "bisse@thievesguild","pirate@thievesguild","gryma@thievesguild" //  Abi, Wpn, Ski

#define GUILD_ASSASSIN     "Khorsabad Assassin"
#define GUILD_ASSASSIN_ATL "ability_trainer@assassin", "weapon_trainer@assassin", "spell_trainer@assassin", "skill_trainer@assassin" //  Abi, Wpn, Spl, Ski

#define GUILD_UDG_CLERIC  "Udgaard Healer"
#define GUILD_CLERIC_ATL  "tack@clericguild", "tick@clericguild", "isabella@clericguild", "bartholomew@clericguild" //  Abi, Wpn, Spl, Ski

#define GUILD_NYM_DRUID   "Nymgaard Druid"
#define GUILD_DRUID_ATL   "oak@druid", "elm@druid", "maple@druid", "willow@druid" //  Abi, Wpn, Spl, Ski

#define GUILD_MYSTIC      "Midgaard Mystic"
#define GUILD_MYSTIC_ATL  "", "", "", "" //  Abi, Wpn, Spl, Ski

#define GUILD_NECROMANCER "Necromancer"
#define GUILD_NECROMANCER_ATL  "kadiya@necro_guild", "krain@necro_guild", "murdock@necro_guild", "meclor@necro_guild" //  Abi, Spl, Wpn, Ski


#define GUILD_UDG_MAGE    "Udgaard Conjurer"
#define GUILD_MAGE_ATL    "merlin@mageguild", "zofax@mageguild", "caval@mageguild", "fifiz@mageguild" //  Abi, Spl, Wpn, Ski

#define GUILD_SORCERER     "Midgaard Sorcerer"
#define GUILD_SORCERER_ATL "calastran@sorcerer", "lasheetah@sorcerer","claudius@sorcerer", "forbin@sorcerer" // Ability, Weapon, Spell, Skill 


#define GUILD_SL   {GUILD_UDG_FIGHTER, GUILD_KNIGHT, GUILD_PALADIN, GUILD_NYM_RANGER,   \
	                GUILD_UDG_THIEF, GUILD_ASSASSIN, GUILD_UDG_CLERIC, GUILD_NYM_DRUID, \
	                GUILD_MYSTIC, GUILD_NECROMANCER, GUILD_UDG_MAGE, GUILD_SORCERER}
#define GUILD_INIT_SL  {"F", "DK", "P",  "R", "T", "A", "H", "D", "M", "N",  "C", "S"}


//
// Possibly have fewer professions.
//    4 core professions, fighter, mage, priest, thief
//   and then the well known hybrids:
//      fighter/mage, fighter/priest, fighter/thief, mage/thief
//  Depends how you want to structure the game vs. the rules really.            
//   
#define PROFESSION_FIGHTER      0
#define PROFESSION_KNIGHT       1
#define PROFESSION_PALADIN      2
#define PROFESSION_RANGER       3
#define PROFESSION_THIEF        4
#define PROFESSION_ASSASSIN     5
#define PROFESSION_PRIEST       6
#define PROFESSION_DRUID        7
#define PROFESSION_MYSTIC       8 
#define PROFESSION_NECROMANCER  9
#define PROFESSION_CONJURER    10
#define PROFESSION_SORCERER    11
#define PROFESSION_BARBARIAN   12
#define PROFESSION_MAX         13 /* 11+1 */

#define PROFESSION_STRINGS "Fighter", "Knight", "Paladin", "Ranger", "Thief", "Assassin", \
                           "Priest", "Druid", "Mystic","Necromancer", "Conjurer", "Mage", "Barbarian"


#define QUEST_WW              "Wight warrens quest"
#define QUEST_WELMAR          "King Welmar Quest"
#define QUEST_RABBIT_ONGOING  "Rabbit Stew Ongoing"
#define QUEST_RABBIT_COMPLETE "Rabbit Stew Complete"
#define QUEST_POT_COMPLETE    "Mary's Pot Retrieved"

#define MIN_HUMANOID_PC      0
#define RACE_HUMAN           0     /* PC race */
#define RACE_ELF             1     /* PC race */
#define RACE_DWARF           2     /* PC race */
#define RACE_HALFLING        3     /* PC race */
#define RACE_GNOME           4     /* PC race */
#define RACE_HALF_ORC        5
#define RACE_HALF_OGRE       6
#define RACE_HALF_ELF        7
#define RACE_BROWNIE         8
#define RACE_GROLL           9
#define RACE_DARK_ELF       10
#define MAX_HUMANOID_PC     100



/*Insert any new definitions here... */
/*check values.h for max number of races.*/
/* keep in mind if you need more ask the vme coders.*/

/*
	*	Weapon defines:
	*
	*	When changing the weapons defines make sure you  remember to change
	*	the weapons.def and messages files.
	*
*/

#define WPN_BATTLE_AXE    7  /* Two Handed Axe */
#define WPN_HAND_AXE      8  /* One Handed Axe */

#define WPN_WAR_MATTOCK   9  /* Two Handed mattock */
#define WPN_ADZE         76  /* One handed "mattock" */

#define WPN_WAR_MAUL     54  /* Two handed hammer, maul, sledge hammer */
#define WPN_WAR_HAMMER   10  /* One handed hammer, e.g. Mjolnir */



#define WPN_BATTLE_CLUB  23  /* Two handed club */
#define WPN_CLUB         24  /* One Handed club */

#define WPN_BATTLE_MACE  21  /* Two Handed mace (crushing) */
#define WPN_MACE         22  /* One Handed mace */

#define WPN_FLAIL        26  /* Two handed (ball and chain) */
#define WPN_MORNING_STAR 25  /* One handed (ball and chain). Fantasy weapon. often called (flail) morning star */

//#define WPN_MORNING_STAR 91  /* One Handed (mace with spikes) */
                             // Are we missing a two handed morning star (or spiked club)?


#define WPN_GREAT_SWORD  11  /* Two Handed Sword */
#define WPN_LONG_SWORD   17  /* One handed sword */
#define WPN_SCIMITAR     12  /* One handed, slim curved blade */
#define WPN_FALCHION     14  /* One handed, broad curved blade */
#define WPN_SHORT_SWORD  19
#define WPN_RAPIER       18  // Pointy, piercing
#define WPN_MAIN_GAUCHE  73
#define WPN_STILETTO     97
#define WPN_DAGGER       20
#define WPN_SICKLE       31  /* One handed, slashing */

#define WPN_HALBERD      29  /* Two handed polearm club (crush) */
#define WPN_BARDICHE     30  /* Two handed polearm axe (slash)*/ 
#define WPN_SCYTHE       32  /* Two handed (grim reaper) */
#define WPN_QUARTERSTAFF 27  /* Two handed */
#define WPN_SPEAR        28  /* Two handed Pole arm (pierce) */
#define WPN_JAVELIN      85  /* Mostly designed to be thrown */
#define WPN_TRIDENT      33
#define WPN_LANCE        49


#define WPN_FIST         34
#define WPN_KICK         35
#define WPN_BITE         36
#define WPN_STING        37
#define WPN_CLAW         38
#define WPN_CRUSH        39
#define WPN_KNEE         55
#define WPN_ELBOW        56
#define WPN_KNUCKLES     99  // metal on your knuckles

#define WPN_BOW          42 /* Here down to Staff are Rangers Guild Jan 98 */
#define WPN_CROSSBOW     43
#define WPN_SLING        44
#define WPN_WHIP         40

// MISSING WEAPONS ?

// Heavy / light Xbox (we already have one)
// Short / long / composite bow (we already have one)
// Shuriken. 
// Darts. 
// Blow-dart. 
// Bola.
// Iron glove (armored fist)


// Map the old surplus weapons to the more generic weapon types.
// E.g. the broad sword is the same skill category as long sword.

#define WPN_BROAD_SWORD  WPN_LONG_SWORD
#define WPN_LUCERN_HAMMER WPN_HALBERD
#define WPN_KATANA WPN_SCIMITAR
#define WPN_WAKIZASHI WPN_SCIMITAR
#define WPN_NAGINATA WPN_BARDICHE
#define WPN_BASTARD_SWORD1 WPN_LONG_SWORD
#define WPN_BASTARD_SWORD2 WPN_LONG_SWORD
#define WPN_HAND_HAMMER WPN_WAR_HAMMER
#define WPN_KOPESH WPN_HAND_AXE
#define WPN_FLAMBERGE WPN_LONG_SWORD
#define WPN_MACHETE WPN_SHORT_SWORD
#define WPN_PIKE WPN_SPEAR
#define WPN_WAR_AXE WPN_BATTLE_AXE
#define WPN_GAFF WPN_JAVELIN
#define WPN_CUTLASS WPN_STILETTO
#define WPN_GREAT_AXE WPN_BATTLE_AXE
#define WPN_SABER WPN_SCIMITAR
#define WPN_LOCHABER_AXE WPN_HALBERD
#define WPN_BALL_CHAIN WPN_MORNING_STAR
#define WPN_FOOT_LANCE WPN_LANCE
#define WPN_POLE_AXE WPN_BARDICHE
#define WPN_CLAYMORE WPN_LONG_SWORD
#define WPN_DIRK WPN_DAGGER
#define WPN_ICE_PICK WPN_HAND_AXE
#define WPN_RANSEUR WPN_SPEAR
#define WPN_TALWAR WPN_SCIMITAR
#define WPN_ESTOC WPN_LONG_SWORD
#define WPN_VOULGE WPN_BARDICHE
#define WPN_FISH_SPINE WPN_LONG_SWORD
#define WPN_SHOCK_LANCE WPN_LANCE
#define WPN_BILL_GUISARME WPN_HALBERD
#define WPN_JEDDART_AXE WPN_BARDICHE
#define WPN_BEC_DE_CORBIN WPN_HALBERD
#define WPN_FAUCHARD_FORK WPN_BARDICHE
#define WPN_SHAMSHIR WPN_SCIMITAR
#define WPN_SAPPARA WPN_LONG_SWORD
#define WPN_POLE_CLEAVER WPN_BARDICHE
#define WPN_CROWBILL WPN_HALBERD
#define WPN_CLEAVER WPN_HAND_AXE
#define WPN_MILITARY_FORK WPN_TRIDENT
#define WPN_FIGHTING_STAFF WPN_QUARTERSTAFF
#define WPN_STAFF WPN_QUARTERSTAFF


#define LAST_WEAPON       101

// #define WPN_HAND_HAMMER  72  
///#define WPN_KATANA       13
///#define WPN_KOPESH       15
///#define WPN_BROAD_SWORD  16
///#define WPN_WAKIZASHI    41
/// #define WPN_FIGHTING_STAFF 45 /* Two handed */
/// #define WPN_SABER        46
/// #define WPN_CUTLASS      47
/// #define WPN_MACHETE      48
///#define WPN_SHOCK_LANCE  50
/// #define WPN_PIKE         51
///#define WPN_GREAT_AXE    52
///#define WPN_CLAYMORE     53
///#define WPN_FAUCHARD     57
///#define WPN_SAPPARA      58
///#define WPN_POLE_CLEAVER 59
///#define WPN_CROWBILL     60
/// #define WPN_ESTOC        61
///#define WPN_BILL_GUISARME 62
///#define WPN_GUISARME     63
///#define WPN_NAGINATA     64
///#define WPN_SPETUM       65
///#define WPN_LUCERN_HAMMER 66
///#define WPN_BASTARD_SWORD2 67 /* SPECIAL USE FLAMETONGUE AND AVENGER */
///#define WPN_BASTARD_SWORD1 68   /* DITTO */
/// #define WPN_PARTISAN       69
///#define WPN_PILUM          70
///#define WPN_JEDDART_AXE    71
///#define WPN_COMMON_MAUL    74
///#define WPN_POLE_AXE       75
///#define WPN_ADZE           76
///#define WPN_STAFF          77
///#define WPN_BACULUS        78
///#define WPN_FISH_SPINE     79
///#define WPN_GAFF           80
/// #define WPN_FLAMBERGE      81
/// #define WPN_BEC_DE_CORBIN  82
/// #define WPN_VOULGE         83
///#define WPN_WAR_AXE        84
///#define WPN_MILITARY_FORK  86
/// #define WPN_ICE_PICK       87
/// #define WPN_DIRK           88
/// #define WPN_SHAMSHIR       89
/// #define WPN_FAUCHARD_FORK  90
///#define WPN_CUDGEL         92
///#define WPN_RANSEUR        93
///#define WPN_FOOT_LANCE     94
/// #define WPN_BILL_HOOK      95
///#define WPN_TALWAR         96
/// #define WPN_CLEAVER        98
/// #define WPN_LOCHABER_AXE  100
/// #define WPN_GLAIVE        101
/*check vme.h for max number of weapons.*/
/* keep in mind if you need more ask the vme coders.*/

/*
	*	Skill defines:
	*
	*	Remember if you add to the skills defines you need to make the
	*	necesary changes to skills.def and to the cmd_* defines in this file.
	*
*/
#define SKI_TURN_UNDEAD        0
#define SKI_SCROLL_USE         1
#define SKI_WAND_USE           2
#define SKI_CONSIDER           3
#define SKI_DIAGNOSTICS        4
#define SKI_APPRAISAL          5
#define SKI_VENTRILOQUATE      6
#define SKI_WEATHER_WATCH      7
#define SKI_FLEE               8
#define SKI_SNEAK              9
#define SKI_BACKSTAB          10
#define SKI_HIDE              11
#define SKI_FIRST_AID         12
#define SKI_PICK_LOCK         13
#define SKI_STEAL             14
#define SKI_RESCUE            15
#define SKI_SEARCH            16
#define SKI_LEADERSHIP        17
#define SKI_KICK              18
#define SKI_SWIMMING          19
#define SKI_BASH              20
#define SKI_CLIMB             21
#define SKI_SHIELD            22
#define SKI_TRIP              23
#define SKI_DUAL_WIELD        24
#define SKI_CUFF              25
#define SKI_RESIZE_CLOTHES    26
#define SKI_RESIZE_LEATHER    27
#define SKI_RESIZE_METAL      28
#define SKI_EVALUATE          29 /* "Fake skill to simulate combinations */
#define SKI_PEEK              30
#define SKI_PICK_POCKETS      31
#define SKI_FILCH             32
#define SKI_DISARM            33
#define SKI_SKIN              34
#define SKI_SHELTER           35
#define SKI_SOOTHE            36
#define SKI_AMBUSH            37
#define SKI_CURARE            38
#define SKI_FASHION           39
#define SKI_BUTCHER           40
#define SKI_LAY_TRAP          41
#define SKI_SHOOT             42
#define SKI_HERBS             43
#define SKI_FORAGE            44
#define SKI_DOWSE             45
#define SKI_TRACK             46
#define SKI_HUNT              47
#define SKI_THROW             48
#define SKI_COOK              49
#define SKI_SCAN              50
#define SKI_SLIP              51
#define SKI_PALM              52
#define SKI_PLANT             53
#define SKI_STALK             54
#define SKI_KNEE              55
#define SKI_ELBOW             56
#define SKI_HIT               57
#define SKI_PUNCH             58
#define SKI_GLANCE            59
#define SKI_ARM_LEATHER       60
#define SKI_ARM_HLEATHER      61
#define SKI_ARM_CHAIN         62
#define SKI_ARM_PLATE         63
#define SKI_FRENZY			  64
#define SKI_BERSERK		      65
#define SKI_RAGE              66
#define SKI_TAUNT             67

#define LAST_SKILL            75



/* Insert any new definitions here... */
/*check values.h for max number of weapons.*/
/* keep in mind if you need more ask the vme coders.*/

/*
	*	Spell defines:
	*
	*	Remember when adding or removing spells from the defines you also
	*	need to change spells.def and recompile the zone with the spells in it.
	*
*/

#define SPL_CALL_LIGHTNING    12  /* Cell Group  */
#define SPL_BLESS             13  /* D I V I N E */
#define SPL_CURSE             14
#define SPL_REMOVE_CURSE      15
#define SPL_CURE_WOUNDS_1     16
#define SPL_CURE_WOUNDS_2     17
#define SPL_CURE_WOUNDS_3     18
#define SPL_CAUSE_WOUNDS_1    19
#define SPL_CAUSE_WOUNDS_2    20
#define SPL_CAUSE_WOUNDS_3    21
#define SPL_DISPEL_EVIL       22
#define SPL_REPEL_UNDEAD_1    23
#define SPL_REPEL_UNDEAD_2    24
#define SPL_BLIND             25
#define SPL_CURE_BLIND        26
#define SPL_LOCATE_OBJECT     27
#define SPL_LOCATE_CHAR       28

#define SPL_RAISE_MAG         29  /* P R O T E C T I O N */
#define SPL_RAISE_DIV         30
#define SPL_RAISE_STR         31
#define SPL_RAISE_DEX         32
#define SPL_RAISE_CON         33
#define SPL_RAISE_CHA         34
#define SPL_RAISE_BRA         35
#define SPL_SUN_RAY           36
#define SPL_DIVINE_RESIST     37
#define SPL_QUICKEN           38
#define SPL_HASTE             39
#define SPL_RAISE_SUMMONING   40
#define SPL_AWAKEN            41
#define SPL_MIND_SHIELD       42
#define SPL_HEAT_RESI         43
#define SPL_COLD_RESI         44
#define SPL_ELECTRICITY_RESI  45
#define SPL_POISON_RESI       46
#define SPL_ACID_RESI         47
#define SPL_PRO_EVIL          48
#define SPL_SANCTUARY         49
#define SPL_DISPEL_MAGIC      50
#define SPL_SUSTAIN           51
#define SPL_LOCK              52
#define SPL_UNLOCK            53
#define SPL_DROWSE            54
#define SPL_SLOW              55
#define SPL_DUST_DEVIL        56
#define SPL_DET_ALIGN         57  /* D E T E C T I O N */
#define SPL_DET_INVISIBLE     58
#define SPL_DET_MAGIC         59
#define SPL_DET_POISON        60
#define SPL_DET_UNDEAD        61
#define SPL_DET_CURSE         62
#define SPL_SENSE_LIFE        63
#define SPL_IDENTIFY_1        64
#define SPL_IDENTIFY_2        65

#define SPL_RANDOM_TELEPORT   66  /* S U M M O N I N G */
#define SPL_CLEAR_SKIES       67
#define SPL_STORM_CALL        68
#define SPL_WORD_OF_RECALL    69
#define SPL_CONTROL_TELEPORT  70
#define SPL_MINOR_GATE        71
#define SPL_GATE              72
#define SPL_CREATE_FOOD       73  /* C R E A T I O N */
#define SPL_CREATE_WATER      74
#define SPL_LIGHT_1           75
#define SPL_LIGHT_2           76
#define SPL_DARKNESS_1        77
#define SPL_DARKNESS_2        78
#define SPL_STUN              79
#define SPL_HOLD              80
#define SPL_ANIMATE_DEAD      81
#define SPL_LEATHER_SKIN      82
#define SPL_BARK_SKIN         83
#define SPL_CONTROL_UNDEAD    84
#define SPL_BONE_SKIN         85
#define SPL_STONE_SKIN        86
#define SPL_AID               87
#define SPL_COLOURSPRAY_1     88  /* M I N D */
#define SPL_COLOURSPRAY_2     89
#define SPL_COLOURSPRAY_3     90
#define SPL_INVISIBILITY      91
#define SPL_WIZARD_EYE        92
#define SPL_FEAR              93
#define SPL_CONFUSION         94
#define SPL_SLEEP             95
#define SPL_XRAY_VISION       96
#define SPL_CALM              97
#define SPL_SUMMER_RAIN       98
#define SPL_COMMAND           99
#define SPL_LEAVING          100
#define SPL_FIREBALL_1       101  /* H E A T */
#define SPL_FIREBALL_2       102
#define SPL_FIREBALL_3       103

#define SPL_FROSTBALL_1      104  /* C O L D */
#define SPL_FROSTBALL_2      105
#define SPL_FROSTBALL_3      106

#define SPL_LIGHTNING_1      107  /* C E L L */
#define SPL_LIGHTNING_2      108
#define SPL_LIGHTNING_3      109

#define SPL_STINKING_CLOUD_1 110  /* I N T E R N A L */
#define SPL_STINKING_CLOUD_2 111
#define SPL_STINKING_CLOUD_3 112
#define SPL_POISON           113
#define SPL_REMOVE_POISON    114
#define SPL_ENERGY_DRAIN     115
#define SPL_DISEASE_1        116
#define SPL_DISEASE_2        117
#define SPL_REM_DISEASE      118
#define SPL_ACIDBALL_1       119  /* E X T E R N A L */
#define SPL_ACIDBALL_2       120
#define SPL_ACIDBALL_3       121
#define SPL_LEAF_SHIELD      122
#define SPL_FIND_PATH        123  /* Divine   */
#define SPL_DISPEL_GOOD      124
#define SPL_PRO_GOOD         125
#define SPL_TRANSPORT        126
#define SPL_FIRE_BREATH      127
#define SPL_FROST_BREATH     128
#define SPL_LIGHTNING_BREATH 129
#define SPL_ACID_BREATH      130
#define SPL_GAS_BREATH       131
#define SPL_LIGHT_BREATH     132
#define SPL_HOLD_MONSTER     133
#define SPL_HOLD_UNDEAD      134
#define SPL_RAISE_DEAD       135
#define SPL_RESURRECTION     136
#define SPL_TOTAL_RECALL     137
#define SPL_UNDEAD_DOOR      138
#define SPL_LIFE_PROTECTION  139
#define SPL_ENERGY_BOLT      140
#define SPL_CLENCHED_FIST    141
#define SPL_METEOR_SHOWER    142
#define SPL_SUN_BEAM         143
#define SPL_SOLAR_FLARE      144
#define SPL_SUMMON_DEVIL     145
#define SPL_SUMMON_DEMON     146
#define SPL_SUMMON_FIRE      147
#define SPL_SUMMON_WATER     148
#define SPL_SUMMON_AIR       149
#define SPL_SUMMON_EARTH     150
#define SPL_CHARGE_WAND      151
#define SPL_CHARGE_STAFF     152
#define SPL_MENDING          153
#define SPL_REPAIR           154
#define SPL_RECONSTRUCT      155
#define SPL_SENDING          156
#define SPL_REFIT            157
#define SPL_FIND_WANTED      158
#define SPL_LOCATE_WANTED    159
#define SPL_STORM_GATE       160
#define SPL_SUN_GLOBE        161
#define SPL_MAGIC_CANDLE     162
#define SPL_SONIC_BREATH     163
#define SPL_SHARD_BREATH     164
#define SPL_CONE_SHARD       165
#define SPL_SACRED           166
#define	SPL_CAUSE_INSTABILITY 167
#define	SPL_CAUSE_FLIGHT     168
#define	SPL_SLOW_ROT         169
#define	SPL_HAMMER_HAND      170
#define	SPL_CAUSE_PANIC      171
#define	SPL_BANISH           172
#define	SPL_HOLD_FLIGHT      173
#define	SPL_BLINDING_FLASH   174
#define SPL_BLIZZARD         175
#define SPL_SUMMON_ANIMAL    176
#define SPL_HEAL_ANIMAL      177
#define SPL_HOLD_ANIMAL      178
#define SPL_CHARM_ANIMAL     179
#define SPL_LOCATE_ANIMAL    180
#define SPL_ENTANGLE         181
#define SPL_REFRESH          182
#define SPL_TIGER_CLAW       183
#define SPL_EAGLE_CRY        184
#define SPL_GROUP_TELEPORT   185
#define SPL_RAISE_HPP        186
#define SPL_SOUL_SHIFT       187
#define SPL_AID_UNDEAD       188
#define SPL_HOLY_WORD        189
#define SPL_UNHOLY_WORD      190
#define SPL_FINAL_REST       191
#define SPL_RAISE_SKELETON   192
#define SPL_RAISE_CHAMPION   193
#define SPL_RAISE_HORDE      194
#define SPL_PORTAL           195
#define SPL_TRAVEL           196
#define SPL_GLOW_FUNGUS      197
#define SPL_SEN_INVISIBLE    198
#define SPL_MANA_BOOST       199  /* Creation */
#define SPL_TSUNAMI          200
#define SPL_QUICKSAND        201
#define SPL_HEDGE_WALL       202
#define SPL_POISON_IVY       203
#define SPL_THORN_STRIKE     204
#define SPL_WIND_SLASH       205
#define SPL_PETRIFY          206
#define SPL_MUD_PUDDLE       207
#define SPL_EARTH_QUAKE      208
#define SPL_BRAMBLES         209
#define SPL_POLARITY         210
#define SPL_SUMMON_LIGHT     211
#define SPL_ICE_THORN        212
#define SPL_WIZARD_MARK      213
#define SPL_FOCUS            214
#define SPL_FIREBALL_4       215  /* H E A T */
#define SPL_FROSTBALL_4      216  /* C O L D */
#define SPL_LIGHTNING_4      217  /* C E L L */
#define SPL_ACIDBALL_4       218  /* E X T E R N A L */
#define SPL_COLOURSPRAY_4    219  /* M I N D */
#define SPL_STINKING_CLOUD_4 220  /* I N T E R N A L */
#define SPL_CAUSE_WOUNDS_4   221  /* DIVINE */
#define SPL_HOLY_WORD_2      222
#define SPL_HOLY_WORD_3      223
#define SPL_HOLY_WORD_4      224
#define SPL_UNHOLY_WORD_2    225
#define SPL_UNHOLY_WORD_3    226
#define SPL_UNHOLY_WORD_4    227
#define SPL_CONDEMN          228
#define SPL_DISPLACE_MAGIC   229
#define SPL_LAVA_1           230
#define SPL_LAVA_2           231
#define SPL_LAVA_3           232
#define SPL_LAVA_4           233
#define SPL_MIRE             234
#define SPL_NIGHT_GLOBE      235
#define LAST_SPELL           236
/* Just insert any new spells here... just keep incrementing by one */

/* keep in mind if you need more ask the vme coders.*/

/*
	*	Monster Races:
	*
	*	Remember if you change  monster races values you must recompile all spells
	*	 skills, zones, and anything that has races so that they have the new
	*	values.
	*
*/
#define MIN_HUMANOID_NPC         110
#define RACE_KINGRAITH           120
#define RACE_GOBLIN              121
#define RACE_HOBGOBLIN           122
#define RACE_KOBOLD              123
#define RACE_DRACONIAN           124
#define RACE_OGRE                125
#define RACE_ORC                 126
#define RACE_TROLL               127
#define RACE_SKAVEN              128
#define RACE_TANNARI             129
#define RACE_DROW                130
#define RACE_GIANT               131 /* use with morph only */
#define RACE_GIANT_MOB           132
#define RACE_DRIDER              133
#define MAX_HUMANOID_NPC         200
/*Ants, beetles, butterflies,lobsters, shrimp, crabs,scorpions, spiders, ticks*/
#define RACE_ARTHROPODA_MIN      220

#define RACE_BEE                 221
#define RACE_BEETLE              222
#define RACE_BUTTERFLY           223
#define RACE_COCKROACH           224
#define RACE_DRAGONFLY           225
#define RACE_INSECT              226
#define RACE_MANTIS              227
#define RACE_MOSQUITO            228
#define RACE_SCORPION            229
#define RACE_SPIDER              230
#define RACE_TRIPHID             231
#define RACE_ANT                 232
#define RACE_ARTHROPODA_MAX      269

/*Lugworms, earthworms,leeches */
#define RACE_ANNELIDA_MIN        270

#define RACE_LEECH               272
#define RACE_WORM                273
#define RACE_MAGGOT              274

#define RACE_ANNELIDA_MAX        299

/* Coral, hydra, jellyfish,Portuguese man-of-war, sea anemone */
#define RACE_CNIDARIA_MIN        300

#define RACE_JELLYFISH           301

#define RACE_CNIDARIA_MAX        329

/* Hagfish, lamprey */
#define RACE_JAWLESS_FISH_MIN    330

#define RACE_EEL                 331

#define RACE_JAWLESS_FISH_MAX    349

/*  CARTILAGINOUS FISH Sharks, skates, rays, chimaeras */
#define RACE_CARTIL_FISH_MIN     350

#define RACE_MANTA_RAY           351
#define RACE_SEA_HORSE           352
#define RACE_SHARK               353
#define RACE_CARTIL_FISH_MAX     399

/* Sturgeon, herring, salmon, perch, cod, coelacanth */
#define RACE_BONEY_FISH_MIN      400

#define RACE_MINNOW              401
#define RACE_PIRANHA             402
#define RACE_BARACUDA            403

#define RACE_BONEY_FISH_MAX      449

/* Frogs and toads, salamanders, newts, caecilians */
#define RACE_AMPHIBIAN_MIN       450

#define RACE_FROG                451
#define RACE_TOAD                452

#define RACE_AMPHIBIAN_MAX       499

/* Snakes, crocodiles, alligators, lizards, turtles, tortoises */
#define RACE_REPTILES_MIN        500

#define RACE_ALLIGATOR           501
#define RACE_CHAMELEON           502
#define RACE_CROCODILE           503
#define RACE_DINOSAUR            504
#define RACE_LIZARD              505
#define RACE_SNAKE               506
#define RACE_TURTLE              507
#define RACE_OTHER_REPTILE       508
#define RACE_REPTILES_MAX        549

/* Chitons, oysters, snails, clams, squid */
#define RACE_MOLLUSK_MIN         550

#define RACE_CRAB                551
#define RACE_SQUID               552
#define RACE_OCTOPUS             553
#define RACE_OYSTER              554

#define RACE_MOLLUSK_MAX         599

/* Penguin, flamingo, eagle, turkey, thrush, parrot */
#define RACE_BIRD_MIN            600
#define RACE_BIRD                601
#define RACE_CHICKEN             602
#define RACE_DUCK                603
#define RACE_EAGLE               604
#define RACE_GOOSE               605
#define RACE_PIGEON              607
#define RACE_PEACOCK             608
#define RACE_OSTRICH             609
#define RACE_OWL                 610
#define RACE_SEAGULL             611
#define RACE_SWAN                612
#define RACE_TURKEY              613
#define RACE_VULTURE             614

#define RACE_BIRD_MAX            699

/* Platypus, kangaroo, bat, lion, wolf, */
/*mouse, seal, antelope, cow, dolphin, whale, lemur, monkey, ape, human */
#define RACE_MAMMAL_MIN          700
#define RACE_BADGER              701
#define RACE_BAT                 702
#define RACE_BEAR                703
#define RACE_BEAVER              704
#define RACE_BOAR                705
#define RACE_CAMEL               706
#define RACE_CAT                 707
#define RACE_CHIPMUNK            708
#define RACE_COW                 709
#define RACE_DEER                710
#define RACE_DOG                 711
#define RACE_DOLPHIN             712
#define RACE_ELK                 713
#define RACE_FERRET              714
#define RACE_FOX                 715
#define RACE_GOAT                716
#define RACE_HAMSTER             717
#define RACE_HORSE               718
#define RACE_LEOPARD             719
#define RACE_LION                720
#define RACE_LLAMA               721
#define RACE_MOLE                722
#define RACE_MONKEY              723
#define RACE_MOOSE               724
#define RACE_MOUNTAIN_LION       725
#define RACE_MOUSE               726
#define RACE_PIG                 727
#define RACE_RABBIT              728
#define RACE_RACCOON             729
#define RACE_RAT                 730
#define RACE_RHINO               731
#define RACE_SEAL                732
#define RACE_SHEEP               733
#define RACE_SKUNK               734
#define RACE_SLOTH               735
#define RACE_SQUIRREL            736
#define RACE_TIGER               738
#define RACE_WALRUS              739
#define RACE_WEASLE              740
#define RACE_WERERAT             741
#define RACE_WHALE               742
#define RACE_WOLF                743
#define RACE_PORCUPINE           744
#define RACE_ELEPHANT            745
#define RACE_WARTHOG             745
#define RACE_MAMMAL_MAX          899

/* plants */
#define RACE_PLANT_MIN           900
#define RACE_FLOWER              901
#define RACE_TUMBLEWEED          902
#define RACE_SEAWEED             903
#define RACE_PEPPER              904
#define RACE_RASPBERRY           905
#define RACE_CARROT              906
#define RACE_VINE                907
#define RACE_BLUEBERRY           908
#define RACE_TREE                910
#define RACE_LOSKA               920
#define RACE_CACTUS              921
#define RACE_PUMPKIN             922
#define RACE_BEAN                923
#define RACE_STRAWBERRY          924
#define RACE_POTATO              925
#define RACE_TOMATO              926
#define RACE_CABBAGE             927
#define RACE_LETTUCE             928
#define RACE_WHEAT               929
#define RACE_CORN                930
#define RACE_ACIDPLANT           940
#define RACE_OTHER_PLANT         941
#define RACE_PLANT_GROW          942 /* for dargs plants needs changed - darg */
#define RACE_PLANT_MAX          1099


/*fantasy */



#define RACE_NORSE_GOD_MIN       1100 /* For use with special object */
#define RACE_NORSE_GOD           1101
#define RACE_NORSE_GOD_AIR       1102
#define RACE_NORSE_GOD_EARTH     1103
#define RACE_NORSE_GOD_FIRE      1104
#define RACE_NORSE_GOD_FROST     1105
#define RACE_NORSE_GOD_LIGHT     1106
#define RACE_NORSE_GOD_WATER     1107
#define RACE_NORSE_GOD_DARKNESS  1108
#define RACE_NORSE_GOD_MAX       1149 /* For use with special object */


#define RACE_DRAGON_MIN          1150  /* For use with special object */
#define RACE_DRAGON_BLACK        1151
#define RACE_DRAGON_BLUE         1152
#define RACE_DRAGON_GREEN        1153
#define RACE_DRAGON_RED          1154
#define RACE_DRAGON_WHITE        1155
#define RACE_DRAGON_SILVER       1156
#define RACE_DRAGON_TURTLE       1157
#define RACE_DRAGON_LAVA         1158
#define RACE_DRAGON_SHADOW       1159
#define RACE_DRAGON_LIZARD       1160
#define RACE_DRAGON_MAX          1199  /* For use with special object */



#define RACE_SPIRIT_MIN          1200 /* For use with special object */
#define RACE_UNDEAD              1201
#define RACE_SPIRIT              1202
#define RACE_GHOST               1203
#define RACE_MUMMIE              1204
#define RACE_ZOMBIE              1205
#define RACE_GHOUL               1206
#define RACE_SKELETON            1207
#define RACE_SPIRIT_MAX          1249  /* For use with special object */

#define RACE_LICH_MIN            1250 /* For use with special object */
#define RACE_LICH_AIR            1251
#define RACE_LICH_EARTH          1252
#define RACE_LICH_FIRE           1253
#define RACE_LICH_FROST          1254
#define RACE_LICH_LIGHT          1255
#define RACE_LICH_WATER          1256
#define RACE_LICH_MAX            1299 /* For use with special object */

#define RACE_ELEMENTAL_MIN       1300 /* For use with special object */
#define RACE_ELEMENTAL_AIR       1301
#define RACE_ELEMENTAL_EARTH     1302
#define RACE_ELEMENTAL_FIRE      1303
#define RACE_ELEMENTAL_FROST     1304
#define RACE_ELEMENTAL_LIGHT     1305
#define RACE_ELEMENTAL_WATER     1306
#define RACE_ELEMENTAL_MAX       1359 /* For use with special object */



#define RACE_DEMON_MIN           1360 /* For use with special object */
#define RACE_LESSER_DEMON        1361
#define RACE_GREATER_DEMON       1362
#define RACE_LESSER_DEVIL        1363
#define RACE_GREATER_DEVIL       1364
#define RACE_SNAKE_DEMON         1365
#define RACE_DEMON_MAX           1399 /* For use with special object */


#define RACE_HOMARID             2400
#define RACE_MERMAID             2401
#define RACE_PIXIE               2402
#define RACE_VAMPIRE             2403
#define RACE_WYRM                2404
#define RACE_MINOTAUR            2405

#define RACE_PEGASUS             2408
#define RACE_BANSHEE             2409
#define RACE_SAND_CRAWLER        2410
#define RACE_DRYAD               2411
#define RACE_NAIAD               2412
#define RACE_STONE_RENDER        2413
#define RACE_UR_VILE             2414
#define RACE_DEATH_GHAST         2415
#define RACE_SHADOW_BEAST        2416

#define RACE_YOGOLOTH            2418
#define RACE_YETI                2419
#define RACE_WEREWOLF            2420
#define RACE_CREEPER             2421  /* cross between a worm, a lizard, and a roach. */
#define RACE_MERMAN              2423
#define RACE_STATUE              2424
#define RACE_GOLEM               2425
#define RACE_SLIME               2426
#define RACE_UNICORN             2427
#define RACE_SPRITE              2428
#define RACE_MIST_DWELLER        2429
#define RACE_GARGOYLE            2430
#define RACE_MUTANT              2431
#define RACE_CAVE_FISHER         2432
#define RACE_SHADOW_DEVIL        2433
#define RACE_NIXIE               2434
#define RACE_NYMPH               2435
#define RACE_SATYR               2436
#define RACE_FAUN                2437
#define RACE_LEPRECHAUN          2438
#define RACE_SYLPH               2439
#define RACE_HERMIT              2440
#define RACE_SHARGUGH            2441
#define RACE_PUDDING             2442
#define RACE_MIMIC               2443

#define RACE_FAMILIAR            3000 /* Wierdo race... */
#define RACE_OTHER_CREATURE      3050
#define RACE_OTHER_HUMANOID      3100
#define RACE_OTHER_MAMMAL        3200

/*check values.h for max number of Monster races*/
/* keep in mind if you need more ask the vme coders.*/

/*
	*	Id values:
	*
	*	Remember to recompile any spell using the new id values you use.
	*	 Just replace the blank values with your new id.
	*
*/

#define ID_NBLESS                89
#define ID_FOCUS                 90
#define ID_MIRE                  91
#define ID_CONDEMN               92
#define ID_BLANK4                93
#define ID_BLANK5                94
#define ID_BLANK6                95
#define ID_BLANK7                96
#define ID_BLANK8                97
#define ID_BLANK9                98
#define ID_BLANK10               99
#define ID_BLANK11              100
/*check values.h for max number of id values*/
/* keep in mind if you need more ask the vme coders.*/
#define CLAN_MASTER    "$clan_master"
#define  CLAN_MEMBER  "$clan_member"
#define  CLAN_TATTOO  "clan_taTtoo"
#define CLAN_NAME "$clan_name"
#define CLAN_LIST "$clan_list"
#define CLAN_SYMBOLIC "$clan_symbolic"
#define CLAN_TREASURY "$clan_treasury"
#define CLAN_QUARTERMASTER "$clan_quartermaster"
#define CLAN_MONEY "$clan_money"
#define CLAN_STORAGE "$clan_storage"
#define CLAN_DONATION "$clan_donation"
#define CLAN_MAX_STORAGE  25
#define CLAN_MAX_DONATION  50
#define CLAN_RANK   "$clan_rank"
#define CLAN_ROST   "$clan_rost"
#define CLAN_LORD   "$clan_LORD"

/* added for new look move */

#define CANT_BURY   "$cannot_bury_this"

#define NO_BURY   \
extra{CANT_BURY}  \
""

  /* The following definitions are for water movement */
#define SWIM_ON \
extra{"$swim_on"} \
""

#define IS_FISH \
extra{"$is_fish"} \
""

#define IS_AMPHIB \
extra{"$is_amphib"} \
""

/* This allows a builder to put in a lock difficulty for pick lock purposes.
   Perhaps we can even have lock and unlock spells use this? 

#define LOCK_DIFF(diff)    \
extra{LOCK_DIFFICULTY} {diff}  \
""*/

/* #define DOOR_LOCK_DIFF(nlock, elock, slock, wlock, ulock, dlock, nelock,nwlock, selock, swlock) \
extra{DOOR_LOCK_DIFFICULTY} {nlock, elock, slock, wlock, ulock, dlock,nelock, nwlock, selock, swlock} \
""*/


// #define LOCK_DIFFICULTY   "$lock_diff"
// #define DOOR_LOCK_DIFFICULTY   "$door_lock_diff"


#define ITEM_VEHICLE   28

#define SECRET_DOOR     "$secret_door"
#define SECRET_OBJECT   "$secret_object"

#define SEARCH_DRAIN_AMOUNT  10      /* This is the amount drained when
searching. change this if you want to modify amount */

/*#define DOOR_DIFFICULTY(DIR, SKILL) \
extra{SECRET_DOOR} {DIR, SKILL}            \
""                                         \
dilcopy secret_wipe@basemove(DIR);*/

#define VEHICLE_DEFINITION(capac, fuel, leav_o, arriv_o) \
extra{"$leaving"}                                        \
leav_o                                                   \
extra{"$arriving"}                                       \
arriv_o                                                  \
NO_BURY                                                  \
type ITEM_VEHICLE                                        \
manipulate {MANIPULATE_TAKE, MANIPULATE_ENTER}           \
flags {UNIT_FL_TRANS}                                    \
capacity capac                                           \
value[1]   fuel                                          \
value[2]   fuel                                          \
dilcopy recharge_fuel@basemove();


#define BOAT_DEFINITION(capac, leav_o, arriv_o)          \
extra{"$leaving"}                                        \
leav_o                                                   \
extra{"$arriving"}                                       \
arriv_o                                                  \
NO_BURY                                                  \
type ITEM_BOAT                                           \
manipulate {MANIPULATE_TAKE, MANIPULATE_ENTER}           \
flags {UNIT_FL_TRANS}                                    \
capacity capac
/* end new look move stuff */

/* for new makemoney */
#define  IRON          0
#define  COPPER        1
#define  SILVER        2
#define  GOLD          3
#define  PLAT          4


#define  IRON_WEIGH    10
#define  COPPER_WEIGH  8
#define  SILVER_WEIGH  7
#define  GOLD_WEIGH    6
#define  PLAT_WEIGH    5

#define  MONEY_STRING  {"iron piece", "copper piece", "silver piece", "gold piece", "platinum piece"}
#define  MONEY_LOAD    {"iron_piece@basis", "copper_piece@basis", "silver_piece@basis", "gold_piece@basis", "platinum_piece@basis"}
/* end new make money */

/*for new restricts*/
#define GUILD_R "$guild_restrict"
#define ANTI_GUILD_R "$anti_guild_restrict"
#define QUEST_R "$quest_restrict"
#define ALIGNMENT_R "$alignment_restrict"
#define MAX_LEVEL_R "$max_level_restrict"
#define LEVEL_R "$level_restrict"
#define RACE_R "$race_restrict"
#define SEX_R "$sex_restrict"
#define ABILITY_R "$ability_restrict"
#define WEAPON_R "$weapon_restrict"
#define SPELL_R "$spell_restrict"
#define SKILL_R "$skill_restrict"
#define PLAYER_R "$player_restrict"
#define PROFESSION_R "$profession_restrict"
#define ANTI_PROFESSION_R "$anti_profession_restrict"


#define QUEST_RESTRICT(QUEST,DAMAGE,PERCENT,ACTION) \
extra {QUEST_R,QUEST}{DAMAGE,PERCENT} \
ACTION


#define ALIGNMENT_RESTRICT(MAX_ALI,MIN_ALI,DAMAGE,PERCENT,ACTION) \
extra {ALIGNMENT_R}{DAMAGE,PERCENT,MAX_ALI,MIN_ALI} \
ACTION

#define MAX_LEVEL_RESTRICT(LEVEL,DAMAGE,PERCENT,ACTION) \
extra {MAX_LEVEL_R}{DAMAGE,PERCENT,LEVEL} \
ACTION


#define LEVEL_RESTRICT(LEVEL,DAMAGE,PERCENT,ACTION) \
extra {LEVEL_R}{DAMAGE,PERCENT,LEVEL} \
ACTION

#define RACE_RESTRICT(RACE,DAMAGE,PERCENT,ACTION) \
extra {RACE_R}{DAMAGE,PERCENT,RACE} \
ACTION

#define SEX_RESTRICT(SEX,DAMAGE,PERCENT,ACTION) \
extra {SEX_R}{DAMAGE,PERCENT,SEX} \
ACTION

#define ABILITY_RESTRICT(ABILITY,NEEDED,DAMAGE,PERCENT,ACTION) \
extra {ABILITY_R}{DAMAGE,PERCENT,ABILITY,NEEDED} \
ACTION


#define WEAPON_RESTRICT(WEAPON,NEEDED,DAMAGE,PERCENT,ACTION) \
extra {WEAPON_R}{DAMAGE,PERCENT,WEAPON,NEEDED} \
ACTION

#define SPELL_RESTRICT(SPELL,NEEDED,DAMAGE,PERCENT,ACTION) \
extra {SPELL_R}{DAMAGE,PERCENT,SPELL,NEEDED} \
ACTION

#define SKILL_RESTRICT(SKILL,NEEDED,DAMAGE,PERCENT,ACTION) \
extra {SKILL_R}{DAMAGE,PERCENT,SKILL,NEEDED} \
ACTION

/*
*It will be very rare to use the macro for player restrict.  This is something that will be set by a quest
*or by an admin with the set command.
*exceptions might be an item that was bought for a player or a clan
*item in which you know the players name when it is built.
*/
#define PLAYER_RESTRICT(NAME,DAMAGE,PERCENT,ACTION) \
extra {PLAYER_R, NAME}{DAMAGE,PERCENT} \
ACTION


#define DIL_GINT_EFFDEX     0  // return the CHAR's effective_dex()
#define DIL_GINT_REQXP      1  // return the required_xp(i) for level i using the INT parameter i
#define DIL_GINT_LEVELXP    2  // return the level_xp(i) (XP needed for level i->i+1)
#define DIL_GINT_DESCRIPTOR 3  // returns 0 if PC has no descriptor. 1 for all else
#define DIL_GINT_CALLGUARDS 4  // Call the guards for DIL blow_whistle


#endif /* _MUD_VALUES_H */

