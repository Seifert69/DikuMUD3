
/*
 $Author: All $
 $RCSfile: nanny.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.12 $
 */

#include <stdlib.h>
#include <string>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
#include <unistd.h>
#elif defined LINUX
#include <crypt.h>
#endif

#include "db.h"
#include "badnames.h"
#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "comm.h"
#include "system.h"
#include "db.h"
#include "utility.h"
#include "handler.h"
#include "interpreter.h"
#include "ban.h"
#include "money.h"
#include "files.h"
#include "protocol.h"
#include "common.h"
#include "affect.h"
#include "vmelimits.h"
#include "account.h"
#include "trie.h"
#include "constants.h"
#include "main.h"
#include "modify.h"
#include "dilrun.h"
#ifndef _WINDOWS
#include <arpa/telnet.h>
#endif

#define STATE(d) ((d)->state)

void enter_game(class unit_data *ch, int dilway = FALSE);
void start_all_special(class unit_data *u);
void stop_all_special(class unit_data *u);
extern class unit_data *unit_list;
extern class descriptor_data *descriptor_list;
extern int wizlock;
extern int possible_saves;
int dilmenu;
void nanny_get_name(class descriptor_data *d, char *arg);
void set_descriptor_fptr(class descriptor_data *d,
                         void (*fptr)(class descriptor_data *, char *),
                         ubit1 call);
void nanny_menu(class descriptor_data *d, char *arg);
void nanny_new_pwd(class descriptor_data *d, char *arg);
void multi_close(struct multi_element *pe);
int player_exists(const char *pName);
void save_player_file(class unit_data *ch);

extern struct diltemplate *nanny_dil_tmpl;
char *read_info_file(char *name, char *oldstr);
char *m_pBadNames = NULL;
char *m_pBadStrings = NULL;

/* *************************************************************************
 *  Stuff for controlling the non-playing sockets (get name, pwd etc)      *
 ************************************************************************* */

int _parse_name(const char *arg, char *name)
{
   int i;

   class badnames_list badnames;
   class badnames_list badstrings;
   m_pBadNames = NULL;
   m_pBadStrings = NULL;

   arg = skip_spaces(arg); /* skip whitespaces */

   /* i+1 = chars copied    */
   /* PC_MAX_NAME-1 = chars */

   for (i = 0; (*name = *arg); arg++, i++, name++)
      if ((*arg <= ' ') || !isalpha(*arg) || (i + 1) > (PC_MAX_NAME - 1))
         return 1;

   if (i <= 2) /* Names must be at least 3 chars due to passwords */
      return 1;

   if (fill_word(name - i)) /* Don't allow fillwords */
      return 1;

   touch_file(str_cc(g_cServerConfig.m_etcdir, BAD_STRINGS_FILE));
   m_pBadStrings =
       read_info_file(str_cc(g_cServerConfig.m_etcdir, BAD_STRINGS_FILE), m_pBadStrings);
   badstrings.create(m_pBadStrings);
   FREE(m_pBadStrings);

   if (badstrings.in(name - i))
      return 2;

   touch_file(str_cc(g_cServerConfig.m_etcdir, BAD_NAMES_FILE));
   m_pBadNames =
       read_info_file(str_cc(g_cServerConfig.m_etcdir, BAD_NAMES_FILE), m_pBadNames);
   badnames.create(m_pBadNames);
   FREE(m_pBadNames);

   if (badnames.equal(name - i))
      return 3;

   str_lower(name);

   return 0;
}

/* This is called from main_loop every n seconds (long duration) to check */
/* for idle time for any descriptors                                      */
void check_idle(void)
{
   class descriptor_data *d, *next_d;
   time_t now = time(0);

   for (d = descriptor_list; d; d = next_d)
   {
      next_d = d->next;
      d->timer++;
      if (!descriptor_is_playing(d)) /* Not in game yet */
      {
         if (d->fptr == nanny_get_name)
         {
            if (d->timer >= 2)
            {
               slog(LOG_ALL, 0,
                    "Kicking out idle player waiting for name.");
               descriptor_close(d);
            }
         }
         else
         {
            if (d->timer >= 10)
            {
               slog(LOG_ALL, 0, "Kicking out player from menu.");
               descriptor_close(d);
            }
         }
      }
      else
      {
         if (d->timer >= 20 && IS_MORTAL(d->character))
         {
            slog(LOG_ALL, 0,
                 "Kicking out idle player and making link-dead.");
            descriptor_close(d);
         }
         else if (IS_PC(d->character) &&
                  now - d->logon >= SECS_PER_REAL_HOUR / 3)
         {
            send_to_char("Autosave.<br/>", d->character);
            save_player(d->character);
            save_player_contents(d->character, TRUE); /* No compress */
            /* Only save one player per autosave... */
            return;
         }
      }
   }
}

