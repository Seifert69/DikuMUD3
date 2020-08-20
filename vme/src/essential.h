/*
 $Author: All $
 $RCSfile: essential.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.6 $
 */

#ifndef _MUD_ESSENTIAL_H
#define _MUD_ESSENTIAL_H

#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include "membug.h"

#ifdef _WINDOWS
#include <ctype.h>
char *crypt (char *salt, char *password);
#endif

enum log_level
{ LOG_OFF, LOG_BRIEF, LOG_EXTENSIVE, LOG_ALL, LOG_DIL};

#define MAX_STRING_LENGTH  12288
#define MAX_INPUT_LENGTH     512

#define TRUE  1
#define FALSE 0


#define BITCONV16(i) ((((ubit16) i)>>8)|(( ((ubit16) i)&255)<<8))
#define BITCONV32(i) ((((ubit32) i)>>16)|(( ((ubit32) i)&65535)<<16))

#define ISNEWL(ch) ((ch) == '\n' || (ch) == '\r')

#define SGN(i) (((i)>=0)?1:-1)

#define ODD(i)  ((i) & 1)
#define EVEN(i) (!((i) & 1))

#define TAIL(str) for(; *(str); (str)++)

#define STRCAT(p,c) strcat(p,c); TAIL(p)

#define STR(str) (str ? str : "(NIL POINTER)")

#define IF_STR(st) ((st) ? (st) : "")

#define ANA(c)     \
  (strchr("aeiouyAEIOUY", c) ? "an" : "a")

#define CAPC(st)  (*(st) = toupper(*(st)))
#define CAP(st)   (CAPC(st), (st))

/*define CREATE(res, type, num) \
  do { if (((res) = (type *) calloc((num), sizeof(type))) == NULL) \
	 assert(FALSE); } while(0)
*/
/*define RECREATE(res, type, num) \
  do { if (((res) = (type *) realloc((res), sizeof(type) * (num))) == NULL) \
	 assert(FALSE); } while(0)*/


#define IS_SET(flag,bit)  ((flag) & (bit))

#define SET_BIT(var,bit)  ((var) |= (bit))

#define REMOVE_BIT(var,bit)  ((var) &= ~(bit) )

#define TOGGLE_BIT(var,bit)  ((var) ^= (bit))

#define SWITCH(a,b) { (a) ^= (b); (b) ^= (a); (a) ^= (b); }
#define DEBUG(a) \
   do { \
   fprintf(stderr, a); \
   } while (0)


#ifndef HPUX
typedef signed char sbit8;
typedef unsigned char ubit8;
typedef signed short int sbit16;
typedef unsigned short int ubit16;

#ifdef DOS
typedef signed long sbit32;
typedef unsigned long ubit32;
#else
typedef signed int         sbit32;
typedef unsigned int       ubit32;
typedef signed long int    sbit64;  // MS2020
typedef unsigned long int  ubit64;  // MS2020
#endif

#endif /* HPUX */

typedef ubit8 ubit1;		/* Boolean */
extern FILE *log_file_fd;
void slog (enum log_level, ubit8, const char *, ...);

#define CREATE(res, type, num) \
if (((res) = (type *) calloc((num), sizeof(type))) == NULL) \
    assert(FALSE);

#define RECREATE(res, type, num) \
if (((res) = (type *) realloc((res), sizeof(type) * (num))) == NULL) \
    assert(FALSE);

#define FREE(p) { free(p); p = NULL; }

#endif // MUD_ESSENTIAL
