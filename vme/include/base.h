/* *********************************************************************** *
 * File   : base.h                                    Part of Valhalla MUD *
 * Version: 2.03                                                           *
 * Author : All
 *                                                                         *
 * Purpose: Definitions for a right menagerie                              *
 *									   *
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

#ifndef _MUD_BASE_H
#define _MUD_BASE_H


#define BASE_GOLEM(MLVL, GENDER) \
    race RACE_GOLEM \
    sex GENDER \
    level MLVL \
    height 250 \
    weight 700 \
    alignment 0 \
    NATURAL_DEF(WPN_FIST, ARM_PLATE) \
    MSET_ABILITY(30,20,20,30,0,0,0,0)  \
    MSET_WEAPON(10,30,10,10,10,10)     \
    MSET_SPELL(0,0,5,5,5,5,0,0,0,0,0)


#define BASE_ANT(MLVL, GENDER) \
  weight 12 * MLVL\
  height 35 * MLVL/2\
  level MLVL \
  race RACE_ANT \
  sex GENDER \
  alignment 0 \
  NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
  MSET_ABILITY (40, 15, 15, 30, 0, 0, 0, 0)  \
  MSET_WEAPON ( 0, 5, 5, 5, 70, 5) \
  MSET_SPELL ( 1, 0, 0, 0, 0, 2, 2, 2, 2, 2, 1)

#define BASE_BUTTERFLY(MLVL, GENDER) \
   race RACE_OTHER_CREATURE \
   level MLVL \
   sex GENDER \
   height    10 + MLVL/4   /* cm               */ \
   weight     3 + MLVL/4   /* Pounds           */ \
   NATURAL_DEF(WPN_BITE, ARM_CLOTHES) \
   alignment 0 \
   MSET_ABILITY(20,15,10,10,15,15,15,0)      \
   MSET_WEAPON(8,8,8,8,8,0)          \
   MSET_SPELL(4,4,4,20,4,4,4,4,4,4,4)

/*----------------------------BASE DRAGONS--------------------------*/


#define BASE_DRAGON(MLVL, GENDER, BREATH)  \
   sex GENDER \
   level MLVL \
   height    400 + MLVL*15 \
   weight    800 + MLVL*30 \
   NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
   exp       150  /* For breath weapon */ \
   dilcopy combat_mag@function(BREATH, "", 25, 2);

#define BASE_DRAGON_BLACK(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "acid breath")  \
   MSET_ABILITY(20,12,12,12,12,12,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,30) \
   alignment -750 \
   race RACE_DRAGON_BLACK

#define BASE_DRAGON_BLUE(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "lightning breath")  \
   MSET_ABILITY(20,12,12,12,14,10,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,0,0,30,0,0) \
   alignment -1000 \
   race RACE_DRAGON_BLUE

#define BASE_DRAGON_GREEN(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "gas breath")  \
   MSET_ABILITY(20,12,12,12,14,10,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,30,0) \
   alignment -1000 \
   race RACE_DRAGON_GREEN

#define BASE_DRAGON_LAVA(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "fire breath")   \
   MSET_ABILITY(20,12,12,12,14,10,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,30,0,0,0,0) \
   alignment -1000 \
   race RACE_DRAGON_LAVA

#define BASE_DRAGON_RED(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "fire breath")   \
   MSET_ABILITY(20,12,12,12,16,8,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,30,0,0,0,0) \
   alignment -750 \
   race RACE_DRAGON_RED

#define BASE_DRAGON_WHITE(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "frost breath")   \
   MSET_ABILITY(20,14,12,12,10,12,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,0,30,0,0,0) \
   alignment -750 \
   race RACE_DRAGON_WHITE

#define BASE_DRAGON_SILVER(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "light breath")   \
   MSET_ABILITY(20,12,12,12,16,8,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,30,0,0,0,0,0) \
   alignment 1000 \
   race RACE_DRAGON_SILVER

#define BASE_DRAGON_TURTLE(MLVL, GENDER)  \
   BASE_DRAGON(MLVL, GENDER, "fire breath")   \
   MSET_ABILITY(20,12,12,12,14,10,20,0)  \
   MSET_WEAPON(10,10,10,5,30,5) \
   MSET_SPELL(0,0,0,0,0,0,30,0,0,0,0) \
   alignment 0 \
   race RACE_DRAGON_TURTLE


#define BASE_GARGOYLE(MLVL, GENDER) \
   race RACE_GARGOYLE \
   level MLVL \
   sex GENDER \
   height 50 + MLVL \
   weight 200 + 3*MLVL \
   alignment -700 \
   NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
   MSET_ABILITY (40, 10, 25, 20, 5, 0, 0, 0) \
   MSET_WEAPON ( 2, 2, 2, 2, 40, 2 ) \
   MSET_SPELL ( 5, 0, 0, 1, 1, 6, 6, 6, 5, 17, 3 )

#define BASE_GHOST(MLVL, GENDER) \
   race RACE_GHOST \
   level MLVL \
   height   180 + MLVL/4 \
   weight   150 + MLVL\
   NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
   exp 120  \
   alignment -950                 \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);

