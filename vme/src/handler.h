/*
 $Author: All $
 $RCSfile: handler.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#pragma once
#include "unitfind.h"

#include <string>

class descriptor_data *unit_is_edited(class unit_data *u);

void szonelog(class zone_type *zone, const char *fmt, ...);

class extra_descr_data *quest_add(class unit_data *ch, const char *name, const char *descr);

void insert_in_unit_list(class unit_data *u);
void remove_from_unit_list(class unit_data *unit);

class unit_fptr *find_fptr(class unit_data *u, ubit16 index);
class unit_fptr *create_fptr(class unit_data *u, ubit16 index, ubit16 priority, ubit16 beat, ubit16 flags, void *data);
void destroy_fptr(class unit_data *u, class unit_fptr *f);

void stop_following(class unit_data *ch);
void start_following(class unit_data *ch, class unit_data *leader);
void stop_snoopwrite(unit_data *unit);
void stop_fightfollow(unit_data *unit);

void modify_bright(class unit_data *unit, int bright);
void trans_set(class unit_data *u);
void trans_unset(class unit_data *u);

class unit_data *equipment(class unit_data *ch, ubit8 pos);
class unit_data *equipment_type(class unit_data *ch, int pos, ubit8 type);
void equip_char(class unit_data *ch, class unit_data *obj, ubit8 pos);

class unit_data *unequip_object(class unit_data *obj);

int unit_recursive(class unit_data *from, class unit_data *to);
class zone_type *unit_zone(const class unit_data *unit);
class unit_data *unit_room(class unit_data *unit);

/* If the transfered unit MIGHT be money, remember to pile_money() it!!!!! */
void unit_up(class unit_data *unit);
void unit_from_unit(class unit_data *unit);
void unit_down(class unit_data *unit, class unit_data *to);
void unit_to_unit(class unit_data *unit, class unit_data *to);

void extract_unit(class unit_data *unit);

void weight_change_unit(class unit_data *unit, int weight);

std::string unit_trace_up(class unit_data *unit);
void intern_unit_to_unit(class unit_data *, class unit_data *, ubit1);
void snoop(class unit_data *ch, class unit_data *victim);
void switchbody(class unit_data *ch, class unit_data *victim);
void unsnoop(class unit_data *ch, int mode);
void unswitchbody(class unit_data *npc);
