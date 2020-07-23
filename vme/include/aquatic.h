
#ifndef _MUD_AQUATIC_H
#define _MUD_AQUATIC_H


#include <bottom.h>
/*  water spirit */
#define BASE_WATER_SPIRIT(MLVL,GENDER,ARM) \
   race RACE_SPIRIT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,66,914) \
   SETHEIGHT(MLVL,58,800)\
   alignment -1000 \
   NATURAL_DEF(WPN_CLAW, ARM) \
   MSET_ABILITY (20,15,15,15,10,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   IS_FISH \
   exp 125 \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE, CHAR_SNEAK}\
   dilcopy poi_resist@function();


#define M_WATER_SPIRIT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_WATER_SPIRIT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_WATER_SPIRIT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_WATER_SPIRIT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_LEATHER)

#define M_WATER_SPIRIT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_HLEATHER)

#define M_WATER_SPIRIT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_PLATE)

#define M_WATER_SPIRIT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_WATER_SPIRIT (MLVL,GENDER,ARM_PLATE)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy combat_mag@function("icestorm", "", 25, 2); \
   dilcopy aggressive@function(0,10,2,0,1,PULSE_SEC*12,-1,"ANY",\
   {"$1n mumbles something as he attacks $3n!", \
    "$1n mumbles something as he attacks YOU!"}); \
   exp 130

/* Water_SPIRIT end */

/* PIRANHA */
#define BASE_PIRANHA(MLVL,GENDER,ARM) \
race RACE_PIRANHA \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,10,60) \
SETHEIGHT(MLVL,30,172) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
 dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "ANY", \
                            {"$1n attacks $3n!", \
							   "$1n attacks you!"}); \
IS_FISH

#define M_PIRANHA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)

#define M_PIRANHA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_PIRANHA(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* PIRANHA end */

/* baracuda */
#define BASE_BARACUDA(MLVL,GENDER,ARM) \
race RACE_BARACUDA \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,10,60) \
SETHEIGHT(MLVL,30,172) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
 dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "ANY", \
                            {"$1n attacks $3n with $1s deadly jaws!", \
							   "$1n attacks you with $1s deadly jaws!"}); \
IS_FISH

#define M_BARACUDA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)

#define M_BARACUDA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_BARACUDA(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* BARACUDA end */

/* CRAB   */
#define BASE_CRAB(MLVL,GENDER,ARM) \
race RACE_CRAB \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,50) \
SETHEIGHT(MLVL,7,120) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,3,4,4,3,4,4,4,3,3,4) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} 


#define M_CRAB_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CRAB (MLVL,GENDER,ARM_LEATHER)

#define M_CRAB_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CRAB (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n pinches you with $1s claws!", \
                           "$1n pinches $3n with $1s claws!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CRAB end */

/* MANTA_RAY */
#define BASE_MANTA_RAY(MLVL,GENDER,ARM) \
race RACE_MANTA_RAY \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_KICK,ARM) \
alignment 0 \
SETWEIGHT(MLVL,105,430) \
SETHEIGHT(MLVL,90,260) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,4,4,4,3,4,3,3,3,4) \
IS_FISH

#define M_MANTA_RAY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_LEATHER)

#define M_MANTA_RAY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MANTA_RAY (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* MANTA_RAY end */

/* MERMAID */
#define BASE_MERMAID(MLVL,ARM) \
race RACE_MERMAID \
level MLVL \
sex SEX_FEMALE \
NATURAL_DEF(WPN_FIST,ARM) \
alignment 0 \
SETWEIGHT(MLVL,32,340) \
SETHEIGHT(MLVL,48,180) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,4,3,4,3,4,4,3,3,4) \
IS_AMPHIB \
SWIM_ON 


#define M_MERMAID_TINY(MLVL) /*level 1 to 25 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MERMAID (MLVL,ARM_LEATHER)

#define M_MERMAID_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MERMAID (MLVL,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* MERMAID end */

/* MERMAN */
#define BASE_MERMAN(MLVL,ARM) \
race RACE_MERMAN \
level MLVL \
sex SEX_MALE \
NATURAL_DEF(WPN_FIST,ARM) \
alignment 0 \
SETWEIGHT(MLVL,50,500) \
SETHEIGHT(MLVL,60,215) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,4,3,4,3,4,4,3,3,4) \
IS_AMPHIB \
SWIM_ON 


#define M_MERMAN_TINY(MLVL) /*level 1 to 25 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MERMAN (MLVL,ARM_LEATHER)

#define M_MERMAN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MERMAN (MLVL,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* MERMAN end */

/* SEA_HORSE */
#define BASE_SEA_HORSE(MLVL,GENDER,ARM) \
race RACE_SEA_HORSE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_KICK,ARM) \
alignment 0 \
SETWEIGHT(MLVL,5,100) \
SETHEIGHT(MLVL,60,600) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
IS_FISH 

#define M_SEA_HORSE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_LEATHER)

#define M_SEA_HORSE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SEA_HORSE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SEA_HORSE end */