#define BASE_GHOUL(MLVL, GENDER) \
   race RACE_GHOST \
   level MLVL \
   height   80 + MLVL/4\
   weight   120 +MLVL/2\
   NATURAL_DEF(WPN_CLAW, ARM_CHAIN) \
   exp 125  \
   alignment -950                 \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);


#define BASE_LICH(MLVL, GENDER) \
  level MLVL \
  weight 160 + MLVL/4 \
  height 200 + MLVL/2 \
  race RACE_LICH \
  sex GENDER \
  alignment -1000 \
  NATURAL_DEF(WPN_FIST, ARM_CHAIN) \
  MSET_ABILITY ( 30, 14, 10, 5, 12, 10, 4, 15) \
  MSET_WEAPON (13, 6, 4, 5, 5, 0) \
  MSET_SPELL ( 15, 6, 0, 0, 6, 10, 5, 5, 10, 5, 5) \
  dilcopy combat_mag@function("energy drain", "", 25, 2);

#define BASE_MUMMIE(MLVL, GENDER) \
  race RACE_MUMMIE \
  level MLVL \
  sex GENDER \
  height 160+ MLVL/4 \
  weight 160+ MLVL \
  NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
  alignment -550 \
  MSET_ABILITY(15,20,20,10,5,30,0,0) \
  MSET_WEAPON(10,10,10,10,15,10)     \
  MSET_SPELL(0,0,0,5,5,5,5,5,5,5,0)

/* -------------------- BASE NORSE GODS -------------------------- */

#define BASE_NORSE_GOD1(MLVL, GENDER)  \
   race RACE_NORSE_GOD \
   sex GENDER \
   level MLVL \
   height    350 \
   weight    400 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   exp        70  \
   alignment  550 \
   MSET_ABILITY(10,15,10,10,10,15,15,15)  \
   MSET_WEAPON(15,10,5,5,5,5) \
   MSET_SPELL(5,5,5,20,5,0,0,0,0,15,0) \
   dilcopy combat_mag@function("lightning", "heal", 25, 2);


#define BASE_NORSE_GOD2(MLVL, GENDER)  \
   race RACE_NORSE_GOD \
   sex GENDER \
   level MLVL \
   height    350 \
   weight    400 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   exp        70 \
   alignment  750 \
   MSET_ABILITY(15,20,10,15,10,10,10,10)  \
   MSET_WEAPON(5,15,5,5,20,10) \
   MSET_SPELL(0,0,0,0,0,0,20,0,0,20,0) \
   dilcopy combat_mag@function("firestorm", "heal", 25, 2);
 

#define BASE_NORSE_GOD3(MLVL, GENDER)  \
   race RACE_NORSE_GOD \
   sex GENDER \
   level MLVL \
   height    350 \
   weight    400 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   exp        70  \
   alignment  850 \
   MSET_ABILITY(15,15,12,15,11,10,11,11)  \
   MSET_WEAPON(5,10,5,10,10,0) \
   MSET_SPELL(12,0,0,8,0,0,12,8,8,0,12) \
   dilcopy combat_mag@function("acid ball", "heal", 25, 2);


#define BASE_SKELETON(MLVL, GENDER) \
   weight 100 + 2*MLVL \
   height 150 + MLVL/2 \
   level MLVL \
   race RACE_SKELETON \
   sex SEX_NEUTRAL \
   alignment -600 \
   NATURAL_DEF(WPN_CLAW, ARM_PLATE) \
   MSET_ABILITY (24, 20, 30, 10, 6, 5, 5, 0) \
   MSET_WEAPON ( 5, 10, 10, 10, 20, 10) \
   MSET_SPELL ( 3, 0, 0, 0, 0, 5, 5, 5, 5, 7, 5)

#define BASE_SPIRIT(MLVL, GENDER) \
   weight 100 + MLVL  \
   height 150 + MLVL/2 \
   level 50 \
   race RACE_SPIRIT \
   sex SEX_NEUTRAL \
   alignment -600 \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   MSET_ABILITY (10,30,15,15,5,0,0,25) \
   MSET_WEAPON(6,6,6,6,20,6) \
   MSET_SPELL(2,2,2,2,2,2,2,2,2,21,11) \
   dilcopy combat_mag@function("energy drain", "", 25, 2);


#define BASE_ZOMBIE(MLVL, GENDER) \
  race RACE_ZOMBIE \
  level MLVL \
  sex GENDER \
  height 160 + MLVL/4 \
  weight 160 + MLVL/2 \
  NATURAL_DEF(WPN_CLAW, ARM_LEATHER) \
  alignment -550 \
  MSET_ABILITY(15,20,20,10,5,30,0,0) \
  MSET_WEAPON(10,10,10,10,15,10)     \
  MSET_SPELL(0,0,0,5,5,5,5,5,5,5,0)


/* ==================== STANDARD HUMANOID RACES ======================= */


