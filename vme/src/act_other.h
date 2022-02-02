#pragma once

#include "interpreter.h"
#include "structs.h"

void race_adjust(unit_data *);
void start_player(unit_data *ch);
void do_save(unit_data *, char *, const command_info *);
