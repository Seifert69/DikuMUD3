/*
 $Author: tperry $
 $RCSfile: magic.h,v $
 $Date: 2003/01/02 23:39:20 $
 $Revision: 2.3 $
 */

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "spells.h"
#include "handler.h"
#include "vmelimits.h"
#include "affect.h"

#include "db.h"
#include "interpreter.h"
#include "utility.h"
#include "common.h"
#include "fight.h"

#ifndef _MUD_MAGIC_H
#define _MUD_MAGIC_H

/* #define MAGIC_POWER(ch)  \
   MAX(CHAR_MAG(ch), CHAR_DIV(ch)) */

int dil_effect(char *pStr, struct spell_args *sa);
int object_power(class unit_data *unit);
ubit1 use_mana(class unit_data *medium, int mana);
ubit1 cast_magic_now(class unit_data *ch, int mana);

ubit1 may_teleport_away(class unit_data *unit);
ubit1 may_teleport_to(class unit_data *unit, class unit_data *dest);
ubit1 may_teleport(class unit_data *unit, class unit_data *dest);

int spell_cast_check(class unit_data *att, int spell);
int spell_resistance(class unit_data *att, class unit_data *def,
                     int spell);
int spell_ability(class unit_data *u, int ability, int spell);
int spell_attack_ability(class unit_data *medium, int spell);
int spell_attack_skill(class unit_data *unit, int spell);
int spell_defense_skill(class unit_data *unit, int spell);

int spell_offensive(struct spell_args *sa, int spell_number, int bonus);

int variation(int n, int d, int u);
/*last two internal spells*/
void spell_clear_skies(struct spell_args *sa);
void spell_storm_call(struct spell_args *sa);

#endif /* _MUD_MAGIC_H */
