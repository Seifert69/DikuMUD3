/*
 $Author: All $
 $RCSfile: teach.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.5 $
 */

#include "comm.h"
#include "common.h"
#include "formatter.h"
#include "guild.h"
#include "handler.h"
#include "interpreter.h"
#include "modify.h"
#include "money.h"
#include "skills.h"
#include "slog.h"
#include "szonelog.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"
#include "vme.h"
#include "vmelimits.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>

#define PRACTICE_COST_LEVEL (25)

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
    unit_data *teacher;     // Points to the teacher
    ubit8 type;             /* Ability, spell, skill, weapon */
    const char *pGuildName; // Empty or set to the name of the guild
    teacher_msg msgs;
    skill_teach_type *teaches; /* Array of skills */
    tree_type *tree;
    const char **text;
};

struct pc_train_values
{
    sbit16 *values;
    ubit8 *lvl;
    sbit32 *practice_points;
};

static int gold_cost(unit_data *ch, skill_teach_type *s, int level)
{
    // It is free to practice < level 25.
    if (ch->isChar() && CHAR_LEVEL(ch) < PRACTICE_COST_LEVEL)
        return 0;

    if (level < 1)
    {
        return s->min_cost_per_point;
    }
    else if (level >= s->max_skill)
    {
        return s->max_cost_per_point;
    }
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

    if (skill < 10)
    {
        return PRACTICE_SKILL_GAIN;
    }
    else if (skill < 20)
    {
        return PRACTICE_SKILL_GAIN - 1;
    }
    else if (skill < 40)
    {
        return PRACTICE_SKILL_GAIN - 2;
    }
    else if (skill < 75)
    {
        return PRACTICE_SKILL_GAIN - 3;
    }
    else
    {
        return 1;
    }
}

// cost: is the profession cost for this skill/ability, e.g. +2.
// racemodifier: is the race modifier for this skill/ability, e.g. +3
// level: 0 = 1st practice this char level, 1 = 2nd practice this char level, etc.
// virtual_level: is the vlevel of the player
//
// when modifiers sum to zero, it returns AVERAGE_SKILL_COST (10)
//
// If the sum of mods > 0 it goes down by one per mod to minimum of 3
// Otherwise, grows by 2x for each sum mod negative
// because half cost = 5, double cost = 20.
// For each training level after 1, cost increases by 50%.
//
// Returning zero means you can't learn more at this level.
// Otherwise returns the cost of training.
//
int actual_cost(int cost, sbit8 racemodifier, int level, int virtual_level)
{
    int mod = 0;
    int pct = 0;
    int avg_skill_cost = 0;
    int calccost = 0;

    avg_skill_cost = AVERAGE_SKILL_COST;

    /* Add in this section if you want training to be more expensive after vlevel 100.
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
    }*/

    pct = 100;
    mod = cost + racemodifier;

    if (level >= 1) // 2nd, 3rd ... attempt to train
    {
        pct = pow(1.5, level) * 100.0;

        if (level == 1)
        { // 2nd training attempt
            if (mod <= 0)
            {
                return 0; // You can not train second time per level if your total mod is 0 or worse
            }
        }

        if (level >= 2)
        { // 3rd training attempt this level
            if (mod < 3)
            {
                return 0; // You can not train three times per level if your total mod is worse than 3
            }
        }
    }

    if (mod == 0)
    {
        calccost = (avg_skill_cost * pct + 99) / 100;
    }
    else if (mod > 0)
    {
        if (mod > 5)
        {
            mod = 5; // 10-5 = 5 is the cheapest cost
        }

        // SO best possible training progression for any char is: 4, 6, 9, 14
        calccost = ((avg_skill_cost - mod) * pct + 99) / 100;
    }
    else // mod < 0
    {
        if (mod < -7)
        {
            mod = -7;
        }
        calccost = ((avg_skill_cost - 3 * mod) * pct + 99) / 100;
    }

    if (calccost > 5 * avg_skill_cost)
    { // Too expensive
        return 0;
    }
    else
    {
        return calccost;
    }
}

void clear_training_level(unit_data *ch)
{
    int i = 0;

    assert(ch->isPC());

    for (i = 0; i < SPL_TREE_MAX; i++)
    {
        UPC(ch)->setSpellLevelAtIndexTo(i, 0);
    }

    for (i = 0; i < WPN_TREE_MAX; i++)
    {
        UPC(ch)->setWeaponSkillLevelAtIndexTo(i, 0);
    }

    for (i = 0; i < SKI_TREE_MAX; i++)
    {
        UPC(ch)->setSkillLevelAtIndexTo(i, 0);
    }

    for (i = 0; i < ABIL_TREE_MAX; i++)
    {
        UPC(ch)->setAbilityLevelAtIndexTo(i, 0);
    }
}

int teaches_index(skill_teach_type *teaches_skills, int node)
{
    int i = 0;

    for (i = 0; teaches_skills[i].node != -1; i++)
    {
        if (teaches_skills[i].node == node)
        {
            return i;
        }
    }

    return -1;
}

