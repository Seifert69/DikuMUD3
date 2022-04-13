/*
 $Author: All $
 $RCSfile: system.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.8 $
 */
#include "system.h"

#include "account.h"
#include "comm.h"
#include "db.h"
#include "dilrun.h"
#include "formatter.h"
#include "handler.h"
#include "hookmud.h"
#include "interpreter.h"
#include "main_functions.h"
#include "nanny.h"
#include "pcsave.h"
#include "protocol.h"
#include "slog.h"
#include "structs.h"
#include "utils.h"
#include "vmelimits.h"

#include <sys/resource.h>
#include <sys/un.h>

#include <cstdio>
#include <cstdlib>

void MplexSendSetup(descriptor_data *d)
{
    assert(d);

    protocol_send_setup(d->getMultiHookPtr(), d->getMultiHookID(), &PC_SETUP(d->cgetCharacter()));
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                      Descriptor Routines                          */
/*                                                                   */
/* ----------------------------------------------------------------- */

/* Call only once when creating a new char (guest)    */
void init_char(unit_data *ch)
{
    int i = 0;

    if (g_cServerConfig.isBBS())
    {
        PC_SETUP_ECHO(ch) = TRUE;
        PC_SETUP_REDRAW(ch) = TRUE;
        PC_SETUP_EMULATION(ch) = TERM_ANSI;
        PC_SETUP_TELNET(ch) = FALSE;

        SET_BIT(PC_FLAGS(ch), PC_INFORM);
    }
    else
    {
        PC_SETUP_ECHO(ch) = FALSE;
        PC_SETUP_REDRAW(ch) = FALSE;
        PC_SETUP_EMULATION(ch) = TERM_TTY;
        PC_SETUP_TELNET(ch) = TRUE;
    }

    PC_SETUP_WIDTH(ch) = 80;
    PC_SETUP_HEIGHT(ch) = 15;
    PC_SETUP_COLOUR(ch) = 0;
    PC_CRACK_ATTEMPTS(ch) = 0;

    account_defaults(ch);

    getCharPoints(ch).setPosition(POSITION_STANDING);
    getCharPoints(ch).setSpeed(SPEED_DEFAULT);
    getCharPoints(ch).setRace(RACE_HUMAN);
    getCharPoints(ch).setSex(SEX_MALE);

    const auto now = time(nullptr);
    PC_TIME(ch).setPlayerLastConnectTime(now);
    PC_TIME(ch).setPlayerBirthday(now);
    PC_TIME(ch).setPlayerCharacterCreationTime(now);
    PC_TIME(ch).setTotalTimePlayedInSeconds(0);
    PC_LIFESPAN(ch) = 100;

    getCharPoints(ch).setPlayerExperience(0);
    getCharPoints(ch).setLevel(0);
    PC_ID(ch) = -1;
    PC_CRIMES(ch) = 0;

    PC_ABILITY_POINTS(ch) = 0;
    PC_SKILL_POINTS(ch) = 0;

    /* *** if this is our first player --- he be God *** */
    if (g_player_id == -7)
    {
        g_player_id = 1;
        PC_ID(ch) = new_player_id();
    }

    getCharPoints(ch).setAttackType(WPN_FIST);
    getCharPoints(ch).setNaturalArmor(ARM_CLOTHES);

    UNIT_HIT(ch) = 1;
    ch->setMaximumHitpoints(1);

    getCharPoints(ch).setMana(mana_limit(ch));
    getCharPoints(ch).setEndurance(move_limit(ch));
    CHAR_LAST_ROOM(ch) = nullptr;

    getCharPoints(ch).setAllCharacterFlags(0);
    getCharPoints(ch).setCharacterFlag(CHAR_PROTECTED);

    for (i = 0; i < 3; i++)
    {
        PC_COND(ch, i) = (CHAR_LEVEL(ch) >= 200 ? 48 : 24);
    }

    PC_COND(ch, DRUNK) = 0;

    set_title(ch);
}

int g_no_connections = 0;     /* No of used descriptors                    */
int g_max_no_connections = 0; /* Statistics                                */

/* Pass the multi-fd which is to be associated with this new descriptor */
/* Note that id zero signifies that mplex descriptor has no mplex'er    */
descriptor_data *descriptor_new(cMultiHook *pe)
{
    return new descriptor_data(pe);
}

/* Flush should be set to true, when there is noone to receive the  */
/* data, i.e. when for example the mplex'er is down or if the       */
/* mplex'er has sent a terminate request to the server. Nothing     */
/* bad happens if you mess up, but the mplex'er receives data to    */
/* an unknown id...                                                 */
void descriptor_close(descriptor_data *d, int bSendClose, int bReconnect)
{
    descriptor_data *tmp = nullptr;
    diltemplate *link_dead = nullptr;
    assert(d->cgetCharacter());

    /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
    /* If unit has been extracted, then all his data is half erased */
    /* (affects, etc) and he shall not be saved!                    */
    if (!char_is_playing(d->getCharacter())) /* In menu - extract completely */
    {
        assert(!UNIT_IN(d->cgetCharacter()));
        assert(!d->cgetCharacter()->gnext);
        assert(!d->cgetCharacter()->gprevious);

        /* Important that we set to NULL before calling extract,
           otherwise we just go to the menu... ... ... */
        if (PC_IS_UNSAVED(d->cgetCharacter()))
        {
            g_possible_saves--;
        }
        CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr;
        extract_unit(d->getCharacter());
        d->setCharacter(nullptr);
        /* Too much log slog(LOG_ALL, "Losing descriptor from menu."); */
    }
    else
    {
        d->clearLocalString();
        d->setPostEditFunctionPtr(nullptr);
        d->setEditing(nullptr);
        d->setEditReference(nullptr);

        // Here we don't stop_fightfollow - do we ?
        stop_snoopwrite(d->getCharacter());

        // if (CHAR_IS_SNOOPING(d->character) || CHAR_IS_SNOOPED(d->character))
        //    unsnoop(d->character, 1);

        // if (CHAR_IS_SWITCHED(d->character))
        //    unswitchbody(d->character);

        assert(!d->cgetSnoopData().getSnooping() && !d->cgetSnoopData().getSnoopBy());
        assert(!d->cgetOriginalCharacter());

        act("$1n has lost $1s link.", A_HIDEINV, d->cgetCharacter(), cActParameter(), cActParameter(), TO_ROOM);
        slog(LOG_BRIEF, UNIT_MINV(d->cgetCharacter()), "Closing link and making link dead: %s.", UNIT_NAME(d->cgetCharacter()));

        if (!d->getCharacter()->is_destructed())
        {
            if (IS_PC(d->cgetCharacter()))
            {
                UPC(d->cgetCharacter())->disconnect_game();
            }
            if (!bReconnect)
            {
                if (!PC_IS_UNSAVED(d->cgetCharacter()))
                {
                    /* We need to save player to update his time status! */
                    save_player(d->getCharacter()); /* Save non-guests */
                    save_player_contents(d->getCharacter(), TRUE);
                    link_dead = find_dil_template("link_dead@basis");
                    if (link_dead)
                    {
                        CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr;
                        dilprg *prg = dil_copy_template(link_dead, d->getCharacter(), nullptr);
                        if (prg)
                        {
                            prg->waitcmd = WAITCMD_MAXINST - 1;
                            dil_activate(prg);
                        }
                    }
                }
                else
                {
                    CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr; // Prevent counting down players, we did above
                    extract_unit(d->getCharacter());               /* We extract guests */
                    g_possible_saves--;
                }
            }
        }
        /* Important we set tp null AFTER calling save - otherwise
           time played does not get updated. */
        CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr;
        d->setCharacter(nullptr);
    }

    if (bSendClose && d->getMultiHookPtr()->IsHooked())
    {
        protocol_send_close(d->getMultiHookPtr(), d->getMultiHookID());
    }

    g_no_connections--;

    if (g_next_to_process == d)
    { /* to avoid crashing the process loop */
        g_next_to_process = g_next_to_process->getNext();
    }

    if (d == g_descriptor_list)
    { /* this is the head of the list */
        g_descriptor_list = g_descriptor_list->getNext();
    }
    else /* This is somewhere inside the list */
    {
        /* Locate the previous element */
        for (tmp = g_descriptor_list; tmp && (tmp->getNext() != d); tmp = tmp->getNext())
        {
            ;
        }
        tmp->setNext(d->getNext());
    }

    delete d;
}

/* Returns how much memory is in use by system. This may help spotting
   possible fragmentation of memory. */
void system_memory(unit_data *ch)
{
#ifdef LINUX
    rusage rusage_data;
    int n = 0;

    n = getrusage(RUSAGE_CHILDREN, &rusage_data);

    if (n != 0)
    {
        slog(LOG_ALL, 0, "System memory status error.");
    }
    else
    {
        auto msg = diku::format_to_str("Vol. Switches       %8ld<br/>"
                                       "Max RSS             %8ld<br/>"
                                       "Shared memory size  %8ld<br/>"
                                       "Unshared data size  %8ld<br/>"
                                       "Unshared stack size %8ld<br/><br/>",
                                       rusage_data.ru_nvcsw,
                                       rusage_data.ru_maxrss,
                                       rusage_data.ru_ixrss,
                                       rusage_data.ru_isrss,
                                       rusage_data.ru_idrss);
        send_to_char(msg, ch);
    }
#endif
}
