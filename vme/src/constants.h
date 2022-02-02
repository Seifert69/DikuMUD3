/*
 $Author: tperry $
 $RCSfile: constants.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "skills.h"
#include "values.h"

extern const skill_interval g_how_good[];
extern const skill_interval g_weapon_skills[];
extern const skill_interval g_spell_skills[];
extern const skill_interval g_skill_skills[];

extern const char *g_char_pos[];
extern const char *g_enter_dirs[];
extern const int g_rev_dir[];
extern const char *g_room_landscape[];
extern const int g_movement_loss[];
extern const char *g_weekdays[];
extern const char *g_month_name[];
extern const sbit8 g_time_light[SUN_SET + 1];
extern const char *g_fullness[];
extern const char *g_arm_text[];
extern const char *g_shi_text[];
extern const char *g_bodyparts[];
extern const ubit8 g_bodyparts_cover[31][5];
extern const char *g_where[];
extern const char *g_npc_class_types[];
extern const char *g_sfb_flags[];
extern const char *g_unit_manipulate[];
extern const char *g_unit_open_flags[];
extern const char *g_unit_flags[];
extern const char *g_obj_flags[];
extern const char *g_room_flags[];
extern const char *g_char_flags[];
extern const char *g_pc_flags[];
extern const char *g_npc_flags[];
extern const char *g_unit_status[];
extern const char *g_char_sex[];
extern const char *g_obj_types[];
extern const char *g_equip_pos[];
extern const char *g_obj_pos[];
extern const char *g_connected_types[];
