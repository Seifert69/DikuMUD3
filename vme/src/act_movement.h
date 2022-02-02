#pragma once

#include "interpreter.h"
#include "structs.h"

const char *single_unit_messg(unit_data *unit, const char *type, const char *pSubStr, const char *mesg);
void do_move(unit_data *, char *, const command_info *);
