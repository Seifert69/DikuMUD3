/*
 $Author: All $
 $RCSfile: skills.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.3 $
 */

#include <stdlib.h>
#include <stdio.h>

#include "cmdload.h"
#include "structs.h"
#include "skills.h"
#include "common.h"
#include "utils.h"
#include "utility.h"
#include "comm.h"
#include "files.h"
#include "db.h"
#include "textutil.h"
#include "interpreter.h"
#include "trie.h"
#include "spelldef.h"

int hit_location_table[] = /* Determine by using 2d8 */
{
    WEAR_HEAD, /* 2..3   =>  4.68% for Head  */
    WEAR_HEAD,
    WEAR_HANDS, /* 4      =>  4.69% for Hands */
    WEAR_ARMS,  /* 5..7   => 23.43% for Arms  */
    WEAR_ARMS,
    WEAR_ARMS,
    WEAR_BODY, /* 8..10  => 34.38% for Body  */
    WEAR_BODY,
    WEAR_BODY,
    WEAR_LEGS, /* 11..14 => 28.12% for legs  */
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_LEGS,
    WEAR_FEET, /* 15..16 => 4.68% for feet   */
    WEAR_FEET
};

const char *professions[PROFESSION_MAX+1] = {PROFESSION_STRINGS, NULL};

struct profession_cost ability_prof_table[ABIL_TREE_MAX + 1];

struct profession_cost weapon_prof_table[WPN_TREE_MAX + 1];

