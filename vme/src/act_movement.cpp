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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "affect.h"
#include "movement.h"
#include "constants.h"
#include "vmelimits.h"
#include "main.h"
#include "account.h"
#include "common.h"
#include "dilsup.h"
#include "fight.h"
#include "skills.h"

extern struct command_info *cmd_dirs[MAX_EXIT + 1];


// I had to add a act() kludge here. 
// The $arrive_ and $leave_ extras depend on $2t for special descriptions.
// However, many act() in movement require 3 units, so therefore $2t in extras is
// string substituted in this procedure
//
const char *single_unit_messg(class unit_data *unit,
                              const char *type,
                              int direction,
                              char *mesg)
{
   class extra_descr_data *exd = UNIT_EXTRA(unit).m_pList;

   if (exd)
      exd = exd->find_raw(type);

   if (exd && exd->descr.c_str())
   {
      if (is_in(direction, 0, MAX_EXIT))
      {
         if ((exd->names.Name(1) == NULL) || str_cstr(exd->names.Name(1), dirs_short[direction]))
         {
            strcpy(mesg, exd->descr.c_str());
            str_substitute("$2t", dirs[direction], mesg);
         }
      }
      else
      {
         slog(LOG_ALL, 0, "Illegal direction %d", direction);
      }
   }

   return mesg;
}

/* Has 'pc' found the door at 'dir'? If direction exits and it is closed  */
/* and hidden then the door is found if it has been searched for.         */
int has_found_door(class unit_data *pc, int dir)
{
   class extra_descr_data *exd;
   class unit_affected_type *af;
   char buf[MAX_INPUT_LENGTH];

   if (!IS_ROOM(UNIT_IN(pc)))
      return FALSE;

   if (!ROOM_EXIT(UNIT_IN(pc), dir))
      return FALSE;

   if (!IS_PC(pc))
      return TRUE;

   if (!IS_SET(ROOM_EXIT(UNIT_IN(pc), dir)->exit_info, EX_HIDDEN))
      return TRUE;

   strcpy(buf, SECRET_DOOR);
   strcat(buf, itoa(dir));

   exd = UNIT_EXTRA(UNIT_IN(pc)).find_raw(buf);

   if (exd == NULL)
      return FALSE;

   return exd->names.IsName(UNIT_NAMES(pc).Name(0)) != NULL;

   if (IS_SET(ROOM_EXIT(UNIT_IN(pc), dir)->exit_info, EX_CLOSED))
   {
      for (af = UNIT_AFFECTED(UNIT_IN(pc)); af; af = af->next)
         if (af->id == ID_SPOTTED_SECRET && PC_ID(pc) == af->data[0])
            return TRUE;
      return FALSE;
   }
   else
      return TRUE;
}

class unit_data *in_room(class unit_data *u)
{
   while (u && !IS_ROOM(u))
      u = UNIT_IN(u);

   assert(u);

   return u;
}


