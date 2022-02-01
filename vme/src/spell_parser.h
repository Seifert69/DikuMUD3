#pragma once

#include "spells.h"

#include <vme.h>

void boot_spell();
void do_cast(unit_data *, char *, const command_info *);
void set_spellargs(spell_args *sa, unit_data *caster, unit_data *medium, unit_data *target, const char *arg, int hm);

/* Spell externs */
extern spell_info_type g_spell_info[SPL_TREE_MAX];
