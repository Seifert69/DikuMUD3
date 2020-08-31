/*
 $Author: All $
 $RCSfile: fight.h,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.2 $
 */

#ifndef _MUD_FIGHT_H
#define _MUD_FIGHT_H

#include <string>
#include <cstring>


struct SFightColorSet
{
    char *pAttacker;
    char *pDefender;
    char *pOthers;
};

class unit_data *raw_kill (class unit_data * ch);
void die (class unit_data * ch);



void update_pos (class unit_data * victim);

void damage (class unit_data * ch, class unit_data * victim,
             class unit_data * medium,
             int damage, int attackcat, int weapontype, int hitloc,
             int bDisplay = TRUE);

int pk_test (class unit_data * att, class unit_data * def, int message);
int one_hit (class unit_data * att, class unit_data * def,
             int bonus, int wpn_type, int primary = TRUE, int attack = TRUE);
int simple_one_hit (class unit_data * att, class unit_data * def);
int char_dual_wield (class unit_data * ch);
void melee_violence (class unit_data * ch, int primary);
int melee_bonus(class unit_data *att, class unit_data *def,
                int hit_loc,
                int *pAtt_weapon_type, class unit_data **pAtt_weapon,
                int *pDef_armour_type, class unit_data **pDef_armour,
                int primary = TRUE, std::string *pStat = NULL);
int shield_bonus (class unit_data * att, class unit_data * def,
                  class unit_data ** pDef_shield);
void modify_hit(class unit_data *ch, int hit);

#endif
