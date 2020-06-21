/* *********************************************************************** *
 * File   : composed.h                                Part of Valhalla MUD *
 * Version: 3.01                                                           *
 * Author : All
 *                                                                         *
 * Purpose: Definitions for a right menagerie                              *
 *								           *
 * Comments:  Please send all comments and additions to one of the above   *
 *            authors.                                                     *
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

#ifndef _MUD_COMPOSED_H
#define _MUD_COMPOSED_H

#include "wmacros.h"
#include "liquid.h"
#include "base.h"


/*

   ORDINARY  :   Level   0 -  50
   FANTASTIC :   Level  51 - 100
   DEMI-GODS :   Level 101 - 150
   GODS      :   Level 151 - 199

*/


/*
   Monsters are categorized by their race and sorted by their level in the
   definitions below.

   The "humanoid" macros are an execption to this and are located at the
   end of the file.

   Use macros whenever possible - if no appropriate macros exists, then please
   make your own similar macro and it will be included in this file in time
   to come...

   Remember that you can (usually) override macro defaults by specifying
   for example a different alignment AFTER the macro. Exmaple:

      A good troll:

      M_TROLL(85, SEX_FEMALE)
      alignment +1000

   You can't however, override flags or special routines. These can only
   be added (i.e. for a dragon macro it is not possible to override the
   breath weapon attack.
*/


/*
   #define MSET_ABILITY(str,dex,con,hpp,bra,cha,mag,div)
   #define MSET_WEAPON(axe_ham, sword, club_mace, flail_pole, unarmed, special)
   #define MSET_SPELL(div, pro, det, sum, cre, min, hea, col, cel, int, ext)
*/



/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                                 MAMMALS                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_BIRD_TINY(GENDER) \
   race RACE_BIRD \
   level 0 \
   sex GENDER \
   height    10  /* cm      */ \
   weight     3  /* Pounds  */ \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   alignment 0 \
   MSET_ABILITY(1,1,1,1,1,95,0,0)     /* Is that a Bird? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_SQUIRREL \
   race RACE_SQUIRREL \
   level 0 \
   sex SEX_NEUTRAL \
   height 15  \
   weight 35 \
   alignment 350 \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   MSET_ABILITY(15,45,25,5,5,5,0,0) \
   MSET_WEAPON(6, 6, 7, 5, 60, 6 ) \
   MSET_SPELL ( 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 0 )


#define M_RABBIT(GENDER)      \
   race RACE_RABBIT \
   level 1 \
   sex GENDER \
   height    15       /* 15cm          */ \
   weight     8       /* 8 Pounds      */ \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   alignment 200      /* Neutral->Good */ \
   MSET_ABILITY(5,10,5,5,5,70,0,0)    /* Rabbits are CUTE! */ \
   MSET_WEAPON(6,6,6,6,6,6)           /* Not a fighter! :) */ \
   MSET_SPELL(4,6,6,6,6,6,6,6,6,6,6)  /* Resistances       */


