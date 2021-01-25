
#ifndef _MUD_MONSTER_H
#define _MUD_MONSTER_H

#include <bottom.h>

/* CREEPER */
#define BASE_CREEPER(MLVL,GENDER,ARM) \
   race RACE_CREEPER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -1000 \
   SETWEIGHT(MLVL,9,1836) \
   SETHEIGHT(MLVL,3,566) \
   MSET_ABILITY(30,25,2,27,10,3,0,3) \
   MSET_WEAPON(22,25,20,0,20,0) \
   MSET_SPELL(1,1,1,0,1,1,1,1,1,2,3) \
   romflags {CHAR_DETECT_INVISIBLE}

#define M_CREEPER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_LEATHER)

#define M_CREEPER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_LEATHER)

#define M_CREEPER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_LEATHER)

#define M_CREEPER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_HLEATHER)

#define M_CREEPER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_CHAIN)

#define M_CREEPER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CREEPER(MLVL,GENDER,ARM_PLATE)


#define M_CREEPER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CREEPER(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

#define M_CREEPER_AGG(MLVL,GENDER) /* level 1 to 199 has aggro */ \
BASE_CREEPER(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 110
/* CREEPER end */

/* OSTRICH */
#define BASE_OSTRICH(MLVL,GENDER,ARM) \
race RACE_OSTRICH \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,500) \
SETHEIGHT(MLVL,18,240) \
MSET_ABILITY(17,17,19,10,12,9,8,8) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_OSTRICH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_LEATHER)

#define M_OSTRICH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_LEATHER)

#define M_OSTRICH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_LEATHER)

#define M_OSTRICH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_HLEATHER)

#define M_OSTRICH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_CHAIN)

#define M_OSTRICH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_PLATE)


#define M_OSTRICH_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_OSTRICH(MLVL,GENDER,ARM_HLEATHER)  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n stings you!", \
                           "$1n stings $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n stings $3n!", "$1n stings you!"}); \
exp 125

/* OSTRICH end */


/* BEE */
#define BASE_BEE(MLVL,GENDER,ARM) \
race RACE_BEE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,3) \
SETHEIGHT(MLVL,6,30) \
MSET_ABILITY(17,17,19,10,12,9,8,8) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_BEE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BEE(MLVL,GENDER,ARM_LEATHER)

#define M_BEE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BEE(MLVL,GENDER,ARM_LEATHER)

#define M_BEE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BEE(MLVL,GENDER,ARM_LEATHER)

#define M_BEE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_BEE(MLVL,GENDER,ARM_HLEATHER)

#define M_BEE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BEE(MLVL,GENDER,ARM_CHAIN)

#define M_BEE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BEE(MLVL,GENDER,ARM_PLATE)


#define M_BEE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BEE(MLVL,GENDER,ARM_HLEATHER)  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n stings you!", \
                           "$1n stings $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n stings $3n!", "$1n stings you!"}); \
exp 125

/* BEE end */


/* BEETLE */
#define BASE_BEETLE(MLVL,GENDER,ARM) \
race RACE_BEETLE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,30) \
SETHEIGHT(MLVL,18,60) \
MSET_ABILITY(17,17,19,10,12,9,8,8) \
MSET_WEAPON(6,6,6,6,15,6) \
MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_BEETLE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_LEATHER)

#define M_BEETLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_LEATHER)

#define M_BEETLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_LEATHER)

#define M_BEETLE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_HLEATHER)

#define M_BEETLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_CHAIN)

#define M_BEETLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BEETLE(MLVL,GENDER,ARM_PLATE)


#define M_BEETLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BEETLE(MLVL,GENDER,ARM_HLEATHER)  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* BEETLE end */

/* MOSQUITO   */
#define BASE_MOSQUITO(MLVL,GENDER,ARM) \
race RACE_MOSQUITO \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,10) \
SETHEIGHT(MLVL,7,60) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,4,4,3,3,4,4,3,3,4) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} \
dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"});

#define M_MOSQUITO_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_LEATHER)

#define M_MOSQUITO_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_MOSQUITO(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
exp 125

/* MOSQUITO end */

/* DINOSAUR */
#define BASE_DINOSAUR(MLVL,GENDER,ARM) \
   race RACE_DINOSAUR \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   SETWEIGHT(MLVL,1,5000) \
   SETHEIGHT(MLVL,30,600) \
   MSET_ABILITY(17,17,19,10,12,9,8,8) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define BASE_DINOSAUR_BITE(MLVL,GENDER,ARM) \
   BASE_DINOSAUR(MLVL,GENDER,ARM)

#define BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM) \
   BASE_DINOSAUR(MLVL,GENDER,ARM)

#define BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM) \
   BASE_DINOSAUR(MLVL,GENDER,ARM)

#define M_DINOSAUR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_HLEATHER)

#define M_DINOSAUR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_CHAIN)

#define M_DINOSAUR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DINOSAUR(MLVL,GENDER,ARM_PLATE)

#define M_DINOSAUR_BITE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_BITE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_BITE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_BITE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_HLEATHER)

#define M_DINOSAUR_BITE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_CHAIN)

#define M_DINOSAUR_BITE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DINOSAUR_BITE(MLVL,GENDER,ARM_PLATE)

#define M_DINOSAUR_CLAW_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CLAW_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CLAW_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CLAW_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_HLEATHER)

#define M_DINOSAUR_CLAW_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_CHAIN)

#define M_DINOSAUR_CLAW_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DINOSAUR_CLAW(MLVL,GENDER,ARM_PLATE)

#define M_DINOSAUR_CRUSH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CRUSH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CRUSH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_LEATHER)

#define M_DINOSAUR_CRUSH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_HLEATHER)

#define M_DINOSAUR_CRUSH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_CHAIN)

#define M_DINOSAUR_CRUSH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DINOSAUR_CRUSH(MLVL,GENDER,ARM_PLATE)

/* DINOSAUR end */




/* LEOPARD */
#define BASE_LEOPARD(MLVL,GENDER,ARM) \
   race RACE_LEOPARD \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,85,350) \
   SETHEIGHT(MLVL,60,120) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 0  \
   MSET_ABILITY(15,25,30,20,10,0,0,0) \
   MSET_WEAPON(10,10,10,10,25,10) \
   MSET_SPELL(2,3,2,2,3,2,2,2,2,3,2)
   
#define M_LEOPARD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)

#define M_LEOPARD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_LEOPARD(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n growls loudly as it claws you!", \
                                "$1n growls loudly as it claws $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* LEOPARD end */

/* TRIPHIDS */
#define BASE_TRIPHID(MLVL, GENDER, ARM)  \
   race RACE_TRIPHID \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,100,850) \
   SETHEIGHT(MLVL,65,250) \
   MSET_ABILITY(5,5,10,10,15,15,20,20) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_TRIPHID_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)

#define M_TRIPHID_MAD(MLVL,GENDER) /* level 1 to 199 */ \
   BASE_TRIPHID(MLVL,GENDER,ARM_CLOTHES)  \
   dilcopy comb_poison@function("<div class='cpr'>You feel poison burning your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s fangs deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 130

/* TRIPHIDS END */

/* HAMSTER */
#define BASE_HAMSTER(MLVL,GENDER,ARM) \
   race RACE_HAMSTER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 500 \
   SETWEIGHT(MLVL,1,5) \
   SETHEIGHT(MLVL,7,30) \
   MSET_ABILITY(5,5,10,10,15,15,20,20) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_HAMSTER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_CLOTHES)

#define M_HAMSTER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_CLOTHES)

#define M_HAMSTER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_CLOTHES)

#define M_HAMSTER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_CLOTHES)

#define M_HAMSTER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_LEATHER)

#define M_HAMSTER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_LEATHER)

#define M_HAMSTER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_HAMSTER(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeaks loudly as it bites you!", \
                                "$1n squeaks loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* HAMSTER end */



/* MOOSE */
#define BASE_MOOSE(MLVL,GENDER,ARM) \
   race RACE_MOOSE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 800 \
   SETWEIGHT(MLVL,125,1200) \
   SETHEIGHT(MLVL,96,160) \
   exp 100 \
   alignment -200 \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON( 10, 10, 10, 10, 20, 10 ) \
   MSET_SPELL( 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 )