struct profession_cost skill_prof_table[SKI_TREE_MAX + 1];
/*
//                        L  MAG DIV STR DEX CON CHA BRA  HP       FI  KN  PA  RA  TH  AS  PR  DR  MY  NE  CO  MA 
{SKI_TURN_UNDEAD      ,   5,   0,  1,  0,  0,  0,  0,  0,  0,      -5, -3, -3, -5, -5, -5, +5, -5, -5, -5, -5, -5},
{SKI_SCROLL_USE       ,  13,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -3, +2, +5, -3, +2, +5, +5},
{SKI_WAND_USE         ,   0,   0,  0,  0,  0,  0,  0,  3,  0,      -5, -5, -5, -5, -5, -5, -5, +5, -3,  0, +5, +5},
{SKI_CONSIDER         ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0, +2, +2, +5, +5, +5, +2, +4, -5, -5, +5,  0},
{SKI_DIAGNOSTICS      ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0, +2, +2, +2, +5, +5, +5, +4,  0, +5, +5, -5},
{SKI_APPRAISAL        ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +5, +5, -5, -5, -5, -5, -5, -5, -5},
{SKI_VENTRILOQUATE    ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, +5, -5, -5, -5, -5, -5, -5, -5},
{SKI_WEATHER_WATCH    ,  12,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, +5},
{SKI_FLEE             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0, -5, -5, +4, +5, +5,  0,  0, -3,  0,  0,  0},
{SKI_SNEAK            ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +2, +2, +2, -5, -5, -5, -5, -5, -5},
{SKI_BACKSTAB         ,   0,   0,  0,  0,  3,  0,  0,  0,  0,      -5, -5, -5, -5, +2, +5, -5, -5, -5, -5, -5, -5},
{SKI_HIDE             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5,  0, +2, +5, -5, -5, -5, -5, -5, -5},
{SKI_FIRST_AID        ,   6,   0,  0,  0,  0,  0,  0,  0,  0,      -5, +2, +2, +2,  0, -5, +5, +2,  0, -5, -5, -5},
//                        L  MAG DIV STR DEX CON CHA BRA  HP       FI  KN  PA  RA  TH  AS  PR  DR  MY  NE  CO  MA 
{SKI_PICK_LOCK        ,   2,   0,  0,  0, 10,  0,  0,  0,  0,      -5, -5, -5, -5, +2, -5, -5, -5, -5, -5, -5, -5},
{SKI_STEAL            ,  10,   0,  0,  0, 30,  0,  0,  0,  0,      -5, -5, -5, -5,  0, -5, -5, -5, -5, -5, -5, -5},
{SKI_RESCUE           ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0, +5, +5, +5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_SEARCH           ,   2,   0,  0,  0,  0,  0,  0,  0,  0,      -1, -2, -2, +3, +5, +2, -2, -2, -3, -3, -3, -3},
{SKI_LEADERSHIP       ,  12,   0,  0,  0,  0,  0,  0,  0,  0,      +2, +5, +5, +5, -5, -5, -5, -5, -5,  0, -5, -5},
{SKI_KICK             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_SWIMMING         ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0,  0,  0, +1, +1, +1, -1, +2, -1, -2, -2, -2},
{SKI_BASH             ,   3,   0,  0,  0,  0,  0,  0,  0,  0,      +2, +2, +2, +2, -5, -5, -5, -4, -5, -5, -5, -5},
{SKI_CLIMB            ,   0,   0,  0,  0,  0,  0,  0,  0,  0,       0, -3, -3,  0, +2, +2, -3, -3, -3, -3, -3, -3},
{SKI_SHIELD           ,   3,   0,  0,  0,  0,  0,  0,  0,  0,      +5, +2, +2, +2,  0,  0,  0, -4, -3, -5, -5, -5},
//                        L  MAG DIV STR DEX CON CHA BRA  HP       FI  KN  PA  RA  TH  AS  PR  DR  MY  NE  CO  MA 
{SKI_TRIP             ,   7,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5,  0, -5, -5, -5, -5, -5, -5, -5},
{SKI_DUAL_WIELD       ,  10,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5,  0, +2, -5, -5, -5, -5, -5, -5},
{SKI_CUFF             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_RESIZE_CLOTHES   ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_RESIZE_LEATHER   ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_RESIZE_METAL     ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_EVALUATE         ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_PEEK             ,   8,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5,  0, -5, -5, -5, -5, -5, -5, -5},
{SKI_PICK_POCKETS     ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, +2, -5, -5, -5, -5, -5, -5, -5},
{SKI_FILCH            ,   5,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, +5, -5, -5, -5, -5, -5, -5, -5},
//                        L  MAG DIV STR DEX CON CHA BRA  HP       FI  KN  PA  RA  TH  AS  PR  DR  MY  NE  CO  MA 
{SKI_DISARM           ,   4,   0,  0,  0,  0,  0,  0,  0,  0,      +2, -5, -5, -5, -1,  0, -5, -5, -5, -5, -5, -5},
{SKI_SKIN             ,   2,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +2, -5, -5, -5,  0, -5, -5, -5, -5},
{SKI_SHELTER          ,   8,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_SOOTHE           ,   3,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +2, -5, -5, -5, +2, -5, -5, -5, -5},
{SKI_AMBUSH           ,  18,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_CURARE           ,  40,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +3, -5, -5, -5, +3, -5, -5, -5, -5},
{SKI_FASHION          ,   1,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +3, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_BUTCHER          ,   4,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +3, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_LAY_TRAP         ,  28,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +4, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_SHOOT            ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_HERBS            ,  12,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5,  0, -5, -5, -5, +5, -5, -5, -5, -5},
{SKI_FORAGE           ,   3,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +1, -5, -5, -5, +1, -5, -5, -5, -5},
{SKI_DOWSE            ,   5,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5,  0, -5, -5, -5, +2, -5, -5, -5, -5},
{SKI_TRACK            ,   6,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_HUNT             ,  14,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +2, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_THROW            ,  20,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +3, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_COOK             ,  10,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +2, -5, -5, -5,  0, -5, -5, -5, -5},
{SKI_SCAN             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, +3, -5, -5, -5, +3, -5, -5, -5, -5},
{SKI_SLIP             ,  15,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, +1, -5, -5, -5, -5, -5, -5, -5},
//                        L  MAG DIV STR DEX CON CHA BRA  HP       FI  KN  PA  RA  TH  AS  PR  DR  MY  NE  CO  MA 
{SKI_PALM             ,  10,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, +1, -5, -5, -5, -5, -5, -5, -5},
{SKI_PLANT            ,  25,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5,  0, -5, -5, -5, -5, -5, -5, -5},
{SKI_STALK            ,  22,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, +2, -5, -5, -5, -5, -5, -5},
{SKI_KNEE             ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_ELBOW            ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_HIT              ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_PUNCH            ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5},
{SKI_GLANCE           ,   0,   0,  0,  0,  0,  0,  0,  0,  0,      -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5, -5}
};
*/

struct profession_cost spell_prof_table[SPL_TREE_MAX + 1];

const char *spl_text[SPL_TREE_MAX + 1];
struct tree_type spl_tree[SPL_TREE_MAX + 1];
sbit8 racial_spells[PC_RACE_MAX][SPL_TREE_MAX];
struct damage_chart_type spell_chart[SPL_TREE_MAX];

const char *pc_races[PC_RACE_MAX + 1];
const char *pc_race_adverbs[PC_RACE_MAX + 1];
struct race_info_type race_info[PC_RACE_MAX];

