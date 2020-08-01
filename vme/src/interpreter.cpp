/*
 $Author: All $
 $RCSfile: interpreter.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.7 $
 */

#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "textutil.h"
#include "comm.h"
#include "interpreter.h"
#include "db.h"
#include "utils.h"
#include "vmelimits.h"
#include "skills.h"
#include "trie.h"
#include "utility.h"
#include "files.h"
#include "common.h"
#include "constants.h"
#include "unitfind.h"
#include "dilrun.h"
#include "cmdload.h"

/* external fcntls */
extern struct unit_function_array_type unit_function_array[];

struct trie_type *intr_trie = NULL;

struct command_info cmd_auto_tick = {
    0,
    0,
    NULL,
    CMD_AUTO_TICK,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_enter = {
    0,
    0,
    NULL,
    CMD_AUTO_ENTER,
    POSITION_STANDING,
    NULL,
    0,
};

struct command_info cmd_auto_play = {
    0,
    0,
    NULL,
    CMD_AUTO_PLAY,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_leave = {
    0,
    0,
    NULL,
    CMD_AUTO_LEAVE,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_extract = {
    0,
    0,
    NULL,
    CMD_AUTO_EXTRACT,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_death = {
    0,
    0,
    NULL,
    CMD_AUTO_DEATH,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_combat = {
    0,
    0,
    NULL,
    CMD_AUTO_COMBAT,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_unknown = {
    0,
    0,
    NULL,
    CMD_AUTO_UNKNOWN,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_save = {
    0,
    0,
    NULL,
    CMD_AUTO_SAVE,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_msg = {
    0,
    0,
    NULL,
    CMD_AUTO_MSG,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_edit = {
    0,
    0,
    NULL,
    CMD_AUTO_EDIT,
    POSITION_DEAD,
    NULL,
    0,
};

struct command_info cmd_auto_damage = {
    0, 0, NULL, CMD_AUTO_DAMAGE, POSITION_DEAD, NULL, 0};

struct command_info *cmd_follow = NULL;

struct command_info *cmd_dirs[MAX_EXIT+1];


void wrong_position(class unit_data *ch)
{
    static const char *strings[] = {
        "Lie still; you are DEAD!!! :-( <br/>",                               /* Dead     */
        "You are in a pretty bad shape, unable to do anything!<br/>",         /* Mortally */
        "You are in a pretty bad shape, unable to do anything!<br/>",         /* Incap    */
        "All you can do right now, is think about the stars!<br/>",           /* Stunned  */
        "In your dreams, or what?<br/>",                                      /* Sleeping */
        "You feel too relaxed to do that, perhaps you should stand up?<br/>", /*Rst */
        "Maybe you should get on your feet first?<br/>",                      /* Sitting  */
        "No way! You are fighting for your life!<br/>"                        /* Fighting */
    };

    if (CHAR_POS(ch) < POSITION_STANDING)
        send_to_char(strings[CHAR_POS(ch)], ch);
}

#ifdef DEBUG_HISTORY

#define MAX_DEBUG_HISTORY 20

class command_hist
{
public:
    command_hist(void)
    {
        str[0] = pcname[0] = 0;
        fi = NULL;
    }

    char str[MAX_INPUT_LENGTH + 50];
    char pcname[PC_MAX_NAME + 1];
    class file_index_type *fi;
}

command_history_data[MAX_DEBUG_HISTORY];

static int command_history_pos = 0;

static void
add_command_history(class unit_data *u, char *str)
{
    if (IS_PC(u))
        strcpy(command_history_data[command_history_pos].pcname, UNIT_NAME(u));
    else
        command_history_data[command_history_pos].pcname[0] = 0;

    strcpy(command_history_data[command_history_pos].str, str);
    command_history_data[command_history_pos].fi = UNIT_FILE_INDEX(u);

    command_history_pos = (command_history_pos + 1) % MAX_DEBUG_HISTORY;
}

static void
dump_command_history(void)
{
    int i, j;
    for (j = 0, i = command_history_pos; j < MAX_DEBUG_HISTORY; j++)
    {
        slog(LOG_ALL, 0, "CMD %s@%s [%s]",
             command_history_data[i].pcname ? command_history_data[i].pcname : FI_NAME(command_history_data[i].fi),
             command_history_data[i].pcname ? "" : FI_ZONENAME(command_history_data[i].fi),
             command_history_data[i].str);
        i = (i + 1) % MAX_DEBUG_HISTORY;
    }
}

class func_hist
{
public:
    func_hist(void)
    {
        idx = 0;
        flags = 0;
        fi = NULL;
    }
    ubit16 idx;
    ubit16 flags;
    class file_index_type *fi;
}

func_history_data[MAX_DEBUG_HISTORY];

static int func_history_pos = 0;

void add_func_history(class unit_data *u, ubit16 idx, ubit16 flags)
{
    func_history_data[func_history_pos].idx = idx;
    func_history_data[func_history_pos].flags = flags;
    func_history_data[func_history_pos].fi = UNIT_FILE_INDEX(u);

    func_history_pos = (func_history_pos + 1) % MAX_DEBUG_HISTORY;
}

static void
dump_func_history(void)
{
    int i, j;
    char buf2[512];

    for (j = 0, i = func_history_pos; j < MAX_DEBUG_HISTORY; j++)
    {
        slog(LOG_ALL, 0, "FUNC %s@%s: '%s (%d) %s (%d)'",
             FI_NAME(func_history_data[i].fi),
             FI_ZONENAME(func_history_data[i].fi),
             unit_function_array[func_history_data[i].idx].name,
             func_history_data[i].idx,
             sprintbit(buf2, func_history_data[i].flags, sfb_flags),
             func_history_data[i].flags);
        i = (i + 1) % MAX_DEBUG_HISTORY;
    }
}

void dump_debug_history(void)
{
    dump_command_history();
    dump_func_history();
}
#endif

void command_interpreter(class unit_data *ch, const char *cmdArg)
{
    char cmd[MAX_INPUT_LENGTH + 10];
    char excmd[MAX_INPUT_LENGTH + 10];
    char argstr[MAX_INPUT_LENGTH + 10];

    int is_say, is_emote;
    struct command_info *cmd_ptr;
    is_say = FALSE;
    is_emote = FALSE;
    assert(IS_CHAR(ch));

    if (ch->is_destructed())
        return;

    if (IS_PC(ch) && CHAR_DESCRIPTOR(ch) && CHAR_DESCRIPTOR(ch)->editing)
        return;

    if (strlen(cmdArg) > MAX_INPUT_LENGTH)
    {
        slog(LOG_ALL, 0, "%s issued command which was too long: %s",
             UNIT_NAME(ch), cmdArg);
        send_to_char("The issued command was aborted because it was too long, "
                     "please report how you made this happen.<br/>",
                     ch);
        return;
    }

    char cmdCPY[MAX_INPUT_LENGTH + 1];
    strncpy(cmdCPY, cmdArg, MAX_INPUT_LENGTH);
    cmdCPY[MAX_INPUT_LENGTH] = 0;
    char *arg = cmdCPY;

    /* Find first non blank */
    arg = skip_spaces(arg);

    if (CHAR_DESCRIPTOR(ch))
    {
        if (*arg == '!')
        {
            arg = CHAR_DESCRIPTOR(ch)->history;
        }
        else
        {
            if (*arg == '\'')
            {
                is_say = TRUE;
                *arg = ' ';
                strcpy(CHAR_DESCRIPTOR(ch)->history, arg);
            }
            else
            {
                if (*arg == ',')
                {
                    is_emote = TRUE;
                    *arg = ' ';
                    strcpy(CHAR_DESCRIPTOR(ch)->history, arg);
                }
                else
                {
                    strcpy(CHAR_DESCRIPTOR(ch)->history, arg);
                }
            }
        }
    }
#ifdef DEBUG_HISTORY
    add_command_history(ch, arg);
#endif

    if (is_say)
        strcpy(excmd, "say");
    else if (is_emote)
        strcpy(excmd, "emote");
    else
        arg = str_next_word_copy(arg, excmd);

    strcpy(cmd, excmd);
    str_lower(cmd);

    if (CHAR_DESCRIPTOR(ch))
        strcpy(CHAR_DESCRIPTOR(ch)->last_cmd, cmd);

    if (!cmd[0])
    {
        return;
    }
    strncpy(argstr, skip_blanks(arg),
            MAX(0, MAX_INPUT_LENGTH - 1 - strlen(cmd)));
    argstr[MAX(0, MAX_INPUT_LENGTH - 1 - strlen(cmd))] = 0;

    strip_trailing_spaces(argstr);

    if ((cmd_ptr = (struct command_info *)search_trie(cmd, intr_trie)) ==
        NULL)
    {
        struct command_info the_cmd =
            {0, 0, NULL, CMD_AUTO_UNKNOWN, POSITION_DEAD, NULL, 0};

        the_cmd.cmd_str = str_dup(cmd);
        the_cmd.excmd = str_dup(cmd);
        the_cmd.excmdc = str_dup(excmd);

        if (send_preprocess(ch, &the_cmd, argstr) == SFR_SHARE)
            act("$2t is not a known command.", A_ALWAYS, ch, cmd, NULL, TO_CHAR);

        if (the_cmd.cmd_str)
            FREE(the_cmd.cmd_str);
        if (the_cmd.excmd)
            FREE(the_cmd.excmd);
        if (the_cmd.excmdc)
            FREE(the_cmd.excmdc);
        return;
    }

    if (cmd_ptr->combat_buffer && CHAR_COMBAT(ch))
    {
        if (CHAR_COMBAT(ch)->When() >= SPEED_DEFAULT)
        {
            CHAR_COMBAT(ch)->setCommand(cmdArg);
            return;
        }
        else
            CHAR_COMBAT(ch)->changeSpeed(cmd_ptr->combat_speed);
    }

    if (*cmd)
    {
        if (cmd_ptr->excmd)
            FREE(cmd_ptr->excmd);
        cmd_ptr->excmd = str_dup(cmd);
        if (cmd_ptr->excmdc)
            FREE(cmd_ptr->excmdc);
        cmd_ptr->excmdc = str_dup(excmd);
    }
    if (send_preprocess(ch, cmd_ptr, argstr) != SFR_SHARE)
    {
        if (cmd_ptr->excmd)
            FREE(cmd_ptr->excmd);
        if (cmd_ptr->excmdc)
            FREE(cmd_ptr->excmdc);
        return;
    }
    /*else {
    		  if (cmd_ptr->excmd)
    			 FREE(cmd_ptr->excmd);
    		  if (cmd_ptr->excmdc)
    			 FREE(cmd_ptr->excmdc);
    	  }
    */
    if (ch->is_destructed())
    {
        return;
    }

    if (CHAR_LEVEL(CHAR_ORIGINAL(ch)) < cmd_ptr->minimum_level)
    {
        if (cmd_ptr->minimum_level >= 200)
            send_to_char("Arglebargle, glop-glyf!?!<br/>", ch);
        else
            send_to_char("Sorry, this command is not available at your level.<br/>", ch);
        if (cmd_ptr->excmd)
            FREE(cmd_ptr->excmd);
        if (cmd_ptr->excmdc)
            FREE(cmd_ptr->excmdc);
        return;
    }

    if (CHAR_POS(ch) < cmd_ptr->minimum_position)
    {
        wrong_position(ch);
        if (cmd_ptr->excmd)
            FREE(cmd_ptr->excmd);
        if (cmd_ptr->excmdc)
            FREE(cmd_ptr->excmdc);
        return;
    }

    if (cmd_ptr->log_level)
        slog(LOG_ALL, MAX(CHAR_LEVEL(ch), cmd_ptr->log_level),
             "CMDLOG %s: %s %s", UNIT_NAME(ch), cmd_ptr->cmd_str, argstr);

    if (cmd_ptr->tmpl)
    {
        class dilprg *prg;

        prg = dil_copy_template(cmd_ptr->tmpl, ch, NULL);
        prg->waitcmd = WAITCMD_MAXINST - 1; // The usual hack, see db_file

        prg->fp->vars[0].val.string = str_dup(argstr);
        dil_activate_cmd(prg, cmd_ptr);
    }
    else if (cmd_ptr->cmd_fptr)
        ((*cmd_ptr->cmd_fptr)(ch, argstr, cmd_ptr));
    else
    {
        if (IS_MORTAL(ch))
            send_to_char("Arglebargle, glop-glyf!?!<br/>", ch);
        else
            send_to_char("Sorry, that command is not yet implemented...<br/>",
                         ch);
    }
    if (cmd_ptr->excmd)
        FREE(cmd_ptr->excmd);
    if (cmd_ptr->excmdc)
        FREE(cmd_ptr->excmdc);
}

int char_is_playing(class unit_data *u)
{
    return UNIT_IN(u) != NULL;
}

int descriptor_is_playing(class descriptor_data *d)
{
    return d && d->character && char_is_playing(d->character);
}

void descriptor_interpreter(class descriptor_data *d, char *arg)
{
    assert(d->character);
    command_interpreter(d->character, arg);
}

/* Check to see if the full command was typed */
ubit1 cmd_is_abbrev(class unit_data *ch, const struct command_info *cmd)
{
    return CHAR_DESCRIPTOR(ch) && str_ccmp(CHAR_DESCRIPTOR(ch)->last_cmd, cmd->cmd_str);
}

/* To check for commands by string */
ubit1 is_command(const struct command_info *cmd, const char *str)
{
    if ((cmd->no == CMD_AUTO_UNKNOWN))
    {
        return cmd->cmd_str && is_abbrev(cmd->cmd_str, str);
    }
    else
    {
        struct command_info *cmd_ptr =
            (struct command_info *)search_trie(str, intr_trie);

        return (cmd_ptr == cmd);
    }
}

void argument_interpreter(const char *argument, char *first_arg, char *second_arg)
{
    argument = (char *)one_argument(argument, first_arg);

    one_argument(argument, second_arg);
}

int function_activate(class unit_data *u, struct spec_arg *sarg)
{
    if ((u != sarg->activator) ||
        IS_SET(sarg->fptr->flags, SFB_AWARE) ||
        IS_SET(sarg->mflags, SFB_AWARE))
    {
        if (IS_SET(sarg->fptr->flags, sarg->mflags & (~SFB_AWARE | SFB_PROMPT))) //MS2020 added ()
        {
#ifdef DEBUG_HISTORY
            add_func_history(u, sarg->fptr->index, sarg->mflags);
#endif
            assert(!sarg->fptr->is_destructed());
            if (unit_function_array[sarg->fptr->index].func)
                return (*(unit_function_array[sarg->fptr->index].func))(sarg);
            else
                slog(LOG_ALL, 0, "Interpreter: Null function call! (%d)",
                     sarg->fptr->index);
        }
    }
    return SFR_SHARE;
}

/* u is the owner of the function on which the scan is performed */
/* This function sets the 'sarg->fptr' and 'sarg->owner'         */
int unit_function_scan(class unit_data *u, struct spec_arg *sarg)
{
    int res = SFR_SHARE;
    int priority = 0;
    ubit16 orgflag;
    class unit_fptr *next;

    if (g_cServerConfig.m_bNoSpecials)
        return SFR_SHARE;

    assert(u);

    sarg->owner = u;

    //   if (IS_PC(u) && !char_is_playing(u))
    //     return SFR_SHARE;

    for (sarg->fptr = UNIT_FUNC(u); !priority && sarg->fptr; sarg->fptr = next)
    {
        next = sarg->fptr->next; /* Next dude trick */
        orgflag = sarg->fptr->flags;

        if (u->is_destructed())
            return SFR_SHARE;

        if (sarg->fptr->is_destructed())
            continue;

        res = function_activate(u, sarg);

        if ((orgflag != sarg->fptr->flags) && (sarg->fptr->index == SFUN_DIL_INTERNAL))
        {
            void SetFptrTimer(class unit_data * u, class unit_fptr * fptr);
            int diltick, i;
            diltick = FALSE;
            if (IS_SET(sarg->fptr->flags, SFB_TICK))
                diltick = TRUE;
            else if (sarg->fptr->data)
            {
                register class dilprg *prg = (class dilprg *)sarg->fptr->data;
                for (i = 0; i < prg->fp->intrcount; i++)
                    if (IS_SET(prg->fp->intr[i].flags, SFB_TICK))
                        diltick = TRUE;
            }
            if (diltick)
                SetFptrTimer(u, sarg->fptr);
        }

        if (res != SFR_SHARE)
            return res;

        priority |= IS_SET(sarg->fptr->flags, SFB_PRIORITY);
    }

    return SFR_SHARE;
}

/* TODO: Specials may cause movement of one unit to another unit.
   All next-dude tricks below are therefore not good enough,
   they ought to check if the nextdude is still in the same
   location before activating it ?! */

/* Explanation of the spec_arg stuff:

   owner is set by unit_function_scan and points to the unit which has
   been activated (self / this)

   [activator] the unit which caused this message.
   [medium]    the unit which was used as a medium of the operation.
   [target]    the unit which was the target of the operation.
   [pInt]      possible value which was used in the operation.

   This function sets the sarg->mflags to equal mflt.
   if extra_target is set, then also send message to that unit
*/

int basic_special(class unit_data *ch, struct spec_arg *sarg, ubit16 mflt,
                  class unit_data *extra_target, const char *to)
{
    register class unit_data *u, *uu, *next, *nextt, *tou;
    extern class unit_data *unit_list;
    class file_index_type *fi;

    if (to)
    {
        ch = NULL;
        if ((fi = str_to_file_index(to)))
            for (tou = unit_list; tou; tou = tou->gnext)
                if (UNIT_FILE_INDEX(tou) == fi)
                    ch = tou;

        if (ch == NULL)
            return SFR_SHARE;
    }

    sarg->mflags = mflt;
    if (IS_PC(ch) && !UNIT_IN(ch))
    {
        unit_function_scan(ch, sarg);
        return SFR_BLOCK;
    }

    if (IS_ROOM(ch))
    {
        if (UNIT_FUNC(ch) && (unit_function_scan(ch, sarg)) != SFR_SHARE)
            return SFR_BLOCK;

        for (u = UNIT_CONTAINS(ch); u; u = next)
        {
            next = u->next; /* Next dude trick */
            if (UNIT_FUNC(u) && (unit_function_scan(u, sarg)) != SFR_SHARE)
                return SFR_BLOCK;
        }
        return SFR_SHARE;
    }

    if (extra_target && !same_surroundings(ch, extra_target))
        if ((unit_function_scan(extra_target, sarg)) != SFR_SHARE)
            return SFR_BLOCK;

    /* special in room? */
    if (UNIT_IN(ch) && UNIT_FUNC(UNIT_IN(ch)))
    {
        if ((unit_function_scan(UNIT_IN(ch), sarg)) != SFR_SHARE)
            return SFR_BLOCK;
    }

    /* special in inventory or equipment? */
    for (u = UNIT_CONTAINS(ch); u; u = next)
    {
        next = u->next; /* Next dude trick */
        if (UNIT_FUNC(u) && (unit_function_scan(u, sarg)) != SFR_SHARE)
            return SFR_BLOCK;
    }

    if (UNIT_IN(ch))
    {
        /* special in room present? */
        for (u = UNIT_CONTAINS(UNIT_IN(ch)); u; u = next)
        {
            next = u->next; /* Next dude trick */

            if (UNIT_FUNC(u) && (unit_function_scan(u, sarg)) != SFR_SHARE)
                return SFR_BLOCK;

            if (u != ch)
            {
                if (UNIT_IS_TRANSPARENT(u))
                {
                    for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
                    {
                        nextt = uu->next; /* next dude double trick */
                        if (UNIT_FUNC(uu) &&
                            (unit_function_scan(uu, sarg)) != SFR_SHARE)
                            return SFR_BLOCK;
                    }
                }
                else if (IS_CHAR(u))
                {
                    /* in equipment? */
                    for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
                    {
                        nextt = uu->next; /* Next dude trick */
                        if (UNIT_FUNC(uu) && IS_OBJ(uu) && OBJ_EQP_POS(uu) &&
                            (unit_function_scan(uu, sarg) != SFR_SHARE))
                            return SFR_BLOCK;
                    }
                }
            }
        }

        /* specials outside room */
        if (UNIT_IN(ch) && UNIT_IS_TRANSPARENT(UNIT_IN(ch)) && UNIT_IN(UNIT_IN(ch)))
        {
            /* special in outside room? */
            if (UNIT_FUNC(UNIT_IN(UNIT_IN(ch))))
            {
                if (unit_function_scan(UNIT_IN(UNIT_IN(ch)), sarg) !=
                    SFR_SHARE)
                    return SFR_BLOCK;
            }

            if (UNIT_IN(UNIT_IN(ch)))
            {
                for (u = UNIT_CONTAINS(UNIT_IN(UNIT_IN(ch))); u; u = next)
                {
                    next = u->next; /* Next dude trick */

                    /* No self activation except when dying... */
                    if (UNIT_FUNC(u) &&
                        (unit_function_scan(u, sarg)) != SFR_SHARE)
                        return SFR_BLOCK;

                    if (!UNIT_IN(UNIT_IN(ch)))
                        break;

                    if (u != UNIT_IN(ch))
                    {
                        if (UNIT_IS_TRANSPARENT(u))
                        {
                            for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
                            {
                                nextt = uu->next; /* next dude double trick */
                                if (UNIT_FUNC(uu) &&
                                    (unit_function_scan(uu, sarg)) !=
                                        SFR_SHARE)
                                    return SFR_BLOCK;
                            }
                        }
                        else if (IS_CHAR(u))
                        {
                            /* in equipment? */
                            for (uu = UNIT_CONTAINS(u); uu; uu = nextt)
                            {
                                nextt = uu->next; /* Next dude trick */
                                if (UNIT_FUNC(uu) && IS_OBJ(uu) && OBJ_EQP_POS(uu) && (unit_function_scan(uu, sarg) != SFR_SHARE))
                                    return SFR_BLOCK;
                            }
                        }
                    }
                }
            }
        }
    }
    return SFR_SHARE;
}

/* Preprocessed commands */
int send_preprocess(class unit_data *ch, const struct command_info *cmd,
                    char *arg)
{
    struct spec_arg sarg;

    sarg.activator = ch;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.cmd = (struct command_info *)cmd;
    sarg.arg = arg;

    return basic_special(ch, &sarg, SFB_CMD);
}

int send_edit(class unit_data *ch, char *arg)
{
    struct spec_arg sarg;

    sarg.activator = ch;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.cmd = &cmd_auto_edit;
    sarg.arg = arg;

    return basic_special(ch, &sarg, SFB_EDIT);
}

int send_message(class unit_data *ch, char *arg)
{
    struct spec_arg sarg;

    sarg.activator = ch;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.cmd = &cmd_auto_msg;
    sarg.arg = arg;

    return basic_special(ch, &sarg, SFB_MSG);
}

int send_death(class unit_data *ch)
{
    struct spec_arg sarg;

    sarg.activator = ch;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.cmd = &cmd_auto_death;
    sarg.arg = "";

    return basic_special(ch, &sarg, SFB_DEAD | SFB_AWARE);
}

int send_combat(class unit_data *ch)
{
    struct spec_arg sarg;

    sarg.activator = ch;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.cmd = &cmd_auto_combat;
    sarg.arg = "";

    return basic_special(ch, &sarg, SFB_COM);
}

int send_save_to(class unit_data *from, class unit_data *to)
{
    struct spec_arg sarg;

    assert(to);
    assert(from);

    sarg.activator = from;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.fptr = NULL; /* Set by unit_function_scan */
    sarg.cmd = &cmd_auto_save;
    sarg.arg = "";
    sarg.mflags = SFB_SAVE;

    return unit_function_scan(to, &sarg);
}

int send_prompt(class unit_data *pc)
{
    struct spec_arg sarg;

    sarg.activator = pc;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;
    sarg.fptr = NULL; /* Set by unit_function_scan */
    sarg.cmd = &cmd_auto_tick;
    sarg.arg = "";
    sarg.mflags = SFB_PROMPT | SFB_AWARE;

    return unit_function_scan(pc, &sarg);
}

int send_ack(class unit_data *activator, class unit_data *medium,
             class unit_data *target,
             int *i,
             const struct command_info *cmd,
             const char *arg, class unit_data *extra_target, char *to)
{
    struct spec_arg sarg;
    int j = 0;

    sarg.activator = activator;
    sarg.medium = medium;
    sarg.target = target;

    if (i)
        sarg.pInt = i;
    else
        sarg.pInt = &j;

    sarg.cmd = (struct command_info *)cmd;
    sarg.arg = (char *)arg;

    if (to)
        return basic_special(NULL, &sarg, SFB_PRE, extra_target, to);
    else
        return basic_special(activator, &sarg, SFB_PRE, extra_target, to);
}

void send_done(class unit_data *activator, class unit_data *medium,
               class unit_data *target, int i, const struct command_info *cmd,
               const char *arg, class unit_data *extra_target, const char *to)
{
    struct spec_arg sarg;

    sarg.activator = activator;
    sarg.medium = medium;
    sarg.target = target;
    sarg.pInt = &i;
    sarg.cmd = (struct command_info *)cmd;
    sarg.arg = (char *)arg;

    if (to)
        basic_special(NULL, &sarg, SFB_DONE, extra_target, to);
    else
        basic_special(activator, &sarg, SFB_DONE, extra_target, to);
}

/* Build the trie here :) */
void assign_command_pointers(void)
{
    struct command_info *cmd;
    intr_trie = 0;
    for (cmd = cmdlist; cmd; cmd = cmd->next)
        intr_trie = add_trienode(cmd->cmd_str, intr_trie);

    qsort_triedata(intr_trie);

    for (cmd = cmdlist; cmd; cmd = cmd->next)
        set_triedata(cmd->cmd_str, intr_trie, cmd, FALSE);

    cmd_follow = (struct command_info *)search_trie("follow", intr_trie);

    cmd = (struct command_info *)search_trie("north", intr_trie);
    cmd->no = DIR_NORTH;
    cmd_dirs[DIR_NORTH] = cmd;

    cmd = (struct command_info *)search_trie("northeast", intr_trie);
    cmd->no = DIR_NORTHEAST;
    cmd_dirs[DIR_NORTHEAST] = cmd;
    cmd = (struct command_info *)search_trie("ne", intr_trie);
    cmd->no = DIR_NORTHEAST;

    cmd = (struct command_info *)search_trie("east", intr_trie);
    cmd->no = DIR_EAST;
    cmd_dirs[DIR_EAST] = cmd;

    cmd = (struct command_info *)search_trie("southeast", intr_trie);
    cmd->no = DIR_SOUTHEAST;
    cmd_dirs[DIR_SOUTHEAST] = cmd;

    cmd = (struct command_info *)search_trie("se", intr_trie);
    cmd->no = DIR_SOUTHEAST;

    cmd = (struct command_info *)search_trie("south", intr_trie);
    cmd->no = DIR_SOUTH;
    cmd_dirs[DIR_SOUTH] = cmd;

    cmd = (struct command_info *)search_trie("southwest", intr_trie);
    cmd->no = DIR_SOUTHWEST;
    cmd_dirs[DIR_SOUTHWEST] = cmd;

    cmd = (struct command_info *)search_trie("sw", intr_trie);
    cmd->no = DIR_SOUTHWEST;

    cmd = (struct command_info *)search_trie("west", intr_trie);
    cmd->no = DIR_WEST;
    cmd_dirs[DIR_WEST] = cmd;

    cmd = (struct command_info *)search_trie("northwest", intr_trie);
    cmd->no = DIR_NORTHWEST;
    cmd_dirs[DIR_NORTHWEST] = cmd;

    cmd = (struct command_info *)search_trie("nw", intr_trie);
    cmd->no = DIR_NORTHWEST;
    cmd = (struct command_info *)search_trie("up", intr_trie);
    cmd->no = DIR_UP;
    cmd_dirs[DIR_UP] = cmd;

    cmd = (struct command_info *)search_trie("down", intr_trie);
    cmd->no = DIR_DOWN;
    cmd_dirs[DIR_DOWN] = cmd;
}

void interpreter_dil_check(void)
{
    struct command_info *cmd;
    for (cmd = cmdlist; cmd; cmd = cmd->next)
    {
        if (cmd->tmpl == NULL)
            continue;

        if (cmd->tmpl->argc != 1)
        {
            slog(LOG_ALL, 0, "Interpreter DIL %s expected 1 argument.",
                 cmd->tmpl->prgname);
            cmd->tmpl = NULL;
            continue;
        }

        if (cmd->tmpl->argt[0] != DILV_SP)
        {
            slog(LOG_ALL, 0, "Interpreter DIL %s argument 1 mismatch.",
                 cmd->tmpl->prgname);
            cmd->tmpl = NULL;
            continue;
        }
    }
}

void boot_interpreter(void)
{
    cmd_base_load();
}
