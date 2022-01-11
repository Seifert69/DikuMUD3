/*
 * $Author: tperry $
 * $RCSfile: cmdload.h,v $
 * $Date: 2001/04/29 03:46:05 $
 * $Revision: 2.1 $
 */
#pragma once

#include <vme.h>

#define DIR_CMD 1

struct cmdload_struct
{
    const char *cmd_str;
    void (*cmd_fptr)(class unit_data *ch, char *arg, const struct command_info *c);
    int inttype;
    int dir;
};

void cmd_base_load(void);
void command_read(void);

extern struct command_info *g_cmdlist;