#define M_MOOSE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOOSE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_LEATHER)

#define M_MOOSE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_LEATHER)

#define M_MOOSE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_HLEATHER)

#define M_MOOSE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_CHAIN)

#define M_MOOSE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MOOSE(MLVL,GENDER,ARM_PLATE)

#define M_MOOSE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MOOSE(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "OPPOSITE", \
                 {"$1n attacks $3n!", "$1n attacks you!"}); \
	 exp 125

/* MOOSE end */

/* SLOTH */
#define BASE_SLOTH(MLVL,GENDER,ARM) \
   race RACE_SLOTH \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,40,60) \
   SETHEIGHT(MLVL,80,130) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 500  \
   MSET_ABILITY(17,15,15,13,14,12,7,7) \
   MSET_WEAPON(6,7,8,7,7,6) \
   MSET_SPELL(7,5,6,5,5,6,5,5,5,5,5)

#define M_SLOTH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)

#define M_SLOTH_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SLOTH(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n growls loudly as it claws you!", \
                                "$1n growls loudly as it claws $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* SLOTH end */

/* LION */
#define BASE_LION(MLVL,GENDER,ARM) \
   race RACE_LION \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,120,500) \
   SETHEIGHT(MLVL,60,180) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 0  \
   MSET_ABILITY(25,20,20,25,10,0,0,0) \
   MSET_WEAPON(10,10,10,10,25,10) \
   MSET_SPELL(2,3,2,2,3,2,2,2,2,3,2)

#define M_LION_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_LION_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_LION(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n growls loudly as it claws you!", \
                                "$1n growls loudly as it claws $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* LION end */

/* SMALL_INSECT */
#define BASE_SMALL_INSECT(MLVL,GENDER,ARM) \
   race RACE_INSECT \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,1,2) \
   SETHEIGHT(MLVL,2,15) \
   MSET_ABILITY(20,50,10,19,1,0,0,0)  \
   MSET_WEAPON(20,20,20,20,10,10)

#define M_SMALL_INSECT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_LEATHER)

#define M_SMALL_INSECT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_LEATHER)

#define M_SMALL_INSECT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_LEATHER)

#define M_SMALL_INSECT_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_HLEATHER)

#define M_SMALL_INSECT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_CHAIN)

#define M_SMALL_INSECT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SMALL_INSECT(MLVL,GENDER,ARM_PLATE)


#define M_SMALL_INSECT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SMALL_INSECT(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SMALL_INSECT end */

/* DRAGONFLY */
#define BASE_DRAGONFLY(MLVL,GENDER,ARM) \
   race RACE_DRAGONFLY \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_STING,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,1,50) \
   SETHEIGHT(MLVL,10,115) \
   MSET_ABILITY(20,50,10,19,1,0,0,0)  \
   MSET_WEAPON(20,20,20,20,10,10)

#define M_DRAGONFLY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_LEATHER)

#define M_DRAGONFLY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_LEATHER)

#define M_DRAGONFLY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_LEATHER)

#define M_DRAGONFLY_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_HLEATHER)

#define M_DRAGONFLY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_CHAIN)

#define M_DRAGONFLY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DRAGONFLY(MLVL,GENDER,ARM_PLATE)

/* DRAGONFLY end */

/* ALLIGATOR   */
#define BASE_ALLIGATOR(MLVL,GENDER,ARM) \
   race RACE_ALLIGATOR \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -100 \
   SETWEIGHT(MLVL,15,700) \
   SETHEIGHT(MLVL,30,360) \
   MSET_ABILITY(30,30,20,15,5,0,0,0) \
   MSET_WEAPON(20,20,20,20,10,10)    \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0) \
   flags {UNIT_FL_NO_TELEPORT} \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 70, 0, "ANY", \
      {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 110

#define M_ALLIGATOR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)

#define M_ALLIGATOR_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_ALLIGATOR(MLVL,GENDER,ARM_CHAIN)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
exp 125

/* ALLIGATOR end */

/* CROCODILE   */
#define BASE_CROCODILE(MLVL,GENDER,ARM) \
race RACE_CROCODILE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment -100 \
SETWEIGHT(MLVL,15,200) \
SETHEIGHT(MLVL,30,240) \
MSET_ABILITY(20,20,10,10,10,10,10,10)  \
MSET_WEAPON(10,10,10,5,5,5)           \
MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE} \
dilcopy aggressive@function(0, -1, 0, 0, 3, 70, 0, "ANY", \
             {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 110

#define M_CROCODILE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)

#define M_CROCODILE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CROCODILE(MLVL,GENDER,ARM_CHAIN)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
exp 125

/* CROCODILE end */

/* PIGEON */
#define BASE_PIGEON(MLVL,GENDER,ARM) \
   race RACE_PIGEON \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,15) \
   SETHEIGHT(MLVL,15,48) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(5,5,5,5,20,5)      \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5)

#define M_PIGEON_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_CLOTHES)

#define M_PIGEON_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_LEATHER)

#define M_PIGEON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_HLEATHER)

#define M_PIGEON_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_HLEATHER)

#define M_PIGEON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_CHAIN)

#define M_PIGEON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_PIGEON(MLVL,GENDER,ARM_PLATE)

#define M_PIGEON_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_PIGEON(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to sink its beak into you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* PIGEON end */

/* PEACOCK */
#define BASE_PEACOCK(MLVL,GENDER,ARM) \
   race RACE_PEACOCK \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,5,80) \
   SETHEIGHT(MLVL,30,120) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(5,5,5,5,20,5)      \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5)

#define M_PEACOCK_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_CLOTHES)

#define M_PEACOCK_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_LEATHER)

#define M_PEACOCK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_HLEATHER)

#define M_PEACOCK_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_HLEATHER)

#define M_PEACOCK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_CHAIN)

#define M_PEACOCK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_PEACOCK(MLVL,GENDER,ARM_PLATE)

#define M_PEACOCK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
  BASE_PEACOCK(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to sink its beak into you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* PEACOCK end */

/* LEECH   */
#define BASE_LEECH(MLVL,GENDER,ARM) \
race RACE_LEECH \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,50) \
SETHEIGHT(MLVL,7,120) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,4,4,3,3,4,4,3,3,4) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE}

#define M_LEECH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_LEECH(MLVL,GENDER,ARM_LEATHER)

#define M_LEECH_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_LEECH(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* LEECH end */

/* WEREWOLF  */
#define BASE_WEREWOLF(MLVL,GENDER,ARM) \
race RACE_WEREWOLF \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 0 \
SETWEIGHT(MLVL,140,550) \
SETHEIGHT(MLVL,105,255) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,3,4,3,4,4,4,3,3,4) \
flags {UNIT_FL_NO_TELEPORT} \
romflags {CHAR_DETECT_INVISIBLE}

#define M_WEREWOLF_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WEREWOLF_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_WEREWOLF(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n bites you!", \
                           "$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* WEREWOLF end */

/* CHAMELEON */
#define BASE_CHAMELEON(MLVL,GENDER,ARM) \
race RACE_CHAMELEON \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,50) \
SETHEIGHT(MLVL,7,110) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(4,4,3,4,3,4,4,4,3,3,4)

#define M_CHAMELEON_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_LEATHER)

#define M_CHAMELEON_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_CHAMELEON(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n claws you!", \
                           "$1n claws $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* CHAMELEON end */

/* SCORPION */
#define BASE_SCORPION(MLVL,GENDER,ARM) \
race RACE_SCORPION \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_STING,ARM) \
alignment 0 \
SETWEIGHT(MLVL,1,50) \
SETHEIGHT(MLVL,7,90) \
MSET_ABILITY(25,35,10,20,10,0,0,0) \
MSET_WEAPON(10,10,10,10,10,10) \
MSET_SPELL(3,4,3,4,4,4,4,4,3,3,4) \
dilcopy comb_poison@function("<div class='cpr>You feel fire burning within your veins!</div>", \
   "&c+c$1n strikes $3n with $1s stinger!</div>");

#define M_SCORPION_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_SCORPION(MLVL,GENDER,ARM_LEATHER)

