#ifndef _MUD_PLANT_H
#define _MUD_PLANT_H

#define RACE_BEGONIA	5001
#define RACE_THORNBUSH	5002
#define RACE_ROSEVINE	5003
#define RACE_PUFFER	5004
#define RACE_IVY	5005
#define RACE_MAGNOLIA	5006


#include <bottom.h>

/* FLOWER */
#define BASE_FLOWER(MLVL,GENDER,ARM) \
race RACE_FLOWER \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,75) \
SETHEIGHT(MLVL,20,300) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_FLOWER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_FLOWER (MLVL,GENDER,ARM_CLOTHES)

#define M_FLOWER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_FLOWER (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* FLOWER end */

/* TUMBLEWEED */
#define BASE_TUMBLEWEED(MLVL,GENDER,ARM) \
race RACE_TUMBLEWEED \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,40) \
SETHEIGHT(MLVL,15,120) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_TUMBLEWEED_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_TUMBLEWEED_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_TUMBLEWEED (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n pricks you with $1s thorns!", \
                           "$1n pricks $3n with $1s thorns!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* TUMBLEWEED end */

/* SEAWEED */
#define BASE_SEAWEED(MLVL,GENDER,ARM) \
race RACE_SEAWEED \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,5,100) \
SETHEIGHT(MLVL,60,600) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_SEAWEED_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SEAWEED (MLVL,GENDER,ARM_CLOTHES)

#define M_SEAWEED_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SEAWEED (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n stings you!", \
                           "$1n stings $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SEAWEED end */


/* PEPPER */
#define BASE_PEPPER(MLVL,GENDER,ARM) \
race RACE_PEPPER \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,2,140) \
SETHEIGHT(MLVL,28,200) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_PEPPER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_PEPPER (MLVL,GENDER,ARM_CLOTHES)

#define M_PEPPER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_PEPPER (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* PEPPER end */

/* RASPBERRY */
#define BASE_RASPBERRY(MLVL,GENDER,ARM) \
race RACE_RASPBERRY \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,4,300) \
SETHEIGHT(MLVL,55,450) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_RASPBERRY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_RASPBERRY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_RASPBERRY (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy combat_mag@function("entangle","",0,2); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* RASPBERRY end */

/* CARROT */
#define BASE_CARROT(MLVL,GENDER,ARM) \
race RACE_CARROT \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,140) \
SETHEIGHT(MLVL,15,300) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_CARROT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CARROT (MLVL,GENDER,ARM_CLOTHES)

#define M_CARROT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CARROT (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CARROT end */

/* VINE */
#define BASE_VINE(MLVL,GENDER,ARM) \
race RACE_VINE \
level MLVL \
sex GENDER \
alignment 0 \
SETWEIGHT(MLVL,3,1238) \
SETHEIGHT(MLVL,30,1220) \
NATURAL_DEF(WPN_CRUSH, ARM) \
MSET_ABILITY(15,15,15,15,10,15,0,15) \
MSET_WEAPON(8,10,6,6,6,0)   \
MSET_SPELL(6,6,4,6,4,10,5,6,5,6,6)

#define M_VINE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_VINE (MLVL,GENDER,ARM_CLOTHES)

#define M_VINE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_VINE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(13,5,9,13,15,15,10,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy combat_mag@function("cast entangle","",0,2); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* VINE end */

/* BLUEBERRY */
#define BASE_BLUEBERRY(MLVL,GENDER,ARM) \
race RACE_BLUEBERRY \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,3,238) \
SETHEIGHT(MLVL,30,220) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_BLUEBERRY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_BLUEBERRY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_BLUEBERRY (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* BLUEBERRY end */


/* TREE */
#define BASE_TREE(MLVL,GENDER,ARM) \
   race RACE_TREE \
   level MLVL \
   sex GENDER \
   alignment 0 \
   SETWEIGHT(MLVL,502,5313) \
   SETHEIGHT(MLVL,90,2250) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   MSET_ABILITY(33,12,27,23,2,3,0,0) \
   MSET_WEAPON(6,6,6,6,26,6)           /* Natural attack */ \
   MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)


#define M_TREE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER  \
   SETWEIGHT(MLVL,502,667) \
   SETHEIGHT(MLVL,90,333)

#define M_TREE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,695,1335) \
   SETHEIGHT(MLVL,330,666)

#define M_TREE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1334,2002) \
   SETHEIGHT(MLVL,600,999)

