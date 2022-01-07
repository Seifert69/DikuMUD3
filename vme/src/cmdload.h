/*
 * $Author: tperry $
 * $RCSfile: cmdload.h,v $
 * $Date: 2001/04/29 03:46:05 $
 * $Revision: 2.1 $
 */

#ifndef _MUD_CMDLOAD_H
#define _MUD_CMDLOAD_H
#include "vme.h"

#define DIR_CMD 1

extern const char *g_pc_races[PC_RACE_MAX + 1];
struct cmdload_struct
{
    const char *cmd_str;
    void (*cmd_fptr)(class unit_data *ch, char *arg, const struct command_info *c);
    int inttype;
    int dir;
};

void cmd_base_load(void);
void command_read(void);
#endif
