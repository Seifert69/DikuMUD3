/*
 $Author: All $
 $RCSfile: teach.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.5 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "common.h"
#include "utility.h"
#include "money.h"
#include "vmelimits.h"
#include "guild.h"
#include "spells.h"
#include "vme.h"

#define PRACTICE_COST_LEVEL (START_LEVEL + 5)

#define TEACH_ABILITIES 0
#define TEACH_SPELLS 1
#define TEACH_SKILLS 2
#define TEACH_WEAPONS 3

/* You can practice a low cost skill 3 times per level. I.e. gain 3 ability in a prime stat.
 * meaning you could have 90 in an ability in 30 levels. Or you can have 90 skill in 15 levels.
 */
#define PRACTICE_ABILITY_GAIN (1) // Was 2 in Diku II
#define PRACTICE_SKILL_GAIN (5)   // Was 5 in Diku II. I tried 2 but that's too low.

/* Use -1 in node to indicate end of teach_type */
struct skill_teach_type
{
   ubit8 max_skill;        // Maximum skill that can be taught
   ubit16 min_glevel;      // What **guild** level do you have to be to learn this?
   int node;               // A node in a tree, e.g. index in skl_text[]
   int min_cost_per_point; // The gold point cost per point
   int max_cost_per_point; // The gold point cost per point
};

struct teacher_msg
{
   char *unknown_skill;
   char *no_teaching;
   char *not_enough_gold;
   char *not_enough_points;
   char *teacher_not_good_enough;
   char *not_pure;
   char *remove_inventory;
};

struct teach_packet
{
   ubit8 type; /* Ability, spell, skill, weapon */
   // Obsolete, only guild level in teachers 0 for guild-level, 1 for ordinary level
   struct teacher_msg msgs;
   struct skill_teach_type *teaches; /* Array of skills */
   struct tree_type *tree;
   const char **text;
};

struct pc_train_values
{
   sbit16 *values;
   ubit8 *lvl;
   sbit32 *practice_points;
};

static int gold_cost(struct skill_teach_type *s, int level)
{

   if (level < 1)
      return s->min_cost_per_point;
   else if (level >= s->max_skill)
      return s->max_cost_per_point;
   else
   {
      int i = s->max_cost_per_point - s->min_cost_per_point;

      i /= s->max_skill;

      i = i * level + s->min_cost_per_point;

      return i;
   }
}

int practice_skill_gain(int skill)
{
   // This function was designed on the premise that 5 is the number
   // and the number is 5.
   assert(PRACTICE_SKILL_GAIN == 5);

   if (skill < 40)
      return PRACTICE_SKILL_GAIN;
   else if (skill < 60)
      return PRACTICE_SKILL_GAIN - 1;
   else if (skill < 75)
      return PRACTICE_SKILL_GAIN - 2;
   else if (skill < 100)
      return PRACTICE_SKILL_GAIN - 3;
   else
      return 1;
}

// 0 modifiers returns AVERAGE_SKILL_COST (10)
// If the sum of mods > 0 it goes down by one per mod to minimum of 3
// Otherwise, grows by 2x for each sum mod negative
// because half cost = 5, double cost = 20.
// For each training level after 1, cost increases by 50%.
// 
// Returning zero means you can't learn more at this level
int actual_cost(int cost, sbit8 racemodifier, int level, int virtual_level)
{
   int mod;
   int pct;
   int avg_skill_cost;
   int calccost;

   avg_skill_cost = AVERAGE_SKILL_COST;

   if (virtual_level > 100)
   {
      int i;
      i = MIN(100, virtual_level - 100);
      avg_skill_cost += (i * AVERAGE_SKILL_COST * (ABILITY_POINT_FACTOR - 1)) / 100;
      // With level > 100, the average skill cost will increase as per these average examples
      // 100 : 10 + ( 0   * 10 * (4-1))/100 = 10+0
      // 150 : 10 + ( 50  * 10 * (4-1))/100 = 10+15
      // 200:  10 + ( 100 * 10 * (4-1))/100 = 10+30
      //1000:  10 + ( 100 * 10 * (4-1))/100 = 10+30
   }

   pct = 100;
   if (level >= 1)
      pct = pow(1.5, level) * 100.0;

   mod = cost + racemodifier;

   if (mod == 0)
      calccost = (avg_skill_cost * pct + 99) / 100;
   else if (mod > 0)
   {
      if (mod > 5)
         mod = 5; // 10-5 = 5 is the cheapest cost

      // SO best possible training progression for any char is: 4, 6, 9, 14
      calccost = ((avg_skill_cost - mod) * pct + 99) / 100;
   }
   else // mod < 0
   {
      if (mod < -7)
         mod = -7;
      calccost = ((avg_skill_cost - 3 * mod) * pct + 99) / 100;
   }

   if (calccost > 5*avg_skill_cost) // Too expensive
      return 0;
   else
      return calccost;
}

void clear_training_level(class unit_data *ch)
{
   int i;

   assert(IS_PC(ch));

   for (i = 0; i < SPL_TREE_MAX; i++)
      PC_SPL_LVL(ch, i) = 0;

   for (i = 0; i < WPN_TREE_MAX; i++)
      PC_WPN_LVL(ch, i) = 0;

   for (i = 0; i < SKI_TREE_MAX; i++)
      PC_SKI_LVL(ch, i) = 0;

   for (i = 0; i < ABIL_TREE_MAX; i++)
      PC_ABI_LVL(ch, i) = 0;
}

int teaches_index(struct skill_teach_type *teaches_skills, int node)
{
   int i;

   for (i = 0; teaches_skills[i].node != -1; i++)
      if (teaches_skills[i].node == node)
         return i;

   return -1;
}

const char *trainrestricted(class unit_data *pupil, struct profession_cost *cost_entry, int minguildlevel)
{
   static char buf[MAX_STRING_LENGTH];
   char *c;

   assert(IS_PC(pupil));

   strcpy(buf, "[REQ: ");
   c = buf;
   TAIL(c);

   if (PC_VIRTUAL_LEVEL(pupil) < cost_entry->min_level)
   {
      sprintf(c, "Level:%d ", cost_entry->min_level);
      TAIL(c);
   }

   if (char_guild_level(pupil) < minguildlevel)
   {
      sprintf(c, "Guild level:%d ", minguildlevel);
      TAIL(c);
   }

   for (int i = 0; i < ABIL_TREE_MAX; i++)
   {
      if (CHAR_ABILITY(pupil, i) < cost_entry->min_abil[i])
      {
         sprintf(c, "%s:%d ", g_AbiColl.text[i], cost_entry->min_abil[i]);
         TAIL(c);
      }
   }

   strcat(c, "]");

   if (strlen(buf) == 7)
      return "";
   else
      return buf;
}

