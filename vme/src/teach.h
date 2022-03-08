#pragma once

#include "interpreter.h"

void clear_training_level(unit_data *ch);
int teach_init(spec_arg *sarg);
int teaching(spec_arg *sarg);
int actual_cost(int cost, sbit8 racemodifier, int level, int virtual_level);
