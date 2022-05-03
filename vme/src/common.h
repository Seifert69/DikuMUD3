/*
 $Author: All $
 $RCSfile: common.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */
#pragma once

#include "essential.h"
#include "unit_data.h"

#include <array>

/* ..................................................................... */
/*                           A B I L I T I E S                           */
/* ..................................................................... */

#define SKILL_MAX 500 /* Maximum of any skill / ability */
#define AVERAGE_SKILL_COST 10
#define HITPOINTS_BASE 125
#define ABILITY_POINT_FACTOR (4) /* Abilitypoints multiplied by this */
#define SKILL_POINT_FACTOR (8)   /* Skillpoints   multiplied by this */

/* ..................................................................... */
/*                           A R M O U R S                               */
/* ..................................................................... */

struct shi_info_type
{
    int melee; ///< Protection against melee attacks
};

struct arm_info_type
{
    int slash;
    int bludgeon;
    int pierce;
};

int required_xp(int level);
int level_xp(int level);
void set_hits(unit_data *obj, int craftsmanship);
int skill_point_gain();
int buy_points(int points, int level, int *error);
int ability_point_gain(unit_data *ch);
int ability_point_total(unit_data *ch);
/**
 * Algorithm PS 5
 * Used to distribute points after a given percentage scheme. Each
 * ability (8 of them) contains the percentage number. The percentage
 * must add up to 100%.
 *
 * @tparam T Type in array
 * @tparam MAX_ARRAY Max size of array
 * @param skills Reference to the array to fill
 * @param max Max size of array
 * @param points
 * @param level
 * @return Returns true if error
 */
template<class T, size_t MAX_ARRAY>
int distribute_points(std::array<T, MAX_ARRAY> &skills, int max, int points, int level)
{
    int error = 0;
    int sumerror = 0;

    for (int i = 0; i < max; i++)
    {
        skills[i] = buy_points((int)((double)skills[i] * points / 100.0), level, &error);
        if (error > sumerror)
        {
            sumerror = error;
        }
    }

    return sumerror;
}
int distribute_points(ubit8 *skills, int max, int points, int level);
void set_weapon(unit_data *o);
void set_shield(unit_data *o);
void set_armour(unit_data *o);
int bonus_map_a(int bonus);
int bonus_map_b(int bonus);

extern shi_info_type g_shi_info[];
extern const char *g_dirs[];
extern const char *g_dirs_short[];
extern int g_possible_saves;