void info_show_one(class unit_data *teacher,
                   class unit_data *pupil,
                   ubit8 current_points,
                   ubit8 max_level,
                   int next_point,
                   int gold,
                   int lvl,
                   const char *text,
                   int indent, ubit8 isleaf, int min_level,
                   struct profession_cost *cost_entry,
                   vector<pair<int, string>> &vect)
{
   char buf[256];

   if (isleaf)
   {
      const char *req;
      req = trainrestricted(pupil, cost_entry, min_level);

      if (*req)
      {
         sprintf(buf, "<div class='ca'>%s     %-20s %s</div><br/>",
                 spc(4 * indent), text, req);
         vect.push_back(std::make_pair(1000, buf));
         return;
      }

      if (current_points >= max_level)
      {
         sprintf(buf, "<div class='ca'>%s%3d%% %-20s [Teacher at max]</div><br/>",
                 spc(4 * indent), current_points, text);
         vect.push_back(std::make_pair(1002, buf));
         return;
      }

      if (next_point == 0)
      {
         sprintf(buf, "<div class='ca'>%s%3d%% %-20s [Practice next level]</div><br/>",
                 spc(4 * indent), current_points, text);
         vect.push_back(std::make_pair(1001, buf));
      }
      else
      {
         currency_t currency = local_currency(teacher);

         if (IS_SET(PC_FLAGS(pupil), PC_EXPERT))
            sprintf(buf,
                    "%s%s%3d%% %-20s [%3d%% of %3d%%, points %2d, %s] %s%s<br/>",
                    next_point >= 20 ? "<div class='ca'>" : "", spc(4 * indent), current_points, text, current_points, max_level,
                    next_point,
                    money_string(money_round(TRUE, gold, currency, 1), currency, FALSE),
                    string(lvl, '*').c_str(), next_point >= 20 ? "</div>" : "");
         else
            sprintf(buf, "%s%s%3d%% %-20s [practice points %3d] %s%s<br/>",
                    next_point >= 20 ? "<div class='ca'>" : "", spc(4 * indent), current_points, text,
                    next_point, string(lvl, '*').c_str(), next_point >= 20 ? "</div>" : "");

         vect.push_back(std::make_pair(next_point, buf));
      }
   }
   else // category, not isleaf
   {
      sprintf(buf, "%s     <a cmd='info #'>%s</a><br/>", spc(4 * indent), text);
      vect.push_back(std::make_pair(-1, buf));
   }
}

bool pairISCompareAsc(const std::pair<int, string> &firstElem, const std::pair<int, string> &secondElem)
{
   return firstElem.first < secondElem.first;
}

void info_show_roots(class unit_data *teacher, class unit_data *pupil,
                     class skill_collection *pColl,
                     struct pc_train_values *pTrainValues,
                     struct skill_teach_type *teaches_skills)
{
   int i, cost;
   vector<pair<int, string>> vect;

   for (i = 0; teaches_skills[i].node != -1; i++)
      if ((!TREE_ISROOT(pColl->tree, teaches_skills[i].node) &&
           !TREE_ISLEAF(pColl->tree, teaches_skills[i].node)) ||
          ((TREE_ISROOT(pColl->tree, teaches_skills[i].node) &&
            TREE_ISLEAF(pColl->tree, teaches_skills[i].node))))
      {
         cost = actual_cost(pColl->prof_table[teaches_skills[i].node].profession_cost[PC_PROFESSION(pupil)],
                            pColl->racial[CHAR_RACE(pupil)][teaches_skills[i].node],
                            pTrainValues->lvl[teaches_skills[i].node], PC_VIRTUAL_LEVEL(pupil));

         info_show_one(teacher, pupil,
                       pTrainValues->values[teaches_skills[i].node],
                       teaches_skills[i].max_skill,
                       cost,
                       gold_cost(&teaches_skills[i], pTrainValues->values[teaches_skills[i].node]),
                       pTrainValues->lvl[teaches_skills[i].node],
                       pColl->text[teaches_skills[i].node], 0,
                       TREE_ISLEAF(pColl->tree, teaches_skills[i].node),
                       teaches_skills[i].min_glevel,
                       &pColl->prof_table[teaches_skills[i].node], vect);
      }

   std::sort(vect.begin(), vect.end(), pairISCompareAsc);

   string str;
   str = "<pre>";
   for (auto it = vect.begin(); it != vect.end(); ++it)
      str.append(it->second.c_str());
   str.append("</pre>");

   if (str.length() <= 11)
      str = "Nothing to show (try <a cmd='#'>info roots</a>)<br/>";

   send_to_char(str.c_str(), pupil);
}

void info_show_leaves(class unit_data *teacher, class unit_data *pupil,
                      class skill_collection *pColl,
                      struct skill_teach_type *teaches_skills,
                      struct pc_train_values *pTrainValues)
{
   int i, cost;
   vector<pair<int, string>> vect;

   for (i = 0; teaches_skills[i].node != -1; i++)
      if (TREE_ISLEAF(pColl->tree, teaches_skills[i].node))
      {
         cost = actual_cost(pColl->prof_table[teaches_skills[i].node].profession_cost[PC_PROFESSION(pupil)],
                            pColl->racial[CHAR_RACE(pupil)][teaches_skills[i].node],
                            pTrainValues->lvl[teaches_skills[i].node], PC_VIRTUAL_LEVEL(pupil));

         info_show_one(teacher, pupil,
                       pTrainValues->values[teaches_skills[i].node],
                       teaches_skills[i].max_skill,
                       cost,
                       gold_cost(&teaches_skills[i], pTrainValues->values[teaches_skills[i].node]),
                       pTrainValues->lvl[teaches_skills[i].node],
                       pColl->text[teaches_skills[i].node], 0,
                       TREE_ISLEAF(pColl->tree, teaches_skills[i].node),
                       teaches_skills[i].min_glevel,
                       &pColl->prof_table[teaches_skills[i].node], vect);
      }

