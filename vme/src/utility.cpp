/*
 $Author: All $
 $RCSfile: utility.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */
#include "utility.h"

#include "slog.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

FILE *g_log_file_fd = stderr;

/* ----------------------------------------------------------------- */

int is_in(int a, int from, int to)
{
    return ((a >= from) && (a <= to));
}

#ifndef HPUX
int MIN(int a, int b)
{
    return ((a < b) ? a : b);
}

int MAX(int a, int b)
{
    return ((a > b) ? a : b);
}
#endif /* HPUX */

#ifdef SOLARIS
long lrand48();
#endif

/* creates a random number in interval [from;to] */
int number(int from, int to)
{
#ifndef VMC_SRC
    int temp;
#endif

    if (from > to)
    {
#ifndef VMC_SRC
        temp = to;
        to = from;
        from = temp;
#else
        slog(LOG_ALL, 0, "From %d and to %d in number()!", from, to);
        return (to - from) / 2;
#endif
    }

#ifdef GENERIC_SYSV
    return ((lrand48() % (to - from + 1)) + from);
#else
    return (int)((rand() % (to - from + 1)) + from);
#endif
}

/* simulates dice roll */
int dice(int number, int size)
{
    int r;
    int sum = 0;

    assert(size >= 1);

    for (r = 1; r <= number; r++)
    {
#ifdef GENERIC_SYSV
        sum += ((lrand48() % size) + 1);
#else
        sum += ((rand() % size) + 1);
#endif
    }

    return sum;
}

class log_buffer g_log_buf[MAXLOG];

// Clears dest and then writes the text "bits" into 'dest' and
// also return a char * to the same resulting string (&dest[0])
const char *sprintbit(std::string &dest, ubit32 vektor, const char *names[])
{
    long nr;

    dest = "";

    for (nr = 0; vektor; vektor >>= 1, nr += names[nr] ? 1 : 0)
    {
        if (IS_SET(1, vektor))
        {
            if (names[nr])
            {
                dest.append(names[nr]);
                dest.append(" ");
            }
            else
            {
                dest.append("UNDEFINED ");
            }
        }
    }

    if (dest.empty())
    {
        dest = "NOBITS";
    }

    return &dest[0];
}

char *sprinttype(char *buf, int type, const char *names[])
{
    char *str;
    int nr;

    for (nr = 0; names[nr]; nr++)
    {
        ;
    }

    str = (0 <= type && type < nr) ? (char *)names[type] : (char *)"UNDEFINED";

    if (buf)
    {
        return strcpy(buf, str);
    }
    else
    {
        return str;
    }
}
