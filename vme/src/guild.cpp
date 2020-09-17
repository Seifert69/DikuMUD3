/*
 $Author: All $
 $RCSfile: guild.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.5 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "skills.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "spells.h"
#include "vmelimits.h"
#include "justice.h"
#include "affect.h"
#include "magic.h"
#include "utility.h"
#include "common.h"
#include "money.h"
#include "str_parse.h"
#include "cmdload.h"

struct guild_type
{
    char *pGuildName;

    char **ppLeaveQuest;
    int nLeaveCost;

    char **ppEnterQuest;
    int nEnterCost;

    char **ppExcludeQuest;
};

class extra_descr_data *find_quest(char *word, class unit_data *unit)
{
    if (!IS_PC(unit) || !word)
        return NULL;

    return PC_QUEST(unit).find_raw(word);
}

int char_guild_level(class unit_data *ch)
{
    ubit32 i;
    class extra_descr_data *exd;

    assert(IS_CHAR(ch));

    if (IS_NPC(ch))
        return CHAR_LEVEL(ch);

    if (PC_GUILD(ch) && *PC_GUILD(ch))
    {
        exd = find_quest(str_cc("$", PC_GUILD(ch)), ch);

        if (exd)
        {
            for (i = 0; i < exd->names.Length(); i++)
                if (isdigit(*exd->names.Name(i)))
                    return atoi(exd->names.Name(i));
        }
    }

    return CHAR_LEVEL(ch);
}

void advance_guild_level(class unit_data *ch)
{
    class extra_descr_data *exd;

    int lvl = char_guild_level(ch);
    ubit32 i;

    if (!IS_PC(ch))
        return;

    if (PC_GUILD(ch) && *PC_GUILD(ch))
    {
        exd = find_quest(str_cc("$", PC_GUILD(ch)), ch);

        if (exd)
        {
            for (i = 0; i < exd->names.Length(); i++)
                if (isdigit(*exd->names.Name(i)))
                {
                    lvl++;
                    exd->names.Substitute(i, itoa(lvl));
                    return;
                }
        }
    }
}

static void free_guild_data(struct guild_type *pGt)
{
    if (pGt->pGuildName)
        FREE(pGt->pGuildName);

    if (pGt->ppLeaveQuest)
        free_namelist(pGt->ppLeaveQuest);

    if (pGt->ppEnterQuest)
        free_namelist(pGt->ppEnterQuest);

    if (pGt->ppExcludeQuest)
        free_namelist(pGt->ppExcludeQuest);

    FREE(pGt);
}

static struct guild_type *
parse_guild_data(class unit_data *npc, char *pStr)
{
    char *pTmp1;
    struct guild_type *pG;
    int ok = 0;

    CREATE(pG, struct guild_type, 1);

    pTmp1 = pStr;

    pG->pGuildName = parse_match_name((const char **)&pTmp1, "Guild");
    pG->ppEnterQuest = parse_match_namelist((const char **)&pTmp1, "Guild Enter Quest");
    ok += parse_match_num((const char **)&pTmp1, "Guild Enter Cost", &pG->nEnterCost);
    pG->ppLeaveQuest = parse_match_namelist((const char **)&pTmp1, "Guild Leave Quest");
    ok += parse_match_num((const char **)&pTmp1, "Guild Leave Cost", &pG->nLeaveCost);
    pG->ppExcludeQuest = parse_match_namelist((const char **)&pTmp1, "Guild Exclude Quest");

    if (ok != 2 || !pG->pGuildName || !pG->ppLeaveQuest ||
        !pG->ppEnterQuest || !pG->ppExcludeQuest)
    {
        szonelog(UNIT_FI_ZONE(npc),
                 "GUILD-ERROR (%s@%s): Illegal initialization element.",
                 UNIT_FI_NAME(npc), UNIT_FI_ZONENAME(npc));
        free_guild_data(pG);
        return NULL;
    }

    return pG;
}

int guild_master_init(struct spec_arg *sarg)
{
    struct guild_type *pG;

    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        pG = (struct guild_type *)
            parse_guild_data(sarg->owner, (char *)sarg->fptr->data);
        if (pG == NULL)
        {
            destroy_fptr(sarg->owner, sarg->fptr);
        }
        else
        {
            assert(sarg->fptr->index != SFUN_DIL_INTERNAL);
            FREE(sarg->fptr->data);
            sarg->fptr->data = pG;
            sarg->fptr->index = SFUN_GUILD_INTERNAL;
        }
    }
    return SFR_SHARE;
}

/* Send the string 'msg' to all players that are members of the guild  */
/* 'guild'. Uses the act() function to send string with $1 as 'member' */
/* $2 as 'nonmember' and $3 as character                               */
/* Message will never be sent to 'nonmember'                           */
void act_to_guild(const char *msg, char *guild,
                  class unit_data *member, class unit_data *nonmember)
{
    class descriptor_data *d;