   std::sort(vect.begin(), vect.end(), pairISCompareAsc);
   string str;
   str = "<pre>";
   for (auto it = vect.begin(); it != vect.end(); ++it)
   {
      if (IS_SET(PC_FLAGS(pupil), PC_EXPERT) || it->first <= 25) // Limit display
         str.append(it->second.c_str());
   }
   str.append("</pre>");

   if (str.length() <= 11)
      str = "Nothing to show (try <a cmd='#'>info roots</a>)<br/>";

   send_to_char(str.c_str(), pupil);
}

void info_one_skill(class unit_data *teacher, class unit_data *pupil,
                    class skill_collection *pColl,
                    struct pc_train_values *pTrainValues,
                    struct skill_teach_type *teaches_skills,
                    int teach_index, struct teacher_msg *msgs)
{
   int indent, i, j, cost;
   indent = 0;
   vector<pair<int, string>> vect;

   /* Find category if index is a leaf with a category parent */

   if (TREE_ISLEAF(pColl->tree, teaches_skills[teach_index].node) &&
       !TREE_ISROOT(pColl->tree, teaches_skills[teach_index].node))
   {
      i = TREE_PARENT(pColl->tree, teaches_skills[teach_index].node);

      teach_index = teaches_index(teaches_skills, i);
   }

   if (teach_index == -1)
   {
      send_to_char("Something is fishy about this teacher! "
                   "Please contact a god!<br/>",
                   pupil);
      return;
   }

   /* Show teach_index in case it is a category */
   if (!TREE_ISLEAF(pColl->tree, teaches_skills[teach_index].node))
   {
      i = teaches_skills[teach_index].node;
      cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)], pColl->racial[CHAR_RACE(pupil)][i], pTrainValues->lvl[i], PC_VIRTUAL_LEVEL(pupil));

      info_show_one(teacher, pupil, pTrainValues->values[i],
                    teaches_skills[teach_index].max_skill,
                    cost,
                    gold_cost(&teaches_skills[teach_index], pTrainValues->values[i]),
                    pTrainValues->lvl[i],
                    pColl->text[i],
                    indent++,
                    TREE_ISLEAF(pColl->tree, teaches_skills[teach_index].node),
                    teaches_skills[teach_index].min_glevel,
                    &pColl->prof_table[i], vect);

      /* Show children of teach_index category */
      for (j = 0; teaches_skills[j].node != -1; j++)
         if (TREE_ISLEAF(pColl->tree, teaches_skills[j].node) &&
             (TREE_PARENT(pColl->tree, teaches_skills[j].node) ==
              teaches_skills[teach_index].node))
         {
            /* It is a child */
            i = teaches_skills[j].node;
            cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)], pColl->racial[CHAR_RACE(pupil)][i], pTrainValues->lvl[i], PC_VIRTUAL_LEVEL(pupil));
            info_show_one(teacher, pupil, pTrainValues->values[i],
                          teaches_skills[j].max_skill,
                          cost,
                          gold_cost(&teaches_skills[j], pTrainValues->values[i]),
                          pTrainValues->lvl[i],
                          pColl->text[i],
                          indent,
                          TREE_ISLEAF(pColl->tree, teaches_skills[j].node),
                          teaches_skills[j].min_glevel,
                          &pColl->prof_table[i], vect);
         }
   }
   else // Leaf
   {
      /* Show all leaves, no category above */
      for (j = 0; teaches_skills[j].node != -1; j++)
         if (TREE_ISLEAF(pColl->tree, teaches_skills[j].node))
         {
            /* It is a child */
            i = teaches_skills[j].node;
            cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)], pColl->racial[CHAR_RACE(pupil)][i], pTrainValues->lvl[i], PC_VIRTUAL_LEVEL(pupil));

            info_show_one(teacher, pupil, pTrainValues->values[i],
                          teaches_skills[j].max_skill,
                          cost,
                          gold_cost(&teaches_skills[j], pTrainValues->values[i]),
                          pTrainValues->lvl[i],
                          pColl->text[i],
                          indent,
                          TREE_ISLEAF(pColl->tree, teaches_skills[j].node),
                          teaches_skills[j].min_glevel,
                          &pColl->prof_table[i], vect);
         }
   }

   std::sort(vect.begin(), vect.end(), pairISCompareAsc);
   string str;
   str = "<pre>";
   for (auto it = vect.begin(); it != vect.end(); ++it)
      str.append(it->second.c_str());
   str.append("</pre>");
   send_to_char(str.c_str(), pupil);
}

int pupil_magic(class unit_data *pupil)
{
   class unit_affected_type *af;

   for (af = UNIT_AFFECTED(pupil); af; af = af->next)
      switch (af->id)
      {
      case ID_BLESS:
      case ID_UNHOLY_BLESSING:
      case ID_BERSERK:
      case ID_RAGE:
      case ID_CURSE:
      case ID_ENERGY_DRAIN:
      case ID_SPL_RAISE_MAG:
      case ID_SPL_RAISE_DIV:
      case ID_SPL_RAISE_STR:
      case ID_SPL_RAISE_DEX:
      case ID_SPL_RAISE_CON:
      case ID_SPL_RAISE_CHA:
      case ID_SPL_RAISE_BRA:
      case ID_SPL_RAISE_HPP:
      case ID_SPL_RAISE_DIVINE:
      case ID_SPL_RAISE_SUMMONING:
      case ID_SPL_RAISE_MIND:
      case ID_SPL_RAISE_HEAT:
      case ID_SPL_RAISE_COLD:
      case ID_SPL_RAISE_CELL:
      case ID_SPL_RAISE_INTERNAL:
      case ID_SPL_RAISE_EXTERNAL:
      case ID_RAISE_MAG:
      case ID_RAISE_DIV:
      case ID_RAISE_STR:
      case ID_RAISE_DEX:
      case ID_RAISE_CON:
      case ID_RAISE_CHA:
      case ID_RAISE_BRA:
      case ID_RAISE_HPP:
      case ID_RAISE_DIVINE:
      case ID_RAISE_MIND:
      case ID_RAISE_HEAT:
      case ID_RAISE_COLD:
      case ID_RAISE_CELL:
      case ID_RAISE_INTERNAL:
      case ID_RAISE_EXTERNAL:

      case ID_SPELL_TRANSFER:
      case ID_SKILL_TRANSFER:
      case ID_WEAPON_TRANSFER:

         return TRUE;
      }

   return FALSE;
}

