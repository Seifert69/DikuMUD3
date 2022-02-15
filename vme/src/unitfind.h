/*
 $Author: All $
 $RCSfile: unitfind.h,v $
 $Date: 2003/12/28 22:02:45 $
 $Revision: 2.3 $
 */
#pragma once

#include "essential.h"
#include "structs.h"

#include <vme.h>

#define UVI(i) (g_unit_vector.units[i])
#define UVITOP (g_unit_vector.top)

struct unit_vector_data
{
    unit_data **units;
    int size;
    int top;
};

int follower_count(unit_data *u);
unit_data *get_follower(unit_data *u, int num);
ubit1 same_surroundings(const unit_data *u1, const unit_data *u2);
void scan4_unit_room(unit_data *room, ubit8 type);
void scan4_unit(unit_data *ch, ubit8 type);
unit_data *scan4_ref(unit_data *ch, unit_data *fu);

unit_data *find_unit(const unit_data *ch, char **arg, const unit_data *list, const ubit32 bitvector);
unit_data *find_unit_dil(const unit_data *ch, char **arg, const unit_data *list, const ubit32 bitvector, ubit8 type = 15);
unit_data *find_unit_general(const unit_data *viewer,
                             const unit_data *ch,
                             char **arg,
                             const unit_data *list,
                             const ubit32 bitvector,
                             ubit8 type = FIND_UNIT);
unit_data *find_symbolic(const char *zone, const char *name);
unit_data *find_symbolic_idx(const char *zone, const char *name, int idx);
unit_data *random_unit(unit_data *ref, int sflags, int tflags);

extern unit_vector_data g_unit_vector;
