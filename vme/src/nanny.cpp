/*
 $Author: All $
 $RCSfile: nanny.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.12 $
 */
/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
    #include <unistd.h>
#elif defined LINUX
    #include <crypt.h>
#endif
#ifdef _WINDOWS
    #include <arpa/telnet.h>
#endif

#include "act_other.h"
#include "affect.h"
#include "badnames.h"
#include "ban.h"
#include "comm.h"
#include "constants.h"
#include "db.h"
#include "dilinst.h"
#include "dilrun.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "nanny.h"
#include "pcsave.h"
#include "reception.h"
#include "slog.h"
#include "structs.h"
#include "system.h"
#include "textutil.h"
#include "utils.h"
#include "vmelimits.h"

#include <arpa/inet.h>

#include <cctype>
#include <cstdio>
#include <cstdlib>

int g_dilmenu;

char *m_pBadNames = nullptr;
char *m_pBadStrings = nullptr;

/* *************************************************************************
 *  Stuff for controlling the non-playing sockets (get name, pwd etc)      *
 ************************************************************************* */

int _parse_name(const char *arg, char *name)
{
    int i = 0;

    badnames_list badnames;
    badnames_list badstrings;
    m_pBadNames = nullptr;
    m_pBadStrings = nullptr;

    arg = skip_spaces(arg); /* skip whitespaces */

    /* i+1 = chars copied    */
    /* PC_MAX_NAME-1 = chars */

    for (i = 0; (*name = *arg); arg++, i++, name++)
    {
        if ((*arg <= ' ') || !isalpha(*arg) || (i + 1) > (PC_MAX_NAME - 1))
        {
            return 1;
        }
    }

    if (i <= 2)
    { /* Names must be at least 3 chars due to passwords */
        return 1;
    }

    if (fill_word(name - i))
    { /* Don't allow fillwords */
        return 1;
    }

    touch_file(g_cServerConfig.getFileInEtcDir(BAD_STRINGS_FILE));
    m_pBadStrings = read_info_file(g_cServerConfig.getFileInEtcDir(BAD_STRINGS_FILE), m_pBadStrings);
    badstrings.create(m_pBadStrings);
    FREE(m_pBadStrings);

    if (badstrings.in(name - i))
    {
        return 2;
    }

    touch_file(g_cServerConfig.getFileInEtcDir(BAD_NAMES_FILE));
    m_pBadNames = read_info_file(g_cServerConfig.getFileInEtcDir(BAD_NAMES_FILE), m_pBadNames);
    badnames.create(m_pBadNames);
    FREE(m_pBadNames);

    if (badnames.equal(name - i))
    {
        return 3;
    }

    str_lower(name);

    return 0;
}

/* This is called from main_loop every n seconds (long duration) to check */
/* for idle time for any descriptors                                      */
void check_idle()
{
    descriptor_data *d = nullptr;
    descriptor_data *next_d = nullptr;
    time_t now = time(nullptr);

    for (d = g_descriptor_list; d; d = next_d)
    {
        next_d = d->getNext();
        d->incrementMinutesPlayerIdle();
        if (!descriptor_is_playing(d)) /* Not in game yet */
        {
            if (d->getFunctionPtr() == nanny_get_name)
            {
                if (d->getMinutesPlayerIdle() >= 2)
                {
                    slog(LOG_ALL, 0, "Kicking out idle player waiting for name.");
                    descriptor_close(d);
                }
            }
            else
            {
                if (d->getMinutesPlayerIdle() >= 10)
                {
                    slog(LOG_ALL, 0, "Kicking out player from menu.");
                    descriptor_close(d);
                }
            }
        }
        else
        {
            if (d->getMinutesPlayerIdle() >= 20 && IS_MORTAL(d->cgetCharacter()))
            {
                slog(LOG_ALL, 0, "Kicking out idle player and making link-dead.");
                descriptor_close(d);
            }
            else if (IS_PC(d->cgetCharacter()) && now - d->getLastLogonTime() >= SECS_PER_REAL_HOUR / 3)
            {
                send_to_char("Autosave.<br/>", d->cgetCharacter());
                save_player(d->getCharacter());
                save_player_contents(d->getCharacter(), TRUE); /* No compress */
                /* Only save one player per autosave... */
                return;
            }
        }
    }
}

/* clear some of the the working variables of a char */
void reset_char(unit_data *ch)
{
    /* Ok, this is test to avoid level 255's entering the game... */
    //  assert (CHAR_SEX (ch) != 255);
    //  assert (CHAR_POS (ch) != 255);

    ch->setMaximumHitpoints(hit_limit(ch));
}