#define M_DOG_PUPPY(GENDER)  \
   race RACE_DOG    \
   level 1 \
   sex GENDER       \
   height    60        /* cm             */ \
   weight    44        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(10,10,10,10,5,55,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */ \
   alignment +1000 /* They're too cute :) */


#define M_OWL(GENDER) \
   race RACE_OWL \
   level 1 \
   sex GENDER \
   height 70 \
   weight 40 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment 0 \
   MSET_ABILITY(20,50,15,15,0,0,0,0) \
   MSET_WEAPON(10,10,10,10,30,10)      \
   MSET_SPELL(1,0,0,0,2,2,3,3,3,3,3)


#define M_SKUNK(GENDER)  \
   race RACE_SKUNK    \
   level 2  \
   sex GENDER       \
   height    60        /* cm             */ \
   weight    44        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(15,15,15,15,10,30,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_COW(GENDER) \
   race RACE_COW \
   level 2 \
   height 152 /* cm*/ \
   weight 300 /* Pounds*/ \
   NATURAL_DEF(WPN_KICK, ARM_HLEATHER) \
   alignment 0 \
   MSET_ABILITY(20,15,20,10,5,30,0,0)  \
   MSET_WEAPON(6,6,6,6,15,6)\
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_FOX \
   race RACE_FOX \
   level 2 \
   sex SEX_NEUTRAL \
   height 90  \
   weight 50 \
   alignment 350 \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   MSET_ABILITY(30, 40, 10, 10, 2, 8, 0, 0) \
   MSET_WEAPON ( 6, 6, 7, 5, 60, 6 ) \
   MSET_SPELL ( 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 0 )


#define M_CAT(GENDER) \
   level 2     \
   height   30 \
   weight   20 \
   race RACE_CAT   \
   alignment 400   \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   MSET_ABILITY(20,50,10,10,2,8,0,0)  \
   MSET_WEAPON(6,6,7,5,60,6)          /* 90%, 60% Natural */ \
   MSET_SPELL(0,0,0,0,0,0,4,3,3,0,0)  /* 10%, resistances */


#define M_DOG_BEAGLE(GENDER)  \
   race RACE_DOG    \
   level 2          \
   sex GENDER       \
   height    60        /* cm             */ \
   weight    44        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(25,10,15,15,5,30,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_HORSE_PONY(GENDER) \
   race RACE_HORSE   \
   level 2 \
   height   150       /* cm               */ \
   weight   800       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   MSET_ABILITY(20,20,15,10,5,30,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_EAGLE(GENDER)  \
   race RACE_EAGLE \
   level 3 \
   sex GENDER \
   height    50 \
   weight    45 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment 0 \
   MSET_ABILITY(25,30,20,10,5,10,0,0)  \
   MSET_WEAPON(2,2,2,2,35,2) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_VULTURE(GENDER)  /* BIRD */ \
   race RACE_VULTURE \
   level 3 \
   sex GENDER \
   height    35 \
   weight    60 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment -250 \
   MSET_ABILITY(15,20,20,10,5,10,10,10)  \
   MSET_WEAPON(2,2,2,2,35,2) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
dilcopy fido@function("","");


#define M_DEER(GENDER) \
   race RACE_DEER   \
   level 3 \
   height   200       /* cm               */ \
   weight   242       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   alignment 349 \
   MSET_ABILITY(12,15,12,12,9,40,0,0) /* Is that a deer? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_HORSE_LIGHT(GENDER) \
   race RACE_HORSE   \
   level 3 \
   height   170       /* cm               */ \
   weight   900       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   MSET_ABILITY(20,20,15,10,5,30,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_BADGER(GENDER)  \
   race RACE_BADGER    \
   level 4  \
   sex GENDER       \
   height    60        /* cm             */ \
   weight    44        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(15,15,20,10,20,20,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_MONKEY_THIEF(GENDER) \
  race RACE_MONKEY \
  level 4 \
  weight 60 \
  height 60 \
  sex GENDER \
  alignment 0 \
  NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
  MSET_ABILITY ( 10, 50, 10, 10, 20, 0, 0, 0) \
  MSET_WEAPON (20,20,20,20,10,10) \
  MSET_SPELL ( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)


#define M_HORSE_MEDIUM(GENDER) \
   race RACE_HORSE   \
   level 4 \
   height   180       /* cm               */ \
   weight  1000       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   MSET_ABILITY(20,20,15,10,5,30,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_ELK(GENDER) \
   race RACE_ELK \
   level 5 \
   sex GENDER \
   height 200  \
   weight 350 \
   alignment 350 \
   NATURAL_DEF(WPN_KICK, ARM_HLEATHER) \
   MSET_ABILITY (20, 20, 10, 10, 10, 10, 0, 20) \
   MSET_WEAPON ( 10, 10, 10, 10, 10, 20 ) \
   MSET_SPELL ( 10, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 )

#define M_DOG_ROTTWEILER(GENDER)  \
   race RACE_DOG    \
   level 5 \
   sex GENDER       \
   height    60        /* cm             */ \
   weight    44        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(10,10,10,10,5,55,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_HORSE_HEAVY(GENDER) \
   race RACE_HORSE   \
   level 5 \
   height   200       /* cm               */ \
   weight  1100       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   MSET_ABILITY(20,20,15,10,5,30,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_MOUSE_FIELD_GIANT(GENDER)  \
   race RACE_MOUSE \
   sex GENDER \
   level 6 \
   height 155 \
   weight 190 \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   alignment  150 \
   MSET_ABILITY(35,30,20,10,5,0,0,0)  \
   MSET_WEAPON(2,2,2,2,35,2) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


#define M_BOAR(GENDER)  \
   race RACE_BOAR    \
   level 6  \
   sex GENDER       \
   height   100        /* cm             */ \
   weight   100        /* Pounds         */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(20,20,20,20,10,10,0,0) /* Is that a dog? */ \
   MSET_WEAPON(6,6,6,6,10,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,6,6,6,6,6,6,6,6)  /* Resistances    */


#define M_WOLF_GREY(GENDER) \
   race RACE_WOLF   \
   level 6 \
   height   150   /* cm               */ \
   weight   150   /* Pounds           */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   exp 125 \
   alignment -200 \
   MSET_ABILITY(15,15,15,15,10,30,0,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           \
   MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)   \
dilcopy aggressive@function (0, -1, 0, 0, 3, PULSE_SEC*60, 0, "ANY", {"",""});


#define M_RAT_GIANT(GENDER) \
   race RACE_RAT \
   level 7 \
   sex GENDER \
   height 125 \
   weight 330 \
   NATURAL_DEF(WPN_BITE, ARM_HLEATHER) \
   alignment -750 /* Neutral evil */ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5) /* Perhaps add a BITE_DISEASE */


#define M_WOLF_BLACK(GENDER) \
   race RACE_WOLF   \
   level 8 \
   height   170   /* cm               */ \
   weight   170   /* Pounds           */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   exp 125 \
   alignment -200 \
   MSET_ABILITY(15,15,15,15,10,30,0,0) \
   MSET_WEAPON(6,6,6,6,16,6)          \
   MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)


#define M_BEAR_BROWN(GENDER) \
   race RACE_BEAR   \
   level 8 \
   height   250       /* cm               */ \
   weight   881       /* Pounds           */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   alignment 10 \
   MSET_ABILITY(20,15,20,20,5,20,0,0) /* Is that a Bear? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_MONKEY_LEMURE(GENDER) /* Makier half-money race */ \
  race RACE_LEMURE \
  level 9 \
  weight 280 \
  height 190 \
  sex GENDER \
  alignment -900 \
  NATURAL_DEF(WPN_CRUSH, ARM_CHAIN) \
  MSET_ABILITY ( 15, 5, 10, 25, 5, 0, 10, 10) \
  MSET_WEAPON (0,0,0,0,55,0) \
  MSET_SPELL ( 0, 0, 0, 0, 0, 0, 45, 0, 0, 0, 0) \


#define M_RAVEN_GIANT(GENDER) \
   race RACE_BIRD \
   level 10 \
   sex GENDER \
   height 60\
   weight 35 \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -50 \
   MSET_ABILITY(10,15,20,10,5,10,10,10) \
   MSET_WEAPON(2,2,2,2,35,2)      \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_WARG_VIC(GENDER) \
   race RACE_WOLF   \
   level 12 \
   height   200   /* cm               */ \
   weight   200   /* Pounds           */ \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   exp 125 \
   alignment -200 \
   MSET_ABILITY(15,15,15,15,10,30,0,0)  \
   MSET_WEAPON(6,6,6,6,16,6)            \
   MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)


#define M_BEAR_BLACK(GENDER) \
   race RACE_BEAR   \
   level 13 \
   height   250       /* cm               */ \
   weight   881       /* Pounds           */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   alignment 10 \
   MSET_ABILITY(20,15,20,10,5,30,0,0) /* Is that a Bear? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_LION_MOUNTAIN \
  race RACE_LION \
  level 13 \
  weight 300 \
  height 200 \
  sex SEX_NEUTRAL \
  alignment 0 \
  NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
  MSET_ABILITY(15,15,15,15,10,30,0,0)  \
  MSET_WEAPON(6,6,6,6,16,6)            \
  MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)


#define M_RABBIT_KILLER      \
   race RACE_RABBIT \
   level 15 \
   sex SEX_NEUTRAL \
   height    15       /* 15cm          */ \
   weight     8       /* 8 Pounds      */ \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   alignment 120      /* Neutral->Good */ \
   MSET_ABILITY(5,10,5,5,5,70,0,0)    /* Rabbits are CUTE! */ \
   MSET_WEAPON(6,6,6,6,6,6)           /* Not a fighter! :) */ \
   MSET_SPELL(4,6,6,6,6,6,6,6,6,6,6)  /* Resistances       */


#define M_LEOPARD \
  race RACE_LEOPARD \
  level 15 \
  weight 150 \
  height 200 \
  sex SEX_NEUTRAL \
  alignment 0 \
  NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
  MSET_ABILITY(15,15,15,15,10,30,0,0)  \
  MSET_WEAPON(6,6,6,6,16,6)            \
  MSET_SPELL(4,4,4,4,4,4,6,6,6,6,6)


#define M_WOLF_ARCTIC(GENDER) \
   race RACE_WOLF \
   level 16 \
   height  170 \
   weight  170 \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   exp 125 \
   alignment -250 \
   MSET_ABILITY(20,15,20,20,10,15,0,0) \
   MSET_WEAPON(6,6,6,6,15,6) \
   MSET_SPELL(4,4,4,4,4,6,3,8,6,6,6) \
dilcopy aggressive@function (0, -1, 0, 0, 3, PULSE_SEC*60, 0, "ANY", {"",""});

#define M_BEAR_GRIZZLY(GENDER) \
   race RACE_BEAR   \
   level 17 \
   height   250       /* cm               */ \
   weight   881       /* Pounds           */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   alignment 10 \
   MSET_ABILITY(20,15,20,10,5,30,0,0) /* Is that a Bear? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_BEAR_POLAR(GENDER) \
   race RACE_BEAR   \
   level 21 \
   height   250       /* cm               */ \
   weight   881       /* Pounds           */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   alignment 10 \
   MSET_ABILITY(20,15,20,10,5,30,0,0) /* Is that a Bear? */ \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_RAVEN_HUGIN(GENDER) \
   race RACE_BIRD \
   level 24 \
   sex GENDER \
   height 60\
   weight 35 \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -50 \
   MSET_ABILITY(35,30,20,10,5,0,0,0) \
   MSET_WEAPON(0,0,0,0,10,0)      \
   MSET_SPELL(0,0,0,90,0,0,0,0,0,0,0)


#define M_RAVEN_MUNIN(GENDER) \
   race RACE_BIRD \
   level 25 \
   sex GENDER \
   height 60\
   weight 35 \
   NATURAL_DEF(WPN_BITE, ARM_LEATHER) \
   alignment -50 \
   MSET_ABILITY(35,30,20,10,5,0,0,0) \
   MSET_WEAPON(0,0,0,0,10,0)      \
   MSET_SPELL(0,0,0,90,0,0,0,0,0,0,0)


#define M_EAGLE_SERVANT(GENDER)  \
   race RACE_EAGLE \
   level 25 \
   sex GENDER \
   height    150 \
   weight    210 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment -450 \
   MSET_ABILITY(35,30,20,10,5,0,0,0)  \
   MSET_WEAPON(2,2,2,2,35,2) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_HORSE_FRIGGA(GENDER) \
   race RACE_HORSE   \
   level 28 \
   height   200       /* cm               */ \
   weight  1000       /* Pounds           */ \
   NATURAL_DEF(WPN_KICK, ARM_LEATHER) \
   MSET_ABILITY(35,30,20,10,5,0,0,0) \
   MSET_WEAPON(2,2,2,2,35,2)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_EAGLE_GIANT(GENDER)  \
   race RACE_EAGLE \
   level 30 \
   sex GENDER \
   height    150 \
   weight    210 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment -250 \
   MSET_ABILITY(35,30,20,10,5,0,0,0)  \
   MSET_WEAPON(2,2,2,2,35,2) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


/* Breed between Creature & Wolf. Guards for Necromancers */
#define M_WOLF_VARAC(GENDER) \
   race RACE_WOLF \
   level 45 \
   height 200 \
   weight 400 \
   sex GENDER \
   alignment -1000 \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   MSET_ABILITY ( 25, 25, 10, 20, 6, 0, 14, 0) \
   MSET_WEAPON  (  2, 1, 3, 4, 25, 5 ) \
   MSET_SPELL   ( 10, 0, 0, 3, 0, 0, 10, 0, 8, 20, 9) \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");



/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                               REPTILES                                 */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_TOAD(GENDER) \
   race RACE_TOAD   \
   level 0 \
   height   5 \
   weight  1 \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES)  \
   MSET_ABILITY(5,35,15,10,20,15,0,0) \
   MSET_WEAPON(6,6,6,6,26,6)           /* Natural attack */ \
   MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)   /* Resistances    */


#define M_SNAKE_VIPER \
    race RACE_SNAKE \
    level 3 \
    sex SEX_NEUTRAL \
    height 5 \
    weight 2 \
    alignment -100 \
    NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
    MSET_ABILITY(20,20,10,10,10,10,0,20) \
    MSET_WEAPON(0,0,0,0,70,0) \
    MSET_SPELL(10,2,2,2,2,2,2,2,2,2,2) \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


#define M_SNAKE_ASP \
    race RACE_SNAKE \
    level 5 \
    sex SEX_NEUTRAL \
    height 5 \
    weight 2 \
    alignment -100 \
    NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
    MSET_ABILITY(20,20,10,10,10,10,0,20) \
    MSET_WEAPON(0,0,0,0,70,0) \
    MSET_SPELL(10,2,2,2,2,2,2,2,2,2,2) \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


#define M_BAT_GIANT(GENDER) \
   race RACE_BAT \
   level 11 \
   sex GENDER \
   height 200  /* 6' wingspan */ \
   weight 50 \
   NATURAL_DEF(WPN_BITE, ARM_HLEATHER) \
   alignment -300 /* Barely evil */ \
   MSET_ABILITY(15,20,20,10,5,30,0,0) \
   MSET_WEAPON(6,6,6,6,15,6)          \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)


#define M_ALLIGATOR  \
   race RACE_ALLIGATOR   \
   level 14 \
   sex SEX_NEUTRAL \
   height   300   /* cm      */ \
   weight   300  /* Pounds  */ \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -100      /* Neutral        */ \
   MSET_ABILITY(30,30,20,15,5,0,0,0)  \
   MSET_WEAPON(20,20,20,20,10,10)           \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)


#define M_DINO_TRI(GENDER) \
    race RACE_DINOSAUR \
    sex GENDER \
    level 40 \
    weight 5000 \
    height 600 \
    NATURAL_DEF(WPN_CRUSH, ARM_PLATE) \
    MSET_ABILITY(50,10,20,19,1,0,0,0) \
    MSET_WEAPON(20,20,20,20,10,10)    \
    alignment -1000

#define M_DINO_RAPTOR(GENDER) \
    race RACE_DINOSAUR \
    sex GENDER \
    level 60 \
    weight 10000 \
    height 600 \
    NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
    MSET_ABILITY(20,35,20,20,5,0,0,0) \
    MSET_WEAPON(20,20,20,20,10,10)    \
    alignment -1000

#define M_DINO_REX(GENDER) \
    race RACE_DINOSAUR \
    sex GENDER \
    level 75 \
    weight 5000 \
    height 600 \
    NATURAL_DEF(WPN_BITE, ARM_PLATE) \
    MSET_ABILITY(50,10,20,19,1,0,0,0) \
    MSET_WEAPON(20,20,20,20,10,10)    \
    alignment -1000

/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                               CREATURES                                */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_DUST_DEVIL \
   race RACE_ELEMENTAL_AIR \
   level 4 \
   sex SEX_NEUTRAL \
   height 75  \
   weight 1 \
   alignment 0 \
   NATURAL_DEF(WPN_CRUSH, ARM_HLEATHER) \
   MSET_ABILITY (30, 20, 25, 20, 5, 0, 0, 0) \
   MSET_WEAPON ( 2, 2, 2, 2, 40, 2 ) \
   MSET_SPELL ( 5, 0, 0, 1, 1, 6, 6, 6, 5, 17, 3 )

#define M_DRAGON_TURTLE_BABY(GENDER)  \
   BASE_DRAGON_TURTLE(8, GENDER)

#define M_STONE_RENDER(GENDER) \
   race RACE_STONE_RENDER \
   level 8 \
   sex GENDER \
   height 200 \
   weight 700 \
   alignment -800 \
   NATURAL_DEF(WPN_CRUSH, ARM_PLATE) \
   MSET_ABILITY(25,25,25,25,0,0,0,0) /* 100 % */ \
   MSET_WEAPON(0,0,0,0,90,0)         /*  90 % */ \
   MSET_SPELL(0,0,0,0,0,0,2,2,2,2,2) /*  10 % */


#define M_CAVE_SERVANT(GENDER) \
    race RACE_CAVE_WIGHT \
    sex GENDER \
    level 8 \
    height   220   /* cm */ \
    weight   450   /* Pounds */ \
    alignment -800 \
    NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
    MSET_ABILITY(30,20,20,15,5,2,6,2) \
    MSET_WEAPON(5,5,5,5,40,5) /* 65 % */ \
    MSET_SPELL(0,5,5,0,0,0,0,5,5,10,5)


#define M_GOLEM_FLESH(GENDER) \
   BASE_GOLEM(8, SEX_NEUTRAL)

#define M_CAVE_WIGHT_DELVER(GENDER) \
    race RACE_CAVE_WIGHT \
    sex GENDER \
    level 9 \
    height   220   /* cm */ \
    weight   450   /* Pounds */ \
    alignment -800 \
    NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
    MSET_ABILITY(30,20,20,15,5,2,6,2) \
    MSET_WEAPON(5,5,5,5,40,5) /* 65 % */ \
    MSET_SPELL(0,5,5,0,0,0,0,5,5,10,5)

#define M_DRAGON_SILVER_BABY(GENDER)  \
   BASE_DRAGON_SILVER(9, GENDER)

#define M_CAVE_WIGHT_FIGHTER(GENDER) \
    race RACE_CAVE_WIGHT \
    sex GENDER \
    level 10 \
    height   220   /* cm */ \
    weight   450   /* Pounds */ \
    alignment -800 \
    NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
    MSET_ABILITY(30,20,20,15,5,2,6,2) \
    MSET_WEAPON(5,5,5,5,40,5) /* 65 % */ \
    MSET_SPELL(0,5,5,0,0,0,0,5,5,10,5)


#define M_DRAGON_BLACK_BABY(GENDER)  \
   BASE_DRAGON_BLACK(10, GENDER)

#define M_DRAGON_BLUE_BABY(GENDER)  \
   BASE_DRAGON_BLUE(11, GENDER)

#define M_DRAGON_GREEN_BABY(GENDER)  \
   BASE_DRAGON_GREEN(12, GENDER)

#define M_DRAGON_SHADOW_BABY(GENDER)  \
   BASE_DRAGON_SHADOW(12, GENDER)

#define M_GOLEM_CLAY(GENDER) \
   BASE_GOLEM(12, SEX_NEUTRAL)

#define M_GARGOYLE_STONE( GENDER) \
   BASE_GARGOYLE(12, GENDER)

#define M_DEMON_LESSER(GENDER) \
    race RACE_LESSER_DEMON \
    sex GENDER \
    level 13 \
    weight 250 \
    height 160 \
    NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
    MSET_ABILITY(15,15,10,10,15,0,30,5) /* 100 % */ \
    MSET_WEAPON(2,2,2,2,20,2)           /*  30 % */ \
    MSET_SPELL(7,1,1,5,1,11,7,2,25,6,4) /*  70 % */ \
    alignment -1000 \
    dilcopy combat_mag@function("lightning bolt", "", 25, 2);

#define M_DRAGON_LAVA_BABY(GENDER)  \
   BASE_DRAGON_LAVA(13, GENDER)

#define M_UR_VILE(GENDER) \
    race RACE_UR_VILE \
    sex GENDER \
    level 13 \
    alignment -950   \
    height 150+36 \
    weight 350+3*36 \
    NATURAL_DEF(WPN_FIST, ARM_CLOTHES)  \
    MSET_ABILITY(18,15,12,20,10,5,15,5) /* 100 % */ \
    MSET_WEAPON(6,6,6,6,40,6) /* 65 % */ \
    MSET_SPELL(5,2,3,0,0,0,0,5,5,5,5)

#define M_DRAGON_LIZARD_BABY(GENDER)  \
   BASE_DRAGON_LIZARD(13, GENDER)

#define M_DRAGON_WHITE_BABY(GENDER)  \
   BASE_DRAGON_WHITE(14, GENDER)

#define M_GOLEM_TAR(GENDER) \
   BASE_GOLEM(14, SEX_NEUTRAL)

#define M_MIST_DWELLER \
   race RACE_MIST_DWELLER \
   level 15 \
   weight 75 \
   height 105 \
   sex SEX_NETURAL \
   alignment 0 \
   NATURAL_DEF(WPN_FIST, ARM_HLEATHER) \
   MSET_ABILITY ( 5, 5, 20, 10, 35, 0, 25, 0) \
   MSET_WEAPON (1,1,2,2,2,2) \
   MSET_SPELL ( 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 80)\
   dilcopy combat_mag@function("acid ball", "", 25, 2);

#define M_DRAGON_RED_BABY(GENDER)  \
   BASE_DRAGON_RED(16, GENDER)

#define M_ELEMENTAL_FIRE \
   race RACE_ELEMENTAL_FIRE \
   level 17 \
   sex SEX_NEUTRAL \
   height 200  \
   weight 100 \
   alignment -350 \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
   MSET_ABILITY (40, 10, 25, 20, 5, 0, 0, 0) \
   MSET_WEAPON ( 2, 2, 2, 2, 40, 2 ) \
   MSET_SPELL ( 5, 0, 0, 1, 1, 6, 6, 6, 5, 17, 3 )\
   dilcopy combat_mag@function("burn", "", 25, 2);

#define M_UR_VILE_LOREMASTER(GENDER) \
    race RACE_UR_VILE \
    sex GENDER \
    level 18 \
    alignment -950   \
    height 150+44 \
    weight 350+3*44 \
    NATURAL_DEF(WPN_FIST, ARM_CLOTHES)  \
    MSET_ABILITY(18,15,12,20,10,5,15,5) /* 100 % */ \
    MSET_WEAPON(6,6,6,6,40,6) /* 65 % */ \
    MSET_SPELL(5,2,3,0,0,0,0,5,5,5,5)

#define M_GOLEM_BRASS(GENDER) \
   BASE_GOLEM(18, SEX_NEUTRAL)

#define M_DRAGON_TURTLE_YOUNG(GENDER)  \
   BASE_DRAGON_TURTLE(18, GENDER)

#define M_DRAGON_SILVER_YOUNG(GENDER)  \
   BASE_DRAGON_SILVER(19, GENDER)

#define M_DRAGON_BLACK_YOUNG(GENDER)  \
   BASE_DRAGON_BLACK(20, GENDER)

#define M_DEMON_AVG(GENDER) \
    race RACE_LESSER_DEMON \
    sex GENDER \
    level 20 \
    weight 250 \
    height 160 \
    NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
    MSET_ABILITY(15,15,10,10,15,0,30,5) /* 100 % */ \
    MSET_WEAPON(2,2,2,2,20,2)           /*  30 % */ \
    MSET_SPELL(7,1,1,5,1,11,7,2,25,6,4) /*  70 % */ \
    alignment -1000 \
    dilcopy combat_mag@function("firestorm", "", 25, 2);

#define M_DRAGON_BLUE_YOUNG(GENDER)  \
   BASE_DRAGON_BLUE(21, GENDER)

#define M_TAR_BEAST \
   race RACE_GOLEM\
   alignment -1000\
   sex SEX_NEUTRAL\
   level 21\
   height 200\
   weight 200\
   NATURAL_DEF(WPN_FIST, ARM_PLATE)\
   exp 140\
   MSET_ABILITY(20, 12, 12, 12, 12, 12, 20, 0)\
   MSET_WEAPON(10, 5, 5, 5, 30, 5)\
   MSET_SPELL(0,0,0,0,0,5,5,0,0,30,0)\
   dilcopy combat_mag@function("acid ball", "", 25, 2);


#define M_LAVA_BEAST \
   race RACE_ELEMENTAL_FIRE\
   alignment -1000\
   sex SEX_NEUTRAL\
   level 22\
   height 200\
   weight 200\
   NATURAL_DEF(WPN_FIST, ARM_PLATE)\
   exp 140\
   MSET_ABILITY(20, 12, 12, 12, 12, 12, 20, 0)\
   MSET_WEAPON(10, 5, 5, 5, 30, 5)\
   MSET_SPELL(5,0,0,0,0,5,30,0,0,0,0)\
   dilcopy combat_mag@function("fireball", "", 25, 2);

#define M_DRAGON_GREEN_YOUNG(GENDER)  \
   BASE_DRAGON_GREEN(22, GENDER)

#define M_DRAGON_SHADOW_YOUNG(GENDER)  \
   BASE_DRAGON_SHADOW(22, GENDER)


#define M_YOGOLOTH(GENDER) \
   race RACE_YOGOLOTH \
   level 23 \
   height 210 \
   weight 400 \
   sex GENDER \
   alignment -1000 \
   NATURAL_DEF(WPN_BITE, ARM_PLATE) \
   MSET_ABILITY ( 20, 12, 12, 12, 12, 12, 20, 0) \
   MSET_WEAPON  ( 10, 5, 10, 5, 35, 5 ) \
   MSET_SPELL   ( 0, 0, 0, 0, 0, 0, 30, 0, 0, 0, 0)


#define M_GOLEM_STONE(GENDER) \
   BASE_GOLEM(23, SEX_NEUTRAL)


#define M_DRAGON_LAVA_YOUNG(GENDER)  \
   BASE_DRAGON_LAVA(23, GENDER)

#define M_DRAGON_LIZARD_YOUNG(GENDER)  \
   BASE_DRAGON_LIZARD(23, GENDER)

#define M_DRAGON_WHITE_YOUNG(GENDER)  \
   BASE_DRAGON_WHITE(24, GENDER)

#define M_NORSE_STATUE_TYR \
   race RACE_GOLEM \
   level 25 \
   sex SEX_NEUTRAL \
   height 600 \
   weight 800 \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
   alignment 0 \
   MSET_ABILITY(25,20,15,10,10,10,10,0) \
   MSET_WEAPON(30,20,10,5,20,15)     \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)


#define M_DRAGON_RED_YOUNG(GENDER)  \
   BASE_DRAGON_RED(26, GENDER)

#define M_NORSE_STATUE_FRIGGA \
   race RACE_GOLEM \
   level 26 \
   sex SEX_NEUTRAL \
   height 600 \
   weight 800 \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
   alignment 0 \
   MSET_ABILITY(25,20,15,10,10,10,10,0) \
   MSET_WEAPON(25,25,10,5,20,15)     \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)


