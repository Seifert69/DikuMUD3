/*
 $Author: All $
 $RCSfile: fight.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "common.h"
#include "vmelimits.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "spells.h"
#include "textutil.h"
#include "affect.h"
#include "justice.h"
#include "utility.h"
#include "protocol.h"
#include "fight.h"
#include "combat.h"
#include "files.h"
#include "dilrun.h"

/* Structures */

struct combat_single_msg
{
   char *to_char;
   char *to_vict;
   char *to_notvict;
};

struct combat_msg_packet
{
   struct combat_single_msg cmiss;
   struct combat_single_msg shield;
   struct combat_single_msg nodam;
   struct combat_single_msg entire;
   struct combat_single_msg head;
   struct combat_single_msg hand;
   struct combat_single_msg arm;
   struct combat_single_msg body;
   struct combat_single_msg legs;
   struct combat_single_msg feet;
   struct combat_single_msg die;

   struct combat_msg_packet *next;
};

struct combat_msg_list
{
   int group;      /* Which message group      */
   int *no;        /* Which message number #'s */
   int no_of_msgs; /* # of msgs to choose from */
   struct combat_msg_packet *msg;
};

struct combat_msg_list fight_messages[COM_MAX_MSGS];

/* External structures */

extern class unit_data *unit_list;

/* External procedures */

void gain_exp(class unit_data *ch, int gain);
char *fread_string(FILE *f1);
void stop_follower(class unit_data *ch);

/* Returns TRUE if combat is not allowed, and FALSE if combat is allowed */
/* Also used for steal, etc.                                             */
/* If message is true, then a message is sent to the 'att'               */

int pk_test(class unit_data *att, class unit_data *def, int message)
{
   if (IS_PC(att) && IS_PC(def) && att != def)
   {
      if (CHAR_LEVEL(att) <= 3)
      {
         if (message)
            act("You are not old enough to do that!",
                A_ALWAYS, att, 0, def, TO_CHAR);
         return TRUE;
      }
      if (CHAR_LEVEL(def) <= 3)
      {
         if (message)
            act("$3e is too young to die now!",
                A_ALWAYS, att, 0, def, TO_CHAR);
         return TRUE;
      }
   }

   if (!g_cServerConfig.m_bBOB)
      return FALSE;

   if ((att != def) && IS_PC(att) && IS_PC(def) &&
       (!IS_SET(PC_FLAGS(att), PC_PK_RELAXED) ||
        !IS_SET(PC_FLAGS(def), PC_PK_RELAXED)))
   {
      /* Ok, he's allowed to attack only if the other guy is rewarded! */
      /* if (affected_by_spell(def, ID_REWARD))
           return FALSE; */

      if (message)
      {
         if (!IS_SET(PC_FLAGS(att), PC_PK_RELAXED))
            act("You are not allowed to do this unless you sign the "
                "book of blood.",
                A_ALWAYS, att, 0, def, TO_CHAR);
         else
            act("You are not allowed to do this until $3n signs the "
                "book of blood.",
                A_ALWAYS, att, 0, def, TO_CHAR);
      }
      return TRUE;
   }

   return FALSE;
}

int char_dual_wield(class unit_data *ch)
{
   return equipment_type(ch, WEAR_WIELD, ITEM_WEAPON) &&
          equipment_type(ch, WEAR_HOLD, ITEM_WEAPON);
}

/* Given an amount of experience, what is the 'virtual' level of the char? */
int virtual_level(class unit_data *ch)
{
   if (IS_NPC(ch))
      return CHAR_LEVEL(ch);
   else
   {
      if (IS_MORTAL(ch))
      {
         /* Above max mortal is just max mortal (shouldnt be possible) */
         if (CHAR_LEVEL(ch) >= MORTAL_MAX_LEVEL)
            return CHAR_LEVEL(ch);

         if (CHAR_EXP(ch) < required_xp(1 + PC_VIRTUAL_LEVEL(ch)))
            return CHAR_LEVEL(ch);
         else
            return CHAR_LEVEL(ch) + 1 +
                   (CHAR_EXP(ch) - required_xp(1 + CHAR_LEVEL(ch))) /
                       level_xp(CHAR_LEVEL(ch));
      }
      else
         return CHAR_LEVEL(ch);
   }
}

/* -------------------------------------------------------------------- */

int num_in_msg(struct combat_msg_list *msg, int no)
{
   int i;

   for (i = 0; msg->no[i] != -1; i++)
      if (msg->no[i] == no)
         return TRUE;

   return FALSE;
}

void fread_single(FILE *f1, struct combat_single_msg *msg)
{
   msg->to_char = fread_string(f1);
   msg->to_vict = fread_string(f1);
   msg->to_notvict = fread_string(f1);
   int mstmp = fscanf(f1, " ");
   if (mstmp < 0)
   {
      slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
      assert(FALSE);
   }
}

int load_msg_prehead(FILE *f1, struct combat_msg_list *msg)
{
   int no[200], pos, grp;
   char shit[100];

   /* A group of -1 indicates end */
   int mstmp = fscanf(f1, " %d", &grp);
   if (mstmp < 0)
   {
      slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
      assert(FALSE);
   }

   if (grp == -1)
      return -1;

   pos = 0;

   do
   {
      mstmp = fscanf(f1, " %d", &no[pos++]);
      if (mstmp < 1)
      {
         slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
         assert(FALSE);
      }
   } while ((no[pos - 1] != -1) && (pos < 199));

   no[pos - 1] = -1;

   mstmp = fscanf(f1, " %[^\n\r] ", shit); /* Skip any remark */
   if (mstmp < 0)
   {
      slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in fight.cpp", mstmp);
      assert(FALSE);
   }

   CREATE(msg->no, int, pos);
   for (pos--; pos >= 0; pos--)
      msg->no[pos] = no[pos];

   return grp;
}