void pc_data::connect_game()
{
    //  assert (CHAR_DESCRIPTOR (pc));

    if (this->is_destructed() || !CHAR_DESCRIPTOR(this))
    {
        return;
    }

    m_time.setPlayerLastConnectTime(time(nullptr));
    CHAR_DESCRIPTOR(this)->setLastLogonTime(time(nullptr));

    CHAR_DESCRIPTOR(this)->CreateBBS();

    set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);

    g_no_players++;
    if (g_no_players > g_max_no_players)
    {
        g_max_no_players = g_no_players;
    }
}

void pc_data::disconnect_game()
{
    if (CHAR_DESCRIPTOR(this))
    {
        CHAR_DESCRIPTOR(this)->RemoveBBS();

        g_no_players--;
    }
}

void pc_data::reconnect_game(descriptor_data *d)
{
    // char *color;
    // char tbuf[MAX_STRING_LENGTH * 2];

    if (this->is_destructed() || !d)
    {
        return;
    }

    CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr;
    extract_unit(d->getCharacter()); // Toss out the temporary unit and take over the new one
    d->setCharacter(this);
    CHAR_DESCRIPTOR(this) = d;

    dil_destroy("link_dead@basis", this);

    ActivateDil(this); // ch could potentially get zapped here.

    if (this->is_destructed() || !d)
    {
        return;
    }

    this->connect_game();
    /* MS2020
     color = UPC (ch)->color.save_string ();
     s printf (tbuf, "%s%s%s", CONTROL_COLOR_CREATE, color, CONTROL_COLOR_END);
     send_to_char (tbuf, ch);
     delete color;
 */
    send_to_char("Reconnecting.<br/>", this);

    if (CHAR_LAST_ROOM(this) && (CHAR_LAST_ROOM(this) != UNIT_IN(this)))
    {
        act("$1n has reconnected, and is moved to another location.",
            A_HIDEINV,
            cActParameter(this),
            cActParameter(),
            cActParameter(),
            TO_ROOM);
        unit_from_unit(this);
        unit_to_unit(this, CHAR_LAST_ROOM(this));
        CHAR_LAST_ROOM(this) = nullptr;
    }
    act("$1n has reconnected.", A_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), TO_ROOM);
    slog(LOG_BRIEF, UNIT_MINV(this), "%s[%s] has reconnected.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->getHostname());
    CHAR_DESCRIPTOR(this)->setLastLogonTime(::time(nullptr));
    m_time.setPlayerLastConnectTime(time(nullptr));
    //      stop_affect(ch);
    //      stop_all_special(ch);
    //      start_affect(ch);               /* Activate affect ticks */
    //   start_all_special(ch);     /* Activate fptr ticks   */

    set_descriptor_fptr(d, descriptor_interpreter, FALSE);
}

void update_lasthost(unit_data *pc, ubit32 s_addr)
{
    if ((sbit32)s_addr == -1)
    {
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        if (PC_LASTHOST(pc)[i] == s_addr)
        {
            return;
        }
    }

    memmove(&PC_LASTHOST(pc)[0], &PC_LASTHOST(pc)[1], sizeof(ubit32) * 4);
    PC_LASTHOST(pc)[4] = s_addr;
}

// Take a player which is in the game and move to the menu
// Don't stop the DIL program executing as pdontstop
//
void pc_data::gstate_tomenu(dilprg *pdontstop)
{
    if (this->is_destructed())
    {
        return;
    }

    if (!char_is_playing(this))
    {
        return;
    }

    // slog(LOG_ALL, 0, "DEBUG: To menu %s", UNIT_NAME(this));

    if (!PC_IS_UNSAVED(this))
    {
        save_player(this);
        save_player_contents(this, TRUE);
    }

    CHAR_LAST_ROOM(this) = unit_room(this);

    stop_fightfollow(this);
    stop_snoopwrite(this);

    descriptor_data *tmp_descr = CHAR_DESCRIPTOR(this);
    CHAR_DESCRIPTOR(this) = nullptr;

    while (UNIT_CONTAINS(this))
    {
        extract_unit(UNIT_CONTAINS(this));
    }

    CHAR_DESCRIPTOR(this) = tmp_descr;

    unit_from_unit(this);
    remove_from_unit_list(this);
    dil_stop_special(this, pdontstop);
}

#define DILWAY 1

