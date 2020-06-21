/*
 $Author: All $
 $RCSfile: vmc.h,v $
 $Date: 2003/12/31 01:21:59 $
 $Revision: 2.1 $
 */

#ifndef _MUD_VMC_H
#define _MUD_VMC_H

#include "utils.h"
#define VERSION "2.0"

#define MAX_STRING 4096
#define MAX_SYMBOL 21

#define SYMSIZE 40

#define INPUT_SUFFIX      "zon"
#define OUTPUT_RSUFFIX    "reset"
#define OUTPUT_WSUFFIX    "data"

#ifdef CPP
#undef CPP
#endif

/* I hate that you can't get `gcc -E' to write to stdout... /gnort */

#if defined(AMIGA)
#define CPP               "gnu:lib/gcc-lib/mc68020-cbm-amigados/2.6.3/cpp"
#else
#define CPP               "cpp"
#endif

#ifndef CPPI
#define CPPI              "."
#endif

#ifndef ZCAT
#define ZCAT              "zcat" /* filter to use with .Z-suffixes */
#endif

#ifndef CAT
#define CAT               "cat"
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

struct zone_info
{
    class unit_data *z_rooms;
    class unit_data *z_mobiles;
    class unit_data *z_objects;
    struct reset_command *z_table;
    struct zone_header z_zone;
    struct diltemplate *z_tmpl;
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
    struct reset_command *next;
};

class unit_data *mcreate_unit(int type);
class room_direction_data *mcreate_exit(void);
struct unit_affected_type *mcreate_affect();
void *mmalloc(int size);
void mem_reset(void);
void mem_init(void);

void process_unit(class unit_data *u);

#define MCREATE(result, type, number) \
	((result) = (type *) mmalloc(sizeof(type) * number))

#define UNIT_IDENT(unit) ((char *) (unit)->gnext) /* can you say 'filth'? */
/* You betcha... */
#define UNIT_IDENT_ASSIGN(unit,val) ((unit)->gnext = (class unit_data *)(val))

#endif /* _MUD_VMC_H */
