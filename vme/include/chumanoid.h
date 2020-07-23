#ifndef _MUD_CHUMANOID_H
#define _MUD_CHUMANOID_H

#include <bottom.h>

/* ====================== CHILD HUMANOID RACES ======================== */

/* This is your standard Udgaard / child Midgaard shop keeper. */
#define C_SHOP_KEEPER(MLVL, GENDER, MRACE)\
   race MRACE \
   level MLVL \
   sex GENDER \
   romflags {CHAR_PROTECTED, CHAR_WIMPY} \
   SETWEIGHT(MLVL,80,160)\
   SETHEIGHT(MLVL,85,170)\
   NATURAL_DEF(WPN_FIST, ARM_LEATHER)     \
   exp -500   /* lets protect him a bit */ \
   alignment 355      /* Good          */ \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */ 


/* ==================================================================== */
/*                           STANDARD KINGRAITH                         */
/* ==================================================================== */

#define BASE_CHILD_KINGRAITH(MLVL, GENDER) \
   race RACE_KINGRAITH \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,180)/* pounds       */ \
   SETHEIGHT(MLVL,86,177)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_PLATE)  \
   flags {UNIT_FL_NO_TELEPORT} 


#define BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_KINGRAITH(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_KINGRAITH(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_KINGRAITH_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_KINGRAITH(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_KINGRAITH(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH(MLVL, GENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_KINGRAITH_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_KINGRAITH_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_KINGRAITH_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_KINGRAITH_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_KINGRAITH_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_KINGRAITH_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_KINGRAITH_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_KINGRAITH_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_KINGRAITH_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_KINGRAITH_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_KINGRAITH_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_KINGRAITH_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_KINGRAITH_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_KINGRAITH_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_KINGRAITH_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_KINGRAITH_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_KINGRAITH_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_KINGRAITH_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_KINGRAITH_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_KINGRAITH_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */




/* ==================================================================== */
/*                           STANDARD DWARFS                            */
/* ==================================================================== */