#define M_TREE_LARGE(MLVL,GENDER) \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2000,2670) \
   SETHEIGHT(MLVL,970,1332)

#define M_TREE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2660,3204) \
   SETHEIGHT(MLVL,1280,1600)

#define M_TREE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,3180,5313) \
   SETHEIGHT(MLVL,1600,2250)

#define M_TREE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_TREE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy combat_mag@function ("thorn", "heal", 25, 2); \
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "ANY", \
                 {"$1n attacks the evil $3n!", "$1n attacks you!"}); \
	 exp 125

/* TREE end */

/* LOSKA */
#define BASE_LOSKA(MLVL,GENDER,ARM) \
race RACE_LOSKA \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment -550 \
SETWEIGHT(MLVL,6,530) \
SETHEIGHT(MLVL,10,645) \
MSET_ABILITY(10,10,15,15,15,15,0,20) \
MSET_WEAPON(6,6,6,6,16,6) \
MSET_SPELL(4,4,4,4,4,14,4,4,4,4,4) \
dilcopy combat_mag@function("colourbeam","",0,2);


#define M_LOSKA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_LOSKA (MLVL,GENDER,ARM_CLOTHES)

#define M_LOSKA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_LOSKA (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* LOSKA end */


/* CACTUS */
#define BASE_CACTUS(MLVL,GENDER,ARM) \
race RACE_CACTUS \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment -450 \
SETWEIGHT(MLVL,6,530) \
SETHEIGHT(MLVL,7,645) \
MSET_ABILITY(21,12,20,25,12,0,10,0) \
MSET_WEAPON(6,6,6,6,20,6) \
MSET_SPELL(2,2,2,2,2,5,8,6,6,8,7)


#define M_CACTUS_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)

#define M_CACTUS_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CACTUS (MLVL,GENDER,ARM_CHAIN)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n shoots $1s spines at you!", \
                           "$1n shoots $1s spines at $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CACTUS end */

/* PUMPKIN */
#define BASE_PUMPKIN(MLVL,GENDER,ARM) \
race RACE_PUMPKIN \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,3,238) \
SETHEIGHT(MLVL,30,220) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_PUMPKIN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_CLOTHES)

#define M_PUMPKIN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_PUMPKIN (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy combat_mag@function("entangle","",0,2); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* PUMPKIN end */

/* BEAN */
#define BASE_BEAN(MLVL,GENDER,ARM) \
race RACE_BEAN \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,3,238) \
SETHEIGHT(MLVL,30,220) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_BEAN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_BEAN (MLVL,GENDER,ARM_CLOTHES)

#define M_BEAN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_BEAN (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n spears you with a bean!", \
                           "$1n spears $3n with a bean!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* BEAN end */

/* STRAWBERRY */
#define BASE_STRAWBERRY(MLVL,GENDER,ARM) \
race RACE_STRAWBERRY \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,2,289) \
SETHEIGHT(MLVL,22,189) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_STRAWBERRY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_CLOTHES)

#define M_STRAWBERRY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_STRAWBERRY (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n throws $1s berries at you!", \
                           "$1n throws $1s berries at $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* STRAWBERRY end */

/* POTATO */
#define BASE_POTATO(MLVL,GENDER,ARM) \
race RACE_POTATO \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,3,238) \
SETHEIGHT(MLVL,30,220) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_POTATO_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_POTATO (MLVL,GENDER,ARM_CLOTHES)

#define M_POTATO_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_POTATO (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* POTATO end */