/* clear some of the the working variables of a char */
void reset_char(class unit_data *ch)
{
   /* Ok, this is test to avoid level 255's entering the game... */
   //  assert (CHAR_SEX (ch) != 255);
   //  assert (CHAR_POS (ch) != 255);

   UNIT_MAX_HIT(ch) = hit_limit(ch);
}

void pc_data::connect_game(void)
{
   //  assert (CHAR_DESCRIPTOR (pc));

   if (this->is_destructed() || !CHAR_DESCRIPTOR(this))
      return;

   PC_TIME(this).connect = time(0);
   CHAR_DESCRIPTOR(this)->logon = time(0);

   CHAR_DESCRIPTOR(this)->CreateBBS();

   set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);

   no_players++;
   if (no_players > max_no_players)
      max_no_players = no_players;
}

void pc_data::disconnect_game(void)
{
   if (CHAR_DESCRIPTOR(this))
   {
      CHAR_DESCRIPTOR(this)->RemoveBBS();

      no_players--;
   }
}

void pc_data::reconnect_game(class descriptor_data *d)
{
   //char *color;
   //char tbuf[MAX_STRING_LENGTH * 2];

   if (this->is_destructed() || !d)
      return;

   CHAR_DESCRIPTOR(d->character) = NULL;
   extract_unit(d->character); // Toss out the temporary unit and take over the new one
   d->character = this;
   CHAR_DESCRIPTOR(this) = d;

   dil_destroy("link_dead@basis", this);

   ActivateDil(this); // ch could potentially get zapped here.

   if (this->is_destructed() || !d)
      return;

   this->connect_game();
   /* MS2020
    color = UPC (ch)->color.save_string ();
    sprintf (tbuf, "%s%s%s", CONTROL_COLOR_CREATE, color, CONTROL_COLOR_END);
    send_to_char (tbuf, ch);
    delete color;
*/
   send_to_char("Reconnecting.<br/>", this);

   if (CHAR_LAST_ROOM(this) && (CHAR_LAST_ROOM(this) != UNIT_IN(this)))
   {
      act("$1n has reconnected, and is moved to another location.",
          A_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), TO_ROOM);
      unit_from_unit(this);
      unit_to_unit(this, CHAR_LAST_ROOM(this));
      CHAR_LAST_ROOM(this) = NULL;
   }
   act("$1n has reconnected.", A_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), TO_ROOM);
   slog(LOG_BRIEF, UNIT_MINV(this), "%s[%s] has reconnected.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->host);
   CHAR_DESCRIPTOR(this)->logon = ::time(0);
   PC_TIME(this).connect = ::time(0);
   //      stop_affect(ch);
   //      stop_all_special(ch);
   //      start_affect(ch);               /* Activate affect ticks */
   //   start_all_special(ch);     /* Activate fptr ticks   */

   set_descriptor_fptr(d, descriptor_interpreter, FALSE);
}

void update_lasthost(class unit_data *pc, ubit32 s_addr)
{
   if ((sbit32)s_addr == -1)
      return;

   for (int i = 0; i < 5; i++)
      if (PC_LASTHOST(pc)[i] == s_addr)
         return;

   memmove(&PC_LASTHOST(pc)[0], &PC_LASTHOST(pc)[1], sizeof(ubit32) * 4);
   PC_LASTHOST(pc)
   [4] = s_addr;
}