const char *trainrestricted(unit_data *pupil, profession_cost *cost_entry, int minguildlevel, const char *pCurrentGuild)
{
    static char buf[MAX_STRING_LENGTH];
    char *c = nullptr;

    assert(pupil->isPC());

    strcpy(buf, "[REQ: ");
    c = buf;
    TAIL(c);

    // You must have at least the minimum GUILD level from the .def files or the teacher's def.
    if (char_guild_level(pupil, pCurrentGuild) < MAX(minguildlevel, cost_entry->min_level))
    {
        sprintf(c, "Guild level:%d ", MAX(minguildlevel, cost_entry->min_level));
        TAIL(c);
    }

    // This should never trigger, should always be caught above.
    if (PC_VIRTUAL_LEVEL(pupil) < cost_entry->min_level)
    {
        sprintf(c, "Level:%d ", cost_entry->min_level);
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
    {
        return "";
    }
    else
    {
        return buf;
    }
}

void info_show_one(unit_data *teacher,
                   unit_data *pupil,
                   ubit8 current_points,
                   ubit8 max_level,
                   int next_point,
                   int gold,
                   int lvl,
                   const char *text,
                   int indent,
                   ubit8 isleaf,
                   int min_level,
                   profession_cost *cost_entry,
                   std::vector<std::pair<int, std::string>> &vect,
                   const char *pGuildName)
{
    if (isleaf)
    {
        const char *req = nullptr;
        req = trainrestricted(pupil, cost_entry, min_level, pGuildName);

        if (*req)
        {
            auto str = diku::format_to_str("<div class='ca'>%s     %-20s %s</div><br/>", spc(4 * indent), text, req);
            vect.push_back(std::make_pair(1000, str));
            return;
        }

        if (current_points >= max_level)
        {
            auto str =
                diku::format_to_str("<div class='ca'>%s%3d%% %-20s [Teacher at max]</div><br/>", spc(4 * indent), current_points, text);
            vect.push_back(std::make_pair(1002, str));
            return;
        }

        if (next_point == 0)
        {
            auto str = diku::format_to_str("<div class='ca'>%s%3d%% %-20s [Practice next level]</div><br/>",
                                           spc(4 * indent),
                                           current_points,
                                           text);
            vect.push_back(std::make_pair(1001, str));
        }
        else
        {
            currency_t currency = local_currency(teacher);

            if (IS_SET(PC_FLAGS(pupil), PC_EXPERT))
            {
                auto str = diku::format_to_str("%s%s%3d%% %-20s [%3d%% of %3d%%, points %2d, %s] %s%s<br/>",
                                               next_point >= 20 ? "<div class='ca'>" : "",
                                               spc(4 * indent),
                                               current_points,
                                               text,
                                               current_points,
                                               max_level,
                                               next_point,
                                               money_string(money_round(TRUE, gold, currency, 1), currency, FALSE),
                                               std::string(lvl, '*').c_str(),
                                               next_point >= 20 ? "</div>" : "");
                vect.push_back(std::make_pair(next_point, str));
            }
            else
            {
                auto str = diku::format_to_str("%s%s%3d%% %-20s [practice points %3d] %s%s<br/>",
                                               next_point >= 20 ? "<div class='ca'>" : "",
                                               spc(4 * indent),
                                               current_points,
                                               text,
                                               next_point,
                                               std::string(lvl, '*').c_str(),
                                               next_point >= 20 ? "</div>" : "");
                vect.push_back(std::make_pair(next_point, str));
            }
        }
    }
    else // category, not isleaf
    {
        auto str = diku::format_to_str("%s     <a cmd='info #'>%s</a><br/>", spc(4 * indent), text);
        vect.push_back(std::make_pair(-1, str));
    }
}

bool pairISCompareAsc(const std::pair<int, std::string> &firstElem, const std::pair<int, std::string> &secondElem)
{
    return firstElem.first < secondElem.first;
}

void info_show_roots(unit_data *teacher,
                     unit_data *pupil,
                     skill_collection *pColl,
                     pc_train_values *pTrainValues,
                     skill_teach_type *teaches_skills,
                     const char *pGuildName)
{
    int i = 0;
    int cost = 0;
    std::vector<std::pair<int, std::string>> vect;

    for (i = 0; teaches_skills[i].node != -1; i++)
    {
        if ((!TREE_ISROOT(pColl->tree, teaches_skills[i].node) && !TREE_ISLEAF(pColl->tree, teaches_skills[i].node)) ||
            ((TREE_ISROOT(pColl->tree, teaches_skills[i].node) && TREE_ISLEAF(pColl->tree, teaches_skills[i].node))))
        {
            cost = actual_cost(pColl->prof_table[teaches_skills[i].node].profession_cost[PC_PROFESSION(pupil)],
                               pColl->racial[CHAR_RACE(pupil)][teaches_skills[i].node],
                               pTrainValues->lvl[teaches_skills[i].node],
                               PC_VIRTUAL_LEVEL(pupil));

            info_show_one(teacher,
                          pupil,
                          pTrainValues->values[teaches_skills[i].node],
                          teaches_skills[i].max_skill,
                          cost,
                          gold_cost(pupil, &teaches_skills[i], pTrainValues->values[teaches_skills[i].node]),
                          pTrainValues->lvl[teaches_skills[i].node],
                          pColl->text[teaches_skills[i].node],
                          0,
                          TREE_ISLEAF(pColl->tree, teaches_skills[i].node),
                          teaches_skills[i].min_glevel,
                          &pColl->prof_table[teaches_skills[i].node],
                          vect,
                          pGuildName);
        }
    }

    std::sort(vect.begin(), vect.end(), pairISCompareAsc);

    std::string str;
    str = "<pre>";
    for (auto it = vect.begin(); it != vect.end(); ++it)
    {
        str.append(it->second);
    }
    str.append("</pre>");

    if (str.length() <= 11)
    {
        str = "Nothing to show (try <a cmd='#'>info roots</a>)<br/>";
    }

    send_to_char(str.c_str(), pupil);
}

void info_show_leaves(unit_data *teacher,
                      unit_data *pupil,
                      skill_collection *pColl,
                      skill_teach_type *teaches_skills,
                      pc_train_values *pTrainValues,
                      const char *pGuildName)
{
    int i = 0;
    int cost = 0;
    std::vector<std::pair<int, std::string>> vect;

    for (i = 0; teaches_skills[i].node != -1; i++)
    {
        if (TREE_ISLEAF(pColl->tree, teaches_skills[i].node))
        {
            cost = actual_cost(pColl->prof_table[teaches_skills[i].node].profession_cost[PC_PROFESSION(pupil)],
                               pColl->racial[CHAR_RACE(pupil)][teaches_skills[i].node],
                               pTrainValues->lvl[teaches_skills[i].node],
                               PC_VIRTUAL_LEVEL(pupil));

            info_show_one(teacher,
                          pupil,
                          pTrainValues->values[teaches_skills[i].node],
                          teaches_skills[i].max_skill,
                          cost,
                          gold_cost(pupil, &teaches_skills[i], pTrainValues->values[teaches_skills[i].node]),
                          pTrainValues->lvl[teaches_skills[i].node],
                          pColl->text[teaches_skills[i].node],
                          0,
                          TREE_ISLEAF(pColl->tree, teaches_skills[i].node),
                          teaches_skills[i].min_glevel,
                          &pColl->prof_table[teaches_skills[i].node],
                          vect,
                          pGuildName);
        }
    }

    std::sort(vect.begin(), vect.end(), pairISCompareAsc);
    std::string str;
    str = "<pre>";
    for (auto it = vect.begin(); it != vect.end(); ++it)
    {
        if (IS_SET(PC_FLAGS(pupil), PC_EXPERT) || it->first <= 25)
        { // Limit display
            str.append(it->second);
        }
    }
    str.append("</pre>");

    if (str.length() <= 11)
    {
        str = "Nothing to show (try <a cmd='#'>info roots</a>)<br/>";
    }

    send_to_char(str.c_str(), pupil);
}

void info_one_skill(unit_data *teacher,
                    unit_data *pupil,
                    skill_collection *pColl,
                    pc_train_values *pTrainValues,
                    skill_teach_type *teaches_skills,
                    int teach_index,
                    teacher_msg *msgs,
                    const char *pGuildName)
{
    int indent = 0;
    int i = 0;
    int j = 0;
    int cost = 0;
    indent = 0;
    std::vector<std::pair<int, std::string>> vect;

    /* Find category if index is a leaf with a category parent */

    if (TREE_ISLEAF(pColl->tree, teaches_skills[teach_index].node) && !TREE_ISROOT(pColl->tree, teaches_skills[teach_index].node))
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
        cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)],
                           pColl->racial[CHAR_RACE(pupil)][i],
                           pTrainValues->lvl[i],
                           PC_VIRTUAL_LEVEL(pupil));

        info_show_one(teacher,
                      pupil,
                      pTrainValues->values[i],
                      teaches_skills[teach_index].max_skill,
                      cost,
                      gold_cost(pupil, &teaches_skills[teach_index], pTrainValues->values[i]),
                      pTrainValues->lvl[i],
                      pColl->text[i],
                      indent++,
                      TREE_ISLEAF(pColl->tree, teaches_skills[teach_index].node),
                      teaches_skills[teach_index].min_glevel,
                      &pColl->prof_table[i],
                      vect,
                      pGuildName);

        /* Show children of teach_index category */
        for (j = 0; teaches_skills[j].node != -1; j++)
        {
            if (TREE_ISLEAF(pColl->tree, teaches_skills[j].node) &&
                (TREE_PARENT(pColl->tree, teaches_skills[j].node) == teaches_skills[teach_index].node))
            {
                /* It is a child */
                i = teaches_skills[j].node;
                cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)],
                                   pColl->racial[CHAR_RACE(pupil)][i],
                                   pTrainValues->lvl[i],
                                   PC_VIRTUAL_LEVEL(pupil));
                info_show_one(teacher,
                              pupil,
                              pTrainValues->values[i],
                              teaches_skills[j].max_skill,
                              cost,
                              gold_cost(pupil, &teaches_skills[j], pTrainValues->values[i]),
                              pTrainValues->lvl[i],
                              pColl->text[i],
                              indent,
                              TREE_ISLEAF(pColl->tree, teaches_skills[j].node),
                              teaches_skills[j].min_glevel,
                              &pColl->prof_table[i],
                              vect,
                              pGuildName);
            }
        }
    }
    else // Leaf
    {
        /* Show all leaves, no category above */
        for (j = 0; teaches_skills[j].node != -1; j++)
        {
            if (TREE_ISLEAF(pColl->tree, teaches_skills[j].node))
            {
                /* It is a child */
                i = teaches_skills[j].node;
                cost = actual_cost(pColl->prof_table[i].profession_cost[PC_PROFESSION(pupil)],
                                   pColl->racial[CHAR_RACE(pupil)][i],
                                   pTrainValues->lvl[i],
                                   PC_VIRTUAL_LEVEL(pupil));

                info_show_one(teacher,
                              pupil,
                              pTrainValues->values[i],
                              teaches_skills[j].max_skill,
                              cost,
                              gold_cost(pupil, &teaches_skills[j], pTrainValues->values[i]),
                              pTrainValues->lvl[i],
                              pColl->text[i],
                              indent,
                              TREE_ISLEAF(pColl->tree, teaches_skills[j].node),
                              teaches_skills[j].min_glevel,
                              &pColl->prof_table[i],
                              vect,
                              pGuildName);
            }
        }
    }

    std::sort(vect.begin(), vect.end(), pairISCompareAsc);
    std::string str;
    str = "<pre>";
    for (auto it = vect.begin(); it != vect.end(); ++it)
    {
        str.append(it->second);
    }
    str.append("</pre>");
    send_to_char(str.c_str(), pupil);
}

