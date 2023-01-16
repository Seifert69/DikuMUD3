/* *********************************************************************** *
 * File   : act_movement.c                            Part of Valhalla MUD *
 * Version: 1.60                                                           *
 * Author : All                                                            *
 *                                                                         *
 * Purpose: Routines for moving, entering, leaving, open, close,           *
 *          positions and follow.                                          *
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

/* 23/07/92 seifert: Fixed serious simple-move bug, now tests for != SHARE */
/* 26/07/92 seifert: Split find_door into two. For use with sfun_door..    */
/* 29/07/92 seifert: Fixed bug in simple_move                              */
/* 24/09/92 gnort  : do_follow no longer assumpts char picks char as leader*/
/* 30/09/92 seifert: Various additions and bug fixes                       */
/* 02/10/92 gnort  : do_sail, in boat/obj check && changed to ||           */
/* 07/10/92 gnort  : Various additions and bug fixes                       */
/* 17/11/92 HHS    : Reinstated endurance cost                             */
/* 17/11/92 HHS    : Added simle ride func. (like sail, does nothing yet)  */
/* 23/08/93 jubal  : Fixed messages in do_sail                             */
/* 23/08/93 jubal  : Added messages to leader when start/stop follow       */
/* 23/08/93 jubal  : Fixed (nearly - still acttrouble) msgs around open etc*/
// 15-Jul-2022  sei: Removed do_follow C source since it's a DIL command

#include "char_follow_type.h"
#include "comm.h"
#include "common.h"
#include "constants.h"
#include "dilsup.h"
#include "fight.h"
#include "handler.h"
#include "interpreter.h"
#include "movement.h"
#include "skills.h"
#include "slog.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utils.h"
#include "vmelimits.h"

#include <cstdio>
#include <cstring>

// I had to add a act() kludge here.
// The $arrive_ and $leave_ extras depend on $2t for special descriptions.
// However, many act() in movement require 3 units, so therefore $2t in extras is
// string substituted in this procedure
//
const char *single_unit_messg(unit_data *unit, const char *type, int direction, char *mesg)
{
    extra_descr_data *exd = unit->getExtraList().m_pList;

    if (exd)
    {
        exd = exd->find_raw(type);
    }

    if (exd && exd->descr.c_str())
    {
        if (is_in(direction, 0, MAX_EXIT))
        {
            if ((exd->names.Name(1) == nullptr) || str_cstr(exd->names.Name(1), g_dirs_short[direction]))
            {
                strcpy(mesg, exd->descr.c_str());
                str_substitute("$2t", g_dirs[direction], mesg);
            }
        }
        else
        {
            slog(LOG_ALL, 0, "Illegal direction %d", direction);
        }
    }

    return mesg;
}

/**
 * Has 'pc' found the door at 'dir'? If direction exits and it is closed
 * and hidden then the door is found if it has been searched for.
 */