void practice_base(int type,
                   struct teach_packet *pckt,
                   struct pc_train_values *pTrainValues,
                   int teach_index, int cost)
{
   if (!TREE_ISLEAF(pckt->tree, pckt->teaches[teach_index].node))
   {
      slog(LOG_EXTENSIVE, 0, "ERROR: Practice base: not a leaf!");
      return;
   }

   *pTrainValues->practice_points -= cost;

   pTrainValues->lvl[pckt->teaches[teach_index].node]++;

   if (type == TEACH_ABILITIES)
      pTrainValues->values[pckt->teaches[teach_index].node] += PRACTICE_ABILITY_GAIN;
   else
      pTrainValues->values[pckt->teaches[teach_index].node] += practice_skill_gain(pTrainValues->values[pckt->teaches[teach_index].node]);

   // Set parent nodes to 1/2 for each level up

   int idx = pckt->teaches[teach_index].node;

   while (pTrainValues->values[idx] > 2 * pTrainValues->values[TREE_PARENT(pckt->tree, idx)])
   {
      int pidx = TREE_PARENT(pckt->tree, idx);

      pTrainValues->lvl[pidx]++;
      pTrainValues->values[pidx] = pTrainValues->values[idx] / 2;

      if (TREE_ISROOT(pckt->tree, pidx))
         break;

      idx = TREE_PARENT(pckt->tree, idx);
   }
}

