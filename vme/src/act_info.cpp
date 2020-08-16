/*
 $Author: All $
 $RCSfile: act_info.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "files.h"
#include "skills.h"
#include "db.h"
#include "spells.h"
#include "vmelimits.h"
#include "affect.h"
#include "utility.h"
#include "trie.h"
#include "textutil.h"
#include "money.h"
#include "protocol.h"
#include "constants.h"
#include "common.h"
#include "guild.h"

/* extern variables */

extern class descriptor_data *descriptor_list;
extern class unit_data *unit_list;


/* also used in "corpses" wizard-command */
char *in_string(class unit_data *ch, class unit_data *u)
{
    static char in_str[512];
    char *tmp = in_str;

    while ((u = UNIT_IN(u)))
        if (IS_ROOM(u))
        {
            sprintf(tmp, "<a href='#' cmd='goto #'>%s@%s</a>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
            return in_str;
        }
        else
        {
            sprintf(tmp, "%s/", UNIT_SEE_NAME(ch, u));
            TAIL(tmp);
        }

    /*  error(HERE, "Something that is UNIT_IN, not in a room!");*/
    return (NULL);
}

void player_where(class unit_data *ch, char *arg)
{
    char buf[160];
    class descriptor_data *d;
    int any = FALSE;

    for (d = descriptor_list; d; d = d->next)
    {
        if (d->character && (d->character != ch) && UNIT_IN(d->character) && descriptor_is_playing(d) && (str_is_empty(arg) || !str_ccmp(arg, UNIT_NAME(d->character))) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) && d->original == NULL && CHAR_CAN_SEE(ch, d->character) &&
            unit_zone(ch) == unit_zone(d->character))
        {
            sprintf(buf, "%-30s at %s<br/>", UNIT_NAME(d->character),
                    TITLENAME(unit_room(d->character)));
            send_to_char(buf, ch);
            any = TRUE;
        }
    }

    if (!any)
    {
        if (str_is_empty(arg))
            send_to_char("No other visible players in this area.<br/>", ch);
        else
            send_to_char("No such player found in this area.<br/>", ch);
    }
}

void do_where(class unit_data *ch, char *aaa, const struct command_info *cmd)
{
    char buf1[MAX_STRING_LENGTH], buf2[512];
    register class unit_data *i;
    class descriptor_data *d;
    char *arg = (char *)aaa;
    string mystr;
    int nCount = 0;

    if (IS_MORTAL(ch))
    {
        player_where(ch, arg);
        return;
    }

    if (str_is_empty(arg))
    {
        mystr = "<u>Players</u><br/>";

        for (d = descriptor_list; d; d = d->next)
            if (d->character && UNIT_IN(d->character) && descriptor_is_playing(d) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) && (d->original == NULL || CHAR_LEVEL(ch) >= UNIT_MINV(d->original)))
            {
                nCount++;
                if (d->original) /* If switched */
                    sprintf(buf2, " In body of %s", UNIT_NAME(d->character));
                else
                    buf2[0] = '\0';

                sprintf(buf1, "%-20s - %s [%s]%s<br/>",
                        UNIT_NAME(CHAR_ORIGINAL(d->character)),
                        UNIT_SEE_TITLE(ch, UNIT_IN(d->character)),
                        in_string(ch, d->character), buf2);
                mystr.append(buf1);
            }
    }
    else /* Arg was not empty */
    {
        mystr = "";

        for (i = unit_list; i; i = i->gnext)
        {
            if (UNIT_IN(i) && UNIT_NAMES(i).IsName(arg) && CHAR_LEVEL(ch) >= UNIT_MINV(i))
            {
                nCount++;
                if (nCount++ > 100)
                    continue;

                sprintf(buf1, "%-30s - %s [%s]<br/>",
                        TITLENAME(i),
                        UNIT_SEE_TITLE(ch, UNIT_IN(i)),
                        (!in_string(ch, i) ? "MENU" : in_string(ch, i)));

                mystr.append(buf1);
            }
        }
    }

    if (mystr.length() < 1)
        send_to_char("Couldn't find any such thing.<br/>", ch);
    else
    {
        if (nCount > 100)
            mystr.append("...<br/>");
        mystr.append("Found ");
        mystr.append(itoa(nCount));
        mystr.append(" matches<br/>");
        page_string(CHAR_DESCRIPTOR(ch), mystr.c_str());
    }
}
