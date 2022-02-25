/*
 $Author: tperry $
 $RCSfile: guild.h,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */
#pragma once

#include "interpreter.h"
#include "structs.h"

int char_guild_level(unit_data *ch);
int char_guild_level(unit_data *ch, const char *pGuildName);