#define M_SCORPION_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_SCORPION(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* SCORPION end */

/* TIGER */
#define BASE_TIGER(MLVL,GENDER,ARM) \
   race RACE_TIGER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,200,555) \
   SETHEIGHT(MLVL,90,180) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(5,5,5,5,25,10) \
   MSET_SPELL(3,5,3,4,5,4,4,3,4,7,3) \
   romflags {CHAR_HIDE, CHAR_SNEAK}

#define M_TIGER_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_TIGER(MLVL,GENDER,ARM_CLOTHES)

#define M_TIGER_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_TIGER(MLVL,GENDER,ARM_CLOTHES)

#define M_TIGER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_TIGER(MLVL,GENDER,ARM_LEATHER)

#define M_TIGER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_TIGER(MLVL,GENDER,ARM_HLEATHER)

#define M_TIGER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_TIGER(MLVL,GENDER,ARM_HLEATHER)

#define M_TIGER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_TIGER(MLVL,GENDER,ARM_HLEATHER)

#define M_TIGER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_TIGER(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
    dilcopy aggressive@function(0, -1, 0, 1, 3, 48, 2, "ANY", \
   {"$1n growls loudly and attacks $3n!", \
   "$1n growls loudly and attacks you!"}); \
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s teeth deep into $3n!"); \
   exp 130

/* TIGER end */

/* MANTIS */
#define BASE_MANTIS(MLVL,GENDER,ARM) \
   race RACE_MANTIS \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,300) \
   SETHEIGHT(MLVL,3,300) \
   MSET_ABILITY(25,30,15,30,0,0,0,0) \
   MSET_WEAPON(5,5,5,5,25,10) \
   MSET_SPELL(3,5,3,4,5,4,4,3,4,7,3) \
   romflags {CHAR_HIDE, CHAR_SNEAK} \
   dilcopy aggressive@function(0, -1, 0, 1, 3, 48, 2, "ANY", \
   {"$1n screeches loudly and attacks $3n!", \
   "$1n screeches loudly and attacks you!"}); \
   exp 120

#define M_MANTIS_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_CLOTHES)

#define M_MANTIS_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_CLOTHES)

#define M_MANTIS_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_LEATHER)

#define M_MANTIS_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_HLEATHER)

#define M_MANTIS_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_HLEATHER)

#define M_MANTIS_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MANTIS(MLVL,GENDER,ARM_HLEATHER)

#define M_MANTIS_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MANTIS(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   exp 130

/* MANTIS end */

/* MONKEY */
#define BASE_MONKEY(MLVL,GENDER,ARM) \
   race RACE_MONKEY \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,15,250) \
   SETHEIGHT(MLVL,30,150) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_MONKEY_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_CLOTHES)

#define M_MONKEY_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_CLOTHES)

#define M_MONKEY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_LEATHER)

#define M_MONKEY_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_HLEATHER)

#define M_MONKEY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_HLEATHER)

#define M_MONKEY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MONKEY(MLVL,GENDER,ARM_HLEATHER)

#define M_MONKEY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MONKEY(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s teeth deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* MONKEY end */

/* LLAMA */
#define BASE_LLAMA(MLVL,GENDER,ARM) \
   race RACE_LLAMA \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,20,800) \
   SETHEIGHT(MLVL,98,300) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */

#define M_LLAMA_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_LEATHER)

#define M_LLAMA_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_LEATHER)

#define M_LLAMA_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_LEATHER)

#define M_LLAMA_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_HLEATHER)

#define M_LLAMA_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_CHAIN)

#define M_LLAMA_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_LLAMA(MLVL,GENDER,ARM_PLATE)
/* LLAMA end */

/* BAT */
#define BASE_BAT(MLVL,GENDER,ARM) \
   race RACE_BAT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,100) \
   SETHEIGHT(MLVL,15,180) /* 6' wingspan */  \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -300  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_BAT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)

#define M_BAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BAT(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n hisses loudly as it bites you!", \
                                "$1n hisses loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* BAT end */

/* CHIPMUNK */
#define BASE_CHIPMUNK(MLVL,GENDER,ARM) \
   race RACE_CHIPMUNK \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,30) \
   SETHEIGHT(MLVL,8,150) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 750  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_CHIPMUNK_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)

#define M_CHIPMUNK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_CHIPMUNK(MLVL,GENDER,ARM_HLEATHER)  \
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

/* CHIPMUNK end */

/* GOOSE*/
#define BASE_GOOSE(MLVL,GENDER,ARM) \
   race RACE_GOOSE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,8,80) \
   SETHEIGHT(MLVL,45,150) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_GOOSE_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_CLOTHES)

#define M_GOOSE_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_LEATHER)

#define M_GOOSE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_HLEATHER)

#define M_GOOSE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_HLEATHER)


#define M_GOOSE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_CHAIN)

#define M_GOOSE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_GOOSE(MLVL,GENDER,ARM_PLATE)

#define M_GOOSE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_GOOSE(MLVL,GENDER,ARM_HLEATHER)  \
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

/* GOOSE end */

/* MOLE */
#define BASE_MOLE(MLVL,GENDER,ARM) \
   race RACE_MOLE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,10) \
   SETHEIGHT(MLVL,1,90) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -750 /* Neutral evil */ \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_MOLE_TINY(MLVL,GENDER) /*level 1 to 25 */\
   BASE_MOLE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MOLE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MOLE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOLE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MOLE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */\
   BASE_MOLE(MLVL,GENDER,ARM_LEATHER)

#define M_MOLE_GODLY(MLVL,GENDER) /* level 121 to 199 */\
   BASE_MOLE(MLVL,GENDER,ARM_LEATHER)

#define M_MOLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */\
   BASE_MOLE(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeaks loudly as it bites you!", \
                                "$1n squeaks loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* MOLE end */

/* BEAVER */
#define BASE_BEAVER(MLVL,GENDER,ARM) \
   race RACE_BEAVER \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,85) \
   SETHEIGHT(MLVL,15,150) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -750 /* Neutral evil */ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_BEAVER_TINY(MLVL,GENDER) /*level 1 to 25 */\
   BASE_BEAVER(MLVL,GENDER,ARM_CLOTHES)

#define M_BEAVER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BEAVER(MLVL,GENDER,ARM_CLOTHES)

#define M_BEAVER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BEAVER(MLVL,GENDER,ARM_CLOTHES)

#define M_BEAVER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BEAVER(MLVL,GENDER,ARM_CLOTHES)

#define M_BEAVER_XLARGE(MLVL,GENDER) /* level 101 to 120 */\
   BASE_BEAVER(MLVL,GENDER,ARM_LEATHER)

#define M_BEAVER_GODLY(MLVL,GENDER) /* level 121 to 199 */\
   BASE_BEAVER(MLVL,GENDER,ARM_LEATHER)

#define M_BEAVER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */\
   BASE_BEAVER(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeaks loudly as it bites you!", \
                                "$1n squeaks loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* BEAVER end */

/* TURTLE */
#define BASE_TURTLE(MLVL,GENDER,ARM) \
race RACE_TURTLE \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment -550 \
SETWEIGHT(MLVL,2,500) \
SETHEIGHT(MLVL,20,180) \
MSET_ABILITY(30,10,20,20,20,0,0,0) \
MSET_WEAPON(6,6,6,6,26,6) \
MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)

#define M_TURTLE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_TURTLE(MLVL,GENDER,ARM_CLOTHES)

#define M_TURTLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_TURTLE(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* TURTLE end */

/* COW */
#define BASE_COW(MLVL,GENDER,ARM) \
   race RACE_COW \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 400 \
   SETWEIGHT(MLVL,85,1500) \
   SETHEIGHT(MLVL,76,450) \
   exp 100 \
   alignment -200 \
   MSET_ABILITY(20,20,20,20,5,15,0,0)  \
   MSET_WEAPON(6,6,6,6,15,6)\
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_COW_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_COW(MLVL,GENDER,ARM_LEATHER)

#define M_COW_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_COW(MLVL,GENDER,ARM_LEATHER)

#define M_COW_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_COW(MLVL,GENDER,ARM_LEATHER)

