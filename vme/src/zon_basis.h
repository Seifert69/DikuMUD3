#pragma once

#include "interpreter.h"
#include "structs.h"

int system_check(unit_data *pc, const char *buf);
void basis_boot();
void execute_append(unit_data *pc, const char *str);
int log_object(spec_arg *sarg);
int error_rod(spec_arg *sarg);
int info_rod(spec_arg *sarg);
int admin_obj(spec_arg *sarg);

extern file_index_type *g_letter_fi;
extern unit_data *g_destroy_room;
extern unit_data *g_entry_room;
extern unit_data *g_void_room;
