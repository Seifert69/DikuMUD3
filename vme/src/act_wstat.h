#pragma once

#include "interpreter.h"
#include "structs.h"
#include "utility.h"

void do_wstat(unit_data *, char *, const command_info *);
void do_wedit(unit_data *, char *, const command_info *);
const char *stat_obj_data(unit_data *u, obj_type_t *obj_data);
