#ifndef _MUD_FANTASY_H
#define _MUD_FANTASY_H

#include <bottom.h>

/* SWANMAY*/
#define BASE_SWANMAY(MLVL,GENDER,ARM) \
   race RACE_SWAN \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,30,220) \
   SETHEIGHT(MLVL,45,150) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) 


#define M_SWANMAY_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_CLOTHES)

#define M_SWANMAY_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_LEATHER)

#define M_SWANMAY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_HLEATHER)

#define M_SWANMAY_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_HLEATHER)


#define M_SWANMAY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_CHAIN)

#define M_SWANMAY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_PLATE)

#define M_SWANMAY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SWANMAY(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n manages to grab you with its bill!", \
                                "$1n manages to grab $3n with $1s bill!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* SWANMAY end */

/* HOMARID */
#define BASE_HOMARID(MLVL,GENDER,HEIGHT,WEIGHT) \
   race RACE_HOMARID \
   level MLVL \
   sex GENDER \
   height HEIGHT \
   weight WEIGHT \
   alignment -350 \
   IS_AMPHIB \
   SWIM_ON \
   NATURAL_DEF(WPN_CLAW,ARM_PLATE) \
   MSET_ABILITY(15,15,25,20,10,10,5,0) \
   MSET_WEAPON(0,0,0,0,20,0) \
   MSET_SPELL(0,10,10,10,10,0,10,10,0,10,10) 


#define HOMARID_MAGE(MLVL,GENDER,ATT_SPL,DEF_SPL) \
   BASE_HOMARID(MLVL,GENDER,150,170) \
   dilcopy combat_mag@function(ATT_SPL,DEF_SPL,25,2);

#define HOMARID_WARRIOR(MLVL,GENDER,HEIGHT,WEIGHT) \
   BASE_HOMARID(MLVL,GENDER,HEIGHT,WEIGHT)

#define BASE_MERFOLK(MLVL,GENDER,HEIGHT,WEIGHT) \
   race RACE_MERMAID \
   level MLVL \
   sex GENDER \
   height HEIGHT \
   weight WEIGHT \
   alignment 250 \
   IS_AMPHIB \
   SWIM_ON \
   NATURAL_DEF(WPN_FIST,ARM_CHAIN) \
   MSET_ABILITY(10,15,15,15,15,15,5,10) \
   MSET_WEAPON(0,10,0,15,15,0) \
   MSET_SPELL(10,5,0,5,10,10,10,10,0,0,0) 


#define MERFOLK_WARRIOR(MLVL,GENDER) \
   BASE_MERFOLK(MLVL,GENDER,220,180)

#define MERFOLK_SHAMAN(MLVL,GENDER,ATT_SPL,DEF_SPL) \
   BASE_MERFOLK(MLVL,GENDER,200,170) \
   dilcopy combat_mag@function(ATT_SPL,DEF_SPL,25,2);

/* end homarid */

/* PIXIE   */
#define BASE_PIXIE(MLVL,GENDER,ARM) \
race RACE_PIXIE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
SETWEIGHT(MLVL,2,30) \
SETHEIGHT(MLVL,17,60) \
alignment 330 \
MSET_ABILITY(20,15,10,10,15,15,15,0) \
MSET_WEAPON(8,8,8,8,8,0) \
MSET_SPELL(4,4,4,20,4,4,4,4,4,4,4)  \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} 


#define M_PIXIE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_PIXIE(MLVL,GENDER,ARM_LEATHER)

#define M_PIXIE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_PIXIE(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* PIXIE end */

/* VAMPIRE */
#define BASE_VAMPIRE(MLVL,GENDER,ARM) \
race RACE_VAMPIRE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment -1000 \
SETWEIGHT(MLVL,70,350) \
SETHEIGHT(MLVL,90,261) \
romflags {CHAR_DETECT_INVISIBLE,CHAR_HIDE} \
MSET_ABILITY (10, 10, 10, 10, 20, 20, 0, 20)  \
MSET_WEAPON ( 10, 10, 10, 10, 10, 10) \
MSET_SPELL ( 2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2) \
dilcopy combat_mag@function("energy drain", "", 50, 2); \
dilcopy comb_poison@function("$1n bites you.", "$1n bites $3n."); \
exp 125  


#define M_VAMPIRE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_HLEATHER)

#define M_VAMPIRE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_HLEATHER)

#define M_VAMPIRE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_HLEATHER)

#define M_VAMPIRE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_CHAIN)

#define M_VAMPIRE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_PLATE)

#define M_VAMPIRE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_VAMPIRE (MLVL,GENDER,ARM_PLATE)
/* VAMPIRE end */

/* WYRM */
#define BASE_WYRM(MLVL,GENDER,ARM) \
   race RACE_WYRM \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 400 \
   SETWEIGHT(MLVL,135,500) \
   SETHEIGHT(MLVL,98,300) \
   exp 110 \
   alignment -1000 \
   MSET_ABILITY(15,15,15,15,15,10,15,0)  \
   MSET_WEAPON(6,6,6,6,15,6)\
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
   romflags CHAR_DETECT_INVISIBLE 


#define M_WYRM_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_WYRM(MLVL,GENDER,ARM_LEATHER)

#define M_WYRM_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_WYRM(MLVL,GENDER,ARM_HLEATHER)

#define M_WYRM_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_WYRM(MLVL,GENDER,ARM_HLEATHER)

#define M_WYRM_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_WYRM(MLVL,GENDER,ARM_HLEATHER)

#define M_WYRM_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_WYRM(MLVL,GENDER,ARM_CHAIN)

#define M_WYRM_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_WYRM(MLVL,GENDER,ARM_PLATE)

#define M_WYRM_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_WYRM(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s teeth into $3n!"); \
      dilcopy aggressive@function(0,10,2,0,1,PULSE_SEC*12,-1,"ANY",\
   {"$1n attacks $3n!", \
    "$1n attacks YOU!"}); \
	 exp 135

/* WYRM end */

/* MINOTAUR */
#define BASE_MINOTAUR(MLVL,GENDER,ARM) \
   race RACE_MINOTAUR \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 400 \
   SETWEIGHT(MLVL,135,1175) \
   SETHEIGHT(MLVL,98,300) \
   exp 110 \
   alignment -1000 \
   MSET_ABILITY(15,15,15,15,15,10,15,0)  \
   MSET_WEAPON(6,6,6,6,15,6)\
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
   dilcopy combat_mag@function("madness"," ",0,2); 



#define M_MINOTAUR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_LEATHER)

#define M_MINOTAUR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_HLEATHER)

#define M_MINOTAUR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_HLEATHER)

#define M_MINOTAUR_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_HLEATHER)

#define M_MINOTAUR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_CHAIN)

#define M_MINOTAUR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_PLATE)

#define M_MINOTAUR_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MINOTAUR(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bash its hoofs into you!", \
                                "$1n bashs $1s hoofs into $3n!"); \
      dilcopy aggressive@function(0,10,2,0,1,PULSE_SEC*12,-1,"ANY",\
   {"$1n bellows as $1e charges $3n!", \
    "$1n bellows as $1e attacks YOU!"}); \
	 exp 135

/* MINOTAUR end */

#include <bottom.h>

/* DEVILS */
#define BASE_DEVIL(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,160,800) /* based on orginal one for height and weight */ \
   SETHEIGHT(MLVL,100,360) \
   NATURAL_DEF(WPN_FIST, ARM) \
   alignment -1000 \
   romflags {CHAR_DETECT_INVISIBLE, CHAR_DETECT_LIFE} \
   flags {CHAR_SNEAK,CHAR_HIDE} \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,15,10) \
   MSET_SPELL(3,3,3,3,3,3,3,3,3,3,5) \
dilcopy aggressive@function(0, -1, 0, 20, 2, PULSE_SEC*6, 1, "GOOD", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); 


#define BASE_LESSER_DEVIL(MLVL,GENDER,ARM,ATT_SPL,DEF_SPL) \
   BASE_DEVIL(MLVL,GENDER,ARM) \
   race RACE_LESSER_DEVIL \
   NATURAL_DEF(WPN_FIST, ARM) \
   MSET_ABILITY(15,15,15,15,15,10,15,0) /* 100 % */ \
   MSET_WEAPON(7,7,7,7,14,7)           /*  49 % */ \
   MSET_SPELL(5,4,4,4,4,4,4,4,4,7,7) /*  51 % */ \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2); \
   exp 125

