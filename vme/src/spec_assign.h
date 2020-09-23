/*
 $Author: tperry $
 $RCSfile: spec_assign.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */
#ifndef _MUD_SPEC_ASS_H
#define _MUD_SPEC_ASS_H

#include "interpreter.h"

int dil_init(struct spec_arg *sarg);
int dil_direct_init(struct spec_arg *sarg);

int hunting(struct spec_arg *sarg);

int bank(struct spec_arg *sarg);

int dictionary(struct spec_arg *sarg);

int guard_way_level(struct spec_arg *sarg);

/*guild variables*/
int guard_guild_way(struct spec_arg *sarg);
int teach_init(struct spec_arg *sarg);
int teaching(struct spec_arg *sarg);
int teach_members_only(struct spec_arg *sarg);
int guild_master(struct spec_arg *sarg);
int guild_basis(struct spec_arg *sarg);
int guild_title(struct spec_arg *sarg);
int guild_master_init(struct spec_arg *sarg);

int run_dil(struct spec_arg *sarg);
int oracle(struct spec_arg *sarg);

/*admin variables*/
int log_object(struct spec_arg *sarg);
int error_rod(struct spec_arg *sarg);
int info_rod(struct spec_arg *sarg);
int admin_obj(struct spec_arg *sarg);
int ball(struct spec_arg *sarg);
int slime_obj(struct spec_arg *sarg);

/*justice variables*/
int accuse(struct spec_arg *sarg);
int protect_lawful(struct spec_arg *sarg);
int npc_visit_room(struct spec_arg *sarg);
int justice_scales(struct spec_arg *sarg);
int whistle(struct spec_arg *sarg);
int reward_board(struct spec_arg *sarg);
int reward_give(struct spec_arg *sarg);

#endif
