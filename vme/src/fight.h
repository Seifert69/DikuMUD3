/*
 $Author: All $
 $RCSfile: fight.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */
#pragma once

#include "essential.h"

#include <cstring>
#include <string>

struct SFightColorSet
{
    char *pAttacker;
    char *pDefender;
    char *pOthers;
};

void die(class unit_data *ch);

void update_pos(class unit_data *victim);

void damage(class unit_data *ch,
            class unit_data *victim,
            class unit_data *medium,
            int damage,
            int attackcat,
            int weapontype,
            int hitloc,
            int bDisplay = TRUE);

int pk_test(class unit_data *att, class unit_data *def, int message);
int one_hit(class unit_data *att, class unit_data *def, int bonus, int wpn_type, int primary = TRUE, int attack = TRUE);
int simple_one_hit(class unit_data *att, class unit_data *def);
int char_dual_wield(class unit_data *ch);
void melee_violence(class unit_data *ch, int primary);
void modify_hit(class unit_data *ch, int hit);
int roll_boost(int roll, int level);
void damage_object(class unit_data *ch, class unit_data *obj, int dam);
void load_messages(void);
int hunting(struct spec_arg *sarg);