void load_messages(void)
{
   FILE *f1;
   int i, grp;
   struct combat_msg_packet *messages;

   if (!(f1 = fopen(str_cc(g_cServerConfig.m_etcdir, MESS_FILE), "r")))
   {
      slog(LOG_ALL, 0, "Unable to read fight messages.");
      exit(11);
   }

   char shit[200];
   fread_line_commented(f1, shit, sizeof(shit));

   for (i = 0; i < COM_MAX_MSGS; i++)
   {
      fight_messages[i].group = -1;
      fight_messages[i].no = 0; /* Nil */
      fight_messages[i].no_of_msgs = 0;
      fight_messages[i].msg = 0;
   }

   i = 0;
   while (TRUE)
   {
      grp = load_msg_prehead(f1, &fight_messages[i]);

      if (grp == -1)
         break;

      if (i >= COM_MAX_MSGS)
      {
         slog(LOG_OFF, 0, "Too many combat messages.");
         exit(12);
      }

      CREATE(messages, struct combat_msg_packet, 1);

      fight_messages[i].no_of_msgs++;
      fight_messages[i].group = grp;
      /* fight_messages[i].no already set in load_msg_prehead */
      messages->next = fight_messages[i].msg;
      fight_messages[i].msg = messages;

      fread_single(f1, &(messages->cmiss));
      fread_single(f1, &(messages->shield));
      fread_single(f1, &(messages->nodam));
      fread_single(f1, &(messages->entire));
      fread_single(f1, &(messages->head));
      fread_single(f1, &(messages->hand));
      fread_single(f1, &(messages->arm));
      fread_single(f1, &(messages->body));
      fread_single(f1, &(messages->legs));
      fread_single(f1, &(messages->feet));
      fread_single(f1, &(messages->die));

      i++;
   }

   fclose(f1);
}

/* -------------------------------------------------------------------- */

/* Returns index in [0..8] */
int damage_index(int dam, int maxhp)
{
   int p;

   if (dam < 1)
      return 0;
   else if (dam >= maxhp)
      return 8;

   p = (100 * dam) / maxhp; /* Calculate relative damage (%) */

   if (p < 5)
      return 0;
   else if (p < 10)
      return 1;
   else if (p < 15)
      return 2;
   else if (p < 20)
      return 3;
   else if (p < 25) /* 20%..25% is default at own level */
      return 4;
   else if (p < 35)
      return 5;
   else if (p < 50)
      return 6;
   else if (p < 70)
      return 7;
   else
      return 8;
}

char *sub_damage(char *str, int damage, int max_hp)
{
   static char buf[256];
   char *cp;

   /* We can always make more/new blocks */
   const char *damage_blocks[16][9] = {
       {"ineptly", /*       A       */
        "awkwardly",
        "clumsily",
        "simply", /* How it appears/looks */
        "competently",
        "smoothly",
        "elegantly",
        "expertly",
        "superbly"},

       {"meekly", /*       B      */
        "tamely",
        "gently",
        "moderately", /* The temper/mood of the attacker */
        "boldly",
        "cruelly",
        "grimly",
        "viciously",
        "savagely"},

       {"caress", /*       C       */
        "tickle",
        "scratch",
        "cut", /*   Cut and Slash Weaponry 1 */
        "chop",
        "gash",
        "lacerate",
        "mangle",
        "mutilate"},

       {"caresses", /*       D       */
        "tickles",
        "scratches",
        "cuts", /*   Cut and Slash Weaponry 2 */
        "chops",
        "gashes",
        "lacerates",
        "mangles",
        "mutilates"},

       {"caressed", /*       E       */
        "tickled",
        "scratched",
        "cut", /*   Cut and Slash Weaponry 1 */
        "chopped",
        "gashed",
        "lacerated",
        "mangled",
        "mutilated"},

       {"comb", /*       F       */
        "shave",
        "scratch",
        "rip", /*   Cut and Slash Weaponry 3 */
        "slash",
        "rend",
        "incise",
        "rupture",
        "mince"},

       {"combs", /*       G       */
        "shaves",
        "scratches",
        "rips", /*   Cut and Slash Weaponry 4 */
        "slashes",
        "rends",
        "incises",
        "ruptures",
        "minces"},

       {"combed", /*       H       */
        "shaved",
        "scratched",
        "ripped", /*   Cut and Slash Weaponry 3 */
        "slashed",
        "rended",
        "incised",
        "ruptured",
        "minced"},

       {"nudge", /*       I       */
        "graze",
        "whack",
        "beat", /*   Bludgeon Weaponry   */
        "thump",
        "flog",
        "pound",
        "slam",
        "batter"},

       {"nudges", /*       J      */
        "grazes",
        "whacks",
        "beats", /*   Bludgeon Weaponry   */
        "thumps",
        "flogs",
        "pounds",
        "slams",
        "batters"},

       {"nudged", /*       K       */
        "grazed",
        "whacked",
        "beaten", /*   Bludgeon Weaponry   */
        "thumped",
        "flogged",
        "pounded",
        "slammed",
        "battered"},

       {"poke", /*       L      */
        "sting",
        "prick",
        "stab", /*   Piercing Weaponry   */
        "pierce",
        "spike",
        "penetrate",
        "lance",
        "impale"},

       {"pokes", /*       M      */
        "stings",
        "pricks",
        "stabs", /*   Piercing Weaponry   */
        "pierces",
        "spikes",
        "penetrates",
        "lances",
        "impales"},

       {"poked", /*       N      */
        "stung",
        "pricked",
        "stabbed", /*   Piercing Weaponry   */
        "pierced",
        "spiked",
        "penetrated",
        "lanced",
        "impaled"},

       {"insignificantly", /*       O      */
        "slightly",
        "lightly",
        "fairly", /*   Adverbes   */
        "seriously",
        "severely",
        "gravely",
        "critically",
        "fatally"},

       {"nudge", /*       P       */
        "graze",
        "bump",
        "knock", /*   Fist        */
        "strike",
        "thump",
        "batter",
        "redesign",
        "shatter"}};

   assert(str);

   cp = buf;

   for (; *str; str++)
      if (*str == '#')
      {
         str++;
         if (*str >= 'A' && *str <= 'P')
            strcpy(cp,
                   damage_blocks[*str - 'A'][damage_index(damage, max_hp)]);
         else
         {
            slog(LOG_ALL, 0, "Illegal damage block reference.");
            strcpy(cp, "ILLEGAL BLOCK");
         }
         TAIL(cp);
      }
      else
         *(cp++) = *str;

   *cp = 0;

   return buf;
}

