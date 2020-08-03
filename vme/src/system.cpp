/*
 $Author: All $
 $RCSfile: system.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.8 $
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/resource.h>

#include <netinet/tcp.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "structs.h"
#include "utils.h"
#include "system.h"
#include "db.h"
#include "utility.h"
#include "comm.h"
#include "textutil.h"
#include "ban.h"
#include "handler.h"
#include "files.h"
#include "protocol.h"
#include "main.h"
#include "account.h"
#include "vmelimits.h"
#include "db_file.h"
#include "str_parse.h"
#include "common.h"
#include "dilrun.h"
#include "hookmud.h"


/* extern vars */
extern class descriptor_data *descriptor_list;
extern class descriptor_data *next_to_process;
extern int possible_saves;

void MplexSendSetup(class descriptor_data *d)
{
    assert(d);

    protocol_send_setup(d->multi, d->id, &PC_SETUP(d->character));
}

/* ----------------------------------------------------------------- */
/*                                                                   */
/*                      Descriptor Routines                          */
/*                                                                   */
/* ----------------------------------------------------------------- */

/* Call only once when creating a new char (guest)    */
void init_char(class unit_data *ch)
{
    int new_player_id(void);

    int i;

    extern sbit32 player_id;

    int required_xp(int level);

    if (g_cServerConfig.m_bBBS)
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

    PC_TIME(ch).connect = PC_TIME(ch).birth = PC_TIME(ch).creation =
        time(0);
    PC_TIME(ch).played = 0;
    PC_LIFESPAN(ch) = 100;

    CHAR_EXP(ch) = 0;
    CHAR_LEVEL(ch) = 0;
    PC_ID(ch) = -1;
    PC_CRIMES(ch) = 0;

    PC_ABILITY_POINTS(ch) = 0;
    PC_SKILL_POINTS(ch) = 0;

    /* *** if this is our first player --- he be God *** */
    if (player_id == -7)
    {
        player_id = 1;
        PC_ID(ch) = new_player_id();
    }

    CHAR_ATTACK_TYPE(ch) = WPN_FIST;
    CHAR_NATURAL_ARMOUR(ch) = ARM_CLOTHES;

    UNIT_HIT(ch) = UNIT_MAX_HIT(ch) = 1;

    CHAR_MANA(ch) = mana_limit(ch);
    CHAR_ENDURANCE(ch) = move_limit(ch);
    CHAR_LAST_ROOM(ch) = NULL;

    CHAR_FLAGS(ch) = 0;
    SET_BIT(CHAR_FLAGS(ch), CHAR_PROTECTED);

    for (i = 0; i < 3; i++)
        PC_COND(ch, i) = (CHAR_LEVEL(ch) >= 200 ? 48 : 24);

    PC_COND(ch, DRUNK) = 0;

    set_title(ch);
}

int no_connections = 0;     /* No of used descriptors                    */
int max_no_connections = 0; /* Statistics                                */

descriptor_data::descriptor_data(cMultiHook *pe)
{
    static int nid = 0;

    void nanny_get_name(class descriptor_data * d, char *arg);

    no_connections++;
    if (no_connections > max_no_connections)
        max_no_connections = no_connections;

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

    localstr = NULL;
    postedit = NULL;
    editing = NULL;
    editref = NULL;

    original = 0;
    snoop.snooping = 0;
    snoop.snoop_by = 0;
    replyid = (ubit32)-1;

    /* Make a new PC struct */
    character = new EMPLACE(unit_data) unit_data(UNIT_ST_PC);
    init_char(character);
    CHAR_DESCRIPTOR(character) = this;

    /* prepend to list */
    next = descriptor_list;
    descriptor_list = this;
}

void descriptor_data::RemoveBBS(void)
{
    if (nLine != 255)
    {
        char buf[512];

        sprintf(buf, BBS_DIR "%d.%d", nPort, nLine);
        remove(buf);
    }
}

void descriptor_data::CreateBBS(void)
{
    if (nLine != 255)
    {
        char buf[512];
        FILE *f;

        sprintf(buf, BBS_DIR "%d.%d", nPort, nLine);

        if (!character)
        {
            slog(LOG_ALL, 0, "No character in %s.", buf);
            return;
        }

        f = fopen(buf, "wb");

        if (!f)
        {
            slog(LOG_ALL, 0, "Could not create %s.", buf);
            return;
        }

        if (account_is_overdue(this->character))
            fprintf(f, "1\n");
        else
            fprintf(f, "0\n");

        fprintf(f, "%d\n", PC_ACCOUNT(this->character).total_credit);
        fprintf(f, "%s\n", PC_FILENAME(this->character));
        fprintf(f, "%d\n", CHAR_LEVEL(this->character));
        fprintf(f, "%d\n", PC_ACCESS_LEVEL(this->character));

        fclose(f);
    }
}