#define ALAS_NOWAY "Alas, you cannot go that way...<br/>"
/* Other logic has figured out if there was a direction, if it was open, if you're sleeping, mesgs, etc. */
/* This is the actual move between two rooms. Direction only needed for command_info event. */
/* Endurance is calculated and subtracted here */
/* ch is the CHAR doing the move command. mover is the vehicle, steed or boat or the char itself */
/* 1 = success, 0 = fail, -1 = dead */
int room_move(class unit_data *ch, class unit_data *mover, class unit_data *room_from, class unit_data *room_to,
              int bIsFollower, int direction,
              const char *pLeaveSelf, const char *pLeaveOther,
              const char *pArrSelf, const char *pArrOther,
              const char *pPassengersO)
{
   int res;
   class unit_data *u;

   room_from = in_room(ch);

   assert(IS_ROOM(room_from));
   assert(IS_ROOM(room_to));

   /* If we are not following, then we already issued the special
      in command interpreter! */
   if (bIsFollower)
   {
      char buf[MAX_INPUT_LENGTH];
      strcpy(buf, "");
      res = send_preprocess(mover, cmd_dirs[direction], buf);
      if (ch->is_destructed())
         return -1;
   }
   else
      res = SFR_SHARE;

   if ((res != SFR_SHARE) || (room_from != in_room(ch)))
      return 0;

   if (!pay_point_charlie(ch, room_to))
      return 0;

   for (u = UNIT_CONTAINS(ch); u; u = u->next)
      if (!pay_point_charlie(u, room_to))
         return 0;

   if (!str_is_empty(pLeaveSelf))
      act(pLeaveSelf, A_ALWAYS, ch, room_from, mover, TO_CHAR);

   unit_from_unit(mover);

   if (UNIT_CONTAINS(room_from) && !str_is_empty(pLeaveOther))
   {
      if ((mover != ch) ||  !CHAR_HAS_FLAG(ch, CHAR_SNEAK))
         act(pLeaveOther, A_HIDEINV, UNIT_CONTAINS(room_from), ch, mover, TO_ALL);
   }

   if (UNIT_CONTAINS(room_to) && !str_is_empty(pArrOther))
   {
      if ((mover != ch) ||  !CHAR_HAS_FLAG(ch, CHAR_SNEAK))
         act(pArrOther, A_HIDEINV, UNIT_CONTAINS(room_to), ch, mover, TO_ALL);
   }

   // These two volatile flags are removed when you move.
   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET | CHAR_SELF_DEFENCE);

   unit_to_unit(mover, room_to);

   if (!str_is_empty(pArrSelf))
      act(pArrSelf, A_ALWAYS, ch, room_to, mover, TO_CHAR);

   command_interpreter(ch, "look :brief:");

   if (ch != mover)
   {  
      if (IS_CHAR(mover))
         command_interpreter(mover, "look :brief:");

      for (u = UNIT_CONTAINS(mover); u; u = u->next)
      {
         if ((u != ch) && IS_CHAR(u))
         {
            act(pPassengersO, A_SOMEONE, u, ch, mover, TO_CHAR);
            command_interpreter(u, "look :brief:");
         }
      }
   }

   send_done(mover, ch, room_from, direction, &cmd_auto_enter, "");

   return 1;
}


#define S_SWIM_ON "$swim_on"

#define S_IS_FISH "$is_fish"

#define S_IS_AMPHIB "$is_amphib"