#define M_COW_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_COW(MLVL,GENDER,ARM_HLEATHER)

#define M_COW_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_COW(MLVL,GENDER,ARM_CHAIN)

#define M_COW_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_COW(MLVL,GENDER,ARM_PLATE)

#define M_COW_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_COW(MLVL,GENDER,ARM_HLEATHER)  \
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
	 exp 125

/* COW end */

/* DEER */
#define BASE_DEER(MLVL,GENDER,ARM) \
   race RACE_DEER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 349 \
   SETWEIGHT(MLVL,131,1110) \
   SETHEIGHT(MLVL,66,450) \
   exp 100 \
   alignment -200 \
   MSET_ABILITY(18,25,15,20,22,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_DEER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DEER(MLVL,GENDER,ARM_CLOTHES)

#define M_DEER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DEER(MLVL,GENDER,ARM_LEATHER)

#define M_DEER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DEER(MLVL,GENDER,ARM_LEATHER)

#define M_DEER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DEER(MLVL,GENDER,ARM_HLEATHER)

#define M_DEER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DEER(MLVL,GENDER,ARM_CHAIN)

#define M_DEER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DEER(MLVL,GENDER,ARM_PLATE)

#define M_DEER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_DEER(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to kick you tearing your flesh!", \
                                "$1n kicks $3n tearing $3s flesh!"); \
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "OPPOSITE", \
                 {"$1n attacks the evil $3n!", "$1n attacks you!"}); \
	 exp 125

/* DEER end */


/* PIG */
#define BASE_PIG(MLVL,GENDER,ARM) \
   race RACE_PIG \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,81,400) \
   SETHEIGHT(MLVL,66,180) \
   MSET_ABILITY(25,17,18,20,10,5,5,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_PIG_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_PIG(MLVL,GENDER,ARM_CLOTHES)

#define M_PIG_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_PIG(MLVL,GENDER,ARM_LEATHER)

#define M_PIG_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_PIG(MLVL,GENDER,ARM_LEATHER)

#define M_PIG_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_PIG(MLVL,GENDER,ARM_HLEATHER)

#define M_PIG_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_PIG(MLVL,GENDER,ARM_CHAIN)

#define M_PIG_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_PIG(MLVL,GENDER,ARM_PLATE)

#define M_PIG_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_PIG(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeals at it bites you!", \
                                "$1n squeals loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "ANY", \
                 {"$1n attacks $3n!", "$1n attacks you!"}); \
	 exp 125

/* PIG end */


/* MOUSE */
#define BASE_MOUSE(MLVL,GENDER,ARM) \
   race RACE_MOUSE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 500 \
   SETWEIGHT(MLVL,1,5) \
   SETHEIGHT(MLVL,7,30) \
   MSET_ABILITY(5,5,10,10,15,15,20,20) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_MOUSE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOUSE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOUSE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOUSE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_CLOTHES)

#define M_MOUSE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_LEATHER)

#define M_MOUSE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MOUSE(MLVL,GENDER,ARM_LEATHER)

#define M_MOUSE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MOUSE(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeaks loudly as it bites you!", \
                                "$1n squeaks loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* MOUSE end */

/* SNAKE */

#define BASE_SNAKE(MLVL,GENDER,ARM) \
   race RACE_SNAKE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,3,110) \
   SETHEIGHT(MLVL,15,270) \
   NATURAL_DEF(WPN_BITE, ARM)\
   alignment -100 \
   MSET_ABILITY(6,14,10,10,10,20,15,15)\
   MSET_WEAPON(5,5,5,5,20,5)\
   MSET_SPELL(18,2,2,2,2,2,19,2,2,2,2) \
   romflags {CHAR_DETECT_INVISIBLE}


#define BASE_SNAKE_POISON(MLVL,GENDER,ARM) \
   BASE_SNAKE(MLVL,GENDER,ARM) \
   exp 125 \
   alignment -500 \
   MSET_ABILITY(15,15,10,20,20,10,5,5) \
   MSET_WEAPON(10,10,10,10,15,10) \
   MSET_SPELL(1,0,0,0,1,5,5,5,5,8,5) \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy comb_poison@function("<div class='cpr'>$1n drives $1s fangs deep into $3n!</div>", \
   "<div class='cpr'>$1n grimly bites $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, -49, 0, PULSE_SEC*11, -1, "ANY", \
   {"$1n shows a sharp set of fangs just before shooting $1s head at $3n!", \
	"$1n shows a sharp set of fangs and then shoots $1s head at YOU!"});

/* non-poison snakes */
#define M_SNAKE_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SNAKE(MLVL,GENDER,ARM_LEATHER)

/* poison mean snakes */
#define M_SNAKE_POISON_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_POISON_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_POISON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_POISON_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_POISON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

#define M_SNAKE_POISON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SNAKE_POISON(MLVL,GENDER,ARM_LEATHER)

/* SNAKE end */

/* bird */

#define BASE_BIRD(MLVL,GENDER,ARM) \
   race RACE_BIRD \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,50) \
   SETHEIGHT(MLVL,2,90) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_BIRD_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_BIRD(MLVL,GENDER,ARM_CLOTHES)

#define M_BIRD_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_BIRD(MLVL,GENDER,ARM_CLOTHES)

#define M_BIRD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BIRD(MLVL,GENDER,ARM_LEATHER)

#define M_BIRD_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BIRD(MLVL,GENDER,ARM_HLEATHER)

#define M_BIRD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BIRD(MLVL,GENDER,ARM_CHAIN)

#define M_BIRD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BIRD(MLVL,GENDER,ARM_PLATE)

#define M_BIRD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BIRD(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* bird end */

/* SEAGULL */

#define BASE_SEAGULL(MLVL,GENDER,ARM) \
   race RACE_SEAGULL \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,25) \
   SETHEIGHT(MLVL,30,90) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_SEAGULL_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_CLOTHES)

#define M_SEAGULL_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_CLOTHES)

#define M_SEAGULL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_LEATHER)

#define M_SEAGULL_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_HLEATHER)

#define M_SEAGULL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_CHAIN)

#define M_SEAGULL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_PLATE)

#define M_SEAGULL_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SEAGULL(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to bite you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* SEAGULL end */

/* VULTURE */
#define BASE_VULTURE(MLVL,GENDER,ARM) \
   race RACE_VULTURE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,1,60) \
   SETHEIGHT(MLVL,45,120) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_VULTURE_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_VULTURE(MLVL,GENDER,ARM_CLOTHES)

#define M_VULTURE_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_VULTURE(MLVL,GENDER,ARM_HLEATHER)

#define M_VULTURE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BIRD(MLVL,GENDER,ARM_CHAIN)

#define M_VULTURE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BIRD(MLVL,GENDER,ARM_CHAIN)

#define M_VULTURE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BIRD(MLVL,GENDER,ARM_PLATE)

#define M_VULTURE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BIRD(MLVL,GENDER,ARM_PLATE)

#define M_VULTURE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BIRD(MLVL,GENDER,ARM_CHAIN)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n manages to tear some flesh from you!", \
                                "$1n tears some flesh from $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* VULTURE end */

/* DUCK*/
#define BASE_DUCK(MLVL,GENDER,ARM) \
   race RACE_DUCK \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,1,60) \
   SETHEIGHT(MLVL,15,120) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_DUCK_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_DUCK(MLVL,GENDER,ARM_CLOTHES)

#define M_DUCK_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_DUCK(MLVL,GENDER,ARM_LEATHER)

#define M_DUCK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DUCK(MLVL,GENDER,ARM_HLEATHER)

#define M_DUCK_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DUCK(MLVL,GENDER,ARM_HLEATHER)

#define M_DUCK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DUCK(MLVL,GENDER,ARM_CHAIN)

#define M_DUCK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DUCK(MLVL,GENDER,ARM_PLATE)

#define M_DUCK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_DUCK(MLVL,GENDER,ARM_HLEATHER)  \
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

/* DUCK end */

