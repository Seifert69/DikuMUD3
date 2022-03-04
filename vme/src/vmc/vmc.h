/*
 $Author: All $
 $RCSfile: vmc.h,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.1 $
 */
#pragma once

#include "utils.h"
#define VERSION "2.0"

#define MAX_STRING 4096
#define MAX_SYMBOL 21

#define SYMSIZE 40

#define INPUT_SUFFIX "zon"
#define OUTPUT_RSUFFIX "reset"
#define OUTPUT_WSUFFIX "data"

#ifdef CPP
    #undef CPP
#endif

/* I hate that you can't get `gcc -E' to write to stdout... /gnort */

#if defined(AMIGA)
    #define CPP "gnu:lib/gcc-lib/mc68020-cbm-amigados/2.6.3/cpp"
#else
    #define CPP "cpp"
#endif

#ifndef CPPI
    #define CPPI "."
#endif

#ifndef ZCAT
    #define ZCAT "zcat" /* filter to use with .Z-suffixes */
#endif

#ifndef CAT
    #define CAT "cat"
#endif

struct zone_header
{
    char *title;
    char *name;
    unsigned short lifespan;
    unsigned char reset_mode;
    char **creators;
    char *notes;
    char *help;
    int weather;
};

class reset_command;

struct zone_info
{
    unit_data *z_rooms;
    unit_data *z_mobiles;
    unit_data *z_objects;
    reset_command *z_table;
    zone_header z_zone;
    diltemplate *z_tmpl;
};

struct reset_command
{
    char cmd_no;
    char *ref1;
    char *ref2;
    sbit16 num1;
    sbit16 num2;
    sbit16 num3;
    char direction;
    char cmpl;
    reset_command *next;
};

unit_data *mcreate_unit(int type);
room_direction_data *mcreate_exit();
unit_affected_type *mcreate_affect();
void *mmalloc(int size);
void mem_reset();
void mem_init();

void ShowUsage(char *name);
void fix(char *file);

#define UNIT_IDENT(unit) ((char *)(unit)->gnext) /* can you say 'filth'? */
/* You betcha... */
#define UNIT_IDENT_ASSIGN(unit, val) ((unit)->gnext = (class unit_data *)(val))

extern char **g_tmplnames;
extern const char *g_error_zone_name;
extern int g_errcon;
extern int g_fatal_warnings;
extern int g_verbose;
extern int g_nooutput;
extern int g_make;
extern bool g_quiet_compile;