int generic_move(class unit_data *ch, class unit_data *mover, int direction, int following)
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
   class unit_data *room_from, *room_to;
   char aLeaveSelf[MAX_STRING_LENGTH];
   char aLeaveOther[MAX_STRING_LENGTH];
   char aArrSelf[MAX_STRING_LENGTH];
   char aArrOther[MAX_STRING_LENGTH];
   char aPassengersOther[MAX_STRING_LENGTH];
   const char *lo,*ls,*as,*ao;
   aLeaveOther[0] = 0;
   aLeaveSelf[0] = 0;
   aArrOther[0] = 0;
   aArrSelf[0] = 0;
   aPassengersOther[0] = 0;

   ls = aLeaveSelf;
   lo = aLeaveOther;
   ao = aArrOther;
   as = aArrSelf;

   room_from = in_room(ch);

   assert((ch == mover) || (UNIT_IN(ch) == mover));

   if (!IS_ROOM(UNIT_IN(mover)))
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
   if (ROOM_EXIT(room_from, direction)->to_room == 0)
   {
      send_to_char(ALAS_NOWAY, ch);
      return 0;
   }

   room_to = ROOM_EXIT(room_from, direction)->to_room;

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

      sprintf(aLeaveOther, "$2n leaves %s.", dirs[direction]);
      ls = single_unit_messg(room_from, "$leave_s", direction, aLeaveSelf);
      lo = single_unit_messg(room_from, "$leave_o", direction, aLeaveOther);

      sprintf(aArrOther, "$2n has arrived from %s.", enter_dirs[rev_dir[direction]]);
      as = single_unit_messg(room_to, "$arrive_s", rev_dir[direction], aArrSelf);
      ao = single_unit_messg(room_to, "$arrive_o", rev_dir[direction], aArrOther);
   }
   else // Steed or boat code (shares scan for passenger combat)
   {
      for (class unit_data *u = UNIT_CONTAINS(mover); u; u = u->next)
            if (IS_CHAR(u) && CHAR_FIGHTING(u))
            {
               act("You can't get away like that in the middle of combat.",
                  A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
               act("You can't get away like that in the middle of combat.",
                  A_ALWAYS, mover, cActParameter(), cActParameter(), TO_CHAR);
               return 0;
            }

      if (IS_CHAR(mover)) // Mounted on a steed
      {
         sprintf(aLeaveSelf, "You ride your $3N %s.", dirs[direction]);
         sprintf(aLeaveOther, "$2n rides $2s $3N %s.", dirs[direction]);
         sprintf(aArrOther, "$2n rides $2s $3N in from %s.", enter_dirs[rev_dir[direction]]);
         sprintf(aPassengersOther, "$2n rides $2s $3N %s.", dirs[direction]);

         if (CHAR_POS(mover) < POSITION_STANDING)
         {
            if (CHAR_POS(mover) == POSITION_FIGHTING)
            {
               send_to_char("You are fighting for your life!<br/>", mover);
               if (ch != mover)
                  act("Your $3n is busy fighting!", A_ALWAYS, ch, cActParameter(), mover, TO_CHAR);
            }
            else if (CHAR_POS(mover) >= POSITION_RESTING)
            {
               send_to_char("Perhaps you should get on your feet first?<br/>", mover);
               if (ch != mover)
                  act("Your $3n needs to get up first.", A_ALWAYS, ch, cActParameter(), mover, TO_CHAR);
            }

            return 0;
         }
         if (!IS_NPC(mover))
         {
            send_to_char("You must be on a mount if you wish to ride.<br/>", ch);
            return 0;            
         }
      }
      else // IS_OBJ 
      {
         if (OBJ_TYPE(mover) == ITEM_VEHICLE)
         {
            sprintf(aLeaveSelf, "You drive your $3N %s.", dirs[direction]);
            sprintf(aLeaveOther, "$2n drives $2s $3N %s.", dirs[direction]);
            sprintf(aArrOther, "$2n drives $2s $3N in from %s.", enter_dirs[rev_dir[direction]]);
            sprintf(aPassengersOther, "$2n drives $2s $3N %s.", dirs[direction]);
         }
         else
         {
            sprintf(aLeaveSelf, "You sail your $3N %s.", dirs[direction]);
            sprintf(aLeaveOther, "$2n sails $2s $3N %s.", dirs[direction]);
            sprintf(aArrOther, "$2n sails $2s $3N in from %s.", enter_dirs[rev_dir[direction]]);
            sprintf(aPassengersOther, "$2n sails $2s $3N %s.", dirs[direction]);
         }

         if (CHAR_POS(ch) < POSITION_STANDING)
         {
            if (CHAR_POS(ch) == POSITION_FIGHTING)
               send_to_char("You are fighting for your life!<br/>", ch);
            else if (CHAR_POS(ch) < POSITION_RESTING)
               send_to_char("Twinkling stars...<br/>", ch);
            return 0;
         }
   
         if (IS_SET(UNIT_FLAGS(mover), UNIT_FL_BURIED))
         {
            act("Your $2N is buried! It cannot move.", A_ALWAYS, ch, mover, cActParameter(), TO_CHAR);
            return 0;
         }

         if (!IS_SET(UNIT_FLAGS(mover), UNIT_FL_TRANS))
         {  
            act("Alas, you cannot go that way...", A_SOMEONE, mover, cActParameter(), cActParameter(), TO_CHAR);
            return 0;
         }

         if ((OBJ_TYPE(mover) != ITEM_VEHICLE) && (OBJ_TYPE(mover) != ITEM_BOAT))
         {
            act("This isn't a vehicle!", A_SOMEONE, mover, cActParameter(), cActParameter(), TO_CHAR);
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
               act("Your $2N doesnt have the energy to move.", A_SOMEONE, mover, mover.outside, null, TO_CHAR);
               return;
            }
         }*/
      }
   }

   if (IS_SET(ROOM_EXIT(room_from, direction)->exit_info, EX_CLOSED))
   {
      if (!has_found_door(ch, direction))
         send_to_char(ALAS_NOWAY, ch);
      else
         act("The $3t seems to be closed.",
             A_SOMEONE, ch, cActParameter(), ROOM_DOOR_NAME(room_from, direction), TO_CHAR);
      return 0;
   }

   if (IS_SET(ROOM_EXIT(room_from, direction)->exit_info, EX_CLIMB))
   {
      act("Alas, you must climb to go that way.", A_SOMEONE, ch, cActParameter(), cActParameter(), TO_CHAR);
      return 0;
   }

   // If you go washed into the ocean
   int bOceanEscape= (ROOM_LANDSCAPE(room_from) == SECT_WATER_SAIL) && (ROOM_LANDSCAPE(room_to) != SECT_WATER_SAIL);

   if (((ROOM_LANDSCAPE(room_from) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(room_to) == SECT_WATER_SAIL)) && !bOceanEscape)
   {
      if (IS_CHAR(mover))
      {
         act("You need to get yourself a boat.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
         return 0;
      }
      else  // Sailing in an object
      {
         if (!IS_OBJ(mover) || OBJ_TYPE(mover) != ITEM_BOAT)
         {
            act("You must be inside a boat if you want to sail.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
            return 0;
         }
      }
   }
   else if ((ROOM_LANDSCAPE(room_from) == SECT_WATER_SWIM) || (ROOM_LANDSCAPE(room_to) == SECT_WATER_SWIM) || bOceanEscape)
   {
      if (IS_CHAR(mover))
      {
         if (!UNIT_EXTRA(mover).find_raw(S_SWIM_ON) && (!UNIT_EXTRA(mover).find_raw(S_IS_FISH) || !UNIT_EXTRA(mover).find_raw(S_IS_AMPHIB)))
         {
            act("You might want to swim.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
            return 0;
         }

         int skillbonus = 0;

         if ((CHAR_RACE(mover) >= RACE_ARTHROPODA_MIN) && (CHAR_RACE(mover) <= RACE_ARTHROPODA_MAX))
         {
            skillbonus = 50; // Additional bonus for races born to swim :) 
         }
         if (UNIT_EXTRA(mover).find_raw(S_IS_AMPHIB))
         {
            skillbonus = 80;
         }
         if (UNIT_EXTRA(mover).find_raw(S_IS_FISH))
         {
            skillbonus = 100;
         }

         int diff = ROOM_EXIT(room_from, direction)->difficulty;

         if (bOceanEscape)
         {
            slog(LOG_ALL, 0, "Ocean escape from %s@%s direction %d adding 150 to difficulty.", 
                  UNIT_FI_NAME(room_from), UNIT_FI_ZONENAME(room_from), direction);
            diff += 150;
            act("You really should have been in a boat here, swimming is incredible difficult.", 
                  A_ALWAYS, mover, cActParameter(), cActParameter(), TO_CHAR);
         }

         diff -= skillbonus; // Make it less difficult if it's kind of fishy

         int skilltest = skillchecksa(mover, SKI_SWIMMING, ABIL_CON, diff);

         if (skilltest < 0)
         {
            act("As you try to swim, you flounder and take in water!", A_ALWAYS, mover, cActParameter(), cActParameter(), TO_CHAR);
            act("$1n tries to swim, but flounders and takes in water!", A_ALWAYS, mover, cActParameter(), cActParameter(), TO_REST);
            modify_hit(mover, (skilltest / 10));

            if (mover->is_destructed() || (CHAR_POS(mover) == POSITION_DEAD))
               return -1;
            else
               return 0;
         }
      } // IS_CHAR
      else  // Inside an object
      {
         if (!IS_OBJ(mover) || OBJ_TYPE(mover) != ITEM_BOAT)
         {
            act("You must be inside a boat if you want to sail.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
            return 0;
         }
      }
   } 
   else // we're not swimming
   {
      if (UNIT_EXTRA(mover).find_raw(S_IS_FISH))
      {
         act("No little fishies on land.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
         return 0; //Fish can't walk on land. 2020 I would think checking for race was equally important as extra?
      }

      if (IS_OBJ(mover) && (OBJ_TYPE(mover) == ITEM_BOAT))
      {
         act("You can't sail your boat on land.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
         return 0;
      }
   }

   if (IS_CHAR(mover))
   {
      int need_movement =
          (movement_loss[ROOM_LANDSCAPE(room_from)] +
           movement_loss[ROOM_LANDSCAPE(room_to)]) / 2;

      int overweight = UNIT_CONTAINING_W(ch) - char_carry_w_limit(mover);

      if (overweight > 0)
      {
         send_to_char("You're carrying more than you can and it is exhausting.<br/>", mover);
         need_movement += 1 + (overweight / 10);
      }

      if (CHAR_ENDURANCE(mover) < need_movement)
      {
         if (!following)
            send_to_char("You are too exhausted.<br/>", mover);
         else
            send_to_char("You are too exhausted to keep up.<br/>", mover);

         if (ch != mover)
            act("Your $2n is too exhausted to move.", A_ALWAYS, ch, mover, cActParameter(), TO_CHAR);
         return 0;
      }
      if (CHAR_LEVEL(ch) < 200)
         CHAR_ENDURANCE(ch) -= need_movement;
   }

   return room_move(ch, mover, room_from, room_to, following, direction, ls, lo, as, ao, aPassengersOther);
}

/* Following defaults to false. If it is set to TRUE, then it will generate
   the special to check if the move is allowed. */
int self_walk(class unit_data *ch, class unit_data *mover, int direction, int following)
/*
       Returns :
       1 : If succes.
       0 : If fail
       -1 : If dead.
       */
{
   class unit_data *room_from;

   room_from = in_room(ch);

   int res = generic_move(ch, mover, direction, following);

   if (!CHAR_MASTER(ch) && !CHAR_FOLLOWERS(ch))
      return res;

   if (res == 1 && (in_room(ch) != room_from))
   {
      class unit_data *u = NULL;

      if (IS_CHAR(ch) && CHAR_FOLLOWERS(ch))
         u = ch;

      if (IS_CHAR(mover) && CHAR_FOLLOWERS(mover))
         u = mover;

      if (u && CHAR_FOLLOWERS(u))
      {
         int i, j;
         struct char_follow_type *k;

         for (i = 0;; i++) /* This shit is needed because the follow  */
         {                 /* structure can be destroyed by this move */
            for (j = 0, k = CHAR_FOLLOWERS(u); k && j < i; j++, k = k->next)
               ;

            if (k == NULL)
               break;

            if (room_from == in_room(k->follower) &&
                CHAR_POS(k->follower) >= POSITION_STANDING)
            {
               act("You follow $3n.<br/>", A_SOMEONE, k->follower, cActParameter(), ch, TO_CHAR);
               if (IS_ROOM(UNIT_IN(k->follower)))
                  self_walk(k->follower, k->follower, direction, TRUE);
               else
                  self_walk(k->follower, UNIT_IN(k->follower), direction, TRUE);
            }
         }
      }
   }

   return res;
}



/* dir must be one of [CMD_NORTH..MAX_EXIT]              */
void move_dir(class unit_data *ch, int dir)
{
   assert((dir >= 0) && (dir <= MAX_EXIT));

   if (CHAR_POS(ch) < POSITION_STANDING)
   {
      if (CHAR_POS(ch) == POSITION_FIGHTING)
         send_to_char("You are fighting for your life!<br/>", ch);
      else
         send_to_char("Perhaps you should get on your feet first?<br/>", ch);
      return;
   }

   if (IS_OBJ(UNIT_IN(ch)))
      self_walk(ch, UNIT_IN(ch), dir, 0);
   else if (IS_CHAR(UNIT_IN(ch)))
      self_walk(ch, UNIT_IN(ch), dir, 0);
   else if (IS_ROOM(UNIT_IN(ch)))
      self_walk(ch, ch, dir, 0);
   else
   {
      slog(LOG_ALL, 0, "Unit %s is inside an unexpected unit type %s", UNIT_NAME(ch), UNIT_NAME(UNIT_IN(ch)));
      act("Hmm. You shouldnt be in here. You're pushed out.", A_SOMEONE, ch, cActParameter(), cActParameter(), TO_CHAR);
      if (UNIT_IN(UNIT_IN(ch)))
         UNIT_IN(ch) = UNIT_IN(UNIT_IN(ch));
      command_interpreter(ch, "look");
      act("$1n appears out of thin air.", A_HIDEINV, ch, cActParameter(), cActParameter(), TO_REST);
   }
}

void do_move(class unit_data *ch, char *argument, const struct command_info *cmd)
{
   /* NOTICE! It uses cmd->no for efficiency, thus cmd->no MUST */
   /* be one of CMD_NORTH..CMD_DOWN which again MUST BE 0..5    */

   assert((cmd->no >= 0) && (cmd->no <= MAX_EXIT));

   move_dir(ch, cmd->no);
}

/* Returns -1 if door is not found, otherwise 0..5 for door exit     */
/* If err_msg points to anything, an appropriate message is copied   */
/* into it (which should be sent in case -1 is returned)             */
/* Will find ALL doors, including hidden doors.                      */
/* check_hidden: if FALSE, hidden doors will be ignored, otherwise   */
/*               hidden doors will be considered.                    */
/* err_msg:      if TRUE, error messages will be shown.              */

int low_find_door(class unit_data *ch, char *doorstr,
                  int err_msg, int check_hidden)
{
   char buf[256], dir[256];
   char *dirdoorstr;
   int door;

   dirdoorstr = one_argument(doorstr, dir);

   if (str_is_empty(dir))
   {
      if (err_msg)
         act("What?", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
      return -1;
   }

   if (!IS_ROOM(UNIT_IN(ch)))
   {
      if (err_msg)
         act("You see no such exit.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
      return -1;
   }

   if ((door = search_block(dir, dirs, FALSE)) != -1) /* Partial Match */
   {
      /* A direction and name was specified */
      if (str_is_empty(dirdoorstr))
      {
         if (ROOM_EXIT(UNIT_IN(ch), door) && (!check_hidden || has_found_door(ch, door)))
            return door;

         if (err_msg)
            act("You see no exit in that direction.", A_ALWAYS, ch, dirdoorstr, cActParameter(), TO_CHAR);

         return -1;
      }      

      if (ROOM_EXIT(UNIT_IN(ch), door))
      {
         if (ROOM_EXIT(UNIT_IN(ch), door)->open_name.IsName(dirdoorstr) &&
             (!check_hidden || has_found_door(ch, door)))
            return door;

         if (err_msg)
            act("You see no $2t in that direction.",
                A_ALWAYS, ch, dirdoorstr, cActParameter(), TO_CHAR);
         return -1;
      }

      if (err_msg)
         act("You see no exit in that direction.", A_ALWAYS, ch, cActParameter(), cActParameter(), TO_CHAR);
      return -1;
   }

   /* No direction was specified, try to locate the keyword */
   one_argument(doorstr, buf);

   for (door = 0; door <= MAX_EXIT; door++)
   {
      if (ROOM_EXIT(UNIT_IN(ch), door))
      {
         if (ROOM_EXIT(UNIT_IN(ch), door)->open_name.IsName(doorstr) &&
             (!check_hidden || has_found_door(ch, door)))
            return door;
      }
   }

   if (err_msg)
      act("You see no $2t here.", A_ALWAYS, ch, doorstr, cActParameter(), TO_CHAR);

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
      res = send_preprocess(ch, cmd_dirs[direction], buf);
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
         act("The $3t seems to be closed.",
             A_SOMEONE, ch, 0, ROOM_DOOR_NAME(room, direction), TO_CHAR);
      return 0;
   }

   if ((ROOM_LANDSCAPE(room) == SECT_WATER_SAIL) ||
       (ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room) ==
        SECT_WATER_SAIL))
   {
      send_to_char("You need sail in a boat to go there.<br/>", ch);
      return 0;
   }

   need_movement =
       (movement_loss[ROOM_LANDSCAPE(room)] +
        movement_loss[ROOM_LANDSCAPE(ROOM_EXIT(room, direction)->to_room)]) /
       2;

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

   dirbuf[0] = dirs[direction][0];

   c = single_unit_messg(room,
                         "$leave_o",
                         dirbuf,
                         "$1n leaves $3t.");

   if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
      act(c, A_HIDEINV, ch, room, dirs[direction], TO_ROOM);

   c = single_unit_messg(room, "$leave_s",
                         dirbuf, "");

   if (!str_is_empty(c))
      act(c, A_ALWAYS, ch, room, dirs[direction], TO_CHAR);

   unit_from_unit(ch);

   REMOVE_BIT(CHAR_FLAGS(ch), CHAR_LEGAL_TARGET | CHAR_SELF_DEFENCE);

   unit_to_unit(ch, to);

   dirbuf[0] = dirs[rev_dir[direction]][0];

   c = single_unit_messg(room, "$arrive_o", dirbuf,
                         "$1n has arrived from $3t.");

   if (!CHAR_HAS_FLAG(ch, CHAR_SNEAK) && !str_is_empty(c))
      act(c, A_HIDEINV, ch, to, enter_dirs[rev_dir[direction]],
          TO_ROOM);

   c = single_unit_messg(to, "$arrive_s", dirbuf, "");

   if (!str_is_empty(c))
      act(c, A_ALWAYS, ch, to,
          enter_dirs[rev_dir[direction]], TO_CHAR);

   command_interpreter(ch, "look");

   send_done(ch, ch, room, direction, &cmd_auto_enter, ""); // Send to was_in

   return (1);
}

void do_follow(class unit_data *ch, char *arg,
               const struct command_info *cmd)
{
   class unit_data *leader = NULL;

   void stop_follower(class unit_data * ch);
   void add_follower(class unit_data * ch, class unit_data * leader);

   if (str_is_empty(arg) ||
       ((leader = find_unit(ch, &arg, 0, FIND_UNIT_SURRO)) == ch))
   {
      if (CHAR_MASTER(ch))
      {
         act("You stop following $3n.", A_SOMEONE, ch, 0, CHAR_MASTER(ch), TO_CHAR);
         if (same_surroundings(ch, CHAR_MASTER(ch)))
            act("$1n stops following you.",
                A_HIDEINV, ch, 0, CHAR_MASTER(ch), TO_VICT);
         stop_following(ch);
      }
      else
         send_to_char("Who do you wish to follow?<br/>", ch);

      return;
   }

   if ((leader == NULL) || !IS_CHAR(leader))
   {
      send_to_char("You see no person by that name here!<br/>", ch);
      return;
   }

   if (CHAR_MASTER(ch))
   {
      act("You stop following $3n.", A_SOMEONE, ch, 0,
          CHAR_MASTER(ch), TO_CHAR);
      if (same_surroundings(ch, CHAR_MASTER(ch)))
         act("$1n stops following you.", A_HIDEINV,
             ch, 0, CHAR_MASTER(ch), TO_VICT);
      stop_following(ch);
   }

   start_following(ch, leader);

   act("You now follow $3n.", A_SOMEONE, ch, 0, leader, TO_CHAR);
   act("$1n starts following you.", A_HIDEINV, ch, 0, leader, TO_VICT);
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
      act("You must be on a mount if you want to ride.",
          A_ALWAYS, ch, 0, 0, TO_CHAR);
      return;
   }

   if (CHAR_FIGHTING(beast))
   {
      act("You must be in control to do that.",
          A_ALWAYS, ch, 0, 0, TO_CHAR);
      return;
   }

   for (class unit_data *u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = u->next)
      if (IS_CHAR(u) && CHAR_FIGHTING(u))
      {
         act("You can't just ride away in the middle of a combat.",
             A_ALWAYS, ch, 0, 0, TO_CHAR);
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
         act("The $3t seems to be closed.",
             A_SOMEONE, ch, 0, ROOM_DOOR_NAME(room, direction), TO_CHAR);
      return;
   }

   to_room = ROOM_EXIT(room, direction)->to_room;

   if (ROOM_LANDSCAPE(to_room) == SECT_WATER_SAIL ||
       ROOM_LANDSCAPE(to_room) == SECT_WATER_SWIM)
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
      act("You must be inside a boat if you want to sail.",
          A_ALWAYS, ch, 0, 0, TO_CHAR);
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
         act("You can't just sail away in the middle of a combat.",
             A_ALWAYS, ch, 0, 0, TO_CHAR);
         return;
      }

   /* Direction is possible */
   if (IS_SET(ROOM_EXIT(room, dir)->exit_info, EX_CLOSED))
   {
      /* XXX MS2020 backdoor(ch, arg, cmd); */

      act("The $3t seems to be closed.", A_SOMEONE, ch, 0,
          ROOM_DOOR_NAME(room, dir), TO_CHAR);
      return;
   }

   /* Direction is not closed */
   if (ROOM_EXIT(room, dir)->to_room == 0)
   {
      send_to_char("Alas, you can't sail that way.<br/>", ch);
      return;
   }

   if (ROOM_LANDSCAPE(room) != SECT_WATER_SAIL &&
       ROOM_LANDSCAPE(room) != SECT_WATER_SWIM &&
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

   res = send_preprocess(boat, cmd_dirs[direction], buf);

   for (u = UNIT_CONTAINS(boat); u; u = u->next)
      if (!pay_point_charlie(u, to))
         return 0;

   if (boat->is_destructed())
      return -1;

   if (res != SFR_SHARE || was_in != UNIT_IN(boat))
      return 0;

   unit_from_unit(boat);

   if (UNIT_CONTAINS(was_in))
      act("$2n sails $3t.",
          A_HIDEINV, UNIT_CONTAINS(was_in), boat, dirs[direction], TO_ALL);

   if (UNIT_CONTAINS(to))
      act("$2n has arrived from $3t.", A_HIDEINV,
          UNIT_CONTAINS(to),
          boat, enter_dirs[rev_dir[direction]], TO_ALL);

   unit_to_unit(boat, to);

   for (u = UNIT_CONTAINS(boat); u; u = u->next)
      if (IS_CHAR(u))
      {
         act("$1n sails $2t with you.",
             A_SOMEONE, boat, dirs[direction], u, TO_VICT);
         command_interpreter(u, "look");
      }

   send_done(boat, captain, was_in, direction, &cmd_auto_enter, "");

   return 1;
}

/* For riding mounts! */
int do_simple_ride(class unit_data *beast,
                   class unit_data *master, int direction)

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
   res = send_preprocess(beast, cmd_dirs[direction], buf);

   for (u = UNIT_CONTAINS(beast); u; u = u->next)
      if (!pay_point_charlie(u, to))
         return 0;

   if (beast->is_destructed())
      return -1;

   if ((res != SFR_SHARE) || (was_in != UNIT_IN(beast)))
      return 0;

   if ((ROOM_LANDSCAPE(UNIT_IN(beast)) == SECT_WATER_SAIL) ||
       (ROOM_LANDSCAPE(to) == SECT_WATER_SAIL))
   {
      act("$1n refuses to go there.", A_SOMEONE, beast, 0, 0, TO_ROOM);
      return 0;
   }

   need_movement =
       (movement_loss[ROOM_LANDSCAPE(UNIT_IN(beast))] +
        movement_loss[ROOM_LANDSCAPE(to)]) /
       2;

   if (CHAR_ENDURANCE(beast) < need_movement)
   {
      send_to_char("The beast is too exhausted.<br/>", UNIT_CONTAINS(beast));
      return 0;
   }

   CHAR_ENDURANCE(beast) -= need_movement;

   unit_from_unit(beast);

   if (UNIT_CONTAINS(was_in))
      act("$2n rides $3t.",
          A_HIDEINV, UNIT_CONTAINS(was_in), beast, dirs[direction], TO_ALL);

   if (UNIT_CONTAINS(to))
      act("$2n has arrived from $3t.", A_HIDEINV,
          UNIT_CONTAINS(to),
          beast, enter_dirs[rev_dir[direction]], TO_ALL);

   unit_to_unit(beast, to);

   for (u = UNIT_CONTAINS(beast); u; u = u->next)
   {
      if (IS_CHAR(u))
      {
         act("$1n rides $2t with you.",
             A_SOMEONE, beast, dirs[direction], u, TO_VICT);
         command_interpreter(u, "look");
      }
   }

   send_done(beast, master, was_in, direction, &cmd_auto_enter, "");

   return 1;
}


#endif