#define M_NORSE_STATUE_ODIN \
   race RACE_GOLEM \
   level 27 \
   sex SEX_NEUTRAL \
   height 600 \
   weight 800 \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
   alignment 0 \
   MSET_ABILITY(25,20,15,10,10,10,10,0) \
   MSET_WEAPON(20,20,10,15,20,15)     \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)

#define M_GOLEM_IRON(GENDER) \
   BASE_GOLEM(28, SEX_NEUTRAL)

#define M_GOLEM_STEEL(GENDER) \
   BASE_GOLEM(32, SEX_NEUTRAL)

#define M_DRAGON_TURTLE_MATURE(GENDER)  \
   BASE_DRAGON_TURTLE(38 , GENDER)

#define M_DRAGON_SILVER_MATURE(GENDER)  \
   BASE_DRAGON_SILVER(39, GENDER)

#define M_DRAGON_BLACK_MATURE(GENDER)  \
   BASE_DRAGON_BLACK(40 , GENDER)

#define M_DRAGON_BLUE_MATURE(GENDER)  \
   BASE_DRAGON_BLUE(41, GENDER)

#define M_DRAGON_GREEN_MATURE(GENDER)  \
   BASE_DRAGON_GREEN(42, GENDER)

#define M_DRAGON_SHADOW_MATURE(GENDER)  \
   BASE_DRAGON_SHADOW(42, GENDER)

