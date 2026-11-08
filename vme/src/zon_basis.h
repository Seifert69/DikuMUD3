#pragma once

#include "interpreter.h"

void basis_boot();
int log_object(spec_arg *sarg);
int error_rod(spec_arg *sarg);
int info_rod(spec_arg *sarg);
int admin_obj(spec_arg *sarg);

extern file_index_type *g_letter_fi;
extern unit_data *g_destroy_room;
extern unit_data *g_entry_room;
extern unit_data *g_void_room;