// Take a player which is in the menu and move to the game
// Don't start the DIL program executing as pdontstart
//
void pc_data::gstate_togame(dilprg *pdontstop)
{
    descriptor_data *i = nullptr;
    const time_t last_connect = m_time.getPlayerLastConnectTime();

    if (this->is_destructed())
    {
        return;
    }

    if (char_is_playing(this))
    { // Are we in the menu?
        return;
    }

    // slog(LOG_ALL, 0, "DEBUG: To game %s", UNIT_NAME(this));

    if (CHAR_DESCRIPTOR(this))
    {
        update_lasthost(this, inet_addr(CHAR_DESCRIPTOR(this)->getHostname()));

        CHAR_DESCRIPTOR(this)->setMinutesPlayerIdle(0);
        CHAR_DESCRIPTOR(this)->setPromptMode(PROMPT_EXPECT);
        CHAR_DESCRIPTOR(this)->setLastLogonTime(::time(nullptr));
        m_time.setPlayerLastConnectTime(time(nullptr));
        set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);
        dil_destroy("link_dead@basis", this);

        this->connect_game();
    }

    unit_data *load_room = nullptr;
    if (CHAR_LAST_ROOM(this))
    {
        load_room = CHAR_LAST_ROOM(this);
        CHAR_LAST_ROOM(this) = nullptr;
    }
    else
    {
        load_room = hometown_unit(PC_HOME(this));
    }

    reset_char(this);

    insert_in_unit_list(this);
    unit_to_unit(this, load_room);

    if (CHAR_DESCRIPTOR(this) && !DILWAY) /* Only do these things if player is connected */
    {
        auto msg = diku::format_to_str("%s has entered the world.<br/>", UNIT_NAME(this));

        for (i = g_descriptor_list; i; i = i->getNext())
        {
            if (descriptor_is_playing(i) && i->cgetCharacter() != this && CHAR_CAN_SEE(CHAR_ORIGINAL(i->cgetCharacter()), this) &&
                IS_PC(CHAR_ORIGINAL(i->cgetCharacter())) && IS_SET(PC_FLAGS(CHAR_ORIGINAL(i->cgetCharacter())), PC_INFORM) &&
                !same_surroundings(this, i->cgetCharacter()))
            {
                send_to_descriptor(msg, i);
            }
        }

        act("$1n has arrived.", A_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), TO_ROOM);
    }

    /* New player stats. Level can be zero after reroll while ID is not. */
    if ((CHAR_LEVEL(this) == 0) && PC_IS_UNSAVED(this))
    {
        slog(LOG_BRIEF, 0, "%s[%s] (GUEST) has entered the game.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->getHostname());

        PC_ID(this) = new_player_id();

        start_player(this);
    }
    else
    {
        if (!DILWAY)
        {
            command_interpreter(this, "look");
        }
    }
    if (file_exists(ContentsFileName(PC_FILENAME(this))))
    {
        load_contents(PC_FILENAME(this), this);

        if (!DILWAY)
        {
            rent_calc(this, last_connect);
        }
    }

    /*		if (!dilway)*/
    if (strcmp(g_cServerConfig.getImmortalName().c_str(), UNIT_NAME(this)) == 0)
    {
        points.setLevel(ULTIMATE_LEVEL);
    }

    if (IS_ULTIMATE(this) && PC_IS_UNSAVED(this))
    {
        save_player(this);
    }

    start_affect(this); /* Activate affect ticks */
    // start_all_special(this); /* Activate fptr ticks   */
    if (!DILWAY)
    {
        send_done(this, nullptr, load_room, 0, &g_cmd_auto_play, "");
        send_done(this, nullptr, load_room, 0, &g_cmd_auto_play, "", nullptr, "void@basis");
    }

    ActivateDil(this);
    dil_start_special(this, pdontstop);
}

/* Set 'd' to 'ch' and enter the game.                            */
/* If ch has UNIT_IN set, then it is because ch must be link dead */
/*   and thus a reconnect is performed.                           */
/* If UNIT_IN is not set, then the char must be put inside the    */
/*   game, and his inventory loaded.                              */
void enter_game(unit_data *ch, int dilway)
{
    UPC(ch)->gstate_togame(nullptr);
}

void set_descriptor_fptr(descriptor_data *d, void (*fptr)(descriptor_data *, char *), ubit1 call)
{
    if (d->getFunctionPtr() == interpreter_string_add)
    {
        d->clearLocalString();
        d->setEditReference(nullptr);
        d->setPostEditFunctionPtr(nullptr);
        d->setEditing(nullptr);
    }

    d->setFunctionPtr(fptr);
    if (call)
    {
        char constStr[] = ""; // cheat to get rid of warnings.  todo fix correctly
        d->setState(0);
        d->callFunctionPtr(d, constStr);
    }
    else
    {
        d->setState(1);
    }
}