/* TOMATO */
#define BASE_TOMATO(MLVL,GENDER,ARM) \
race RACE_TOMATO \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,225) \
SETHEIGHT(MLVL,20,115) \
MSET_ABILITY(20,20,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_TOMATO_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_TOMATO (MLVL,GENDER,ARM_CLOTHES)

#define M_TOMATO_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_TOMATO (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* TOMATO end */

/* CABBAGE */
#define BASE_CABBAGE(MLVL,GENDER,ARM) \
race RACE_CABBAGE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,225) \
SETHEIGHT(MLVL,35,225) \
MSET_ABILITY(20,15,25,20,10,5,5,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_CABBAGE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CABBAGE (MLVL,GENDER,ARM_CLOTHES)

#define M_CABBAGE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CABBAGE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CABBAGE end */

/* LETTUCE */
#define BASE_LETTUCE(MLVL,GENDER,ARM) \
race RACE_LETTUCE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 0 \
SETWEIGHT(MLVL,2,118) \
SETHEIGHT(MLVL,21,185) \
MSET_ABILITY(20,15,25,20,10,5,5,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_LETTUCE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_LETTUCE (MLVL,GENDER,ARM_CLOTHES)

#define M_LETTUCE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_LETTUCE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* LETTUCE end */

/* WHEAT */
#define BASE_WHEAT(MLVL,GENDER,ARM) \
race RACE_WHEAT \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,100) \
SETHEIGHT(MLVL,8,200) \
MSET_ABILITY(20,15,25,20,10,5,5,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_WHEAT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_WHEAT (MLVL,GENDER,ARM_CLOTHES)

#define M_WHEAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_WHEAT (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* WHEAT end */

/* CORN */
#define BASE_CORN(MLVL,GENDER,ARM) \
race RACE_CORN \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,200) \
SETHEIGHT(MLVL,15,428) \
MSET_ABILITY(20,15,25,20,10,5,5,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_CORN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CORN (MLVL,GENDER,ARM_CLOTHES)

#define M_CORN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CORN (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n slices you with $1s leaves!", \
                           "$1n slices $3n with $1s leaves!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CORN end */

/* ACIDPLANT */
#define BASE_ACIDPLANT(MLVL,GENDER,ARM) \
race RACE_ACIDPLANT \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment -500 \
SETWEIGHT(MLVL,1,200) \
SETHEIGHT(MLVL,15,350) \
MSET_ABILITY(20,15,25,20,10,5,5,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
dilcopy combat_mag@function("spray of acid", "", 60, 2); \
exp 110

#define M_ACIDPLANT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_CLOTHES)

#define M_ACIDPLANT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_ACIDPLANT(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(20,18,5,25,5,5,22,0) \
MSET_WEAPON(5,5,5,5,25,10) \
MSET_SPELL(3,5,3,4,5,4,4,3,7,4,3) \
dilcopy aggressive@function (0, -1, 0, 10, 3, PULSE_SEC*40, 0, "ANY", \
                                {"$1n comes alive and attacks $3n!", \
								 "$1n comes alive!"}); \
exp 125
/* ACIDPLANT end */


/* added by Heulflodyn for Div based plants 4/22/2008 */

/* BEGONIA */
#define BASE_BEGONIA(MLVL,GENDER,ARM) \
race RACE_BEGONIA \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,75) \
SETHEIGHT(MLVL,20,300) \
MSET_ABILITY(10,20,10,10,20,10,10,10) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(6,6,4,5,5,5,5,5,6,4,4)

#define M_BEGONIA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_BEGONIA (MLVL,GENDER,ARM_CLOTHES)

#define M_BEGONIA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_BEGONIA (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,10,10,15,15,15,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(6,6,4,6,6,6,6,6,6,4,4)\
dilcopy aggressive@function(0, -1, 0, 25, 2, PULSE_SEC*50, 0, "ANY", \
                    {"$1n sprays pollen in $3n's face!", "$1n sprays pollen in your face!"}); \
dilcopy combat_mag@function ("slow", "cause light wounds", 10, 2); \
exp 125

/*BEGONIA end*/

/* IVY */
#define BASE_IVY(MLVL,GENDER,ARM) \
race RACE_IVY \
level MLVL \
sex GENDER \
alignment 0 \
SETWEIGHT(MLVL,3,1238) \
SETHEIGHT(MLVL,30,1220) \
NATURAL_DEF(WPN_CRUSH, ARM) \
MSET_ABILITY(15,10,10,15,10,15,10,10) \
MSET_WEAPON(8,10,6,6,6,0)   \
MSET_SPELL(6,6,4,6,4,10,5,6,5,6,6)

#define M_IVY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_IVY (MLVL,GENDER,ARM_CLOTHES)

#define M_IVY_MAD(MLVL,GENDER) /* level 1 to 199 has  entangle and poison ivy */ \
BASE_IVY (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(13,5,9,13,15,15,10,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy combat_mag@function("entangle","poison ivy",0,2); \
dilcopy aggressive@function(0, -1, 0, 20, 3, PULSE_SEC*50, 0, "ANY", \
                       {"$1n wraps its tendrils around $3n tightly!",  \
                      "$1n wraps itself around your feet. You can't move!"}); \
exp 125

/* IVY end */


#define BASE_PUFFER(MLVL,GENDER,ARM) \
race RACE_PUFFER \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CRUSH,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,75) \
SETHEIGHT(MLVL,20,300) \
MSET_ABILITY(10,10,20,20,20,0,10,10) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(6,6,4,5,5,5,5,5,6,4,4)

#define M_PUFFER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_PUFFER (MLVL,GENDER,ARM_CLOTHES)

#define M_PUFFER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_PUFFER (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,10,10,15,15,5,10,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(6,6,4,6,6,6,6,6,6,4,4)\
dilcopy aggressive@function(0, -1, 0, 25, 2, PULSE_SEC*50, 0, "ANY", \
                    {"$1n sprays spores in $3n's face!", "$1n sprays spores in your face!"}); \
dilcopy combat_mag@function ("stinking cloud", "", 10, 2); \
exp 125

/* PUFFER end */

#define BASE_MAGNOLIA(MLVL,GENDER,ARM) \
   race RACE_MAGNOLIA \
   level MLVL \
   sex GENDER \
   alignment 0 \
   SETWEIGHT(MLVL,502,5313) \
   SETHEIGHT(MLVL,90,2250) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   MSET_ABILITY(28,7,22,18,2,3,10,10) \
   MSET_WEAPON(6,6,6,6,26,6)           /* Natural attack */ \
   MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)


#define M_MAGNOLIA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER  \
   SETWEIGHT(MLVL,502,667) \
   SETHEIGHT(MLVL,90,333)

#define M_MAGNOLIA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,695,1335) \
   SETHEIGHT(MLVL,330,666)

#define M_MAGNOLIA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1334,2002) \
   SETHEIGHT(MLVL,600,999)

#define M_MAGNOLIA_LARGE(MLVL,GENDER) \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2000,2670) \
   SETHEIGHT(MLVL,970,1332)

#define M_MAGNOLIA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2660,3204) \
   SETHEIGHT(MLVL,1280,1600)

#define M_MAGNOLIA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,3180,5313) \
   SETHEIGHT(MLVL,1600,2250)

#define M_MAGNOLIA_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MAGNOLIA (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(6,4,4,6,4,6,6,6,6,6,6) \
   dilcopy combat_mag@function ("sleep", "toxicate", 25, 2); \
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "ANY", \
                 {"$1n drips sap on $3n!", "$1n drips something sticky on you!"}); \
	 exp 125

/* THORNBUSH */
#define BASE_THORNBUSH(MLVL,GENDER,ARM) \
race RACE_THORNBUSH \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,4,300) \
SETHEIGHT(MLVL,55,450) \
MSET_ABILITY(10,20,20,20,20,0,10,0) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(6,4,4,5,5,5,5,5,6,6,4)


