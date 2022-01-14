#pragma once

void do_snoop(class unit_data *, char *, const struct command_info *);
void do_reset(class unit_data *, char *, const struct command_info *);
void do_shutdown(class unit_data *, char *, const struct command_info *);
void do_load(class unit_data *, char *, const struct command_info *);
void do_at(class unit_data *, char *, const struct command_info *);
void do_users(class unit_data *, char *, const struct command_info *);
void do_switch(class unit_data *, char *, const struct command_info *);
void do_timewarp(class unit_data *, char *, const struct command_info *);
void do_crash(class unit_data *, char *, const struct command_info *);
void do_wizlock(class unit_data *, char *, const struct command_info *);
