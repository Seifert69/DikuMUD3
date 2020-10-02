/*
 $Author: All $
 $RCSfile: interpreter.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.5 $
 */

#ifndef _MUD_INTERPRETER_H
#define _MUD_INTERPRETER_H

#include "dil.h"
extern struct command_info *cmdlist;
int char_is_playing(class unit_data *u);
int descriptor_is_playing(class descriptor_data *d);
void set_descriptor_fptr(class descriptor_data *d,
                         void (*fptr)(class descriptor_data *, char *),
                         ubit1 call);
void descriptor_interpreter(class descriptor_data *d, char *arg);
void interpreter_string_add(class descriptor_data *d, char *str);

struct spec_arg
{
    class unit_data *owner;     /* Who is this?                       */
    class unit_data *activator; /* Who performed the operation        */
    class unit_data *medium;    /* Possibly what is used in operation */
    class unit_data *target;    /* Possible target of operation       */

    struct command_info *cmd;
    class unit_fptr *fptr; /* The fptr is allowed to be modified, destroyed */

    int *pInt; /* Potential int to modify */
    const char *arg;
    ubit32 mflags; /* Would like to make constant, but then can't define.. */
};

struct command_info
{
    ubit8 combat_speed;  /* The speed of a combat command         */
    ubit8 combat_buffer; /* Use the combat speed / buffer system? */

    char *cmd_str;

    sbit32 no;

    ubit8 minimum_position;

    void (*cmd_fptr)(class unit_data *ch, char *arg,
                     const struct command_info *c);

    ubit8 minimum_level;
    ubit8 log_level; /* For logging certain immortal commands */

    struct diltemplate *tmpl; /* Perhaps a DIL template...         */
    ubit32 type;
    int inttype;
    int dir;
    struct command_info *next;
    struct command_info *prev;
    char *excmd;
    char *excmdc;
};

/* Bitmasks to determine what kind of messages is to be send
   to a special function. */

struct unit_function_array_type
{
    const char *name;
    int (*func)(struct spec_arg *sarg);
    ubit16 priority;
    int save_w_d; /* May it be saved if it has data? True/false */
    ubit16 sfb;   /* what kind of messages should be send */
    sbit16 tick;  /* Default tick count */
};

extern struct command_info cmd_auto_play;
extern struct command_info cmd_auto_leave;
extern struct command_info cmd_auto_enter;
extern struct command_info cmd_auto_tick;
extern struct command_info cmd_auto_extract;
extern struct command_info cmd_auto_death;
extern struct command_info cmd_auto_combat;
extern struct command_info cmd_auto_unknown;
extern struct command_info cmd_auto_save;
extern struct command_info cmd_auto_msg;
extern struct command_info cmd_auto_edit;
extern struct command_info cmd_auto_damage;
extern struct command_info cmd_a_social;

/* To check for commands by string */
ubit1 is_command(const struct command_info *cmd, const char *str);

/* Check to see if typed command is abbreviated */
ubit1 cmd_is_abbrev(class unit_data *ch, const struct command_info *cmd);

/* Interpreter routines */
void wrong_position(class unit_data *ch);
void command_interpreter(class unit_data *ch, const char *cmdArg);
void argument_interpreter(const char *argument,
                          char *first_arg, char *second_arg);
void half_chop(char *string, char *arg1, char *arg2);

/* The routine to check for special routines */

int unit_function_scan(class unit_data *u, struct spec_arg *sarg);
int function_activate(class unit_data *u, struct spec_arg *sarg);
#ifdef DMSERVER
int basic_special(class unit_data *ch, struct spec_arg *sarg, ubit16 mflt,
                  class unit_data *extra_target = NULL, const char *to = NULL);
#endif
int send_preprocess(class unit_data *ch, const struct command_info *cmd,
                    char *arg);
void send_done(class unit_data *activator, class unit_data *medium,
               class unit_data *target, int i,
               const struct command_info *cmd, const char *arg,
               class unit_data *extra_target = NULL, const char *to = NULL);
int send_ack(class unit_data *activator, class unit_data *medium,
             class unit_data *target, int *i,
             const struct command_info *cmd, const char *arg,
             class unit_data *extra_target = NULL, char *to = NULL);
int send_message(class unit_data *ch, char *arg);
int send_edit(class unit_data *ch, char *arg);
int send_death(class unit_data *ch);
int send_combat(class unit_data *ch);
int send_prompt(class unit_data *pc);
int send_save_to(class unit_data *from, class unit_data *to);

#include "spec_assign.h"

void do_snoop(class unit_data *, char *, const struct command_info *);
void do_rent(class unit_data *, char *, const struct command_info *);
void do_reset(class unit_data *, char *, const struct command_info *);
void do_kill(class unit_data *, char *, const struct command_info *);
void do_force(class unit_data *, char *, const struct command_info *);
void do_shutdown(class unit_data *, char *, const struct command_info *);
void do_execute(class unit_data *, char *, const struct command_info *);
void do_save(class unit_data *, char *, const struct command_info *);
void do_hit(class unit_data *, char *, const struct command_info *);
void do_set(class unit_data *, char *, const struct command_info *);
void do_setskill(class unit_data *, char *, const struct command_info *);
void do_wstat(class unit_data *, char *, const struct command_info *);
void do_wedit(class unit_data *, char *, const struct command_info *);
void do_load(class unit_data *, char *, const struct command_info *);
void do_cast(class unit_data *, char *, const struct command_info *);
void do_at(class unit_data *, char *, const struct command_info *);
void do_backstab(class unit_data *, char *, const struct command_info *);
void do_users(class unit_data *, char *, const struct command_info *);
void do_where(class unit_data *, char *, const struct command_info *);
void do_level(class unit_data *, char *, const struct command_info *);
void do_consider(class unit_data *, char *, const struct command_info *);
void do_ban(class unit_data *, char *, const struct command_info *);
void do_switch(class unit_data *, char *, const struct command_info *);
void do_change(class unit_data *, char *, const struct command_info *);
void do_color(class unit_data *, char *, const struct command_info *);
void do_timewarp(class unit_data *, char *, const struct command_info *);
void do_crash(class unit_data *, char *, const struct command_info *);
void do_wizlock(class unit_data *, char *, const struct command_info *);
void do_account(class unit_data *, char *, const struct command_info *);
void do_move(class unit_data *, char *, const struct command_info *);


#endif /* _MUD_INTERPRETER_H */