int practice(class unit_data *teacher, class unit_data *pupil,
             class skill_collection *pColl,
             struct pc_train_values *pTrainValues,
             struct teach_packet *pckt,
             int teach_index)
{
   int cost;
   char buf[512];
   currency_t currency = local_currency(teacher);
   amount_t amt;

   assert(teach_index != -1);

   if (!TREE_ISLEAF(pColl->tree, pckt->teaches[teach_index].node))
   {
      sprintf(buf,
              "It is not possible to practice the category '%s'.<br/>"
              "The category is there to prevent you from being flooded with information.<br/>"
              "Try the command: 'info %s' on the category itself,<br/>"
              "to see which skills it contains.<br/>",
              pColl->text[pckt->teaches[teach_index].node],
              pColl->text[pckt->teaches[teach_index].node]);
      send_to_char(buf, pupil);
      return TRUE;
   }

   if (pckt->teaches[teach_index].max_skill <=
       pTrainValues->values[pckt->teaches[teach_index].node])
   {
      act(pckt->msgs.teacher_not_good_enough,
          A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
      return TRUE;
   }

   const char *req;
   req = trainrestricted(pupil, &pColl->prof_table[pckt->teaches[teach_index].node], pckt->teaches[teach_index].min_glevel);
   if (*req)
   {
      sprintf(buf, "To practice %s you need to meet the following requirements %s.<br/>",
              pColl->text[pckt->teaches[teach_index].node], req);
      send_to_char(buf, pupil);
      return TRUE;
   }

   cost = actual_cost(pColl->prof_table[pckt->teaches[teach_index].node].profession_cost[PC_PROFESSION(pupil)],
                      pColl->racial[CHAR_RACE(pupil)][pckt->teaches[teach_index].node], pTrainValues->lvl[pckt->teaches[teach_index].node], PC_VIRTUAL_LEVEL(pupil));

   if (cost == 0)
   {
      act("$1n tells you, 'You've learned all you can about $2t at this level.'",
          A_ALWAYS, teacher,
          pColl->text[pckt->teaches[teach_index].node], pupil, TO_VICT);
      return TRUE;
   }

   assert(cost > 0);

   if (*pTrainValues->practice_points < cost)
   {
      sprintf(buf, pckt->msgs.not_enough_points, cost);
      act(buf, A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
      if (CHAR_LEVEL(pupil) == START_LEVEL)
         send_to_char("Beginners note: Go on adventure and gain a level.<br/>"
                      "Then come back and practice afterwards.<br/>",
                      pupil);
      return TRUE;
   }

   if (pupil_magic(pupil))
   {
      act(pckt->msgs.not_pure, A_SOMEONE, teacher, cActParameter(), pupil,
          TO_VICT);
      return TRUE;
   }

   /* Ok, now we can teach */

   amt = money_round(TRUE, gold_cost(&pckt->teaches[teach_index], pTrainValues->values[pckt->teaches[teach_index].node]),
                     currency, 1);

   if (CHAR_LEVEL(pupil) > PRACTICE_COST_LEVEL &&
       !char_can_afford(pupil, amt, currency))
   {
      sprintf(buf, pckt->msgs.not_enough_gold,
              money_string(amt, local_currency(pupil), TRUE));
      act(buf, A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
      return TRUE;
   }

   if (CHAR_LEVEL(pupil) > PRACTICE_COST_LEVEL)
      money_from_unit(pupil, amt, currency);

   practice_base(pckt->type, pckt, pTrainValues, teach_index, cost);

   act("You finish training $2t with $1n.", A_ALWAYS,
       teacher,
       pColl->text[pckt->teaches[teach_index].node], pupil, TO_VICT);

   return FALSE;
}

void shuffle(int *array, size_t n)
{
   if (n > 1)
   {
      size_t i;
      for (i = 0; i < n - 1; i++)
      {
         size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
         int t = array[j];
         array[j] = array[i];
         array[i] = t;
      }
   }
}

// Any skill that costs more than costlimit will not be trained
int auto_train(int type,
               class unit_data *pupil,
               class skill_collection *pColl,
               struct pc_train_values *pTrainValues,
               struct teach_packet *pckt,
               int costlimit)
{
   int teach_index, cost;
   const char *req;

   int nTrained = 0;

   int nLen;

   // How many values do we need to train?
   for (nLen = 0; pckt->teaches[nLen].node != -1; nLen++)
      ;

   if (nLen < 1)
      return 0;

   int aTeachIndex[nLen];

   for (int i = 0; i < nLen; i++)
      aTeachIndex[i] = i;

   // When a char has nearly identical costs, let's shuffe around.
   shuffle(aTeachIndex, nLen);

   for (int i = 0; i < nLen; i++)
   {
      teach_index = aTeachIndex[i];
      int nodeidx = pckt->teaches[teach_index].node;

      if (TREE_ISLEAF(pColl->tree, nodeidx))
      {
         if (!pColl->tree[nodeidx].bAutoTrain)
            continue;

         if (pTrainValues->values[nodeidx] >= pckt->teaches[teach_index].max_skill)
            continue;

         req = trainrestricted(pupil, &pColl->prof_table[nodeidx], pckt->teaches[teach_index].min_glevel);
         if (*req)
            continue;

         cost = actual_cost(pColl->prof_table[nodeidx].profession_cost[PC_PROFESSION(pupil)],
                            pColl->racial[CHAR_RACE(pupil)][nodeidx],
                            pTrainValues->lvl[nodeidx], PC_VIRTUAL_LEVEL(pupil));

         if (strcmp(pColl->text[nodeidx], "armor plate") == 0)
         {
            cost++;
         }

         if (cost < 1)
            continue;


         if (strcmp(pColl->text[nodeidx], "armor plate") == 0)
         {
            cost--;
         }

         // Once a skills, weapon or spell reaches 50, reduce likelihood slightly we'll train it again
         if (type != TEACH_ABILITIES)
            if (pTrainValues->values[nodeidx] >= 50)
            {
               int rnd = number(25, pTrainValues->values[nodeidx]);

               if (rnd > 30)
                  continue;
            }

         if ((type == TEACH_ABILITIES) && (cost > 10) && (pTrainValues->lvl[nodeidx] == 0) && (CHAR_LEVEL(pupil) > 2))
         {
            // Special logic to ensure at least a minimum of ability points

            if ((nodeidx == ABIL_CON) && (CHAR_ABILITY(pupil, ABIL_CON) < CHAR_ABILITY(pupil, ABIL_HP) / 2))
               ;
            else if ((nodeidx == ABIL_BRA) && (CHAR_ABILITY(pupil, ABIL_BRA) < MAX(CHAR_ABILITY(pupil, ABIL_MAG), CHAR_ABILITY(pupil, ABIL_DIV)) / 2))
               ;
            else if ((nodeidx == ABIL_CHA) && (CHAR_ABILITY(pupil, ABIL_CHA) < MAX(CHAR_ABILITY(pupil, ABIL_MAG), CHAR_ABILITY(pupil, ABIL_DIV)) / 2))
               ;
            else if ((nodeidx == ABIL_HP) && ((CHAR_ABILITY(pupil, ABIL_HP) * cost) / 10 < CHAR_LEVEL(pupil)))
               ;
            else if (cost > costlimit)
               continue;
         }
         else
         {
            if (cost > costlimit)
               continue;
         }

         if (*pTrainValues->practice_points < cost)
            continue;

         nTrained++;

         practice_base(type, pckt, pTrainValues, teach_index, cost);

         act("You train $2t.", A_ALWAYS, pupil, pColl->text[nodeidx], cActParameter(), TO_CHAR);
      }
   }

   return nTrained;
}

struct teach_packet *get_teacher(const char *pName)
{
   class unit_data *u;
   class unit_fptr *f;
   struct teach_packet *pckt;

   char name[MAX_INPUT_LENGTH];
   char zone[MAX_INPUT_LENGTH];

   if (str_is_empty(pName))
      return NULL;

   name[0] = 0;
   zone[0] = 0;

   split_fi_ref(pName, zone, name);

   u = find_symbolic(zone, name);

   if (!u)
   {
      slog(LOG_EXTENSIVE, 0, "ERROR: get_teacher() Teacher %s not found.", pName);
      return NULL;
   }

   for (f = UNIT_FUNC(u); f; f = f->next)
      if (f->index == SFUN_TEACHING)
         break;

   if (!f)
   {
      slog(LOG_EXTENSIVE, 0, "ERROR: get_teacher() FUNC for %s not found.", pName);
      return NULL;
   }

   pckt = (struct teach_packet *)f->data;
   assert(pckt);

   return pckt;
}

class skill_collection *get_pc_train_values(class unit_data *pupil, int type, struct pc_train_values *pValues)
{
   switch (type)
   {
   case TEACH_ABILITIES:
      pValues->values = &CHAR_ABILITY(pupil, 0); // Current ability
      pValues->lvl = &PC_ABI_LVL(pupil, 0);      // How many times you've trained this level on this ability
      pValues->practice_points = &PC_ABILITY_POINTS(pupil);
      return &g_AbiColl;

   case TEACH_SKILLS:
      pValues->values = &PC_SKI_SKILL(pupil, 0);
      pValues->lvl = &PC_SKI_LVL(pupil, 0);
      pValues->practice_points = &PC_SKILL_POINTS(pupil);
      return &g_SkiColl;

   case TEACH_SPELLS:
      pValues->values = &PC_SPL_SKILL(pupil, 0);
      pValues->lvl = &PC_SPL_LVL(pupil, 0);
      pValues->practice_points = &PC_SKILL_POINTS(pupil);
      return &g_SplColl;
      break;

   case TEACH_WEAPONS:
      pValues->values = &PC_WPN_SKILL(pupil, 0);
      pValues->lvl = &PC_WPN_LVL(pupil, 0);
      pValues->practice_points = &PC_SKILL_POINTS(pupil);
      return &g_WpnColl;
      break;

   default:
      assert(FALSE);
   }

   return NULL; // Can't happen
}

int teach_basis(struct spec_arg *sarg, struct teach_packet *pckt)
{
   int index;
   char buf[MAX_INPUT_LENGTH + 10];
   const char *arg;
   class skill_collection *pColl = NULL;

   if (!is_command(sarg->cmd, "info") && !is_command(sarg->cmd, "practice"))
      return SFR_SHARE;

   if (!IS_PC(sarg->activator))
   {
      send_to_char("That wouldn't be wise.<br/>", sarg->activator);
      return SFR_BLOCK;
   }

   if (!is_in(PC_PROFESSION(sarg->activator), 0, PROFESSION_MAX))
   {
      send_to_char("You don't have a profession set yet. Please get help.<br/>", sarg->activator);
      return SFR_BLOCK;
   }

   if (!CHAR_IS_READY(sarg->owner))
   {
      act("$1n is not capable of teaching now.",
          A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   static const char *remote_train[] = {"ability", "skill", "spell", "weapon", NULL};
   arg = str_next_word(sarg->arg, buf);

   index = search_block(buf, remote_train, TRUE);
   if (index < 0)
      arg = sarg->arg; // Restore the arg because no remote keyword was there.

   struct pc_train_values TrainValues;
   pColl = get_pc_train_values(sarg->activator, pckt->type, &TrainValues);

   if (str_is_empty(arg))
   {
      if (is_command(sarg->cmd, "info"))
      {
         info_show_leaves(sarg->owner, sarg->activator, pColl, pckt->teaches, &TrainValues);
         sprintf(buf, "<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
         send_to_char(buf, sarg->activator);
      }
      else
      {
         act("$1n asks, 'What do you wish to practice, $3n?'",
             A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
         if (CHAR_LEVEL(sarg->activator) == START_LEVEL)
            send_to_char("Beginners note: Try the 'info' command NOW.<br/>",
                         sarg->activator);
      }

      return SFR_BLOCK;
   }

   arg = skip_spaces(arg);

   if (str_ccmp(arg, "roots") == 0)
   {
      info_show_roots(sarg->owner, sarg->activator, pColl, &TrainValues, pckt->teaches);
      sprintf(buf, "<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
      send_to_char(buf, sarg->activator);
      return SFR_BLOCK;
   }

   if (str_ccmp(arg, "auto") == 0)
   {
      if (TrainValues.values == NULL)
      {
         sprintf(buf, "<br/>Please specify ability, skill, spell, weapon before the auto keyword.<br/>");
         send_to_char(buf, sarg->activator);
      }
      else
      {
         int nCount;
         struct teach_packet *p[4];
         class extra_descr_data *exd;

         exd = PC_QUEST(sarg->activator).find_raw("$autotrain");

         if (!exd)
         {
            sprintf(buf, "Your guild is not setup properly. If you're already in a guild please contact an administrator.<br/>");
            send_to_char(buf, sarg->activator);
            return SFR_BLOCK;
         }

         p[0] = get_teacher(exd->names.Name(1));
         p[1] = get_teacher(exd->names.Name(2));
         p[2] = get_teacher(exd->names.Name(3));
         p[3] = get_teacher(exd->names.Name(4));

         for (int i = 8; i < 16; i += 2)
         {
            nCount = 0;

            for (int j = 0; j < 4; j++)
            {
               if (!p[j])
                  continue;

               pColl = get_pc_train_values(sarg->activator, p[j]->type, &TrainValues);
               nCount += auto_train(p[j]->type, sarg->activator, pColl, &TrainValues, p[j], i);
            }
         }

         sprintf(buf, "<br/>Done auto practicing. You have %d ability and %d skill points left.<br/>",
                 PC_ABILITY_POINTS(sarg->activator), PC_SKILL_POINTS(sarg->activator));
         send_to_char(buf, sarg->activator);
      }

      return SFR_BLOCK;
   }

   index = search_block_abbrevs(arg, pckt->text, (const char **)&arg);

   if (index == -1)
   {
      act(pckt->msgs.unknown_skill,
          A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   index = teaches_index(pckt->teaches, index);
   if (index == -1)
   {
      act(pckt->msgs.no_teaching,
          A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
      return SFR_BLOCK;
   }

   if (is_command(sarg->cmd, "info"))
   {
      info_one_skill(sarg->owner, sarg->activator, pColl, &TrainValues,
                     pckt->teaches,
                     index, &pckt->msgs);
      sprintf(buf, "<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
      send_to_char(buf, sarg->activator);
   }
   else /* Practice! */
   {
      practice(sarg->owner, sarg->activator, pColl, &TrainValues, pckt, index);

      /* If hpp changed, then update no of maximum hitpoints */
      UNIT_MAX_HIT(sarg->activator) = hit_limit(sarg->activator);

      info_one_skill(sarg->owner, sarg->activator, pColl,
                     &TrainValues, pckt->teaches, index, &pckt->msgs);
      sprintf(buf, "<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
      send_to_char(buf, sarg->activator);
   }

   return SFR_BLOCK;
}

int teaching(struct spec_arg *sarg)
{
   struct teach_packet *packet;

   packet = (struct teach_packet *)sarg->fptr->data;
   assert(packet);

   if (sarg->cmd->no == CMD_AUTO_EXTRACT)
   {
      if (packet->msgs.unknown_skill)
         FREE(packet->msgs.unknown_skill);

      if (packet->msgs.no_teaching)
         FREE(packet->msgs.no_teaching);

      if (packet->msgs.not_enough_gold)
         FREE(packet->msgs.not_enough_gold);

      if (packet->msgs.not_enough_points)
         FREE(packet->msgs.not_enough_points);

      if (packet->msgs.teacher_not_good_enough)
         FREE(packet->msgs.teacher_not_good_enough);

      if (packet->msgs.not_pure)
         FREE(packet->msgs.not_pure);

      if (packet->msgs.remove_inventory)
         FREE(packet->msgs.remove_inventory);

      if (packet->teaches)
         FREE(packet->teaches);

      FREE(packet);

      sarg->fptr->data = 0;
      return SFR_BLOCK;
   }

   return teach_basis(sarg, packet);
}

const char *get_next_str(const char *data, char *dest)
{
   while (isspace(*data))
      data++;

   while (*data && *data != ';')
      *dest++ = *data++;

   if (*data) /* == ';' */
      data++;

   *dest = 0;

   return data;
}

//
// Given a profession's cost nCost what is the maximum delta to 100.
// 0 = 0, 1 = +5, 2 = +8, 3 = +10
// < 0 is -10 per point
int max_skill_mod(int nCost)
{
   if (nCost == 0)
      return 0;
   else if (nCost == 1)
      return +5;
   else if (nCost == 2)
      return +8;
   else if (nCost == 3)
      return +10;
   else if (nCost > 3)
      return 10 + nCost - 3;

   return nCost * 10; // Will be negative by -10 per point
}

// level, max-train, <skill>, min_cost_per_point (gold), max_cost_per_point (gold), {costs}, 0
// {costs} is one or more integers
// I'm a bit puzzled on the {costs}, it seems like for each time you train cost goes up the the
// next array item. I would have expected it was more like one up every 5 or more training sessions.
int teach_init(struct spec_arg *sarg)
{
   const char *c;
   int i, count, n;
   //int realm = -1;
   char buf[MAX_STRING_LENGTH];
   struct teach_packet *packet;
   struct skill_teach_type a_skill;

   static const char *teach_types[] = {
       "abilities",
       "spells",
       "skills",
       "weapons",
       NULL};

   if (!(c = (char *)sarg->fptr->data))
   {
      szonelog(UNIT_FI_ZONE(sarg->owner),
               "%s@%s: No text data for teacher-init!",
               UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   c = get_next_str(c, buf); /* Get type */

   if ((i = search_block(buf, teach_types, TRUE)) == -1)
   {
      szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Illegal teach type %s in "
                                          "teacher init!",
               UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner), buf);
      destroy_fptr(sarg->owner, sarg->fptr);
      return SFR_BLOCK;
   }

   CREATE(packet, struct teach_packet, 1);
   packet->type = i;

   // --- Next field ---

   c = get_next_str(c, buf);
   // This used to be the guild level type for teachers. Obsoleted. So packet->level_type is obsoleted.
   //
   // Now this is used where the empty string means only teach what's in the text.
   // Otherwise the string must be set to a profession. And in that case, all skills from that profession
   // are copied onto the teacher.  And additional text after that is used to modify the defaults.
   //
   int nProfession = -1;
   if (!str_is_empty(buf))
   {
      extern const char *professions[];

      if ((nProfession = search_block(buf, professions, TRUE)) == -1)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Unknown profession %s in teacher-init.",
                  UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner), buf);
      }
   }

   c = get_next_str(c, buf);
   packet->msgs.unknown_skill = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.no_teaching = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.not_enough_gold = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.not_enough_points = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.teacher_not_good_enough = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.not_pure = str_dup(buf);
   c = get_next_str(c, buf);
   packet->msgs.remove_inventory = str_dup(buf);

   count = 1;
   CREATE(packet->teaches, struct skill_teach_type, 1);

   packet->teaches[count - 1].node = -1;
   packet->teaches[count - 1].min_cost_per_point = -1;
   packet->teaches[count - 1].max_cost_per_point = -1;

   switch (packet->type)
   {
   case TEACH_ABILITIES:
      packet->tree = g_AbiColl.tree;
      packet->text = g_AbiColl.text;

      if (nProfession > -1)
      {
         // Copy in all abilities for profession
         for (n = 0; packet->text[n] != NULL; n++)
         {
            if ((g_AbiColl.prof_table[n].profession_cost[nProfession] >= -3) && !g_AbiColl.tree[n].bAutoTeacherNoAdd)
            {
               a_skill.min_glevel = 0;
               a_skill.max_skill = 100 + max_skill_mod(g_AbiColl.prof_table[n].profession_cost[nProfession]);
               a_skill.node = n;
               a_skill.min_cost_per_point = 10;
               a_skill.max_cost_per_point = 10000 + -1000 * max_skill_mod(g_AbiColl.prof_table[n].profession_cost[nProfession]);
               packet->teaches[count - 1] = a_skill;

               count++;

               RECREATE(packet->teaches, struct skill_teach_type, count);
               packet->teaches[count - 1].node = -1;
               packet->teaches[count - 1].min_cost_per_point = -1;
               packet->teaches[count - 1].max_cost_per_point = -1;
            }
         }
      }
      break;

   case TEACH_SKILLS:
      packet->tree = g_SkiColl.tree;
      packet->text = g_SkiColl.text;
      if (nProfession > -1)
      {
         // Copy in all abilities for profession
         for (n = 0; packet->text[n] != NULL; n++)
         {
            if (g_SkiColl.prof_table[n].profession_cost[nProfession] >= -3 && !g_SkiColl.tree[n].bAutoTeacherNoAdd)
            {
               a_skill.min_glevel = 0;
               a_skill.max_skill = 100 + max_skill_mod(g_SkiColl.prof_table[n].profession_cost[nProfession]);
               a_skill.node = n;
               a_skill.min_cost_per_point = 10;
               a_skill.max_cost_per_point = 10000 + -1000 * max_skill_mod(g_SkiColl.prof_table[n].profession_cost[nProfession]);
               packet->teaches[count - 1] = a_skill;

               count++;

               RECREATE(packet->teaches, struct skill_teach_type, count);
               packet->teaches[count - 1].node = -1;
               packet->teaches[count - 1].min_cost_per_point = -1;
               packet->teaches[count - 1].max_cost_per_point = -1;
            }
         }
      }
      break;

   case TEACH_SPELLS:
      packet->tree = g_SplColl.tree;
      packet->text = g_SplColl.text;
      if (nProfession > -1)
      {
         // Copy in all abilities for profession
         for (n = 0; packet->text[n] != NULL; n++)
         {
            if (g_SplColl.prof_table[n].profession_cost[nProfession] >= -3 && !g_SplColl.tree[n].bAutoTeacherNoAdd)
            {
               a_skill.min_glevel = 0;
               a_skill.max_skill = 100 + max_skill_mod(g_SplColl.prof_table[n].profession_cost[nProfession]);
               a_skill.node = n;
               a_skill.min_cost_per_point = 10;
               a_skill.max_cost_per_point = 10000 + -1000 * max_skill_mod(g_SplColl.prof_table[n].profession_cost[nProfession]);
               packet->teaches[count - 1] = a_skill;

               count++;

               RECREATE(packet->teaches, struct skill_teach_type, count);
               packet->teaches[count - 1].node = -1;
               packet->teaches[count - 1].min_cost_per_point = -1;
               packet->teaches[count - 1].max_cost_per_point = -1;
            }
         }
      }
      break;

   case TEACH_WEAPONS:
      packet->tree = g_WpnColl.tree;
      packet->text = g_WpnColl.text;
      if (nProfession > -1)
      {
         // Copy in all abilities for profession
         for (n = 0; packet->text[n] != NULL; n++)
         {
            if (g_WpnColl.prof_table[n].profession_cost[nProfession] >= -3 && !g_WpnColl.tree[n].bAutoTeacherNoAdd)
            {
               a_skill.min_glevel = 0;
               a_skill.max_skill = 100 + max_skill_mod(g_WpnColl.prof_table[n].profession_cost[nProfession]);
               a_skill.node = n;
               a_skill.min_cost_per_point = 1;
               a_skill.max_cost_per_point = MAX(1, 1000 + -100 * max_skill_mod(g_WpnColl.prof_table[n].profession_cost[nProfession]));
               packet->teaches[count - 1] = a_skill;

               count++;

               RECREATE(packet->teaches, struct skill_teach_type, count);
               packet->teaches[count - 1].node = -1;
               packet->teaches[count - 1].min_cost_per_point = -1;
               packet->teaches[count - 1].max_cost_per_point = -1;
            }
         }
      }
      break;

   default:
      szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Illegal type in teacher-init",
               UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
      break;
   }

   for (;;)
   {
      c = get_next_str(c, buf);
      if (!*buf)
         break;

      a_skill.min_glevel = atoi(buf);
      if (!is_in(a_skill.min_glevel, 0, 250))
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Minimum level expected "
                  "between 1 - 250 (is %s)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }

      c = get_next_str(c, buf);
      if (!*buf)
         break;

      a_skill.max_skill = atoi(buf);

      if (!is_in(a_skill.max_skill, 1, 150))
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Maximum skill expected "
                  "between 1 - 100 (is %s)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }

      c = get_next_str(c, buf);
      if (!*buf)
         break;

      strip_trailing_blanks(buf);

      if ((i = search_block(buf, packet->text, TRUE)) == -1)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Illegal teacher-init "
                                             "skill: %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.node = i;

      if (packet->text == g_SplColl.text)
      {
         /* We have a spell teacher in Udgaard teaching all kinds of magic. 
               
            if (realm == -1)
            {
                realm = spell_info[i].realm;
            }
            else if (realm != spell_info[i].realm)
            {
                szonelog(UNIT_FI_ZONE(sarg->owner),
                         "%s@%s: Differing realms in %s",
                         UNIT_FI_NAME(sarg->owner),
                         UNIT_FI_ZONENAME(sarg->owner), g_SplColl.text[i]);
                c = str_line(c, buf);
                continue;
            }*/
      }

      /* Get cost in gold */

      c = get_next_str(c, buf);
      if (!*buf)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Premature ending of "
                                             "teacher-init! (expected cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         c = str_line(c, buf);
         continue;
      }

      i = atoi(buf);
      if (i == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal min cost amount "
                  "in teacher init (be > 0): %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.min_cost_per_point = i;

      c = get_next_str(c, buf);
      if (!*buf)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Premature ending of "
                                             "teacher-init! (expected cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         break;
      }

      i = atoi(buf);
      if (i == 0)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner),
                  "%s@%s: Illegal max cost amount "
                  "in teacher init (be > 0): %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }
      a_skill.max_cost_per_point = i;

      if (a_skill.max_cost_per_point <= a_skill.min_cost_per_point)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Max amount is <= "
                                             "min amount... must be error!: %s",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner), buf);
         c = str_line(c, buf);
         continue;
      }

      // The training cost is obsoleted
      //
      n = 0;

      do
      {
         c = get_next_str(c, buf);
         if (!*buf)
         {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "%s@%s: Premature ending of "
                     "teacher-init! (expected point)",
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner));
            c = str_line(c, buf);
            continue;
         }

         i = atoi(buf);
         n++;
      } while ((i != 0) && (n < 15));

      if ((i != 0) || !*buf)
         break;

      if (n < 1)
      {
         szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Premature ending of "
                                             "teacher-init! (expected points cost)",
                  UNIT_FI_NAME(sarg->owner),
                  UNIT_FI_ZONENAME(sarg->owner));
         c = str_line(c, buf);
         continue;
      }

      /* Spheres are inserted automagically */
      if (!TREE_ISLEAF(packet->tree, a_skill.node))
         continue;

      for (n = 0; n < count; n++)
         if (packet->teaches[n].node == a_skill.node)
            break;

      if (n < count) // Aha, skill was here already
      {
         packet->teaches[n].min_glevel = a_skill.min_glevel;
         if (a_skill.max_skill < 100)
            packet->teaches[n].max_skill = MIN(packet->teaches[n].max_skill, a_skill.max_skill);
         if (a_skill.max_skill > 100)
            packet->teaches[n].max_skill = MAX(packet->teaches[n].max_skill, a_skill.max_skill);

         // Retain the calculated max & min and cost.
         // I'm quite sure they're "more fun & diverse"
         // Than what's been entered by hand.
      }
      else
      {
         packet->teaches[count - 1] = a_skill;
         count++;
         RECREATE(packet->teaches, struct skill_teach_type, count);

         packet->teaches[count - 1].node = -1;
         packet->teaches[count - 1].min_cost_per_point = -1;
         packet->teaches[count - 1].max_cost_per_point = -1;
      }
   }

   for (i = 0; packet->teaches[i].node != -1; i++)
   {
      if (TREE_ISLEAF(packet->tree, packet->teaches[i].node) &&
          !TREE_ISROOT(packet->tree, packet->teaches[i].node))
      {
         if (teaches_index(packet->teaches,
                           TREE_PARENT(packet->tree,
                                       packet->teaches[i].node)) == -1)
         {
            packet->teaches[count - 1].max_skill = 100;
            packet->teaches[count - 1].min_glevel = 0;
            packet->teaches[count - 1].node =
                TREE_PARENT(packet->tree, packet->teaches[i].node);
            packet->teaches[count - 1].min_cost_per_point = 0;
            packet->teaches[count - 1].max_cost_per_point = 0;

            count++;
            RECREATE(packet->teaches, struct skill_teach_type, count);

            packet->teaches[count - 1].node = -1;
            packet->teaches[count - 1].min_cost_per_point = -1;
            packet->teaches[count - 1].max_cost_per_point = -1;
         }
      }
   }
   packet->teaches[count - 1].node = -1;
   packet->teaches[count - 1].min_cost_per_point = -1;
   packet->teaches[count - 1].max_cost_per_point = -1;

   sarg->fptr->index = SFUN_TEACHING;
   sarg->fptr->flags = SFB_CMD;
   sarg->fptr->heart_beat = PULSE_SEC;

   assert(sarg->fptr->index != SFUN_DIL_INTERNAL);
   FREE(sarg->fptr->data); /* Free the text string! */
   sarg->fptr->data = packet;

   /* Call teaching in case initialization occurs with first command! */
   return teaching(sarg);
}
