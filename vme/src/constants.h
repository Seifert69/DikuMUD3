/*
 $Author: tperry $
 $RCSfile: constants.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */

#ifndef _MUD_CONSTANTS_H
#define _MUD_CONSTANTS_H

#include "values.h"
#include "skills.h"

extern const struct skill_interval how_good[];
extern const struct skill_interval weapon_skills[];
extern const struct skill_interval spell_skills[];
extern const struct skill_interval skill_skills[];

extern const char *char_pos[];
extern const char *enter_dirs[];
extern const int rev_dir[];
extern const char *room_landscape[];
extern const int movement_loss[];
extern const char *weekdays[];
extern const char *month_name[];
extern const sbit8 time_light[SUN_SET + 1];
extern const char *color_liquid[];
// extern const int drink_aff[LIQ_MAX+2][3];
extern const char *fullness[];
extern const char *arm_text[];
extern const char *shi_text[];
extern const char *bodyparts[];
extern const ubit8 bodyparts_cover[31][5];
extern const char *where[];
extern const char *npc_class_types[];
extern const char *sfb_flags[];
extern const char *unit_manipulate[];
extern const char *unit_open_flags[];
extern const char *unit_flags[];
extern const char *obj_flags[];
extern const char *room_flags[];
extern const char *char_flags[];
extern const char *pc_flags[];
extern const char *npc_flags[];
extern const char *unit_status[];
extern const char *char_sex[];
extern const char *obj_types[];
extern const char *equip_pos[];
extern const char *obj_pos[];
extern const char *connected_types[];

#endif
