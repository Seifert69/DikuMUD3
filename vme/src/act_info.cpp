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
#include "nanny.h"
#include "pcsave.h"
#include "textutil.h"
#include "utility.h"
#include "utils.h"
#include "weather.h"
#include "when_cache.h"
#include "when_limit.h"

#include <chrono>
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

static constexpr std::chrono::seconds WHEN_LOOKUP_WAIT{5}; ///< Between one mortal's lookups that miss the cache

/**
 * Who asks "when", for what they may see and for the limit alike: an immortal
 * switched into a monster asks as themselves, and a monster that follows a player
 * (ordered to, or made to by a script) as that player. Anyone else is a monster,
 * and may not ask.
 */
static unit_data *when_asker(unit_data *ch)
{
    unit_data *who = CHAR_ORIGINAL(ch);
    if (who->isNPC() && CHAR_MASTER(who) && CHAR_MASTER(who)->isPC())
    {
        who = CHAR_MASTER(who);
    }
    return who;
}

/**
 * Whether "when" hides the player in entry from ch. The same rule for players in
 * the game and players read from their file, so the answer doesn't change when
 * they log on or off: wizard invisibility above ch's level, or invisibility that
 * ch can't detect. Unlike CHAR_CAN_SEE() it ignores light, blindness and where
 * they are. ch asks as when_asker() says, as for the limit.
 */
static bool when_hides(unit_data *ch, const cWhenEntry &entry)
{
    const unit_data *asker = when_asker(ch);

    if (CHAR_LEVEL(asker) < entry.wizinv)
    {
        return true;
    }
    return CHAR_LEVEL(asker) < CREATOR_LEVEL && entry.invisible && !CHAR_HAS_FLAG(asker, CHAR_DETECT_INVISIBLE);
}

/**
 * Whether ch may make a "when" lookup that misses the cache (a file read, or a
 * name with no file), and if so counts it. A mortal waits WHEN_LOOKUP_WAIT
 * between them, kept per player id so that reconnecting doesn't start over.
 * There is no limit for all players together: each player file is read at most
 * once per boot, and a name with no file reads nothing.
 * Immortals aren't limited, also when switched into a monster.
 */
static bool when_lookup_allowed(unit_data *ch)
{
    static cWhenLimit limit(WHEN_LOOKUP_WAIT);

    unit_data *who = when_asker(ch);
    if (CHAR_LEVEL(who) >= IMMORTAL_LEVEL)
    {
        return true;
    }

    // Players without an id yet (-1) share one wait
    long key = PC_ID(who);

    // A monotonic clock with sub-second resolution: whole seconds from time() made
    // the wait anything between 4 and 5 seconds
    return limit.Allow(key, cWhenLimit::clock::now());
}

/**
 * when <player>: when a player last logged on. Only players ask (see
 * when_asker()). Players in the game, link dead included, are "playing right now".
 * Players hidden by when_hides() are treated as unknown.
 *
 * Other players come from when_cache(): a player file is read the first time
 * someone asks about it, at most once per boot, and the server's saves and
 * deletions keep the entry up to date (a file changed by hand is not seen until
 * that player is saved again, or the next boot). A hidden player in the game is
 * looked up the same way, so the limit and the cache act the same whether they
 * are here or not.
 */
void do_when(unit_data *ch, char *arg, const command_info *cmd)
{
    static const char *no_one = "No one by that name here.<br/>";
    char name[MAX_INPUT_LENGTH];

    if (!when_asker(ch)->isPC())
    {
        return;
    }

    if (str_is_empty(arg))
    {
        send_to_char("Usage: when &lt;player name&gt;<br/>", ch);
        return;
    }

    one_argument(arg, name);

    // Players are at the head of the unit list, as in "who"
    bool hidden_here = false;
    for (unit_data *u = g_unit_list; u && u->isPC(); u = u->getGlobalNext())
    {
        if (!str_ccmp(PC_FILENAME(u), name))
        {
            // Not the file: it may still hold the visibility from before they went invisible
            const char *here = u->getNames().Name();
            if (!str_is_empty(here) && (when_asker(ch) == u || !when_hides(ch, when_entry_of(u))))
            {
                send_to_char(diku::format_to_str("%s is playing right now.<br/>", here), ch);
                return;
            }
            hidden_here = true;
            break;
        }
    }

    const cWhenEntry *entry = when_cache().Find(name);
    if (entry == nullptr)
    {
        // The name becomes part of the player file path, so the login prompt's rules
        // for a player name (3 to 14 letters, nothing it bans). Only here, before a file
        // read: they read the ban files.
        char file_name[MAX_INPUT_LENGTH];
        if (_parse_name(name, file_name))
        {
            send_to_char(no_one, ch);
            return;
        }

        // Reading a player file is the expensive part, so it is limited for mortals (as
        // "save" limits saves)
        if (!when_lookup_allowed(ch))
        {
            send_to_char("Wait a few seconds before asking about another player who is not here.<br/>", ch);
            return;
        }

        // Only the player file is read (not the inventory), and it is never saved. A
        // file that load_player() rejects (another player's name in it) is remembered
        // as no one. A name with no file isn't remembered, so made-up names don't fill
        // the cache.
        unit_data *pc = load_player(file_name);
        if (pc)
        {
            when_cache().Store(file_name, when_entry_of(pc));
            extract_unit(pc);
        }
        else if (player_exists(file_name))
        {
            when_cache().Store(file_name, cWhenEntry{});
        }
        entry = when_cache().Find(file_name);
    }

    if (hidden_here || entry == nullptr || !entry->found || when_hides(ch, *entry))
    {
        send_to_char(no_one, ch);
        return;
    }

    std::string msg = diku::format_to_str("%s last logged on %s", entry->name, time_ago_string(entry->last, time(nullptr)));

    char date[64];
    const tm *local = localtime(&entry->last);
    if (local && strftime(date, sizeof(date), "%a %d %b %Y %H:%M", local))
    {
        msg += diku::format_to_str(" (%s)", date);
    }

    send_to_char(msg + ".<br/>", ch);
}
