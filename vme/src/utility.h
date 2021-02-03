/*
 $Author: tperry $
 $RCSfile: utility.h,v $
 $Date: 2001/04/29 03:46:07 $
 $Revision: 2.1 $
 */

#ifndef _MUD_UTILITY_H
#define _MUD_UTILITY_H

#ifdef MIN
#undef MIN
#endif

#ifdef MAX
#undef MAX
#endif

int is_in(int a, int from, int to);
int MIN(int a, int b);
int MAX(int a, int b);

int number(int from, int to);
int dice(int number, int size);

char *sprintbit(char *buf, ubit32 vektor, const char *names[]);
char *sprinttype(char *buf, int type, const char *names[]);

/* in game log stuff below */

#define MAXLOG 10

#define HERE __FILE__, __LINE__

/* Use like this:  error(HERE, "Something went wrong: %s", buf); */
void error(const char *file, int line, const char *fmt, ...);

class unit_data *hometown_unit(char *str);

class log_buffer
{
public:
    log_buffer(void)
    {
        str[0] = 0;
    }

    char str[MAX_INPUT_LENGTH + 50];
    enum log_level level;
    ubit8 wizinv_level;
};

/* For the printing of obj_type information, as used by the identify spell and
 * wstat command.  Look in act_stat.c for details.
 */
struct obj_type_t
{
    const char *fmt;
    ubit8 v[5];
};

char *stat_obj_data(class unit_data *u, struct obj_type_t *obj_data);

#endif
