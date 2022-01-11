/*
 $Author: All $
 $RCSfile: common.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.4 $
 */
#pragma once

#include "essential.h"

/* ..................................................................... */
/*                           A B I L I T I E S                           */
/* ..................................................................... */

#define SKILL_MAX 250 /* Maximum of any skill / ability */
#define AVERAGE_SKILL_COST 10
#define HITPOINTS_BASE 125
#define ABILITY_POINT_FACTOR (4) /* Abilitypoints multiplied by this */
#define SKILL_POINT_FACTOR (8)   /* Skillpoints   multiplied by this */

/* ..................................................................... */
/*                           A R M O U R S                               */
/* ..................................................................... */

struct shi_info_type
{
    int melee; /* Protection against melee attacks */
};

struct arm_info_type
{
    int slash;
    int bludgeon;
    int pierce;
};

int required_xp(int level);
int level_xp(int level);
void set_hits(class unit_data *obj, int craftsmanship);
int skill_point_gain(void);
int ability_point_gain(class unit_data *ch);
int ability_point_total(class unit_data *ch);
int distribute_points(sbit16 *skills, int max, int points, int level);
int distribute_points(ubit8 *skills, int max, int points, int level);
int buy_points(int points, int level, int *error);
void set_weapon(class unit_data *o);
void set_shield(class unit_data *o);
void set_armour(class unit_data *o);

extern struct shi_info_type g_shi_info[];
extern const char *g_dirs[];
extern const char *g_dirs_short[];
extern int g_possible_saves;
