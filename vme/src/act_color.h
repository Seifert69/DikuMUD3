#pragma once

#include "interpreter.h"

int is_forground(char *cstr);
int is_background(char *cstr);
void do_color(unit_data *, char *, const command_info *);