const char *ski_text[SKI_TREE_MAX + 1];
struct tree_type ski_tree[SKI_TREE_MAX + 1];
sbit8 racial_skills[PC_RACE_MAX][SKI_TREE_MAX];

const char *abil_text[ABIL_TREE_MAX + 1];
struct tree_type abil_tree[ABIL_TREE_MAX + 1];
sbit8 racial_ability[PC_RACE_MAX][ABIL_TREE_MAX];

const char *wpn_text[WPN_TREE_MAX + 1];
struct tree_type wpn_tree[WPN_TREE_MAX + 1];
sbit8 racial_weapons[PC_RACE_MAX][WPN_TREE_MAX];
struct damage_chart_type weapon_chart[WPN_TREE_MAX];
struct wpn_info_type wpn_info[WPN_TREE_MAX];

/* ===================================================================== */
int get_racial_ability(int nRace, int nAbility)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nAbility < 0) or (nAbility >= ABIL_TREE_MAX))
        return 0;
    
    return racial_ability[nRace][nAbility];
}

int get_racial_weapon(int nRace, int nWeapon)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nWeapon < 0) or (nWeapon >= WPN_TREE_MAX))
        return 0;
    
    return racial_weapons[nRace][nWeapon];
}

int get_racial_skill(int nRace, int nSkill)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nSkill < 0) or (nSkill >= SKI_TREE_MAX))
        return 0;
    
    return racial_skills[nRace][nSkill];
}

int get_racial_spells(int nRace, int nSpell)
{
    if ((nRace < 0) or (nRace >= PC_RACE_MAX))
        return 0;

    if ((nSpell < 0) or (nSpell >= SPL_TREE_MAX))
        return 0;
    
    return racial_spells[nRace][nSpell];
}

/* ===================================================================== */

void roll_description(class unit_data *att, const char *text, int roll)
{
    if (roll >= 200)
    {
        switch (roll / 100)
        {
        case 2:
            act("Great $2t!", A_ALWAYS, att, text, 0, TO_CHAR);
            act("$1n makes a great $2t!", A_ALWAYS, att, text, 0, TO_ROOM);
            break;

        case 3:
            act("Superb $2t!", A_ALWAYS, att, text, 0, TO_CHAR);
            act("$1n makes a superb $2t!", A_ALWAYS, att, text, 0, TO_ROOM);
            break;

        default:
            act("Divine $2t!", A_ALWAYS, att, text, 0, TO_CHAR);
            act("$1n makes a divinely inspired $2t!",
                A_ALWAYS, att, text, 0, TO_ROOM);
            break;
        }
    }
    else if (roll <= -100)
    {
        act("You fumble!", A_ALWAYS, att, text, 0, TO_CHAR);
        act("$1n fumbles!", A_ALWAYS, att, text, 0, TO_ROOM);
    }
}

int open_ended_roll(int size, int end)
{
    int i, s;

    s = i = number(1, size);

    if (i >= size - end + 1)  //1d100,5 means 96..100 (5% chance) xsuccess
    {
        do
        {
            s += (i = number(1, size));
        } while (i >= size - end + 1);
    }
    else if (i < end)        //1d100,5 means 1..4 (4% chance) of fumble
    {
        do
        {
            s -= (i = number(1, size));
        } while (i >= size - end + 1);
    }

    return s;
}

/* Copy of DIL function in skills.zon                            */
/* skillchecksa() checks 'self' versus the difficulty            */
/*    SkillIdx is the skill in question.                         */ 
/*    AbiIdx is the primary ability related                      */
/* Result > 0 is success                                         */
/* If skill and difficulty are equal => 50% chance success       */
int skillchecksa(class unit_data *u, int skillidx, int abiidx, int difficulty)
{
    int roll; 
    int skl;
    int hm;

    roll = open100();

    if (UNIT_TYPE(u) == UNIT_ST_PC)
        skl = (PC_SKI_SKILL(u, skillidx) * 2 + CHAR_ABILITY(u, abiidx)) / 3; /* 2/3rd skill, 1/3rd ability */
    else if (UNIT_TYPE(u) == UNIT_ST_NPC)
        skl = CHAR_ABILITY(u, abiidx);
    else
        skl = 0;

    hm = roll + skl - difficulty - 50;

    /* if (difficulty < 0)
        slog(LOG_ALL, 0, "skillchecksa() < 0  difficulty  = %d", difficulty);*/
    //slog(LOG_ALL, 0, "skillchecksa() 1d100OE = %d + skill = %d - difficulty %d - 50 = %d", roll, skl, difficulty, hm);

    return hm;
}