/* EAGLE */
#define BASE_EAGLE(MLVL,GENDER,ARM) \
   race RACE_EAGLE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,100) \
   SETHEIGHT(MLVL,45,120) \
   MSET_ABILITY(35,30,20,10,5,0,0,0) \
   MSET_WEAPON(5,5,5,5,50,5)      \
   MSET_SPELL(2,2,2,10,2,2,2,2,1,0,0)

#define M_EAGLE_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_CLOTHES)

#define M_EAGLE_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_HLEATHER)

#define M_EAGLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_HLEATHER)

#define M_EAGLE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_CHAIN)

#define M_EAGLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_CHAIN)

#define M_EAGLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_EAGLE(MLVL,GENDER,ARM_PLATE)

#define M_EAGLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_EAGLE(MLVL,GENDER,ARM_CHAIN)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to sink its claws into you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* EAGLE end */

/* TURKEY */
#define BASE_TURKEY(MLVL,GENDER,ARM) \
   race RACE_TURKEY \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,10,34) \
   SETHEIGHT(MLVL,30,120) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(5,5,5,5,20,5)      \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5)

#define M_TURKEY_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_CLOTHES)

#define M_TURKEY_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_LEATHER)

#define M_TURKEY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_HLEATHER)

#define M_TURKEY_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_HLEATHER)

#define M_TURKEY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_CHAIN)

#define M_TURKEY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_TURKEY(MLVL,GENDER,ARM_PLATE)

#define M_TURKEY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_TURKEY(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to sink its beak into you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* TURKEY end */

/* swan */
#define BASE_SWAN(MLVL,GENDER,ARM) \
   race RACE_SWAN \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CRUSH,ARM) \
   alignment -250 \
   SETWEIGHT(MLVL,15,105) \
   SETHEIGHT(MLVL,45,360) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_SWAN_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_SWAN(MLVL,GENDER,ARM_CLOTHES)

#define M_SWAN_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_SWAN(MLVL,GENDER,ARM_LEATHER)

#define M_SWAN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SWAN(MLVL,GENDER,ARM_HLEATHER)

#define M_SWAN_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SWAN(MLVL,GENDER,ARM_HLEATHER)

#define M_SWAN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SWAN(MLVL,GENDER,ARM_CHAIN)

#define M_SWAN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SWAN(MLVL,GENDER,ARM_PLATE)

#define M_SWAN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SWAN(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(10,15,10,10,15,10,15,15) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n manages to crush you with its wings!", \
                                "$1n manages to crush $3n with $1s wings!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* SWAN end */

/* WOLF */
#define BASE_WOLF(MLVL,GENDER,ARM) \
   race RACE_WOLF \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 400 \
   SETWEIGHT(MLVL,100,700) \
   SETHEIGHT(MLVL,90,290) \
   exp 110 \
   alignment -200 \
   MSET_ABILITY(15,15,15,15,10,30,0,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           \
   MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)   \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"});

#define M_WOLF_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_WOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WOLF_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_WOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WOLF_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_WOLF(MLVL,GENDER,ARM_LEATHER)

#define M_WOLF_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_WOLF(MLVL,GENDER,ARM_HLEATHER)

#define M_WOLF_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_WOLF(MLVL,GENDER,ARM_CHAIN)

#define M_WOLF_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_WOLF(MLVL,GENDER,ARM_PLATE)

#define M_WOLF_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_WOLF(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its claws into you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   exp 125

/* WOLF end */

/* BEAR */
#define BASE_BEAR(MLVL,GENDER,ARM) \
   race RACE_BEAR \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   SETWEIGHT(MLVL,50,1200) \
   SETHEIGHT(MLVL,30,720) \
   exp 110 \
   alignment -200 \
   MSET_ABILITY(20,15,20,20,5,20,0,0) /* Is that a Bear? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */ \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 115

#define M_BEAR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BEAR(MLVL,GENDER,ARM_LEATHER)

#define M_BEAR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BEAR(MLVL,GENDER,ARM_LEATHER)

#define M_BEAR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BEAR(MLVL,GENDER,ARM_LEATHER)

#define M_BEAR_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BEAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BEAR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BEAR(MLVL,GENDER,ARM_CHAIN)

#define M_BEAR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BEAR(MLVL,GENDER,ARM_PLATE)

#define M_BEAR_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BEAR(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its claws into you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   exp 125

/* BEAR end */




/* dog */
#define BASE_DOG(MLVL,GENDER,ARM) \
   race RACE_DOG \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,10,285) \
   SETHEIGHT(MLVL,15,360) \
   MSET_ABILITY(15,20,15,20,10,10,10,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define M_DOG_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_DOG(MLVL,GENDER,ARM_LEATHER)

#define M_DOG_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_DOG(MLVL,GENDER,ARM_LEATHER)

#define M_DOG_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_DOG(MLVL,GENDER,ARM_LEATHER)

#define M_DOG_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_DOG(MLVL,GENDER,ARM_HLEATHER)

#define M_DOG_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_DOG(MLVL,GENDER,ARM_CHAIN)

#define M_DOG_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_DOG(MLVL,GENDER,ARM_PLATE)

#define M_DOG_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_DOG(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its fangs into you!", \
                                "$1n sinks $1s fangs deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* dog end */

/* fox */
#define BASE_FOX(MLVL,GENDER,ARM) \
   race RACE_FOX \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,10,225) \
   SETHEIGHT(MLVL,15,201) \
   MSET_ABILITY(30, 40, 10, 10, 2, 8, 0, 0) \
   MSET_WEAPON( 6, 6, 7, 5, 60, 6 ) \
   MSET_SPELL( 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 0 )

#define M_FOX_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_FOX(MLVL,GENDER,ARM_CLOTHES)

#define M_FOX_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_FOX(MLVL,GENDER,ARM_LEATHER)

#define M_FOX_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_FOX(MLVL,GENDER,ARM_LEATHER)

#define M_FOX_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_FOX(MLVL,GENDER,ARM_HLEATHER)

#define M_FOX_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_FOX(MLVL,GENDER,ARM_CHAIN)

#define M_FOX_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_FOX(MLVL,GENDER,ARM_PLATE)

#define M_FOX_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_FOX(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its fangs into you!", \
                                "$1n sinks $1s fangs deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* FOX end */

/* CAT */
#define BASE_CAT(MLVL,GENDER,ARM) \
   race RACE_CAT \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CLAW,ARM) \
   alignment 400 \
   SETWEIGHT(MLVL,4,90) \
   SETHEIGHT(MLVL,15,185) \
   MSET_ABILITY(20,50,10,10,2,8,0,0)  \
   MSET_WEAPON(6,6,7,5,60,6)          /* 90%, 60% Natural */ \
   MSET_SPELL(0,0,0,0,0,0,4,3,3,0,0)  /* 10%, resistances */

#define M_CAT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_CAT(MLVL,GENDER,ARM_CLOTHES)

#define M_CAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_CAT(MLVL,GENDER,ARM_LEATHER)

#define M_CAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CAT(MLVL,GENDER,ARM_LEATHER)

#define M_CAT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_CAT(MLVL,GENDER,ARM_HLEATHER)

#define M_CAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CAT(MLVL,GENDER,ARM_CHAIN)

#define M_CAT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CAT(MLVL,GENDER,ARM_PLATE)

#define M_CAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_CAT(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its claws into you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* CAT end */

