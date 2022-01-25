#pragma once

#include <vme.h>

void boot_spell(void);
void do_cast(class unit_data *, char *, const struct command_info *);
void set_spellargs(struct spell_args *sa,
                   class unit_data *caster,
                   class unit_data *medium,
                   class unit_data *target,
                   const char *arg,
                   int hm);

/* Spell externs */
extern struct spell_info_type g_spell_info[SPL_TREE_MAX];
