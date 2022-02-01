#pragma once

#include "essential.h"
#include "interpreter.h"
#include "structs.h"

#include <string>

int experience_modification(unit_data *att, unit_data *def);
int spell_bonus(unit_data *att,
                unit_data *medium,
                unit_data *def,
                int hit_loc,
                int spell_number,
                int *pDef_armour_type,
                unit_data **pDef_armour,
                std::string *pStat);
int shield_bonus(unit_data *att, unit_data *def, unit_data **pDef_shield);
int melee_bonus(unit_data *att,
                unit_data *def,
                int hit_loc,
                int *pAtt_weapon_type,
                unit_data **pAtt_weapon,
                int *pDef_armour_type,
                unit_data **pDef_armour,
                int primary = TRUE,
                std::string *pStat = nullptr);
void do_consider(unit_data *, char *, const command_info *);