/* ==================================================================== */
/*                           STANDARD DWARFS                            */
/* ==================================================================== */

#define BASE_DWARF(MLVL, GENDER) \
   race RACE_DWARF \
   level MLVL \
   sex GENDER \
   height   121       /* cm            */ \
   weight   150       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_DWARF_WARRIOR(MLVL, GENDER) \
   BASE_DWARF(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_DWARF_THIEF(MLVL, GENDER)                      \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD ELFS                              */
/* ==================================================================== */

#define BASE_ELF(MLVL, GENDER) \
   race RACE_ELF \
   level MLVL \
   sex GENDER \
   height   152       /* cm            */ \
   weight   105       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_ELF_WARRIOR(MLVL, GENDER) \
   BASE_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,18,12,25,10,10,5,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_ELF_THIEF(MLVL, GENDER)                      \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD GNOMES                            */
/* ==================================================================== */

#define BASE_GNOME(MLVL, GENDER) \
   race RACE_GNOME \
   level MLVL \
   sex GENDER \
   height   100       /* cm            */ \
   weight   84        /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_GNOME_WARRIOR(MLVL, GENDER) \
   BASE_GNOME(MLVL, GENDER)                  \
   MSET_ABILITY(15,18,15,20,15,12,5,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);


#define BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GNOME_THIEF(MLVL, GENDER)                      \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD GOBLINS                           */
/* ==================================================================== */

#define BASE_GOBLIN(MLVL, GENDER) \
   race RACE_GOBLIN \
   level MLVL \
   sex GENDER \
   height   90       /* cm            */ \
   weight   90       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER)

#define BASE_GOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_GOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,15,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,13,0,0)

/* ==================================================================== */
/*                           STANDARD HALFLINGS                         */
/* ==================================================================== */

#define BASE_HALFLING(MLVL, GENDER) \
   race RACE_HALFLING \
   level MLVL \
   sex GENDER \
   height   95       /* cm            */ \
   weight   80       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_HALFLING_WARRIOR(MLVL, GENDER) \
   BASE_HALFLING(MLVL, GENDER)                  \
   MSET_ABILITY(12,23,20,17,13,15,0,0)       \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)         \

#define BASE_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HALFLING(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HALFLING_THIEF(MLVL, GENDER)                      \
   BASE_HALFLING(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(10,38,15,17,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD HOBGOBLINS                        */
/* ==================================================================== */

#define BASE_HOBGOBLIN(MLVL, GENDER) \
   race RACE_HOBGOBLIN \
   level MLVL \
   sex GENDER \
   height   130       /* cm            */ \
   weight   120       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER)

#define BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_HOBGOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(18,20,22,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 7, 8,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);


#define BASE_HOBGOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,12,0,0)


/* ==================================================================== */
/*                           STANDARD HUMANS                            */
/* ==================================================================== */

#define BASE_HUMAN(MLVL, GENDER) \
   race RACE_HUMAN \
   level MLVL \
   sex GENDER \
   height   170       /* cm            */ \
   weight   150       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_HUMAN_WARRIOR(MLVL, GENDER) \
   BASE_HUMAN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_HUMAN_THIEF(MLVL, GENDER)                      \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD KOBOLDS                           */
/* ==================================================================== */

#define BASE_KOBOLD(MLVL, GENDER) \
   race RACE_KOBOLD \
   level MLVL \
   sex GENDER \
   height   91       /* cm            */ \
   weight   87       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)

#define BASE_KOBOLD_WARRIOR(MLVL, GENDER) \
   BASE_KOBOLD(MLVL, GENDER)                  \
   MSET_ABILITY(16,18,15,19,14,18,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_KOBOLD_THIEF(MLVL, GENDER)                      \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,22,17,20,15,10,0,0)


/* ==================================================================== */
/*                           STANDARD LIZARDMAN                         */
/* ==================================================================== */

#define BASE_LIZARDMAN(MLVL, GENDER) \
   race RACE_LIZARD \
   level MLVL \
   sex GENDER \
   height   180       /* cm            */ \
   weight   160       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN)

#define BASE_LIZARDMAN_WARRIOR(MLVL, GENDER) \
   BASE_LIZARDMAN(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_LIZARDMAN_THIEF(MLVL, GENDER)                      \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* ==================================================================== */
/*                           STANDARD ORCS                              */
/* ==================================================================== */

#define BASE_ORC(MLVL, GENDER) \
   race RACE_ORC \
   level MLVL \
   sex GENDER \
   height   160       /* cm            */ \
   weight   160       /* Pounds        */ \
   NATURAL_DEF(WPN_FIST, ARM_CHAIN)  \
   MSET_ABILITY(25,15,25,25,5,5,0,0) \
   MSET_WEAPON(10,30,25,5,25,5)      \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)

#define BASE_ORC_WARRIOR(MLVL, GENDER) \
   BASE_ORC(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,25,25,5,5,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_ORC(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_ORC(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_ORC_THIEF(MLVL, GENDER)                      \
   BASE_ORC(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)



#endif
