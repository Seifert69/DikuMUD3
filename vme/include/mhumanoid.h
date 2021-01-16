#ifndef _MUD_MHUMANOID_H
#define _MUD_MHUMANOID_H

#include "bottom.h"


/* ==================== STANDARD HUMANOID RACES ======================= */
/* This is your standard Udgaard / Midgaard shop keeper. */
#define M_SHOP_KEEPER(MLVL, GENDER, MRACE) \
   race MRACE \
   level MLVL \
   sex GENDER \
   romflags {CHAR_PROTECTED, CHAR_WIMPY} \
   SETWEIGHT(MLVL,110,500) \
   SETHEIGHT(MLVL,135,222) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)     \
   exp -400   /* lets protect him a bit */ \
   alignment 355      /* Good          */ \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */    


/* ==================================================================== */
/*                           STANDARD KINGRAITH                         */
/* ==================================================================== */

#define BASE_KINGRAITH(MLVL, GENDER) \
   race RACE_KINGRAITH \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,180,500)/* pounds       */ \
   SETHEIGHT(MLVL,177,300)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER) \
   flags {UNIT_FL_NO_TELEPORT}   



#define BASE_KINGRAITH_WARRIOR(MLVL, GENDER) \
   BASE_KINGRAITH(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */ \

#define BASE_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_KINGRAITH(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_KINGRAITH(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_KINGRAITH_THIEF(MLVL, GENDER)                      \
   BASE_KINGRAITH(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_KINGRAITH(MLVL, GENDER) \
   BASE_KINGRAITH(MLVL, GENDER)              \
   MSET_ABILITY(20,10,13,20,10,17,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_KINGRAITH_THIEF_AXE(MLVL, GENDER) \
   BASE_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_KINGRAITH_THIEF_SWORD(MLVL, GENDER) \
   BASE_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_KINGRAITH_THIEF_CLUB(MLVL, GENDER) \
   BASE_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_KINGRAITH_THIEF_POLE(MLVL, GENDER) \
   BASE_KINGRAITH_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_KINGRAITH_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_KINGRAITH_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_KINGRAITH_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_KINGRAITH_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_KINGRAITH_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_KINGRAITH_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_KINGRAITH_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_KINGRAITH_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_KINGRAITH_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_KINGRAITH_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_KINGRAITH_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_KINGRAITH_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_KINGRAITH_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_KINGRAITH_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_KINGRAITH_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_KINGRAITH_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_KINGRAITH_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_KINGRAITH_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_KINGRAITH_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */


/* ==================================================================== */
/*                           STANDARD DWARFS                            */
/* ==================================================================== */
#define BASE_DWARF(MLVL, GENDER) \
   race RACE_DWARF \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,155,400) \
   SETHEIGHT(MLVL,102,225) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_DWARF_WARRIOR(MLVL, GENDER) \
   BASE_DWARF(MLVL, GENDER)                  \
   MSET_ABILITY(25,25,25,25,0,0,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_DWARF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY(19,16,10,10,15,10,0,20)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_DWARF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_ABILITY( 13, 12,10,10,18,17,20, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_DWARF_THIEF(MLVL, GENDER)                      \
   BASE_DWARF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_DWARF(MLVL, GENDER) \
   BASE_DWARF(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

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

#define BASE_ELF(MLVL, GENDER) \
race RACE_ELF \
level MLVL \
sex GENDER \
SETWEIGHT(MLVL,135,187) /* pounds       */ \
SETHEIGHT(MLVL,138,174) /* cm            */ \
NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   



#define BASE_ELF_WARRIOR(MLVL, GENDER) \
   BASE_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,20,0,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(17,18,10,10,15,10,0,20)                     \
   MSET_SPELL(7,7,7,7,5,7,7,5,5,7,6)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 10,10,10,10,20,20,20, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_ELF_THIEF(MLVL, GENDER)                      \
   BASE_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_ELF(MLVL, GENDER) \
   BASE_ELF(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

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
/*                         STANDARD DARK ELFS                           */
/* ==================================================================== */

#define BASE_DARK_ELF(MLVL, GENDER) \
   race RACE_DARK_ELF \
   level MLVL \
   alignment -800 \
   sex GENDER \
   SETWEIGHT(MLVL,93,209)/* pounds       */ \
   SETHEIGHT(MLVL,145,170)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_DARK_ELF_WARRIOR(MLVL, GENDER) \
   BASE_DARK_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,30,20,15,15,0,0,0) \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */

#define BASE_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_DARK_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_DARK_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_DARK_ELF_THIEF(MLVL, GENDER)                      \
   BASE_DARK_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_DARK_ELF(MLVL, GENDER) \
   BASE_DARK_ELF(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_DARK_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_DARK_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_DARK_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_DARK_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_DARK_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_DARK_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_DARK_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_DARK_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_DARK_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_DARK_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_DARK_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_DARK_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_DARK_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_DARK_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_DARK_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_DARK_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_DARK_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

/* -------------------------------------------------------------------- */

#define M_DARK_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_DARK_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_DARK_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_DARK_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_DARK_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_DARK_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_DARK_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                         STANDARD HALF ELFS                           */
/* ==================================================================== */

#define BASE_HALF_ELF(MLVL, GENDER) \
   race RACE_HALF_ELF \
   level MLVL \
   alignment 0 \
   sex GENDER \
   SETWEIGHT(MLVL,140,250) /* pounds       */ \
   SETHEIGHT(MLVL,111,171) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_HALF_ELF_WARRIOR(MLVL, GENDER) \
   BASE_HALF_ELF(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,20,0,0,0) \
   MSET_SPELL(0,3,1,1,1,1,4,1,1,1,1)  /* 15%, resistances    */

#define BASE_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HALF_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HALF_ELF(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_HALF_ELF_THIEF(MLVL, GENDER)                      \
   BASE_HALF_ELF(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_HALF_ELF(MLVL, GENDER) \
   BASE_HALF_ELF(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_HALF_ELF_THIEF_AXE(MLVL, GENDER) \
   BASE_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALF_ELF_THIEF_SWORD(MLVL, GENDER) \
   BASE_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALF_ELF_THIEF_CLUB(MLVL, GENDER) \
   BASE_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_HALF_ELF_THIEF_POLE(MLVL, GENDER) \
   BASE_HALF_ELF_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_HALF_ELF_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_HALF_ELF_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_HALF_ELF_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_HALF_ELF_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_HALF_ELF_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_HALF_ELF_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_ELF_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_HALF_ELF_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_HALF_ELF_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_HALF_ELF_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_HALF_ELF_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALF_ELF_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% sword user    */

/* -------------------------------------------------------------------- */

#define M_HALF_ELF_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_HALF_ELF_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_HALF_ELF_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_HALF_ELF_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_HALF_ELF_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_HALF_ELF_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_HALF_ELF_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HALF OGRE                         */
/* ==================================================================== */

#define BASE_HALF_OGRE(MLVL, GENDER) \
   race RACE_HALF_OGRE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,208,448) /* pounds */ \
   SETHEIGHT(MLVL,160,240) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_HALF_OGRE_WARRIOR(MLVL, GENDER) \
   BASE_HALF_OGRE(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_HALF_OGRE_THIEF(MLVL, GENDER)                      \
   BASE_HALF_OGRE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_HALF_OGRE(MLVL, GENDER) \
   BASE_HALF_OGRE(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_HALF_OGRE_THIEF_AXE(MLVL, GENDER) \
   BASE_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALF_OGRE_THIEF_SWORD(MLVL, GENDER) \
   BASE_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_HALF_OGRE_THIEF_CLUB(MLVL, GENDER) \
   BASE_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_HALF_OGRE_THIEF_POLE(MLVL, GENDER) \
   BASE_HALF_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_HALF_OGRE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_HALF_OGRE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_HALF_OGRE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_HALF_OGRE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_HALF_OGRE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_HALF_OGRE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_HALF_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_HALF_OGRE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_HALF_OGRE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_HALF_OGRE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_HALF_OGRE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_HALF_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_HALF_OGRE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_HALF_OGRE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_HALF_OGRE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_HALF_OGRE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_HALF_OGRE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_HALF_OGRE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_HALF_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GNOMES                            */
/* ==================================================================== */

#define BASE_GNOME(MLVL, GENDER) \
   race RACE_GNOME \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,71,187)/* pounds       */ \
   SETHEIGHT(MLVL,84,104)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_GNOME_WARRIOR(MLVL, GENDER) \
   BASE_GNOME(MLVL, GENDER)                  \
   MSET_ABILITY(15,18,15,20,15,12,5,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GNOME_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY(12,13,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_GNOME_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GNOME_THIEF(MLVL, GENDER)                      \
   BASE_GNOME(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)
/* ==================================================================== */

#define M_AVG_GNOME(MLVL, GENDER) \
   BASE_GNOME(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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
/*                           STANDARD GOBLINS                           */
/* ==================================================================== */

#define BASE_GOBLIN(MLVL, GENDER) \
   race RACE_GOBLIN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,90,180)/* pounds       */ \
   SETHEIGHT(MLVL,90,180)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER)   


#define BASE_GOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_GOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,15,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_GOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_GOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,12,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_GOBLIN(MLVL, GENDER) \
   BASE_GOBLIN(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

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
/*                           STANDARD GROLLS                            */
/* ==================================================================== */
#define BASE_GROLL(MLVL, GENDER) \
   race RACE_GROLL \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,213,493) \
   SETHEIGHT(MLVL,180,250) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_GROLL_WARRIOR(MLVL, GENDER) \
   BASE_GROLL(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_GROLL(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_GROLL(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GROLL_THIEF(MLVL, GENDER)                      \
   BASE_GROLL(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_GROLL(MLVL, GENDER) \
   BASE_GROLL(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_GROLL_THIEF_AXE(MLVL, GENDER) \
   BASE_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GROLL_THIEF_SWORD(MLVL, GENDER) \
   BASE_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_GROLL_THIEF_CLUB(MLVL, GENDER) \
   BASE_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_GROLL_THIEF_POLE(MLVL, GENDER) \
   BASE_GROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_GROLL_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_GROLL_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_GROLL_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_GROLL_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_GROLL_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_GROLL_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_GROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_GROLL_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_GROLL_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_GROLL_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_GROLL_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_GROLL_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_GROLL_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_GROLL_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_GROLL_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_GROLL_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_GROLL_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_GROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD HALFLINGS                         */
/* ==================================================================== */

#define BASE_HALFLING(MLVL, GENDER) \
   race RACE_HALFLING \
   SETWEIGHT(MLVL,65,137)/* pounds       */ \
   SETHEIGHT(MLVL,72,106)/* cm            */ \
   level MLVL \
   sex GENDER \
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

#define BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)\
   BASE_HALFLING(MLVL, GENDER)                     \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)          \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_HALFLING_THIEF(MLVL, GENDER)                      \
   BASE_HALFLING(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(10,38,15,17,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_HALFLING(MLVL, GENDER) \
   BASE_HALFLING(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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
#define M_HALFLING_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_HALFLING_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_HALFLING_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_HALFLING_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_HALFLING_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_HALFLING_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_HALFLING_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* --------------------------------------------------------------------- */

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
/*                           STANDARD HOBGOBLINS                        */
/* ==================================================================== */

#define BASE_HOBGOBLIN(MLVL, GENDER) \
   race RACE_HOBGOBLIN \
   SETWEIGHT(MLVL,145,207)/* pounds       */ \
   SETHEIGHT(MLVL,114,150)/* cm            */ \
   level MLVL \
   sex GENDER \
   NATURAL_DEF(WPN_FIST, ARM_LEATHER)   


#define BASE_HOBGOBLIN_WARRIOR(MLVL, GENDER) \
   BASE_HOBGOBLIN(MLVL, GENDER)                  \
   MSET_ABILITY(18,20,22,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_HOBGOBLIN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HOBGOBLIN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_ABILITY( 7, 8,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);


#define BASE_HOBGOBLIN_THIEF(MLVL, GENDER)                      \
   BASE_HOBGOBLIN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,18,20,20,15,12,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_HOBGOBLIN(MLVL, GENDER) \
   BASE_HOBGOBLIN(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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

#define BASE_HUMAN(MLVL, GENDER) \
   race RACE_HUMAN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,180,350) /* pounds */ \
   SETHEIGHT(MLVL,120,220) /* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_HUMAN_WARRIOR(MLVL, GENDER) \
   BASE_HUMAN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_HUMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_HUMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_HUMAN_THIEF(MLVL, GENDER)                      \
   BASE_HUMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_HUMAN(MLVL, GENDER) \
   BASE_HUMAN(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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
/*                           STANDARD KOBOLDS                           */
/* ==================================================================== */

#define BASE_KOBOLD(MLVL, GENDER) \
   race RACE_KOBOLD \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,75,124)/* pounds       */ \
   SETHEIGHT(MLVL,78,118)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) \
   exp 110 \
   dilcopy aggressive@function (0, -1, 0, -30, 2, PULSE_SEC*22, 0, "ANY", \
        {"$1n yodels savagely, attacking $3n!", \
         "$1n yodels savagely and charges toward you!"}); 



#define BASE_KOBOLD_WARRIOR(MLVL, GENDER) \
   BASE_KOBOLD(MLVL, GENDER)                  \
   MSET_ABILITY(16,18,15,19,14,18,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_KOBOLD_THIEF(MLVL, GENDER)                      \
   BASE_KOBOLD(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,22,17,20,15,10,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_KOBOLD(MLVL, GENDER) \
   BASE_KOBOLD(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

/* -------------------------------------------------------------------- */

#define M_KOBOLD_THIEF_AXE(MLVL, GENDER) \
   BASE_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_KOBOLD_THIEF_SWORD(MLVL, GENDER) \
   BASE_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_KOBOLD_THIEF_CLUB(MLVL, GENDER) \
   BASE_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_KOBOLD_THIEF_POLE(MLVL, GENDER) \
   BASE_KOBOLD_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_KOBOLD_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_KOBOLD_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_KOBOLD_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_KOBOLD_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_KOBOLD_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_KOBOLD_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_KOBOLD_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_KOBOLD_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_KOBOLD_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_KOBOLD_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_KOBOLD_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_KOBOLD_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_KOBOLD_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_KOBOLD_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_KOBOLD_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_KOBOLD_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_KOBOLD_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_KOBOLD_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_KOBOLD_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD LIZARDMAN                         */
/* ==================================================================== */

#define BASE_LIZARDMAN(MLVL, GENDER) \
   race RACE_DRACONIAN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,180,500)/* pounds       */ \
   SETHEIGHT(MLVL,177,300)/* cm            */ \
   NATURAL_DEF(WPN_FIST, ARM_PLATE) \
   flags {UNIT_FL_NO_TELEPORT}

#define BASE_LIZARDMAN_WARRIOR(MLVL, GENDER) \
   BASE_LIZARDMAN(MLVL, GENDER)                  \
   MSET_ABILITY(25,15,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_LIZARDMAN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY(15,10,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_ABILITY( 15,10,10,10,18,17,20, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_LIZARDMAN_THIEF(MLVL, GENDER)                      \
   BASE_LIZARDMAN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_LIZARDMAN(MLVL, GENDER) \
   BASE_LIZARDMAN(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
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

#define M_LIZARDMAN_MAGE_SUM(MLVL, GENDER, ATT_SPL) \
   BASE_LIZARDMAN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,35,7,7,7,5,5,5,5)   /* 90%, 35% summoning user  */

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
/*                           STANDARD OGRES                            */
/* ==================================================================== */
#define BASE_OGRE(MLVL, GENDER) \
   race RACE_OGRE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,235,545) \
   SETHEIGHT(MLVL,200,260) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES)   


#define BASE_OGRE_WARRIOR(MLVL, GENDER) \
   BASE_OGRE(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,20,0,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_OGRE(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_OGRE_THIEF(MLVL, GENDER)                      \
   BASE_OGRE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_OGRE(MLVL, GENDER) \
   BASE_OGRE(MLVL, GENDER)              \
   MSET_ABILITY(18,18,14,20,15,5,5,5)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_OGRE_THIEF_AXE(MLVL, GENDER) \
   BASE_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_OGRE_THIEF_SWORD(MLVL, GENDER) \
   BASE_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_OGRE_THIEF_CLUB(MLVL, GENDER) \
   BASE_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_OGRE_THIEF_POLE(MLVL, GENDER) \
   BASE_OGRE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_OGRE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_OGRE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_OGRE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_OGRE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_OGRE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_OGRE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_OGRE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_OGRE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_OGRE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_OGRE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_OGRE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_OGRE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_OGRE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_OGRE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_OGRE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_OGRE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_OGRE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_OGRE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_OGRE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD ORCS                              */
/* ==================================================================== */

#define BASE_ORC(MLVL, GENDER) \
   race RACE_ORC \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,129,189)/* pounds       */ \
   SETHEIGHT(MLVL,140,175)/* cm            */ \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER)  \
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
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_ORC_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_ORC(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_ORC_THIEF(MLVL, GENDER)                      \
   BASE_ORC(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(25,20,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_ORC(MLVL, GENDER) \
   BASE_ORC(MLVL, GENDER)              \
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
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Natural      */

#define M_ORC_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_ORC_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD TROLLS                            */
/* ==================================================================== */

#define BASE_TROLL(MLVL, GENDER) \
   race RACE_TROLL \
   level MLVL \
   sex GENDER \
   alignment -880 \
   SETWEIGHT(MLVL,190,440) \
   SETHEIGHT(MLVL,160,240) \
   NATURAL_DEF(WPN_FIST, ARM_HLEATHER)   


#define BASE_TROLL_WARRIOR(MLVL, GENDER) \
   BASE_TROLL(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_TROLL(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_TROLL(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_TROLL_THIEF(MLVL, GENDER)                      \
   BASE_TROLL(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_TROLL(MLVL, GENDER) \
   BASE_TROLL(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_TROLL_THIEF_AXE(MLVL, GENDER) \
   BASE_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_TROLL_THIEF_SWORD(MLVL, GENDER) \
   BASE_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_TROLL_THIEF_CLUB(MLVL, GENDER) \
   BASE_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_TROLL_THIEF_POLE(MLVL, GENDER) \
   BASE_TROLL_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_TROLL_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_TROLL_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_TROLL_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_TROLL_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_TROLL_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_TROLL_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_TROLL_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_TROLL_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_TROLL_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_TROLL_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_TROLL_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_TROLL_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_TROLL_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_TROLL_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_TROLL_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_TROLL_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_TROLL_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_TROLL_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_TROLL_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD BROWNIES                          */
/* ==================================================================== */
#define BASE_BROWNIE(MLVL, GENDER) \
   race RACE_BROWNIE \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,50,161) \
   SETHEIGHT(MLVL,60,98) \
   NATURAL_DEF(WPN_FIST, ARM_CLOTHES) 


#define BASE_BROWNIE_WARRIOR(MLVL, GENDER) \
   BASE_BROWNIE(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_BROWNIE(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_BROWNIE(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_BROWNIE_THIEF(MLVL, GENDER)                      \
   BASE_BROWNIE(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_BROWNIE(MLVL, GENDER) \
   BASE_BROWNIE(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_BROWNIE_THIEF_AXE(MLVL, GENDER) \
   BASE_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_BROWNIE_THIEF_SWORD(MLVL, GENDER) \
   BASE_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_BROWNIE_THIEF_CLUB(MLVL, GENDER) \
   BASE_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_BROWNIE_THIEF_POLE(MLVL, GENDER) \
   BASE_BROWNIE_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_BROWNIE_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_BROWNIE_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_BROWNIE_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_BROWNIE_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_BROWNIE_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_BROWNIE_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_BROWNIE_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_BROWNIE_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_BROWNIE_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_BROWNIE_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_BROWNIE_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_BROWNIE_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_BROWNIE_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_BROWNIE_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_BROWNIE_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_BROWNIE_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_BROWNIE_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_BROWNIE_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_BROWNIE_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD SKAVENS                            */
/* ==================================================================== */

#define BASE_SKAVEN(MLVL, GENDER) \
   race RACE_SKAVEN \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,180,375) /* pounds       */ \
   SETHEIGHT(MLVL,105,220) /* cm            */ \
   alignment -600 \
   NATURAL_DEF(WPN_CLAW, ARM_HLEATHER) \
   dilcopy bitedisease@skaven(); \
   exp 110   


#define BASE_SKAVEN_WARRIOR(MLVL, GENDER) \
   BASE_SKAVEN(MLVL, GENDER)                  \
   MSET_ABILITY(15,20,20,20,10,15,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_SKAVEN(MLVL, GENDER)                                 \
   MSET_ABILITY(11,14,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_SKAVEN(MLVL, GENDER)                                 \
   MSET_ABILITY( 5,10,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_SKAVEN_THIEF(MLVL, GENDER)                      \
   BASE_SKAVEN(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,30,15,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_SKAVEN(MLVL, GENDER) \
   BASE_SKAVEN(MLVL, GENDER)  \
   MSET_ABILITY(20,26,14,20,10,4,1,5) \
   MSET_WEAPON(10,10,10,10,10,10) \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)

/* -------------------------------------------------------------------- */

#define M_SKAVEN_THIEF_AXE(MLVL, GENDER) \
   BASE_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_SKAVEN_THIEF_SWORD(MLVL, GENDER) \
   BASE_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_SKAVEN_THIEF_CLUB(MLVL, GENDER) \
   BASE_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_SKAVEN_THIEF_POLE(MLVL, GENDER) \
   BASE_SKAVEN_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_SKAVEN_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_SKAVEN_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_SKAVEN_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_SKAVEN_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_SKAVEN_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_SKAVEN_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_SKAVEN_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_SKAVEN_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_SKAVEN_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_SKAVEN_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_SKAVEN_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_SKAVEN_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_SKAVEN_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_SKAVEN_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_SKAVEN_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_SKAVEN_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_SKAVEN_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_SKAVEN_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_SKAVEN_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                 STANDARD TANNARIS  half human half vulture           */
/* ==================================================================== */
#define BASE_TANNARI(MLVL, GENDER) \
   race RACE_TANNARI \
   level MLVL \
   sex GENDER \
   SETWEIGHT(MLVL,120,350) \
   SETHEIGHT(MLVL,90,185) \
   NATURAL_DEF(WPN_CLAW, ARM_CLOTHES)   


#define BASE_TANNARI_WARRIOR(MLVL, GENDER) \
   BASE_TANNARI(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)   \
   BASE_TANNARI(MLVL, GENDER)                                 \
   MSET_ABILITY(14,11,10,10,15,10,0,30)                     \
   MSET_SPELL(25,0,5,5,5,5,5,5,5,5,5)  /* 70%, 25% DIV */   \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)              \
   BASE_TANNARI(MLVL, GENDER)                                 \
   MSET_ABILITY( 10, 5,10,10,23,12,30, 0)                    \
   MSET_WEAPON(1,4,1,4,0,0)                /* 12%  */       \
   exp 115 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_TANNARI_THIEF(MLVL, GENDER)                      \
   BASE_TANNARI(MLVL, GENDER)                                 \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1) /* 15% resistances */  \
   MSET_ABILITY(15,25,20,20,15,5,0,0)

/* -------------------------------------------------------------------- */

#define M_AVG_TANNARI(MLVL, GENDER) \
   BASE_TANNARI(MLVL, GENDER)              \
   MSET_ABILITY(10,10,10,23,15,22,10,0)   \
   MSET_WEAPON(10,10,10,10,10,10)    /*  Average in everything, any wpn */ \
   MSET_SPELL(4,2,2,2,2,2,2,6,6,6,6)  /* Resistances       */

#define M_TANNARI_THIEF_AXE(MLVL, GENDER) \
   BASE_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(25,15,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_TANNARI_THIEF_SWORD(MLVL, GENDER) \
   BASE_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,25,15,10,10,10)    /* 85%, 25% Swords/Piercing */

#define M_TANNARI_THIEF_CLUB(MLVL, GENDER) \
   BASE_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,25,15,10,10)    /* 80%, 25% Club/Mace/Flail/Mr Star */

#define M_TANNARI_THIEF_POLE(MLVL, GENDER) \
   BASE_TANNARI_THIEF(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Polearms */

/* -------------------------------------------------------------------- */

#define M_TANNARI_MAGE_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,35,5,5,5,5)   /* 90%, 35% fire user     */

#define M_TANNARI_MAGE_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,35,5,5,5)   /* 90%, 35% frost user    */

#define M_TANNARI_MAGE_ACID(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,5,5,35)   /* 90%, 35% acid user     */

#define M_TANNARI_MAGE_ELECTRICITY(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,7,5,5,35,5,5)   /* 90%, 35% electricity   */

#define M_TANNARI_MAGE_MIND(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,35,7,5,5,5,5)   /* 90%, 35% mind          */

#define M_TANNARI_MAGE_GAS(MLVL, GENDER, ATT_SPL) \
   BASE_TANNARI_MAGE(MLVL, GENDER, ATT_SPL)        \
   MSET_SPELL(0,7,7,7,7,5,7,5,5,35,5)   /* 90%, 35% internal      */

/* -------------------------------------------------------------------- */

#define M_TANNARI_CLERIC_HAMMER(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(11,2,7,5,3,2)           /* 30%, 11% hammer user   */

#define M_TANNARI_CLERIC_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(5,2,11,7,3,2)           /* 30%, 11% mace or club user     */

#define M_TANNARI_CLERIC_STAFF(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,2,5,11,3,2)           /* 30%, 11% staff user    */

#define M_TANNARI_CLERIC_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_TANNARI_CLERIC(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(7,11,5,2,3,2)           /* 30%, 11% staff user    */

/* -------------------------------------------------------------------- */

#define M_TANNARI_WARRIOR_SWORD(MLVL, GENDER)  \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,25,15,15,10,5)     /* 85%/25 Sword-category */

#define M_TANNARI_WARRIOR_POLE(MLVL, GENDER)   \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,25,10,5)    /* 85% Pole-category */

#define M_TANNARI_WARRIOR_AXE(MLVL, GENDER)    \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(25,15,15,15,10,5)     /* 85% Axe / Hammer */

#define M_TANNARI_WARRIOR_CLUB(MLVL, GENDER)   \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,25,10,15,5)     /* 85% Club/Mace/Flail    */

#define M_TANNARI_WARRIOR_NATURAL(MLVL, GENDER)   \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,25,5)     /* 85% Club/Mace/Flail    */

#define M_TANNARI_WARRIOR_SPECIAL(MLVL, GENDER)   \
   BASE_TANNARI_WARRIOR(MLVL, GENDER)          \
   MSET_WEAPON(15,15,15,10,5,25)     /* 85% Special...    */

/* ==================================================================== */
/*                           STANDARD GIANTS                            */
/* ==================================================================== */

#define BASE_GIANT_MOB(MLVL, GENDER) \
   race RACE_GIANT_MOB \
   level MLVL \
   sex GENDER \
   alignment -880 \
   SETWEIGHT(MLVL,500,600) \
   SETHEIGHT(MLVL,300,500) \
   NATURAL_DEF(WPN_FIST, ARM_HLEATHER)   


#define BASE_GIANT_HILL(MLVL, GENDER) \
   BASE_GIANT_MOB(MLVL, GENDER)                  \
   MSET_ABILITY(20,20,20,20,10,10,0,0)       \
   MSET_SPELL(0,0,0,5,0,5,1,1,1,1,1)  /* 15%, resistances    */

#define BASE_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)/*limited clerical*/ \
   BASE_GIANT_MOB(MLVL, GENDER)  \
   MSET_ABILITY(17,6,18,19,10,10,0,20) \
   MSET_SPELL(15,0,4,4,4,1,5,5,4,4,4)  /* 50,DIV */ \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, DEF_SPL, 25, 2);

#define BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL) /*Fire */ \
   BASE_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \
   MSET_SPELL(2,2,5,5,5,5,15,0,5,5,6)  /* 55, Fire */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL) /*Frost */ \
   BASE_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \
   MSET_SPELL(2,2,5,5,5,5,0,15,5,5,6)  /* 55,frost */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);

#define BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) /*ELEC */ \
   BASE_GIANT_MOB(MLVL, GENDER)                                 \
   MSET_ABILITY(20,5,10,15,15,15,20,0) \
   MSET_SPELL(2,2,5,5,5,5,0,6,15,5,5)  /* 55%, elec */   \
   exp 130 \
   dilcopy combat_mag@function(ATT_SPL, "", 25, 2);
   
/* -------------------------------------------------------------------- */
  /*Hill Giants slow and stupid */
#define M_AVG_GIANT_HILL(MLVL, GENDER) \
   BASE_GIANT_HILL(MLVL, GENDER)              \
   MSET_ABILITY(25,5,25,25,5,10,5,0) /*not too smart*/  \
   MSET_WEAPON(15,5,15,5,15,5)    /* 60 bashes stuff mainly */ \
   MSET_SPELL(4,2,2,2,2,2,6,6,2,6,6)  /* 40 Resistance low to non elemental*/

#define M_GIANT_HILL_AXE(MLVL, GENDER) \
   BASE_GIANT_HILL(MLVL, GENDER)          \
   MSET_WEAPON(30,5,20,5,20,5)    /* 85% use a sword on it! */

#define M_GIANT_HILL_CLUB(MLVL, GENDER) \
   BASE_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(15,5,30,20,10,5)    /* 85%, Club/Mace/Flail/Mr Star */

#define M_GIANT_HILL_POLE(MLVL, GENDER) \
   BASE_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(20,5,20,25,10,5)    /* 85%, Polearms */

#define M_GIANT_HILL_NATURAL(MLVL, GENDER) \
   BASE_GIANT_HILL(MLVL, GENDER)        \
   MSET_WEAPON(15,10,15,25,10,10)    /* 80%, 25% Punch/Kick/Crush  */   

/* -------------------------------------------------------------------- */
     /*Fire Giants not so slow and can throw spells */
#define M_AVG_GIANT_FIRE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(15,5,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,5,10,5,15,5)    /* 50 bashes stuff mainly */ 
 
	  
#define M_GIANT_FIRE_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define M_GIANT_FIRE_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define M_GIANT_FIRE_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define M_GIANT_FIRE_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define M_GIANT_FIRE_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FIRE(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   
/*-------------------------------------------------------------------*/
     /*Frost Giants not so slow and can throw spells */
	
#define M_AVG_GIANT_FROST(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(15,10,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,5,10,0,15,5)    /* 45 bashes stuff mainly */ 
	 
	  
#define M_GIANT_FROST_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define M_GIANT_FROST_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define M_GIANT_FROST_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define M_GIANT_FROST_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)       \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define M_GIANT_FROST_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_FROST(MLVL, GENDER, ATT_SPL)       \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   

/*-------------------------------------------------------------------*/
     /*Frost Giants not so slow and can throw spells */
	
#define M_AVG_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) \
  BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)              \
   MSET_ABILITY(15,10,25,20,10,10,10,0) /*not too smart*/  \
   MSET_WEAPON(10,0,10,5,15,5)    /* 50 bashes stuff mainly */ 
 	 
	  
#define M_GIANT_CLOUD_AXE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(25,0,10,0,10,0)    /* 45 hammers, axes */
   
#define M_GIANT_CLOUD_SWORD(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(15,20,5,0,5,0)    /* 45 Swords */              

#define M_GIANT_CLOUD_CLUB(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)         \
   MSET_WEAPON(15,0,20,0,10,0)    /* 45 flail, mace club */ 
   
#define M_GIANT_CLOUD_POLE(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL)        \
   MSET_WEAPON(15,0,5,20,5,0)    /* 45 spear, pointed sticks */
   
#define M_GIANT_CLOUD_NATURAL(MLVL, GENDER, ATT_SPL) \
   BASE_GIANT_CLOUD(MLVL, GENDER, ATT_SPL) )       \
   MSET_WEAPON(15,0,10,0,20,0)    /* 45 kick, fist,etc  */                   

/* -------------------------------------------------------------------- */

#define M_GIANT_ETTIN_AXE(MLVL, GENDER, ATT_SPL, DEF_SPL)         \
   BASE_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(20,0,15,0,15,0)           /* 30%, 11% hammer user   */

#define M_GIANT_ETTIN_CLUB(MLVL, GENDER, ATT_SPL, DEF_SPL)           \
   BASE_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(10,0,20,5,15,0)            /* 30%, 11% mace or club user     */

#define M_GIANT_ETTIN_NATURAL(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                  \
   MSET_WEAPON(15,0,15,0,20,0)             /* 30%, 11% staff user    */

#define M_GIANT_ETTIN_SWORD(MLVL, GENDER, ATT_SPL, DEF_SPL)          \
   BASE_GIANT_ETTIN(MLVL, GENDER, ATT_SPL, DEF_SPL)                   \
   MSET_WEAPON(10,20,15,0,5,0)           /*    */

/* ==================================================================== */
#endif