#define BASE_GREATER_DEVIL(MLVL,GENDER,ARM,ATT_SPL,DEF_SPL) \
   BASE_DEVIL(MLVL,GENDER,ARM) \
   race RACE_GREATER_DEVIL \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   MSET_ABILITY(10,10,10,10,15,15,30,0) /* 100 % */ \
   MSET_WEAPON(2,11,2,2,11,2)           /*  30 % */ \
   MSET_SPELL(7,1,1,5,1,11,7,2,4,6,25) /*  70 % */ \
   alignment -1000 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2); \
   exp 125


/* non aggro */
#define M_GREATER_DEVIL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_LEATHER,"fireball","")

#define M_GREATER_DEVIL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_LEATHER,"fireball","")

#define M_GREATER_DEVIL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"firestorm","")

#define M_GREATER_DEVIL_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"firestorm","")

#define M_GREATER_DEVIL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"fire blast","")

#define M_GREATER_DEVIL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GREATER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"fire blast","")


/* aggro DEVIL */
#define M_LESSER_DEVIL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_CLOTHES,"burn","")

#define M_LESSER_DEVIL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_CLOTHES,"burn","")

#define M_LESSER_DEVIL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_LEATHER,"fireball","")

#define M_LESSER_DEVIL_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_LEATHER,"fireball","")

#define M_LESSER_DEVIL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"firestorm","")

#define M_LESSER_DEVIL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LESSER_DEVIL(MLVL,GENDER,ARM_HLEATHER,"firestorm","")


/* DEVIL end */

/* Pegasus */
#define BASE_PEGASUS(MLVL,GENDER,ARM) \
   race RACE_PEGASUS \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 700 \
   SETWEIGHT(MLVL,100,2000) \
   SETHEIGHT(MLVL,75,360) \
   MSET_ABILITY(22,21,17,25,15,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */ 


#define M_PEGASUS_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_PEGASUS (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER  \
   SETWEIGHT(MLVL,150,225) \
   SETHEIGHT(MLVL,75,105)

#define M_PEGASUS_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_PEGASUS (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,225,400) \
   SETHEIGHT(MLVL,105,135)

#define M_PEGASUS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_PEGASUS (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,400,900) \
   SETHEIGHT(MLVL,135,165)

#define M_PEGASUS_LARGE(MLVL,GENDER) \
   BASE_PEGASUS (MLVL,GENDER,ARM_HLEATHER)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,900,1500) \
   SETHEIGHT(MLVL,165,195)

#define M_PEGASUS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_PEGASUS (MLVL,GENDER,ARM_CHAIN)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1500,2000) \
   SETHEIGHT(MLVL,195,225)

#define M_PEGASUS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_PEGASUS (MLVL,GENDER,ARM_PLATE)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2000,4000) \
   SETHEIGHT(MLVL,225,360)
/* PEGASUS end */

/* BANSHEE */
#define BASE_BANSHEE(MLVL,GENDER,ARM) \
race RACE_BANSHEE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment -800 \
SETWEIGHT(MLVL,70,300) \
SETHEIGHT(MLVL,90,221) \
MSET_ABILITY (10, 10, 10, 10, 20, 20, 0, 20)  \
MSET_WEAPON ( 10, 10, 10, 10, 10, 10) \
MSET_SPELL ( 2, 2, 2, 2, 2, 2, 2, 2, 2, 20, 2) \
dilcopy combat_mag@function("madness", "", 25, 2); \
dilcopy aggressive@function(0, -1, 0, 0, 0, 10, 0, "ANY", \
{"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125 


#define M_BANSHEE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_HLEATHER)

#define M_BANSHEE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_HLEATHER)

#define M_BANSHEE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_HLEATHER)

#define M_BANSHEE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_CHAIN)

#define M_BANSHEE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_PLATE)

#define M_BANSHEE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BANSHEE (MLVL,GENDER,ARM_PLATE)
/* BANSHEE end */

/* SAND_CRAWLER   */
#define BASE_SAND_CRAWLER(MLVL,GENDER,ARM) \
race RACE_SAND_CRAWLER \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,30,300) \
SETHEIGHT(MLVL,30,300) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,3,3,4,4,4,4,3,3,4) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} 


#define M_SAND_CRAWLER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_LEATHER)

#define M_SAND_CRAWLER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SAND_CRAWLER (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SAND_CRAWLER end */

/* DRYAD  tree nymph */
#define BASE_DRYAD(MLVL,GENDER,ARM) \
race RACE_DRYAD \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,60,170) \
SETHEIGHT(MLVL,60,160) \
romflags {CHAR_DETECT_INVISIBLE} \
MSET_ABILITY(5,15,5,15,25,10,2,23) \
MSET_WEAPON(5,5,5,5,5,5)     \
MSET_SPELL(5,5,5,5,5,5,5,5,5,20,5) \
dilcopy combat_mag@function("stinking cloud","cure serious wounds",25,2); \
exp 120 


#define M_DRYAD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_DRYAD (MLVL,GENDER,ARM_LEATHER)

#define M_DRYAD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_DRYAD (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* DRYAD end */

/* NAIAD  pond nymph */
#define BASE_NAIAD(MLVL,GENDER,ARM) \
race RACE_NAIAD \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment 0 \
SETWEIGHT(MLVL,25,80) \
SETHEIGHT(MLVL,45,140) \
MSET_ABILITY(15,10,5,15,25,10,2,18) \
MSET_WEAPON(5,5,5,5,5,5) \
MSET_SPELL(20,3,3,3,3,23,3,3,3,3,3) \
dilcopy combat_mag@function("colourspray","cure light wounds",25,2); \
romflags {CHAR_DETECT_INVISIBLE} \
flags {UNIT_FL_NO_TELEPORT} \
exp 120 


#define M_NAIAD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_NAIAD (MLVL,GENDER,ARM_LEATHER)

#define M_NAIAD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_NAIAD (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* NAIAD end */

/* STONE RENDER */
#define BASE_STONE_RENDER(MLVL,GENDER,ARM) \
   race RACE_STONE_RENDER \
   level MLVL \
   sex GENDER \
   alignment -800 \
   SETWEIGHT(MLVL,200,6000) \
   SETHEIGHT(MLVL,100,600) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   MSET_ABILITY(18,14,18,18,18,14,0,0) \
   MSET_WEAPON(10,15,14,15,20,15) \
   MSET_SPELL(1,1,1,1,1,1,1,1,1,1,1) 


#define M_STONE_RENDER_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_CHAIN)

#define M_STONE_RENDER_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_CHAIN)

#define M_STONE_RENDER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_CHAIN)

#define M_STONE_RENDER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_PLATE)

#define M_STONE_RENDER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_PLATE)

#define M_STONE_RENDER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_PLATE)

#define M_STONE_RENDER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_STONE_RENDER (MLVL,GENDER,ARM_PLATE)  \
   alignment -1000  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* STONE_RENDER end */

/* NORSE_GOD */
#define BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   SETWEIGHT(MLVL,320,800) \
   SETHEIGHT(MLVL,180,450) \
   alignment 0 \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_WEAPON( 5,5,5,5,15,5) /* 40 */ \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   flags{UNIT_FL_NO_TELEPORT} \
   exp 110  


#define BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_AIR \
   MSET_ABILITY(10,10,10,15,20,15,0,20) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,10,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, -2, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
      dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
	exp 140