/* "Defender" must resist against attack from attacker.    */
/* I.e. attacker tries to steal from defender, or backstab */
/* or first aid, etc.                                      */
/* A result >= 0 means "resisted successfully by n.        */
/* A result < 0 means "failed resistance by n".            */
int resistance_skill_check(int att_skill1, int def_skill1,
                           int att_skill2, int def_skill2)
{
    return open100() + att_skill1 + att_skill2 - def_skill1 - def_skill2 - 50;
}

int weight_size(int lbs)
{
    if (lbs <= 5)
        return SIZ_TINY;
    else if (lbs <= 40)
        return SIZ_SMALL;
    else if (lbs <= 160)
        return SIZ_MEDIUM;
    else if (lbs <= 500)
        return SIZ_LARGE;
    else
        return SIZ_HUGE;
}

int weapon_fumble(class unit_data *weapon, int roll)
{
    assert(IS_OBJ(weapon) && (OBJ_TYPE(weapon) == ITEM_WEAPON));

    return roll <= weapon_chart[OBJ_VALUE(weapon, 0)].fumble;
}

int chart_damage(int roll, struct damage_chart_element_type *element)
{
    if (element->alpha == 0)
    {
        slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
        return element->basedam;
    }

    if (roll < element->offset)
        return 0;
    else
        return element->basedam + ((roll - element->offset) / element->alpha);
}

/* Size is for natural attacks to limit max damage for such */
int chart_size_damage(int roll, struct damage_chart_element_type *element,
                      int lbs)
{
    if (element->alpha == 0)
    {
        slog(LOG_ALL, 0, "Error: Damage chart alpha is 0!");
        return element->basedam;
    }

    if (roll < element->offset)
        return 0;
    else
    {
        switch (weight_size(lbs))
        {
        case SIZ_TINY:
            roll = MIN(100, roll);
            break;

        case SIZ_SMALL:
            roll = MIN(110, roll);
            break;

        case SIZ_MEDIUM:
            roll = MIN(130, roll);
            break;

        case SIZ_LARGE:
            roll = MIN(150, roll);
            break;
        }

        return element->basedam + ((roll - element->offset) / element->alpha);
    }
}

int weapon_damage(int roll, int weapon_type, int armour_type)
{
    if (!is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX))
    {
        slog(LOG_ALL, 0, "Illegal weapon type.");
        return 0;
    }

    if (!is_in(armour_type, ARM_CLOTHES, ARM_PLATE))
    {
        slog(LOG_ALL, 0, "Illegal armour type.");
        armour_type = ARM_CLOTHES;
    }

    return chart_damage(roll,
                        &(weapon_chart[weapon_type].element[armour_type]));
}

int natural_damage(int roll, int weapon_type, int armour_type, int lbs)
{
    if (!is_in(weapon_type, WPN_GROUP_MAX, WPN_TREE_MAX))
    {
        slog(LOG_ALL, 0, "Illegal weapon type.");
        return 0;
    }

    if (!is_in(armour_type, ARM_CLOTHES, ARM_PLATE))
    {
        slog(LOG_ALL, 0, "Illegal armour type.");
        armour_type = ARM_CLOTHES;
    }

    return chart_size_damage(roll,
                             &(weapon_chart[weapon_type].element[armour_type]),
                             lbs);
}

/* Return [0..200] for skill when defending with a weapon */
int weapon_defense_skill(class unit_data *ch, int skill)
{
    int max;

    if (IS_PC(ch))
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            max = PC_WPN_SKILL(ch, skill) / 2;
        else
            max = PC_WPN_SKILL(ch, skill);

        while (!TREE_ISROOT(wpn_tree, skill))
        {
            skill = TREE_PARENT(wpn_tree, skill);

            if (PC_WPN_SKILL(ch, skill) > max)
                max = PC_WPN_SKILL(ch, skill);
        }

        return max;
    }
    else /* a NPC */
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            skill = TREE_PARENT(wpn_tree, skill);

        if (TREE_ISROOT(wpn_tree, skill))
            max = NPC_WPN_SKILL(ch, skill);
        else
            max = NPC_WPN_SKILL(ch, skill) / 2;

        while (!TREE_ISROOT(wpn_tree, skill))
        {
            skill = TREE_PARENT(wpn_tree, skill);

            if (NPC_WPN_SKILL(ch, skill) > max)
                max = NPC_WPN_SKILL(ch, skill);
        }

        return max;
    }
}

