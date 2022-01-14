#pragma once

#include "essential.h"

char *ContentsFileName(const char *);
class unit_data *restore_all_unit(char *filename, unit_data *udest);
int diff(char *ref, ubit32 reflen, char *obj, int objlen, char *dif, int diflen, ubit32 crc);
int patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res, int reslen, ubit32 crc);
ubit32 rent_calc(class unit_data *ch, time_t savetime);
void reception_boot(void);
int save_contents(const char *pFileName, class unit_data *unit, int fast, int bContainer);
void store_all_unit(class unit_data *u, char *fname, int svcont);
void do_rent(class unit_data *, char *, const struct command_info *);
void load_contents(const char *pFileName, class unit_data *unit);

extern class file_index_type *g_slime_fi;
