#pragma once

#include "interpreter.h"
#include "structs.h"

void do_snoop(unit_data *, char *, const command_info *);
void do_reset(unit_data *, char *, const command_info *);
void do_shutdown(unit_data *, char *, const command_info *);
void do_load(unit_data *, char *, const command_info *);
void do_at(unit_data *, char *, const command_info *);
void do_users(unit_data *, char *, const command_info *);
void do_switch(unit_data *, char *, const command_info *);
void do_timewarp(unit_data *, char *, const command_info *);
void do_crash(unit_data *, char *, const command_info *);
void do_wizlock(unit_data *, char *, const command_info *);
