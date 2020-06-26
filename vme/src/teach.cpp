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

#define PRACTICE_COST_LEVEL (START_LEVEL + 5)

#define TEACH_ABILITIES 0
#define TEACH_SPELLS 1
#define TEACH_SKILLS 2
#define TEACH_WEAPONS 3

/* You can practice a low cost skill 3 times per level. I.e. gain 3 ability in a prime stat.
 * meaning you could have 90 in an ability in 30 levels. Or you can have 90 skill in 15 levels.
 */
#define PRACTICE_ABILITY_GAIN (1)  // Was 2 in Diku II
#define PRACTICE_SKILL_GAIN (2)    // Was 5 in Diku II. 

/* Use -1 in node to indicate end of teach_type */
struct skill_teach_type
{
    ubit8 max_skill;        /* Maximum skill that can be taught            */
    ubit16 min_level;       /* What level do you have to be to learn this? */
    int node;               /* A node in a tree, e.g. index in skl_text[]  */
    int min_cost_per_point; /* The gold point cost per point  */
    int max_cost_per_point; /* The gold point cost per point  */
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
    ubit8 type;       /* Ability, spell, skill, weapon */
    // Obsolete, only guild level in teachers 0 for guild-level, 1 for ordinary level
    struct teacher_msg msgs;
    struct skill_teach_type *teaches; /* Array of skills */
    struct tree_type *tree;
    const char **text;
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


// 0 modifiers returns AVERAGE_SKILL_COST (10)
// If the sum of mods > 0 it goes down by one per mod to minimum of 3
// Otherwise, grows by 2x for each sum mod negative 
// because half cost = 5, double cost = 20.
// For each training level after 1, cost increases by 50%.
int actual_cost(int cost, sbit8 racemodifier, int level)
{
    int mod;
    int pct;

    pct = 100;
    if (level >= 1)
      pct = pow(1.5, level)*100.0;

    mod = cost + racemodifier;

    if (mod == 0)
        return (AVERAGE_SKILL_COST*pct+99)/100;
    else if (mod > 0)
    {
        if (mod > 5)
            mod = 5; // 10-5 = 5 is the cheapest cost

        // SO best possible training progression for any char is: 4, 6, 9, 14
        return ((AVERAGE_SKILL_COST - mod)*pct+99)/100; 
    }
    else // mod < 0
    {
        if (mod < -7)
            mod = -7;
        return ((AVERAGE_SKILL_COST - 3 * mod)*pct+99)/100;
    }
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

    strcpy(buf, "[REQ: ");
    c = buf;
    TAIL(c);

    if (CHAR_LEVEL(pupil) < cost_entry->min_level)
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
            sprintf(c, "%s:%d ", abil_text[i], cost_entry->min_abil[i]);
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
                   const char *text,
                   int indent, ubit8 isleaf, int min_level,
                   struct profession_cost *cost_entry)
{
    char buf[256];

    if (isleaf)
    {
        const char *req;
        req = trainrestricted(pupil, cost_entry, min_level);

        if (*req)
        {
            sprintf(buf, "<pre>%s     %-20s %s</pre>",
                    spc(4 * indent), text, req);
            send_to_char(buf, pupil);
            return;
        }

        if (next_point == 0)
        {
            sprintf(buf, "<pre>%s%3d%% %-20s [Practice next level]</pre>",
                    spc(4 * indent), current_points, text);
        }
        else
        {
            currency_t currency = local_currency(teacher);

            if (IS_SET(PC_FLAGS(pupil), PC_EXPERT))
                sprintf(buf,
                        "<pre>%s%3d%% %-20s [%3d%% of %3d%%, points %2d, %s]</pre>",
                        spc(4 * indent),  current_points, text,   current_points, max_level,
                        next_point,
                        money_string(money_round(TRUE, gold, currency, 1),
                                     currency, FALSE));
            else
                sprintf(buf, "<pre>%s%3d%% %-20s [practice points %3d]</pre>",
                        spc(4 * indent), current_points, text, next_point);
        }
    }
    else
        sprintf(buf, "<pre>%s     %-20s</pre>", spc(4 * indent), text);