static void combat_send(struct combat_single_msg *msg,
                        class unit_data *arg1,
                        class unit_data *arg2,
                        class unit_data *arg3,
                        int dam, const char *color1, const char *color2, const char *color3)
{
   if (msg->to_char)
   {
      cact(sub_damage(msg->to_char, dam, UNIT_MAX_HIT(arg3)),
           A_SOMEONE, arg1, arg2, arg3, TO_CHAR, color1);
   }

   if (msg->to_vict)
   {
      cact(sub_damage(msg->to_vict, dam, UNIT_MAX_HIT(arg3)),
           A_ALWAYS, arg1, arg2, arg3, TO_VICT, color2);
   }

   if (msg->to_notvict)
   {
      cact(sub_damage(msg->to_notvict, dam, UNIT_MAX_HIT(arg3)),
           A_SOMEONE, arg1, arg2, arg3, TO_NOTVICT, color3);
   }
}

/* Hit Location is one of:                                */
/* WEAR_HEAD, WEAR_HANDS, WEAR_ARMS, WEAR_BODY,           */
/* WEAR_LEGS or WEAR_FEET for actual body locations.      */
/* Use WEAR_SHIELD if hit is blocked by a shield          */
/* Use COM_MSG_DEAD when a killing blow                   */
/* Use COM_MSG_MISS when a complete miss                  */
/* Use COM_MSG_NODAM when a hit but no damage is given    */

