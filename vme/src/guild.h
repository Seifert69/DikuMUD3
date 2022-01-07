/*
 $Author: tperry $
 $RCSfile: guild.h,v $
 $Date: 2001/04/29 03:46:06 $
 $Revision: 2.1 $
 */

#ifndef _MUD_GUILD_H
#define _MUD_GUILD_H

int char_guild_level(class unit_data *ch);
void advance_guild_level(class unit_data *ch);

#endif