/* ELEPHANT */
#define BASE_ELEPHANT(MLVL,GENDER,ARM) \
   race RACE_ELEPHANT \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CRUSH,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,175,14000) \
   SETHEIGHT(MLVL,75,390) \
   MSET_ABILITY(25,25,25,25,0,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */

#define M_ELEPHANT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_LEATHER)

#define M_ELEPHANT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_LEATHER)

#define M_ELEPHANT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_LEATHER)

#define M_ELEPHANT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_HLEATHER)

#define M_ELEPHANT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_CHAIN)

#define M_ELEPHANT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELEPHANT(MLVL,GENDER,ARM_PLATE)
/* ELEPHANT end */

/* horse */
#define BASE_HORSE(MLVL,GENDER,ARM) \
   race RACE_HORSE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,150,2000) \
   SETHEIGHT(MLVL,75,360) \
   MSET_ABILITY(22,21,17,25,15,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */

#define M_HORSE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_HORSE(MLVL,GENDER,ARM_LEATHER)

#define M_HORSE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_HORSE(MLVL,GENDER,ARM_LEATHER)

#define M_HORSE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_HORSE(MLVL,GENDER,ARM_LEATHER)

#define M_HORSE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_HORSE(MLVL,GENDER,ARM_HLEATHER)

#define M_HORSE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_HORSE(MLVL,GENDER,ARM_CHAIN)

#define M_HORSE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_HORSE(MLVL,GENDER,ARM_PLATE)
/* horse end */

/* RHINO */
#define BASE_RHINO(MLVL,GENDER,ARM) \
   race RACE_RHINO \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_CRUSH,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,350,3000) \
   SETHEIGHT(MLVL,75,360) \
   MSET_ABILITY(22,21,17,25,15,0,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */

#define M_RHINO_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_RHINO(MLVL,GENDER,ARM_LEATHER)

#define M_RHINO_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_RHINO(MLVL,GENDER,ARM_LEATHER)

#define M_RHINO_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_RHINO(MLVL,GENDER,ARM_LEATHER)

#define M_RHINO_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_RHINO(MLVL,GENDER,ARM_HLEATHER)

#define M_RHINO_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_RHINO(MLVL,GENDER,ARM_CHAIN)

#define M_RHINO_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_RHINO(MLVL,GENDER,ARM_PLATE)
/* RHINO end */

/* RABBIT */
#define BASE_RABBIT(MLVL,GENDER,ARM) \
   race RACE_RABBIT \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 200      /* Neutral->Good */ \
   MSET_ABILITY(20,20,20,20,20,0,0,0)    /* Rabbits are CUTE! */ \
   MSET_WEAPON(6,6,6,6,6,6)           /* Not a fighter! :) */ \
   MSET_SPELL(4,6,6,6,6,6,6,6,6,6,6)  /* Resistances       */ \
   SETWEIGHT(MLVL,8,45) \
   SETHEIGHT(MLVL,15,120)

#define M_RABBIT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_LEATHER)

#define M_RABBIT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_LEATHER)

#define M_RABBIT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_LEATHER)

#define M_RABBIT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_HLEATHER)

#define M_RABBIT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_CHAIN)

#define M_RABBIT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_RABBIT(MLVL,GENDER,ARM_PLATE)

#define M_RABBIT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_RABBIT(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n manages to sink its claws into you!", \
                                "$1n sinks $1s claws deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* RABBIT end */

/* BUTTERFLY */
#define BASE_BUTTERFLY(MLVL,GENDER,ARM) \
   race RACE_BUTTERFLY \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,12) \
   SETHEIGHT(MLVL,2,180) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 200 \
   MSET_ABILITY(5,5,10,10,15,15,15,25)      \
   MSET_WEAPON(8,8,8,8,8,0)          \
   MSET_SPELL(4,4,4,20,4,4,4,4,4,4,4)

#define M_BUTTERFLY_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_CLOTHES)

#define M_BUTTERFLY_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_CLOTHES)

#define M_BUTTERFLY_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_CLOTHES)

#define M_BUTTERFLY_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_LEATHER)

#define M_BUTTERFLY_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_LEATHER)

#define M_BUTTERFLY_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_LEATHER)

#define M_BUTTERFLY_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BUTTERFLY(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* BUTTERFLY end */

/* BOAR */
#define BASE_BOAR(MLVL,GENDER,ARM) \
   race RACE_BOAR \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,4,600) \
   SETHEIGHT(MLVL,15,240) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -500      /*       */ \
   MSET_ABILITY(20,20,20,20,10,10,0,0) /* Is that a BOAR? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */ \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*60, 0, "ANY", \
                   {"$1n beserks and attacks $3n without notice!", \
				   "$1n beserks and attacks you!"});

#define M_BOAR_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BOAR(MLVL,GENDER,ARM_LEATHER)

#define M_BOAR_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BOAR_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BOAR_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BOAR_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BOAR_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)

#define M_BOAR_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BOAR(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n grunts loudly as it bites you!", \
                                "$1n grunts loudly as it bites $3n!"); \
   dilcopy aggressive@function (0, -1, 0, 10, 3, PULSE_SEC*40, 0, "OPPOSITE", \
                                {"$1n grunts and charges $3n!", \
								 "$1n tries to drive you away!"}); \
   exp 125

/* BOAR end */

/* SKUNK */
#define BASE_SKUNK(MLVL,GENDER,ARM) \
   race RACE_SKUNK \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,60) \
   SETHEIGHT(MLVL,15,120) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(15,15,15,15,10,30,0,0) /* Is that a skunk? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */ \
   dilcopy smelldil@hobgob();


#define M_SKUNK_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SKUNK(MLVL,GENDER,ARM_LEATHER)

#define M_SKUNK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SKUNK(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* SKUNK end */

/* BADGER */
#define BASE_BADGER(MLVL,GENDER,ARM) \
   race RACE_BADGER \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,160) \
   SETHEIGHT(MLVL,15,150) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -750 /* Neutral evil */ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_BADGER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)

#define M_BADGER_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_BADGER(MLVL,GENDER,ARM_HLEATHER)  \
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

/* BADGER end */

/* RAT */
#define BASE_RAT(MLVL,GENDER,ARM) \
   race RACE_RAT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,30) \
   SETHEIGHT(MLVL,21,90) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -750 /* Neutral evil */ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)  \
   dilcopy combat_mag@function("plague", "", 0, 2);

#define M_RAT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_RAT(MLVL,GENDER,ARM_CLOTHES)

#define M_RAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_RAT(MLVL,GENDER,ARM_LEATHER)

#define M_RAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_RAT(MLVL,GENDER,ARM_LEATHER)

#define M_RAT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_RAT(MLVL,GENDER,ARM_LEATHER)

#define M_RAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_RAT(MLVL,GENDER,ARM_LEATHER)

#define M_RAT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_RAT(MLVL,GENDER,ARM_LEATHER)

#define M_RAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_RAT(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n squeaks loudly as it bites you!", \
                                "$1n squeaks loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* RAT end */

/* SPIDERS */
#define BASE_SPIDER(MLVL, GENDER, ARM)  \
   race RACE_SPIDER \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,1,60) \
   SETHEIGHT(MLVL,7,240) \
   MSET_ABILITY(5,5,10,10,15,15,20,20) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)

