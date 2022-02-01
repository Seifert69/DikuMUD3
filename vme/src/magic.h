/*
 $Author: tperry $
 $RCSfile: magic.h,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.3 $
 */
#pragma once

#include "essential.h"
#include "spells.h"
#include "structs.h"

/* #define MAGIC_POWER(ch)  \
   MAX(CHAR_MAG(ch), CHAR_DIV(ch)) */

int dil_effect(char *pStr, spell_args *sa);
int object_power(unit_data *unit);
ubit1 use_mana(unit_data *medium, int mana);
ubit1 cast_magic_now(unit_data *ch, int mana);

ubit1 may_teleport_away(unit_data *unit);
ubit1 may_teleport_to(unit_data *unit, unit_data *dest);
ubit1 may_teleport(unit_data *unit, unit_data *dest);

int spell_cast_check(unit_data *att, int spell);
int spell_resistance(unit_data *att, unit_data *def, int spell);
int spell_ability(unit_data *u, int ability, int spell);
int spell_attack_ability(unit_data *medium, int spell);
int spell_attack_skill(unit_data *unit, int spell);
int spell_defense_skill(unit_data *unit, int spell);

int spell_offensive(spell_args *sa, int spell_number, int bonus);

int variation(int n, int d, int u);
/*last two internal spells*/
void spell_clear_skies(spell_args *sa);
void spell_storm_call(spell_args *sa);