    extern class descriptor_data *descriptor_list;

    if (guild == NULL || *guild == '\0')
    {
        slog(LOG_ALL, 0, "No guild name in send_to_guild");
        return;
    }

    for (d = descriptor_list; d; d = d->next)
        if (descriptor_is_playing(d) &&
            (d->character != nonmember) &&
            IS_PC(d->character) &&
            PC_GUILD(d->character) &&
            strcmp(PC_GUILD(d->character), guild) == 0)
            act(msg, A_ALWAYS, member, nonmember, d->character, TO_VICT);
}

/* Purpose: To be used as a guild 'block' routine before special commands: */
/*          CMD_PRACTICE, CMD_INFO                                         */
/* Format:  <Guild Name><#><Text String>                                   */
/* Example: "wizard#This is a members only club, $3n."                     */
/*          $1 and $3 can be used in text string.                          */
/*                                                                         */
int teach_members_only(struct spec_arg *sarg)
{
    char *str;
    int guild;

    if ((is_command(sarg->cmd, "practice")) &&
        IS_PC(sarg->activator) && CHAR_AWAKE(sarg->owner))
    {
        if (!(str = strchr((char *)sarg->fptr->data, '#')))
        {
            slog(LOG_ALL, 0, "Error in SFUN argument, teach_members_only");
            return SFR_SHARE;
        }

        if (PC_GUILD(sarg->activator))
        {
            *str = 0;
            guild =
                strcmp((char *)sarg->fptr->data, PC_GUILD(sarg->activator));
            *str = '#';
        }
        else
            guild = -1;

        if (guild != 0)
        {
            act(str + 1, A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
            return SFR_BLOCK;
        }
    }

    return SFR_SHARE;
}

/* Purpose: Used to prevent non-members to enter a guild.                 */
/* Fptr->data is a string, containing                                     */
/*    "<exitname>#<guild name>#[location]#[exclude]#<msg1>#<msg2>"        */
/* Where <exitname> is 0,1,2,3,4,5 for n,e,s,w,u,d respectively           */
/* <location> is the symbolic name of the room/obj the mobile must be in  */
/* The message is sent to room, you can use $1 and $3n                    */
/* Example: "1#warrior#midgaard/cth_square3#guard#$1n stops you and says, */
/*          'Members only!'#$1n stops $3n, and says, 'Members only!'"     */

int guard_guild_way(struct spec_arg *sarg)
{
    char *str, *location, *excl = NULL, *msg1 = NULL, *msg2 = NULL, *guild_no;
    int guild_cmp;

    int charname_in_list(class unit_data * ch, char *arg);

    if ((str = (char *)sarg->fptr->data) && (sarg->cmd->inttype == DIR_CMD) &&
        (sarg->cmd->dir == (*str - '0')) && CHAR_IS_READY(sarg->owner))
    {
        guild_no = str + 2;

        if (!(location = strchr(guild_no, '#')) ||
            !(excl = strchr(location + 1, '#')) ||
            !(msg1 = strchr(excl + 1, '#')) || !(msg2 = strchr(msg1 + 1, '#')))
        {
            slog(LOG_ALL, 0, "Illegal data string in guard_way: %s", str);
            return SFR_SHARE;
        }

        if (IS_PC(sarg->activator))
        {
            if ((PC_GUILD(sarg->activator) != NULL) &&
                (*PC_GUILD(sarg->activator) != '\0'))
            {
                *location = '\0';
                guild_cmp = strcmp(PC_GUILD(sarg->activator), guild_no);
                *location = '#';

                if (guild_cmp == 0)
                    return SFR_SHARE;
            }
            else
                /* Uhm. Well, if you are not a member of *any* guild      */
                /* you should be able to enter... Well, maybe as a guest? */
                /* I mean, you need someone to assist you??? Hm? I'm too  */
                /* lazy to do so now. ANyway it is another fundtion       */
                return SFR_SHARE;
        }

        *excl = '\0';
        if (*(location + 1) && strcmp(location + 1, UNIT_FI_NAME(UNIT_IN(sarg->owner))))
        {
            *excl = '#';
            return SFR_SHARE;
        }
        *excl = '#';

        *msg1 = '\0';
        if (charname_in_list(sarg->activator, excl + 1))
        {
            *msg1 = '#';
            return SFR_SHARE;
        }
        *msg2 = '\0';
        act(msg1 + 1, A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_VICT);
        act(msg2 + 1, A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_NOTVICT);
        *msg1 = '#';
        *msg2 = '#';
        return SFR_BLOCK;
    }

    return SFR_SHARE;
}

void leave_guild(class unit_data *player)
{
    class extra_descr_data *exd;

    assert(PC_GUILD(player));

    if (CHAR_LEVEL(player) > START_LEVEL)
    {
        exd = find_quest(str_cc("$", PC_GUILD(player)), player);

        if (exd && !exd->names.IsName("quitter"))
            exd->names.AppendName("quitter");

        send_to_char("You are no longer a member of your guild.<br/>", player);
    }
    else
    {
        if ((exd = find_quest(str_cc("$", PC_GUILD(player)), player)))
            PC_QUEST(player).erase(exd);

        send_to_char("You are no longer a member of your guild, but you are "
                     "welcome back at any time.<br/>",
                     player);
    }

    FREE(PC_GUILD(player));

    PC_GUILD(player) = NULL;
    PC_GUILD_TIME(player) = PC_TIME(player).played;
}

void guild_banish_player(class unit_data *ch)
{
    char *c;
    class extra_descr_data *pExd;

    if (!IS_PC(ch))
        return;

    if (PC_GUILD(ch))
    {
        c = str_dup(PC_GUILD(ch));
        leave_guild(ch);

        pExd = find_quest(str_cc("$", c), ch);
        if (pExd && !pExd->names.IsName("banished"))
        {
            pExd->names.AppendName("banished");
            send_to_char("You have been banished from your own guild!<br/>",
                         ch);
        }

        FREE(c);
    }
}

int can_leave_guild(struct guild_type *pG,
                    class unit_data *master, class unit_data *ch)
{
    char **p;
    currency_t currency = local_currency(master);

    if (!IS_PC(ch))
        return FALSE;

    if (!PC_GUILD(ch) || (strcmp(PC_GUILD(ch), pG->pGuildName) != 0))
    {
        act("$1n says, 'You are not a member here, $3n'",
            A_SOMEONE, master, cActParameter(), ch, TO_ROOM);
        return FALSE;
    }

    if (CHAR_LEVEL(ch) > START_LEVEL)
    {
        for (p = pG->ppLeaveQuest; *p; p++)
        {
            if (*p && **p)
            {
                if (find_quest(*p, ch) == NULL)
                {
                    act("$1n says, 'You must first complete the $2t quest, $3n'",
                        A_SOMEONE, master, *p, ch, TO_ROOM);
                    return FALSE;
                }
            }
        }

        if (!char_can_afford(ch, pG->nLeaveCost, currency))
        {
            act("$1n says, 'You can't afford the cost of $2t, $3n.'",
                A_SOMEONE, master, money_string(pG->nLeaveCost, currency, TRUE), ch, TO_ROOM);
            return FALSE;
        }
    }

    return TRUE;
}

void join_guild(class unit_data *ch, char *guild_name)
{
    class extra_descr_data *exd;

    assert(IS_PC(ch));

    if (PC_GUILD(ch))
        FREE(PC_GUILD(ch));

    PC_GUILD(ch) = str_dup(guild_name);
    PC_GUILD_TIME(ch) = PC_TIME(ch).played;

    exd = quest_add(ch, str_cc("$", PC_GUILD(ch)), itoa(time(0)));
    exd->names.AppendName("0");
    exd->names.AppendName("$guild");
}

int can_join_guild(struct guild_type *pG,
                   class unit_data *master, class unit_data *ch)
{
    currency_t currency = local_currency(master);
    char **p;

    if (!IS_PC(ch))
        return FALSE;

    if (PC_GUILD(ch))
    {
        if (strcmp(pG->pGuildName, PC_GUILD(ch)) == 0)
            act("$1n says, 'You are already a member, $3n'",
                A_SOMEONE, master, cActParameter(), ch, TO_ROOM);
        else
            act("$1n says, 'You must first break your ties with your current"
                " guild, $3n'",
                A_SOMEONE, master, cActParameter(), ch, TO_ROOM);
        return FALSE;
    }

    if (CHAR_LEVEL(ch) > START_LEVEL)
    {
        for (p = pG->ppEnterQuest; *p; p++)
        {
            if (**p && find_quest(*p, ch) == NULL)
            {
                act("$1n says, 'You must first complete the $2t quest, $3n'",
                    A_SOMEONE, master, *p, ch, TO_ROOM);
                return FALSE;
            }
        }

        if (find_quest(str_cc("$", pG->pGuildName), ch))
        {
            act("$1n says, 'Don't you think we remeber how you acted last "
                "time, $3n? ",
                A_SOMEONE, master, cActParameter(), ch, TO_ROOM);
            return FALSE;
        }

        for (p = pG->ppExcludeQuest; *p; p++)
        {
            if (find_quest(*p, ch))
            {
                act("$1n says, 'We will never be able to accept you as a "
                    "member due to the $2t quest, $3n'",
                    A_SOMEONE, master, *p, ch, TO_ROOM);
                return FALSE;
            }
        }

        if (!char_can_afford(ch, pG->nEnterCost, currency))
        {
            act("$1n says, 'You can't afford the entry cost of $2t, $3n.'",
                A_SOMEONE, master, money_string(pG->nEnterCost, currency, TRUE), ch, TO_ROOM);
            return FALSE;
        }
    }

    return TRUE;
}

/* You must be able to be expelled, to resign, to enroll and to obtain */
/* a higher guild status. All these matters will be implemented after  */
/* I have got the experience system to work.                           */
int guild_master(struct spec_arg *sarg)
{
    char *arg = (char *)sarg->arg;
    static int pc_pos = -2;
    struct guild_type *pG = (struct guild_type *)sarg->fptr->data;

    if (!pG)
        return SFR_SHARE;

    if (sarg->cmd->no == CMD_AUTO_EXTRACT)
    {
        free_guild_data(pG);
        sarg->fptr->data = NULL;
        return SFR_SHARE;
    }

    if (!sarg->activator || !IS_PC(sarg->activator))
        return SFR_SHARE;

    if (!CHAR_IS_READY(sarg->activator) || !CHAR_IS_READY(sarg->owner))
        return SFR_SHARE;

    if (strcmp(sarg->cmd->cmd_str, "join") == 0)
    {
        if (PC_GUILD(sarg->activator) == NULL ||
            *PC_GUILD(sarg->activator) == '\0')
        {
            if (can_join_guild(pG, sarg->owner, sarg->activator))
            {
                act("$1n says, 'Welcome in our guild, $3n'",
                    A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);

                if (CHAR_LEVEL(sarg->activator) > START_LEVEL)
                    money_transfer(sarg->activator, sarg->owner, pG->nEnterCost,
                                   local_currency(sarg->owner));

                join_guild(sarg->activator, pG->pGuildName);
            }
        }
        else
        {
            act("$1n says, 'You must first break your ties with your current"
                " guild, $3n'",
                A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
        }
        return SFR_BLOCK;
    }
    else if (is_command(sarg->cmd, "insult"))
    {
        if (find_unit(sarg->activator, &arg, 0, FIND_UNIT_SURRO) !=
            sarg->owner)
            return SFR_SHARE;

        if (pc_pos != PC_ID(sarg->activator))
        {
            act("$1n says, 'Do that again $3n and I will kick you out of this "
                "guild.'",
                A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
            pc_pos = PC_ID(sarg->activator);
        }
        else /* Match! */
        {
            if (can_leave_guild(pG, sarg->owner, sarg->activator))
            {
                act("$1n says, 'So be it. Buggar ye off, $3n'",
                    A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);

                if (CHAR_LEVEL(sarg->activator) > START_LEVEL)
                    money_transfer(sarg->activator, sarg->owner, pG->nLeaveCost,
                                   local_currency(sarg->owner));

                leave_guild(sarg->activator);

                pc_pos = -2;
            }
        }
        return SFR_BLOCK;
    }

    return SFR_SHARE;
}

/* Purpose: To be used as a guild general stuff routine.                   */
/* sarg->fptr->data contains guild name.                                         */
int guild_basis(struct spec_arg *sarg)
{
    int i;
    class unit_data *u;

    if (sarg->cmd->no == CMD_AUTO_DEATH && sarg->owner == sarg->activator)
    {
        scan4_unit(sarg->owner, UNIT_ST_PC);

        for (i = 0; i < unit_vector.top; i++)
        {
            u = unit_vector.units[i];
            if (IS_PC(u) && CHAR_FIGHTING(u) == sarg->owner && PC_GUILD(u) &&
                CHAR_CAN_SEE(sarg->owner, u))
            {
                if (strcmp(PC_GUILD(u), (char *)sarg->fptr->data) == 0)
                    guild_banish_player(u);
            }
        }

        return SFR_BLOCK;
    }

    if ((sarg->cmd->no == CMD_AUTO_COMBAT) && (CHAR_FIGHTING(sarg->activator) == sarg->owner))
    {
        /* Alert guild members of attack */
        /* Uhm, how do we know it is the 'first' attack, so that */
        /* we wont send this message all the time???             */
        act_to_guild("$1n tells you, 'Help! Our guild is attacked by $2n'",
                     (char *)sarg->fptr->data, sarg->owner, sarg->activator);
        return SFR_SHARE;
    }

    return SFR_SHARE;
}

int guild_title(struct spec_arg *sarg)
{
    char buf[MAX_STRING_LENGTH], male[MAX_STRING_LENGTH],
        female[MAX_STRING_LENGTH];
    char *c;
    int i, title_no;

    extern const char *pc_race_adverbs[PC_RACE_MAX + 1];

    if (!is_command(sarg->cmd, "title") || !IS_PC(sarg->activator))
        return SFR_SHARE;

    c = (char *)sarg->fptr->data;

    if (!(c = str_line(c, buf)))
        return SFR_BLOCK;

    if ((PC_GUILD(sarg->activator) == NULL) ||
        (strcmp(PC_GUILD(sarg->activator), buf) != 0))
    {
        act("$1n says, 'Thou art not a member $3n'",
            A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
        return SFR_BLOCK;
    }

    title_no =
        (PC_TIME(sarg->activator).played -
         PC_GUILD_TIME(sarg->activator)) /
        (SECS_PER_REAL_HOUR * 4);
    male[0] = female[0] = 0;

    for (i = 0;; i++)
    {
        if (!(c = str_line(c, male)))
            break;

        if (!(c = str_line(c, female)))
            break;

        if (i == title_no)
            break;
    }

    if (CHAR_SEX(sarg->activator) == SEX_FEMALE)
        sprintf(buf, female, pc_race_adverbs[CHAR_RACE(sarg->activator)]);
    else
        sprintf(buf, male, pc_race_adverbs[CHAR_RACE(sarg->activator)]);

    if (strcmp(buf, UNIT_TITLE_STRING(sarg->activator)) == 0)
    {
        if (c == NULL)
            act("$1n says, 'You have reached the ultimate title, $3n'",
                A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
        else
            act("$1n says, 'You must first be an older member $3N'",
                A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);
        return SFR_BLOCK;
    }

    UNIT_TITLE(sarg->activator) = (buf);

    act("$1n says, 'Enjoy the new title, $3n!'",
        A_SOMEONE, sarg->owner, cActParameter(), sarg->activator, TO_ROOM);

    return SFR_BLOCK;
}

void do_guild(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    char buf[MAX_STRING_LENGTH];
    int found = FALSE;
    class extra_descr_data *exd;

    if (!IS_PC(ch))
    {
        send_to_char("You ain't nothin' but a hound-dog...<br/>", ch);
        return;
    }

    for (exd = PC_QUEST(ch).m_pList; exd; exd = exd->next)
    {
        if (exd->names.IsName("$guild"))
        {
            found = TRUE;
            sprintf(buf, "%-30s %-2s<br/>",
                    exd->names.Name(0) + 1, exd->names.Name(1));
            send_to_char(buf, ch);
        }
    }

    if (!found)
        send_to_char("None.<br/>", ch);
}