/* EEL */
#define BASE_EEL(MLVL,GENDER,ARM) \
race RACE_EEL \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,10,48) \
SETHEIGHT(MLVL,90,360) \
MSET_ABILITY(10,10,10,20,20,10,20,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH


#define M_EEL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)

#define M_EEL_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_EEL (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(10,5,5,10,10,20,20,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* EEL end */

/* JELLYFISH */
#define BASE_JELLYFISH(MLVL,GENDER,ARM) \
race RACE_JELLYFISH \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,30) \
SETHEIGHT(MLVL,30,500) \
MSET_ABILITY(10,10,10,20,20,10,20,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH


#define M_JELLYFISH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)

#define M_JELLYFISH_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_JELLYFISH (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(10,5,5,10,10,20,20,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
 dilcopy comb_poison@function( "&c+rYour skin burns like fire!&[default]", \
                  "&c+c$1n stings $3n with its tentacles!&[default]"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* JELLYFISH end */

/* MINNOW */
#define BASE_MINNOW(MLVL,GENDER,ARM) \
race RACE_MINNOW \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,8) \
SETHEIGHT(MLVL,1,45) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH

#define M_MINNOW_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)

#define M_MINNOW_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MINNOW (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* MINNOW end */

/* ORCA max weight 12060 max height 27 feet 90*/
#define BASE_ORCA(MLVL,GENDER,ARM) \
race RACE_WHALE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 900 \
SETWEIGHT(MLVL,1000,12060) \
SETHEIGHT(MLVL,180,810) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH 

#define M_ORCA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)

#define M_ORCA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_ORCA (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* ORCA end */

/*  Gray whale is  45 feet max weight 70000 lbs on 90 */

#define BASE_GRAY_WHALE(MLVL,GENDER,ARM) \
race RACE_WHALE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 900 \
SETWEIGHT(MLVL,2000,32767) \
SETHEIGHT(MLVL,300,1350) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH

#define M_GRAY_WHALE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_GRAY_WHALE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_GRAY_WHALE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* GRAY_WHALE end */

/* BLUE WHALE 80 feet max weight 142000 lbs max */
#define BASE_BLUE_WHALE(MLVL,GENDER,ARM) \
race RACE_WHALE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 900 \
SETWEIGHT(MLVL,8000,30000) \
SETHEIGHT(MLVL,600,2400) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH

#define M_BLUE_WHALE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)

#define M_BLUE_WHALE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_BLUE_WHALE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* BLUE_WHALE end */

/* SQUID 60 feet max weight 12000 lbs max  */
#define BASE_SQUID(MLVL,GENDER,ARM) \
race RACE_SQUID \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 900 \
SETWEIGHT(MLVL,1,12000) \
SETHEIGHT(MLVL,30,1800) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH

#define M_SQUID_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)

#define M_SQUID_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SQUID (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SQUID end */

/* WALRUS is 10 feet max weight 2800 lbs max */
#define BASE_WALRUS(MLVL,GENDER,ARM) \
race RACE_WALRUS \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,120,2800) \
SETHEIGHT(MLVL,30,300) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
SWIM_ON \
IS_AMPHIB 


#define M_WALRUS_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)

#define M_WALRUS_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_WALRUS (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* WALRUS end */

/* seal is 6 feet max weight 318 lbs */
#define BASE_SEAL(MLVL,GENDER,ARM) \
race RACE_SEAL \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,45,318) \
SETHEIGHT(MLVL,30,180) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
SWIM_ON \
IS_AMPHIB 


#define M_SEAL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)

#define M_SEAL_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SEAL (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SEAL end */

/* dolphin 12 feet max weight 1440 lbs */
#define BASE_DOLPHIN(MLVL,GENDER,ARM) \
race RACE_DOLPHIN \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,120,1440) \
SETHEIGHT(MLVL,30,360) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
IS_FISH 


#define M_DOLPHIN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)

#define M_DOLPHIN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_DOLPHIN (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* DOLPHIN end */

/* great white shark */
#define BASE_GWHITE_SHARK(MLVL,GENDER,ARM) \
race RACE_SHARK \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,120,800) \
SETHEIGHT(MLVL,30,360) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
flags {UNIT_FL_NO_TELEPORT} \
 dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "ANY", \
                            {"$1n attacks $3n with $1s deadly jaws!", \
							   "$1n attacks you with $1s deadly jaws!"}); \
IS_FISH 


#define M_GWHITE_SHARK_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)

#define M_GWHITE_SHARK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_GWHITE_SHARK (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* GWHITE_SHARK end */



/* OCTOPUS */
#define BASE_OCTOPUS(MLVL,GENDER,ARM) \
   race RACE_OCTOPUS \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,22,4378) \
   SETHEIGHT(MLVL,30,5967) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   alignment -150   \
   MSET_ABILITY(30,20,10,20,5,0,15,0) \
   MSET_WEAPON(0,0,0,0,85,0) \
   /* A decent saving throw versus magic */ \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 40, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 105 \
IS_FISH 



#define M_OCTOPUS_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_CLOTHES)  \
   SETWEIGHT(MLVL,4,40) \
   SETHEIGHT(MLVL,15,45)

#define M_OCTOPUS_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_CLOTHES)  \
   SETWEIGHT(MLVL,35,60) \
   SETHEIGHT(MLVL,40,75)

#define M_OCTOPUS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_CLOTHES)  \
   SETWEIGHT(MLVL,50,100) \
   SETHEIGHT(MLVL,70,90)

#define M_OCTOPUS_LARGE(MLVL,GENDER) \
   BASE_OCTOPUS (MLVL,GENDER,ARM_CLOTHES)  /* level 76 to 100 */ \
   SETWEIGHT(MLVL,75,220) \
   SETHEIGHT(MLVL,80,180)

#define M_OCTOPUS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_LEATHER)  \
   SETWEIGHT(MLVL,200,460) \
   SETHEIGHT(MLVL,160,360)

#define M_OCTOPUS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_LEATHER)  \
   SETWEIGHT(MLVL,380,750) \
   SETHEIGHT(MLVL,320,500)

#define M_OCTOPUS_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_OCTOPUS (MLVL,GENDER,ARM_CLOTHES)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites &3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* OCTOPUS end */







#endif