#define BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_EARTH \
   MSET_ABILITY(10,10,10,15,20,15,0,20) \
   MSET_SPELL( 5,5,5,5,10,5,5,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 2, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 140

#define BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_FIRE \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,10,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 1, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 140

#define BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_FROST \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,10,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
    dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
    exp 140

#define BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_LIGHT \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,10,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, -1, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 140

#define BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_WATER \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*10, -2, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 140

#define BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_NORSE_GOD(MLVL,GENDER,ARM) \
   race RACE_NORSE_GOD_DARKNESS \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*10, -2, "OPPOSITE", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 140

#define M_NORSE_GOD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_LEATHER)

#define M_NORSE_GOD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_LEATHER)

#define M_NORSE_GOD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_LEATHER)

#define M_NORSE_GOD_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_HLEATHER)

#define M_NORSE_GOD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_CHAIN)

#define M_NORSE_GOD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD(MLVL,GENDER,ARM_PLATE)

#define M_NORSE_GOD_AIR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_LEATHER,"fart")

#define M_NORSE_GOD_AIR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_LEATHER, "fart")

#define M_NORSE_GOD_AIR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_LEATHER, "bad breath")

#define M_NORSE_GOD_AIR_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_HLEATHER, "stinking cloud")

#define M_NORSE_GOD_AIR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_CHAIN, "stinking cloud")

#define M_NORSE_GOD_AIR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_AIR(MLVL,GENDER,ARM_PLATE, "stinking breath")

#define M_NORSE_GOD_EARTH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_NORSE_GOD_EARTH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_NORSE_GOD_EARTH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_NORSE_GOD_EARTH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_HLEATHER, "earthquake")

#define M_NORSE_GOD_EARTH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_CHAIN, "earthquake")

#define M_NORSE_GOD_EARTH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_EARTH(MLVL,GENDER,ARM_PLATE, "earthquake")

#define M_NORSE_GOD_FIRE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_LEATHER, "burn")

#define M_NORSE_GOD_FIRE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_NORSE_GOD_FIRE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_NORSE_GOD_FIRE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_HLEATHER, "fore storm")

#define M_NORSE_GOD_FIRE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_CHAIN, "fire storm")

#define M_NORSE_GOD_FIRE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_FIRE(MLVL,GENDER,ARM_PLATE, "fire breath")

#define M_NORSE_GOD_FROST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_LEATHER, "snowball")

#define M_NORSE_GOD_FROST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_NORSE_GOD_FROST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_NORSE_GOD_FROST_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_HLEATHER, "iceball")

#define M_NORSE_GOD_FROST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_CHAIN, "iceball")

#define M_NORSE_GOD_FROST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_FROST(MLVL,GENDER,ARM_PLATE, "icestorm")

#define M_NORSE_GOD_LIGHT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_NORSE_GOD_LIGHT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_NORSE_GOD_LIGHT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_NORSE_GOD_LIGHT_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_HLEATHER, "energy drain")

#define M_NORSE_GOD_LIGHT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_CHAIN, "energy drain")

#define M_NORSE_GOD_LIGHT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_LIGHT(MLVL,GENDER,ARM_PLATE, "energy drain")

#define M_NORSE_GOD_WATER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_LEATHER, "acid drop")

#define M_NORSE_GOD_WATER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_NORSE_GOD_WATER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_NORSE_GOD_WATER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_HLEATHER, "acid ball")

#define M_NORSE_GOD_WATER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_CHAIN, "acid ball")

#define M_NORSE_GOD_WATER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_WATER(MLVL,GENDER,ARM_PLATE, "acid swirl")

#define M_NORSE_GOD_DARKNESS_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_LEATHER, "acid drop")

#define M_NORSE_GOD_DARKNESS_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_NORSE_GOD_DARKNESS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_NORSE_GOD_DARKNESS_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_HLEATHER, "acid ball")

#define M_NORSE_GOD_DARKNESS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_CHAIN, "acid ball")

#define M_NORSE_GOD_DARKNESS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_NORSE_GOD_DARKNESS(MLVL,GENDER,ARM_PLATE, "acid swirl")

/* NORSE_GOD end */


/* UNDEADS */
#define BASE_UNDEAD(MLVL, GENDER, ARM)  \
   race RACE_UNDEAD \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -100  /* not all UNDEADs are evil */ \
   romflags {CHAR_DETECT_INVISIBLE} \
   SETWEIGHT(MLVL,2,200) \
   SETHEIGHT(MLVL,30,300) \
   dilcopy poi_resist@function(); \
   MSET_ABILITY(18,7,18,7,25,0,0,25) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(3,3,3,3,3,3,3,3,3,25,3) 



#define BASE_UNDEAD_INVIS(MLVL, GENDER, ARM)  \
   BASE_UNDEAD(MLVL,GENDER,ARM) \
   race RACE_UNDEAD \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -750  /*these are evil */ \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy poi_resist@function(); \
   MSET_ABILITY (8,7,8,7,25,20,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2); \
   dilcopy comb_poison@function("<div class='cpr'>You feel fire burning within your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*60, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 170

/* -------------------------------------------------------------------- */
/*                   UNDEADS                                            */
/* -------------------------------------------------------------------- */
#define M_UNDEAD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)

#define M_UNDEAD_MAD(MLVL,GENDER) /* level 1 to 199 */ \
   BASE_UNDEAD(MLVL,GENDER,ARM_CLOTHES)  \
   dilcopy comb_poison@function("<div class='cpr'>You feel poison burning your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 130

/* -------------------------------------------------------------------- */
/*            INVISIBLE MEAN UNDEADS                                     */
/* -------------------------------------------------------------------- */

#define M_INVIS_UNDEAD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_UNDEAD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_UNDEAD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_UNDEAD_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_UNDEAD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_UNDEAD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_UNDEAD_INVIS(MLVL,GENDER,ARM_CLOTHES)

/* UNDEADS END */

/* GARGOYLE */
#define BASE_GARGOYLE(MLVL,GENDER,ARM) \
   race RACE_GARGOYLE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,300,700) \
   SETHEIGHT(MLVL,150,221) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,17,10)          /* Natural attack */ \
   MSET_SPELL(3,3,3,3,3,3,3,3,3,3,3)  /* Resistances    */ 


#define M_GARGOYLE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_HLEATHER)

#define M_GARGOYLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_HLEATHER)

#define M_GARGOYLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_HLEATHER)

#define M_GARGOYLE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_CHAIN)

#define M_GARGOYLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_PLATE)

#define M_GARGOYLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_PLATE)

#define M_GARGOYLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_GARGOYLE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to claw you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* GARGOYLE end */

/* MUTANT */
#define BASE_MUTANT(MLVL,GENDER,ARM) \
   race RACE_MUTANT \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,400,10000) \
   SETHEIGHT(MLVL,355,1800) \
   MSET_ABILITY ( 10, 10, 10, 10, 25, 10, 25, 0) \
   MSET_WEAPON (1,1,2,2,2,2) \
   MSET_SPELL ( 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 40)\
   romflags {CHAR_DETECT_INVISIBLE,CHAR_SNEAK} \
   dilcopy combat_mag@function("acid ball", "", 25, 2); 


#define M_MUTANT_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_MUTANT (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,400,1000) \
   SETHEIGHT(MLVL,105,350)

#define M_MUTANT_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_MUTANT (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1000,2000) \
   SETHEIGHT(MLVL,335,665)

#define M_MUTANT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MUTANT (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2000,4000) \
   SETHEIGHT(MLVL,652,980)

#define M_MUTANT_LARGE(MLVL,GENDER) \
   BASE_MUTANT (MLVL,GENDER,ARM_CLOTHES)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,3500,5000) \
   SETHEIGHT(MLVL,972,1220)

#define M_MUTANT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MUTANT (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,5000,8000) \
   SETHEIGHT(MLVL,1202,1500)

#define M_MUTANT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MUTANT (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,7500,15000) \
   SETHEIGHT(MLVL,1550,1800)

#define M_MUTANT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MUTANT (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s teeth deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125
/* mutant end */

/* CAVE_FISHER */
#define BASE_CAVE_FISHER(MLVL,GENDER,ARM) \
   race RACE_CAVE_FISHER \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,10,500) \
   SETHEIGHT(MLVL,16,480) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 0  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5) 


#define M_CAVE_FISHER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)

#define M_CAVE_FISHER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_CAVE_FISHER (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n growls loudly as it bites you!", \
                                "$1n growls loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* CAVE_FISHER end */

/* DRAGONS */

#define BASE_DRAGON(MLVL, GENDER,ARM, BREATH)  \
sex GENDER \
level MLVL \
NATURAL_DEF(WPN_CLAW, ARM) \
SETWEIGHT(MLVL,800,30000) \
SETHEIGHT(MLVL,150,3000) \
MSET_ABILITY(25,10,10,10,20,10,15,0)  \
MSET_WEAPON(5,5,5,5,20,5) \
MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5) \
romflags {CHAR_DETECT_INVISIBLE} \
dilcopy combat_mag@function(BREATH, "", 25, 2); \
ATTACK_DEFENSE(+100, +100) \
exp 160  \
dilcopy command_block@basis("backstab", \
"You stealthily attempt to backstab $3n but $3e catches you.", \
"$1n stealthily tries to backstab $3n but $3e catches $1m."); 



#define BASE_DRAGON_BLACK(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM,"acid breath")  \
   alignment -750 \
   race RACE_DRAGON_BLACK \
MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,5,10) \
   SETWEIGHT(MLVL,900,27125) \
   SETHEIGHT(MLVL,360,2850) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 45, 2, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \


#define BASE_DRAGON_RED(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "shard breath")   \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,10,5,5,5,5,5,5,5) \
   alignment -750 \
   race RACE_DRAGON_RED \
   SETWEIGHT(MLVL,825,26750) \
   SETHEIGHT(MLVL,330,2700) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \

#define BASE_DRAGON_WHITE(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "frost breath")   \
   MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,5,5,10,5,5,5) \
   alignment 750 \
   race RACE_DRAGON_WHITE \
   SETWEIGHT(MLVL,750,26375) \
   SETHEIGHT(MLVL,300,2550) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 45, -2, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \

#define BASE_DRAGON_BLUE(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "lightning breath")  \
MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,5,5,5,10,5,5) \
   alignment -1000 \
   race RACE_DRAGON_BLUE \
   SETWEIGHT(MLVL,675,26000) \
   SETHEIGHT(MLVL,270,2400) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 45, 1, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \

#define BASE_DRAGON_GREEN(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "gas breath")  \
   MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,10,5) \
   alignment -1000 \
   race RACE_DRAGON_GREEN \
   SETWEIGHT(MLVL,600,25625) \
   SETHEIGHT(MLVL,240,2250) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 45, 0, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \

#define BASE_DRAGON_SILVER(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "sonic breath")   \
   MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,10,5,5,5,5,5) \
   alignment 1000 \
   race RACE_DRAGON_SILVER \
   SETWEIGHT(MLVL,525,25250) \
   SETHEIGHT(MLVL,210,2100) \
dilcopy aggressive@function(0, -1, 0, 0, 3, 45, 0, "ANY",\
               {"$1n attacks $3n!", "$1n attacks you!"}); \

#define BASE_DRAGON_LAVA(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "fire breath")   \
   MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,5,10,5,5,5,5) \
   alignment -1000 \
   race RACE_DRAGON_LAVA \
   SETWEIGHT(MLVL,450,24875) \
   SETHEIGHT(MLVL,180,1950) \
   exp 150

#define BASE_DRAGON_TURTLE(MLVL, GENDER, ARM)  \
   BASE_DRAGON(MLVL, GENDER,ARM, "light breath")/*change to colour */ \
MSET_ABILITY(25,10,10,10,20,10,15,0)  \
   MSET_WEAPON(5,5,5,5,15,5) \
   MSET_SPELL(5,5,5,5,5,10,5,5,5,5,5) \
   alignment 1000 \
   race RACE_DRAGON_TURTLE \
   SETWEIGHT(MLVL,375,20400) \
   SETHEIGHT(MLVL,150,1800) \
   exp 150

/* -------------------------------------------------------------------- */
/*                          BLACK DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_BLACK_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_BLACK_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_BLACK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLACK_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLACK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLACK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_BLACK(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          RED DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_RED_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_RED_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_RED_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_RED_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_RED_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_RED_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_RED(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          WHITE DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_WHITE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_WHITE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_WHITE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_WHITE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_WHITE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_WHITE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_WHITE(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          BLUE DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_BLUE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_BLUE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_BLUE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLUE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLUE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_BLUE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_BLUE(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          GREEN DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_GREEN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_GREEN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_GREEN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_GREEN_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_GREEN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_GREEN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_GREEN(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          SILVER DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_SILVER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_SILVER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_SILVER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_SILVER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_SILVER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_SILVER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_SILVER(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          LAVA DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_LAVA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_LAVA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_LAVA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_LAVA_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_LAVA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_LAVA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_LAVA(MLVL,GENDER,ARM_PLATE)

/* -------------------------------------------------------------------- */
/*                          TURTLE DRAGONS                                            */
/* -------------------------------------------------------------------- */
#define M_DRAGON_TURTLE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGON_TURTLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGON_TURTLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_TURTLE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_TURTLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_PLATE)

#define M_DRAGON_TURTLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGON_TURTLE(MLVL,GENDER,ARM_PLATE)



/* DRAGON end */

/* SCARECROW */

#define BASE_SCARECROW(MLVL,GENDER,ARM) \
 race RACE_HUMAN \
 level MLVL \
 sex GENDER \
 SETWEIGHT(MLVL,50,600) \
 SETHEIGHT(MLVL,120,360) \
 NATURAL_DEF(WPN_FIST,ARM) \
 alignment -900 \
 MSET_ABILITY(8,8,8,30,2,20,12,12) \
 MSET_WEAPON(6,6,6,6,15,6) \
 MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) 


#define M_SCARECROW_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)

#define M_SCARECROW_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)

#define M_SCARECROW_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)

#define M_SCARECROW_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)

#define M_SCARECROW_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)

#define M_SCARECROW_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,500,800) \
   SETHEIGHT(MLVL,360,720)

#define M_SCARECROW_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SCARECROW (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125
/* SCARECROW END */


/* SPRITE */

#define BASE_SPRITE(MLVL,GENDER,ARM) \
   race RACE_SPRITE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,43,24) \
   SETHEIGHT(MLVL,48,48) \
   NATURAL_DEF(WPN_CLAW, ARM)\
   exp 100 \
   alignment 0 \
   MSET_ABILITY(17,28,10,10,10,5,10,10)\
   MSET_WEAPON(5,5,5,5,20,5)\
   MSET_SPELL(18,2,2,2,2,2,19,2,2,2,2) 


#define M_SPRITE_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,42,66) \
   SETHEIGHT(MLVL,47,72)

#define M_SPRITE_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,60,84) \
   SETHEIGHT(MLVL,66,90)

#define M_SPRITE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,78,102 \
   SETHEIGHT(MLVL,84,108)

#define M_SPRITE_LARGE(MLVL,GENDER) \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,96,120) \
   SETHEIGHT(MLVL,102,126)

#define M_SPRITE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,114,138) \
   SETHEIGHT(MLVL,120,144)

#define M_SPRITE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,132,156) \
   SETHEIGHT(MLVL,138,162)

#define M_SPRITE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SPRITE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* SPRITE end */

/* Golems - A race of creatures created from a material but really have no shape */

#define BASE_GOLEM(MLVL,GENDER, ARM) \
    race RACE_GOLEM \
	sex GENDER \
    level MLVL \
    SETWEIGHT(MLVL,334,7500) \
    SETHEIGHT(MLVL,441,2800) \
    alignment 0 \
    NATURAL_DEF(WPN_CRUSH, ARM)  


#define M_GOLEM_STEEL(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_PLATE)\
    MSET_ABILITY(30,20,20,30,0,0,0,0)  \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_IRON(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_PLATE)\
    MSET_ABILITY(30,20,20,30,0,0,0,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_STONE(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_PLATE)\
    MSET_ABILITY(30,20,20,30,0,0,0,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_BRASS(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_CHAIN)\
	MSET_ABILITY(20,20,20,25,15,0,0,0) \
    MSET_WEAPON(10,30,10,10,10,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_TAR(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_HLEATHER)\
    MSET_ABILITY(13,13,13,15,15,11,20,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0) \
    alignment -1000 \
    exp 110 \
    dilcopy combat_mag@function("fireball","",0,2);

#define M_GOLEM_CLAY(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_LEATHER) \
    MSET_ABILITY(30,20,20,30,0,0,0,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_FLESH(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_CLOTHES) \
    MSET_ABILITY(30,20,20,30,0,0,0,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)

#define M_GOLEM_CLOUD(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_CLOTHES) \
    MSET_ABILITY(20,20,20,20,20,0,0,0) \
    MSET_WEAPON(10,10,10,10,30,10) \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0) \
	flags {UNIT_FL_INVISIBLE} \
    romflags {CHAR_DETECT_INVISIBLE, CHAR_SNEAK}

#define M_GOLEM_WOOD(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_CHAIN) \
    MSET_ABILITY(17,28,10,10,10,5,10,10) \
    MSET_WEAPON(5,5,5,5,20,5) \
    MSET_SPELL(18,2,2,2,2,2,19,2,2,2,2)


#define M_GOLEM_STRAW(MLVL,GENDER) \
    BASE_GOLEM(MLVL, GENDER, ARM_CLOTHES) \
    MSET_ABILITY(17,28,10,10,10,5,10,10) \
    MSET_WEAPON(5,5,5,5,20,5) \
    MSET_SPELL(18,2,2,2,2,2,19,2,2,2,2)

/* end golems */

/* MIST_DWELLER */

#define BASE_MIST_DWELLER(MLVL,GENDER,ARM) \
   race RACE_MIST_DWELLER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_FIST,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,50,215) \
   SETHEIGHT(MLVL,105,300) \
   MSET_ABILITY ( 10, 10, 10, 10, 25, 10, 25, 0) \
   MSET_WEAPON (1,1,2,2,2,2) \
   MSET_SPELL ( 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 40)\
   romflags {CHAR_DETECT_INVISIBLE,CHAR_SNEAK} \
   dilcopy combat_mag@function("acid ball", "", 25, 2); 


#define M_MIST_DWELLER_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,50,100) \
   SETHEIGHT(MLVL,110,140)

#define M_MIST_DWELLER_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,96,118) \
   SETHEIGHT(MLVL,135,165)

#define M_MIST_DWELLER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,107,130) \
   SETHEIGHT(MLVL,152,180)

#define M_MIST_DWELLER_LARGE(MLVL,GENDER) \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_CLOTHES)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,118,157) \
   SETHEIGHT(MLVL,172,220)

#define M_MIST_DWELLER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,140,214) \
   SETHEIGHT(MLVL,202,300)

#define M_MIST_DWELLER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,170,24) \
   SETHEIGHT(MLVL,250,279)

#define M_MIST_DWELLER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MIST_DWELLER (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125
/* mist dweller end */

/* SPIRIT */
#define BASE_SPIRIT(MLVL,GENDER,ARM) \
   race RACE_SPIRIT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,66,914) \
   SETHEIGHT(MLVL,58,800)\
   alignment -600 \
   NATURAL_DEF(WPN_CLAW, ARM) \
   MSET_ABILITY (20,15,15,15,10,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   exp 125 \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE, CHAR_SNEAK}\
   dilcopy poi_resist@function(); \
   dilcopy no_disarm@garden(); \
   dilcopy no_ambush@garden();


#define M_SPIRIT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_SPIRIT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_SPIRIT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_CLOTHES)

#define M_SPIRIT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_LEATHER)

#define M_SPIRIT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_LEATHER)

#define M_SPIRIT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_HLEATHER)

#define M_SPIRIT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SPIRIT (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy combat_mag@function("energy drain", "", 25, 2); \
   dilcopy aggressive@function(0,10,2,0,1,PULSE_SEC*12,-1,"ANY",\
   {"$1n screams as $1e attacks $1s prey, $3n!", \
    "$1n screams savagely as $1e attacks YOU!"}); \
   exp 130

/* SPIRIT end */

/* STATUE */
#define BASE_STATUE(MLVL,GENDER,ARM) \
   race RACE_STATUE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,66,914) \
   SETHEIGHT(MLVL,58,800)\
   alignment -600 \
   NATURAL_DEF(WPN_CLAW, ARM) \
   MSET_ABILITY (13,12,12,13,14,12,12,12) \
   MSET_WEAPON(8,8,8,8,15,8) \
   MSET_SPELL(4,4,4,4,4,4,4,4,4,5,4) \
   exp 125 \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE, CHAR_SNEAK}\
   dilcopy poi_resist@function(); \
   dilcopy no_disarm@garden(); \
   dilcopy no_ambush@garden();




