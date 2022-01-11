#pragma once

#include "essential.h"

#include <string>

int experience_modification(class unit_data *att, class unit_data *def);
int spell_bonus(class unit_data *att,
                class unit_data *medium,
                class unit_data *def,
                int hit_loc,
                int spell_number,
                int *pDef_armour_type,
                class unit_data **pDef_armour,
                std::string *pStat);
int shield_bonus(class unit_data *att, class unit_data *def, class unit_data **pDef_shield);
int melee_bonus(class unit_data *att,
                class unit_data *def,
                int hit_loc,
                int *pAtt_weapon_type,
                class unit_data **pAtt_weapon,
                int *pDef_armour_type,
                class unit_data **pDef_armour,
                int primary = TRUE,
                std::string *pStat = NULL);
void do_consider(class unit_data *, char *, const struct command_info *);
