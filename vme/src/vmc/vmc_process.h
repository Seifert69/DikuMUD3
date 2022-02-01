#pragma once

#include "structs.h"

void dmc_error(int fatal, const char *fmt, ...);
void init_unit(unit_data *u);
void process_unit(unit_data *u);