/* Return TRUE if help is given (found)... */

/*int nanny_help_check(class descriptor_data *d, char *arg, char *def)
{
   char buf[200+MAX_INPUT_LENGTH];

   int help_base(class descriptor_data *d, char *arg);

   arg = skip_spaces(arg);

   arg = str_next_word(arg, buf);

   str_lower(buf);
   if (!(strcmp("help", buf)==0 || strcmp("hel", buf)==0 ||
         strcmp("he", buf)==0 || strcmp("h", buf)==0 ||
         strcmp(buf, "?")==0))
     return FALSE;

   if (str_is_empty(arg))
     arg = def;

   if (!help_base(d, arg))
   {
      s printf(buf, "There is no help on the subject '%s'.<br/>", arg);
      send_to_descriptor(buf, d);
   }
   return TRUE;
}
*/

void nanny_close(descriptor_data *d, char *arg)
{
    descriptor_close(d);
}

void nanny_motd(descriptor_data *d, char *arg)
{
    diltemplate *on_connect = nullptr;
    g_dilmenu = FALSE;
    on_connect = find_dil_template("on_connect@basis");
    if (on_connect)
    {
        g_dilmenu = TRUE;
        // Nono... only enter the game when entering from the menu (DIL) enter_game(d->character, TRUE);
        dilprg *prg = dil_copy_template(on_connect, d->getCharacter(), nullptr);
        if (prg)
        {
            set_descriptor_fptr(d, descriptor_interpreter, TRUE);
            prg->waitcmd = WAITCMD_MAXINST - 1;
            dil_activate(prg);
        }
        else
        {
            slog(LOG_ALL, 0, "nanny_motd() no on_connect@basis");
        }
    }
    else
    {
        /*fuck*/
        std::string buf{"Welcome to "};
        buf += g_cServerConfig.getMudName() + "!<br/>";
        send_to_descriptor(buf, d);
        enter_game(d->getCharacter());
    }
}

void nanny_throw(descriptor_data *d, char *arg)
{
    descriptor_data *td = nullptr;
    unit_data *u = nullptr;

    if (d->postincrementState() == 0)
    {
        send_to_descriptor("Already playing!<br/>Throw the other copy out? (Y/N) ", d);
        return;
    }

    assert(d->cgetCharacter());

    if (*arg == 'y' || *arg == 'Y')
    {
        // Close all descriptors except the one that just said YES
        while ((td = find_descriptor(PC_FILENAME(d->cgetCharacter()), d)))
        {
            send_to_descriptor("You got purged by your alter ego from the menu.<br/>", td);
            descriptor_close(td, TRUE, TRUE);
        }

        assert(d->cgetCharacter());

        // Scan the game for a unit that is a PCs and hold the same name
        // they should all be descriptorless now (except for d trying to login)
        for (u = g_unit_list; u; u = u->gnext)
        {
            if (!IS_PC(u))
            {
                break; // PCs are always first in the list
            }

            if (str_ccmp(PC_FILENAME(d->cgetCharacter()), PC_FILENAME(u)) == 0)
            {
                //	      assert (!CHAR_DESCRIPTOR (u));
                //	      assert (UNIT_IN (u));

                if (!UNIT_IN(u))
                {
                    slog(LOG_ALL, 0, "nanny_throw() player found but not in any units. Debug info - inspect me.");
                }

                /*
                // If it's a guest player
                if (PC_IS_UNSAVED(u))
                {
                   // descriptor is closed, no msg will arrive : send_to_char("You got purged by someone in the menu.<br/>", u);
                   extract_unit(u);
                   break; // Break so that the guest gets purged
                } */

                CHAR_LAST_ROOM(u) = UNIT_IN(u);
                UPC(u)->reconnect_game(d);
                return;
            }
        }

        // Reconnecting character was NOT in the game, in the menu, so for guests, just close
        if (PC_IS_UNSAVED(d->cgetCharacter()) || !player_exists(PC_FILENAME(d->cgetCharacter())))
        {
            send_to_descriptor("Menu Guest, purging all connections - please retry.<br/>", d);
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }

        // Reconnecting character was NOT in the game, so just set to MOTD and move on
        set_descriptor_fptr(d, nanny_motd, TRUE);
        return;
    }
    else if (*arg == 'n' || *arg == 'N')
    {
        send_to_descriptor("Ok, goodbye then.<br/>", d);
        set_descriptor_fptr(d, nanny_close, TRUE);
    }
    else
    {
        send_to_descriptor("Throw the other copy out? Please type Yes or No: ", d);
    }
}