#define M_DRAGON_LAVA_MATURE(GENDER)  \
   BASE_DRAGON_LAVA(43, GENDER)

#define M_DRAGON_LIZARD_MATURE(GENDER)  \
   BASE_DRAGON_LIZARD(43, GENDER)

#define M_DRAGON_WHITE_MATURE(GENDER)  \
   BASE_DRAGON_WHITE(44, GENDER)

#define M_DEMON_GREATER(GENDER) \
    race RACE_GREATER_DEMON \
    sex GENDER \
    level 45 \
    weight 250 \
    height 200 \
    NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
    MSET_ABILITY(15,15,10,10,15,0,30,5) /* 100 % */ \
    MSET_WEAPON(2,2,2,2,20,2)           /*  30 % */ \
    MSET_SPELL(7,1,1,5,1,11,7,2,25,6,4) /*  70 % */ \
    alignment -1000 \
    dilcopy combat_mag@function("acid ball", "", 25, 2);

#define M_DRAGON_RED_MATURE(GENDER)  \
   BASE_DRAGON_RED(46, GENDER)


#define M_UNICORN(GENDER) \
   race RACE_UNICORN \
   sex GENDER \
   level 65 \
   weight 370 /* 185 kilos */ \
   height 170 /* 1,70 m  */ \
   NATURAL_DEF(WPN_STING, ARM_CHAIN) \
   alignment 1000 \
   MSET_ABILITY(10, 15, 10, 15, 15, 10,  15,  10) \
   MSET_WEAPON( 5, 10, 5, 10, 5, 5) \
   MSET_SPELL( 10, 0, 0, 13, 0, 10, 5, 6, 6, 5, 5) \
   dilcopy combat_mag@function("colourspray", "heal", 25, 2);