descriptor_data::~descriptor_data(void)
{
    RemoveBBS();
    nLine = 255;
}

/* Pass the multi-fd which is to be associated with this new descriptor */
/* Note that id zero signifies that mplex descriptor has no mplex'er    */
class descriptor_data *descriptor_new(class cMultiHook *pe)
{
    return new descriptor_data(pe);
}

/* Flush should be set to true, when there is noone to receive the  */
/* data, i.e. when for example the mplex'er is down or if the       */
/* mplex'er has sent a terminate request to the server. Nothing     */
/* bad happens if you mess up, but the mplex'er receives data to    */
/* an unknown id...                                                 */
void descriptor_close(class descriptor_data *d, int bSendClose, int bReconnect)
{
    class descriptor_data *tmp;
    struct diltemplate *link_dead;
    void unsnoop(class unit_data * ch, int mode);
    void unswitchbody(class unit_data * npc);

    assert(d->character);

    /* Descriptor must be either in the game (UNIT_IN) or in menu.  */
    /* If unit has been extracted, then all his data is half erased */
    /* (affects, etc) and he shall not be saved!                    */
    if (!UNIT_IN(d->character)) /* In menu - extract completely */
    {
        assert(!UNIT_IN(d->character));
        assert(!d->character->gnext);
        assert(!d->character->gprevious);

        /* Important that we set to NULL before calling extract,
           otherwise we just go to the menu... ... ... */
        if (PC_IS_UNSAVED(d->character))
            possible_saves--;
        CHAR_DESCRIPTOR(d->character) = NULL;
        extract_unit(d->character);
        d->character = NULL;
        /* Too much log slog(LOG_ALL, "Losing descriptor from menu."); */
    }
    else
    {
        if (d->localstr)
            FREE(d->localstr);

        d->localstr = NULL;
        d->postedit = NULL;
        d->editing = NULL;
        d->editref = NULL;

        if (CHAR_IS_SNOOPING(d->character) || CHAR_IS_SNOOPED(d->character))
            unsnoop(d->character, 1);

        if (CHAR_IS_SWITCHED(d->character))
            unswitchbody(d->character);

        assert(!d->snoop.snooping && !d->snoop.snoop_by);
        assert(!d->original);

        act("$1n has lost $1s link.", A_HIDEINV, d->character, 0, 0, TO_ROOM);
        slog(LOG_BRIEF, UNIT_MINV(d->character),
             "Closing link and making link dead: %s.",
             UNIT_NAME(d->character));

        if (!d->character->is_destructed())
        {
            void disconnect_game(class unit_data * pc);

            disconnect_game(d->character);
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
                        CHAR_DESCRIPTOR(d->character) = NULL;
                        class dilprg *prg =
                            dil_copy_template(link_dead, d->character, NULL);
                        if (prg)
                        {
                            prg->waitcmd = WAITCMD_MAXINST - 1;
                            dil_activate(prg);
                        }
                    }
                }
                else
                {
                    extract_unit(d->character); /* We extract guests */
                    possible_saves--;
                }
            }
        }
        /* Important we set tp null AFTER calling save - otherwise
           time played does not get updated. */
        CHAR_DESCRIPTOR(d->character) = NULL;
        d->character = NULL;
    }

    if (bSendClose && d->multi->IsHooked())
        protocol_send_close(d->multi, d->id);

    no_connections--;

    if (next_to_process == d) /* to avoid crashing the process loop */
        next_to_process = next_to_process->next;

    if (d == descriptor_list) /* this is the head of the list */
        descriptor_list = descriptor_list->next;
    else /* This is somewhere inside the list */
    {
        /* Locate the previous element */
        for (tmp = descriptor_list; tmp && (tmp->next != d); tmp = tmp->next)
            ;
        tmp->next = d->next;
    }

    delete d;
}

/* Returns how much memory is in use by system. This may help spotting
   possible fragmentation of memory. */
void system_memory(class unit_data *ch)
{
#ifdef LINUX
    struct rusage rusage_data;
    int n;
    char Buf[1024];

    n = getrusage(RUSAGE_CHILDREN, &rusage_data);

    if (n != 0)
        slog(LOG_ALL, 0, "System memory status error.");
    else
    {
        sprintf(Buf,
                "Vol. Switches       %8ld<br/>"
                "Max RSS             %8ld<br/>"
                "Shared memory size  %8ld<br/>"
                "Unshared data size  %8ld<br/>"
                "Unshared stack size %8ld<br/><br/>",
                rusage_data.ru_nvcsw,
                rusage_data.ru_maxrss,
                rusage_data.ru_ixrss,
                rusage_data.ru_isrss, rusage_data.ru_idrss);
        send_to_char(Buf, ch);
    }
#endif
}