#define M_THORNBUSH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_CLOTHES)

#define M_THORNBUSH_MAD(MLVL,GENDER) /* level 1 to 199 has thorn strike and aggro */ \
BASE_THORNBUSH (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(6,4,4,6,6,6,6,6,6,6,4)\
dilcopy combat_mag@function("thorn strike","",0,2); \
dilcopy aggressive@function(0, -1, 0, 20, 3, PULSE_SEC*25, 0, "ANY", \
                    {"$1n shoots thorns at $3n!", "$1n shoots thorns at you!"}); \
exp 130

/* THORNBUSH end */


/*Rosevine*/
#define BASE_ROSEVINE(MLVL,GENDER,ARM) \
race RACE_ROSEVINE \
level MLVL \
sex GENDER \
alignment 0 \
SETWEIGHT(MLVL,3,1238) \
SETHEIGHT(MLVL,30,1220) \
NATURAL_DEF(WPN_CRUSH, ARM) \
MSET_ABILITY(15,15,15,15,10,15,15,0) \
MSET_WEAPON(8,10,6,6,6,0)   \
MSET_SPELL(6,6,4,6,4,10,5,6,5,6,6)

#define M_ROSEVINE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_CLOTHES)

#define M_ROSEVINE_MAD(MLVL,GENDER) /* level 1 to 199 has aggro w/entangle and thorn strike */ \
BASE_ROSEVINE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(13,5,9,13,15,15,10,20) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy combat_mag@function("entangle","thorn strike",0,2); \
dilcopy aggressive@function(0, -1, 0, 34, 3, PULSE_SEC*30, 0, "ANY", \
                     {"$1n digs its thorns into $3n!", "$1n wraps itself around you and digs its thorns in."}); \
exp 125

/* ROSEVINE end */




#endif