void combat_message(class unit_data *att,
                    class unit_data *def,
                    class unit_data *medium,
                    int damage, int msg_group, int msg_number, int hit_location)
{
   struct combat_msg_packet *msg;
   int i, r;

   msg = 0;

   for (i = 0; i < COM_MAX_MSGS; i++)
      if ((fight_messages[i].group == msg_group) &&
          num_in_msg(&fight_messages[i], msg_number))
      {
         r = number(0, fight_messages[i].no_of_msgs - 1);
         msg = fight_messages[i].msg;
         for (; msg && r; msg = msg->next, r--)
            ;
         break;
      }

   if (msg)
   {
      switch (hit_location)
      {
      case COM_MSG_MISS:
         combat_send(&(msg->cmiss), att, medium, def, damage, "miss_me",
                     "miss_opponent", "miss_other");
         break;

      case COM_MSG_EBODY:
         combat_send(&(msg->entire), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case COM_MSG_NODAM:
         combat_send(&(msg->nodam), att, medium, def, damage, "nodam_me",
                     "nodam_opponent", "nodam_other");
         break;

      case WEAR_BODY:
         combat_send(&(msg->body), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_ARMS:
         combat_send(&(msg->arm), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_LEGS:
         combat_send(&(msg->legs), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_HEAD:
         combat_send(&(msg->head), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_HANDS:
         combat_send(&(msg->hand), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_FEET:
         combat_send(&(msg->feet), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      case WEAR_SHIELD:
         combat_send(&(msg->shield), att, medium, def, damage, "shield_me",
                     "shield_opponent", "shield_other");
         break;

      case COM_MSG_DEAD:
         combat_send(&(msg->die), att, medium, def, damage, "hit_me",
                     "hit_opponent", "hit_other");
         break;

      default:
         act("Undefined hitlocation!", A_SOMEONE, att, 0, 0, TO_ALL);
         break;
      }
   }
   else
   {
      act("Undefined message numbers!", A_SOMEONE, att, 0, 0, TO_ALL);
      slog(LOG_ALL, 0, "combat_message: Unknown message group %d, number %d", msg_group, msg_number);
   }
}

/* -------------------------------------------------------------------- */

void update_pos(class unit_data *victim)
{

   if ((UNIT_HIT(victim) > 0) && (CHAR_POS(victim) > POSITION_STUNNED))
      return;
   else if (UNIT_HIT(victim) > 0)
      CHAR_POS(victim) = POSITION_STANDING;
   else if (UNIT_HIT(victim) <= -11)
      CHAR_POS(victim) = POSITION_DEAD;
   else if (UNIT_HIT(victim) <= -6)
      CHAR_POS(victim) = POSITION_MORTALLYW;
   else if (UNIT_HIT(victim) <= -3)
      CHAR_POS(victim) = POSITION_INCAP;
   else
      CHAR_POS(victim) = POSITION_STUNNED;
}

/* -------------------------------------------------------------------- */

/* When ch kills victim */
static void change_alignment(class unit_data *slayer, class unit_data *victim)
{
   int adjust = 0;
   int diff = 0;

   diff = UNIT_ALIGNMENT(slayer) - UNIT_ALIGNMENT(victim);

   if (UNIT_IS_GOOD(slayer))
   {
      if (UNIT_IS_EVIL(victim)) /* Diff >= 700 */
         adjust = (diff - 700) / 70 + 1;
      else if (UNIT_IS_NEUTRAL(victim))
      {
         if (UNIT_ALIGNMENT(victim) <= -100)
            adjust = 1;
         else if (UNIT_ALIGNMENT(victim) >= 100)
            adjust = -3;
      }
      else /* Victim is good */
         adjust =
             -(UNIT_ALIGNMENT(slayer) + UNIT_ALIGNMENT(victim) - 700) / 5 - 3;
   }
   else if (UNIT_IS_EVIL(slayer))
   {
      if (UNIT_IS_GOOD(victim)) /* Diff <= -700 */
         adjust = (diff + 700) / 35 - 2;
      else if (diff > 0)
         adjust = +5;
   }
   else /* Neutral slayer - diff in -1350..+1350 */
   {
      if (diff > 350)
         adjust = (diff - 350) / 30 + 1;
      else if (diff < -350)
         adjust = (diff + 350) / 30 - 1;
   }

   diff = CHAR_LEVEL(slayer) - CHAR_LEVEL(victim);
   if (diff > 0)
      adjust += (adjust * MIN(10, diff)) / 10;
   else if (diff < 0)
      adjust += (adjust * MAX(-10, diff)) / 20;

   adjust = MIN(200, adjust);
   adjust = MAX(-200, adjust);

   UNIT_ALIGNMENT(slayer) += adjust;
   UNIT_ALIGNMENT(slayer) = MIN(1000, UNIT_ALIGNMENT(slayer));
   UNIT_ALIGNMENT(slayer) = MAX(-1000, UNIT_ALIGNMENT(slayer));
}

/* Do all the gain stuff for CH where no is the number of players */
/* which are to share the "share" of experience                   */
static void person_gain(class unit_data *ch, class unit_data *dead,
                        int share, int grouped, int maxlevel)
{
   if (share > 0)
   {
      if (UNIT_IS_GOOD(ch) && UNIT_IS_EVIL(dead))
         share += share / 5;

      if (UNIT_IS_EVIL(ch) && UNIT_IS_GOOD(dead))
         share += share / 10;
   }

   if (IS_PC(ch))
   {
      if (share > 0)
      {
         if (IS_SET(CHAR_FLAGS(ch), CHAR_WIMPY))
            share /= 2; /* Only 50% in wimpy */

         if (CHAR_LEVEL(ch) < maxlevel - 5)
            share -=
                (MIN(95, 7 * (maxlevel - (CHAR_LEVEL(ch) + 5))) * share) /
                100;
      }

      gain_exp(ch, share);

      if (share > 0)
      {
         cact("You receive $2d experience points.",
              A_ALWAYS, ch, &share, 0, TO_CHAR, "xpgain");
      }
      else if (share == 0)
      {
         cact("You receive no experience points.",
              A_ALWAYS, ch, 0, 0, TO_CHAR, "xpgain");
      }
      else /* less than zero */
      {
         cact("You are penalized by $2d experience.",
              A_ALWAYS, ch, &share, 0, TO_CHAR, "xpgain");
      }
   }

   change_alignment(ch, dead);
}

/* This takes 100% care of victim and attackers change in experience */
/* and alignments. Do not fiddle with these values anywhere else     */
/* when a kill has been made                                         */
static void exp_align_gain(class unit_data *ch, class unit_data *victim)
{
   int rellevel, sumlevel, maxlevel, minlevel, no_members = 1, share;
   class unit_data *head;
   struct char_follow_type *f;

   int experience_modification(class unit_data * att, class unit_data * def);

   maxlevel = CHAR_LEVEL(ch);

   if (IS_PC(victim) && IS_SET(PC_FLAGS(victim), PC_SPIRIT))
   {
      act("Oh dear, what a mess!", A_SOMEONE, victim, 0, 0, TO_ROOM);
      slog(LOG_EXTENSIVE, 0, "Oh dear, a spirit was killed!");
      return;
   }

   head = ch; /* Who is head of potential group? */

   if (CHAR_HAS_FLAG(ch, CHAR_GROUP) && CHAR_MASTER(ch) &&
       CHAR_HAS_FLAG(CHAR_MASTER(ch), CHAR_GROUP) &&
       same_surroundings(ch, CHAR_MASTER(ch)))
      head = CHAR_MASTER(ch);

   if (IS_PC(victim))
      share = 0;
   else /* NPC killed */
   {
      class unit_affected_type *paf;

      paf = affected_by_spell(victim, ID_MAX_ATTACKER);

      if (paf)
         maxlevel = MAX(virtual_level(head), paf->data[0]);
      else
         maxlevel = virtual_level(head);

      minlevel = maxlevel;
      sumlevel = maxlevel;
      no_members = 1;

      if (CHAR_HAS_FLAG(ch, CHAR_GROUP))
      {
         for (f = CHAR_FOLLOWERS(head); f; f = f->next)
            if (CHAR_HAS_FLAG(f->follower, CHAR_GROUP) &&
                same_surroundings(head, f->follower))
            {
               sumlevel += virtual_level(f->follower);

               maxlevel = MAX(virtual_level(f->follower), maxlevel);
               minlevel = MIN(virtual_level(f->follower), minlevel);

               no_members++;
            }

         assert((no_members > 0) && (sumlevel >= 0));
      }

      share = CHAR_EXP(victim);

      if (share > 0)
      {
         /* rellevel = virtual_level(victim) - maxlevel - (no_members - 1); */
         rellevel = virtual_level(victim) - maxlevel;

         /* Check for level difference */

         if (rellevel < 0)
            share = (MAX(0, 100 + 15 * rellevel) * share) / 100;
         else if (rellevel > 0)
            share = ((100 + 15 * rellevel) * share) / 100;

         if (no_members > 1)
            share = (4 * share) / (3 + no_members);

         share = MIN(100 * no_members + 300, share);
      }
   }

   share = (8 * share) / 10; /* At 90% now... */

   if (!CHAR_HAS_FLAG(ch, CHAR_GROUP))
      person_gain(ch, victim, share, FALSE, maxlevel);
   else
   {
      if (CHAR_HAS_FLAG(head, CHAR_GROUP) && same_surroundings(head, ch))
      {
         person_gain(head, victim, share, (no_members > 1), maxlevel);
      }

      for (f = CHAR_FOLLOWERS(head); f; f = f->next)
         if (CHAR_HAS_FLAG(f->follower, CHAR_GROUP) &&
             same_surroundings(f->follower, ch))
         {
            person_gain(f->follower, victim, share,
                        (no_members > 1), maxlevel);
         }
   }
}

int lose_exp(class unit_data *ch)
{
   int loss, i;

   assert(IS_PC(ch));

   /* This first line takes care of any xp earned above required level. */

   loss = MAX(0, (CHAR_EXP(ch) - required_xp(PC_VIRTUAL_LEVEL(ch))) / 2);

   /* This line makes sure, that you lose at most half a level...       */

   loss = MIN(loss, level_xp(PC_VIRTUAL_LEVEL(ch)) / 2);

   /* This line takes care of the case where you have less or almost    */
   /* equal XP to your required. You thus lose at least 1/5th your      */
   /* level.                                                            */

   loss = MAX(loss, level_xp(PC_VIRTUAL_LEVEL(ch)) / 5);

   /* This line takes care of newbies, setting the lower bound... */
   i = MAX(0, (CHAR_EXP(ch) - required_xp(START_LEVEL)) / 2);

   if (loss > i)
      loss = i;

   assert(loss >= 0);

   return loss;
}

/* Die is only called when a PC or NPC is killed for real, causing XP loss
   and transfer of rewards */

void die(class unit_data *ch)
{
   struct diltemplate *death;

   if (ch->is_destructed())
      return;

   death = find_dil_template("death@death");
   if (death)
   {
      send_death(ch);
      class dilprg *prg = dil_copy_template(death, ch, NULL);
      prg->waitcmd = WAITCMD_MAXINST - 1;
      dil_activate(prg);
      return;
   }
}

/* -------------------------------------------------------------------- */

/* Call when adding or subtracting hitpoints to/from a character */
void modify_hit(class unit_data *ch, int hit)
{
   if (CHAR_POS(ch) > POSITION_DEAD)
   {
      UNIT_HIT(ch) += hit;
      UNIT_HIT(ch) = MIN(hit_limit(ch), UNIT_HIT(ch));

      update_pos(ch);

      if (CHAR_POS(ch) == POSITION_DEAD)
         die(ch);
   }
}

void damage(class unit_data *ch, class unit_data *victim,
            class unit_data *medium,
            int dam, int attack_group, int attack_number, int hit_location,
            int bDisplay)
{
   int max_hit;
   class unit_affected_type *paf;
   class unit_data *sch;

   if (ch->is_destructed() || victim->is_destructed())
      return;

   /* Minimum is 0 damage points [ no maximum! ] */
   dam = MAX(dam, 0);

   /* If neither are allowed to attack each other... */
   if (pk_test(ch, victim, FALSE) && pk_test(victim, ch, FALSE))
      dam = 0;

   if (IS_IMMORTAL(victim))
      dam = 0;

   if ((CHAR_POS(victim) == POSITION_SLEEPING) && (dam > 0))
   {
      CHAR_POS(victim) = POSITION_RESTING;
      send_to_char("OUCH! You wake up!<br/>", victim);
   }

   if (victim != ch)
   {
      if (CHAR_LAST_ATTACKER(victim))
         FREE(CHAR_LAST_ATTACKER(victim));
      if (ch->names.Name())
         CHAR_LAST_ATTACKER(victim) = str_dup(ch->names.Name());
      CHAR_LAST_ATTACKER_TYPE(victim) = UNIT_TYPE(ch);
      if (IS_SET(CHAR_FLAGS(victim), CHAR_HIDE))
      {
         if ((paf = affected_by_spell(victim, ID_HIDE)))
            destroy_affect(paf);
         REMOVE_BIT(CHAR_FLAGS(victim), CHAR_HIDE);
      }

      if (IS_SET(UNIT_FLAGS(victim), UNIT_FL_INVISIBLE))
      {
         while ((paf = affected_by_spell(victim, ID_INVISIBILITY)))
            destroy_affect(paf);
         REMOVE_BIT(UNIT_FLAGS(victim), UNIT_FL_INVISIBLE);
      }

      if ((paf = affected_by_spell(ch, ID_SANCTUARY)))
         destroy_affect(paf);

      offend_legal_state(ch, victim);

      /* Set victim in fighting position
           if (CHAR_POS(victim) > POSITION_STUNNED)
           {  Don't check for STUNNED! Victim must be set fighting for murder
           resolution!!! */

      if (!CHAR_FIGHTING(victim))
      {
         if (IS_PC(victim) && IS_IMMORTAL(victim) && IS_PC(ch))
         {
            cact("$1n ignores your feeble threats.",
                 A_ALWAYS, victim, 0, ch, TO_VICT, "miss_me");
            cact("$1n ignores $3n's feeble threats.",
                 A_ALWAYS, victim, 0, ch, TO_NOTVICT, "miss_other");
         }
         else
            set_fighting(victim, ch, TRUE);
      }
      else
      {
         set_fighting(ch, victim); // If already fighting it makes no diff
      }

      if (CHAR_MASTER(victim) == ch)
      {
         stop_following(victim);
         send_to_char("You stop following the jerk that is hitting you.<br/>", victim);
      }
   }

   char arg[40];

   sprintf(arg, "%d %d %d", attack_group, attack_number, hit_location);

   if (send_ack(ch, medium, victim, &dam,
                &cmd_auto_damage, arg, victim) == SFR_BLOCK)
      return;
   if (victim != ch)
   {
      if ((paf = affected_by_spell(victim, ID_MAX_ATTACKER)))
         paf->data[0] = MAX(CHAR_LEVEL(ch), paf->data[0]);
      else
      {
         class unit_affected_type af;

         af.id = ID_MAX_ATTACKER;
         af.duration = 4;
         af.beat = WAIT_SEC * 60 * 5;
         af.firstf_i = TIF_NONE;
         af.tickf_i = TIF_NONE;
         af.lastf_i = TIF_NONE;
         af.applyf_i = APF_NONE;
         af.data[0] = CHAR_LEVEL(ch);
         af.data[1] = af.data[2] = 0;

         create_affect(victim, &af);
      }
   }
   UNIT_HIT(victim) -= dam;

   update_pos(victim);

   if (bDisplay)
   {
      if (CHAR_POS(victim) > POSITION_DEAD)
         combat_message(ch, victim, medium, dam,
                        attack_group, attack_number, hit_location);
      else
         combat_message(ch, victim, medium, dam,
                        attack_group, attack_number, COM_MSG_DEAD);
   }

   switch (CHAR_POS(victim))
   {
   case POSITION_MORTALLYW:
      send_to_char("You are mortally wounded!<br/>", victim);
      act("$1n is mortally wounded!", A_SOMEONE, victim, 0, 0, TO_ROOM);
      break;

   case POSITION_INCAP:
      send_to_char("You are incapacitated!<br/>", victim);
      act("$1n is incapacitated!", A_SOMEONE, victim, 0, 0, TO_ROOM);
      break;

   case POSITION_STUNNED:
      act("$1n is stunned, but will probably regain consciousness again.",
          A_HIDEINV, victim, 0, 0, TO_ROOM);
      act("You're stunned, but will probably regain consciousness again.",
          A_SOMEONE, victim, 0, 0, TO_CHAR);
      break;

   case POSITION_DEAD:
      act("$1n is dead!", A_HIDEINV, victim, 0, 0, TO_ROOM);
      break;

   default: /* >= POSITION SLEEPING */
      assert(CHAR_POS(victim) >= POSITION_SLEEPING);

      max_hit = hit_limit(victim);

      if (dam > (max_hit / 5))
      {
         cact("That Really did HURT!", A_SOMEONE, victim, 0, 0, TO_CHAR,
              "hit_me");
         cact("$1n screams with agony!", A_SOMEONE, victim, 0, 0, TO_ROOM,
              "hit_other");
      }

      if (UNIT_HIT(victim) < (max_hit / 5))
         cact("You wish that your wounds would stop BLEEDING that much!",
              A_SOMEONE, victim, 0, 0, TO_CHAR, "hit_me");

      if ((dam > UNIT_HIT(victim) / 4) || (UNIT_HIT(victim) < (max_hit / 4)))
         if (IS_SET(CHAR_FLAGS(victim), CHAR_WIMPY))
         {
            command_interpreter(victim, "flee");
            return;
         }
      break;
   }

   /* MS: Moved check to stop_fighting till after DEAD check so that the
       special routines can check WHO actually killed the person in question
       by testing is FIGHTING(u) == victim. When FIGHTING(u) is NULL
       it is supposed to equal one self... */

   /* Murder! */
   if (CHAR_POS(victim) == POSITION_DEAD)
   {
      sch = NULL;
      if (ch == victim)
      {
         if (CHAR_LAST_ATTACKER(victim))
         {
            for (sch = UNIT_CONTAINS(UNIT_IN(victim)); sch;
                 sch = sch->next)
            {
               if (CHAR_LAST_ATTACKER(victim) && sch->names.Name())
                  if (strcmp(CHAR_LAST_ATTACKER(victim),
                             sch->names.Name()) == 0)
                     if (CHAR_LAST_ATTACKER_TYPE(victim) == UNIT_TYPE(sch))
                        break;
            }
         }
         if (sch)
            ch = sch;
      }

      if (ch != victim)
      {
         exp_align_gain(ch, victim);

         /* Except in self defence & legal target makes a crime */
         if (!IS_SET(CHAR_FLAGS(victim), CHAR_LEGAL_TARGET) &&
             !IS_SET(CHAR_FLAGS(ch), CHAR_SELF_DEFENCE))
         {
            if (IS_PC(victim))
            {
               if (!IS_SET(CHAR_FLAGS(victim), CHAR_OUTLAW) ||
                   IS_SET(CHAR_FLAGS(victim), CHAR_PROTECTED))
                  log_crime(ch, victim, CRIME_PK);
            }
            else
            {
               if (IS_SET(CHAR_FLAGS(victim), CHAR_PROTECTED))

                  log_crime(NULL, victim, CRIME_MURDER);
            }
         }
      }

      if (!IS_NPC(victim))
      {
         slog(LOG_EXTENSIVE, MAX(UNIT_MINV(ch), UNIT_MINV(victim)),
              "%s killed by %s at %s", STR(UNIT_NAME(victim)),
              TITLENAME(ch), STR(TITLENAME(UNIT_IN(ch))));
      }

      if (victim == ch)
      {
         SET_BIT(CHAR_FLAGS(ch), CHAR_KILL_SELF);
         die(ch);
      }
      else
      {
         stop_fighting(victim);
         set_fighting(victim, ch, TRUE);
         set_fighting(ch, victim, TRUE);

         die(victim); /* Lose experience */
      }

      return;
   }

   if (CHAR_POS(victim) <= POSITION_STUNNED)
   {
      if (CHAR_FIGHTING(victim))
         stop_fighting(victim);
      if (CHAR_FIGHTING(ch) == victim)
         stop_fighting(ch);
   }

   if (IS_PC(victim) && !CHAR_DESCRIPTOR(victim) && CHAR_AWAKE(victim))
   {
      command_interpreter(victim, "flee");
      if (victim->is_destructed() || CHAR_POS(victim) == POSITION_DEAD)
         return;
   }
}

void break_object(class unit_data *obj)
{
   char tmp[256];

   if (OBJ_EQP_POS(obj))
      unequip_object(obj);

   sprintf(tmp, "%s (broken)", STR(UNIT_TITLE_STRING(obj)));

   UNIT_TITLE(obj) = (tmp);

   OBJ_VALUE(obj, 0) = 0;
   OBJ_VALUE(obj, 1) = 0;
   OBJ_VALUE(obj, 2) = 0;
   OBJ_VALUE(obj, 3) = 0;
   OBJ_TYPE(obj) = ITEM_TRASH;

   affect_clear_unit(obj);

   OBJ_PRICE(obj) = 0;
   OBJ_PRICE_DAY(obj) = 0;
   OBJ_RESISTANCE(obj) = 0;
}

/* 'ch' is optional, and will receive a message if 'obj' breaks */
void damage_object(class unit_data *ch, class unit_data *obj, int dam)
{
   if (obj == NULL)
      return;

   assert(IS_OBJ(obj));

   if (UNIT_HIT(obj) < 0) /* Endless */
      return;

   if (OBJ_TYPE(obj) == ITEM_WEAPON)
      dam /= 4; // Adjustments requested...
   else if (OBJ_TYPE(obj) == ITEM_ARMOR)
      dam *= 1; // Ditto...

   switch (OBJ_TYPE(obj))
   {
   case ITEM_SHIELD:
      dam *= 2;
      /* Fallthrough */

   case ITEM_WEAPON:
   case ITEM_ARMOR:
      dam -= OBJ_VALUE(obj, 2); /* Just subtract (or add) 25 magic points */
   }

   dam /= 10;

   if (dam > 0)
   {
      UNIT_HIT(obj) -= dam;

      if (UNIT_HIT(obj) < 0)
      {
         if (ch)
         {
            act("Your $3N is broken by the impact!!!",
                A_ALWAYS, ch, 0, obj, TO_CHAR);
            act("$1n's $3N is broken by the impact!!!",
                A_ALWAYS, ch, 0, obj, TO_ROOM);
         }

         break_object(obj);
      }
   }
}

/* -1 if fails, >= 0 amount of damage */

int one_hit(class unit_data *att, class unit_data *def,
            int bonus, int att_weapon_type, int primary, int attack)
{
   int dam, hm, hit_loc;
   int roll;

   int def_armour_type;
   int def_shield_bonus;

   class unit_data *att_weapon;
   class unit_data *def_armour;
   class unit_data *def_shield;

   assert(IS_CHAR(att) && IS_CHAR(def));

   if (CHAR_POS(att) < POSITION_SLEEPING)
      return -1;

   if (CHAR_POS(def) <= POSITION_DEAD)
      return -1;

   if (att == def)
      return -1;

   if (!same_surroundings(att, def))
      return -1;

   if (attack)
   {
      if (!CHAR_FIGHTING(att))
         set_fighting(att, def, TRUE);

      if (CHAR_POS(att) != POSITION_FIGHTING)
         CHAR_POS(att) = POSITION_FIGHTING;
      add_fighting(att, def, TRUE);
   }
   else
   {
      add_fighting(att, def, FALSE);
   }

   hit_loc = hit_location(att, def);

   hm = melee_bonus(att, def, hit_loc,
                    &att_weapon_type, &att_weapon,
                    &def_armour_type, &def_armour, primary);

   hm += bonus;

   roll = open100();

   if (att_weapon && weapon_fumble(att_weapon, roll))
   {
      cact("You fumble with your $2N!", A_ALWAYS, att, att_weapon, 0,
           TO_CHAR, "miss_me");
      cact("$1n fumbles with $1s $2N!", A_ALWAYS, att, att_weapon, 0,
           TO_ROOM, "miss_other");
      hm = MIN(-10, roll - open100());
      damage_object(att, att_weapon, hm);
      return 0;
   }
   else
   {
      roll_description(att, "hit", roll);
      hm += roll;
   }

   if (CHAR_COMBAT(att))
      CHAR_COMBAT(att)->changeSpeed(wpn_info[att_weapon_type].speed);

   dam = 0;

   if (!CHAR_AWAKE(def))
      hm = MAX(hm, 100);

   if (hm < 0) /* a miss */
   {
      damage(att, def, att_weapon, 0, MSG_TYPE_WEAPON,
             att_weapon_type, COM_MSG_MISS);
   }
   else /* A hit! */
   {
      if (att_weapon)
      {
         if (OBJ_VALUE(att_weapon, 3) == CHAR_RACE(def))
         {
            hm += hm / 5; /* Add 20% bonus extra again to damage */
            act("Your $2N glows!", A_SOMEONE, att, att_weapon, 0, TO_CHAR);
            act("$1n's $2N glows!", A_HIDEINV, att, att_weapon, 0,
                TO_ROOM);
         }
         dam = weapon_damage(hm, att_weapon_type, def_armour_type);
      }
      else
      {
         dam = natural_damage(hm, att_weapon_type,
                              def_armour_type, UNIT_BASE_WEIGHT(att));
      }

      if (dam > 0)
         damage_object(att, att_weapon, dam);

      def_shield_bonus = shield_bonus(att, def, &def_shield);

      if (CHAR_AWAKE(def) &&
          def_shield && wpn_info[att_weapon_type].shield != SHIELD_M_USELESS)
      {
         if ((wpn_info[att_weapon_type].shield == SHIELD_M_BLOCK) &&
             (number(1, 100) <= def_shield_bonus))
         {
            damage_object(def, def_shield, dam);
            damage(att, def, def_shield, 0, MSG_TYPE_WEAPON,
                   att_weapon_type, WEAR_SHIELD);
            return 0;
         }
         if ((wpn_info[att_weapon_type].shield == SHIELD_M_REDUCE) &&
             (number(1, 100) <= def_shield_bonus))
         {
            dam -= (dam * def_shield_bonus) / 100;
         }
      }

      if (dam > 0)
      {
         damage_object(def, def_armour, dam);

         /* May the victim bleed under my beatiful system */
         if (def_armour || CHAR_IS_HUMANOID(def))
            damage(att, def, att_weapon, dam, MSG_TYPE_WEAPON,
                   att_weapon_type, hit_loc);
         else
            damage(att, def, att_weapon, dam, MSG_TYPE_WEAPON,
                   att_weapon_type, COM_MSG_EBODY);
      }
      else
      {
         damage(att, def, att_weapon, 0, MSG_TYPE_WEAPON,
                att_weapon_type, COM_MSG_NODAM);
      }
   }

   return dam;
}

int simple_one_hit(class unit_data *att, class unit_data *def)
{
   return one_hit(att, def, 0, WPN_ROOT, TRUE, TRUE);
}

/* Returns TRUE if ok */
static int check_combat(class unit_data *ch)
{
   if (ch->is_destructed())
   {
      assert(!CHAR_COMBAT(ch));
      return FALSE;
   }

   if (!CHAR_AWAKE(ch))
      stop_fighting(ch);
   else
   {
      while (CHAR_FIGHTING(ch))
         if (!same_surroundings(ch, CHAR_FIGHTING(ch)))
            stop_fighting(ch, CHAR_FIGHTING(ch));
         else
            break;
   }

   return CHAR_FIGHTING(ch) != NULL;
}

/* control the fights going on */
void melee_violence(class unit_data *ch, int primary)
{
   if (!check_combat(ch))
      return;

   send_combat(ch);

   if (!check_combat(ch))
      return;

   if (CHAR_POS(ch) == POSITION_FIGHTING)
      one_hit(ch, CHAR_FIGHTING(ch), 0, WPN_ROOT, primary, TRUE);
   else
   {
      act("You get back in a fighting position, ready to fight!",
          A_SOMEONE, ch, 0, 0, TO_CHAR);
      act("$1n gets back in a fighting position ready to fight!",
          A_SOMEONE, ch, 0, 0, TO_ROOM);
      CHAR_POS(ch) = POSITION_FIGHTING;
   }
}

/* -------------------------------------------------------------------- */
/*                      H U N T I N G   S Y S T E M                     */
/* -------------------------------------------------------------------- */

struct hunt_data
{
   int no;
   int was_legal;
   class unit_data *victim;
};

/* The hunting routine will prefer to hit anyone hunted to those */
/* already engaged in combat. This is to prevent players flee    */
/* and then enter room as the backmost attacker such that a new  */
/* 'tank' is allowed to be hit at.                               */

/* MS: Changed only to re-attack on tick command                 */
/*     Otherwise two characters hunting get really many attacks  */
/*     as they were activated by the flee command                */
int hunting(struct spec_arg *sarg)
{
   struct hunt_data *h;

   if (sarg->cmd->no != CMD_AUTO_TICK)
      return SFR_SHARE;

   h = (struct hunt_data *)sarg->fptr->data;
   assert(h);

   h->no--;

   if (h->no <= 0)
   {
      destroy_fptr(sarg->owner, sarg->fptr); /* Will free data automatically */
      return SFR_BLOCK;
   }

   if (CHAR_AWAKE(sarg->owner))
   {
      /* with new transparency, this should be a list
           of all visible chars in surroundings */

      if (scan4_ref(sarg->owner, h->victim))
      {
         if (!IS_CHAR(h->victim))
         {
            destroy_fptr(sarg->owner, sarg->fptr); /* Will free automatic. */
            return SFR_BLOCK;
         }

         if (!CHAR_CAN_SEE(sarg->owner, h->victim))
            return SFR_SHARE;

         act("$1n growls viciously at $3n.",
             A_SOMEONE, sarg->owner, 0, h->victim, TO_NOTVICT);
         act("$1n growls at you.",
             A_SOMEONE, sarg->owner, 0, h->victim, TO_VICT);

         /* If the victim was a legal target, then it must be such */
         /* again                                                  */
         if (h->was_legal)
            SET_BIT(CHAR_FLAGS(h->victim), CHAR_LEGAL_TARGET);

         if (!CHAR_COMBAT(sarg->owner) ||
             !CHAR_COMBAT(sarg->owner)->FindOpponent(h->victim))
            set_fighting(sarg->owner, h->victim, TRUE);

         destroy_fptr(sarg->owner, sarg->fptr); /* Will free fptr above */

         return SFR_BLOCK;
      }
   }

   return SFR_SHARE;
}