/* Return [0..200] for skill when attacking with a weapon */
int weapon_attack_skill(class unit_data *ch, int skill)
{
    if (IS_PC(ch))
    {
        return PC_WPN_SKILL(ch, skill) == 0 ? -50 : PC_WPN_SKILL(ch, skill);
    }
    else
    {
        if (TREE_ISLEAF(wpn_tree, skill))
            skill = TREE_PARENT(wpn_tree, skill);

        return NPC_WPN_SKILL(ch, skill);
    }
}

/* Return the armour position of where one person hits another */
int hit_location(class unit_data *att, class unit_data *def)
{
    /* Maybe do height reductions later */

    return hit_location_table[dice(2, 8) - 2];
}

/* Return the effective dex of a person in armour ...             */
/* Later we will redo this function - as of now it doesn't matter */
/* what armour you wear                                           */
int effective_dex(class unit_data *ch)
{
    return CHAR_DEX(ch);
}

/* ========================================================================= */


void profession_init(void)
{
    int i;
    int ln;

    ln = sizeof(professions) / sizeof(professions[0]);

    for (i = 0; i < ln; i++)
    {
        professions[i] = NULL;
    }
}


static void profession_read(void)
{
    int idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, PROFESSION_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, PROFESSION_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to read etc/" PROFESSION_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
            str_lower(pTmp);
        }

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, PROFESSION_MAX - 1))
            {
                slog(LOG_ALL, 0, "Profession boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (professions[idx])
               free((char *) professions[idx]);
            professions[idx] = str_dup(pCh);
        }
        else
            slog(LOG_ALL, 0, "Profession boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

void boot_profession(void)
{
    profession_init();
    profession_read();
}

/* ========================================================================= */

static void race_read(void)
{
    int idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;
    char tmp[256];

    touch_file(str_cc(g_cServerConfig.m_etcdir, RACE_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, RACE_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc/" RACE_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
            str_lower(pTmp);
        }

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, PC_RACE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Race boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (pc_races[idx])
            {
                free((char *)pc_races[idx]);
                pc_races[idx] = NULL;
            }
            pc_races[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "adverb", 6) == 0)
        {
            if (pc_race_adverbs[idx])
            {
                free((char *)pc_race_adverbs[idx]);
                pc_race_adverbs[idx] = NULL;
            }
            pc_race_adverbs[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "height male", 11) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.height_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "height female", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.height_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "weight male", 11) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.weight_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "weight female", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.weight_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "lifespan male", 13) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].male.lifespan_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "lifespan female", 15) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].female.lifespan_dice.size = atoi(tmp);
        }
        else if (strncmp(pTmp, "age", 3) == 0)
        {
            pCh = str_next_word(pCh, tmp);
            race_info[idx].age = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].age_dice.reps = atoi(tmp);

            pCh = str_next_word(pCh, tmp);
            race_info[idx].age_dice.size = atoi(tmp);
        }
        else
            slog(LOG_ALL, 0, "Race boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

struct diltemplate *playerinit_tmpl;
struct diltemplate *nanny_dil_tmpl;

static void race_init(void)
{
    int i;

    for (i = 0; i < PC_RACE_MAX; i++)
    {
        memset(&race_info[i], 0, sizeof(struct race_info_type));

        pc_races[i] = NULL;
        pc_race_adverbs[i] = NULL;
    }

    pc_races[PC_RACE_MAX] = NULL;
    pc_race_adverbs[PC_RACE_MAX] = NULL;

    playerinit_tmpl = find_dil_template("playerinit@basis");
    if (playerinit_tmpl == NULL)
        slog(LOG_ALL, 0, "No 'playerinit@basis' DIL template.");
    else
    {
        if (playerinit_tmpl->argc != 0)
        {
            slog(LOG_ALL, 0, "playerinit@basis(); not defined correctly.");
            playerinit_tmpl = NULL;
        }
    }

    nanny_dil_tmpl = find_dil_template("nanny@basis");
    if (nanny_dil_tmpl == NULL)
        slog(LOG_ALL, 0, "No 'nanny@basis' DIL template.");
    else
    {
        if ((nanny_dil_tmpl->argc != 1) || (nanny_dil_tmpl->argt[0] != DILV_SP))
        {
            slog(LOG_ALL, 0, "nanny@basis(string); not defined correctly.");
            nanny_dil_tmpl = NULL;
        }
    }
}

void boot_race(void)
{
    race_init();
    race_read();
}

/* ========================================================================= */

static void ability_read(void)
{
    int dummy, idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, ABILITY_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, ABILITY_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc/" ABILITY_DEFS);
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);

        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if ((pCh = strchr(pTmp, '=')))
        {
            *pCh = 0;
            pCh = skip_blanks(pCh + 1);
            strip_trailing_blanks(pCh);
        }

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (pCh == NULL || str_is_empty(pCh))
            continue;

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, 0, ABIL_TREE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Ability boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (abil_text[idx])
            {
                free((char *)abil_text[idx]);
                abil_text[idx] = NULL;
            }
            abil_text[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
            {
                slog(LOG_ALL, 0, "Abilities: race modifier %d for %s not in [-3..+3]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Abilities: Illegal race in: %s", pTmp);
            else
                racial_ability[ridx][idx] = dummy;
        }
        else if (strncmp(pTmp, "profession ", 11) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -9, +5))
            {
                slog(LOG_ALL, 0, "Abilities: profession modifier %d for %s not in [-9..+5]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 11, professions, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Abilities: Illegal profession %s", pTmp);
            else
                ability_prof_table[idx].profession_cost[ridx] = dummy;
        }
        else
            slog(LOG_ALL, 0, "Ability boot unknown string: %s", pTmp);
    }

    fclose(fl);
}

