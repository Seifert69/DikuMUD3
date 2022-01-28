/*
 $Author: All $
 $RCSfile: act_info.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include "comm.h"
#include "db.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "structs.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cstring>

/* also used in "corpses" wizard-command */
const char *in_string(class unit_data *ch, class unit_data *u)
{
    static std::string in_str;
    in_str.clear();
    while ((u = UNIT_IN(u)))
    {
        if (IS_ROOM(u))
        {
            in_str = diku::format_to_str("<a href='#' cmd='goto #'>%s@%s</a>", UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
            return in_str.c_str();
        }
        else
        {
            in_str += diku::format_to_str("%s/", UNIT_SEE_NAME(ch, u));
        }
    }

    /*  error(HERE, "Something that is UNIT_IN, not in a room!");*/

    return nullptr;
}

void player_where(class unit_data *ch, char *arg)
{
    class descriptor_data *d = nullptr;
    int any = FALSE;

    for (d = g_descriptor_list; d; d = d->next)
    {
        if (d->character && (d->character != ch) && UNIT_IN(d->character) && descriptor_is_playing(d) &&
            (str_is_empty(arg) || !str_ccmp(arg, UNIT_NAME(d->character))) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) &&
            d->original == nullptr && CHAR_CAN_SEE(ch, d->character) && unit_zone(ch) == unit_zone(d->character))
        {
            auto msg = diku::format_to_str("%-30s at %s<br/>", UNIT_NAME(d->character), TITLENAME(unit_room(d->character)));
            send_to_char(msg, ch);
            any = TRUE;
        }
    }

    if (!any)
    {
        if (str_is_empty(arg))
        {
            send_to_char("No other visible players in this area.<br/>", ch);
        }
        else
        {
            send_to_char("No such player found in this area.<br/>", ch);
        }
    }
}

void do_where(class unit_data *ch, char *aaa, const struct command_info *cmd)
{
    class unit_data *i = nullptr;
    class descriptor_data *d = nullptr;
    char *arg = (char *)aaa;
    std::string mystr;
    int nCount = 0;

    if (IS_MORTAL(ch))
    {
        player_where(ch, arg);
        return;
    }

    if (str_is_empty(arg))
    {
        mystr = "<u>Players</u><br/>";

        for (d = g_descriptor_list; d; d = d->next)
        {
            if (d->character && UNIT_IN(d->character) && descriptor_is_playing(d) && CHAR_LEVEL(ch) >= UNIT_MINV(d->character) &&
                (d->original == nullptr || CHAR_LEVEL(ch) >= UNIT_MINV(d->original)))
            {
                nCount++;
                std::string whose_body;
                if (d->original)
                { /* If switched */
                    whose_body = diku::format_to_str(" In body of %s", UNIT_NAME(d->character));
                }

                mystr += diku::format_to_str("%-20s - %s [%s]%s<br/>",
                                             UNIT_NAME(CHAR_ORIGINAL(d->character)),
                                             UNIT_SEE_TITLE(ch, UNIT_IN(d->character)),
                                             in_string(ch, d->character),
                                             whose_body);
            }
        }
    }
    else /* Arg was not empty */
    {
        mystr.clear();

        for (i = g_unit_list; i; i = i->gnext)
        {
            if (UNIT_IN(i) && UNIT_NAMES(i).IsName(arg) && CHAR_LEVEL(ch) >= UNIT_MINV(i))
            {
                nCount++;
                if (nCount++ > 100)
                {
                    continue;
                }

                mystr += diku::format_to_str("%-30s - %s [%s]<br/>",
                                             TITLENAME(i),
                                             UNIT_SEE_TITLE(ch, UNIT_IN(i)),
                                             (!in_string(ch, i) ? "MENU" : in_string(ch, i)));
            }
        }
    }

    if (mystr.empty())
    {
        send_to_char("Couldn't find any such thing.<br/>", ch);
    }
    else
    {
        if (nCount > 100)
        {
            mystr += "...<br/>";
        }
        mystr += "Found ";
        mystr += itoa(nCount);
        mystr += " matches<br/>";
        page_string(CHAR_DESCRIPTOR(ch), mystr.c_str());
    }
}
