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

    protocol_send_setup(d->getMultiHookPtr(), d->getMultiHookID(), &PC_SETUP(d->character));
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

    CHAR_POS(ch) = POSITION_STANDING;
    CHAR_SPEED(ch) = SPEED_DEFAULT;
    CHAR_RACE(ch) = RACE_HUMAN;
    CHAR_SEX(ch) = SEX_MALE;

    PC_TIME(ch).connect = PC_TIME(ch).birth = PC_TIME(ch).creation = time(nullptr);
    PC_TIME(ch).played = 0;
    PC_LIFESPAN(ch) = 100;

    CHAR_EXP(ch) = 0;
    CHAR_LEVEL(ch) = 0;
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

    CHAR_ATTACK_TYPE(ch) = WPN_FIST;
    CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;

    UNIT_HIT(ch) = UNIT_MAX_HIT(ch) = 1;

    CHAR_MANA(ch) = mana_limit(ch);
    CHAR_ENDURANCE(ch) = move_limit(ch);
    CHAR_LAST_ROOM(ch) = nullptr;

    CHAR_FLAGS(ch) = 0;
    SET_BIT(CHAR_FLAGS(ch), CHAR_PROTECTED);

    for (i = 0; i < 3; i++)
    {
        PC_COND(ch, i) = (CHAR_LEVEL(ch) >= 200 ? 48 : 24);
    }

    PC_COND(ch, DRUNK) = 0;

    set_title(ch);
}

int g_no_connections = 0;     /* No of used descriptors                    */
int g_max_no_connections = 0; /* Statistics                                */

descriptor_data::descriptor_data(cMultiHook *pe)
{
    static int nid = 0;

    g_no_connections++;
    if (g_no_connections > g_max_no_connections)
    {
        g_max_no_connections = g_no_connections;
    }

    /* init desc data */
    multi = pe;
    id = ++nid;

    state = 0;
    fptr = nanny_get_name;
    wait = 1;
    timer = 0;
    prompt_mode = PROMPT_SENT;
    *last_cmd = *history = '\0';

    host[0] = 0;
    nPort = 0;
    nLine = 255;

    localstr = nullptr;
    postedit = nullptr;
    editing = nullptr;
    editref = nullptr;

    original = nullptr;
    snoop.setSnooping(nullptr);
    snoop.setSnoopBy(nullptr);
    replyid = (ubit32)-1;

    /* Make a new PC struct */
    character = new EMPLACE(pc_data) pc_data;
    init_char(character);
    CHAR_DESCRIPTOR(character) = this;

    /* prepend to list */
    next = g_descriptor_list;
    g_descriptor_list = this;
}

void descriptor_data::RemoveBBS()
{
    if (nLine != 255)
    {
        auto filename = diku::format_to_str("%s%d.%d", BBS_DIR, nPort, nLine);
        remove(filename.c_str());
    }
}

void descriptor_data::CreateBBS()
{
    if (nLine != 255)
    {
        auto filename = diku::format_to_str("%s%d.%d", BBS_DIR, nPort, nLine);

        if (!character)
        {
            slog(LOG_ALL, 0, "No character in %s.", filename);
            return;
        }

        FILE *f = fopen(filename.c_str(), "wb");

        if (!f)
        {
            slog(LOG_ALL, 0, "Could not create %s.", filename);
            return;
        }

        if (account_is_overdue(this->character))
        {
            fprintf(f, "1\n");
        }
        else
        {
            fprintf(f, "0\n");
        }

        fprintf(f, "%d\n", PC_ACCOUNT(this->character).total_credit);
        fprintf(f, "%s\n", PC_FILENAME(this->character));
        fprintf(f, "%d\n", CHAR_LEVEL(this->character));
        fprintf(f, "%d\n", PC_ACCESS_LEVEL(this->character));

        fclose(f);
    }
}

descriptor_data::~descriptor_data()
{
    RemoveBBS();
    nLine = 255;
}

time_t descriptor_data::getLastLogonTime() const
{
    return logon;
}

void descriptor_data::setLastLogonTime(time_t value)
{
    logon = value;
}

cMultiHook *descriptor_data::getMultiHookPtr()
{
    return multi;
}

ubit16 descriptor_data::getMultiHookID() const
{
    return id;
}

const descriptor_data::FunctionPtr descriptor_data::getFunctionPtr() const
{
    return fptr;
}

void descriptor_data::callFunctionPtr(descriptor_data *data, char *arg) const
{
    fptr(data, arg);
}

void descriptor_data::setFunctionPtr(descriptor_data::FunctionPtr value)
{
    fptr = value;
}

int descriptor_data::getState() const
{
    return state;
}

void descriptor_data::setState(int value)
{
    state = value;
}
int descriptor_data::postincrementState()
{
    return state++;
}

const char *descriptor_data::getHostname() const
{
    return host;
}

void descriptor_data::setHostname(const char *value)
{
    strncpy(host, value, sizeof(host));
    host[sizeof(host) - 1] = 0;
}

ubit16 descriptor_data::getMplexPortNum() const
{
    return nPort;
}

void descriptor_data::setMplexPortNum(ubit16 value)
{
    nPort = value;
}

