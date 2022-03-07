/*
 $Author: All $
 $RCSfile: guild.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.5 $
 */
#include "cmdload.h"
#include "comm.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "magic.h"
#include "main_functions.h"
#include "money.h"
#include "skills.h"
#include "slog.h"
#include "spec_procs.h"
#include "str_parse.h"
#include "structs.h"
#include "szonelog.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utils.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

struct guild_type
{
    char *pGuildName;

    char **ppLeaveQuest;
    int nLeaveCost;

    char **ppEnterQuest;
    int nEnterCost;

    char **ppExcludeQuest;
};

extra_descr_data *find_quest(char *word, unit_data *unit)
{
    if (!IS_PC(unit) || !word)
    {
        return nullptr;
    }

    return PC_QUEST(unit).find_raw(word);
}

extra_descr_data *find_info(char *word, unit_data *unit)
{
    if (!IS_PC(unit) || !word)
    {
        return nullptr;
    }

    return PC_INFO(unit).find_raw(word);
}

// Return the guild level of the character's guild pGuildName
//
int char_guild_level(unit_data *ch, const char *pGuildName)
{
    extra_descr_data *exd = nullptr;

    assert(IS_CHAR(ch));

    if (IS_NPC(ch))
    {
        return CHAR_LEVEL(ch);
    }

    if (str_is_empty(pGuildName))
    {
        // E.g. a rogue teacher, not a guild teacher
        return MAX(CHAR_LEVEL(ch), PC_VIRTUAL_LEVEL(ch));
    }

    exd = find_info(str_cc("$guild/", pGuildName), ch);

    if (exd)
    {
        if (exd->names.Length() >= 2)
        {
            if (isdigit(*exd->names.Name(1)))
            {
                return atoi(exd->names.Name(1));
            }
        }
    }

    return 0;
}

// Return the level of the character's current main guild
//
int char_guild_level(unit_data *ch)
{
    assert(IS_CHAR(ch));

    if (IS_NPC(ch))
    {
        return CHAR_LEVEL(ch);
    }

    return char_guild_level(ch, PC_GUILD(ch));
}

/* Send the string 'msg' to all players that are members of the guild  */
/* 'guild'. Uses the act() function to send string with $1 as 'member' */
/* $2 as 'nonmember' and $3 as character                               */
/* Message will never be sent to 'nonmember'                           */
void act_to_guild(const char *msg, char *guild, unit_data *member, unit_data *nonmember)
{
    descriptor_data *d = nullptr;

    if (guild == nullptr || *guild == '\0')
    {
        slog(LOG_ALL, 0, "No guild name in send_to_guild");
        return;
    }

    for (d = g_descriptor_list; d; d = d->getNext())
    {
        if (descriptor_is_playing(d) && (d->cgetCharacter() != nonmember) && IS_PC(d->cgetCharacter()) && PC_GUILD(d->cgetCharacter()) &&
            strcmp(PC_GUILD(d->cgetCharacter()), guild) == 0)
        {
            act(msg, A_ALWAYS, member, nonmember, d->cgetCharacter(), TO_VICT);
        }
    }
}

/* Purpose: Used to prevent non-members to enter a guild.                 */
/* Fptr->data is a string, containing                                     */
/*    "<exitname>#<guild name>#[location]#[exclude]#<msg1>#<msg2>"        */
/* Where <exitname> is 0,1,2,3,4,5 for n,e,s,w,u,d respectively           */
/* <location> is the symbolic name of the room/obj the mobile must be in  */
/* The message is sent to room, you can use $1 and $3n                    */
/* Example: "1#warrior#midgaard/cth_square3#guard#$1n stops you and says, */
/*          'Members only!'#$1n stops $3n, and says, 'Members only!'"     */

int guard_guild_way(spec_arg *sarg)
{
    char *str = nullptr;
    char *location = nullptr;
    char *excl = nullptr;
    char *msg1 = nullptr;
    char *msg2 = nullptr;
    char *guild_no = nullptr;
    int guild_cmp = 0;

    if ((str = (char *)sarg->fptr->getData()) && (sarg->cmd->inttype == DIR_CMD) && (sarg->cmd->dir == (*str - '0')) &&
        CHAR_IS_READY(sarg->owner))
    {
        guild_no = str + 2;

        if (!(location = strchr(guild_no, '#')) || !(excl = strchr(location + 1, '#')) || !(msg1 = strchr(excl + 1, '#')) ||
            !(msg2 = strchr(msg1 + 1, '#')))
        {
            slog(LOG_ALL, 0, "Illegal data string in guard_way: %s", str);
            return SFR_SHARE;
        }

        if (IS_PC(sarg->activator))
        {
            if ((PC_GUILD(sarg->activator) != nullptr) && (*PC_GUILD(sarg->activator) != '\0'))
            {
                *location = '\0';
                guild_cmp = strcmp(PC_GUILD(sarg->activator), guild_no);
                *location = '#';

                if (guild_cmp == 0)
                {
                    return SFR_SHARE;
                }
            }
            else
            {
                /* Uhm. Well, if you are not a member of *any* guild      */
                /* you should be able to enter... Well, maybe as a guest? */
                /* I mean, you need someone to assist you??? Hm? I'm too  */
                /* lazy to do so now. ANyway it is another fundtion       */
                return SFR_SHARE;
            }
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