void nanny_dil(descriptor_data *d, char *arg)
{
    extra_descr_data *exd = nullptr;

    exd = UNIT_EXTRA(d->getCharacter()).find_raw("$nanny");

    if (exd && !str_is_empty(exd->names.Name(1)))
    {
        char buf[256];

        strcpy(buf, exd->names.Name(1));
    }

    if (g_nanny_dil_tmpl)
    {
        dilprg *prg = nullptr;

        prg = dil_copy_template(g_nanny_dil_tmpl, d->getCharacter(), nullptr);
        if (prg)
        {
            prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

            prg->fp->vars[0].val.string = str_dup(arg);

            dil_activate(prg);
        }
    }

    if (d->cgetCharacter() && UNIT_EXTRA(d->getCharacter()).find_raw("$nanny") == nullptr)
    {
        g_dilmenu = TRUE;
        enter_game(d->getCharacter(), TRUE);
    }
}

void nanny_pwd_confirm(descriptor_data *d, char *arg)
{
    unit_data *u = nullptr;

    if (d->postincrementState() == 0)
    {
        send_to_descriptor("<br/>Please retype password: ", d);
        send_to_descriptor(scriptwrap("PasswordOn()").c_str(), d);
        return;
    }

    if (pwdcompare(crypt(arg, PC_FILENAME(d->cgetCharacter())), PC_PWD(d->cgetCharacter()), PC_MAX_PASSWORD))
    {
        auto str = diku::format_to_str("PasswordOff('', '%s')", g_cServerConfig.getMudName().c_str());
        send_to_descriptor(scriptwrap(str), d);
        send_to_descriptor("Passwords don't match.<br/>", d);
        set_descriptor_fptr(d, nanny_new_pwd, TRUE);
        return;
    }

    auto str = diku::format_to_str("PasswordOff('%s', '%s')", PC_FILENAME(d->cgetCharacter()), g_cServerConfig.getMudName().c_str());
    send_to_descriptor(scriptwrap(str), d);

    descriptor_data *td = nullptr;
    while ((td = find_descriptor(PC_FILENAME(d->cgetCharacter()), d)))
    {
        send_to_descriptor("You got purged by your alter ego from the menu.<br/>", td);
        descriptor_close(td, TRUE, TRUE);
    }

    assign_player_file_index(d->getCharacter());

    /* See if guest is in game, if so - a guest was LD       */
    /* Password has now been redefined                       */
    for (u = g_unit_list; u; u = u->gnext)
    {
        if (IS_PC(u) && (str_ccmp(PC_FILENAME(u), PC_FILENAME(d->cgetCharacter())) == 0))
        {
            UPC(u)->reconnect_game(d);
            return;
        }
    }

    set_descriptor_fptr(d, nanny_dil, TRUE);
}

int check_pwd(descriptor_data *d, char *pwd)
{
    int i = 0;
    int bA = 0;
    int bNA = 0;

    if (strlen(pwd) < 5)
    {
        send_to_descriptor("The chosen password is too short.<br/>"
                           "Your password has to be at least 5 characters and "
                           "must contain both letters and digits.<br/>",
                           d);
        return FALSE;
    }

    pwd[PC_MAX_PASSWORD - 1] = 0;

    bA = FALSE;
    bNA = FALSE;

    for (i = 0; pwd[i]; i++)
    {
        if (isalpha(pwd[i]))
        {
            bA = TRUE;
        }
        else
        {
            bNA = TRUE;
        }
    }

    if (bA == FALSE)
    {
        send_to_descriptor("<br/>The chosen password only contains letters.<br/>"
                           "Your password has to be at least 5 characters and "
                           "must contain both letters and digits.<br/>",
                           d);
        return FALSE;
    }

    if (bNA == FALSE)
    {
        send_to_descriptor("<br/>The chosen password only contains "
                           "non-letters (digits).<br/>"
                           "Your password has to be at least 5 characters and "
                           "must contain both letters and digits.<br/>",
                           d);
        return FALSE;
    }

    return TRUE;
}

