#pragma once

int system_check(class unit_data *pc, char *buf);
void basis_boot(void);
void execute_append(class unit_data *pc, char *str);
int log_object(struct spec_arg *sarg);
int error_rod(struct spec_arg *sarg);
int info_rod(struct spec_arg *sarg);
int admin_obj(struct spec_arg *sarg);

extern class file_index_type *g_letter_fi;
extern class unit_data *g_destroy_room;
extern class unit_data *g_entry_room;
extern unit_data *g_void_room;
