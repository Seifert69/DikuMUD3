/*
 $Author: All $
 $RCSfile: fight.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "essential.h"
#include "interpreter.h"
#include "structs.h"

#include <cstring>
#include <string>

struct SFightColorSet
{
    char *pAttacker;
    char *pDefender;
    char *pOthers;
};

void die(unit_data *ch);

void update_pos(unit_data *victim);

void damage(unit_data *ch,
            unit_data *victim,
            unit_data *medium,
            int damage,
            int attackcat,
            int weapontype,
            int hitloc,
            int bDisplay = TRUE);

int pk_test(unit_data *att, unit_data *def, int message);
int one_hit(unit_data *att, unit_data *def, int bonus, int wpn_type, int primary = TRUE, int attack = TRUE);
int simple_one_hit(unit_data *att, unit_data *def);
int char_dual_wield(unit_data *ch);
void melee_violence(unit_data *ch, int primary);
void modify_hit(unit_data *ch, int hit);
int roll_boost(int roll, int level);
void damage_object(unit_data *ch, unit_data *obj, int dam);
void load_messages();
int hunting(spec_arg *sarg);
bool is_obj_slaying(unit_data *obj, ubit16 opponent_race);