#define M_STATUE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_STATUE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER   \
   SETWEIGHT(MLVL,66,142) \
   SETHEIGHT(MLVL,58,125)

#define M_STATUE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_STATUE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,133,206) \
   SETHEIGHT(MLVL,116,180)

#define M_STATUE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_STATUE (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,200,275) \
   SETHEIGHT(MLVL,175,240)

#define M_STATUE_LARGE(MLVL,GENDER) \
   BASE_STATUE (MLVL,GENDER,ARM_LEATHER)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,265,342) \
   SETHEIGHT(MLVL,232,300)

#define M_STATUE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_STATUE (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,331,400) \
   SETHEIGHT(MLVL,290,350)

#define M_STATUE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_STATUE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,397,914) \
   SETHEIGHT(MLVL,348,800)

#define M_STATUE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_STATUE (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy combat_mag@function("energy drain", "", 25, 2); \
   dilcopy aggressive@function(0,10,2,0,1,PULSE_SEC*12,-1,"ANY",\
   {"$1n screams as $1e attacks $1s prey, $3n!", \
    "$1n screams savagely as $1e attacks YOU!"}); \
   exp 130

/* STATUE end */


/* GHOSTS */
#define BASE_GHOST(MLVL, GENDER, ARM)  \
   race RACE_GHOST \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -100  /* not all ghosts are evil */ \
   romflags {CHAR_DETECT_INVISIBLE} \
   SETWEIGHT(MLVL,2,200) \
   SETHEIGHT(MLVL,30,300) \
   dilcopy poi_resist@function(); \
   MSET_ABILITY(18,7,8,17,25,0,0,25) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(3,3,3,3,3,3,3,3,3,25,3) \
   dilcopy no_disarm@garden(); \
   dilcopy no_ambush@garden();





#define BASE_GHOST_INVIS(MLVL, GENDER, ARM)  \
   BASE_GHOST(MLVL,GENDER,ARM) \
   race RACE_GHOST \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -750  /*these are evil */ \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy poi_resist@function(); \
   MSET_ABILITY (8,7,8,7,25,20,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2); \
   dilcopy comb_poison@function("<div class='cpr'>You feel fire burning within your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*60, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 170

/* -------------------------------------------------------------------- */
/*                   GHOSTS                                            */
/* -------------------------------------------------------------------- */
#define M_GHOST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOST_MAD(MLVL,GENDER) /* level 1 to 199 */ \
   BASE_GHOST(MLVL,GENDER,ARM_CLOTHES)  \
   dilcopy comb_poison@function("<div class='cpr'>You feel poison burning your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 130

/* -------------------------------------------------------------------- */
/*            INVISIBLE MEAN GHOSTS                                     */
/* -------------------------------------------------------------------- */

#define M_INVIS_GHOST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOST_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GHOST_INVIS(MLVL,GENDER,ARM_CLOTHES)

/* GHOSTS END */

/* UNICORN */
#define BASE_UNICORN(MLVL,GENDER,ARM) \
   race RACE_UNICORN \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_STING,ARM) \
   alignment 1000 \
   SETWEIGHT(MLVL,150,4000) \
   SETHEIGHT(MLVL,75,360) \
   MSET_ABILITY(10,15,10,15,15,10,0,25) \
   MSET_WEAPON( 5,10,5,10,5,5) \
   MSET_SPELL( 10,0,0,10,0,13,5,6,6,5,5) \
   exp 110 \
   dilcopy combat_mag@function("colourspray", "heal", 25, 2); 


#define M_UNICORN_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_UNICORN (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER  \
   SETWEIGHT(MLVL,150,225) \
   SETHEIGHT(MLVL,75,105)

#define M_UNICORN_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_UNICORN (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,225,400) \
   SETHEIGHT(MLVL,105,135)

#define M_UNICORN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_UNICORN (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,400,900) \
   SETHEIGHT(MLVL,135,165)

#define M_UNICORN_LARGE(MLVL,GENDER) \
   BASE_UNICORN (MLVL,GENDER,ARM_HLEATHER)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,900,1500) \
   SETHEIGHT(MLVL,165,195)

#define M_UNICORN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_UNICORN (MLVL,GENDER,ARM_CHAIN)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1500,2000) \
   SETHEIGHT(MLVL,195,225)

#define M_UNICORN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_UNICORN (MLVL,GENDER,ARM_PLATE)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,2000,4000) \
   SETHEIGHT(MLVL,225,360)

/* UNICORN end */

/* SLIME & DEVOURERS */
/*will auto wander zone loaded into */
#define BASE_SLIME(MLVL,GENDER,ARM) \
   race RACE_SLIME \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,30,6000) /* based on orginal one for height and weight */ \
   SETHEIGHT(MLVL,19,3621) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 0 \
   MSET_ABILITY(25,10,25,30,10,0,0,0) \
   MSET_WEAPON(6,6,6,35,6,6) \
   MSET_SPELL(3,3,3,3,3,5,3,3,3,3,3) \
   dilcopy wander_zones@function("@loadzone", 60, 0, 0); 


#define BASE_SLIME_AGGR(MLVL,GENDER,ARM) \
   BASE_SLIME(MLVL,GENDER,ARM) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   alignment -500 \
   MSET_ABILITY(14,16,16,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,25,11) /* Attack poison  */ \
   dilcopy comb_poison@function("<div class='cpr'>Poison burns in your veins!</div>", \
   "<div class='hit_other'>$3n winces as $1n stings $1m!</div>"); \
   dilcopy wander_zones@function("@loadzone", 80, 0, 0); \
   exp 125

/* non aggro */
#define M_SLIME_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SLIME (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SLIME (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SLIME (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SLIME (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SLIME (MLVL,GENDER,ARM_LEATHER)

#define M_SLIME_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SLIME (MLVL,GENDER,ARM_LEATHER)

#define M_SLIME_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SLIME (MLVL,GENDER,ARM_HLEATHER) \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
    dilcopy comb_poison@function("<div class='cpr'>Poison burns in your veins!</div>", \
   "<div class='hit_other'>$3n winces as $1n stings $1m!</div>"); \
   dilcopy wander_zones@function("@loadzone", 80, 0, 0); \
   exp 125

