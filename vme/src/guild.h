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
void advance_guild_level(unit_data *ch);
int guard_guild_way(spec_arg *sarg);
int teach_members_only(spec_arg *sarg);
int guild_master_init(spec_arg *sarg);
int guild_basis(spec_arg *sarg);
int guild_title(spec_arg *sarg);
int guild_master(spec_arg *sarg);
