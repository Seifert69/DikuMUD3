/*
 $Author: All $
 $RCSfile: handler.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */

#ifndef _MUD_HANDLER_H
#define _MUD_HANDLER_H

#include "unitfind.h"

class descriptor_data *unit_is_edited (class unit_data * u);

void unit_messg (class unit_data * ch, class unit_data * unit,
                 const char *type, const char *mesg_s, const char *mesg_o);

const char *single_unit_messg (class unit_data * unit,
                               const char *type,
                               const char *pSubStr, const char *mesg);

void szonelog (class zone_type *zone, const char *fmt, ...);

/* From pcsave.c - I'm just tired of specifying them everywhere */
void save_player (class unit_data * pc);
class unit_data *load_player (const char *pName);
void load_contents (const char *pFileName, class unit_data * unit);
void save_player_contents (class unit_data * pc, int fast);


/* handler.c */
class extra_descr_data *quest_add (class unit_data * ch,
                                    const char *name, const char *descr);

void insert_in_unit_list (class unit_data * u);
void remove_from_unit_list (class unit_data * unit);

class unit_fptr *find_fptr (class unit_data * u, ubit16 index);
class unit_fptr *create_fptr(class unit_data *u, ubit16 index, ubit16 priority,
                              ubit16 beat, ubit16 flags, void *data);
void destroy_fptr (class unit_data * u, class unit_fptr *f);

void stop_following (class unit_data * ch);
void start_following (class unit_data * ch, class unit_data * leader);

void modify_bright (class unit_data * unit, int bright);
void trans_set (class unit_data * u);
void trans_unset (class unit_data * u);

class unit_data *equipment (class unit_data * ch, ubit8 pos);
class unit_data *equipment_type (class unit_data * ch, int pos, ubit8 type);
void equip_char (class unit_data * ch, class unit_data * obj, ubit8 pos);

class unit_data *unequip_object (class unit_data * obj);
void recalc_dex_red (class unit_data * ch);

int unit_recursive (class unit_data * from, class unit_data * to);
class zone_type *unit_zone (const class unit_data * unit);
class unit_data *unit_room (class unit_data * unit);

/* If the transfered unit MIGHT be money, remember to pile_money() it!!!!! */
void unit_up (class unit_data * unit);
void unit_from_unit (class unit_data * unit);
void unit_down (class unit_data * unit, class unit_data * to);
void unit_to_unit (class unit_data * unit, class unit_data * to);

void extract_unit (class unit_data * unit);

void weight_change_unit (class unit_data * unit, int weight);

class unit_data *find_unit_in_list_num (int num, class unit_data * list);
class unit_data *find_unit_num (int num);

#endif /* _MUD_HANDLER_H */
