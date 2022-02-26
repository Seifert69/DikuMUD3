/*
 $Author: All $
 $RCSfile: ban.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.2 $
 */
#pragma once

#include "interpreter.h"
#include "structs.h"

#include <string>

#define NO_BAN '0'
#define BAN_NEW_CHARS 'n' /* if(until == 0) ban is forever, otherwise      */
#define BAN_TOTAL 't'     /* it will be lifted when time(0) passes "until" */

char site_banned(const char *cur_site);
void show_ban_text(const char *site, descriptor_data *d);
void load_ban();
void do_ban(unit_data *, char *, const command_info *);