// Take a player which is in the game and move to the menu
// Don't stop the DIL program executing as pdontstop
//
void pc_data::gstate_tomenu(dilprg *pdontstop)
{
   void dil_stop_special(class unit_data *unt, class dilprg *aprg);
   void stop_snoopwrite(unit_data *unit);

   if (this->is_destructed())
      return;

   if (!char_is_playing(this))
      return;

   //slog(LOG_ALL, 0, "DEBUG: To menu %s", UNIT_NAME(this));

   if (!PC_IS_UNSAVED(this))
   {
      save_player(this);
      save_player_contents(this, TRUE);
   }

   CHAR_LAST_ROOM(this) = unit_room(this);

   stop_fightfollow(this);
   stop_snoopwrite(this);

   descriptor_data *tmp_descr = CHAR_DESCRIPTOR(this);
   CHAR_DESCRIPTOR(this) = NULL;

   while (UNIT_CONTAINS(this))
      extract_unit(UNIT_CONTAINS(this));

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
   class descriptor_data *i;
   //char tbuf[MAX_STRING_LENGTH * 2];
   char buf[256];
   time_t last_connect = PC_TIME(this).connect;
   //char *color;

   char *ContentsFileName(const char *);
   void dil_start_special(class unit_data *unt, class dilprg *aprg);

   if (this->is_destructed())
      return;

   if (char_is_playing(this)) // Are we in the menu?
      return; 

   //slog(LOG_ALL, 0, "DEBUG: To game %s", UNIT_NAME(this));

   if (CHAR_DESCRIPTOR(this))
   {
      update_lasthost(this, inet_addr(CHAR_DESCRIPTOR(this)->host));

      CHAR_DESCRIPTOR(this)->timer = 0;
      CHAR_DESCRIPTOR(this)->prompt_mode = PROMPT_EXPECT;
      CHAR_DESCRIPTOR(this)->logon = ::time(0);
      PC_TIME(this).connect = ::time(0);
      set_descriptor_fptr(CHAR_DESCRIPTOR(this), descriptor_interpreter, FALSE);
      dil_destroy("link_dead@basis", this);

      this->connect_game();
   }

   unit_data *load_room;
   if (CHAR_LAST_ROOM(this))
   {
      load_room = CHAR_LAST_ROOM(this);
      CHAR_LAST_ROOM(this) = NULL;
   }
   else
      load_room = hometown_unit(PC_HOME(this));

   reset_char(this);

   insert_in_unit_list(this);
   unit_to_unit(this, load_room);

   if (CHAR_DESCRIPTOR(this) && !DILWAY) /* Only do these things if player is connected */
   {
      sprintf(buf, "%s has entered the world.<br/>", UNIT_NAME(this));

      for (i = descriptor_list; i; i = i->next)
         if (descriptor_is_playing(i) && i->character != this &&
             CHAR_CAN_SEE(CHAR_ORIGINAL(i->character), this) &&
             IS_PC(CHAR_ORIGINAL(i->character)) &&
             IS_SET(PC_FLAGS(CHAR_ORIGINAL(i->character)), PC_INFORM) &&
             !same_surroundings(this, i->character))
            send_to_descriptor(buf, i);

      act("$1n has arrived.", A_HIDEINV, cActParameter(this), cActParameter(), cActParameter(), TO_ROOM);
   }

   /* New player stats. Level can be zero after reroll while ID is not. */
   if ((CHAR_LEVEL(this) == 0) && PC_IS_UNSAVED(this))
   {
      void start_player(class unit_data * ch);
      slog(LOG_BRIEF, 0, "%s[%s] (GUEST) has entered the game.", PC_FILENAME(this), CHAR_DESCRIPTOR(this)->host);

      sbit32 new_player_id(void);

      PC_ID(this) = new_player_id();

      start_player(this);
   }
   else
   {
      if (!DILWAY)
         command_interpreter(this, "look");
   }
   if (file_exists(ContentsFileName(PC_FILENAME(this))))
   {
      ubit32 rent_calc(class unit_data * ch, time_t savetime);

      load_contents(PC_FILENAME(this), this);

      if (!DILWAY)
         rent_calc(this, last_connect);
   }

   /*		if (!dilway)*/
   if (strcmp(g_cServerConfig.m_pImmortName, UNIT_NAME(this)) == 0)
      CHAR_LEVEL(this) = ULTIMATE_LEVEL;

   if (IS_ULTIMATE(this) && PC_IS_UNSAVED(this))
      save_player(this);

   start_affect(this);      /* Activate affect ticks */
   // start_all_special(this); /* Activate fptr ticks   */
   if (!DILWAY)
   {
      send_done(this, NULL, load_room, 0, &cmd_auto_play, "");
      send_done(this, NULL, load_room, 0, &cmd_auto_play, "", NULL, "void@basis");
   }

   ActivateDil(this);
   dil_start_special(this, pdontstop);
}


