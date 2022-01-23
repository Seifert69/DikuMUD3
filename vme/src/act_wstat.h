#pragma once

void do_wstat(class unit_data *, char *, const struct command_info *);
void do_wedit(class unit_data *, char *, const struct command_info *);
const char *stat_obj_data(class unit_data *u, struct obj_type_t *obj_data);