void nanny_new_pwd(descriptor_data *d, char *arg)
{
    if (d->postincrementState() == 0)
    {
        auto msg = diku::format_to_str("Give me a new password for %s: ", UNIT_NAME(d->cgetCharacter()));
        send_to_descriptor(msg, d);
        send_to_descriptor(scriptwrap("PasswordOn()"), d);
        return;
    }

    auto str = diku::format_to_str("PasswordOff('', '%s')", g_cServerConfig.getMudName());
    send_to_descriptor(scriptwrap(str), d);

    if (!check_pwd(d, arg))
    {
        set_descriptor_fptr(d, nanny_new_pwd, TRUE);
        return;
    }

    strncpy(PC_PWD(d->cgetCharacter()), crypt(arg, PC_FILENAME(d->cgetCharacter())), PC_MAX_PASSWORD);
    PC_PWD(d->cgetCharacter())[PC_MAX_PASSWORD - 1] = 0;

    set_descriptor_fptr(d, nanny_pwd_confirm, TRUE);
}

/* Return TRUE when done... */
ubit1 base_string_add(descriptor_data *d, char *str)
{
    char *scan = nullptr;
    int terminator = 0;

    if (d->postincrementState() == 0)
    {
        send_to_descriptor("Terminate with a '@'.<br/>", d);
        if (d->getLocalString())
        {
            slog(LOG_ALL, 0, "Spooky localstr in base_string_add - tell papi.");
        }
        d->clearLocalString();
        return FALSE;
    }

    /* determine if this is the terminal string, and truncate if so */
    for (scan = str; *scan; scan++)
    {
        if ((terminator = (*scan == '@' && scan[1] == '\0')))
        {
            *scan = '\0';
            break;
        }
    }

    if (MAX_STRING_LENGTH - (d->getLocalString() ? strlen(d->getLocalString()) : 0) < strlen(str))
    {
        str[MAX_STRING_LENGTH - (d->getLocalString() ? strlen(d->getLocalString()) : 0)] = '\0';
        terminator = 1;

        send_to_descriptor("String too long - Truncated.<br/>", d);
    }

    d->setLocalString(str);

    if (terminator)
    {
        if (d->hasPostEditFunctionPtr())
        {
            d->callPostEditFunctionPtr(d);
        }

        d->clearLocalString();
        d->setEditReference(nullptr);
        d->setPostEditFunctionPtr(nullptr);
        d->setEditing(nullptr);

        return TRUE;
    }
    else
    {
        d->appendLocalString("<br/>");
    }

    return FALSE;
}

/* Add user input to the 'current' string (as defined by d->str) */
void interpreter_string_add(descriptor_data *d, char *str)
{
    if (base_string_add(d, str))
    {
        set_descriptor_fptr(d, descriptor_interpreter, FALSE);
    }
}

/* Removes empty descriptions and makes ONE newline after each. */

void nanny_fix_descriptions(unit_data *u)
{
    extra_descr_data *exd = nullptr;
    char buf[1024];

    for (exd = UNIT_EXTRA(u).m_pList; exd; exd = exd->next)
    {
        if (exd->names.Name())
        {
            strcpy(buf, exd->names.Name());
        }
        else
        {
            *buf = 0;
        }

        if (!exd->names.Name() || search_block(buf, g_bodyparts, TRUE))
        {
            if (str_is_empty(exd->descr.c_str()))
            {
                UNIT_EXTRA(u).erase(exd);
                nanny_fix_descriptions(u); // MS2020 terrible kludge :)
                return;
            }
            else
            {
                char buf[MAX_STRING_LENGTH];
                strcpy(buf, exd->descr.c_str());
                strip_trailing_blanks(buf);
                exd->descr = (buf);
            }
        }
    }
}

void nanny_menu(descriptor_data *d, char *arg)
{
    nanny_close(d, arg);
}