int pupil_magic(unit_data *pupil)
{
    unit_affected_type *af = nullptr;

    for (af = pupil->getUnitAffected(); af; af = af->getNext())
    {
        switch (af->getID())
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
    }

    return FALSE;
}

void practice_base(int type, teach_packet *pckt, pc_train_values *pTrainValues, int teach_index, int cost)
{
    if (!TREE_ISLEAF(pckt->tree, pckt->teaches[teach_index].node))
    {
        slog(LOG_EXTENSIVE, 0, "ERROR: Practice base: not a leaf!");
        return;
    }

    *pTrainValues->practice_points -= cost;

    pTrainValues->lvl[pckt->teaches[teach_index].node]++;

    if (type == TEACH_ABILITIES)
    {
        pTrainValues->values[pckt->teaches[teach_index].node] += PRACTICE_ABILITY_GAIN;
    }
    else
    {
        pTrainValues->values[pckt->teaches[teach_index].node] += practice_skill_gain(pTrainValues->values[pckt->teaches[teach_index].node]);
    }

    // Set parent nodes to 1/2 for each level up

    int idx = pckt->teaches[teach_index].node;

    while (pTrainValues->values[idx] > 2 * pTrainValues->values[TREE_PARENT(pckt->tree, idx)])
    {
        int pidx = TREE_PARENT(pckt->tree, idx);

        pTrainValues->lvl[pidx]++;
        pTrainValues->values[pidx] = pTrainValues->values[idx] / 2;

        if (TREE_ISROOT(pckt->tree, pidx))
        {
            break;
        }

        idx = TREE_PARENT(pckt->tree, idx);
    }
}