/* Set 'd' to 'ch' and enter the game.                            */
/* If ch has UNIT_IN set, then it is because ch must be link dead */
/*   and thus a reconnect is performed.                           */
/* If UNIT_IN is not set, then the char must be put inside the    */
/*   game, and his inventory loaded.                              */
void enter_game(class unit_data *ch, int dilway)
{
   UPC(ch)->gstate_togame(NULL);
}


void set_descriptor_fptr(class descriptor_data *d,
                         void (*fptr)(class descriptor_data *, char *),
                         ubit1 call)
{
   if (d->fptr == interpreter_string_add)
   {
      if (d->localstr)
         FREE(d->localstr);

      d->localstr = NULL;
      d->editref = NULL;
      d->postedit = NULL;
      d->editing = NULL;
   }

   d->fptr = fptr;
   if (call)
   {
      char constStr[] = ""; //cheat to get rid of warnings.  todo fix correctly
      d->state = 0;
      (d->fptr)(d, constStr);
   }
   else
      d->state = 1;
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
      sprintf(buf, "There is no help on the subject '%s'.<br/>", arg);
      send_to_descriptor(buf, d);
   }
   return TRUE;
}
*/

void nanny_close(class descriptor_data *d, char *arg)
{
   descriptor_close(d);
}

void nanny_motd(class descriptor_data *d, char *arg)
{
   struct diltemplate *on_connect;
   dilmenu = FALSE;
   on_connect = find_dil_template("on_connect@basis");
   if (on_connect)
   {
      dilmenu = TRUE;
      // Nono... only enter the game when entering from the menu (DIL) enter_game(d->character, TRUE);
      class dilprg *prg = dil_copy_template(on_connect, d->character, NULL);
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
      char buf[200];
      sprintf(buf, "Welcome to %s!<br/>", g_cServerConfig.m_mudname);
      send_to_descriptor(buf, d);
      enter_game(d->character);
   }
   return;
}