#define M_DRAGON_TURTLE_OLD(GENDER)  \
   BASE_DRAGON_TURTLE(68,  GENDER)

#define M_DRAGON_SILVER_OLD(GENDER)  \
   BASE_DRAGON_SILVER(69, GENDER)

#define M_DRAGON_BLACK_OLD(GENDER)  \
   BASE_DRAGON_BLACK(70, GENDER)

#define M_DRAGON_BLUE_OLD(GENDER)  \
   BASE_DRAGON_BLUE(71, GENDER)

#define M_DRAGON_GREEN_OLD(GENDER)  \
   BASE_DRAGON_GREEN(72, GENDER)

#define M_DRAGON_SHADOW_OLD(GENDER)  \
   BASE_DRAGON_SHADOW(72, GENDER)

#define M_DRAGON_LAVA_OLD(GENDER)  \
   BASE_DRAGON_LAVA(73, GENDER)

#define M_DRAGON_LIZARD_OLD(GENDER)  \
   BASE_DRAGON_LIZARD(73, GENDER)

#define M_DRAGON_WHITE_OLD(GENDER)  \
   BASE_DRAGON_WHITE(74, GENDER)

#define M_DRAGON_RED_OLD(GENDER)  \
   BASE_DRAGON_RED(76, GENDER)


/* The ancient dragons barely reach the Demigod status */

#define M_DRAGON_TURTLE_ANCIENT(GENDER)  \
   BASE_DRAGON_TURTLE(98, GENDER)

#define M_DRAGON_SILVER_ANCIENT(GENDER)  \
   BASE_DRAGON_SILVER(109, GENDER)

#define M_DRAGON_BLACK_ANCIENT(GENDER)  \
   BASE_DRAGON_BLACK(110, GENDER)

#define M_DRAGON_BLUE_ANCIENT(GENDER)  \
   BASE_DRAGON_BLUE(111, GENDER)

#define M_DRAGON_GREEN_ANCIENT(GENDER)  \
   BASE_DRAGON_GREEN(112, GENDER)

#define M_DRAGON_SHADOW_ANCIENT(GENDER)  \
   BASE_DRAGON_SHADOW(112, GENDER)

#define M_DRAGON_LAVA_ANCIENT(GENDER)  \
   BASE_DRAGON_LAVA(113, GENDER)

#define M_DRAGON_LIZARD_ANCIENT(GENDER)  \
   BASE_DRAGON_LIZARD(113, GENDER)

#define M_DRAGON_WHITE_ANCIENT(GENDER)  \
   BASE_DRAGON_WHITE(114, GENDER)

#define M_DRAGON_RED_ANCIENT(GENDER)  \
   BASE_DRAGON_RED(116, GENDER)

/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                               UNDEAD                                   */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_ZOMBIE_TWISTED \
	BASE_ZOMBIE(3, SEX_NEUTRAL)

#define M_ZOMBIE_HOLLOW \
	BASE_ZOMBIE(4, SEX_NEUTRAL)

#define M_SPIRIT_WEAK(GENDER) \
   BASE_SPIRIT(5, GENDER)

#define M_GHOUL_SWAMP(MLVL, GENDER) \
	BASE_GHOUL(5, SEX_NEUTRAL)

#define M_ZOMBIE_DWARF \
	BASE_ZOMBIE(5, SEX_NEUTRAL)

#define M_ZOMBIE_STINKING \
	BASE_ZOMBIE(6, SEX_NEUTRAL)

#define M_ZOMBIE_AVG \
	BASE_ZOMBIE(6, SEX_NEUTRAL)

#define M_GHOST_WEAK(GENDER) \
	BASE_GHOST(6, GENDER)

#define M_GHOUL_LESSER(GENDER) \
   race RACE_GHOST \
   level 6 \
   height   100 \
   weight   120 \
   NATURAL_DEF(WPN_CLAW, ARM_CHAIN) \
   exp 150  \
   alignment -950                 \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);

#define M_SPIRIT_LESSER(GENDER) \
   BASE_SPIRIT(9, GENDER)

#define M_MUMMIE_AVG(GENDER) \
	BASE_MUMMIE(10, GENDER)

