#pragma once

#include "essential.h"

int _parse_name(const char *arg, char *name);
void check_idle(void);
void enter_game(class unit_data *ch, int dilway = FALSE);
void nanny_close(class descriptor_data *d, char *arg);
void nanny_get_name(class descriptor_data *d, char *arg);
void nanny_menu(class descriptor_data *d, char *arg);
void nanny_new_pwd(class descriptor_data *d, char *arg);
void reset_char(class unit_data *ch);
void set_descriptor_fptr(class descriptor_data *d, void (*fptr)(class descriptor_data *, char *), ubit1 call);
void interpreter_string_add(class descriptor_data *d, char *str);

extern int g_dilmenu;