    send_to_char(buf, pupil);
}



void info_show_roots(class unit_data *teacher, class unit_data *pupil,
                     struct skill_teach_type *teaches_skills,
                     struct tree_type *tree,
                     const char *text[],
                     sbit16 pc_values[],
                     ubit8 pc_lvl[], sbit8 pc_cost[], struct profession_cost *cost_table)
{
    int i, cost;

    for (i = 0; teaches_skills[i].node != -1; i++)
        if ((!TREE_ISROOT(tree, teaches_skills[i].node) &&
             !TREE_ISLEAF(tree, teaches_skills[i].node)) ||
            ((TREE_ISROOT(tree, teaches_skills[i].node) &&
              TREE_ISLEAF(tree, teaches_skills[i].node))))
        {
            cost = actual_cost(cost_table[teaches_skills[i].node].profession_cost[PC_PROFESSION(pupil)],
                               pc_cost[teaches_skills[i].node],
                               pc_lvl[teaches_skills[i].node]);

            info_show_one(teacher, pupil,
                          pc_values[teaches_skills[i].node],
                          teaches_skills[i].max_skill,
                          cost,
                          gold_cost(&teaches_skills[i],
                                    pc_values[teaches_skills[i].node]),
                          text[teaches_skills[i].node], 0,
                          TREE_ISLEAF(tree, teaches_skills[i].node),
                          teaches_skills[i].min_level,
                          &cost_table[teaches_skills[i].node]);
        }
}

void info_one_skill(class unit_data *teacher, class unit_data *pupil,
                    struct skill_teach_type *teaches_skills,
                    struct tree_type *tree,
                    const char *text[],
                    sbit16 pc_values[], ubit8 pc_lvl[], sbit8 pc_cost[],
                    struct profession_cost *cost_table,
                    int teach_index, struct teacher_msg *msgs)
{
    int indent, i, j, cost;
    indent = 0;

    /* Find category if index is a leaf with a category parent */

    if (TREE_ISLEAF(tree, teaches_skills[teach_index].node) &&
        !TREE_ISROOT(tree, teaches_skills[teach_index].node))
    {
        i = TREE_PARENT(tree, teaches_skills[teach_index].node);

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
    if (!TREE_ISLEAF(tree, teaches_skills[teach_index].node))
    {
        i = teaches_skills[teach_index].node;
        cost = actual_cost(cost_table[i].profession_cost[PC_PROFESSION(pupil)], pc_cost[i], pc_lvl[i]);

        info_show_one(teacher, pupil, pc_values[i],
                      teaches_skills[teach_index].max_skill,
                      cost,
                      gold_cost(&teaches_skills[teach_index], pc_values[i]),
                      text[i],
                      indent++,
                      TREE_ISLEAF(tree, teaches_skills[teach_index].node),
                      teaches_skills[teach_index].min_level,
                      &cost_table[i]);

        /* Show children of teach_index category */
        for (j = 0; teaches_skills[j].node != -1; j++)
            if (TREE_ISLEAF(tree, teaches_skills[j].node) &&
                (TREE_PARENT(tree, teaches_skills[j].node) ==
                 teaches_skills[teach_index].node))
            {
                /* It is a child */
                i = teaches_skills[j].node;
                cost = actual_cost(cost_table[i].profession_cost[PC_PROFESSION(pupil)], pc_cost[i], pc_lvl[i]);
                info_show_one(teacher, pupil, pc_values[i],
                              teaches_skills[j].max_skill,
                              cost,
                              gold_cost(&teaches_skills[j], pc_values[i]),
                              text[i],
                              indent,
                              TREE_ISLEAF(tree, teaches_skills[j].node),
                              teaches_skills[j].min_level,
                              &cost_table[i]);
            }
    }
    else // Leaf
    {
        /* Show all leaves, no category above */
        for (j = 0; teaches_skills[j].node != -1; j++)
            if (TREE_ISLEAF(tree, teaches_skills[j].node))
            {
                /* It is a child */
                i = teaches_skills[j].node;
                cost = actual_cost(cost_table[i].profession_cost[PC_PROFESSION(pupil)], pc_cost[i], pc_lvl[i]);

                info_show_one(teacher, pupil, pc_values[i],
                              teaches_skills[j].max_skill,
                              cost,
                              gold_cost(&teaches_skills[j], pc_values[i]),
                              text[i],
                              indent,
                              TREE_ISLEAF(tree, teaches_skills[j].node),
                              teaches_skills[j].min_level,
                              &cost_table[i]);
            }
    }
}

int pupil_magic(class unit_data *pupil)
{
    struct unit_affected_type *af;

    for (af = UNIT_AFFECTED(pupil); af; af = af->next)
        switch (af->id)
        {
        case ID_BLESS:
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

int practice(struct spec_arg *sarg, struct teach_packet *pckt,
             struct tree_type *tree,
             const char *text[],
             sbit16 pc_values[], ubit8 pc_lvl[], sbit8 pc_cost[],
             struct profession_cost *cost_table,
             sbit32 *practice_points, int teach_index)
{
    int cost;
    char buf[512];
    currency_t currency = local_currency(sarg->owner);
    amount_t amt;

    assert(teach_index != -1);

    if (!TREE_ISLEAF(tree, pckt->teaches[teach_index].node))
    {
        sprintf(buf,
                "It is not possible to practice the category '%s'.<br/>"
                "The category is there to prevent you from being flooded with information.<br/>"
                "Try the command: 'info %s' on the category itself,<br/>"
                "to see which skills it contains.<br/>",
                text[pckt->teaches[teach_index].node],
                text[pckt->teaches[teach_index].node]);
        send_to_char(buf, sarg->activator);
        return TRUE;
    }

    if (pckt->teaches[teach_index].max_skill <=
        pc_values[pckt->teaches[teach_index].node])
    {
        act(pckt->msgs.teacher_not_good_enough,
            A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        return TRUE;
    }

    const char *req;
    req = trainrestricted(sarg->activator, &cost_table[pckt->teaches[teach_index].node], pckt->teaches[teach_index].min_level);
    if (*req)
    {
        sprintf(buf, "To practice %s you need to meet the following requirements %s.<br/>",
                text[pckt->teaches[teach_index].node], req);
        send_to_char(buf, sarg->activator);
        return TRUE;

    }

    cost = actual_cost(cost_table[pckt->teaches[teach_index].node].profession_cost[PC_PROFESSION(sarg->activator)],
                       pc_cost[pckt->teaches[teach_index].node], pc_lvl[pckt->teaches[teach_index].node]);

    if (cost == 0)
    {
        act("$1n tells you, 'You've learned all you can about $2t at this level.'",
            A_ALWAYS, sarg->owner,
            text[pckt->teaches[teach_index].node], sarg->activator, TO_VICT);
        return TRUE;
    }

    assert(cost > 0);

    if (*practice_points < cost)
    {
        sprintf(buf, pckt->msgs.not_enough_points, cost);
        act(buf, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        if (CHAR_LEVEL(sarg->activator) == START_LEVEL)
            send_to_char("Beginners note: Go on adventure and gain a level.<br/>"
                         "Then come back and practice afterwards.<br/>",
                         sarg->activator);
        return TRUE;
    }

    if (pupil_magic(sarg->activator))
    {
        act(pckt->msgs.not_pure, A_SOMEONE, sarg->owner, 0, sarg->activator,
            TO_VICT);
        return TRUE;
    }

    /* Ok, now we can teach */

    amt = money_round(TRUE, gold_cost(&pckt->teaches[teach_index], pc_values[pckt->teaches[teach_index].node]),
                      currency, 1);

    if (CHAR_LEVEL(sarg->activator) > PRACTICE_COST_LEVEL &&
        !char_can_afford(sarg->activator, amt, currency))
    {
        sprintf(buf, pckt->msgs.not_enough_gold,
                money_string(amt, local_currency(sarg->activator), TRUE));
        act(buf, A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        return TRUE;
    }

    *practice_points -= (int)cost;

    pc_lvl[pckt->teaches[teach_index].node]++;

    if (CHAR_LEVEL(sarg->activator) > PRACTICE_COST_LEVEL)
        money_from_unit(sarg->activator, amt, currency);

    if (pckt->type == TEACH_ABILITIES)
        pc_values[pckt->teaches[teach_index].node] += PRACTICE_ABILITY_GAIN;
    else
        pc_values[pckt->teaches[teach_index].node] += PRACTICE_SKILL_GAIN;

    int idx = pckt->teaches[teach_index].node;

    while (pc_values[idx] > 2 * pc_values[TREE_PARENT(tree, idx)])
    {
        int pidx = TREE_PARENT(tree, idx);

        pc_lvl[pidx]++;
        pc_values[pidx] = pc_values[idx] / 2;

        if (TREE_ISROOT(tree, pidx))
            break;

        idx = TREE_PARENT(tree, idx);
    }

    act("You finish training $2t with $1n.", A_ALWAYS,
        sarg->owner,
        text[pckt->teaches[teach_index].node], sarg->activator, TO_VICT);

    return FALSE;
}



int teach_basis(struct spec_arg *sarg, struct teach_packet *pckt)
{
    int index;
    sbit16 *pc_values = NULL;
    ubit8 *pc_lvl = NULL;
    sbit8 *pc_cost = NULL;
    sbit32 *practice_points = NULL;
    struct profession_cost *cost_table;
    char buf[MAX_INPUT_LENGTH];
    char *arg;

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
            A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    switch (pckt->type)
    {
    case TEACH_ABILITIES:
        pc_values = &CHAR_ABILITY(sarg->activator, 0);  // Current ability
        pc_lvl = &PC_ABI_LVL(sarg->activator, 0);       // How many times you've trained this level on this ability
        pc_cost = &racial_ability[CHAR_RACE(sarg->activator)][0];     // MS2020: Oddly racial mods saved on char?
        cost_table = ability_prof_table;
        practice_points = &PC_ABILITY_POINTS(sarg->activator);
        break;

    case TEACH_SKILLS:
        pc_values = &PC_SKI_SKILL(sarg->activator, 0);
        pc_lvl = &PC_SKI_LVL(sarg->activator, 0);
        pc_cost = &racial_skills[CHAR_RACE(sarg->activator)][0];
        cost_table = skill_prof_table;
        practice_points = &PC_SKILL_POINTS(sarg->activator);
        break;

    case TEACH_SPELLS:
        pc_values = &PC_SPL_SKILL(sarg->activator, 0);
        pc_lvl = &PC_SPL_LVL(sarg->activator, 0);
        pc_cost = &racial_spells[CHAR_RACE(sarg->activator)][0];
        cost_table = spell_prof_table;
        practice_points = &PC_SKILL_POINTS(sarg->activator);
        break;

    case TEACH_WEAPONS:
        pc_values = &PC_WPN_SKILL(sarg->activator, 0);
        pc_lvl = &PC_WPN_LVL(sarg->activator, 0);
        pc_cost = &racial_weapons[CHAR_RACE(sarg->activator)][0];
        cost_table = weapon_prof_table;
        practice_points = &PC_SKILL_POINTS(sarg->activator);
        break;

    default:
        assert(FALSE);
    }

    if (str_is_empty(sarg->arg))
    {
        if (is_command(sarg->cmd, "info"))
        {
            info_show_roots(sarg->owner, sarg->activator, pckt->teaches,
                            pckt->tree, pckt->text,
                            pc_values, pc_lvl, pc_cost, cost_table);
            sprintf(buf, "<br/>You have %lu practice points left.<br/>",
                    (unsigned long)*practice_points);
            send_to_char(buf, sarg->activator);
        }
        else
        {
            act("$1n asks, 'What do you wish to practice, $3n?'",
                A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
            if (CHAR_LEVEL(sarg->activator) == START_LEVEL)
                send_to_char("Beginners note: Try the 'info' command NOW.<br/>",
                             sarg->activator);
        }

        return SFR_BLOCK;
    }

    arg = skip_spaces(sarg->arg);
    index = search_block_abbrevs(arg, pckt->text, (const char **)&arg);

    if (index == -1)
    {
        act(pckt->msgs.unknown_skill,
            A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    index = teaches_index(pckt->teaches, index);
    if (index == -1)
    {
        act(pckt->msgs.no_teaching,
            A_SOMEONE, sarg->owner, 0, sarg->activator, TO_VICT);
        return SFR_BLOCK;
    }

    if (is_command(sarg->cmd, "info"))
    {
        info_one_skill(sarg->owner, sarg->activator,
                       pckt->teaches,
                       pckt->tree,
                       pckt->text,
                       pc_values, pc_lvl, pc_cost, cost_table,
                       index, &pckt->msgs);
        sprintf(buf, "<br/>You have %lu practice points left.<br/>",
                (unsigned long)*practice_points);
        send_to_char(buf, sarg->activator);
    }
    else /* Practice! */
    {
        practice(sarg, pckt,
                 pckt->tree,
                 pckt->text,
                 pc_values, pc_lvl, pc_cost, cost_table, practice_points, index);

        /* If hpp changed, then update no of maximum hitpoints */
        UNIT_MAX_HIT(sarg->activator) = hit_limit(sarg->activator);

        info_one_skill(sarg->owner, sarg->activator,
                       pckt->teaches,
                       pckt->tree,
                       pckt->text,
                       pc_values, pc_lvl, pc_cost, cost_table,
                       index, &pckt->msgs);
        sprintf(buf, "<br/>You have %lu practice points left.<br/>",
                (unsigned long)*practice_points);
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

    if (sarg->cmd->no != 0)
        return SFR_SHARE;

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

    switch (i)
    {
    case TEACH_ABILITIES:
        packet->tree = abil_tree;
        packet->text = abil_text;
        break;

    case TEACH_SKILLS:
        packet->tree = ski_tree;
        packet->text = ski_text;
        break;

    case TEACH_SPELLS:
        packet->tree = spl_tree;
        packet->text = spl_text;
        break;

    case TEACH_WEAPONS:
        packet->tree = wpn_tree;
        packet->text = wpn_text;
        break;

    default:
        szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Illegal type in "
                                            "teacher-init",
                 UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
        break;
    }

    c = get_next_str(c, buf);
    // It only makes sense to support the guild level type for teachers.
    // So packet->level_type is obsoleted
    if (!str_is_number(buf))
    {
        szonelog(UNIT_FI_ZONE(sarg->owner), "%s@%s: Illegal level-type in "
                                            "teacher-init.",
                 UNIT_FI_NAME(sarg->owner),
                 UNIT_FI_ZONENAME(sarg->owner));
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

    for (;;)
    {
        c = get_next_str(c, buf);
        if (!*buf)
            break;

        a_skill.min_level = atoi(buf);
        if (!is_in(a_skill.min_level, 0, 250))
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

        if (packet->text == spl_text)
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
                         UNIT_FI_ZONENAME(sarg->owner), spl_text[i]);
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

        packet->teaches[count - 1] = a_skill;
        count++;
        RECREATE(packet->teaches, struct skill_teach_type, count);
    }

    packet->teaches[count - 1].node = -1;
    packet->teaches[count - 1].min_cost_per_point = -1;
    packet->teaches[count - 1].max_cost_per_point = -1;

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
                packet->teaches[count - 1].min_level = 0;
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
    sarg->fptr->heart_beat = 0;
    FREE(sarg->fptr->data); /* Free the text string! */
    sarg->fptr->data = packet;

    /* Call teaching in case initialization occurs with first command! */
    return teaching(sarg);
}