#define M_GHOUL_AVG(GENDER) \
   race RACE_GHOST \
   level 11 \
   height   100 \
   weight   120 \
   NATURAL_DEF(WPN_CLAW, ARM_CHAIN) \
   exp 150  \
   alignment -950                 \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);

#define M_GHOST_LESSER(GENDER) \
	BASE_GHOST(11, GENDER)

#define M_ZOMBIE_GIANT \
	BASE_ZOMBIE(12, SEX_NEUTRAL)

#define M_SPIRIT_AVG(GENDER) \
        BASE_SPIRIT(13, GENDER)

#define M_SKELETON(GENDER) \
	BASE_SKELETON(13, GENDER)

#define M_GHOUL_GREATER(GENDER) \
   race RACE_GHOST \
   level 16 \
   height   100 \
   weight   120 \
   NATURAL_DEF(WPN_CLAW, ARM_CHAIN) \
   exp 150  \
   alignment -950                 \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);


#define M_BANSHEE \
  weight 100 \
  height 170 \
  level 18 \
  race RACE_BANSHEE \
  sex SEX_NEUTRAL \
  alignment -800 \
  NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
  MSET_ABILITY (10, 10, 20, 10, 20, 10, 10, 10)  \
  MSET_WEAPON ( 3, 3, 3, 3, 5, 5) \
  MSET_SPELL ( 10, 0, 3, 10, 10, 10, 5, 5, 5, 15, 5) \
  dilcopy combat_mag@function("madness", "", 25, 2);


#define M_SKELETON_BLACK(GENDER) \
	BASE_SKELETON(18, GENDER)

#define M_GHOST_AVG(GENDER) \
	BASE_GHOST(19, GENDER)

#define M_SPIRIT_GREATER(GENDER) \
        BASE_SPIRIT(20, GENDER)

#define M_LICH_AGMARR \
	BASE_LICH(24, SEX_MALE)

#define M_GHOST_GREATER(GENDER) \
	BASE_GHOST(25, GENDER)

#define M_LICH_KILBIA \
	BASE_LICH(40, SEX_FEMALE)

#define M_LICH_JOLIEN \
	BASE_LICH(43, SEX_FEMALE)

/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                                      MARINE                            */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_CRAB_SMALL \
   romflags CHAR_MUTE \
   race RACE_CRAB \
   level 0 \
   sex SEX_NEUTRAL \
   height 13 \
   weight 4 \
   NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
   alignment 0 \
   MSET_ABILITY(25,20,20,20,15,0,0,0) \
   MSET_WEAPON(5,10,5,10,10,10)      \
   MSET_SPELL(10,0,0,0,0,10,0,0,5,5,20)

#define M_CRAB_GIANT \
   romflags CHAR_MUTE \
   race RACE_CRAB \
   level 7 \
   sex SEX_NEUTRAL \
   height 130 \
   weight 280 \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   alignment 0 \
   MSET_ABILITY(25,20,20,20,15,0,0,0) \
   MSET_WEAPON(5,10,5,10,10,10)      \
   MSET_SPELL(10,0,0,0,0,10,0,0,5,5,20)


#define M_CAVE_FISHER(GENDER) \
   race RACE_CAVE_FISHER \
   level 15 \
   height   150   /* cm               */ \
   weight   250   /* Pounds           */ \
   NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
   MSET_ABILITY(19,21,21,19,12,8,0,0)  \
   MSET_WEAPON(6,6,6,6,15,6)          /* Natural attack */ \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6)  /* Resistances    */


#define M_SHARK \
   race RACE_SHARK  \
   level 16         \
   alignment 0      \
   sex SEX_NEUTRAL  \
   NATURAL_DEF(WPN_BITE,ARM_HLEATHER) \
   weight 500 \
   height 300 \
   MSET_ABILITY(30,20,20,30,0,0,0,0) \
   MSET_WEAPON(10,10,10,10,50,10)


#define M_OCTOPUS(GENDER) \
   race RACE_OCTOPUS \
   level 22 \
   sex GENDER \
   height 100+(5*30) \
   weight 200+(10*30)\
   NATURAL_DEF(WPN_CRUSH, ARM_CHAIN) \
   MSET_ABILITY(30,20,10,20,5,0,15,0) \
   MSET_WEAPON(0,0,0,0,85,0) \
   /* A decent saving throw versus magic */ \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)


/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                               PLANTS                                   */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_LOSKA \
   race RACE_OTHER_PLANT \
   level 6 \
   weight 35 \
   height 39 \
   sex SEX_NEUTRAL \
   alignment -950 \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) \
   MSET_ABILITY ( 10, 5, 5, 5, 35, 10, 10, 20) \
   MSET_WEAPON (0,0,0,0,10,0) \
   MSET_SPELL ( 30, 0, 0, 0, 0, 20, 0, 0, 20, 20, 0)


#define M_CACTUS_HUGE \
   race RACE_CACTUS \
   level 30\
   sex SEX_NEUTRAL\
   height 600\
   weight 150\
   alignment -500\
   NATURAL_DEF(WPN_CLAW, ARM_CHAIN)\
   MSET_ABILITY(30,25,10,25,5,5,0,0)\
   MSET_WEAPON(5,5,5,5,25,10)\
   MSET_SPELL(3,5,3,4,5,4,4,3,7,4,3)


#define M_TREE(GENDER) \
   race RACE_TREE   \
   level 45 \
   height   600 \
   weight  1200 \
   NATURAL_DEF(WPN_CRUSH, ARM_HLEATHER) \
   MSET_ABILITY(33,12,27,23,2,3,0,0) \
   MSET_WEAPON(6,6,6,6,26,6)           /* Natural attack */ \
   MSET_SPELL(4,4,4,4,4,4,4,4,4,4,4)   /* Resistances    */



/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                               INSECTS                                  */
/*                                                                        */
/* ---------------------------------------------------------------------- */

#define M_ANT_LARVAE(GENDER) \
	BASE_ANT(2, GENDER)

#define M_ANT_DRONE(GENDER) \
	BASE_ANT(3, GENDER)

#define M_ANT_SOLDIER(GENDER) \
	BASE_ANT(3, GENDER)

#define M_ANT_WORKER(GENDER) \
	BASE_ANT(3, GENDER)

#define M_COCKROACH_GIANT(GENDER) \
   race RACE_COCKROACH \
   level 3 \
   sex GENDER \
   height 200 \
   weight 330 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -500 /* evil */ \
   MSET_ABILITY(20,25,15,20,10,10,0,0) \
   MSET_WEAPON(10,10,10,10,20,5)      \
   MSET_SPELL(5,0,0,0,0,5,5,5,5,5,5) /* Perhaps add a BITE_DISEASE */

#define M_ANT_FIRE(GENDER) \
	BASE_ANT(4, GENDER)\
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");

#define M_ANT_QUEEN(GENDER) \
	BASE_ANT(4, GENDER)


#define M_LEECH \
  level 5 \
  weight 40 \
  height 20 \
  race RACE_LEECH \
  sex SEX_NEUTRAL \
  alignment 0 \
  NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
  MSET_ABILITY ( 40, 25, 10, 20, 5, 0, 0, 0) \
  MSET_WEAPON (20,20,20,10,30,10) \
  MSET_SPELL ( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0) \


#define M_MOSQUITO_GIANT  \
   race RACE_MOSQUITO \
   sex SEX_NEUTRAL \
   level 8 \
   height 15 \
   weight 10 \
   NATURAL_DEF(WPN_STING, ARM_CLOTHES) \
   exp       125  \
   alignment  -150 \
   MSET_ABILITY(10,40,10,20,10,10,0,0)  \
   MSET_WEAPON(20,20,20,20,10,10) \
   MSET_SPELL(5,5,5,20,5,0,0,0,0,15,0) \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");

#define M_BUTTERFLY_MARLEY \
	BASE_BUTTERFLY(10, SEX_MALE)

