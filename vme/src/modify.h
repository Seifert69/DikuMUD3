/*
 $Author: All $
 $RCSfile: modify.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.2 $
 */
#pragma once

void edit_extra(class descriptor_data *d);
void do_set(class unit_data *, char *, const struct command_info *);
void do_setskill(class unit_data *, char *, const struct command_info *);

extern class skill_collection g_AbiColl;
extern class skill_collection g_WpnColl;
extern class skill_collection g_SkiColl;
extern class skill_collection g_SplColl;
