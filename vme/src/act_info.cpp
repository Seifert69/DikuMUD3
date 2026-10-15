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
#include "pcsave.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"

#include <cctype>
#include <ctime>

/**
 * @note also used in "corpses" wizard-command
 */
const char *in_string(const unit_data *ch, const unit_data *u)
{
    static std::string in_str;
    in_str.clear();
    while ((u = u->getUnitIn()))
    {
        if (u->isRoom())
        {
            in_str = diku::format_to_str("<a href='#' cmd='goto #'>%s</a>", u->getFileIndexSymName());
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
        if (d->cgetCharacter() && (d->cgetCharacter() != ch) && d->cgetCharacter()->getUnitIn() && descriptor_is_playing(d) &&
            (str_is_empty(arg) || !str_ccmp(arg, d->cgetCharacter()->getNames().Name())) &&
            CHAR_LEVEL(ch) >= d->cgetCharacter()->getLevelOfWizardInvisibility() && d->cgetOriginalCharacter() == nullptr &&
            CHAR_CAN_SEE(ch, d->cgetCharacter()) && unit_zone(ch) == unit_zone(d->cgetCharacter()))
        {
            auto msg =
                diku::format_to_str("%-30s at %s<br/>", d->cgetCharacter()->getNames().Name(), TITLENAME(unit_room(d->getCharacter())));
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
            if (d->cgetCharacter() && d->cgetCharacter()->getUnitIn() && descriptor_is_playing(d) &&
                CHAR_LEVEL(ch) >= d->cgetCharacter()->getLevelOfWizardInvisibility() &&
                (d->cgetOriginalCharacter() == nullptr || CHAR_LEVEL(ch) >= d->cgetOriginalCharacter()->getLevelOfWizardInvisibility()))
            {
                nCount++;
                std::string whose_body;
                if (d->cgetOriginalCharacter())
                { /* If switched */
                    whose_body = diku::format_to_str(" In body of %s", d->cgetCharacter()->getNames().Name());
                }

                mystr += diku::format_to_str("%-20s - %s [%s]%s<br/>",
                                             CHAR_ORIGINAL(d->getCharacter())->getNames().Name(),
                                             UNIT_SEE_TITLE(ch, d->getCharacter()->getUnitIn()),
                                             in_string(ch, d->cgetCharacter()),
                                             whose_body);
            }
        }
    }
    else /* Arg was not empty */
    {
        mystr.clear();

        for (i = g_unit_list; i; i = i->getGlobalNext())
        {
            if (i->getUnitIn() && i->getNames().IsName(arg) && CHAR_LEVEL(ch) >= i->getLevelOfWizardInvisibility())
            {
                nCount++;
                if (nCount++ > 100)
                {
                    continue;
                }

                mystr += diku::format_to_str("%-30s - %s [%s]<br/>",
                                             TITLENAME(i),
                                             UNIT_SEE_TITLE(ch, i->getUnitIn()),
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

/**
 * Whether "when" hides player pc from ch. The same rule for players in the game
 * and players read from their file, so the answer doesn't change when they log
 * on or off: wizard invisibility above ch's level, or invisibility that ch can't
 * detect. Unlike CHAR_CAN_SEE() it ignores light, blindness and where they are.
 */
static bool when_hides(const unit_data *ch, const unit_data *pc)
{
    if (ch == pc)
    {
        return false;
    }
    if (CHAR_LEVEL(ch) < pc->getLevelOfWizardInvisibility())
    {
        return true;
    }
    return CHAR_LEVEL(ch) < CREATOR_LEVEL && IS_SET(pc->getUnitFlags(), UNIT_FL_INVISIBLE) && !CHAR_HAS_FLAG(ch, CHAR_DETECT_INVISIBLE);
}

/**
 * when <player>: when a player last logged on. Players in the game, link dead
 * included, are "playing right now". Players hidden by when_hides() are treated
 * as unknown.
 */
void do_when(unit_data *ch, char *arg, const command_info *cmd)
{
    static const char *no_one = "No one by that name here.<br/>";
    char name[MAX_INPUT_LENGTH];

    if (str_is_empty(arg))
    {
        send_to_char("Usage: when &lt;player name&gt;<br/>", ch);
        return;
    }

    one_argument(arg, name);

    // The name becomes part of the player file path, so only letters, as in player names
    size_t len = strlen(name);
    if (len == 0 || len >= pc_data::PC_MAX_NAME)
    {
        send_to_char(no_one, ch);
        return;
    }
    for (size_t i = 0; i < len; i++)
    {
        if (!isalpha((unsigned char)name[i]))
        {
            send_to_char(no_one, ch);
            return;
        }
    }

    // Players are at the head of the unit list, as in "who"
    for (unit_data *u = g_unit_list; u && u->isPC(); u = u->getGlobalNext())
    {
        if (!str_ccmp(PC_FILENAME(u), name))
        {
            // Not the file: it may still hold the visibility from before they went invisible
            if (when_hides(ch, u))
            {
                send_to_char(no_one, ch);
                return;
            }
            send_to_char(diku::format_to_str("%s is playing right now.<br/>", u->getNames().Name()), ch);
            return;
        }
    }

    // Only the player file is read (not the inventory), and it is never saved
    unit_data *pc = load_player(name);
    if (pc == nullptr)
    {
        send_to_char(no_one, ch);
        return;
    }

    const time_t last = PC_TIME(pc).getPlayerLastConnectTime();
    const bool hidden = when_hides(ch, pc);
    const std::string pc_name = pc->getNames().Name();
    extract_unit(pc);

    if (hidden || last == 0)
    {
        send_to_char(no_one, ch);
        return;
    }

    std::string msg = diku::format_to_str("%s last logged on %s", pc_name, time_ago_string(last, time(nullptr)));

    char date[64];
    const tm *local = localtime(&last);
    if (local && strftime(date, sizeof(date), "%a %d %b %Y %H:%M", local))
    {
        msg += diku::format_to_str(" (%s)", date);
    }

    send_to_char(msg + ".<br/>", ch);
}
