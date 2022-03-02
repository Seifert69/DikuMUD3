/*
 $Author: All $
 $RCSfile: act_info.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.6 $
 */

#include "comm.h"
#include "db.h"
#include "descriptor_data.h"
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
const char *in_string(const unit_data *ch, const unit_data *u)
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

void player_where(unit_data *ch, char *arg)
{
    descriptor_data *d = nullptr;
    int any = FALSE;

    for (d = g_descriptor_list; d; d = d->getNext())
    {
        if (d->cgetCharacter() && (d->cgetCharacter() != ch) && UNIT_IN(d->cgetCharacter()) && descriptor_is_playing(d) &&
            (str_is_empty(arg) || !str_ccmp(arg, UNIT_NAME(d->cgetCharacter()))) && CHAR_LEVEL(ch) >= UNIT_MINV(d->cgetCharacter()) &&
            d->cgetOriginalCharacter() == nullptr && CHAR_CAN_SEE(ch, d->cgetCharacter()) && unit_zone(ch) == unit_zone(d->cgetCharacter()))
        {
            auto msg = diku::format_to_str("%-30s at %s<br/>", UNIT_NAME(d->cgetCharacter()), TITLENAME(unit_room(d->getCharacter())));
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

void do_where(unit_data *ch, char *aaa, const command_info *cmd)
{
    unit_data *i = nullptr;
    descriptor_data *d = nullptr;
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

        for (d = g_descriptor_list; d; d = d->getNext())
        {
            if (d->cgetCharacter() && UNIT_IN(d->cgetCharacter()) && descriptor_is_playing(d) &&
                CHAR_LEVEL(ch) >= UNIT_MINV(d->cgetCharacter()) &&
                (d->cgetOriginalCharacter() == nullptr || CHAR_LEVEL(ch) >= UNIT_MINV(d->cgetOriginalCharacter())))
            {
                nCount++;
                std::string whose_body;
                if (d->cgetOriginalCharacter())
                { /* If switched */
                    whose_body = diku::format_to_str(" In body of %s", UNIT_NAME(d->cgetCharacter()));
                }

                mystr += diku::format_to_str("%-20s - %s [%s]%s<br/>",
                                             UNIT_NAME(CHAR_ORIGINAL(d->cgetCharacter())),
                                             UNIT_SEE_TITLE(ch, UNIT_IN(d->cgetCharacter())),
                                             in_string(ch, d->cgetCharacter()),
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