void nanny_existing_pwd(descriptor_data *d, char *arg)
{
    descriptor_data *td = nullptr;
    unit_data *u = nullptr;

    /* PC_ID(d->character) can be -1 when a newbie is in the game and
        someone logins with the same name! */

    d->postincrementState();

    if (d->getState() == 1)
    {
        if (PC_CRACK_ATTEMPTS(d->cgetCharacter()) > 2)
        {
            auto msg = diku::format_to_str("<br/>ATTENTION: Your password has been "
                                           "attempted cracked %d times since your last logon."
                                           " Press [enter] and wait for the password prompt.",
                                           PC_CRACK_ATTEMPTS(d->cgetCharacter()));
            send_to_descriptor(msg, d);
            d->setLoopWaitCounter(MIN(30, PC_CRACK_ATTEMPTS(d->cgetCharacter())) * 2 * PULSE_SEC);
            return;
        }
        d->postincrementState();
    }

    if (d->getState() == 2)
    {
        auto msg = diku::format_to_str("Welcome back %s, please enter your password: ", UNIT_NAME(d->cgetCharacter()));
        send_to_descriptor(msg, d);
        send_to_descriptor(scriptwrap("PasswordOn()").c_str(), d);
        return;
    }

    auto str = diku::format_to_str("PasswordOff('%s', '%s')", UNIT_NAME(d->cgetCharacter()), g_cServerConfig.getMudName().c_str());
    send_to_descriptor(scriptwrap(str), d);

    if (str_is_empty(arg))
    {
        // UNIT_NAMES(d->character).Free();
        send_to_descriptor("<br/>Wrong password, please login again.", d);
        set_descriptor_fptr(d, nanny_close, TRUE);
        // set_descriptor_fptr(d, nanny_get_name, FALSE);
        return;
    }

    // MS2020: The PC_PWD (the first two letters) contains the salt interpreted by Crypt
    // when using the default encryption (only allows 8 chars)
    // We could switch to
    //(gdb) print (char *) crypt(arg, "$1$papi")
    // "$1$Pa$N7RTSV11rv3qkWzsTFHU5."
    // Which would allow any pwd length but not work on Macs. Or as Ken suggests we could
    // have two iterations of the default to support up to 16 chars pwd.
    int nCmp = 0;
    nCmp = pwdcompare(crypt(arg, PC_PWD(d->cgetCharacter())), PC_PWD(d->cgetCharacter()), PC_MAX_PASSWORD);

    if (nCmp != 0)
    {
        if (!str_is_empty(arg))
        {
            slog(LOG_ALL, 0, "%s entered a wrong password [%s].", PC_FILENAME(d->cgetCharacter()), d->getHostname());
            PC_CRACK_ATTEMPTS(d->cgetCharacter())++;

            if ((td = find_descriptor(PC_FILENAME(d->cgetCharacter()), d)))
            {
                send_to_descriptor("<br/>Someone just attempted to login under "
                                   "your name using an illegal password.<br/>",
                                   td);
                PC_CRACK_ATTEMPTS(td->cgetCharacter())++;
                d->setLoopWaitCounter(PULSE_SEC * 5 + PC_CRACK_ATTEMPTS(td->cgetCharacter()) * PULSE_SEC);
            }
            else if (!PC_IS_UNSAVED(d->cgetCharacter()))
            {
                save_player_file(d->getCharacter());
            }
        }

        send_to_descriptor("<br/>Wrong password.<br/>", d);
        set_descriptor_fptr(d, nanny_close, TRUE);
        return;
    }

    PC_CRACK_ATTEMPTS(d->cgetCharacter()) = 0;

    const auto last_connect = PC_TIME(d->cgetCharacter()).getPlayerLastConnectTime();
    auto msg2 = diku::format_to_str("<br/>Welcome back %s, you last visited %s on %s<br/>",
                                    UNIT_NAME(d->cgetCharacter()),
                                    g_cServerConfig.getMudName().c_str(),
                                    ctime(&last_connect));
    send_to_descriptor(msg2, d);

    if ((td = find_descriptor(PC_FILENAME(d->cgetCharacter()), d)))
    {
        set_descriptor_fptr(d, nanny_throw, TRUE);
        return;
    }

    /* See if player is in game (guests are not created in file entries) */
    /* Enters game (reconnects) if true                                  */
    for (u = g_unit_list; u; u = u->gnext)
    {
        if (!IS_PC(u))
        {
            break;
        }

        if (str_ccmp(PC_FILENAME(u), PC_FILENAME(d->cgetCharacter())) == 0)
        {
            //	  assert (!CHAR_DESCRIPTOR (u));
            //	  assert (UNIT_IN (u));

            UPC(u)->reconnect_game(d);
            return;
        }
    }

    /* Ok, he wasn't Link Dead, lets enter the game via menu */
    slog(LOG_BRIEF, CHAR_LEVEL(d->cgetCharacter()), "%s[%s] has connected.", PC_FILENAME(d->cgetCharacter()), d->getHostname());

    send_to_descriptor("<br/>", d);
    set_descriptor_fptr(d, nanny_motd, TRUE);
}

