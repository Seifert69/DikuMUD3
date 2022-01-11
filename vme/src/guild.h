/*
 $Author: tperry $
 $RCSfile: guild.h,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */
#pragma once

int char_guild_level(class unit_data *ch);
void advance_guild_level(class unit_data *ch);
int guard_guild_way(struct spec_arg *sarg);
int teach_members_only(struct spec_arg *sarg);
int guild_master_init(struct spec_arg *sarg);
int guild_basis(struct spec_arg *sarg);
int guild_title(struct spec_arg *sarg);
int guild_master(struct spec_arg *sarg);