#define BASE_CHILD_DWARF(MLVL, GENDER) \
   race RACE_DWARF \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,18,170) \
   SETHEIGHT(MLVL,30,122) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_DWARF(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_DWARF_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_DWARF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_DWARF(MLVL, GENDER) \
   BASE_CHILD_DWARF(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_DWARF_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_DWARF_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_DWARF_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_DWARF_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_DWARF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_DWARF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_DWARF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_DWARF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_DWARF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_DWARF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_DWARF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DWARF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_DWARF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_DWARF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_DWARF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_DWARF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_DWARF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_DWARF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_DWARF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_DWARF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_DWARF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_DWARF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_DWARF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD ELFS                              */
/* ==================================================================== */

#define BASE_CHILD_ELF(MLVL, GENDER) \
   race RACE_ELF \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,68,93)/* pounds       */ \
   SETHEIGHT(MLVL,69,138)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_ELF_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,18,12,25,10,10,5,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_ELF_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_ELF(MLVL, GENDER) \
   BASE_CHILD_ELF(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

/* -------------------------------------------------------------------- */

#define C_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                         STANDARD DARK ELFS                           */
/* ==================================================================== */

#define BASE_CHILD_DARK_ELF(MLVL, GENDER) \
   race RACE_DARK_ELF \
   level MLVL \
   alignment -800 \
   sex GENDER \
   SETWEIGHT(MLVL,40,93)/* pounds       */ \
   SETHEIGHT(MLVL,70,145)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,30,20,15,15,0,0,0) \
   MSET_SPELL(4,4,4,4,4,4,5,5,6,5,5) \  /* 50%, resistances    */

#define BASE_CHILD_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_DARK_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_DARK_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_DARK_ELF_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_DARK_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_DARK_ELF(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_DARK_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_DARK_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_DARK_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_DARK_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_DARK_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_DARK_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_DARK_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_DARK_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_DARK_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_DARK_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_DARK_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_DARK_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_DARK_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_DARK_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

/* -------------------------------------------------------------------- */

#define C_DARK_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_DARK_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_DARK_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_DARK_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_DARK_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_DARK_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                         STANDARD HALF ELFS                           */
/* ==================================================================== */

#define BASE_CHILD_HALF_ELF(MLVL, GENDER) \
   race RACE_HALF_ELF \
   level MLVL \
   alignment 0 \
   sex GENDER \
   SETWEIGHT(MLVL,56,140) /* pounds       */ \
   SETHEIGHT(MLVL,111,111) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_SPELL(0,3,1,1,1,1,4,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_HALF_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_HALF_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_HALF_ELF_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_HALF_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_HALF_ELF(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_HALF_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALF_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALF_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_HALF_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_HALF_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_HALF_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_HALF_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_HALF_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_HALF_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_HALF_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_HALF_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_HALF_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_HALF_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_HALF_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

/* -------------------------------------------------------------------- */

#define C_HALF_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_HALF_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_HALF_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_HALF_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_HALF_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_HALF_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GNOMES                            */
/* ==================================================================== */

#define BASE_CHILD_GNOME(MLVL, GENDER) \
   race RACE_GNOME \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,35,71)/* pounds       */ \
   SETHEIGHT(MLVL,42,84)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_GNOME(MLVL, GENDER)                  \
   MSET_ABILITY(15,18,15,20,15,12,5,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_GNOME_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_GNOME(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)
/* ==================================================================== */

#define C_AVG_GNOME(MLVL, GENDER) \
   BASE_CHILD_GNOME(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_GNOME_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GNOME_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GNOME_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_GNOME_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_GNOME_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_GNOME_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_GNOME_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_GNOME_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_GNOME_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_GNOME_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_GNOME_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GNOME_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_GNOME_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_GNOME_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_GNOME_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_GNOME_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_GNOME_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_GNOME_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_GNOME_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_GNOME_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_GNOME_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_GNOME_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_GNOME_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GOBLINS                           */
/* ==================================================================== */

#define BASE_CHILD_GOBLIN(MLVL, GENDER) \
   race RACE_GOBLIN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,45,90)/* pounds       */ \
   SETHEIGHT(MLVL,45,90)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER) 


#define BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_GOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,15,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_GOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_GOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,12,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_GOBLIN(MLVL, GENDER) \
   BASE_CHILD_GOBLIN(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_GOBLIN_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GOBLIN_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GOBLIN_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_GOBLIN_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_GOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_GOBLIN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_GOBLIN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_GOBLIN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_GOBLIN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_GOBLIN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_GOBLIN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_GOBLIN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_GOBLIN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_GOBLIN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_GOBLIN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_GOBLIN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_GOBLIN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_GOBLIN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_GOBLIN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_GOBLIN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_GOBLIN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_GOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                          CHILD GROLLS                                */
/* ==================================================================== */
#define BASE_CHILD_GROLL(MLVL, GENDER) \
   race RACE_GROLL \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,106,212) \
   SETHEIGHT(MLVL,90,180) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_GROLL(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_GROLL(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_GROLL(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_GROLL_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_GROLL(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_GROLL(MLVL, GENDER) \
   BASE_CHILD_GROLL(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_GROLL_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GROLL_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_GROLL_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_GROLL_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_GROLL_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_GROLL_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_GROLL_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_GROLL_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_GROLL_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_GROLL_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_GROLL_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_GROLL_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_GROLL_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_GROLL_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_GROLL_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_GROLL_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_GROLL_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_GROLL_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_GROLL_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_GROLL_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HALF OGRE                         */
/* ==================================================================== */

#define BASE_CHILD_HALF_OGRE(MLVL, GENDER) \
   race RACE_HALF_OGRE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,104,208) /* pounds */ \
   SETHEIGHT(MLVL,80,160) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_HALF_OGRE_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_HALF_OGRE(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_HALF_OGRE_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALF_OGRE_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALF_OGRE_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_HALF_OGRE_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_HALF_OGRE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_HALF_OGRE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_HALF_OGRE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_HALF_OGRE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_HALF_OGRE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_HALF_OGRE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_HALF_OGRE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_HALF_OGRE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_HALF_OGRE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_HALF_OGRE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_HALF_OGRE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_HALF_OGRE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_HALF_OGRE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_HALF_OGRE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_HALF_OGRE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_HALF_OGRE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HALFLINGS                         */
/* ==================================================================== */

#define BASE_CHILD_HALFLING(MLVL, GENDER) \
   race RACE_HALFLING \
   SETWEIGHT(MLVL,28,65)/* pounds       */ \
   SETHEIGHT(MLVL,33,72)/* cm            */ \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_HALFLING(MLVL, GENDER)                  \
   MSET_ABILITY(12,23,20,17,13,15,0,0)       \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0)         \

#define BASE_CHILD_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_HALFLING(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_HALFLING(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);


#define BASE_CHILD_HALFLING_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_HALFLING(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(10,38,15,17,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_HALFLING(MLVL, GENDER) \
   BASE_CHILD_HALFLING(MLVL, GENDER)              \
   MSET_ABILITY(10,15,10,23,20,22,0,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */


#define C_HALFLING_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALFLING_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HALFLING_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_HALFLING_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_HALFLING_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/*-------------------------------------------------------------------*/
#define C_HALFLING_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_HALFLING_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_HALFLING_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_HALFLING_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_HALFLING_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_HALFLING_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* --------------------------------------------------------------------- */

#define C_HALFLING_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_HALFLING_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_HALFLING_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_HALFLING_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HALFLING_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_HALFLING_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(30,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_HALFLING_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,20,20,30,10,5)    /* 85% Pole-category */

#define C_HALFLING_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(30,20,20,15,10,5)     /* 85% Axe / Hammer */

#define C_HALFLING_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(20,15,30,15,15,5)     /* 85% Club/Mace/Flail    */

#define C_HALFLING_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(20,15,15,15,30,5)     /* 85% Club/Mace/Flail    */

#define C_HALFLING_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_HALFLING_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,20,15,15,5,30)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HOBGOBLINS                        */
/* ==================================================================== */

#define BASE_CHILD_HOBGOBLIN(MLVL, GENDER) \
   race RACE_HOBGOBLIN \
   SETWEIGHT(MLVL,74,145)/* pounds       */ \
   SETHEIGHT(MLVL,52,114)/* cm            */ \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER) 


#define BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(18,20,22,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 7, 8,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);


#define BASE_CHILD_HOBGOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,12,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_HOBGOBLIN(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_HOBGOBLIN_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HOBGOBLIN_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HOBGOBLIN_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_HOBGOBLIN_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_HOBGOBLIN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_HOBGOBLIN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_HOBGOBLIN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_HOBGOBLIN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_HOBGOBLIN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_HOBGOBLIN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_HOBGOBLIN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_HOBGOBLIN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_HOBGOBLIN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_HOBGOBLIN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_HOBGOBLIN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_HOBGOBLIN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_HOBGOBLIN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_HOBGOBLIN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_HOBGOBLIN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_HOBGOBLIN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_HOBGOBLIN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_HOBGOBLIN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */


/* ==================================================================== */
/*                           STANDARD HUMANS                            */
/* ==================================================================== */

#define BASE_CHILD_HUMAN(MLVL, GENDER) \
   race RACE_HUMAN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,180) /* pounds */ \
   SETHEIGHT(MLVL,60,120) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_HUMAN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_HUMAN_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_HUMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */



#define C_AVG_HUMAN(MLVL, GENDER) \
   BASE_CHILD_HUMAN(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_HUMAN_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HUMAN_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_HUMAN_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_HUMAN_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_HUMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_HUMAN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_HUMAN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_HUMAN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_HUMAN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_HUMAN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_HUMAN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_HUMAN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_HUMAN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_HUMAN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_HUMAN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_HUMAN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_HUMAN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_HUMAN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_HUMAN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_HUMAN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_HUMAN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_HUMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */





/* ==================================================================== */
/*                           STANDARD KOBOLDS                           */
/* ==================================================================== */

#define BASE_CHILD_KOBOLD(MLVL, GENDER) \
   race RACE_KOBOLD \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,38,75)/* pounds       */ \
   SETHEIGHT(MLVL,38,75)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) \
   exp 110 \
   dilcopy aggressive@function (0, -1, 0, -30, 2, PULSE_SEC*22, 0, "ANY", \
        {"$1n yodels savagely, attacking $3n!", \
         "$1n yodels savagely and charges toward you!"}); 



#define BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_KOBOLD(MLVL, GENDER)                  \
   MSET_ABILITY(16,18,15,19,14,18,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_KOBOLD_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_KOBOLD(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,22,17,20,15,10,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_KOBOLD(MLVL, GENDER) \
   BASE_CHILD_KOBOLD(MLVL, GENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_KOBOLD_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_KOBOLD_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_KOBOLD_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_KOBOLD_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_KOBOLD_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_KOBOLD_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_KOBOLD_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_KOBOLD_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_KOBOLD_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_KOBOLD_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_KOBOLD_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_KOBOLD_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_KOBOLD_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_KOBOLD_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_KOBOLD_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_KOBOLD_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_KOBOLD_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_KOBOLD_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_KOBOLD_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_KOBOLD_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD LIZARDMAN                         */
/* ==================================================================== */

#define BASE_CHILD_LIZARDMAN(MLVL, GENDER) \
   race RACE_DRACONIAN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,180)/* pounds       */ \
   SETHEIGHT(MLVL,86,177)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_PLATE)  \
   flags {UNIT_FL_NO_TELEPORT} 


#define BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_LIZARDMAN_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_LIZARDMAN(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN(MLVL, GENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_LIZARDMAN_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_LIZARDMAN_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_LIZARDMAN_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_LIZARDMAN_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_LIZARDMAN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_LIZARDMAN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_LIZARDMAN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_LIZARDMAN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_LIZARDMAN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_LIZARDMAN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_LIZARDMAN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_LIZARDMAN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_LIZARDMAN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_LIZARDMAN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_LIZARDMAN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_LIZARDMAN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_LIZARDMAN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_LIZARDMAN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_LIZARDMAN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_LIZARDMAN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_LIZARDMAN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_LIZARDMAN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD OGRES                            */
/* ==================================================================== */
#define BASE_CHILD_OGRE(MLVL, GENDER) \
   race RACE_OGRE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,117,235) \
   SETHEIGHT(MLVL,100,200) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_OGRE(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,20,00,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_OGRE_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_OGRE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_OGRE(MLVL, GENDER) \
   BASE_CHILD_OGRE(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_OGRE_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_OGRE_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_OGRE_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_OGRE_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_OGRE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_OGRE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_OGRE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_OGRE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_OGRE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_OGRE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_OGRE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_OGRE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_OGRE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_OGRE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_OGRE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_OGRE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_OGRE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_OGRE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_OGRE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_OGRE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */


/* ==================================================================== */
/*                           STANDARD ORCS                              */
/* ==================================================================== */

#define BASE_CHILD_ORC(MLVL, GENDER) \
   race RACE_ORC \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,65,129)/* pounds       */ \
   SETHEIGHT(MLVL,70,140)/* cm            */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER)  \
   MSET_ABILITY(25,15,25,25,5,5,0,0) \
   MSET_WEAPON(10,30,25,5,25,5)      \
   MSET_SPELL(0,0,0,0,0,0,0,0,0,0,0) 


#define BASE_CHILD_ORC_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_ORC(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,25,25,5,5,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_ORC(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_ORC(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_ORC_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_ORC(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_ORC(MLVL, GENDER) \
   BASE_CHILD_ORC(MLVL, GENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define C_ORC_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_ORC_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_ORC_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_ORC_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_ORC_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_ORC_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_ORC_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_ORC_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_ORC_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_ORC_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_ORC_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_ORC_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_ORC_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_ORC_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_ORC_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_ORC_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_ORC_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_ORC_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_ORC_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_ORC_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_ORC_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_ORC_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Natural      */

#define C_ORC_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD TROLLS                            */
/* ==================================================================== */

#define BASE_CHILD_TROLL(MLVL, GENDER) \
   race RACE_TROLL \
   level MLVL \
   sex GENDER \
   alignment -880 \
   SETWEIGHT(MLVL,95,190) \
   SETHEIGHT(MLVL,80,160) \
   NATURAL_DEF(WPN_FIST, ARM_HLEATHER) 


#define BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_TROLL(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_TROLL(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_TROLL(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_TROLL_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_TROLL(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_TROLL(MLVL, GENDER) \
   BASE_CHILD_TROLL(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_TROLL_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_TROLL_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_TROLL_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_TROLL_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_TROLL_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_TROLL_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_TROLL_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_TROLL_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_TROLL_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_TROLL_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_TROLL_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_TROLL_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_TROLL_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_TROLL_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_TROLL_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_TROLL_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_TROLL_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_TROLL_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_TROLL_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_TROLL_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD BROWNIES                          */
/* ==================================================================== */
#define BASE_CHILD_BROWNIE(MLVL, GENDER) \
   race RACE_BROWNIE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,25,65) \
   SETHEIGHT(MLVL,30,68) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_BROWNIE(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_BROWNIE(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_BROWNIE(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_BROWNIE_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_BROWNIE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_BROWNIE(MLVL, GENDER) \
   BASE_CHILD_BROWNIE(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_BROWNIE_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_BROWNIE_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_BROWNIE_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_BROWNIE_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_BROWNIE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_BROWNIE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_BROWNIE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_BROWNIE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_BROWNIE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_BROWNIE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_BROWNIE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_BROWNIE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_BROWNIE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_BROWNIE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_BROWNIE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_BROWNIE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_BROWNIE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_BROWNIE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_BROWNIE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_BROWNIE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */

/* ==================================================================== */
/*                           STANDARD SKAVENS                            */
/* ==================================================================== */

#define BASE_CHILD_SKAVEN(MLVL, GENDER) \
   race RACE_SKAVEN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,180) /* pounds       */ \
   SETHEIGHT(MLVL,52,105) /* cm            */ \
   alignment -600 \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   dilcopy bitedisease@skaven(); \
   exp 110 


#define BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_SKAVEN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_SKAVEN(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_SKAVEN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_SKAVEN_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_SKAVEN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_SKAVEN(MLVL, GENDER) \
   BASE_CHILD_SKAVEN(MLVL, GENDER)  \
   MSET_ABILITY(20,26,14,20,10,4,1,5) \
   MSET_WEAPON(10,10,10,10,10,10) \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6) 


/* -------------------------------------------------------------------- */

#define C_SKAVEN_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_SKAVEN_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_SKAVEN_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_SKAVEN_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_SKAVEN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_SKAVEN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_SKAVEN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_SKAVEN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_SKAVEN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_SKAVEN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_SKAVEN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_SKAVEN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_SKAVEN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_SKAVEN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_SKAVEN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_SKAVEN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_SKAVEN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_SKAVEN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_SKAVEN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_SKAVEN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                 STANDARD TANNARIS  half human half vulture           */
/* ==================================================================== */
#define BASE_CHILD_TANNARI(MLVL, GENDER) \
   race RACE_TANNARI \
   alignment -950 \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,60,120) \
   SETHEIGHT(MLVL,42,95) \
   NATURAL_DEF(WPN_CLAW, ARM_CLOTHES)

#define BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER) \
   BASE_CHILD_TANNARI(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_CHILD_TANNARI(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_CHILD_TANNARI(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_TANNARI_THIEF(MLVL, GENDER)                      \
   BASE_CHILD_TANNARI(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define C_AVG_TANNARI(MLVL, GENDER) \
   BASE_CHILD_TANNARI(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define C_TANNARI_THIEF_AXE(MLVL, GENDER) \
   BASE_CHILD_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_TANNARI_THIEF_SWORD(MLVL, GENDER) \
   BASE_CHILD_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define C_TANNARI_THIEF_CLUB(MLVL, GENDER) \
   BASE_CHILD_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define C_TANNARI_THIEF_POLE(MLVL, GENDER) \
   BASE_CHILD_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define C_TANNARI_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define C_TANNARI_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define C_TANNARI_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define C_TANNARI_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define C_TANNARI_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define C_TANNARI_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define C_TANNARI_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define C_TANNARI_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define C_TANNARI_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define C_TANNARI_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define C_TANNARI_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define C_TANNARI_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define C_TANNARI_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define C_TANNARI_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define C_TANNARI_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define C_TANNARI_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_CHILD_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */

/* ==================================================================== */
/*                           STANDARD GIANTS                            */
/* ==================================================================== */

#define BASE_CHILD_GIANT_MOB(MLVL, GENDER) \
   race RACE_GIANT_MOB \
   level MLVL \
   sex GENDER \
   alignment -880 \
   SETWEIGHT(MLVL,200,500) \
   SETHEIGHT(MLVL,100,300) \
   NATURAL_DEF(WPN_FIST, ARM_HLEATHER)   


#define BASE_CHILD_GIANT_HILL(MLVL, GENDER) \
   BASE_CHILD_GIANT_MOB(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_CHILD_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)/*limited clerical*/ \
   BASE_CHILD_GIANT_MOB(MLVL, GENDER)  \
   MSET_ABILITY(17,6,18,19,10,10,0,20) \
   MSET_SPELL(15,0,4,4,4,1,5,5,4,4,4)  /* 50,DIV */ \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL) /*Fire */ \
   BASE_CHILD_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \
   MSET_SPELL(2,2,5,5,5,5,15,0,5,5,6)  /* 55, Fire */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL) /*Frost */ \
   BASE_CHILD_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \                    \
   MSET_SPELL(2,2,5,5,5,5,0,15,5,5,6)  /* 55%, Cold */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) /*ELEC */ \
   BASE_CHILD_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \                    \
   MSET_SPELL(2,2,5,5,5,5,0,6,15,5,5)  /* 55%, elec */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);
   
/* -------------------------------------------------------------------- */
  /*Hill Giants slow and stupid */
#define C_AVG_GIANT_HILL(MLVL, GENDER) \
   BASE_CHILD_GIANT_HILL(MLVL, GENDER)              \
   MSET_ABILITY(20,10,25,25,5,10,5,0) /*not too smart*/  \
   MSET_WEAPON(15,5,15,5,15,5)    /* 60 bashes stuff mainly */ \
   MSET_SPELL(4,2,2,2,2,2,6,6,2,6,6)  /* 40 Resistance low to non elemental*/

#define C_GIANT_HILL_AXE(MLVL, GENDER) \
   BASE_CHILD_GIANT_HILL(MLVL, GENDER)          \
   MSET_WEAPON(30,5,20,5,20,5)    /* 85% use a sword on it! */

#define C_GIANT_HILL_CLUB(MLVL, GENDER) \
   BASE_CHILD_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(15,5,30,20,10,5)    /* 85%, Club/Mace/Flail/Mr Star */

#define C_GIANT_HILL_POLE(MLVL, GENDER) \
   BASE_CHILD_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(20,5,20,25,10,5)    /* 85%, Polearms */

#define C_GIANT_HILL_NATURAL(MLVL, GENDER) \
   BASE_CHILD_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Punch/Kick/Crush  */   

/* -------------------------------------------------------------------- */
     /*Fire Giants not so slow and can throw spells */
#define C_AVG_GIANT_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(15,10,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,0,10,5,15,5)    /* 50 bashes stuff mainly */ 	 
	  
#define C_GIANT_FIRE_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define C_GIANT_FIRE_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define C_GIANT_FIRE_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define C_GIANT_FIRE_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define C_GIANT_FIRE_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   
/*-------------------------------------------------------------------*/
     /*Frost Giants not so slow and can throw spells */
	
#define C_AVG_GIANT_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(10,15,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,0,10,5,15,5)    /* 50 bashes stuff mainly */ 
	 
	  
#define C_GIANT_FROST_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define C_GIANT_FROST_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define C_GIANT_FROST_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define C_GIANT_FROST_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)       \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define C_GIANT_FROST_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_FROST(MLVL, GENDER, ATT_SPL)       \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   

/*-------------------------------------------------------------------*/
     /*Cloud Giants not so slow and can throw spells */
	
#define C_AVG_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(10,15,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,0,10,5,15,5)    /* 50 bashes stuff mainly */ 

	  
#define C_GIANT_CLOUD_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define C_GIANT_CLOUD_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define C_GIANT_CLOUD_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define C_GIANT_CLOUD_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define C_GIANT_CLOUD_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_CHILD_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) )       \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   

/* -------------------------------------------------------------------- */

#define C_GIANT_ETTIN_AXE(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_CHILD_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(20,0,15,0,15,0)           /* 30%, 11% hammer user   */

#define C_GIANT_ETTIN_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_CHILD_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(10,0,20,5,15,0)            /* 30%, 11% mace or club user     */

#define C_GIANT_ETTIN_NATURAL(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(15,0,15,0,20,0)             /* 30%, 11% staff user    */

#define C_GIANT_ETTIN_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_CHILD_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                   \
   MSET_WEAPON(10,20,15,0,5,0)           /*    */

#endif
