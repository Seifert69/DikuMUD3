/*
 $Author: All $
 $RCSfile: modify.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.2 $
 */
#pragma once

#include "interpreter.h"
#include "skills.h"
#include "structs.h"

void edit_extra(descriptor_data *d);
void do_set(unit_data *, char *, const command_info *);
void do_setskill(unit_data *, char *, const command_info *);

extern skill_collection g_AbiColl;
extern skill_collection g_WpnColl;
extern skill_collection g_SkiColl;
extern skill_collection g_SplColl;
