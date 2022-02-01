#pragma once

#include "essential.h"
#include "interpreter.h"
#include "structs.h"

const char *ContentsFileName(const char *);
unit_data *restore_all_unit(char *filename, unit_data *udest);
int diff(char *ref, ubit32 reflen, char *obj, int objlen, char *dif, int diflen, ubit32 crc);
int patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res, int reslen, ubit32 crc);
ubit32 rent_calc(unit_data *ch, time_t savetime);
void reception_boot();
int save_contents(const char *pFileName, unit_data *unit, int fast, int bContainer);
void store_all_unit(unit_data *u, char *fname, int svcont);
void do_rent(unit_data *, char *, const command_info *);
void load_contents(const char *pFileName, unit_data *unit);

extern file_index_type *g_slime_fi;
