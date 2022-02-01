#pragma once

#include "interpreter.h"
#include "structs.h"

int is_slimed(file_index_type *sp);
void slime_boot();
int slime_obj(spec_arg *sarg);