#define M_CLEAN_SPIDER(MLVL, GENDER) \
   race RACE_SPIDER \
   level MLVL \
   height    50+MLVL   /* cm               */ \
   weight   100+MLVL   /* Pounds           */ \
   NATURAL_DEF(WPN_STING, ARM_CHAIN) \
   exp 100  /* Poison sting */     \
   alignment -500                 \
   MSET_ABILITY(14,16,16,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,25,11) /* Attack poison  */ \

#define M_SPIDER_GIANT(GENDER) \
   race RACE_SPIDER \
   level 14  \
   height   205   /* cm               */ \
   weight   555   /* Pounds           */ \
   NATURAL_DEF(WPN_STING, ARM_CHAIN) \
   exp 120  /* Poison sting */     \
   alignment -1000                 \
   MSET_ABILITY(14,16,16,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,25,11) /* Attack poison  */ \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


#define M_DRAGONFLY  \
   race RACE_DRAGONFLY \
   level 17 \
   sex SEX_NEUTRAL \
   height    175 \
   weight    600 \
   NATURAL_DEF(WPN_CRUSH, ARM_CHAIN) \
   alignment 0 \
   MSET_ABILITY(20,50,10,19,1,0,0,0)  \
   MSET_WEAPON(20,20,20,20,10,10)


#define M_MAGGOT(MLVL,GENDER)  \
   race RACE_MAGGOT \
   level  MLVL \
   sex GENDER \
   height   150+MLVL \
   weight    150+MLVL\
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -450 \
   MSET_ABILITY(15,20,20,10,5,10,10,10)  \
   MSET_WEAPON(6,6,5,6,16,6) \
   MSET_SPELL(4,4,4,5,5,5,5,5,6,6,6) \
dilcopy fido@function("","");

#define M_TRIPHID(MLVL,GENDER) \
   race RACE_OTHER_INSECT /* RACE_TRIPHID*/ \
   level MLVL \
   height   150+MLVL   /* cm               */ \
   weight   500+MLVL   /* Pounds           */ \
   alignment -1000                 \
   NATURAL_DEF(WPN_STING, ARM_CHAIN) \
   exp 120  /* Poison sting */     \
   MSET_ABILITY(18,16,12,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(2,2,2,2,2,2,3,2,2,25,10) /* Attack poison  */ \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


#define M_SPIDER(MLVL, GENDER) \
   race RACE_SPIDER \
   level MLVL \
   height    50+MLVL   /* cm               */ \
   weight   100+MLVL   /* Pounds           */ \
   NATURAL_DEF(WPN_STING, ARM_CHAIN) \
   exp 120  /* Poison sting */     \
   alignment -500                 \
   MSET_ABILITY(14,16,16,14,12,8,20,0)  \
   MSET_WEAPON(6,6,6,6,16,6)           /* Natural attack */ \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,25,11) /* Attack poison  */ \
dilcopy comb_poison@function("You feel very ill as $1n bites you!", "$1n sinks $1s fangs deep into $3n!");


/* ---------------------------------------------------------------------- */
/*                                                                        */
/*                          HUMANOIDS                                     */
/*                                                                        */
/* ---------------------------------------------------------------------- */

/* Forrest Brownie like */
#define M_SHARGUGH(GENDER, ATT_SPL) \
   race RACE_BROWNIE \
   sex GENDER \
   level 5 \
   height 80 \
   weight 80 \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER) \
   alignment 750  \
   MSET_ABILITY(11,24,18,8,12,10,17,0) \
   MSET_SPELL( 0, 0, 0, 0, 0,14, 4, 7, 7, 4, 7) /* 43% Mind primary */ \
   MSET_WEAPON(8,7,9,7,23,3)          /* 57% Unarmed primary */  \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);


#define M_GIANT_STONE(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 6 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)



#define M_GIANT_WATER(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 7 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)




#define M_GIANT_FROST(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 8 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)




#define M_GIANT_FIRE(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 8 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)



#define M_TROLL_FOREST(GENDER) \
   race RACE_TROLL \
   level 8 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)



#define M_GIANT_FOREST(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 9 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)



#define M_TROLL_STONE(GENDER) \
   race RACE_TROLL \
   level 10 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)


#define M_TROLL_WAR(GENDER) \
   race RACE_TROLL \
   level 12 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)


#define M_MINOTAUR\
   race RACE_MINOTAUR\
   alignment -1000\
   sex SEX_MALE\
   level 12\
   height 200\
   weight 300\
   NATURAL_DEF(WPN_FIST, ARM_PLATE)\
   exp 130\
   MSET_ABILITY(20, 12, 12, 22, 12, 12, 10, 0)\
   MSET_WEAPON(45, 5, 10, 5, 5, 5)\
   MSET_SPELL(5,0,0,0,5,0,5,0,0,5,5)\
   dilcopy combat_mag@function("madness", "", 25, 2);


#define M_TROLL_HILL(GENDER) \
   race RACE_TROLL \
   level 13 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)



#define M_GIANT_MOUNTAIN(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 14 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)



#define M_TROLL_MOUNTAIN(GENDER) \
   race RACE_TROLL \
   level 14 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)


#define M_TROLL_CAVE(GENDER) \
   race RACE_TROLL \
   level 15 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)



#define M_GIANT_CLOUD(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 16 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)



#define M_TROLL_SNOW(GENDER) \
   race RACE_TROLL \
   level 16 \
   sex GENDER \
   height 320 \
   weight 1000 \
   NATURAL_DEF(WPN_BITE, ARM_CHAIN) \
   alignment -900 \
   MSET_ABILITY (30,25,10,25,2,8,0,0) \
   MSET_WEAPON (10,10,10,10,35,5) \
   MSET_SPELL (2,2,2,2,2,2,2,2,2,2,0)



#define M_GIANT_STORM(GENDER) \
   race RACE_GIANT \
   sex GENDER \
   level 18 \
   height 400 \
   weight 1200 \
   alignment -700 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY(45,20,15,18,1,1,0,0) /* 100 % */ \
   MSET_WEAPON(8,4,40,5,18,5)        /*  80 % */ \
   MSET_SPELL(2,0,0,2,0,0,5,5,2,2,2)




/* USE THESE DEFFINITIONS OF THE NORSE GODS - They are GODS! */

#define M_NORSE_ODIN \
	BASE_NORSE_GOD3(199, SEX_MALE)

#define M_NORSE_HELA \
	BASE_NORSE_GOD3(195, SEX_FEMALE)

#define M_NORSE_THOR \
	BASE_NORSE_GOD1(190, SEX_MALE)

#define M_NORSE_TYR \
	BASE_NORSE_GOD2(185, SEX_MALE)

#define M_NORSE_FREYA \
	BASE_NORSE_GOD3(180, SEX_FEMALE)

#define M_NORSE_FRIGGA \
	BASE_NORSE_GOD3(175, SEX_FEMALE)

#define M_NORSE_HEIMDALL \
	BASE_NORSE_GOD1(170, SEX_MALE)

#define M_NORSE_HERMOD \
	BASE_NORSE_GOD1(165, SEX_MALE)

#define M_NORSE_HODER \
	BASE_NORSE_GOD1(160, SEX_MALE)


/* ==================================================================== */

/* What the heck is this????? */

#define M_SANCTUARIAN(MLVL, GENDER) \
  race RACE_OTHER_HUMANOID \
  sex GENDER \
  level MLVL \
  height 200 \
  weight 200 \
  alignment 1000 \
  NATURAL_DEF(WPN_FIST, ARM_CLOTHES) \
  MSET_ABILITY(5,20,10,10,15,10,10,20) \
  MSET_WEAPON(2,2,20,2,2,2) \
  MSET_SPELL(15,10,10,10,10,1,1,1,1,10,1) \