int has_found_door(unit_data *pc, int dir)
{
    extra_descr_data *exd = nullptr;
    unit_affected_type *af = nullptr;
    char buf[MAX_INPUT_LENGTH];

    if (!pc->getUnitIn()->isRoom())
    {
        return FALSE;
    }

    if (!ROOM_EXIT(pc->getUnitIn(), dir))
    {
        return FALSE;
    }

    if (!pc->isPC())
    {
        return TRUE;
    }

    if (!ROOM_EXIT(pc->getUnitIn(), dir)->isDoorFlagSet(EX_HIDDEN))
    {
        return TRUE;
    }

    strcpy(buf, SECRET_DOOR);
    strcat(buf, itoa(dir));

    exd = pc->getUnitIn()->getExtraList().find_raw(buf);

    if (exd == nullptr)
    {
        return FALSE;
    }

    return exd->names.IsName(pc->getNames().Name(0)) != nullptr;

    if (ROOM_EXIT(pc->getUnitIn(), dir)->isDoorFlagSet(EX_CLOSED))
    {
        for (af = pc->getUnitIn()->getUnitAffected(); af; af = af->getNext())
        {
            if (af->getID() == ID_SPOTTED_SECRET && PC_ID(pc) == af->getDataAtIndex(0))
            {
                return TRUE;
            }
        }
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#define ALAS_NOWAY "Alas, you cannot go that way...<br/>"
/**
 * Other logic has figured out if there was a direction, if it was open, if you're sleeping, mesgs, etc.
 * This is the actual move between two rooms. Direction only needed for command_info event.
 * Endurance is calculated and subtracted here
 * @param ch is the CHAR doing the move command.
 * @param mover is the vehicle, steed or boat or the char itself
 * @param room_from room moving from
 * @param room_to room moving to
 * @param bIsFollower
 * @param direction the direction the move is in, e.g. DIR_EAST
 * @param pLeaveSelf
 * @param pLeaveOther
 * @param pArrSelf
 * @param pArrOther
 * @param pPassengersO
 * @returns 1 = success,<br>
 * 0 = fail,<br>
 * -1 = dead
 */
int room_move(unit_data *ch,
              unit_data *mover,
              unit_data *room_from,
              unit_data *room_to,
              int bIsFollower,
              int direction,
              const char *pLeaveSelf,
              const char *pLeaveOther,
              const char *pArrSelf,
              const char *pArrOther,
              const char *pPassengersO)
{
    int res = 0;
    unit_data *u = nullptr;

    room_from = ch->inRoom();

    assert(room_from->isRoom());
    assert(room_to->isRoom());

    /* If we are not following, then we already issued the special
       in command interpreter! */
    if (bIsFollower)
    {
        char buf[MAX_INPUT_LENGTH];
        strcpy(buf, "");
        res = send_preprocess(mover, g_cmd_dirs[direction], buf);
        if (ch->is_destructed())
        {
            return -1;
        }
    }
    else
    {
        res = SFR_SHARE;
    }

    if ((res != SFR_SHARE) || (room_from != ch->inRoom()))
    {
        return 0;
    }

    if (!pay_point_charlie(ch, room_to))
    {
        return 0;
    }

    for (u = ch->getUnitContains(); u; u = u->getNext())
    {
        if (!pay_point_charlie(u, room_to))
        {
            return 0;
        }
    }

    if (!str_is_empty(pLeaveSelf))
    {
        act(pLeaveSelf, eA_ALWAYS, ch, room_from, mover, eTO_CHAR);
    }

    send_done(ch, room_from, room_to, direction, g_cmd_dirs[direction], "");

    if (ch->is_destructed() || mover->is_destructed() || (mover->inRoom() != room_from))
    {
        // send_done changed something, abort.
        return 0;
    }

    if (room_from->getUnitContains() && !str_is_empty(pLeaveOther))
    {
        if ((mover != ch) || !CHAR_HAS_FLAG(ch, CHAR_SNEAK))
        {
            act(pLeaveOther, eA_HIDEINV, ch, ch, mover, eTO_REST);
        }
    }

    unit_from_unit(mover);
    // These two volatile flags are removed when you move.
    UCHAR(ch)->removeCharacterFlag(CHAR_LEGAL_TARGET | CHAR_SELF_DEFENCE);
    unit_to_unit(mover, room_to);

    if (room_to->getUnitContains() && !str_is_empty(pArrOther))
    {
        if ((mover != ch) || !CHAR_HAS_FLAG(ch, CHAR_SNEAK))
        {
            act(pArrOther, eA_HIDEINV, ch, ch, mover, eTO_REST);
        }
    }

    if (!str_is_empty(pArrSelf))
    {
        act(pArrSelf, eA_ALWAYS, ch, room_to, mover, eTO_CHAR);
    }

    command_interpreter(ch, "look :brief:");

    if (ch != mover)
    {
        if (mover->isChar())
        {
            command_interpreter(mover, "look :brief:");
        }

        for (u = mover->getUnitContains(); u; u = u->getNext())
        {
            if ((u != ch) && u->isChar())
            {
                act(pPassengersO, eA_SOMEONE, u, ch, mover, eTO_CHAR);
                command_interpreter(u, "look :brief:");
            }
        }
    }

    send_done(ch, mover, room_from, direction, &g_cmd_auto_enter, "");
    return 1;
}

#define S_SWIM_ON "$swim_on"

#define S_IS_FISH "$is_fish"

#define S_IS_AMPHIB "$is_amphib"

/**
 *
 * Asserts:<br>
 *      0. ch is in a room, going in a direction n,e,s,w,u or d
 *      1. Does not assert anything about position.
 *      2. That the direction exists.
 *      3. Tests if the door is closed (to make follow work correctly
 *         after special is called!)
 *
 *      Followers won't follow.
 * Following indicates if this is a follower moving because it is following another char.
 *
 * @param ch
 * @param mover
 * @param direction
 * @param following
 * @returns 1 : If success.<br>
 *          0 : If fail<br>
 *          -1 : If dead.<br>
 */
int generic_move(unit_data *ch, unit_data *mover, int direction, int following)
{
    unit_data *room_from = nullptr;
    unit_data *room_to = nullptr;
    char aLeaveSelf[MAX_STRING_LENGTH];
    char aLeaveOther[MAX_STRING_LENGTH];
    char aArrSelf[MAX_STRING_LENGTH];
    char aArrOther[MAX_STRING_LENGTH];
    char aPassengersOther[MAX_STRING_LENGTH];
    const char *lo = nullptr;
    const char *ls = nullptr;
    const char *as = nullptr;
    const char *ao = nullptr;
    aLeaveOther[0] = 0;
    aLeaveSelf[0] = 0;
    aArrOther[0] = 0;
    aArrSelf[0] = 0;
    aPassengersOther[0] = 0;

    ls = aLeaveSelf;
    lo = aLeaveOther;
    ao = aArrOther;
    as = aArrSelf;

    room_from = ch->inRoom();

    assert((ch == mover) || (ch->getUnitIn() == mover));

    if (!mover->getUnitIn()->isRoom())
    {
        send_to_char(ALAS_NOWAY, ch);
        return 0;
    }

    /* Exit exists? */
    if (!ROOM_EXIT(room_from, direction))
    {
        send_to_char(ALAS_NOWAY, ch);
        return 0;
    }

    /* Next room exists? */
    if (ROOM_EXIT(room_from, direction)->getToRoom() == nullptr)
    {
        send_to_char(ALAS_NOWAY, ch);
        return 0;
    }

    room_to = ROOM_EXIT(room_from, direction)->getToRoom();

    if (CHAR_POS(ch) == POSITION_FIGHTING)
    {
        send_to_char("You are fighting for your life!<br/>", ch);
        return 0;
    }

    if (ch == mover) // Old fashioned walking on own legs
    {
        if (CHAR_POS(ch) < POSITION_STANDING)
        {
            if (CHAR_POS(ch) >= POSITION_RESTING)
            {
                send_to_char("Perhaps you should get on your feet first?<br/>", mover);
            }
            return 0;
        }

        snprintf(aLeaveOther, sizeof(aLeaveOther), "$2n leaves %s.", g_dirs[direction]);
        ls = single_unit_messg(room_from, "$leave_s", direction, aLeaveSelf);
        lo = single_unit_messg(room_from, "$leave_o", direction, aLeaveOther);

        snprintf(aArrOther, sizeof(aArrOther), "$2n has arrived from %s.", g_enter_dirs[g_rev_dir[direction]]);
        as = single_unit_messg(room_to, "$arrive_s", g_rev_dir[direction], aArrSelf);
        ao = single_unit_messg(room_to, "$arrive_o", g_rev_dir[direction], aArrOther);
    }
    else // Steed or boat code (shares scan for passenger combat)
    {
        for (unit_data *u = mover->getUnitContains(); u; u = u->getNext())
        {
            if (u->isChar() && CHAR_FIGHTING(u))
            {
                act("You can't get away like that in the middle of combat.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
                act("You can't get away like that in the middle of combat.", eA_ALWAYS, mover, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }
        }

        if (mover->isChar()) // Mounted on a steed
        {
            snprintf(aLeaveSelf, sizeof(aLeaveSelf), "You ride your $3N %s.", g_dirs[direction]);
            snprintf(aLeaveOther, sizeof(aLeaveOther), "$2n rides $2s $3N %s.", g_dirs[direction]);
            snprintf(aArrOther, sizeof(aArrOther), "$2n rides $2s $3N in from %s.", g_enter_dirs[g_rev_dir[direction]]);
            snprintf(aPassengersOther, sizeof(aPassengersOther), "$2n rides $2s $3N %s.", g_dirs[direction]);

            if (CHAR_POS(mover) < POSITION_STANDING)
            {
                if (CHAR_POS(mover) == POSITION_FIGHTING)
                {
                    send_to_char("You are fighting for your life!<br/>", mover);
                    if (ch != mover)
                    {
                        act("Your $3n is busy fighting!", eA_ALWAYS, ch, cActParameter(), mover, eTO_CHAR);
                    }
                }
                else if (CHAR_POS(mover) >= POSITION_RESTING)
                {
                    send_to_char("Perhaps you should get on your feet first?<br/>", mover);
                    if (ch != mover)
                    {
                        act("Your $3n needs to get up first.", eA_ALWAYS, ch, cActParameter(), mover, eTO_CHAR);
                    }
                }

                return 0;
            }
            if (!mover->isNPC())
            {
                send_to_char("You must be on a mount if you wish to ride.<br/>", ch);
                return 0;
            }
        }
        else // IS_OBJ
        {
            if (OBJ_TYPE(mover) == ITEM_VEHICLE)
            {
                snprintf(aLeaveSelf, sizeof(aLeaveSelf), "You drive your $3N %s.", g_dirs[direction]);
                snprintf(aLeaveOther, sizeof(aLeaveOther), "$2n drives $2s $3N %s.", g_dirs[direction]);
                snprintf(aArrOther, sizeof(aArrOther), "$2n drives $2s $3N in from %s.", g_enter_dirs[g_rev_dir[direction]]);
                snprintf(aPassengersOther, sizeof(aPassengersOther), "$2n drives $2s $3N %s.", g_dirs[direction]);
            }
            else
            {
                snprintf(aLeaveSelf, sizeof(aLeaveSelf), "You sail your $3N %s.", g_dirs[direction]);
                snprintf(aLeaveOther, sizeof(aLeaveOther), "$2n sails $2s $3N %s.", g_dirs[direction]);
                snprintf(aArrOther, sizeof(aArrOther), "$2n sails $2s $3N in from %s.", g_enter_dirs[g_rev_dir[direction]]);
                snprintf(aPassengersOther, sizeof(aPassengersOther), "$2n sails $2s $3N %s.", g_dirs[direction]);
            }

            if (CHAR_POS(ch) < POSITION_STANDING)
            {
                if (CHAR_POS(ch) == POSITION_FIGHTING)
                {
                    send_to_char("You are fighting for your life!<br/>", ch);
                }
                else if (CHAR_POS(ch) < POSITION_RESTING)
                {
                    send_to_char("Twinkling stars...<br/>", ch);
                }
                return 0;
            }

            if (IS_SET(mover->getUnitFlags(), UNIT_FL_BURIED))
            {
                act("Your $2N is buried! It cannot move.", eA_ALWAYS, ch, mover, cActParameter(), eTO_CHAR);
                return 0;
            }

            if (!IS_SET(mover->getUnitFlags(), UNIT_FL_TRANS))
            {
                act("Alas, you cannot go that way...", eA_SOMEONE, mover, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }

            if ((OBJ_TYPE(mover) != ITEM_VEHICLE) && (OBJ_TYPE(mover) != ITEM_BOAT))
            {
                act("This isn't a vehicle!", eA_SOMEONE, mover, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }
            /* Now we need to check if the vehicle has the fuel to move (if not a boat)

            if (mover.outside.objecttype == ITEM_VEHICLE)
            {
               end_need := movementloss(room_in.movement);
               test := movementloss(room_going.movement);

               end_need := (end_need + test) / 2;

               if (mover.outside.value[1] - end_need < 0)
               {
                  act("Your $2N doesnt have the energy to move.", eA_SOMEONE, mover, mover.outside, null, eTO_CHAR);
                  return;
               }
            }*/
        }
    }

    if (ROOM_EXIT(room_from, direction)->isDoorFlagSet(EX_CLOSED))
    {
        if (!has_found_door(ch, direction))
        {
            send_to_char(ALAS_NOWAY, ch);
        }
        else
        {
            act("The $3t seems to be closed.", eA_SOMEONE, ch, cActParameter(), ROOM_DOOR_NAME(room_from, direction), eTO_CHAR);
        }
        return 0;
    }

    if (ROOM_EXIT(room_from, direction)->isDoorFlagSet(EX_CLIMB))
    {
        act("Alas, you must climb to go that way.", eA_SOMEONE, ch, cActParameter(), cActParameter(), eTO_CHAR);
        return 0;
    }

    // If you go washed into the ocean
    int bOceanEscape = (ROOM_LANDSCAPE(room_from) == SECT_WATER_SAIL) && (ROOM_LANDSCAPE(room_to) != SECT_WATER_SAIL);

    if (((ROOM_LANDSCAPE(room_from) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(room_to) == SECT_WATER_SAIL)) && !bOceanEscape)
    {
        if (mover->isChar())
        {
            act("You need to get yourself a boat.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
            return 0;
        }
        else // Sailing in an object
        {
            if (!mover->isObj() || OBJ_TYPE(mover) != ITEM_BOAT)
            {
                act("You must be inside a boat if you want to sail.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }
        }
    }
    else if ((ROOM_LANDSCAPE(room_from) == SECT_WATER_SWIM) || (ROOM_LANDSCAPE(room_to) == SECT_WATER_SWIM) || bOceanEscape)
    {
        if (mover->isChar())
        {
            if (!mover->getExtraList().find_raw(S_SWIM_ON) &&
                (!mover->getExtraList().find_raw(S_IS_FISH) || !mover->getExtraList().find_raw(S_IS_AMPHIB)))
            {
                act("You might want to swim.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }

            int skillbonus = 0;

            if ((CHAR_RACE(mover) >= RACE_ARTHROPODA_MIN) && (CHAR_RACE(mover) <= RACE_ARTHROPODA_MAX))
            {
                skillbonus = 50; // Additional bonus for races born to swim :)
            }
            if (mover->getExtraList().find_raw(S_IS_AMPHIB))
            {
                skillbonus = 80;
            }
            if (mover->getExtraList().find_raw(S_IS_FISH))
            {
                skillbonus = 100;
            }

            int diff = ROOM_EXIT(room_from, direction)->getSkillDifficulty();

            if (bOceanEscape)
            {
                slog(LOG_ALL,
                     0,
                     "Ocean escape from %s direction %d adding 150 to difficulty.",
                     room_from->getFileIndexSymName(),
                     direction);
                diff += 150;
                act("You really should have been in a boat here, swimming is incredible difficult.",
                    eA_ALWAYS,
                    mover,
                    cActParameter(),
                    cActParameter(),
                    eTO_CHAR);
            }

            diff -= skillbonus; // Make it less difficult if it's kind of fishy

            int skilltest = skillchecksa(mover, SKI_SWIMMING, ABIL_CON, diff);

            if (skilltest < 0)
            {
                act("As you try to swim, you flounder and take in water!", eA_ALWAYS, mover, cActParameter(), cActParameter(), eTO_CHAR);
                act("$1n tries to swim, but flounders and takes in water!", eA_ALWAYS, mover, cActParameter(), cActParameter(), eTO_REST);
                modify_hit(mover, (skilltest / 10));

                if (mover->is_destructed() || (CHAR_POS(mover) == POSITION_DEAD))
                {
                    return -1;
                }
                else
                {
                    return 0;
                }
            }
        }    // IS_CHAR
        else // Inside an object
        {
            if (!mover->isObj() || OBJ_TYPE(mover) != ITEM_BOAT)
            {
                act("You must be inside a boat if you want to sail.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
                return 0;
            }
        }
    }
    else // we're not swimming
    {
        if (mover->getExtraList().find_raw(S_IS_FISH))
        {
            act("No little fishies on land.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
            return 0; // Fish can't walk on land. 2020 I would think checking for race was equally important as extra?
        }

        if (mover->isObj() && (OBJ_TYPE(mover) == ITEM_BOAT))
        {
            act("You can't sail your boat on land.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
            return 0;
        }
    }

    if (mover->isChar())
    {
        int need_movement = (g_movement_loss[ROOM_LANDSCAPE(room_from)] + g_movement_loss[ROOM_LANDSCAPE(room_to)]) / 2;

        int overweight = ch->getContainingWeight() - char_carry_w_limit(mover);

        if (overweight > 0)
        {
            send_to_char("You're carrying more than you can and it is exhausting.<br/>", mover);
            need_movement += 1 + (overweight / 10);
        }

        if (CHAR_ENDURANCE(mover) < need_movement)
        {
            if (!following)
            {
                send_to_char("You are too exhausted.<br/>", mover);
            }
            else
            {
                send_to_char("You are too exhausted to keep up.<br/>", mover);
            }

            if (ch != mover)
            {
                act("Your $2n is too exhausted to move.", eA_ALWAYS, ch, mover, cActParameter(), eTO_CHAR);
            }
            return 0;
        }
        if (CHAR_LEVEL(ch) < 200)
        {
            UCHAR(ch)->decreaseEnduranceBy(need_movement);
        }
    }

    return room_move(ch, mover, room_from, room_to, following, direction, ls, lo, as, ao, aPassengersOther);
}

/**
 * @param ch
 * @param mover
 * @param direction
 * @param following Following defaults to false. If it is set to TRUE, then it will generate
 * the special to check if the move is allowed.
 * @returns 1 : If success.<br>
 * 0 : If fail<br>
 * -1 : If dead.
 */
int self_walk(unit_data *ch, unit_data *mover, int direction, int following)
{
    unit_data *room_from = nullptr;

    room_from = ch->inRoom();

    int res = generic_move(ch, mover, direction, following);

    if (!CHAR_MASTER(ch) && !CHAR_FOLLOWERS(ch))
    {
        return res;
    }

    if (res == 1 && (ch->inRoom() != room_from))
    {
#ifdef SUSPEKT
        unit_data *u = nullptr;

        if (ch->isChar() && CHAR_FOLLOWERS(ch))
        {
            u = ch;
        }

        if (mover->isChar() && CHAR_FOLLOWERS(mover))
        {
            u = mover;
        }

        if (u && CHAR_FOLLOWERS(u))
        {
            int i = 0;
            int j = 0;
            char_follow_type *k = nullptr;

            for (i = 0;; i++) /* This shit is needed because the follow  */
            {                 /* structure can be destroyed by this move */
                for (j = 0, k = CHAR_FOLLOWERS(u); k && j < i; j++, k = k->getNext())
                {
                    ;
                }

                if (k == nullptr)
                {
                    break;
                }

                if (room_from == k->getFollower()->inRoom() && CHAR_POS(k->getFollower()) >= POSITION_STANDING)
                {
                    act("You follow $3n.<br/>", eA_SOMEONE, k->getFollower(), cActParameter(), ch, eTO_CHAR);
                    if (k->getFollower()->getUnitIn()->isRoom())
                    {
                        self_walk(k->getFollower(), k->getFollower(), direction, TRUE);
                    }
                    else
                    {
                        self_walk(k->getFollower(), k->getFollower()->getUnitIn(), direction, TRUE);
                    }
                }
            }
        }
#endif
    }

    return res;
}

/**
 * @param ch
 * @param dir must be one of [CMD_NORTH..MAX_EXIT]
 */
void move_dir(unit_data *ch, int dir)
{
    assert((dir >= 0) && (dir <= MAX_EXIT));

    if (CHAR_POS(ch) < POSITION_STANDING)
    {
        if (CHAR_POS(ch) == POSITION_FIGHTING)
        {
            send_to_char("You are fighting for your life!<br/>", ch);
        }
        else
        {
            send_to_char("Perhaps you should get on your feet first?<br/>", ch);
        }
        return;
    }

    if (ch->getUnitIn()->isObj())
    {
        self_walk(ch, ch->getUnitIn(), dir, 0);
    }
    else if (ch->getUnitIn()->isChar())
    {
        self_walk(ch, ch->getUnitIn(), dir, 0);
    }
    else if (ch->getUnitIn()->isRoom())
    {
        self_walk(ch, ch, dir, 0);
    }
    else
    {
        slog(LOG_ALL, 0, "Unit %s is inside an unexpected unit type %s", ch->getNames().Name(), ch->getUnitIn()->getNames().Name());
        act("Hmm. You shouldnt be in here. You're pushed out.", eA_SOMEONE, ch, cActParameter(), cActParameter(), eTO_CHAR);
        if (ch->getUnitIn()->getUnitIn())
        {
            ch->setUnitIn(ch->getUnitIn()->getUnitIn());
        }
        command_interpreter(ch, "look");
        act("$1n appears out of thin air.", eA_HIDEINV, ch, cActParameter(), cActParameter(), eTO_REST);
    }
}

void do_move(unit_data *ch, char *argument, const command_info *cmd)
{
    /* NOTICE! It uses cmd->no for efficiency, thus cmd->no MUST */
    /* be one of CMD_NORTH..CMD_DOWN which again MUST BE 0..5    */

    assert((cmd->no >= 0) && (cmd->no <= MAX_EXIT));

    move_dir(ch, cmd->no);
}

/**
 *  Will find ALL doors, including hidden doors.
 * @param ch
 * @param doorstr
 * @param err_msg if TRUE, error messages will be shown.
 * @param check_hidden: if FALSE, hidden doors will be ignored, otherwise hidden doors will be considered.
 * @returns -1 if door is not found, otherwise 0..5 for door exit
 */
int low_find_door(unit_data *ch, char *doorstr, int err_msg, int check_hidden)
{
    char buf[256];
    char dir[256];
    char *dirdoorstr = nullptr;
    int door = 0;

    dirdoorstr = one_argument(doorstr, dir);

    if (str_is_empty(dir))
    {
        if (err_msg)
        {
            act("What?", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
        }
        return -1;
    }

    if (!ch->getUnitIn()->isRoom())
    {
        if (err_msg)
        {
            act("You see no such exit.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
        }
        return -1;
    }

    if ((door = search_block(dir, g_dirs, FALSE)) != -1) /* Partial Match */
    {
        /* A direction and name was specified */
        if (str_is_empty(dirdoorstr))
        {
            if (ROOM_EXIT(ch->getUnitIn(), door) && (!check_hidden || has_found_door(ch, door)))
            {
                return door;
            }

            if (err_msg)
            {
                act("You see no exit in that direction.", eA_ALWAYS, ch, dirdoorstr, cActParameter(), eTO_CHAR);
            }

            return -1;
        }

        if (ROOM_EXIT(ch->getUnitIn(), door))
        {
            if (ROOM_EXIT(ch->getUnitIn(), door)->getOpenName().IsName(dirdoorstr) && (!check_hidden || has_found_door(ch, door)))
            {
                return door;
            }

            if (err_msg)
            {
                act("You see no $2t in that direction.", eA_ALWAYS, ch, dirdoorstr, cActParameter(), eTO_CHAR);
            }
            return -1;
        }

        if (err_msg)
        {
            act("You see no exit in that direction.", eA_ALWAYS, ch, cActParameter(), cActParameter(), eTO_CHAR);
        }
        return -1;
    }

    /* No direction was specified, try to locate the keyword */
    one_argument(doorstr, buf);

    for (door = 0; door <= MAX_EXIT; door++)
    {
        if (ROOM_EXIT(ch->getUnitIn(), door))
        {
            if (ROOM_EXIT(ch->getUnitIn(), door)->getOpenName().IsName(doorstr) && (!check_hidden || has_found_door(ch, door)))
            {
                return door;
            }
        }
    }

    if (err_msg)
    {
        act("You see no $2t here.", eA_ALWAYS, ch, doorstr, cActParameter(), eTO_CHAR);
    }

    return -1;
}

#ifdef _DEPRECATED

int do_simple_move(class unit_data *ch, int direction, int following)
/* Asserts:
        0. ch is in a room, going in a direction n,e,s,w,u or d
        1. Does not assert anything about position.
        2. That the direction exists.
        3. Tests if the door is closed (to make follow work correctly
           after special is called!)

        Followers won't follow.
   Following indicates if this is a follower moving because it is following another char.

        Returns :
        1 : If succes.
        0 : If fail
        -1 : If dead.
        */
{
    int need_movement, res;
    class unit_data *to, *u, *room;
    const char *c;
    char dirbuf[2] = "c";
    char buf[MAX_INPUT_LENGTH];

    room = in_room(ch);

    assert(ROOM_EXIT(room, direction));
    assert(ROOM_EXIT(room, direction)->to_room);

    strcpy(buf, "");
    /* If we are not following, then we already issued the special
       in command interpreter! */
    if (following)
    {
        res = send_preprocess(ch, g_cmd_dirs[direction], buf);
        if (ch->is_destructed())
            return -1;
    }
    else
        res = SFR_SHARE;

    if ((res != SFR_SHARE) || (room != in_room(ch)))
        return 0;

    if (IS_SET(ROOM_EXIT(room, direction)->exit_info, EX_CLOSED))
    {
        if (!has_found_door(ch, direction))
            send_to_char(ALAS_NOWAY, ch);
        else
            act("The $3t seems to be closed.", eA_SOMEONE, ch, 0, ROOM_DOOR_NAME(room, direction), eTO_CHAR);
        return 0;
    }

    if ((ROOM_LANDSCAPE(room) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room) == SECT_WATER_SAIL))
    {
        send_to_char("You need sail in a boat to go there.<br/>", ch);
        return 0;
    }

    need_movement = (g_movement_loss[ROOM_LANDSCAPE(room)] + g_movement_loss[ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room)]) / 2;

    if (CHAR_ENDURANCE(ch) < need_movement)
    {
        if (!following)
            send_to_char("You are too exhausted.<br/>", ch);
        else
            send_to_char("You are too exhausted to follow.<br/>", ch);

        return 0;
    }

    to = ROOM_EXIT(room, direction)->to_room;

    if (!pay_point_charlie(ch, to))
        return 0;

    for (u = UNIT_CONTAINS(ch); u; u = u->next)
        if (!pay_point_charlie(u, to))
            return 0;

    if (CHAR_LEVEL(ch) < 200)
        CHAR_ENDURANCE(ch) -= need_movement;

    dirbuf[0] = g_dirs[direction][0];

    c = single_unit_messg(room, "$leave_o", dirbuf, "$1n leaves $3t.");

    if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
        act(c, eA_HIDEINV, ch, room, g_dirs[direction], TO_ROOM);

    c = single_unit_messg(room, "$leave_s", dirbuf, "");

    if (!str_is_empty(c))
        act(c, eA_ALWAYS, ch, room, g_dirs[direction], eTO_CHAR);

    unit_from_unit(ch);

    REMOVE_BIT(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET | CHAR_SELF_DEFENCE);

    unit_to_unit(ch, to);

    dirbuf[0] = g_dirs[g_rev_dir[direction]][0];

    c = single_unit_messg(room, "$arrive_o", dirbuf, "$1n has arrived from $3t.");

    if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
        act(c, eA_HIDEINV, ch, to, g_enter_dirs[g_rev_dir[direction]], TO_ROOM);

    c = single_unit_messg(to, "$arrive_s", dirbuf, "");

    if (!str_is_empty(c))
        act(c, eA_ALWAYS, ch, to, g_enter_dirs[g_rev_dir[direction]], eTO_CHAR);

    command_interpreter(ch, "look");

    send_done(ch, ch, room, direction, &g_cmd_auto_enter, ""); // Send to was_in

    return (1);
}


void steed_walk(class unit_data *ch, int direction)
{
    /*
   To be expanded:
   Position of mount
   Endurance of mount
 */
    class unit_data *beast, *room, *to_room;

    beast = UNIT_IN(ch);

    if (!IS_NPC(beast))
    {
        act("You must be on a mount if you want to ride.", eA_ALWAYS, ch, 0, 0, eTO_CHAR);
        return;
    }

    if (CHAR_FIGHTING(beast))
    {
        act("You must be in control to do that.", eA_ALWAYS, ch, 0, 0, eTO_CHAR);
        return;
    }

    for (class unit_data *u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
        if (IS_CHAR(u) && CHAR_FIGHTING(u))
        {
            act("You can't just ride away in the middle of a combat.", eA_ALWAYS, ch, 0, 0, eTO_CHAR);
            return;
        }

    room = UNIT_IN(beast);

    if (!room || !IS_ROOM(room) || !ROOM_EXIT(room, direction))
    {
        send_to_char("Alas, you cannot ride that way...<br/>", ch);
        return;
    }

    /* Direction is possible */
    if (IS_SET(ROOM_EXIT(room, direction)->exit_info, EX_CLOSED))
    {
        if (!has_found_door(ch, direction))
            send_to_char(ALAS_NOWAY, ch);
        else
            act("The $3t seems to be closed.", eA_SOMEONE, ch, 0, ROOM_DOOR_NAME(room, direction), eTO_CHAR);
        return;
    }

    to_room = ROOM_EXIT(room, direction)->to_room;

    if (ROOM_LANDSCAPE(to_room) == SECT_WATER_SAIL || ROOM_LANDSCAPE(to_room) == SECT_WATER_SWIM)
    {
        send_to_char("Your mount refuses to carry you there!<br/>", ch);
        return;
    }

    do_simple_ride(beast, ch, direction);
}

/* Expects 'north'..'down' as argument */
void vehicle_walk(class unit_data *ch, int dir)
{
    class unit_data *boat, *room, *u;

    boat = UNIT_IN(ch);

    if (!IS_OBJ(boat) || OBJ_TYPE(boat) != ITEM_BOAT)
    {
        act("You must be inside a boat if you want to sail.", eA_ALWAYS, ch, 0, 0, eTO_CHAR);
        return;
    }

    room = UNIT_IN(boat);

    if (!IS_ROOM(room) || !ROOM_EXIT(room, dir))
    {
        send_to_char("Alas, you cannot sail that way...<br/>", ch);
        return;
    }

    for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
        if (IS_CHAR(u) && CHAR_FIGHTING(u))
        {
            act("You can't just sail away in the middle of a combat.", eA_ALWAYS, ch, 0, 0, eTO_CHAR);
            return;
        }

    /* Direction is possible */
    if (IS_SET(ROOM_EXIT(room, dir)->exit_info, EX_CLOSED))
    {
        /* XXX MS2020 backdoor(ch, arg, cmd); */

        act("The $3t seems to be closed.", eA_SOMEONE, ch, 0, ROOM_DOOR_NAME(room, dir), eTO_CHAR);
        return;
    }

    /* Direction is not closed */
    if (ROOM_EXIT(room, dir)->to_room == 0)
    {
        send_to_char("Alas, you can't sail that way.<br/>", ch);
        return;
    }

    if (ROOM_LANDSCAPE(room) != SECT_WATER_SAIL && ROOM_LANDSCAPE(room) != SECT_WATER_SWIM &&
        ROOM_LANDSCAPE(ROOM_EXIT(room, dir)->to_room) != SECT_WATER_SAIL &&
        ROOM_LANDSCAPE(ROOM_EXIT(room, dir)->to_room) != SECT_WATER_SWIM)
    {
        send_to_char("A boat requires water to sail in!<br/>", ch);
        return;
    }

    do_simple_sail(boat, ch, dir);
}

/* For sailing in boats! */
int do_simple_sail(class unit_data *boat, class unit_data *captain, int direction)

/* Asserts:

        * boat is in a room, going in a direction n,e,s,w,u or d

        * That the direction exists. That there is no door.

        Returns :
        1 : If succes.
        0 : If fail
        -1 : If destroyed!
        */
{
    int res;
    class unit_data *u, *was_in, *to;
    char buf[MAX_STRING_LENGTH];

    assert(UNIT_IN(boat) && IS_OBJ(boat));
    assert(IS_ROOM(UNIT_IN(boat)));
    assert(ROOM_EXIT(UNIT_IN(boat), direction));
    assert(ROOM_EXIT(UNIT_IN(boat), direction)->to_room);

    was_in = UNIT_IN(boat);
    to = ROOM_EXIT(was_in, direction)->to_room;
    strcpy(buf, "");

    /* Ok, the boat must always issue a special call in case some room
       might want to catch it! Not like walking! */

    res = send_preprocess(boat, g_cmd_dirs[direction], buf);

    for (u = UNIT_CONTAINS(boat); u; u = u->next)
        if (!pay_point_charlie(u, to))
            return 0;

    if (boat->is_destructed())
        return -1;

    if (res != SFR_SHARE || was_in != UNIT_IN(boat))
        return 0;

    unit_from_unit(boat);

    if (UNIT_CONTAINS(was_in))
        act("$2n sails $3t.", eA_HIDEINV, UNIT_CONTAINS(was_in), boat, g_dirs[direction], TO_ALL);

    if (UNIT_CONTAINS(to))
        act("$2n has arrived from $3t.", eA_HIDEINV, UNIT_CONTAINS(to), boat, g_enter_dirs[g_rev_dir[direction]], TO_ALL);

    unit_to_unit(boat, to);

    for (u = UNIT_CONTAINS(boat); u; u = u->next)
        if (IS_CHAR(u))
        {
            act("$1n sails $2t with you.", eA_SOMEONE, boat, g_dirs[direction], u, TO_VICT);
            command_interpreter(u, "look");
        }

    send_done(boat, captain, was_in, direction, &g_cmd_auto_enter, "");

    return 1;
}

/* For riding mounts! */
int do_simple_ride(class unit_data *beast, class unit_data *master, int direction)

/* Asserts:

        * beast is in a room, going in a direction n,e,s,w,u or d

        * That the direction exists. That there is no door.

        Returns :
        1 : If succes.
        0 : If fail
        -1 : If destroyed!
        */
{
    int res, need_movement;
    class unit_data *u, *was_in, *to;
    char buf[MAX_INPUT_LENGTH];

    assert(UNIT_IN(beast) && IS_NPC(beast));
    assert(IS_ROOM(UNIT_IN(beast)));
    assert(ROOM_EXIT(UNIT_IN(beast), direction));
    assert(ROOM_EXIT(UNIT_IN(beast), direction)->to_room);

    was_in = UNIT_IN(beast);
    to = ROOM_EXIT(was_in, direction)->to_room;

    strcpy(buf, "");
    res = send_preprocess(beast, g_cmd_dirs[direction], buf);

    for (u = UNIT_CONTAINS(beast); u; u = u->next)
        if (!pay_point_charlie(u, to))
            return 0;

    if (beast->is_destructed())
        return -1;

    if ((res != SFR_SHARE) || (was_in != UNIT_IN(beast)))
        return 0;

    if ((ROOM_LANDSCAPE(UNIT_IN(beast)) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(to) == SECT_WATER_SAIL))
    {
        act("$1n refuses to go there.", eA_SOMEONE, beast, 0, 0, TO_ROOM);
        return 0;
    }

    need_movement = (g_movement_loss[ROOM_LANDSCAPE(UNIT_IN(beast))] + g_movement_loss[ROOM_LANDSCAPE(to)]) / 2;

    if (CHAR_ENDURANCE(beast) < need_movement)
    {
        send_to_char("The beast is too exhausted.<br/>", UNIT_CONTAINS(beast));
        return 0;
    }

    CHAR_ENDURANCE(beast) -= need_movement;

    unit_from_unit(beast);

    if (UNIT_CONTAINS(was_in))
        act("$2n rides $3t.", eA_HIDEINV, UNIT_CONTAINS(was_in), beast, g_dirs[direction], TO_ALL);

    if (UNIT_CONTAINS(to))
        act("$2n has arrived from $3t.", eA_HIDEINV, UNIT_CONTAINS(to), beast, g_enter_dirs[g_rev_dir[direction]], TO_ALL);

    unit_to_unit(beast, to);

    for (u = UNIT_CONTAINS(beast); u; u = u->next)
    {
        if (IS_CHAR(u))
        {
            act("$1n rides $2t with you.", eA_SOMEONE, beast, g_dirs[direction], u, TO_VICT);
            command_interpreter(u, "look");
        }
    }

    send_done(beast, master, was_in, direction, &g_cmd_auto_enter, "");

    return 1;
}

#endif
