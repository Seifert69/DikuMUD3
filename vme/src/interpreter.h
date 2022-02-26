/*
 $Author: All $
 $RCSfile: interpreter.h,v $
 $Date: 2004/03/20 06:13:22 $
 $Revision: 2.5 $
 */
#pragma once

#include "essential.h"
#include "structs.h"
#include "trie.h"

#include <vme.h>

class command_info;

struct spec_arg
{
    unit_data *owner;     /* Who is this?                       */
    unit_data *activator; /* Who performed the operation        */
    unit_data *medium;    /* Possibly what is used in operation */
    unit_data *target;    /* Possible target of operation       */

    command_info *cmd;
    unit_fptr *fptr; /* The fptr is allowed to be modified, destroyed */

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

    void (*cmd_fptr)(unit_data *ch, char *arg, const command_info *c);

    ubit8 minimum_level;
    ubit8 log_level; /* For logging certain immortal commands */

    diltemplate *tmpl; /* Perhaps a DIL template...         */
    ubit32 type;
    int inttype;
    int dir;
    command_info *next;
    command_info *prev;
    char *excmd;
    char *excmdc;
};

/* Bitmasks to determine what kind of messages is to be send
   to a special function. */

struct unit_function_array_type
{
    const char *name;
    int (*func)(spec_arg *sarg);
    ubit16 priority;
    int save_w_d; /* May it be saved if it has data? True/false */
    ubit16 sfb;   /* what kind of messages should be send */
    sbit16 tick;  /* Default tick count */
};

/* To check for commands by string */
ubit1 is_command(const command_info *cmd, const char *str);

/* Check to see if typed command is abbreviated */
ubit1 cmd_is_abbrev(unit_data *ch, const command_info *cmd);

/* Interpreter routines */
void wrong_position(unit_data *ch);
void command_interpreter(unit_data *ch, const char *cmdArg);
void argument_interpreter(const char *argument, char *first_arg, char *second_arg);

/* The routine to check for special routines */

int unit_function_scan(unit_data *u, spec_arg *sarg);
int function_activate(unit_data *u, spec_arg *sarg);
#ifdef DMSERVER
int basic_special(unit_data *ch, spec_arg *sarg, ubit16 mflt, unit_data *extra_target = nullptr, const char *to = nullptr);
#endif
int send_preprocess(unit_data *ch, const command_info *cmd, char *arg);
void send_done(unit_data *activator,
               unit_data *medium,
               unit_data *target,
               int i,
               const command_info *cmd,
               const char *arg,
               unit_data *extra_target = nullptr,
               const char *to = nullptr);
int send_ack(unit_data *activator,
             unit_data *medium,
             unit_data *target,
             int *i,
             const command_info *cmd,
             const char *arg,
             unit_data *extra_target = nullptr,
             char *to = nullptr);
int send_message(unit_data *ch, char *arg);
int send_edit(unit_data *ch, const char *arg);
int send_death(unit_data *ch);
int send_combat(unit_data *ch);
int send_prompt(unit_data *pc);
int send_save_to(unit_data *from, unit_data *to);
void assign_command_pointers();
void boot_interpreter();
void interpreter_dil_check();
int char_is_playing(unit_data *u);
int descriptor_is_playing(descriptor_data *d);
void descriptor_interpreter(descriptor_data *d, char *arg);

extern command_info *g_cmd_follow;
extern command_info *g_cmd_dirs[MAX_EXIT + 1];
extern command_info g_cmd_auto_play;
extern command_info g_cmd_auto_leave;
extern command_info g_cmd_auto_enter;
extern command_info g_cmd_auto_tick;
extern command_info g_cmd_auto_extract;
extern command_info g_cmd_auto_death;
extern command_info g_cmd_auto_combat;
extern command_info g_cmd_auto_unknown;
extern command_info g_cmd_auto_save;
extern command_info g_cmd_auto_msg;
extern command_info g_cmd_auto_edit;
extern command_info g_cmd_auto_damage;
extern trie_type *g_intr_trie;