#define BASE_SPIDER_WEB(MLVL, GENDER, ARM)  \
   BASE_SPIDER(MLVL,GENDER,ARM) \
   race RACE_SPIDER \
   NATURAL_DEF(WPN_STING,ARM) \
   alignment -750 \
   flags {UNIT_FL_INVISIBLE} \
   romflags {CHAR_DETECT_INVISIBLE} \
   dilcopy web@function(); \
   dilcopy poi_resist@function(); \
   dilcopy comb_poison@function("<div class='cpr'>You feel fire burning within your veins!</div>", \
   "&c+c$1n sinks $1s fangs deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*60, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 160

/* -------------------------------------------------------------------- */
/*                   SPIDERS                                            */
/* -------------------------------------------------------------------- */
#define M_SPIDER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_LARGE(MLVL,GENDER)     /* level 76 to 100 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)

#define M_SPIDER_MAD(MLVL,GENDER) /* level 1 to 199 */ \
   BASE_SPIDER(MLVL,GENDER,ARM_CLOTHES)  \
   dilcopy comb_poison@function("<div class='cpr'>You feel poison burning your veins!</div>", \
   "<div class='cpc'>$1n sinks $1s fangs deep into $3n!</div>"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
         {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 130

/* -------------------------------------------------------------------- */
/*            WEB MAKING MEAN SPIDERS                                   */
/* -------------------------------------------------------------------- */

#define M_WEB_SPIDER_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

#define M_WEB_SPIDER_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

#define M_WEB_SPIDER_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

#define M_WEB_SPIDER_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

#define M_WEB_SPIDER_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

#define M_WEB_SPIDER_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SPIDER_WEB(MLVL,GENDER,ARM_CLOTHES)

/* SPIDERS END */

/* COCKROACH */
#define BASE_COCKROACH(MLVL,GENDER,ARM) \
   race RACE_COCKROACH \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,10) \
   SETHEIGHT(MLVL,15,45) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(15,15,15,15,10,30,0,0) /* Is that a COCKROACH? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */

#define M_COCKROACH_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_CLOTHES)

#define M_COCKROACH_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_CLOTHES)

#define M_COCKROACH_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_CLOTHES)

#define M_COCKROACH_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_CLOTHES)

#define M_COCKROACH_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_LEATHER)

#define M_COCKROACH_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_LEATHER)

#define M_COCKROACH_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_COCKROACH(MLVL,GENDER,ARM_CLOTHES)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)\
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* COCKROACH end */

/* WORM */
#define BASE_WORM(MLVL,GENDER,ARM) \
race RACE_WORM \
level MLVL \
sex GENDER \
SETWEIGHT(MLVL,1,100) \
SETHEIGHT(MLVL,15,720) \
NATURAL_DEF(WPN_BITE, ARM) \
alignment 0 \
MSET_ABILITY(20,18,10,10,15,15,12,0) \
MSET_WEAPON(8,8,8,8,10,8) \
MSET_SPELL(4,4,4,10,4,4,4,4,4,4,4)

#define M_WORM_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_WORM(MLVL,GENDER,ARM_CLOTHES)

#define M_WORM_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_WORM(MLVL,GENDER,ARM_CLOTHES)

#define M_WORM_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_WORM(MLVL,GENDER,ARM_CLOTHES)

#define M_WORM_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
BASE_WORM(MLVL,GENDER,ARM_LEATHER)

#define M_WORM_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_WORM(MLVL,GENDER,ARM_LEATHER)

#define M_WORM_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_WORM(MLVL,GENDER,ARM_LEATHER)

#define M_WORM_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_WORM(MLVL,GENDER,ARM_HLEATHER)  \
alignment -150  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n bites you!", \
"$1n bites $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
{"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* WORM end */

/* FERRET */
#define BASE_FERRET(MLVL,GENDER,ARM) \
   race RACE_FERRET \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 500 \
   SETWEIGHT(MLVL,1,25) \
   SETHEIGHT(MLVL,20,60) \
   MSET_ABILITY(10,10,10,10,20,20,10,10) \
   MSET_WEAPON(6,6,6,6,6,6) \
   MSET_SPELL(4,6,6,6,6,6,6,6,6,6,6)

#define M_FERRET_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_FERRET(MLVL,GENDER,ARM_CLOTHES)

#define M_FERRET_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_FERRET(MLVL,GENDER,ARM_CLOTHES)

#define M_FERRET_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_FERRET(MLVL,GENDER,ARM_CLOTHES)

#define M_FERRET_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_FERRET(MLVL,GENDER,ARM_CLOTHES)

#define M_FERRET_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_FERRET(MLVL,GENDER,ARM_LEATHER)

#define M_FERRET_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_FERRET(MLVL,GENDER,ARM_LEATHER)

#define M_FERRET_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_FERRET(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n chatters loudly as it bites you!", \
                                "$1n chatters loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* FERRET end */

/* WERERAT */
#define BASE_WERERAT(MLVL,GENDER,ARM) \
   race RACE_WERERAT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,6,850) \
   SETHEIGHT(MLVL,27,850) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 0 \
   MSET_ABILITY(15,20,15,15,20,10,0,5) \
   MSET_WEAPON(5,5,5,5,35,5)  \
   MSET_SPELL(5,0,0,0,0,5,6,6,6,6,6)

#define M_WERERAT_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_CLOTHES)

#define M_WERERAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_CLOTHES)

#define M_WERERAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_CLOTHES)

#define M_WERERAT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_CLOTHES)

#define M_WERERAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_LEATHER)

#define M_WERERAT_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_WERERAT(MLVL,GENDER,ARM_LEATHER) \

#define M_WERERAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_WERERAT(MLVL,GENDER,ARM_HLEATHER) \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 30, 0, "ANY", \
                  {"$1n attacks $3n!", "$1n attacks you!"}); \
   dilcopy comb_poison@function("$1n bites you!", \
                                "$1n bites $3n!"); \
   exp 125

/* WERERAT end */

/* FROG */
#define BASE_FROG(MLVL,GENDER,ARM) \
race RACE_FROG \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_KICK,ARM) \
alignment -550 \
SETWEIGHT(MLVL,1,125) \
SETHEIGHT(MLVL,5,150) \
MSET_ABILITY(10,20,10,20,15,15,10,0) \
MSET_WEAPON(6,6,6,6,26,6) \
MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)

#define M_FROG_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_FROG(MLVL,GENDER,ARM_CLOTHES)

#define M_FROG_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_FROG(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* FROG end */

/* TOAD */
#define BASE_TOAD(MLVL,GENDER,ARM) \
race RACE_TOAD \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment -550 \
SETWEIGHT(MLVL,1,200) \
SETHEIGHT(MLVL,7,60) \
MSET_ABILITY(5,35,15,10,20,15,0,0) \
MSET_WEAPON(6,6,6,6,26,6) \
MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)

#define M_TOAD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_TOAD(MLVL,GENDER,ARM_CLOTHES)

#define M_TOAD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_TOAD(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* TOAD end */

/* CAMEL */
#define BASE_CAMEL(MLVL,GENDER,ARM) \
   race RACE_CAMEL \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 0 \
   SETWEIGHT(MLVL,150,1800) \
   SETHEIGHT(MLVL,75,360) \
   MSET_ABILITY(19,15,22,24,12,8,0,0) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,6,8,6,6,8,6)

#define M_CAMEL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_LEATHER)

#define M_CAMEL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_LEATHER)

#define M_CAMEL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_LEATHER)

#define M_CAMEL_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_HLEATHER)

#define M_CAMEL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_CHAIN)

#define M_CAMEL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CAMEL(MLVL,GENDER,ARM_PLATE)
/* CAMEL end */

/* OWL */
#define BASE_OWL(MLVL,GENDER,ARM) \
race RACE_OWL \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_CLAW,ARM) \
alignment 800 \
SETWEIGHT(MLVL,3,500) \
SETHEIGHT(MLVL,30,360) \
MSET_ABILITY(5,35,15,20,25,0,0,0) \
MSET_WEAPON(6,6,6,6,26,6) \
MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)

#define M_OWL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_OWL(MLVL,GENDER,ARM_CLOTHES)

#define M_OWL_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_OWL(MLVL,GENDER,ARM_HLEATHER)  \
MSET_ABILITY(15,15,10,15,15,10,10,10) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* OWL end */

/* LIZARD  note once these go over level 100 ish they are considered serpents*/
#define BASE_LIZARD(MLVL,GENDER,ARM) \
race RACE_LIZARD \
level MLVL \
sex GENDER \
NATURAL_DEF(WPN_BITE,ARM) \
alignment 550 \
IS_AMPHIB \
SWIM_ON  \
SETWEIGHT(MLVL,3,500) \
SETHEIGHT(MLVL,30,360) \
MSET_ABILITY(10,20,10,20,10,10,10,10) \
MSET_WEAPON(6,6,6,6,26,6) \
MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)

#define M_LIZARD_TINY(MLVL,GENDER) /*level 1 to 25 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_LARGE(MLVL,GENDER)   /* level 76 to 100 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)

#define M_LIZARD_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
BASE_LIZARD(MLVL,GENDER,ARM_CHAIN)  \
SETWEIGHT(MLVL,1,20000) \
SETHEIGHT(MLVL,30,1400) \
MSET_ABILITY(12,12,11,10,10,15,15,15) \
MSET_WEAPON(6,6,6,6,10,6)          \
MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
dilcopy comb_poison@function("$1n manages to rip your flesh!", \
                           "$1n digs $1s claws deep into $3n!"); \
dilcopy aggressive@function(0, -1, 0, 0, 3, 100, -1, "EVIL", \
                       {"$1n attacks $3n!", "$1n attacks you!"}); \
exp 125

/* LIZARD end */