/* aggro slime */
#define M_SLIME_AGGR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_AGGR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_AGGR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_AGGR_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_CLOTHES)

#define M_SLIME_AGGR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_LEATHER)

#define M_SLIME_AGGR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SLIME_AGGR (MLVL,GENDER,ARM_LEATHER)


/* SLIME end */


/* MUCK_LURKER */
/*will auto wander zone loaded into */
#define BASE_MUCK_LURKER(MLVL,GENDER,ARM) \
   race RACE_SLIME \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,9,1741) /* based on orginal one for height and weight */ \
   SETHEIGHT(MLVL,10,2040) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 0 \
   MSET_ABILITY(20,15,20,15,15,15,0,0) \
   MSET_WEAPON(8,5,17,5,25,9) /*resist blunt weapons*/ \
   MSET_SPELL(3,0,0,0,0,5,3,3,3,7,7) /*slight resist vs poison/acid*/ \
   dilcopy wander_zones@function("@loadzone", 80, 0, 0); \
   exp 105 



#define M_MUCK_LURKER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_CLOTHES)

#define M_MUCK_LURKER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_CLOTHES)

#define M_MUCK_LURKER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_CLOTHES)

#define M_MUCK_LURKER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_CLOTHES)

#define M_MUCK_LURKER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_LEATHER)

#define M_MUCK_LURKER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_LEATHER)

#define M_MUCK_LURKER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MUCK_LURKER (MLVL,GENDER,ARM_HLEATHER) \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY", \
                  {"$1n attacks $3n!", "$1n attacks you!"}); \
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   exp 125

/* MUCK_LURKER end */

/*mimics use acid */

#define BASE_MIMIC(MLVL,GENDER,ARM) \
   race RACE_MIMIC \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,30,6000) /* based on slime for height and weight */ \
   SETHEIGHT(MLVL,19,3621) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 0 \
   MSET_ABILITY(25,10,25,30,10,0,0,0) \
   MSET_WEAPON(6,6,6,6,35,6) \
   MSET_SPELL(3,3,3,3,3,3,3,3,3,3,5)


#define BASE_MIMIC_AGGR(MLVL,GENDER,ARM) \
   BASE_MIMIC(MLVL,GENDER,ARM) \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   alignment 0 \
   MSET_ABILITY(14,16,16,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(4,2,2,2,2,2,0,2,2,11,25) /* Attack acid  */ \
   dilcopy comb_acid@function("<div class='cpr'>Your skin feels like it is on fire!</div>", \
   "<div class='hit_other'>$3n winces as $1n melts $3s skin with acid!</div>"); \
   exp 135

/* non aggro */
#define M_MIMIC_TINY(MLVL) /*level 1 to 25 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_SMALL(MLVL) /* level 26 to 50 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_MEDIUM(MLVL) /* level 51 to 75 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_LARGE(MLVL) /* level 76 to 100 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_XLARGE(MLVL) /* level 101 to 120 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_GODLY(MLVL) /* level 121 to 199 */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_MAD(MLVL) /* level 1 to 199 has acid and aggro */ \
   BASE_MIMIC (MLVL,SEX_NEUTRAL,ARM_CHAIN) \
   alignment 0  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(4,4,4,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,3,6,6,6,15)\
dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
dilcopy comb_acid@function("<div class='cpr'>Your skin feels like it is on fire!</div>", \
   "<div class='hit_other'>$3n winces as $1n melts $3s skin with acid!</div>"); \
   exp 135
   
/* aggro mimic */
#define M_MIMIC_AGGR_TINY(MLVL) /*level 1 to 25 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_AGGR_SMALL(MLVL) /* level 26 to 50 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_AGGR_MEDIUM(MLVL) /* level 51 to 75 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_AGGR_LARGE(MLVL) /* level 76 to 100 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_AGGR_XLARGE(MLVL) /* level 101 to 120 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)

#define M_MIMIC_AGGR_GODLY(MLVL) /* level 121 to 199 */ \
   BASE_MIMIC_AGGR (MLVL,SEX_NEUTRAL,ARM_CHAIN)


/* MIMIC end */

/* MUMMIE */
#define BASE_MUMMIE(MLVL,GENDER,ARM) \
race RACE_MUMMIE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment -550 \
SETWEIGHT(MLVL,122,384) \
SETHEIGHT(MLVL,130,211) \
MSET_ABILITY(15,15,15,20,10,15,0,10) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) 


#define M_MUMMIE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MUMMIE (MLVL,GENDER,ARM_CLOTHES)

#define M_MUMMIE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MUMMIE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n bumps into $3n and attacks!", \
		 "$1n bumps into you and attacks!"}); \
exp 125

/* MUMMIE end */


/* ZOMBIE */
#define BASE_ZOMBIE(MLVL,GENDER,ARM) \
race RACE_ZOMBIE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment -550 \
SETWEIGHT(MLVL,150,400) \
SETHEIGHT(MLVL,150,230) \
MSET_ABILITY(15,15,15,10,10,15,10,10) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  


#define BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM) \
race RACE_ZOMBIE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment -550 \
SETWEIGHT(MLVL,180,600) \
SETHEIGHT(MLVL,150,230) \
MSET_ABILITY(20,20,12,12,10,0,13,13) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) 


/* -------------------------------------------------------------------- */
/*                   ZOMBIES                                            */
/* -------------------------------------------------------------------- */
#define M_ZOMBIE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ZOMBIE(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_ZOMBIE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n bumps into $3n and attacks!", \
		 "$1n bumps into you and attacks!"}); \
exp 125


/* -------------------------------------------------------------------- */
/*            NO WANDER GUARD ZOMBIES                                   */
/* -------------------------------------------------------------------- */

#define M_ZOMBIE_GUARD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ZOMBIE_GUARD(MLVL,GENDER,ARM_CLOTHES)

#define M_ZOMBIE_GUARD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_ZOMBIE (MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n bumps into $3n and attacks!", \
		 "$1n bumps into you and attacks!"}); \
exp 125

/* ZOMBIES END */

/* GHOULS */
#define BASE_GHOUL(MLVL, GENDER, ARM)  \
   race RACE_GHOUL \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -100  /* not all GHOULs are evil */ \
   romflags {CHAR_DETECT_INVISIBLE} \
   SETWEIGHT(MLVL,130,350) \
   SETHEIGHT(MLVL,120,300) \
   dilcopy poi_resist@function(); \
   MSET_ABILITY (10,30,15,15,5,0,10,15) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2); \
   exp 120 