static void ability_init(void)
{
    int i;

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        abil_tree[i].parent = i;
        abil_tree[i].isleaf = TRUE;
        abil_text[i] = NULL;

        for (int j = 0; j < PC_RACE_MAX; j++)
            racial_ability[j][i] = 0;

        ability_prof_table[i].sanity = i;
        ability_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            ability_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            ability_prof_table[i].profession_cost[j] = -7;
    }

    abil_tree[ABIL_TREE_MAX].parent = -1;
    abil_tree[ABIL_TREE_MAX].isleaf = FALSE;
    abil_text[ABIL_TREE_MAX] = NULL;
}


void ability_dump(void)
{
    int i;

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        printf("name                    = %s\n", abil_text[i]);

        if (ability_prof_table[i].min_level > 0 )
            printf("restrict level          = %d\n", ability_prof_table[i].min_level);

        for (int j=0; j < ABIL_TREE_MAX; j++)
            if (ability_prof_table[i].min_abil[j] > 0)
                printf("restrict %s%s    = %s%d\n", abil_text[j], spc(12-strlen(professions[j])),
                 (ability_prof_table[i].min_abil[j] >= 0) ? "+" : "", ability_prof_table[i].min_abil[j]);

        for (int j=0; j < PROFESSION_MAX; j++)
            if (ability_prof_table[i].profession_cost[j] > -7)
                printf("profession %s%s = %s%d\n", professions[j], spc(12-strlen(professions[j])),
                 (ability_prof_table[i].profession_cost[j] >= 0) ? "+" : "", ability_prof_table[i].profession_cost[j]);
    }
}

void boot_ability(void)
{
    ability_init();
    ability_read();
    //ability_dump(); Saved in case someone needs to dump it out to excel or something
}

/* ========================================================================= */

