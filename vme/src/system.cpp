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
#include "utils.h"
#include "vmelimits.h"

#include <sys/resource.h>
#include <sys/un.h>

#include <cstdio>
#include <cstdlib>

void MplexSendSetup(descriptor_data *d)
{
    assert(d);

    protocol_send_setup(d->getMultiHookPtr(), d->getMultiHookID(), &PC_SETUP(d->getCharacter()));
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
        UPC(ch)->getTerminalSetupType().echo = TRUE;
        UPC(ch)->getTerminalSetupType().redraw = TRUE;
        UPC(ch)->getTerminalSetupType().emulation = TERM_ANSI;
        UPC(ch)->getTerminalSetupType().telnet = FALSE;

        UPC(ch)->setPCFlag(PC_INFORM);
    }
    else
    {
        UPC(ch)->getTerminalSetupType().echo = FALSE;
        UPC(ch)->getTerminalSetupType().redraw = FALSE;
        UPC(ch)->getTerminalSetupType().emulation = TERM_TTY;
        UPC(ch)->getTerminalSetupType().telnet = TRUE;
    }

    UPC(ch)->getTerminalSetupType().width = 80;
    UPC(ch)->getTerminalSetupType().height = 15;
    UPC(ch)->getTerminalSetupType().colour_convert = 0;
    UPC(ch)->setNumberOfCrackAttempts(0);

    account_defaults(ch);

    UCHAR(ch)->setPosition(POSITION_STANDING);
    UCHAR(ch)->setSpeed(SPEED_DEFAULT);
    UCHAR(ch)->setRace(RACE_HUMAN);
    UCHAR(ch)->setSex(SEX_MALE);

    const auto now = time(nullptr);
    PC_TIME(ch).setPlayerLastConnectTime(now);
    PC_TIME(ch).setPlayerBirthday(now);
    PC_TIME(ch).setPlayerCharacterCreationTime(now);
    PC_TIME(ch).setTotalTimePlayedInSeconds(0);
    UPC(ch)->setLifespan(100);

    UCHAR(ch)->setPlayerExperience(0);
    UCHAR(ch)->setLevel(0);
    UPC(ch)->setPlayerUID(-1);
    UPC(ch)->setNumberOfCrimesCommitted(0);

    UPC(ch)->setAbilityPoints(0);
    UPC(ch)->setSkillPoints(0);

    /* *** if this is our first player --- he be God *** */
    if (g_player_id == -7)
    {
        g_player_id = 1;
        UPC(ch)->setPlayerUID(new_player_id());
    }

    UCHAR(ch)->setAttackType(WPN_FIST);
    UCHAR(ch)->setNaturalArmor(ARM_CLOTHES);

    ch->setCurrentHitpoints(1);
    ch->setMaximumHitpoints(1);

    UCHAR(ch)->setMana(mana_limit(ch));
    UCHAR(ch)->setEndurance(move_limit(ch));
    UCHAR(ch)->setLastLocation(nullptr);

    UCHAR(ch)->setAllCharacterFlags(0);
    UCHAR(ch)->setCharacterFlag(CHAR_PROTECTED);

    for (i = 0; i < 3; i++)
    {
        UPC(ch)->setConditionAtIndexTo(i, (CHAR_LEVEL(ch) >= 200 ? 48 : 24));
    }

    UPC(ch)->setConditionAtIndexTo(DRUNK, 0);

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
    assert(d->cgetCharacter());

    /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
    /* If unit has been extracted, then all his data is half erased */
    /* (affects, etc) and he shall not be saved!                    */
    if (!char_is_playing(d->getCharacter())) /* In menu - extract completely */
    {
        assert(!d->cgetCharacter()->getUnitIn());
        assert(!d->cgetCharacter()->getGlobalNext());
        assert(!d->cgetCharacter()->getGlobalPrevious());

        /* Important that we set to NULL before calling extract,
           otherwise we just go to the menu... ... ... */
        if (PC_IS_UNSAVED(d->getCharacter()))
        {
            g_possible_saves--;
        }
        UCHAR(d->getCharacter())->setDescriptor(nullptr);
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
        slog(LOG_BRIEF,
             d->cgetCharacter()->getLevelOfWizardInvisibility(),
             "Closing link and making link dead: %s.",
             d->cgetCharacter()->getNames().Name());

        if (!d->getCharacter()->is_destructed())
        {
            if (d->cgetCharacter()->isPC())
            {
                UPC(d->cgetCharacter())->disconnect_game();
            }
            if (!bReconnect)
            {
                if (!PC_IS_UNSAVED(d->getCharacter()))
                {
                    /* We need to save player to update his time status! */
                    save_player(d->getCharacter()); /* Save non-guests */
                    save_player_contents(d->getCharacter(), TRUE);

                    UCHAR(d->cgetCharacter())->setDescriptor(nullptr);
                    dilprg *prg = dil_copy_template(g_dil_link_dead, d->getCharacter(), nullptr);
                    if (prg)
                    {
                        prg->waitcmd = WAITCMD_MAXINST - 1;
                        dil_activate(prg);
                    }
                }
                else
                {
                    UCHAR(d->cgetCharacter())->setDescriptor(nullptr); // Prevent counting down players, we did above
                    extract_unit(d->getCharacter());                   /* We extract guests */
                    g_possible_saves--;
                }
            }
        }
        /* Important we set tp null AFTER calling save - otherwise
           time played does not get updated. */
        UCHAR(d->cgetCharacter())->setDescriptor(nullptr);
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