#define BASE_GHOUL_INVIS(MLVL, GENDER, ARM)  \
   BASE_GHOUL(MLVL,GENDER,ARM) \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -750  /*these are evil */ \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy poi_resist@function(); \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy comb_poison@function("<div class='cpr'>You feel fire burning within your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*60, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 150

/* -------------------------------------------------------------------- */
/*                   GHOULS                                            */
/* -------------------------------------------------------------------- */
#define M_GHOUL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)

#define M_GHOUL_MAD(MLVL,GENDER) /* level 1 to 199 */ \
   BASE_GHOUL(MLVL,GENDER,ARM_CLOTHES)  \
   dilcopy comb_poison@function("<div class='cpr'>You feel poison burning your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s claws deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 130

/* -------------------------------------------------------------------- */
/*            INVISIBLE MEAN GHOULS                                     */
/* -------------------------------------------------------------------- */

#define M_INVIS_GHOUL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOUL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOUL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOUL_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOUL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

#define M_INVIS_GHOUL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GHOUL_INVIS(MLVL,GENDER,ARM_CLOTHES)

/* GHOULS END */

/* LICH */
#define BASE_LICH(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   SETWEIGHT(MLVL,50,995) \
   SETHEIGHT(MLVL,70,540) \
   alignment -1000 \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_WEAPON( 5,5,5,5,15,5) /* 40 */ \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   flags{UNIT_FL_NO_TELEPORT} \
   exp 120 


#define BASE_LICH_AIR(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_AIR \
   MSET_ABILITY(10,10,10,15,20,15,0,20) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,10,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, -2, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
      dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
	exp 130

#define BASE_LICH_EARTH(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_EARTH \
   MSET_ABILITY(10,10,10,15,20,15,0,20) \
   MSET_SPELL( 5,5,5,5,10,5,5,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 2, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130

#define BASE_LICH_FIRE(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_FIRE \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,10,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 1, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130

#define BASE_LICH_FROST(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_FROST \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,10,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
    dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
    exp 130

#define BASE_LICH_LIGHT(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_LIGHT \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,10,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, -1, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130

#define BASE_LICH_WATER(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_LICH(MLVL,GENDER,ARM) \
   race RACE_LICH_WATER \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*10, -2, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130


#define M_LICH_AIR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_LEATHER,"fart")

#define M_LICH_AIR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_LEATHER, "fart")

#define M_LICH_AIR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_LEATHER, "bad breath")

#define M_LICH_AIR_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_HLEATHER, "stinking cloud")

#define M_LICH_AIR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_CHAIN, "stinking cloud")

#define M_LICH_AIR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_AIR(MLVL,GENDER,ARM_PLATE, "stinking breath")

#define M_LICH_EARTH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_LICH_EARTH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_LICH_EARTH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_LICH_EARTH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_HLEATHER, "earthquake")

#define M_LICH_EARTH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_CHAIN, "earthquake")

#define M_LICH_EARTH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_EARTH(MLVL,GENDER,ARM_PLATE, "earthquake")

#define M_LICH_FIRE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_LEATHER, "burn")

#define M_LICH_FIRE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_LICH_FIRE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_LICH_FIRE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_HLEATHER, "fore storm")

#define M_LICH_FIRE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_CHAIN, "fire storm")

#define M_LICH_FIRE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_FIRE(MLVL,GENDER,ARM_PLATE, "fire breath")

#define M_LICH_FROST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_LEATHER, "snowball")

#define M_LICH_FROST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_LICH_FROST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_LICH_FROST_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_HLEATHER, "iceball")

#define M_LICH_FROST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_CHAIN, "iceball")

#define M_LICH_FROST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_FROST(MLVL,GENDER,ARM_PLATE, "icestorm")

#define M_LICH_LIGHT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_LICH_LIGHT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_LICH_LIGHT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_LICH_LIGHT_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_HLEATHER, "energy drain")

#define M_LICH_LIGHT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_CHAIN, "energy drain")

#define M_LICH_LIGHT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_LIGHT(MLVL,GENDER,ARM_LEATHER, "energy drain")

#define M_LICH_WATER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_LEATHER, "acid drop")

#define M_LICH_WATER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_LICH_WATER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_LICH_WATER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_HLEATHER, "acid ball")

#define M_LICH_WATER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_CHAIN, "acid ball")

#define M_LICH_WATER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LICH_WATER(MLVL,GENDER,ARM_PLATE, "acid swirl")

/* LICH end */

/* ELEMENTAL */
#define BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CRUSH,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,18,1466) \
   SETHEIGHT(MLVL,60,1257) \
   MSET_ABILITY(10,10,10,15,20,15,20,0) \
   MSET_WEAPON( 5,5,5,5,15,5) /* 40 */ \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   flags{UNIT_FL_NO_TELEPORT} \
   exp 120 


#define BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM,ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_AIR \
   alignment +650 \
   MSET_ABILITY(15,15,10,10,20,10,10,10) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,10,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "EVIL", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130

#define BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM,ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_EARTH \
   alignment +350 \
   MSET_ABILITY(15,10,10,15,15,15,10,10) \
   MSET_SPELL( 5,5,5,5,10,5,5,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "EVIL", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
   exp 130

#define BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM,ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_FIRE \
   alignment -350 \
   MSET_ABILITY(10,10,10,15,20,15,10,10) \
   MSET_SPELL( 5,5,5,5,5,5,10,5,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "GOOD", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
  exp 130

#define BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM,ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_FROST \
   alignment 0 \
   MSET_ABILITY(10,10,10,15,20,15,10,10) \
   MSET_SPELL( 5,5,5,5,5,5,5,10,5,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "ANY", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
	exp 130

#define BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_LIGHT \
   alignment +900 \
   MSET_ABILITY(10,10,10,15,20,15,10,10) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,10,5,5) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "EVIL", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
	exp 130

#define BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM, ATT_SPL) \
   BASE_ELEMENTAL(MLVL,GENDER,ARM) \
   race RACE_ELEMENTAL_WATER \
   alignment -550 \
   MSET_ABILITY(10,10,10,15,20,15,10,10) \
   MSET_SPELL( 5,5,5,5,5,5,5,5,5,5,10) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*20, 0, "GOOD", \
    {"$1n attacks $3n!","$1n attacks you!"}); \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2); \
	exp 130


#define M_ELEMENTAL_AIR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_LEATHER,"fart")

#define M_ELEMENTAL_AIR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_LEATHER, "fart")

#define M_ELEMENTAL_AIR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_LEATHER, "bad breath")

#define M_ELEMENTAL_AIR_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_HLEATHER, "stinking cloud")

#define M_ELEMENTAL_AIR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_CHAIN, "stinking cloud")

#define M_ELEMENTAL_AIR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_AIR(MLVL,GENDER,ARM_PLATE, "stinking breath")

#define M_ELEMENTAL_EARTH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_LEATHER,"earthquake")

#define M_ELEMENTAL_EARTH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_ELEMENTAL_EARTH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_LEATHER, "earthquake")

#define M_ELEMENTAL_EARTH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_HLEATHER, "earthquake")

#define M_ELEMENTAL_EARTH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_CHAIN, "earthquake")

#define M_ELEMENTAL_EARTH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_EARTH(MLVL,GENDER,ARM_PLATE, "earthquake")

#define M_ELEMENTAL_FIRE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_LEATHER, "burn")

#define M_ELEMENTAL_FIRE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_ELEMENTAL_FIRE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_LEATHER, "fireball")

#define M_ELEMENTAL_FIRE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_HLEATHER, "fire storm")

#define M_ELEMENTAL_FIRE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_CHAIN, "fire storm")

#define M_ELEMENTAL_FIRE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_FIRE(MLVL,GENDER,ARM_PLATE, "fire breath")

#define M_ELEMENTAL_FROST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_LEATHER, "snowball")

#define M_ELEMENTAL_FROST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_ELEMENTAL_FROST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_LEATHER, "frostball")

#define M_ELEMENTAL_FROST_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_HLEATHER, "iceball")

#define M_ELEMENTAL_FROST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_CHAIN, "iceball")

#define M_ELEMENTAL_FROST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_FROST(MLVL,GENDER,ARM_PLATE, "icestorm")

#define M_ELEMENTAL_LIGHT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_LEATHER, "electrical shock")

#define M_ELEMENTAL_LIGHT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_LEATHER, "jolt")

#define M_ELEMENTAL_LIGHT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_LEATHER, "jolt")

#define M_ELEMENTAL_LIGHT_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_HLEATHER, "lightning bolt")

#define M_ELEMENTAL_LIGHT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_CHAIN, "lightning bolt")

#define M_ELEMENTAL_LIGHT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_LIGHT(MLVL,GENDER,ARM_PLATE, "lightning storm")

#define M_ELEMENTAL_WATER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_LEATHER, "acid drop")

#define M_ELEMENTAL_WATER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_ELEMENTAL_WATER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_LEATHER, "spray of acid")

#define M_ELEMENTAL_WATER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_HLEATHER, "acid ball")

#define M_ELEMENTAL_WATER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_CHAIN, "acid ball")

#define M_ELEMENTAL_WATER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEMENTAL_WATER(MLVL,GENDER,ARM_PLATE, "acid swirl")

/* ELEMENTAL end */

/* YOGOLOTH */
#define BASE_YOGOLOTH(MLVL,GENDER,ARM) \
   race RACE_YOGOLOTH \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_STING,ARM) \
   alignment 1000 \
   SETWEIGHT(MLVL,18,3582) \
   SETHEIGHT(MLVL,30,1816) \
   MSET_ABILITY(20,12,12,12,12,12,20,0) \
   MSET_WEAPON( 5,10,5,0,15,5) \
   MSET_SPELL( 10,0,0,10,0,13,5,6,6,5,5) \
   dilcopy combat_mag@function("colourspray", "", 25, 2); \
   exp 120 



#define M_YOGOLOTH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_LEATHER)

#define M_YOGOLOTH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_LEATHER)

#define M_YOGOLOTH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_LEATHER)

#define M_YOGOLOTH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_HLEATHER)

#define M_YOGOLOTH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_CHAIN)

#define M_YOGOLOTH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_YOGOLOTH (MLVL,GENDER,ARM_PLATE)

/* YOGOLOTH end */

/* DEMONS */
#define BASE_DEMON(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,160,800) /* based on orginal one for height and weight */ \
   SETHEIGHT(MLVL,100,360) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -1000 \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(6,21,6,5,21,6) \
   MSET_SPELL(3,3,3,3,3,5,3,3,3,3,3) \
dilcopy aggressive@function(0, -1, 0, 20, 2, PULSE_SEC*6, 1, "GOOD", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); 


#define BASE_DEMON_LESSER(MLVL,GENDER,ARM) \
   BASE_DEMON(MLVL,GENDER,ARM) \
   race RACE_LESSER_DEMON \
   NATURAL_DEF(WPN_CLAW, ARM) \
   MSET_ABILITY(8,8,8,16,15,15,30,0) /* 100 % */ \
   MSET_WEAPON(2,11,2,2,11,2)           /*  30 % */ \
   MSET_SPELL(7,1,1,5,1,11,7,2,25,6,4) /*  70 % */ \
   dilcopy combat_mag@function("lightning bolt", "", 25, 2); \
   exp 125

#define BASE_DEMON_GREATER(MLVL,GENDER,ARM) \
   BASE_DEMON(MLVL,GENDER,ARM) \
   race RACE_GREATER_DEMON \
   NATURAL_DEF(WPN_CRUSH, ARM) \
   MSET_ABILITY(10,10,10,10,15,15,30,0) /* 100 % */ \
   MSET_WEAPON(2,11,2,2,11,2)           /*  30 % */ \
   MSET_SPELL(7,1,1,5,1,11,7,2,4,6,25) /*  70 % */ \
   alignment -1000 \
   dilcopy combat_mag@function("acid ball", "", 25, 2); \
   exp 125
   
  /*added for soul lake*/
#define BASE_SNAKE_DEMON(MLVL,GENDER,ARM)\
   BASE_DEMON(MLVL,GENDER,ARM) \
   race RACE_SNAKE_DEMON \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,3,110) \
   SETHEIGHT(MLVL,15,270) \
   NATURAL_DEF(WPN_BITE, ARM)\
   alignment -1000 \
   MSET_ABILITY(6,14,10,10,10,20,15,15)\
   MSET_WEAPON(5,5,5,5,20,5)\
   MSET_SPELL(3,3,3,30,3,3,2,2,2,2,2)\
   romflags {CHAR_DETECT_INVISIBLE}  \
   dilcopy aggressive@function(0, -1, 0, 20, 2, PULSE_SEC*6, 1, "GOOD", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125   


/* non aggro */
#define M_DEMON_GREATER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_GREATER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_GREATER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_GREATER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_GREATER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_LEATHER)

#define M_DEMON_GREATER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DEMON_GREATER (MLVL,GENDER,ARM_LEATHER)


/* aggro DEMON */
#define M_DEMON_LESSER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_LESSER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_LESSER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_LESSER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_CLOTHES)

#define M_DEMON_LESSER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_LEATHER)

#define M_DEMON_LESSER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DEMON_LESSER (MLVL,GENDER,ARM_LEATHER)
   
/*Snake Demon*/
#define M_SNAKE_DEMON_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_DEMON_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_DEMON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_DEMON_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_DEMON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_DEMON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SNAKE_DEMON(MLVL,GENDER,ARM_LEATHER)


/* DEMON end */
   


/* DEMON end */

/* CAVE_WIGHTS */
#define BASE_CAVE_WIGHT(MLVL,GENDER,ARM) \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,160,800) \
   SETHEIGHT(MLVL,100,360) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment -800 \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
   MSET_ABILITY(30,20,20,15,5,2,6,2) \
   MSET_WEAPON(5,5,5,5,40,5) /* 65 % */ \
   MSET_SPELL(0,5,5,0,0,0,0,5,5,10,5) 


#define M_CAVE_WIGHT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_LEATHER)

#define M_CAVE_WIGHT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CAVE_WIGHT(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n claws you!", \
                           "$1n claws $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CAVE_WIGHT end */

/* UR_VILE   */
#define BASE_UR_VILE(MLVL,GENDER,ARM) \
race RACE_UR_VILE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_FIST,ARM) \
alignment -950 \
SETWEIGHT(MLVL,90,175) /* pounds */ \
SETHEIGHT(MLVL,60,110) /* cm            */ \
MSET_ABILITY(18,15,12,20,10,5,15,5) /* 100 % */ \
MSET_WEAPON(6,6,6,6,40,6) /* 65 % */ \
MSET_SPELL(5,2,3,0,0,0,0,5,5,5,5) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} 