ubit8 descriptor_data::getSerialLine() const
{
    return nLine;
}

void descriptor_data::setSerialLine(ubit8 value)
{
    nLine = value;
}

int descriptor_data::predecrementLoopWaitCounter()
{
    return --wait;
}

void descriptor_data::setLoopWaitCounter(int value)
{
    wait = value;
}

ubit16 descriptor_data::getHoursPlayerIdle() const
{
    return timer;
}

void descriptor_data::setHoursPlayerIdle(ubit16 value)
{
    timer = value;
}

void descriptor_data::incrementHoursPlayerIdle()
{
    ++timer;
}

void descriptor_data::clearLocalString()
{
    FREE(localstr);
    localstr = nullptr;
}

const char *descriptor_data::getLocalString() const
{
    return localstr;
}

void descriptor_data::setLocalString(const char *str)
{
    if (localstr == nullptr)
    {
        CREATE(localstr, char, strlen(str) + 8);
        strcpy(localstr, str);
    }
    else
    {
        RECREATE(localstr, char, strlen(localstr) + strlen(str) + 8);
        strcat(localstr, str);
    }
}

void descriptor_data::appendLocalString(const char *str)
{
    strcat(localstr, "<br/>");
}

bool descriptor_data::hasPostEditFunctionPtr() const
{
    return postedit != nullptr;
}

void descriptor_data::callPostEditFunctionPtr(descriptor_data *data) const
{
    postedit(data);
}

void descriptor_data::setPostEditFunctionPtr(PostEditFunctionPtr value)
{
    postedit = value;
}

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
    assert(d->character);

    /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
    /* If unit has been extracted, then all his data is half erased */
    /* (affects, etc) and he shall not be saved!                    */
    if (!char_is_playing(d->character)) /* In menu - extract completely */
    {
        assert(!UNIT_IN(d->character));
        assert(!d->character->gnext);
        assert(!d->character->gprevious);

        /* Important that we set to NULL before calling extract,
           otherwise we just go to the menu... ... ... */
        if (PC_IS_UNSAVED(d->character))
        {
            g_possible_saves--;
        }
        CHAR_DESCRIPTOR(d->character) = nullptr;
        extract_unit(d->character);
        d->character = nullptr;
        /* Too much log slog(LOG_ALL, "Losing descriptor from menu."); */
    }
    else
    {
        d->clearLocalString();
        d->setPostEditFunctionPtr(nullptr);
        d->editing = nullptr;
        d->editref = nullptr;

        // Here we don't stop_fightfollow - do we ?
        stop_snoopwrite(d->character);

        // if (CHAR_IS_SNOOPING(d->character) || CHAR_IS_SNOOPED(d->character))
        //    unsnoop(d->character, 1);

        // if (CHAR_IS_SWITCHED(d->character))
        //    unswitchbody(d->character);

        assert(!d->snoop.getSnooping() && !d->snoop.getSnoopBy());
        assert(!d->original);

        act("$1n has lost $1s link.", A_HIDEINV, d->character, cActParameter(), cActParameter(), TO_ROOM);
        slog(LOG_BRIEF, UNIT_MINV(d->character), "Closing link and making link dead: %s.", UNIT_NAME(d->character));

        if (!d->character->is_destructed())
        {
            if (IS_PC(d->character))
            {
                UPC(d->character)->disconnect_game();
            }
            if (!bReconnect)
            {
                if (!PC_IS_UNSAVED(d->character))
                {
                    /* We need to save player to update his time status! */
                    save_player(d->character); /* Save non-guests */
                    save_player_contents(d->character, TRUE);
                    link_dead = find_dil_template("link_dead@basis");
                    if (link_dead)
                    {
                        CHAR_DESCRIPTOR(d->character) = nullptr;
                        dilprg *prg = dil_copy_template(link_dead, d->character, nullptr);
                        if (prg)
                        {
                            prg->waitcmd = WAITCMD_MAXINST - 1;
                            dil_activate(prg);
                        }
                    }
                }
                else
                {
                    CHAR_DESCRIPTOR(d->character) = nullptr; // Prevent counting down players, we did above
                    extract_unit(d->character);              /* We extract guests */
                    g_possible_saves--;
                }
            }
        }
        /* Important we set tp null AFTER calling save - otherwise
           time played does not get updated. */
        CHAR_DESCRIPTOR(d->character) = nullptr;
        d->character = nullptr;
    }

    if (bSendClose && d->getMultiHookPtr()->IsHooked())
    {
        protocol_send_close(d->getMultiHookPtr(), d->getMultiHookID());
    }

    g_no_connections--;

    if (g_next_to_process == d)
    { /* to avoid crashing the process loop */
        g_next_to_process = g_next_to_process->next;
    }

    if (d == g_descriptor_list)
    { /* this is the head of the list */
        g_descriptor_list = g_descriptor_list->next;
    }
    else /* This is somewhere inside the list */
    {
        /* Locate the previous element */
        for (tmp = g_descriptor_list; tmp && (tmp->next != d); tmp = tmp->next)
        {
            ;
        }
        tmp->next = d->next;
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