int practice(unit_data *teacher,
             unit_data *pupil,
             skill_collection *pColl,
             pc_train_values *pTrainValues,
             teach_packet *pckt,
             int teach_index)
{
    int cost = 0;
    currency_t currency = local_currency(teacher);
    amount_t amt = 0;

    assert(teach_index != -1);

    if (!TREE_ISLEAF(pColl->tree, pckt->teaches[teach_index].node))
    {
        auto msg = diku::format_to_str("It is not possible to practice the category '%s'.<br/>"
                                       "The category is there to prevent you from being flooded with information.<br/>"
                                       "Try the command: 'info %s' on the category itself,<br/>"
                                       "to see which skills it contains.<br/>",
                                       pColl->text[pckt->teaches[teach_index].node],
                                       pColl->text[pckt->teaches[teach_index].node]);
        send_to_char(msg, pupil);
        return TRUE;
    }

    if (pckt->teaches[teach_index].max_skill <= pTrainValues->values[pckt->teaches[teach_index].node])
    {
        act(pckt->msgs.teacher_not_good_enough, A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
        return TRUE;
    }

    const char *req = nullptr;
    req = trainrestricted(pupil,
                          &pColl->prof_table[pckt->teaches[teach_index].node],
                          pckt->teaches[teach_index].min_glevel,
                          pckt->pGuildName);
    if (*req)
    {
        auto msg = diku::format_to_str("To practice %s you need to meet the following requirements %s.<br/>",
                                       pColl->text[pckt->teaches[teach_index].node],
                                       req);
        send_to_char(msg, pupil);
        return TRUE;
    }

    cost = actual_cost(pColl->prof_table[pckt->teaches[teach_index].node].profession_cost[PC_PROFESSION(pupil)],
                       pColl->racial[CHAR_RACE(pupil)][pckt->teaches[teach_index].node],
                       pTrainValues->lvl[pckt->teaches[teach_index].node],
                       PC_VIRTUAL_LEVEL(pupil));

    if (cost == 0)
    {
        act("$1n tells you, 'You've learned all you can about $2t at this level.'",
            A_ALWAYS,
            teacher,
            pColl->text[pckt->teaches[teach_index].node],
            pupil,
            TO_VICT);
        return TRUE;
    }

    assert(cost > 0);

    if (*pTrainValues->practice_points < cost)
    {
        auto str = diku::format_to_str(pckt->msgs.not_enough_points, cost);
        act(str.c_str(), A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
        if (CHAR_LEVEL(pupil) == START_LEVEL)
        {
            send_to_char("Beginners note: Go on adventure and gain a level.<br/>"
                         "Then come back and practice afterwards.<br/>",
                         pupil);
        }
        return TRUE;
    }

    if (pupil_magic(pupil))
    {
        act(pckt->msgs.not_pure, A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
        return TRUE;
    }

    /* Ok, now we can teach */

    amt = money_round(TRUE,
                      gold_cost(pupil, &pckt->teaches[teach_index], pTrainValues->values[pckt->teaches[teach_index].node]),
                      currency,
                      1);

    if (CHAR_LEVEL(pupil) > PRACTICE_COST_LEVEL)
    {
        if (!char_can_afford(pupil, amt, currency))
        {
            auto str = diku::format_to_str(pckt->msgs.not_enough_gold, money_string(amt, local_currency(pupil), TRUE));
            act(str.c_str(), A_SOMEONE, teacher, cActParameter(), pupil, TO_VICT);
            return TRUE;
        }

        money_from_unit(pupil, amt, currency);
    }

    practice_base(pckt->type, pckt, pTrainValues, teach_index, cost);

    act("You finish training $2t with $1n.", A_ALWAYS, teacher, pColl->text[pckt->teaches[teach_index].node], pupil, TO_VICT);

    return FALSE;
}

void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i = 0;
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
int auto_train(int type, unit_data *pupil, skill_collection *pColl, pc_train_values *pTrainValues, teach_packet *pckt, int costlimit)
{
    int teach_index = 0;
    int cost = 0;
    const char *req = nullptr;

    int nTrained = 0;

    int nLen = 0;

    // How many values do we need to train?
    for (nLen = 0; pckt->teaches[nLen].node != -1; nLen++)
    {
        ;
    }

    if (nLen < 1)
    {
        return 0;
    }

    int aTeachIndex[nLen];

    for (int i = 0; i < nLen; i++)
    {
        aTeachIndex[i] = i;
    }

    // When a char has nearly identical costs, let's shuffe around.
    shuffle(aTeachIndex, nLen);

    for (int i = 0; i < nLen; i++)
    {
        teach_index = aTeachIndex[i];
        int nodeidx = pckt->teaches[teach_index].node;

        if (TREE_ISLEAF(pColl->tree, nodeidx))
        {
            if (!pColl->tree[nodeidx].bAutoTrain)
            {
                continue;
            }

            if (pTrainValues->values[nodeidx] >= pckt->teaches[teach_index].max_skill)
            {
                continue;
            }

            req = trainrestricted(pupil, &pColl->prof_table[nodeidx], pckt->teaches[teach_index].min_glevel, pckt->pGuildName);
            if (*req)
            {
                continue;
            }

            cost = actual_cost(pColl->prof_table[nodeidx].profession_cost[PC_PROFESSION(pupil)],
                               pColl->racial[CHAR_RACE(pupil)][nodeidx],
                               pTrainValues->lvl[nodeidx],
                               PC_VIRTUAL_LEVEL(pupil));

            if (strcmp(pColl->text[nodeidx], "armor plate") == 0)
            {
                cost++;
            }

            if (cost < 1)
            {
                continue;
            }

            if (strcmp(pColl->text[nodeidx], "armor plate") == 0)
            {
                cost--;
            }

            // Once a skills, weapon or spell reaches 50, reduce likelihood slightly we'll train it again
            if (type != TEACH_ABILITIES)
            {
                if (pTrainValues->values[nodeidx] >= 50)
                {
                    int rnd = number(25, pTrainValues->values[nodeidx]);

                    if (rnd > 30)
                    {
                        continue;
                    }
                }
            }

            if ((type == TEACH_ABILITIES) && (cost > 10) && (pTrainValues->lvl[nodeidx] == 0) && (CHAR_LEVEL(pupil) > 2))
            {
                // Special logic to ensure at least a minimum of ability points

                if ((nodeidx == ABIL_CON) && (CHAR_ABILITY(pupil, ABIL_CON) < CHAR_ABILITY(pupil, ABIL_HP) / 2))
                {
                    ;
                }
                else if ((nodeidx == ABIL_BRA) &&
                         (CHAR_ABILITY(pupil, ABIL_BRA) < MAX(CHAR_ABILITY(pupil, ABIL_MAG), CHAR_ABILITY(pupil, ABIL_DIV)) / 2))
                {
                    ;
                }
                else if ((nodeidx == ABIL_CHA) &&
                         (CHAR_ABILITY(pupil, ABIL_CHA) < MAX(CHAR_ABILITY(pupil, ABIL_MAG), CHAR_ABILITY(pupil, ABIL_DIV)) / 2))
                {
                    ;
                }
                else if ((nodeidx == ABIL_HP) && ((CHAR_ABILITY(pupil, ABIL_HP) * cost) / 10 < CHAR_LEVEL(pupil)))
                {
                    ;
                }
                else if (cost > costlimit)
                {
                    continue;
                }
            }
            else
            {
                if (cost > costlimit)
                {
                    continue;
                }
            }

            if (*pTrainValues->practice_points < cost)
            {
                continue;
            }

            if (CHAR_LEVEL(pupil) > PRACTICE_COST_LEVEL)
            {
                currency_t currency = local_currency(pckt->teacher);

                /* Ok, now we pay for training */
                int amt = money_round(TRUE,
                                      gold_cost(pupil, &pckt->teaches[teach_index], pTrainValues->values[pckt->teaches[teach_index].node]),
                                      currency,
                                      1);

                if (!char_can_afford(pupil, amt, currency))
                {
                    auto str = diku::format_to_str(pckt->msgs.not_enough_gold, money_string(amt, local_currency(pupil), TRUE));
                    act(str.c_str(), A_ALWAYS, pckt->teacher, cActParameter(), pupil, TO_VICT);
                    continue;
                }

                money_from_unit(pupil, amt, currency);
            }

            nTrained++;

            practice_base(type, pckt, pTrainValues, teach_index, cost);

            act("You train $2t.", A_ALWAYS, pupil, pColl->text[nodeidx], cActParameter(), TO_CHAR);
        }
    }

    return nTrained;
}

// Pass symbolic name, get teach packet back
teach_packet *get_teacher(const char *pName)
{
    unit_data *u = nullptr;
    unit_fptr *f = nullptr;
    teach_packet *pckt = nullptr;

    char name[MAX_INPUT_LENGTH];
    char zone[MAX_INPUT_LENGTH];

    if (str_is_empty(pName))
    {
        return nullptr;
    }

    name[0] = 0;
    zone[0] = 0;

    split_fi_ref(pName, zone, name);

    u = find_symbolic(zone, name);

    if (!u)
    {
        slog(LOG_EXTENSIVE, 0, "ERROR: get_teacher() Teacher %s not found.", pName);
        return nullptr;
    }

    for (f = u->getFunctionPointer(); f; f = f->getNext())
    {
        if (f->getFunctionPointerIndex() == SFUN_TEACHING)
        {
            break;
        }
    }

    if (!f)
    {
        slog(LOG_EXTENSIVE, 0, "ERROR: get_teacher() FUNC for %s not found.", pName);
        return nullptr;
    }

    pckt = (teach_packet *)f->getData();
    assert(pckt);

    return pckt;
}

skill_collection *get_pc_train_values(unit_data *pupil, int type, pc_train_values *pValues)
{
    switch (type)
    {
        case TEACH_ABILITIES:
            pValues->values = UCHAR(pupil)->getAbilitiesArray().data(); // Current ability
            pValues->lvl = UPC(pupil)->getAbilityLevelArrayPtr();       // How many times you've trained this level on this ability
            pValues->practice_points = UPC(pupil)->getAbilityPointsPtr();
            return &g_AbiColl;

        case TEACH_SKILLS:
            pValues->values = UPC(pupil)->getSkillArrayPtr();
            pValues->lvl = UPC(pupil)->getSkillLevelArrayPtr();
            pValues->practice_points = UPC(pupil)->getSkillPointsPtr();
            return &g_SkiColl;

        case TEACH_SPELLS:
            pValues->values = UPC(pupil)->getSpellSkillArrayPtr();
            pValues->lvl = UPC(pupil)->getSpellLevelArrayPtr();
            pValues->practice_points = UPC(pupil)->getSkillPointsPtr();
            return &g_SplColl;
            break;

        case TEACH_WEAPONS:
            pValues->values = UPC(pupil)->getWeaponSkillArrayPtr();
            pValues->lvl = UPC(pupil)->getWeaponSkillLevelArrayPtr();
            pValues->practice_points = UPC(pupil)->getSkillPointsPtr();
            return &g_WpnColl;
            break;

        default:
            assert(FALSE);
    }

    return nullptr; // Can't happen
}

int teach_basis(spec_arg *sarg, teach_packet *pckt)
{
    int index = 0;
    char buf[MAX_INPUT_LENGTH + 10];
    const char *arg = nullptr;
    skill_collection *pColl = nullptr;

    if (!is_command(sarg->cmd, "info") && !is_command(sarg->cmd, "practice"))
    {
        return SFR_SHARE;
    }

    if (!sarg->activator->isPC())
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
        act("$1n is not capable of teaching now.", A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    static const char *remote_train[] = {"ability", "skill", "spell", "weapon", nullptr};
    arg = str_next_word(sarg->arg, buf);

    index = search_block(buf, remote_train, TRUE);
    if (index < 0)
    {
        arg = sarg->arg; // Restore the arg because no remote keyword was there.
    }

    pc_train_values TrainValues;
    pColl = get_pc_train_values(sarg->activator, pckt->type, &TrainValues);

    if (str_is_empty(arg))
    {
        if (is_command(sarg->cmd, "info"))
        {
            info_show_leaves(sarg->owner, sarg->activator, pColl, pckt->teaches, &TrainValues, pckt->pGuildName);
            auto msg = diku::format_to_str("<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
            send_to_char(msg, sarg->activator);
        }
        else
        {
            act("$1n asks, 'What do you wish to practice, $3n?'", A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
            if (CHAR_LEVEL(sarg->activator) == START_LEVEL)
            {
                send_to_char("Beginners note: Try the 'info' command NOW.<br/>", sarg->activator);
            }
        }

        return SFR_BLOCK;
    }

    arg = skip_spaces(arg);

    if (str_ccmp(arg, "roots") == 0)
    {
        info_show_roots(sarg->owner, sarg->activator, pColl, &TrainValues, pckt->teaches, pckt->pGuildName);
        auto msg = diku::format_to_str("<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
        send_to_char(msg, sarg->activator);
        return SFR_BLOCK;
    }

    if (strncmp(arg, "auto", 4) == 0)
    {
        if (TrainValues.values == nullptr)
        {
            send_to_char("<br/>Please specify ability, skill, spell, weapon before the auto keyword.<br/>", sarg->activator);
        }
        else
        {
            int nCount = 0;
            teach_packet *p[4];
            extra_descr_data *exd = nullptr;

            exd = PC_INFO(sarg->activator).find_raw("$autotrain");

            if (!exd)
            {
                send_to_char("Your guild is not setup properly. If you're already in a guild please contact an administrator.<br/>",
                             sarg->activator);
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
                    {
                        continue;
                    }

                    pColl = get_pc_train_values(sarg->activator, p[j]->type, &TrainValues);
                    nCount += auto_train(p[j]->type, sarg->activator, pColl, &TrainValues, p[j], i);
                }
            }

            auto msg = diku::format_to_str("<br/>Done auto practicing. You have %d ability and %d skill points left.<br/>",
                                           PC_ABILITY_POINTS(sarg->activator),
                                           PC_SKILL_POINTS(sarg->activator));
            send_to_char(msg, sarg->activator);
        }

        return SFR_BLOCK;
    }

    index = search_block_abbrevs(arg, pckt->text, (const char **)&arg);

    if (index == -1)
    {
        act(pckt->msgs.unknown_skill, A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    index = teaches_index(pckt->teaches, index);
    if (index == -1)
    {
        act(pckt->msgs.no_teaching, A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    if (is_command(sarg->cmd, "info"))
    {
        info_one_skill(sarg->owner, sarg->activator, pColl, &TrainValues, pckt->teaches, index, &pckt->msgs, pckt->pGuildName);
        auto msg = diku::format_to_str("<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
        send_to_char(msg, sarg->activator);
    }
    else /* Practice! */
    {
        practice(sarg->owner, sarg->activator, pColl, &TrainValues, pckt, index);

        /* If hpp changed, then update no of maximum hitpoints */
        sarg->activator->setMaximumHitpoints(hit_limit(sarg->activator));

        info_one_skill(sarg->owner, sarg->activator, pColl, &TrainValues, pckt->teaches, index, &pckt->msgs, pckt->pGuildName);
        auto msg = diku::format_to_str("<br/>You have %d practice points left.<br/>", *(TrainValues.practice_points));
        send_to_char(msg, sarg->activator);
    }

    return SFR_BLOCK;
}

int teaching(spec_arg *sarg)
{
    teach_packet *packet = nullptr;

    packet = (teach_packet *)sarg->fptr->getData();
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

        sarg->fptr->setData(nullptr);
        return SFR_BLOCK;
    }

    return teach_basis(sarg, packet);
}

const char *get_next_str(const char *data, char *dest)
{
    while (isspace(*data))
    {
        data++;
    }

    while (*data && *data != ';')
    {
        *dest++ = *data++;
    }

    if (*data)
    { /* == ';' */
        data++;
    }

    *dest = 0;

    return data;
}

//
// Given a profession's cost nCost what is the maximum delta to 100.
//
int max_skill_mod(int nCost)
{
    if (nCost == 0)
    {
        return -20; // 80 max
    }
    else if (nCost == 1)
    {
        return -10; // 90 max
    }
    else if (nCost == 2)
    {
        return 0; // 100 max
    }
    else if (nCost == 3)
    {
        return +5; // 105 max
    }
    else if (nCost == 4)
    {
        return +8; // 108 max
    }
    else if (nCost >= 5)
    {
        return 10 + nCost - 5; // 110 max for 5, 111 max for 6, etc.
    }
    else if (nCost == -1)
    {
        return -30; // 70 max
    }
    else if (nCost == -2)
    {
        return -40; // 60 max
    }
    else if (nCost == -3)
    {
        return -50; // 50 max
    }
    else if (nCost == -4)
    {
        return -60; // 40 max
    }
    else if (nCost == -5)
    {
        return -70; // 30 max
    }
    else if (nCost == -6)
    {
        return -85; // 15 max
    }
    else if (nCost <= -7)
    {
        return -100; // 0 max
    }

    // We should never get here
    return -100;
}

// level, max-train, <skill>, min_cost_per_point (gold), max_cost_per_point (gold), {costs}, 0
// {costs} is one or more integers
// I'm a bit puzzled on the {costs}, it seems like for each time you train cost goes up the the
// next array item. I would have expected it was more like one up every 5 or more training sessions.
int teach_init(spec_arg *sarg)
{
    const char *c = nullptr;
    int i = 0;
    int count = 0;
    int n = 0;
    // int realm = -1;
    char buf[MAX_STRING_LENGTH];
    teach_packet *packet = nullptr;
    skill_teach_type a_skill;

    static const char *teach_types[] = {"abilities", "spells", "skills", "weapons", nullptr};

    if (!(c = (char *)sarg->fptr->getData()))
    {
        szonelog(sarg->owner->getFileIndex()->getZone(), "%s: No text data for teacher-init!", sarg->owner->getFileIndexSymName());
        destroy_fptr(sarg->owner, sarg->fptr);
        return SFR_BLOCK;
    }

    c = get_next_str(c, buf); /* Get type */

    if ((i = search_block(buf, teach_types, TRUE)) == -1)
    {
        szonelog(sarg->owner->getFileIndex()->getZone(),
                 "%s: Illegal teach type %s in "
                 "teacher init!",
                 sarg->owner->getFileIndexSymName(),
                 buf);
        destroy_fptr(sarg->owner, sarg->fptr);
        return SFR_BLOCK;
    }

    CREATE(packet, teach_packet, 1);

    packet->teacher = sarg->owner;
    packet->type = i;

    // --- Next field ---

    c = get_next_str(c, buf);

    // This used to be the guild level type for teachers. Obsoleted. So packet->level_type is obsoleted.
    // 2022: This is now once again in the game.
    //
    // Now this is used where the empty string means only teach what's in the text.
    // Otherwise the string must be set to a profession. And in that case, all skills from that profession
    // are copied onto the teacher.  And additional text after that is used to modify the defaults.
    //
    int nProfession = -1;
    if (!str_is_empty(buf))
    {
        if ((nProfession = search_block(buf, g_professions, TRUE)) == -1)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Unknown profession %s in teacher-init.",
                     sarg->owner->getFileIndexSymName(),
                     buf);
        }
    }

    // Guild name (empty if not a guild?)
    c = get_next_str(c, buf);
    packet->pGuildName = str_dup(buf);

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
    CREATE(packet->teaches, skill_teach_type, 1);

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
                for (n = 0; packet->text[n] != nullptr; n++)
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

                        RECREATE(packet->teaches, skill_teach_type, count);
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
                for (n = 0; packet->text[n] != nullptr; n++)
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

                        RECREATE(packet->teaches, skill_teach_type, count);
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
                for (n = 0; packet->text[n] != nullptr; n++)
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

                        RECREATE(packet->teaches, skill_teach_type, count);
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
                for (n = 0; packet->text[n] != nullptr; n++)
                {
                    if (g_WpnColl.prof_table[n].profession_cost[nProfession] >= -3 && !g_WpnColl.tree[n].bAutoTeacherNoAdd)
                    {
                        a_skill.min_glevel = 0;
                        a_skill.max_skill = 100 + max_skill_mod(g_WpnColl.prof_table[n].profession_cost[nProfession]);
                        a_skill.node = n;
                        a_skill.min_cost_per_point = 1;
                        a_skill.max_cost_per_point =
                            MAX(1, 1000 + -100 * max_skill_mod(g_WpnColl.prof_table[n].profession_cost[nProfession]));
                        packet->teaches[count - 1] = a_skill;

                        count++;

                        RECREATE(packet->teaches, skill_teach_type, count);
                        packet->teaches[count - 1].node = -1;
                        packet->teaches[count - 1].min_cost_per_point = -1;
                        packet->teaches[count - 1].max_cost_per_point = -1;
                    }
                }
            }
            break;

        default:
            szonelog(sarg->owner->getFileIndex()->getZone(), "%s: Illegal type in teacher-init", sarg->owner->getFileIndexSymName());
            break;
    }

    for (;;)
    {
        c = get_next_str(c, buf);
        if (!*buf)
        {
            break;
        }

        a_skill.min_glevel = atoi(buf);
        if (!is_in(a_skill.min_glevel, 0, 250))
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Minimum level expected "
                     "between 1 - 250 (is %s)",
                     sarg->owner->getFileIndexSymName(),
                     buf);
            c = str_line(c, buf);
            continue;
        }

        c = get_next_str(c, buf);
        if (!*buf)
        {
            break;
        }

        a_skill.max_skill = atoi(buf);

        if (!is_in(a_skill.max_skill, 1, 150))
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Maximum skill expected "
                     "between 1 - 100 (is %s)",
                     sarg->owner->getFileIndexSymName(),
                     buf);
            c = str_line(c, buf);
            continue;
        }

        c = get_next_str(c, buf);
        if (!*buf)
        {
            break;
        }

        strip_trailing_blanks(buf);

        if ((i = search_block(buf, packet->text, TRUE)) == -1)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Illegal teacher-init "
                     "skill: %s",
                     sarg->owner->getFileIndexSymName(),
                     buf);
            c = str_line(c, buf);
            continue;
        }
        a_skill.node = i;

        if (packet->text == g_SplColl.text)
        {
            /* We have a spell teacher in Udgaard teaching all kinds of magic.

               if (realm == -1)
               {
                   realm = g_spell_info[i].realm;
               }
               else if (realm != g_spell_info[i].realm)
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
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Premature ending of "
                     "teacher-init! (expected cost)",
                     sarg->owner->getFileIndexSymName());
            c = str_line(c, buf);
            continue;
        }

        i = atoi(buf);
        if (i == 0)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Illegal min cost amount "
                     "in teacher init (be > 0): %s",
                     sarg->owner->getFileIndexSymName(),
                     buf);
            c = str_line(c, buf);
            continue;
        }
        a_skill.min_cost_per_point = i;

        c = get_next_str(c, buf);
        if (!*buf)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Premature ending of "
                     "teacher-init! (expected cost)",
                     sarg->owner->getFileIndexSymName());
            break;
        }

        i = atoi(buf);
        if (i == 0)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Illegal max cost amount "
                     "in teacher init (be > 0): %s",
                     sarg->owner->getFileIndexSymName(),
                     buf);
            c = str_line(c, buf);
            continue;
        }
        a_skill.max_cost_per_point = i;

        if (a_skill.max_cost_per_point <= a_skill.min_cost_per_point)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Max amount is <= "
                     "min amount... must be error!: %s",
                     sarg->owner->getFileIndexSymName(),
                     buf);
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
                szonelog(sarg->owner->getFileIndex()->getZone(),
                         "%s: Premature ending of "
                         "teacher-init! (expected point)",
                         sarg->owner->getFileIndexSymName());
                c = str_line(c, buf);
                continue;
            }

            i = atoi(buf);
            n++;
        } while ((i != 0) && (n < 15));

        if ((i != 0) || !*buf)
        {
            break;
        }

        if (n < 1)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "%s: Premature ending of "
                     "teacher-init! (expected points cost)",
                     sarg->owner->getFileIndexSymName());
            c = str_line(c, buf);
            continue;
        }

        /* Spheres are inserted automagically */
        if (!TREE_ISLEAF(packet->tree, a_skill.node))
        {
            continue;
        }

        for (n = 0; n < count; n++)
        {
            if (packet->teaches[n].node == a_skill.node)
            {
                break;
            }
        }

        if (n < count) // Aha, skill was here already
        {
            packet->teaches[n].min_glevel = a_skill.min_glevel;
            if (a_skill.max_skill < 100)
            {
                packet->teaches[n].max_skill = MIN(packet->teaches[n].max_skill, a_skill.max_skill);
            }
            if (a_skill.max_skill > 100)
            {
                packet->teaches[n].max_skill = MAX(packet->teaches[n].max_skill, a_skill.max_skill);
            }

            // Retain the calculated max & min and cost.
            // I'm quite sure they're "more fun & diverse"
            // Than what's been entered by hand.
        }
        else
        {
            packet->teaches[count - 1] = a_skill;
            count++;
            RECREATE(packet->teaches, skill_teach_type, count);

            packet->teaches[count - 1].node = -1;
            packet->teaches[count - 1].min_cost_per_point = -1;
            packet->teaches[count - 1].max_cost_per_point = -1;
        }
    }

    for (i = 0; packet->teaches[i].node != -1; i++)
    {
        if (TREE_ISLEAF(packet->tree, packet->teaches[i].node) && !TREE_ISROOT(packet->tree, packet->teaches[i].node))
        {
            if (teaches_index(packet->teaches, TREE_PARENT(packet->tree, packet->teaches[i].node)) == -1)
            {
                packet->teaches[count - 1].max_skill = 100;
                packet->teaches[count - 1].min_glevel = 0;
                packet->teaches[count - 1].node = TREE_PARENT(packet->tree, packet->teaches[i].node);
                packet->teaches[count - 1].min_cost_per_point = 0;
                packet->teaches[count - 1].max_cost_per_point = 0;

                count++;
                RECREATE(packet->teaches, skill_teach_type, count);

                packet->teaches[count - 1].node = -1;
                packet->teaches[count - 1].min_cost_per_point = -1;
                packet->teaches[count - 1].max_cost_per_point = -1;
            }
        }
    }
    packet->teaches[count - 1].node = -1;
    packet->teaches[count - 1].min_cost_per_point = -1;
    packet->teaches[count - 1].max_cost_per_point = -1;

    sarg->fptr->setFunctionPointerIndex(SFUN_TEACHING);
    sarg->fptr->setAllActivateOnEventFlags(SFB_CMD);
    sarg->fptr->setHeartBeat(PULSE_SEC);

    assert(sarg->fptr->getFunctionPointerIndex() != SFUN_DIL_INTERNAL);
    sarg->fptr->freeData(); // XXXX Need to get fixed. Free the text string!
    sarg->fptr->setData(packet);

    /* Call teaching in case initialization occurs with first command! */
    return teaching(sarg);
}