static void weapon_read(void)
{
    int dummy, idx = -1;
    char pTmp[256];
    char *pCh;
    FILE *fl;

    touch_file(str_cc(g_cServerConfig.m_etcdir, WEAPON_DEFS));
    if (!(fl = fopen(str_cc(g_cServerConfig.m_etcdir, WEAPON_DEFS), "rb")))
    {
        slog(LOG_ALL, 0, "unable to create etc file");
        exit(0);
    }

    while (!feof(fl))
    {
        char *mstmp = fgets(pTmp, sizeof(pTmp) - 1, fl);
        if (mstmp == NULL)
            continue;

        str_remspc(pTmp);

        if (str_is_empty(pTmp))
            continue;
    
        pCh = strchr(pTmp, '=');
        if (pCh == NULL)
        {
            slog(LOG_ALL, 0, "Weapon boot odd line, no equal sign: %s", pTmp);
            continue;
        }

        *pCh = 0;
        pCh = skip_blanks(pCh + 1);
        strip_trailing_blanks(pCh);

        str_lower(pTmp);
        strip_trailing_blanks(pTmp);

        if (str_is_empty(pTmp) || str_is_empty(pCh))
        {
            slog(LOG_ALL, 0, "Weapon boot odd line: %s = %s", pTmp, pCh);
            continue;
        }

        if (strncmp(pTmp, "index", 5) == 0)
        {
            idx = atoi(pCh);
            if (!str_is_number(pCh) || !is_in(idx, WPN_ROOT, WPN_TREE_MAX - 1))
            {
                slog(LOG_ALL, 0, "Weapon boot error: %s", pCh);
                idx = -1;
            }
            continue;
        }

        if (idx == -1)
            continue;

        if (strncmp(pTmp, "name", 4) == 0)
        {
            if (wpn_text[idx])
            {
                free((char *)wpn_text[idx]);
                wpn_text[idx] = NULL;
            }
            wpn_text[idx] = str_dup(pCh);
        }
        else if (strncmp(pTmp, "shield", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, SHIELD_M_BLOCK, SHIELD_M_USELESS))
                wpn_info[idx].shield = dummy;
        }
        else if (strncmp(pTmp, "sphere", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, WPN_ROOT, WPN_GROUP_MAX - 1))
                wpn_tree[idx].parent = dummy;
        }
        else if (strncmp(pTmp, "race ", 5) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -3, +3))
                continue;

            int ridx = search_block(pTmp + 5, pc_races, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Weapons: Illegal race in: %s", pTmp);
            else
                racial_weapons[ridx][idx] = dummy;
        }
        else if (strncmp(pTmp, "fumble", 6) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 99))
                weapon_chart[idx].fumble = dummy;
        }
        else if (strncmp(pTmp, "hands", 5) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 1, 2))
                wpn_info[idx].hands = dummy;
        }
        else if (strncmp(pTmp, "speed", 5) == 0)
        {
            dummy = atoi(pCh);
            if (is_in(dummy, 0, 12))
                wpn_info[idx].speed = dummy;
        }
        else if (strncmp(pTmp, "type", 4) == 0)
        {
            dummy = atoi(pCh);
            switch (dummy)
            {
            case WPNT_SLASH:
            case WPNT_PIERCE:
            case WPNT_BLUDGEON:
                wpn_info[idx].type = dummy;
            }
        }
        else if (strncmp(pTmp, "profession ", 11) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, -9, +5))
            {
                slog(LOG_ALL, 0, "Weapons: profession modifier %d for %s not in [-9..+5]", dummy, pTmp);
                continue;
            }

            int ridx = search_block(pTmp + 11, professions, TRUE);

            if (ridx == -1)
                slog(LOG_ALL, 0, "Weapons: Illegal profession %s", pTmp);
            else
                weapon_prof_table[idx].profession_cost[ridx] = dummy;
        }
        else if (strncmp(pTmp, "restrict ", 9) == 0)
        {
            dummy = atoi(pCh);
            if (!is_in(dummy, 0, 250))
            {
                slog(LOG_ALL, 0, "Weapons: restrict modifier %d for %s not in [0..250]", dummy, pTmp);
                continue;
            }

            if (strncmp(pTmp+9, "level", 5) == 0)
            {
                weapon_prof_table[idx].min_level = dummy;
            }
            else
            {
                int ridx = search_block(pTmp + 9, abil_text, TRUE);

                if (ridx == -1)
                    slog(LOG_ALL, 0, "Weapons: Illegal restrict %s", pTmp);
                else
                    weapon_prof_table[idx].min_abil[ridx] = dummy;
            }
        }
        else if (strncmp(pTmp, "attack ", 7) == 0)
        {
            char tmp[256];
            int i1, i2, i3;
            int idx2 = -1;

            pCh = str_next_word(pCh, tmp);
            i1 = atoi(tmp);
            pCh = str_next_word(pCh, tmp);
            i2 = atoi(tmp);
            pCh = str_next_word(pCh, tmp);
            i3 = atoi(tmp);

            if (i3 <= 0)
            {
                slog(LOG_ALL, 0,
                     "Weapon init %d: Illegal damage series %d %d %d.", idx,
                     i1, i2, i3);
                continue;
            }

            if (strncmp(pTmp + 7, "clothes", 7) == 0)
                idx2 = ARM_CLOTHES;
            else if (strncmp(pTmp + 7, "sleather", 8) == 0)
                idx2 = ARM_LEATHER;
            else if (strncmp(pTmp + 7, "hleather", 8) == 0)
                idx2 = ARM_HLEATHER;
            else if (strncmp(pTmp + 7, "chain", 5) == 0)
                idx2 = ARM_CHAIN;
            else if (strncmp(pTmp + 7, "plate", 5) == 0)
                idx2 = ARM_PLATE;

            if (idx2 != -1)
            {
                weapon_chart[idx].element[idx2].offset = i1;
                weapon_chart[idx].element[idx2].basedam = i2;
                weapon_chart[idx].element[idx2].alpha = i3;
            }
        }
        else
            slog(LOG_ALL, 0, "Weapon boot unknown string: [%s]", pTmp);
    }

    fclose(fl);
}