/* WEASLE */
#define BASE_WEASLE(MLVL,GENDER,ARM) \
   race RACE_WEASLE \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment -350 \
   SETWEIGHT(MLVL,1,600) \
   SETHEIGHT(MLVL,25,240) \
   MSET_ABILITY(8,27,15,30,10,10,0,0) \
   MSET_WEAPON(6,6,6,6,6,6) \
   MSET_SPELL(4,6,6,6,6,6,6,6,6,6,6)

#define M_WEASLE_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_CLOTHES)

#define M_WEASLE_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_CLOTHES)

#define M_WEASLE_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_CLOTHES)

#define M_WEASLE_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_CLOTHES)

#define M_WEASLE_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_LEATHER)

#define M_WEASLE_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_WEASLE(MLVL,GENDER,ARM_LEATHER)

#define M_WEASLE_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_WEASLE(MLVL,GENDER,ARM_HLEATHER)  \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n chatters loudly as it bites you!", \
                                "$1n chatters loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* WEASLE end */

/* MOUNTAIN LION */
#define BASE_MOUNTAIN_LION(MLVL,GENDER,ARM) \
   race RACE_MOUNTAIN_LION \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,8,400) \
   SETHEIGHT(MLVL,22,300) \
   NATURAL_DEF(WPN_CLAW, ARM) \
   alignment 0  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_MOUNTAIN_LION_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)

#define M_MOUNTAIN_LION_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_MOUNTAIN_LION(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n growls loudly as it claws you!", \
                                "$1n growls loudly as it claws $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* MOUNTAIN LION end */

/* SQUIRREL */
#define BASE_SQUIRREL(MLVL,GENDER,ARM) \
   race RACE_SQUIRREL \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,150) \
   SETHEIGHT(MLVL,8,180) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 750  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_SQUIRREL_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_LARGE(MLVL,GENDER)  /* level 76 to 100 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)

#define M_SQUIRREL_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_SQUIRREL(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n growls loudly as it bites you!", \
                                "$1n growls loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* SQUIRREL end */

/* RACCOON */
#define BASE_RACCOON(MLVL,GENDER,ARM) \
   race RACE_RACCOON \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,160) \
   SETHEIGHT(MLVL,15,150) \
   NATURAL_DEF(WPN_BITE, ARM) \
   alignment 750 /* there cute little bandits*/ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5)

#define M_RACCOON_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)

#define M_RACCOON_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_RACCOON(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n growls loudly as it bites you!", \
                                "$1n growls loudly as it bites $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
                            {"$1n attacks $3n!", "$1n attacks you!"}); \
   exp 125

/* RACCOON end */

/* ELK */
#define BASE_ELK(MLVL,GENDER,ARM) \
   race RACE_ELK \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_KICK,ARM) \
   alignment 349 \
   SETWEIGHT(MLVL,130,1800) \
   SETHEIGHT(MLVL,66,300) \
   exp 100 \
   alignment -200 \
   MSET_ABILITY(25,22,20,20,13,0,0,0) \
   MSET_WEAPON( 10, 10, 10, 10, 20, 10 ) \
   MSET_SPELL( 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 )


#define M_ELK_TINY(MLVL,GENDER) /*level 1 to 25 */ \
   BASE_ELK(MLVL,GENDER,ARM_CLOTHES)

#define M_ELK_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_ELK(MLVL,GENDER,ARM_LEATHER)

#define M_ELK_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_ELK(MLVL,GENDER,ARM_LEATHER)

#define M_ELK_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_ELK(MLVL,GENDER,ARM_HLEATHER)

#define M_ELK_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_ELK(MLVL,GENDER,ARM_CHAIN)

#define M_ELK_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_ELK(MLVL,GENDER,ARM_PLATE)

#define M_ELK_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_ELK(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n manages to kick you tearing your flesh!", \
                                "$1n kicks $3n tearing $3s flesh!"); \
   dilcopy aggressive@function(0, -1, 0, 40, 3, 100, 1, "OPPOSITE", \
                 {"$1n attacks $3n!", "$1n attacks you!"}); \
	 exp 125

/* ELK end */

/* CHICKEN */
#define BASE_CHICKEN(MLVL,GENDER,ARM) \
   race RACE_CHICKEN \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_BITE,ARM) \
   alignment 350 \
   SETWEIGHT(MLVL,1,25) \
   SETHEIGHT(MLVL,15,90) \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_WEAPON(5,5,5,5,20,5)      \
   MSET_SPELL(5,5,5,5,5,5,5,5,5,5,5)

#define M_CHICKEN_TINY(MLVL,GENDER) /* level 1 to 25 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_CLOTHES)

#define M_CHICKEN_SMALL(MLVL,GENDER) /* level 26 to 51 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_LEATHER)

#define M_CHICKEN_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_HLEATHER)

#define M_CHICKEN_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_HLEATHER)

#define M_CHICKEN_XLARGE(MLVL,GENDER) /* level 101 to 120 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_CHAIN)

#define M_CHICKEN_GODLY(MLVL,GENDER) /* level 121 to 199 */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_PLATE)

#define M_CHICKEN_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */ \
   BASE_CHICKEN(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -400  \
   dilcopy comb_poison@function("$1n manages to sink its beak into you!", \
                                "$1n sinks $1s beak deep into $3n!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, 100, 0, "ANY", \
                            {"$1n attacks!", "$1n attacks you!"}); \
   exp 125

/* CHICKEN end */

/* SHEEP */
#define BASE_SHEEP(MLVL,GENDER,ARM) \
   race RACE_SHEEP \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,5,600) \
   SETHEIGHT(MLVL,25,180) \
   NATURAL_DEF(WPN_KICK, ARM) \
   alignment 0       \
   MSET_ABILITY(20,22,20,20,18,0,0,0) /* Is that a SHEEP? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_SHEEP_TINY(MLVL,GENDER) /*level 1 to 25 */\
   BASE_SHEEP(MLVL,GENDER,ARM_LEATHER)

#define M_SHEEP_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)

#define M_SHEEP_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)

#define M_SHEEP_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)

#define M_SHEEP_XLARGE(MLVL,GENDER) /* level 101 to 120 */\
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)

#define M_SHEEP_GODLY(MLVL,GENDER) /* level 121 to 199 */\
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)

#define M_SHEEP_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */\
   BASE_SHEEP(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n rams you with it horns!", \
                                "$1n rams $3n with $1s horns!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
   {"$1n rams $3n with $1s horns!", "$1n rams you with it horns!"}); \
   exp 125

/* SHEEP end */

/* GOAT */
#define BASE_GOAT(MLVL,GENDER,ARM) \
   race RACE_GOAT \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,1,580) \
   SETHEIGHT(MLVL,45,180) \
   NATURAL_DEF(WPN_KICK, ARM) \
   alignment 0       \
   MSET_ABILITY(20,24,20,20,16,0,0,0) /* Is that a GOAT? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */

#define M_GOAT_TINY(MLVL,GENDER) /*level 1 to 25 */\
   BASE_GOAT(MLVL,GENDER,ARM_LEATHER)

#define M_GOAT_SMALL(MLVL,GENDER) /* level 26 to 50 */ \
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)

#define M_GOAT_MEDIUM(MLVL,GENDER) /* level 51 to 75 */ \
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)

#define M_GOAT_LARGE(MLVL,GENDER) /* level 76 to 100 */ \
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)

#define M_GOAT_XLARGE(MLVL,GENDER) /* level 101 to 120 */\
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)

#define M_GOAT_GODLY(MLVL,GENDER) /* level 121 to 199 */\
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)

#define M_GOAT_MAD(MLVL,GENDER) /* level 1 to 199 has poison and aggro */\
   BASE_GOAT(MLVL,GENDER,ARM_HLEATHER)  \
   level MLVL \
   sex GENDER \
   alignment -150  \
   MSET_ABILITY(15,15,10,15,15,10,10,10) \
   MSET_WEAPON(6,6,6,6,10,6)          \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6) \
   dilcopy comb_poison@function("$1n rams you with it horns!", \
                                "$1n rams $3n with $1s horns!"); \
   dilcopy aggressive@function(0, -1, 0, 0, 3, WAIT_SEC*25, 0, "ANY", \
   {"$1n rams $3n with $1s horns!", "$1n rams you with it horns!"}); \
   exp 125

/* GOAT end */




#endif