void nanny_name_confirm(descriptor_data *d, char *arg)
{
    if (d->postincrementState() == 0)
    {
        // MS: removed help option since it was not implemented.
        auto msg = diku::format_to_str("Did I get that right, %s (Y/N)? ", UNIT_NAME(d->cgetCharacter()));
        send_to_descriptor(msg, d);
        return;
    }

    /*   if (nanny_help_check(d, arg, "login"))
        {
           STATE(d) = 0;
           nanny_name_confirm(d, arg);
           return;
        }
     */
    if (*arg == 'y' || *arg == 'Y')
    {
        if (site_banned(d->getHostname()) != NO_BAN)
        {
            show_ban_text(d->getHostname(), d);
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }
        send_to_descriptor("New character.<br/>", d);
        set_descriptor_fptr(d, nanny_new_pwd, TRUE);
    }
    else if (*arg == 'n' || *arg == 'N')
    {
        if (PC_FILENAME(d->cgetCharacter()))
        {
            strcpy(PC_FILENAME(d->cgetCharacter()), "");
        }
        UNIT_NAMES(d->getCharacter()).Free();
        send_to_descriptor("Ok, what IS it, then? ", d);
        set_descriptor_fptr(d, nanny_get_name, FALSE);
    }
    else
    {
        send_to_descriptor("Please type Yes or No: ", d);
    }
}

void nanny_get_name(descriptor_data *d, char *arg)
{
    char tmp_name[100];
    descriptor_data *td = nullptr;

    if (str_is_empty(arg))
    {
        set_descriptor_fptr(d, nanny_close, TRUE);
        return;
    }

    if (_parse_name(arg, tmp_name))
    {
        send_to_descriptor("Illegal name, please try another.<br/>", d);
        send_to_descriptor("Name: ", d);
        return;
    }

    if (player_exists(tmp_name))
    {
        unit_data *ch = nullptr;

        if (site_banned(d->getHostname()) == BAN_TOTAL)
        {
            show_ban_text(d->getHostname(), d);
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }

        /* Known player, lets load his character information. */
        ch = load_player(tmp_name);

        if (ch == nullptr)
        {
            send_to_descriptor("LOAD ERROR! PLEASE MAKE A NOTE OF ANY "
                               "SPECIAL ACTIONS YOU MAY HAVE TAKEN JUST "
                               "PRIOR TO YOUR LAST QUIT OR PERHAPS EVEN "
                               "GAME CRASH. ENTER WITH A GUEST AND CONTACT "
                               "A GOD.<br/>",
                               d);
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }

        CHAR_DESCRIPTOR(d->cgetCharacter()) = nullptr;
        extract_unit(d->getCharacter());

        CHAR_DESCRIPTOR(ch) = d;
        d->setCharacter(ch);

        if (g_wizlock && CHAR_LEVEL(d->cgetCharacter()) < g_wizlock)
        {
            send_to_descriptor("Sorry, the game is wizlocked for "
                               "your level.<br/>",
                               d);
            slog(LOG_BRIEF, 0, "Wizlock lockout for %s.", PC_FILENAME(d->cgetCharacter()));
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }

        PC_SETUP_COLOUR(d->cgetCharacter()) = 0;
        MplexSendSetup(d);
        set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
        return;
    }
    else
    {
        /* Check for g_wizlock */
        if (g_wizlock)
        {
            send_to_descriptor("Sorry, no new players now, the game "
                               "is wizlocked!<br/>",
                               d);
            slog(LOG_BRIEF, 0, "Wizlock lockout for %s.", PC_FILENAME(d->cgetCharacter()));
            set_descriptor_fptr(d, nanny_close, TRUE);
            return;
        }

        /* New player                                           */
        /* Check for both duplicate descriptors, and link death */

        /* all in lowercase... */
        strcpy(PC_FILENAME(d->cgetCharacter()), tmp_name);

        CAPC(tmp_name);
        UNIT_NAMES(d->getCharacter()).AppendName(tmp_name);
        strcpy(PC_PWD(d->cgetCharacter()), "");

        /* If someone is connected, we borrow his pwd */
        if ((td = find_descriptor(tmp_name, d)))
        {
            // Only borrow a password if its already been set
            if (*PC_PWD(td->cgetCharacter()))
            {
                strcpy(PC_PWD(d->cgetCharacter()), PC_PWD(td->cgetCharacter()));
                set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
                return;
            }
        }

        /* Check for LD after PWD confirmation, this is due to */
        /* that the PWD is lost when a guests link is lost!    */

        set_descriptor_fptr(d, nanny_name_confirm, TRUE);
    }
}