#define M_UR_VILE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_UR_VILE(MLVL,GENDER,ARM_LEATHER)

#define M_UR_VILE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_UR_VILE(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* UR_VILE end */

/* DEATH GHAST */
#define BASE_DEATH_GHAST(MLVL,GENDER,ARM) \
   race RACE_DEATH_GHAST \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1000,1500) \
   SETHEIGHT(MLVL,210,420) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment -900       \
   NATURAL_DEF(WPN_CLAW, ARM)         \
   MSET_ABILITY(20,20,20,20,10,10,0,0)     \
   MSET_WEAPON(0,0,0,10,15,0)             \
   MSET_SPELL(10,10,10,10,10,10,10,5,0,0,0) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*6, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 105 




#define M_DEATH_GHAST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_LARGE(MLVL,GENDER) /*level 76 to 100 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)

#define M_DEATH_GHAST_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_DEATH_GHAST (MLVL,GENDER,ARM_LEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   exp 125

/* DEATH_GHAST end */


/* SHADOW BEAST */
#define BASE_SHADOW_BEAST(MLVL,GENDER,ARM) \
   race RACE_SHADOW_BEAST \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,300,800) \
   SETHEIGHT(MLVL,180,550) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment -900      /* Neutral        */ \
   NATURAL_DEF(WPN_CLAW, ARM)         \
   MSET_ABILITY(20,20,20,20,10,10,0,0)     \
   MSET_WEAPON(0,0,0,10,15,0)             \
   MSET_SPELL(10,10,10,10,10,10,10,5,0,0,0) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, PULSE_SEC*6, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 105 




#define M_SHADOW_BEAST_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_CLOTHES)

#define M_SHADOW_BEAST_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_CLOTHES)

#define M_SHADOW_BEAST_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_CLOTHES)

#define M_SHADOW_BEAST_LARGE(MLVL,GENDER) /*level 76 to 100 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_LEATHER)

#define M_SHADOW_BEAST_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_LEATHER)

#define M_SHADOW_BEAST_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_LEATHER)

#define M_SHADOW_BEAST_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SHADOW_BEAST (MLVL,GENDER,ARM_CLOTHES)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   exp 125

/* SHADOW_BEAST end */


/* SKELETON */
#define BASE_SKELETON(MLVL,GENDER,ARM) \
   race RACE_SKELETON \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,225) \
   SETHEIGHT(MLVL,120,300) \
   NATURAL_DEF(WPN_FIST, ARM) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(10,10,20,20,20,0,10,10) /* Is that a SKELETON? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */ 



#define M_SKELETON_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_CLOTHES)

#define M_SKELETON_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_CLOTHES)

#define M_SKELETON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_CLOTHES)

#define M_SKELETON_LARGE(MLVL,GENDER) /*level 76 to 100 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_CLOTHES)

#define M_SKELETON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_LEATHER)

#define M_SKELETON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SKELETON (MLVL,GENDER,ARM_LEATHER)

#define M_SKELETON_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SKELETON (MLVL,GENDER,ARM_CLOTHES)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, -1, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125



/* SKELETON end */

/* YETI */

#define BASE_YETI(MLVL,GENDER,ARM) \
   race RACE_YETI \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_FIST,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,50,215) \
   SETHEIGHT(MLVL,105,300) \
   MSET_ABILITY ( 10, 10, 10, 10, 25, 10, 25, 0) \
   MSET_WEAPON (1,1,2,2,2,2) \
   MSET_SPELL ( 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 40)\
   romflags {CHAR_DETECT_INVISIBLE,CHAR_SNEAK} \
   dilcopy combat_mag@function("acid ball", "", 25, 2); 


#define M_YETI_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_YETI (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,50,100) \
   SETHEIGHT(MLVL,110,140)

#define M_YETI_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_YETI (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,96,118) \
   SETHEIGHT(MLVL,135,165)

#define M_YETI_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_YETI (MLVL,GENDER,ARM_CLOTHES)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,107,130) \
   SETHEIGHT(MLVL,152,180)

#define M_YETI_LARGE(MLVL,GENDER) \
   BASE_YETI (MLVL,GENDER,ARM_CLOTHES)  /* level 76 to 100 */ \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,118,157) \
   SETHEIGHT(MLVL,172,220)

#define M_YETI_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_YETI (MLVL,GENDER,ARM_LEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,140,214) \
   SETHEIGHT(MLVL,202,300)

#define M_YETI_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_YETI (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,170,24) \
   SETHEIGHT(MLVL,250,279)

#define M_YETI_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_YETI (MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125
/* YETI end */




#endif