void nanny_throw(class descriptor_data *d, char *arg)
{
   class descriptor_data *td;
   class unit_data *u;

   if (STATE(d)++ == 0)
   {
      send_to_descriptor("Already playing!<br/>Throw the other copy out? (Y/N) ", d);
      return;
   }

   assert(d->character);

   if (*arg == 'y' || *arg == 'Y')
   {
      // Close all descriptors except the one that just said YES
      while ((td = find_descriptor(PC_FILENAME(d->character), d)))
      {
         send_to_descriptor("You got purged by your alter ego from the menu.<br/>", td);
         descriptor_close(td, TRUE, TRUE);
      }

      assert(d->character);

      // Scan the game for a unit that is a PCs and hold the same name
      // they should all be descriptorless now (except for d trying to login)
      for (u = unit_list; u; u = u->gnext)
      {
         if (!IS_PC(u))
            break; // PCs are always first in the list

         if (str_ccmp(PC_FILENAME(d->character), PC_FILENAME(u)) == 0)
         {
            //	      assert (!CHAR_DESCRIPTOR (u));
            //	      assert (UNIT_IN (u));

            if (!UNIT_IN(u))
               slog(LOG_ALL, 0, "nanny_throw() player found but not in any units. Debug info - inspect me.");

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
      if (PC_IS_UNSAVED(d->character) || !player_exists(PC_FILENAME(d->character)))
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
      send_to_descriptor("Throw the other copy out? Please type Yes or No: ", d);
}

void nanny_dil(class descriptor_data *d, char *arg)
{
   class extra_descr_data *exd;

   exd = UNIT_EXTRA(d->character).find_raw("$nanny");

   if (exd && !str_is_empty(exd->names.Name(1)))
   {
      char buf[256];

      strcpy(buf, exd->names.Name(1));
   }

   if (nanny_dil_tmpl)
   {
      class dilprg *prg;

      prg = dil_copy_template(nanny_dil_tmpl, d->character, NULL);
      if (prg)
      {
         prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

         prg->fp->vars[0].val.string = str_dup(arg);

         dil_activate(prg);
      }
   }

   if (d->character && UNIT_EXTRA(d->character).find_raw("$nanny") == NULL)
   {
      dilmenu = TRUE;
      enter_game(d->character, TRUE);
   }
}

void nanny_pwd_confirm(class descriptor_data *d, char *arg)
{
   class unit_data *u;

   if (STATE(d)++ == 0)
   {
      send_to_descriptor("<br/>Please retype password: ", d);
      send_to_descriptor(scriptwrap("PasswordOn()").c_str(), d);
      return;
   }

   char buf[512];
   if (pwdcompare(crypt(arg, PC_FILENAME(d->character)), PC_PWD(d->character), PC_MAX_PASSWORD))
   {
      sprintf(buf, "PasswordOff('', '%s')", g_cServerConfig.m_mudname);
      send_to_descriptor(scriptwrap(buf).c_str(), d);
      send_to_descriptor("Passwords don't match.<br/>", d);
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
      return;
   }

   sprintf(buf, "PasswordOff('%s', '%s')", PC_FILENAME(d->character), g_cServerConfig.m_mudname);
   send_to_descriptor(scriptwrap(buf).c_str(), d);

   class descriptor_data *td;
   while ((td = find_descriptor(PC_FILENAME(d->character), d)))
   {
      send_to_descriptor("You got purged by your alter ego from the menu.<br/>", td);
      descriptor_close(td, TRUE, TRUE);
   }

   void assign_player_file_index(unit_data *pc);
   assign_player_file_index(d->character);
   
   /* See if guest is in game, if so - a guest was LD       */
   /* Password has now been redefined                       */
   for (u = unit_list; u; u = u->gnext)
      if (IS_PC(u) && (str_ccmp(PC_FILENAME(u),
                                PC_FILENAME(d->character)) == 0))
      {
         UPC(u)->reconnect_game(d);
         return;
      }

   set_descriptor_fptr(d, nanny_dil, TRUE);
}

int check_pwd(class descriptor_data *d, char *pwd)
{
   int i, bA, bNA;

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
      if (isalpha(pwd[i]))
         bA = TRUE;
      else
         bNA = TRUE;

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

void nanny_new_pwd(class descriptor_data *d, char *arg)
{
   if (STATE(d)++ == 0)
   {
      char buf[100];

      sprintf(buf, "Give me a new password for %s: ",
              UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
      send_to_descriptor(scriptwrap("PasswordOn()").c_str(), d);
      return;
   }

   char buf[512];
   sprintf(buf, "PasswordOff('', '%s')", g_cServerConfig.m_mudname);
   send_to_descriptor(scriptwrap(buf).c_str(), d);

   if (!check_pwd(d, arg))
   {
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
      return;
   }

   strncpy(PC_PWD(d->character), crypt(arg, PC_FILENAME(d->character)), PC_MAX_PASSWORD);
   PC_PWD(d->character)
   [PC_MAX_PASSWORD - 1] = 0;

   set_descriptor_fptr(d, nanny_pwd_confirm, TRUE);
}

/* Return TRUE when done... */
ubit1 base_string_add(class descriptor_data *d, char *str)
{
   char *scan;
   int terminator = 0;

   if (STATE(d)++ == 0)
   {
      send_to_descriptor("Terminate with a '@'.<br/>", d);
      if (d->localstr)
      {
         slog(LOG_ALL, 0,
              "Spooky localstr in base_string_add - tell papi.");
         FREE(d->localstr); // Spooky!
      }
      d->localstr = NULL;
      return FALSE;
   }

   /* determine if this is the terminal string, and truncate if so */
   for (scan = str; *scan; scan++)
      if ((terminator = (*scan == '@' && scan[1] == '\0')))
      {
         *scan = '\0';
         break;
      }

   if (MAX_STRING_LENGTH - (d->localstr ? strlen(d->localstr) : 0) <
       strlen(str))
   {
      str[MAX_STRING_LENGTH - (d->localstr ? strlen(d->localstr) : 0)] =
          '\0';
      terminator = 1;

      send_to_descriptor("String too long - Truncated.<br/>", d);
   }

   if (d->localstr == NULL)
   {
      CREATE(d->localstr, char, strlen(str) + 8);
      strcpy(d->localstr, str);
   }
   else
   {
      RECREATE(d->localstr, char, strlen(d->localstr) + strlen(str) + 8);
      strcat(d->localstr, str);
   }

   if (terminator)
   {
      if (d->postedit)
         d->postedit(d);

      if (d->localstr)
         FREE(d->localstr);

      d->localstr = NULL;
      d->editref = NULL;
      d->postedit = NULL;
      d->editing = NULL;

      return TRUE;
   }
   else
      strcat(d->localstr, "<br/>");

   return FALSE;
}

/* Add user input to the 'current' string (as defined by d->str) */
void interpreter_string_add(class descriptor_data *d, char *str)
{
   if (base_string_add(d, str))
      set_descriptor_fptr(d, descriptor_interpreter, FALSE);
}

/* Removes empty descriptions and makes ONE newline after each. */

void nanny_fix_descriptions(class unit_data *u)
{
   class extra_descr_data *exd;
   char buf[1024];

   for (exd = UNIT_EXTRA(u).m_pList; exd; exd = exd->next)
   {
      if (exd->names.Name())
         strcpy(buf, exd->names.Name());
      else
         *buf = 0;

      if (!exd->names.Name() || search_block(buf, bodyparts, TRUE))
      {
         if (str_is_empty(exd->descr.c_str()))
         {
            UNIT_EXTRA(u).erase(exd);
            nanny_fix_descriptions(u); //MS2020 terrible kludge :)
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

void nanny_menu(class descriptor_data *d, char *arg)
{
   nanny_close(d, arg);
}

void nanny_existing_pwd(class descriptor_data *d, char *arg)
{
   char buf[400];
   class descriptor_data *td;
   class unit_data *u;

   /* PC_ID(d->character) can be -1 when a newbie is in the game and
       someone logins with the same name! */

   STATE(d)++;

   if (STATE(d) == 1)
   {
      if (PC_CRACK_ATTEMPTS(d->character) > 2)
      {
         sprintf(buf, "<br/>ATTENTION: Your password has been "
                      "attempted cracked %d times since your last logon."
                      " Press [enter] and wait for the password prompt.",
                 PC_CRACK_ATTEMPTS(d->character));
         send_to_descriptor(buf, d);
         d->wait = MIN(30, PC_CRACK_ATTEMPTS(d->character)) * 2 * PULSE_SEC;
         return;
      }
      STATE(d)++;
   }

   if (STATE(d) == 2)
   {
      sprintf(buf, "Welcome back %s, please enter your password: ", UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
      send_to_descriptor(scriptwrap("PasswordOn()").c_str(), d);
      return;
   }

   sprintf(buf, "PasswordOff('%s', '%s')", UNIT_NAME(d->character), g_cServerConfig.m_mudname);
   send_to_descriptor(scriptwrap(buf).c_str(), d);

   if (str_is_empty(arg))
   {
      //UNIT_NAMES(d->character).Free();
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
   int nCmp;
   nCmp = pwdcompare(crypt(arg, PC_PWD(d->character)), PC_PWD(d->character), PC_MAX_PASSWORD);

   if (nCmp != 0)
   {
      if (!str_is_empty(arg))
      {
         slog(LOG_ALL, 0, "%s entered a wrong password [%s].",
              PC_FILENAME(d->character), d->host);
         PC_CRACK_ATTEMPTS(d->character)++;

         if ((td = find_descriptor(PC_FILENAME(d->character), d)))
         {
            send_to_descriptor("<br/>Someone just attempted to login under "
                               "your name using an illegal password.<br/>", td);
            PC_CRACK_ATTEMPTS(td->character)++;
            d->wait =
                PULSE_SEC * 5 + PC_CRACK_ATTEMPTS(td->character) * PULSE_SEC;
         }
         else if (!PC_IS_UNSAVED(d->character))
            save_player_file(d->character);
      }

      send_to_descriptor("<br/>Wrong password.<br/>", d);
      set_descriptor_fptr(d, nanny_close, TRUE);
      return;
   }

   PC_CRACK_ATTEMPTS(d->character) = 0;

   sprintf(buf, "<br/>Welcome back %s, you last visited %s on %s<br/>",
           UNIT_NAME(d->character), g_cServerConfig.m_mudname,
           ctime(&PC_TIME(d->character).connect));
   send_to_descriptor(buf, d);

   if ((td = find_descriptor(PC_FILENAME(d->character), d)))
   {
      set_descriptor_fptr(d, nanny_throw, TRUE);
      return;
   }

   /* See if player is in game (guests are not created in file entries) */
   /* Enters game (reconnects) if true                                  */
   for (u = unit_list; u; u = u->gnext)
   {
      if (!IS_PC(u))
         break;

      if (str_ccmp(PC_FILENAME(u), PC_FILENAME(d->character)) == 0)
      {
         //	  assert (!CHAR_DESCRIPTOR (u));
         //	  assert (UNIT_IN (u));

         UPC(u)->reconnect_game(d);
         return;
      }
   }

   /* Ok, he wasn't Link Dead, lets enter the game via menu */
   slog(LOG_BRIEF, CHAR_LEVEL(d->character), "%s[%s] has connected.",
        PC_FILENAME(d->character), d->host);

   send_to_descriptor("<br/>", d);
   set_descriptor_fptr(d, nanny_motd, TRUE);
}

void nanny_name_confirm(class descriptor_data *d, char *arg)
{
   if (STATE(d)++ == 0)
   {
      char buf[100];

      // MS: removed help option since it was not implemented.
      sprintf(buf, "Did I get that right, %s (Y/N)? ", UNIT_NAME(d->character));
      send_to_descriptor(buf, d);
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
      if (site_banned(d->host) != NO_BAN)
      {
         show_ban_text(d->host, d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }
      send_to_descriptor("New character.<br/>", d);
      set_descriptor_fptr(d, nanny_new_pwd, TRUE);
   }
   else if (*arg == 'n' || *arg == 'N')
   {
      if (PC_FILENAME(d->character))
         strcpy(PC_FILENAME(d->character), "");
      UNIT_NAMES(d->character).Free();
      send_to_descriptor("Ok, what IS it, then? ", d);
      set_descriptor_fptr(d, nanny_get_name, FALSE);
   }
   else
      send_to_descriptor("Please type Yes or No: ", d);
}

void nanny_get_name(class descriptor_data *d, char *arg)
{
   char tmp_name[100];
   class descriptor_data *td;

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
      class unit_data *ch;

      if (site_banned(d->host) == BAN_TOTAL)
      {
         show_ban_text(d->host, d);
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      /* Known player, lets load his character information. */
      ch = load_player(tmp_name);

      if (ch == NULL)
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

      CHAR_DESCRIPTOR(d->character) = NULL;
      extract_unit(d->character);

      CHAR_DESCRIPTOR(ch) = d;
      d->character = ch;

      if (wizlock && CHAR_LEVEL(d->character) < wizlock)
      {
         send_to_descriptor("Sorry, the game is wizlocked for "
                            "your level.<br/>",
                            d);
         slog(LOG_BRIEF, 0, "Wizlock lockout for %s.",
              PC_FILENAME(d->character));
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      PC_SETUP_COLOUR(d->character) = 0;
      MplexSendSetup(d);
      set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
      return;
   }
   else
   {
      /* Check for wizlock */
      if (wizlock)
      {
         send_to_descriptor("Sorry, no new players now, the game "
                            "is wizlocked!<br/>",
                            d);
         slog(LOG_BRIEF, 0, "Wizlock lockout for %s.", PC_FILENAME(d->character));
         set_descriptor_fptr(d, nanny_close, TRUE);
         return;
      }

      /* New player                                           */
      /* Check for both duplicate descriptors, and link death */

      /* all in lowercase... */
      strcpy(PC_FILENAME(d->character), tmp_name);

      CAPC(tmp_name);
      UNIT_NAMES(d->character).AppendName(tmp_name);
      strcpy(PC_PWD(d->character), "");

      /* If someone is connected, we borrow his pwd */
      if ((td = find_descriptor(tmp_name, d)))
      {
         // Only borrow a password if its already been set
         if (*PC_PWD(td->character))
         {
            strcpy(PC_PWD(d->character), PC_PWD(td->character));
            set_descriptor_fptr(d, nanny_existing_pwd, TRUE);
            return;
         }
      }

      /* Check for LD after PWD confirmation, this is due to */
      /* that the PWD is lost when a guests link is lost!    */

      set_descriptor_fptr(d, nanny_name_confirm, TRUE);
   }
}