/* ==================================================================== */
/*                           STANDARD DWARFS                            */
/* ==================================================================== */

#define M_AVG_DWARF(MLVL, MGENDER) \
   BASE_DWARF(MLVL, MGENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_DWARF_THIEF_AXE(MLVL, GENDER) \
   BASE_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_DWARF_THIEF_SWORD(MLVL, GENDER) \
   BASE_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_DWARF_THIEF_CLUB(MLVL, GENDER) \
   BASE_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_DWARF_THIEF_POLE(MLVL, GENDER) \
   BASE_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_DWARF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_DWARF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_DWARF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_DWARF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_DWARF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_DWARF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_DWARF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_DWARF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_DWARF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_DWARF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_DWARF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_DWARF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_DWARF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_DWARF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_DWARF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_DWARF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD ELFS                              */
/* ==================================================================== */

#define M_AVG_ELF(MLVL, MGENDER) \
   BASE_ELF(MLVL, MGENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GNOME                             */
/* ==================================================================== */

#define M_AVG_GNOME(MLVL, MGENDER) \
   BASE_GNOME(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_GNOME_THIEF_AXE(MLVL, GENDER) \
   BASE_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GNOME_THIEF_SWORD(MLVL, GENDER) \
   BASE_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GNOME_THIEF_CLUB(MLVL, GENDER) \
   BASE_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_GNOME_THIEF_POLE(MLVL, GENDER) \
   BASE_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_GNOME_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_GNOME_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_GNOME_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_GNOME_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_GNOME_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_GNOME_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_GNOME_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_GNOME_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_GNOME_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_GNOME_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_GNOME_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_GNOME_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_GNOME_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_GNOME_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_GNOME_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_GNOME_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GOBLIN                            */
/* ==================================================================== */

#define M_AVG_GOBLIN(MLVL, MGENDER) \
   BASE_GOBLIN(MLVL, MGENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_GOBLIN_THIEF_AXE(MLVL, GENDER) \
   BASE_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GOBLIN_THIEF_SWORD(MLVL, GENDER) \
   BASE_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GOBLIN_THIEF_CLUB(MLVL, GENDER) \
   BASE_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_GOBLIN_THIEF_POLE(MLVL, GENDER) \
   BASE_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_GOBLIN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_GOBLIN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_GOBLIN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_GOBLIN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_GOBLIN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_GOBLIN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_GOBLIN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_GOBLIN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_GOBLIN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_GOBLIN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_GOBLIN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_GOBLIN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_GOBLIN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_GOBLIN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_GOBLIN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_GOBLIN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HALFLINGS                         */
/* ==================================================================== */


#define M_AVG_HALFLING(MLVL, MGENDER) \
   BASE_HALFLING(MLVL, MGENDER)              \
   MSET_ABILITY(10,15,10,23,20,22,0,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */


#define M_HALFLING_THIEF_AXE(MLVL, GENDER) \
   BASE_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALFLING_THIEF_SWORD(MLVL, GENDER) \
   BASE_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALFLING_THIEF_CLUB(MLVL, GENDER) \
   BASE_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_HALFLING_THIEF_POLE(MLVL, GENDER) \
   BASE_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/*-------------------------------------------------------------------*/

#define M_HALFLING_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_HALFLING_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_HALFLING_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_HALFLING_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_HALFLING_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(30,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_HALFLING_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,20,20,30,10,5)    /* 85% Pole-category */

#define M_HALFLING_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(30,20,20,15,10,5)     /* 85% Axe / Hammer */

#define M_HALFLING_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(20,15,30,15,15,5)     /* 85% Club/Mace/Flail    */

#define M_HALFLING_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(20,15,15,15,30,5)     /* 85% Club/Mace/Flail    */

#define M_HALFLING_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,20,15,15,5,30)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HOBGOBLIN                         */
/* ==================================================================== */

#define M_AVG_HOBGOBLIN(MLVL, MGENDER) \
   BASE_HOBGOBLIN(MLVL, MGENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_HOBGOBLIN_THIEF_AXE(MLVL, GENDER) \
   BASE_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HOBGOBLIN_THIEF_SWORD(MLVL, GENDER) \
   BASE_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HOBGOBLIN_THIEF_CLUB(MLVL, GENDER) \
   BASE_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_HOBGOBLIN_THIEF_POLE(MLVL, GENDER) \
   BASE_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_HOBGOBLIN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_HOBGOBLIN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_HOBGOBLIN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_HOBGOBLIN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_HOBGOBLIN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_HOBGOBLIN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_HOBGOBLIN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_HOBGOBLIN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_HOBGOBLIN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_HOBGOBLIN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_HOBGOBLIN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_HOBGOBLIN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_HOBGOBLIN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_HOBGOBLIN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_HOBGOBLIN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_HOBGOBLIN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */



/* ==================================================================== */
/*                           STANDARD HUMANS                            */
/* ==================================================================== */


/* This is your standard Udgaard / Midgaard shop keeper. */
#define M_SHOP_KEEPER(MLVL, GENDER, MRACE)      \
   race MRACE \
   level MLVL \
   sex GENDER \
   romflags {CHAR_PROTECTED, CHAR_WIMPY} \
   height   170       /* 170cm         */ \
   weight   120       /* 120 Pounds    */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)     \
   alignment 355      /* Good          */ \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_AVG_HUMAN(MLVL, MGENDER, MRACE) \
   BASE_HUMAN(MLVL, MGENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_HUMAN_THIEF_AXE(MLVL, GENDER) \
   BASE_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HUMAN_THIEF_SWORD(MLVL, GENDER) \
   BASE_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HUMAN_THIEF_CLUB(MLVL, GENDER) \
   BASE_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_HUMAN_THIEF_POLE(MLVL, GENDER) \
   BASE_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_HUMAN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_HUMAN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_HUMAN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_HUMAN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_HUMAN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_HUMAN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_HUMAN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_HUMAN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_HUMAN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_HUMAN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_HUMAN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_HUMAN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_HUMAN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_HUMAN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_HUMAN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_HUMAN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD LIZARDMAN                         */
/* ==================================================================== */

#define M_AVG_LIZARDMAN(MLVL, MGENDER, MRACE) \
   BASE_LIZARDMAN(MLVL, MGENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_LIZARDMAN_THIEF_AXE(MLVL, GENDER) \
   BASE_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_LIZARDMAN_THIEF_SWORD(MLVL, GENDER) \
   BASE_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_LIZARDMAN_THIEF_CLUB(MLVL, GENDER) \
   BASE_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_LIZARDMAN_THIEF_POLE(MLVL, GENDER) \
   BASE_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_LIZARDMAN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_LIZARDMAN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_LIZARDMAN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_LIZARDMAN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_LIZARDMAN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_LIZARDMAN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_LIZARDMAN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_LIZARDMAN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_LIZARDMAN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_LIZARDMAN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_LIZARDMAN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_LIZARDMAN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_LIZARDMAN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_LIZARDMAN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_LIZARDMAN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_LIZARDMAN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD OCRS                              */
/* ==================================================================== */

#define M_AVG_ORC(MLVL, MGENDER) \
   BASE_ORC(MLVL, MGENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_ORC_THIEF_AXE(MLVL, GENDER) \
   BASE_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_ORC_THIEF_SWORD(MLVL, GENDER) \
   BASE_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_ORC_THIEF_CLUB(MLVL, GENDER) \
   BASE_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_ORC_THIEF_POLE(MLVL, GENDER) \
   BASE_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_ORC_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_ORC_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_ORC_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_ORC_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_ORC_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_ORC_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_ORC_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_ORC_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_ORC_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_ORC_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_ORC_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_ORC_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_ORC_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_ORC_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_ORC_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_ORC_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */
/* ==================================================================== */


#endif










