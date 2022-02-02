#pragma once

#include "essential.h"
#include "structs.h"

int _parse_name(const char *arg, char *name);
void check_idle();
void enter_game(unit_data *ch, int dilway = FALSE);
void nanny_close(descriptor_data *d, char *arg);
void nanny_get_name(descriptor_data *d, char *arg);
void nanny_menu(descriptor_data *d, char *arg);
void nanny_new_pwd(descriptor_data *d, char *arg);
void reset_char(unit_data *ch);
void set_descriptor_fptr(descriptor_data *d, void (*fptr)(descriptor_data *, char *), ubit1 call);
void interpreter_string_add(descriptor_data *d, char *str);

extern int g_dilmenu;
