/*
 $Author: All $
 $RCSfile: utility.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> /* va_args in slog()        */

#include "structs.h"
#include "utils.h"
#include "files.h"
#include "textutil.h"
#include "comm.h"
#include "db.h"
#include "utility.h"
#include "interpreter.h"
#include "db_file.h"

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
#ifdef GENERIC_SYSV
      sum += ((lrand48() % size) + 1);
#else
      sum += ((rand() % size) + 1);
#endif

   return sum;
}

class log_buffer log_buf[MAXLOG];

/* writes a string to the log */
void slog(enum log_level level, ubit8 wizinv_level, const char *fmt, ...)
{
   static ubit8 idx = 0;
   static ubit32 log_size = 0;
   char buf[MAX_STRING_LENGTH], *t;
   va_list args;

   time_t now = time(0);
   char *tmstr = ctime(&now);

   tmstr[strlen(tmstr) - 1] = '\0';

   if (wizinv_level > 0)
      sprintf(buf, "(%d) ", wizinv_level);
   else
      *buf = '\0';

   t = buf;
   TAIL(t);

   va_start(args, fmt);
   vsprintf(t, fmt, args);
   va_end(args);

   /* 5 == " :: \n";  24 == tmstr (Tue Sep 20 18:41:23 1994) */
   log_size += strlen(buf) + 5 + 24;

   if (log_size > 40000000) /* 4 meg is indeed a very big logfile! */
   {
      fprintf(log_file_fd, "Log-file insanely big!  Going down.\n");
      abort(); // Dont use error, it calls syslog!!! *grin*
   }

   fprintf(log_file_fd, "%s :: %s\n", tmstr, buf);
   fflush(log_file_fd);

   if (level > LOG_OFF)
   {
      log_buf[idx].level = level;
      log_buf[idx].wizinv_level = wizinv_level;
      memcpy(log_buf[idx].str, buf, sizeof(log_buf[idx].str) - 1);
      log_buf[idx].str[sizeof(log_buf[idx].str) - 1] = 0;

      idx++;
      idx %= MAXLOG; /* idx = 1 .. MAXLOG-1 */

      log_buf[idx].str[0] = 0;
   }
}

/* MS: Moved szonelog to handler.c to make this module independent. */

/*  Replacing slog/assert(FALSE)
 *  usage: error(HERE, "Bad id: %d", id);
 */
void error(const char *file, int line, const char *fmt, ...)
{
   char buf[512];
   va_list args;

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   slog(LOG_OFF, 0, "%s:%d: %s", file, line, buf);

   abort();
}

char *sprintbit(char *buf, ubit32 vektor, const char *names[])
{
   char *result = buf;
   long nr;

   *result = '\0';

   for (nr = 0; vektor; vektor >>= 1, nr += names[nr] ? 1 : 0)
      if (IS_SET(1, vektor))
      {
         sprintf(result, "%s ", names[nr] ? names[nr] : "UNDEFINED");
         TAIL(result);
      }

   if (!*buf)
      strcpy(buf, "NOBITS");

   return buf;
}

char *sprinttype(char *buf, int type, const char *names[])
{
   char *str;
   int nr;

   for (nr = 0; names[nr]; nr++)
      ;

   str = (0 <= type && type < nr) ? (char *)names[type] : (char *)"UNDEFINED";

   if (buf)
      return strcpy(buf, str);
   else
      return str;
}