static void weapon_init(void)
{
    int i;
    int j;

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        weapon_chart[i].fumble = 0;
        for (j = 0; j < 5; j++)
        {
            weapon_chart[i].element[j].offset = 100;
            weapon_chart[i].element[j].basedam = 0;
            weapon_chart[i].element[j].alpha = 100;
        }

        wpn_info[i].hands = 1;
        wpn_info[i].type = WPNT_SLASH;
        wpn_info[i].speed = 0;
        wpn_info[i].shield = SHIELD_M_BLOCK;

        wpn_tree[i].parent = WPN_ROOT;

        if (i < WPN_GROUP_MAX)
            wpn_tree[i].isleaf = FALSE;
        else
            wpn_tree[i].isleaf = TRUE;

        wpn_text[i] = NULL;

        /* Default to zero */
        for (j = 0; j < PC_RACE_MAX; j++)
            racial_weapons[j][i] = 0;

        /* Clear the weapon_prof table */
        weapon_prof_table[i].sanity = i;
        weapon_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            weapon_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            weapon_prof_table[i].profession_cost[j] = -7;

        if ((i <= LAST_WEAPON) && (weapon_prof_table[i].sanity != i))
        {
            slog(LOG_ALL, 0, "weapon_prof_table[%d] has wrong sanity", i);
            exit(0);
        }
   }

    wpn_tree[WPN_TREE_MAX].parent = -1;
    wpn_tree[WPN_TREE_MAX].isleaf = FALSE;
    wpn_text[WPN_TREE_MAX] = NULL;
}

void weapon_dump(void)
{
    int i;

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        if (wpn_text[i] == NULL)
            continue;

        printf("name                    = %s\n", wpn_text[i]);

        if (weapon_prof_table[i].min_level > 0)
            printf("restrict level          = %d\n", weapon_prof_table[i].min_level);

        for (int j=0; j < ABIL_TREE_MAX; j++)
            if (weapon_prof_table[i].min_abil[j] > 0)
                printf("restrict %s%s    = %s%d\n", abil_text[j], spc(12-strlen(abil_text[j])),
                 (weapon_prof_table[i].min_abil[j] >= 0) ? "+" : "", weapon_prof_table[i].min_abil[j]);

        for (int j=0; j < PROFESSION_MAX; j++)
            if (weapon_prof_table[i].profession_cost[j] > -5)
                printf("profession %s%s = %s%d\n", professions[j], spc(12-strlen(professions[j])),
                 (weapon_prof_table[i].profession_cost[j] >= 0) ? "+" : "", weapon_prof_table[i].profession_cost[j]);

        printf("\n");
    }
    exit(0);
}


void boot_weapon(void)
{
    weapon_init();
    weapon_read();
    //weapon_dump();
}


/* ========================================================================= */

static void skill_init(void)
{
    int i;

    for (i = 0; i < SKI_TREE_MAX; i++)
    {
        ski_tree[i].parent = i;
        ski_tree[i].isleaf = TRUE;

        ski_text[i] = NULL;

        /* Racial skills are all zero */
        for (int j = 0; j < PC_RACE_MAX; j++)
            racial_skills[j][i] = 0;

        /* Clear the skill_prof table */
        skill_prof_table[i].sanity = i;
        skill_prof_table[i].min_level = 0;

        for (int j=0; j < ABIL_TREE_MAX; j++)
            skill_prof_table[i].min_abil[j] = 0;

        for (int j=0; j < PROFESSION_MAX; j++)
            skill_prof_table[i].profession_cost[j] = -7;

        if ((i < LAST_SKILL) && (skill_prof_table[i].sanity != i))
        {
            slog(LOG_ALL, 0, "skill_prof_table[%d] has wrong sanity", i);
            exit(0);
        }
    }

    ski_tree[SKI_TREE_MAX].parent = -1;
    ski_tree[SKI_TREE_MAX].isleaf = FALSE;
    ski_text[SKI_TREE_MAX] = NULL;
}

void boot_skill(void)
{
    skill_init();
}
