/*
 $Author: All $
 $RCSfile: handler.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#pragma once
#include "structs.h"
#include "unitfind.h"
#include "zone_type.h"

#include <string>

descriptor_data *unit_is_edited(unit_data *u);

extra_descr_data *quest_add(unit_data *ch, const char *name, const char *descr);

void insert_in_unit_list(unit_data *u);
void remove_from_unit_list(unit_data *unit);

unit_fptr *find_fptr(unit_data *u, ubit16 index);
unit_fptr *create_fptr(unit_data *u, ubit16 index, ubit16 priority, ubit16 beat, ubit16 flags, void *data);
void destroy_fptr(unit_data *u, unit_fptr *f);

void stop_following(unit_data *ch);
void start_following(unit_data *ch, unit_data *leader);
void stop_snoopwrite(unit_data *unit);
void stop_fightfollow(unit_data *unit);

void modify_bright(unit_data *unit, int bright);
void trans_set(unit_data *u);
void trans_unset(unit_data *u);

unit_data *equipment(unit_data *ch, ubit8 pos);
unit_data *equipment_type(unit_data *ch, int pos, ubit8 type);
void equip_char(unit_data *ch, unit_data *obj, ubit8 pos);

unit_data *unequip_object(unit_data *obj);

int unit_recursive(unit_data *from, unit_data *to);
zone_type *unit_zone(const unit_data *unit);
unit_data *unit_room(unit_data *unit);

/* If the transfered unit MIGHT be money, remember to pile_money() it!!!!! */
void unit_up(unit_data *unit);
void unit_from_unit(unit_data *unit);
void unit_down(unit_data *unit, unit_data *to);
void unit_to_unit(unit_data *unit, unit_data *to);

void extract_unit(unit_data *unit);

void weight_change_unit(unit_data *unit, int weight);

std::string unit_trace_up(unit_data *unit);
void intern_unit_to_unit(unit_data *, unit_data *, ubit1);
void snoop(unit_data *ch, unit_data *victim);
void switchbody(unit_data *ch, unit_data *victim);
void unsnoop(unit_data *ch, int mode);
void unswitchbody(unit_data *npc);
