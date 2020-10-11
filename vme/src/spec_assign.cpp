/* *********************************************************************** *
 * File   : spec_assign.c                             Part of Valhalla MUD *
 * Version: 1.04                                                           *
 * Author : All.                                                           *
 *                                                                         *
 * Purpose: Assignment of special functions constants.                     *
 *                                                                         *
 * Bugs   : Unknown.                                                       *
 * Status : Unpublished.                                                   *
 *                                                                         *
 * Copyright (C) Valhalla (This work is unpublished).                      *
 *                                                                         *
 * This work is a property of:                                             *
 *                                                                         *
 *        Valhalla I/S                                                     *
 *        Noerre Soegade 37A, 4th floor                                    *
 *        1370 Copenhagen K.                                               *
 *        Denmark                                                          *
 *                                                                         *
 * This is an unpublished work containing Valhalla confidential and        *
 * proprietary information. Disclosure, use or reproduction without        *
 * authorization of Valhalla is prohobited.                                *
 * *********************************************************************** */

#include <stdio.h>

#include "utils.h"
#include "interpreter.h"
#include "spec_assign.h"
#include "handler.h"

int spec_unused(struct spec_arg *sarg)
{
#ifdef VMC
#else
    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        slog(LOG_ALL, 0, "Unit %s@%s had undefined special routine.",
             UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
        destroy_fptr(sarg->owner, sarg->fptr);
    }

#endif
    return SFR_SHARE;
}

/* ********************************************************************
 *  Assignments                                                       *
 ******************************************************************** */

#ifdef VMC
#define VMCIT(a) spec_unused
#else
#define VMCIT(a) a
#endif
/* macro definitions for array indices are in values.h */
struct unit_function_array_type unit_function_array[] = {
    {"0 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"DIL Copy Internal", VMCIT(dil_direct_init), FN_PRI_CHORES, SD_NEVER, 0, 0},
    {"2 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"3 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"hunting", VMCIT(hunting), SD_NEVER, FN_PRI_CHORES, SFB_TICK, PULSE_SEC * 15},
    {"5 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"6 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"7 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"#8 XXXUnused", VMCIT(spec_unused), FN_PRI_RES , SD_NEVER, 0, 0},
    {"#9 XXX Unused", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"10 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"11 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"12 XXX Unused", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, SFB_TICK | SFB_PRIORITY, PULSE_SEC * 5}, // Was: npc_visit_room replaced with DIL
    {"13 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"14 XXX UnUsed", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"banking routine", VMCIT(bank), FN_PRI_CHORES, SD_NULL, SFB_CMD, 0},
    {"16 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"17 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"18 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"19 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"20 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"21 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"22 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"23 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"24 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"teach init", VMCIT(teach_init), FN_PRI_CHORES, SD_NEVER, SFB_TICK, PULSE_SEC},
    {"a teacher", VMCIT(teaching), FN_PRI_CHORES, SD_NEVER, SFB_CMD, PULSE_SEC},
    {"27 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"28 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"29 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"30 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Log Object", VMCIT(log_object), FN_PRI_CHORES, SD_NULL, SFB_CMD | SFB_TICK, PULSE_SEC * 5},
    {"32 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"33 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"34 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"35 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"36 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"37 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"38 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"39 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"40 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"41 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"42 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"43 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"44 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"45 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"46 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"47 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"48 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"49 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"50 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Dictionary", VMCIT(dictionary), FN_PRI_CHORES, SD_NULL, SFB_CMD | SFB_SAVE, 0},
    {"52 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"53 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"54 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"55 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"56 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"57 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"58 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"59 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"60 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"61 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"62 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"63 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"64 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"65 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"66 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Guard Guild Way", VMCIT(guard_guild_way), FN_PRI_CHORES, SD_ASCII, SFB_CMD, 0},
    {"Teach Guild Members Only", VMCIT(teach_members_only), FN_PRI_RESCUE, SD_ASCII, SFB_CMD, 0},
    {"#69 XXXUnused", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Guild Master Init", VMCIT(guild_master_init), FN_PRI_CHORES, SD_ASCII, SFB_CMD, 0},
    {"Guild Basis", VMCIT(guild_basis), FN_PRI_CHORES, SD_ASCII, SFB_DEAD | SFB_COM, 0},
    {"72 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"73 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"74 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Guild Titles", VMCIT(guild_title), FN_PRI_CHORES, SD_NULL, SFB_CMD, 0},
    {"#76 XXXUNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Give Reward", VMCIT(reward_give), FN_PRI_CHORES, SD_NEVER, SFB_CMD, 0},
    {"78 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Error file clearing", VMCIT(error_rod), FN_PRI_CHORES, SD_ASCII, SFB_CMD, 0},
    {"User info file clearing", VMCIT(info_rod), FN_PRI_CHORES, SD_ASCII, SFB_CMD, 0},
    {"81 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"DIL", VMCIT(run_dil), FN_PRI_CHORES, SD_NULL, SFB_TICK | SFB_CMD, PULSE_SEC * 1},
    {"Oracle", VMCIT(oracle), FN_PRI_RES, SD_NEVER, SFB_CMD, 0},
    {"Administrator", VMCIT(admin_obj), FN_PRI_CHORES, SD_NULL, SFB_CMD, 0},
    {"85 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Crystal Ball", VMCIT(ball), FN_PRI_CHORES, SD_NULL, SFB_CMD, 0},
    {"Administrator Slime", VMCIT(slime_obj), FN_PRI_CHORES, SD_NULL, SFB_CMD, 0},
    {"88 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"89 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"Guild Master Internal", VMCIT(guild_master), FN_PRI_RES, SD_NEVER, SFB_CMD, 0},
    {"91 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"92 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"93 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"94 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"95 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"96 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"97 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"98 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"99 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"100 XXX UNUSED", VMCIT(spec_unused), FN_PRI_RES, SD_NEVER, 0, 0},
    {"DIL Init", VMCIT(dil_init), FN_PRI_CHORES, SD_ASCII, SFB_ALL, PULSE_SEC * 1}};
