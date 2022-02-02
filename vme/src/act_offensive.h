#pragma once

#include "interpreter.h"
#include "structs.h"

void do_kill(unit_data *, char *, const command_info *);
void do_execute(unit_data *, char *, const command_info *);
void do_hit(unit_data *, char *, const command_info *);
