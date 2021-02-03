/*
 $Author: All $
 $RCSfile: common.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h> /* floor and pow */

#include "structs.h"
#include "utils.h"
#include "common.h"
#include "skills.h"
#include "utility.h"
#include "db.h"

int possible_saves = 0;

/* Used for converting general direction in dmc! */
const char *dirs[] = {
    "north",
    "east",
    "south",
    "west",
    "up",
    "down",
    "northeast",
    "northwest",
    "southeast",
    "southwest",
    NULL};

/* Used for converting general direction in dmc! */
const char *dirs_short[] = {
    "n",
    "e",
    "s",
    "w",
    "u",
    "d",
    "ne",
    "nw",
    "se",
    "sw",
    NULL};

struct shi_info_type shi_info[] = {
    /* %age Chance of blocking an attack if ready to block */
    {15}, /* SHIELD_SMALL  */
    {20}, /* SHIELD_MEDIUM */
    {25}  /* SHIELD_LARGE  */
};

/* PS Algorithm 2                                                      */
/* This algorithm returns the amount of points gained at a particular  */
/* level. Level is given as an argument, and the amount of points you  */
/* will get for that level is returned                                 */
/* Example: A character is about to raise from level 2 to 3. Add       */
/*          ability_point_gain(3) to his ability points                */

int ability_point_gain(class unit_data *ch)
{
    if (IS_NPC(ch))
        return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR;
    else
    {
        if (PC_VIRTUAL_LEVEL(ch) <= 100)
            return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR;
        else
            return 0;
    }
}

/*int ability_point_gain(int level)
{
    if (level <= 100)
        return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR;
    else
        return 0;
}*/

/* PS Algorithm 3                                                      */
/* This algorithm returns the total amount of points gained up to a    */
/* particular level.                                                   */
/* The formula is total up to the current level                        */
// Only used for NPCs and their point distro so we ignore the 100 level cap.
// 
int ability_point_total(class unit_data *ch)
{
    if (IS_NPC(ch))
        return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR * CHAR_LEVEL(ch);
    else
        return AVERAGE_SKILL_COST * ABILITY_POINT_FACTOR * MIN(PC_VIRTUAL_LEVEL(ch), 100);
}


int skill_point_gain(void)
{
    return AVERAGE_SKILL_COST * SKILL_POINT_FACTOR;
}



/* Algorithm PS 4                                                      */
/*                                                                     */
/* This algorithm takes a base cost (cost) and an amount of points     */
/* (points). It then calculates how many ability points you could buy  */
/* for the given amount of points (skill-buy-points)                   */
/*                                                                     */



int buy_points(int points, int level, int *error)
{
    int skill;
    int cost;
    int i;

    assert(level >= 0);

    skill = 0;
    cost = AVERAGE_SKILL_COST;

    while (points > 0)
    {
        for (i = 0; (i <= level) && (points > 0); i++)
        {
            points -= cost;
            skill++;
        }
        cost += AVERAGE_SKILL_COST;
    }

    if (points < 0)
        skill--;

    if (error)
        *error = 0;

    if (skill > SKILL_MAX)
    {
        if (error)
            *error = skill;

        return SKILL_MAX;
    }
    if (skill < 0)
    {
        if (error)
            *error = skill;

        return 0;
    }

    return skill;
}

/* Algorithm PS 5                                                      */
/* Used to distribute points after a given percentage scheme. Each     */
/* ability (8 of them) contains the percentage number. The percentage  */
/* must add up to 100%.                                                */
/* Returns true if error */

int distribute_points(sbit16 *skills, int max, int points, int level)
{
    int i, error, sumerror;

    sumerror = 0;

    for (i = 0; i < max; i++)
    {
        skills[i] = buy_points((int)((double)skills[i] * points / 100.0),
                               level, &error);
        if (error > sumerror)
            sumerror = error;
    }

    return sumerror;
}

int distribute_points(ubit8 *skills, int max, int points, int level)
{
    int i, error, sumerror;

    sumerror = 0;

    for (i = 0; i < max; i++)
    {
        skills[i] = buy_points((int)((double)skills[i] * points / 100.0),
                               level, &error);
        if (error > sumerror)
            sumerror = error;
    }

    return sumerror;
}

/* Given a level, it returns the total amount of experience-points */
/* which is minimum required to be that level. For example         */
/* required_xp(1) = 1650 since it requires 1650 xp to be           */
/* level one.                                                      */

int required_xp(int level)
{
#define LEVEL_MULT 300  // Diku II was : 1500
#define POWER_MULT 200  // Diku II was :  150

    if (level <= 0)
        return 0;

    if (level <= MORTAL_MAX_LEVEL)
        return LEVEL_MULT * level + level * level * POWER_MULT;
    else
        return required_xp(MORTAL_MAX_LEVEL) +
               level_xp(MORTAL_MAX_LEVEL-1) * (level - MORTAL_MAX_LEVEL);

#undef LEVEL_MULT
#undef POWER_MULT
}

/* Given a level, it returns how much XP is required to reach the  */
/* next level. For example, given level 1, it returns that it      */
/* takes level(1+1)-level(1) xp to reach level 2.                    */
/* Level has to be < MORTAL_MAX_LEVEL */
int level_xp(int level)
{
    if (level >= MORTAL_MAX_LEVEL)
        level = MORTAL_MAX_LEVEL-1;

    return required_xp(level+1) - required_xp(level);

    /* if (level <= MORTAL_MAX_LEVEL)
        return 1650 + level * 300;
    else
        return level_xp(MORTAL_MAX_LEVEL);*/
}


/* Primarily used for shields, armours and weapons */
void set_hits(class unit_data *obj, int craftsmanship)
{
    if (UNIT_HIT(obj) == 100) // 100 is the *default* and is overridden
    {
        /* Hits are in [100..6000] based on craft, default == 1000 */

        if (craftsmanship >= 0)
            UNIT_MAX_HIT(obj) = 1000 + (1000 * craftsmanship) / 5;
        else
            UNIT_MAX_HIT(obj) = 1000 - (175 * -craftsmanship) / 5;

        UNIT_HIT(obj) = UNIT_MAX_HIT(obj);
    }
}

/* WEAPONS                               */
/* Input:  Value[0] is weapon category   */
/*         Value[1] is craftsmanship     */
/*         Value[2] is magic bonus       */
/*         Value[3] is slaying race      */
/*         Value[4]                      */
/*                                       */
void set_weapon(class unit_data *weapon)
{
    set_hits(weapon, OBJ_VALUE(weapon, 1));
}

/* ARMOURS                                */
/* INPUT:  Value[0] = Category            */
/*         Value[1] is craftsmanship      */
/*         Value[2] is magic bonus        */

void set_armour(class unit_data *armour)
{
    set_hits(armour, OBJ_VALUE(armour, 1));
}

/* SHIELDS                                  */
/* INPUT:  Value[0] = Shield Category       */
/*         Value[1] is craftsmanship        */
/*         Value[2] is magic bonus          */
/*                                          */
void set_shield(class unit_data *shield)
{
    set_hits(shield, OBJ_VALUE(shield, 1));
}
