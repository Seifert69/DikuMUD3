/*
 $Author: All $
 $RCSfile: textutil.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.4 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "structs.h"
#include "utils.h"
#include "utility.h"
#include "textutil.h"
#include "common.h"

const char *fillwords[] = {
    "a",
    "an",
    "at",
    "from",
    "in",
    "on",
    "of",
    "the",
    "to",
    "with",
    "into",
    NULL};


/*  From char * input stream 'str' copy characters into 'buf' until
 *  end of string or newline. Returns position of 'str' after copied
 *  characters.
 */
char *str_line(const char *str, char *buf)
{
    if (str == NULL || buf == NULL)
        return NULL;

    for (; *str == ' ' || *str == '\n' || *str == '\r'; str++)
        ;

    if (*str == '\0')
        return NULL;

    for (; (*buf = *str); buf++, str++)
        if (*str == '\n' || *str == '\r')
            break;

    *buf = '\0'; /* Erase last separator */

    return (char *)str;
}

/* Make a string lowercase, and also return length of string */
int str_lower(char *s)
{
    int l;

    for (l = 0; *s; s++, l++)
        if (isupper(*s))
            *s = tolower(*s);

    return l;
}

int str_upper(char *s)
{
    int l;

    for (l = 0; *s; s++, l++)
        if (islower(*s))
            *s = toupper(*s);

    return l;
}

/* Make a string lowercase in another buffer. Also return length of string */

int str_lower(const char *s, char *d, int nBufSize)
{
    int l;

    for (l = 0; *s && (l < nBufSize); s++, d++, l++)
        if (isupper(*s))
            *d = tolower(*s);
        else
            *d = *s;

    if (l < nBufSize)
        *d = 0;
    else
    {
        slog(LOG_ALL, 0, "ERROR: str_lower destination buffer too small!");
        *(d - 1) = 0;
    }

    return l;
}

/* Return a string consisting of `n' chars c */
char *str_repeatchar(int n, char c)
{
    static char buf[256];

    if (n > 255)
        n = 255;

    if (n < 0)
        n = 0;

    buf[n] = '\0';

    for (; n;)
        buf[--n] = c;

    return buf;
}

/* Return a string consisting of `n' spaces */
char *spc(int n)
{
    return str_repeatchar(n, ' ');
}

/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
#ifdef DOS
char *itoa_dos(int n)
#else
char *itoa(int n)
#endif
{
    static char buf[32]; /* 32 digits can even cope with 64 bit ints */

    sprintf(buf, "%d", n);
    return buf;
}

/*  Return a pointer to the string containing the ascii reresentation
 *  of the integer 'n'
 *  I've made it the easy way :)
 */
char *ltoa(long n)
{
    static char buf[32]; /* 32 digits can even cope with 64 bit ints */

    sprintf(buf, "%ld", n);
    return buf;
}

/*  STR Convention: "str_" [n] [c] <meaning>
 *  n means - take number as argument, to limit operation
 *  c means - ignore possible difference in case sensitivity
 *  n always precedes c
 *  Example: str_nccmp - compare n letters non-casesensitive
 */

/*  Compare two strings without case-sensitivity
 *  Return result as defined in strcmp
 */
int str_ccmp(const char *s, const char *d)
{
    if (s == d)
        return 0;

    if (s == NULL)
        return -1;
    else if (d == NULL)
        return 1;

    for (; tolower(*s) == tolower(*d); s++, d++)
        if (*s == '\0')
            return 0;

    return (tolower(*s) - tolower(*d));
}

/*  Compare max n chars without case-sensitivity
 *  Return result as defined in strcmp
 */
int str_nccmp(const char *s, const char *d, int n)
{
    if (s == d)
        return 0;

    if (s == NULL)
        return -1;
    else if (d == NULL)
        return 1;

    for (n--; tolower(*s) == tolower(*d); s++, d++, n--)
        if (*s == '\0' || n <= 0)
            return 0;

    return (tolower(*s) - tolower(*d));
}

/* Allocate space for a copy of source */
char *str_dup(const char *source)
{
    char *dest;

    if (source)
    {
        CREATE(dest, char, strlen(source) + 1);
        strcpy(dest, source);

        return dest;
    }

    return NULL;
}

/*  As defined by 2nd Ed. of K&R ANSI C
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
char *str_str(register const char *cs, register const char *ct)
{
    register char *si;
    register char *ti;

    do
    {
        si = (char *)cs;
        ti = (char *)ct;

        while (*si++ == *ti++)
            if (*ti == '\0')
                return (char *)cs;

    } while (*cs++);

    return NULL;
}

/*  As defined by 2nd Ed. of K&R ANSI C, but non case sensitive
 *  Return pointer to first occurence of ct in cs - or NULL
 *  Used to determine ei. "from" and "in"
 */
const char *str_cstr(const char *cs, const char *ct)
{
    return strcasestr(cs, ct); // strstr ignores case

    /*    const char *si = cs;
    const char *ti = ct;

    do
    {
        for (si = cs, ti = ct; tolower(*si) == tolower(*ti); si++)
            if (*++ti == '\0')
                return cs;
    } while (*cs++);

    return NULL;*/
}

/* return string without leading spaces */
char *skip_blanks(const char *string)
{
    if (string == NULL)
        return NULL;

    for (; *string && isspace(*string); string++)
        ;

    return (char *)string;
}

/* return string without leading spaces */
char *skip_spaces(const char *string)
{
    if (string == NULL)
        return NULL;

    for (; *string && isaspace(*string); string++)
        ;

    return (char *)string;
}

void strip_trailing_blanks(char *str)
{
    if (!*str) /* empty string: return at once      */
        return;

    for (; *str; ++str) /* wind to end of string             */
        ;

    if (!isspace(*--str)) /* Not a spaceterminated string      */
        return;           /* This is mainly for `inter-code' strings */

    while (isspace(*--str)) /* rewind to last nonspace character */
        ;

    *++str = '\0'; /* step ahead and end string         */
}

void strip_trailing_spaces(char *str)
{
    if (!*str) /* empty string: return at once      */
        return;

    for (; *str; ++str) /* wind to end of string             */
        ;

    if (!isaspace(*--str)) /* Not a spaceterminated string      */
        return;            /* This is mainly for `inter-code' strings */

    while (isaspace(*--str)) /* rewind to last nonspace character */
        ;

    *++str = '\0'; /* step ahead and end string         */
}

/* Returns true is arg is empty */
ubit1 str_is_empty(const char *arg)
{
    if (arg == NULL)
        return TRUE;

    return *(skip_blanks(arg)) == '\0';
}

/* Check if the string contains nothing but numbers */
ubit1 str_is_number(const char *str)
{
    if (!*str)
        return FALSE;
    if ((*str == '-') || (*str == '+'))
        str++;
    for (; *str; str++)
        if (!isdigit(*str))
            return FALSE;

    return TRUE;
}

/* Check if the next word in str is a number */
ubit1 next_word_is_number(const char *str)
{
    char tmp[MAX_STRING_LENGTH];
    str_next_word(str, tmp);
    return str_is_number(tmp);
}

/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is made into lowercase!                       */
/* Warning, list of names must be in lowercase to match up! */
int search_block(const char *oarg, const char **list, ubit1 exact)
{
    char arg[4096];
    register int i, l;

    /* Make into lower case, and get length of string */
    l = str_lower(oarg, arg, sizeof(arg));

    if (exact)
    {
        for (i = 0; list[i]; i++)
            if (strcmp(arg, list[i]) == 0)
                return i;
    }
    else
    {
        if (!l)
            l = 1; /* Avoid "" to match the first available string */

        for (i = 0; list[i]; i++)
            if (strncmp(arg, list[i], l) == 0)
                return i;
    }

    return -1;
}

/* Block must end with null pointer                                  */
/* Warning, argument 1 is made into lowercase!                       */
int search_block_length(const char *oarg, int length,
                        const char **list, ubit1 exact)
{
    char arg[4096];
    register int i;

    /* Make into lower case, and get length of string */
    str_lower(oarg, arg, sizeof(arg));

    if (exact)
    {
        for (i = 0; list[i]; i++)
            if (!strncmp(arg, list[i], length))
                if (list[i][length] <= ' ')
                    return i;
    }
    else
    {
        if (!length)
            length = 1; /* Avoid "" to match the first available string */
        for (i = 0; list[i]; i++)
            if (!strncmp(arg, list[i], length))
                return i;
    }

    return -1;
}

int fill_word(const char *argument)
{
    return (search_block(argument, fillwords, TRUE) >= 0);
}

/* Exactly as str_next-word, except it wont change the case */
char *str_next_word_copy(const char *argument, char *first_arg)
{
    /* Find first non blank */
    argument = skip_spaces(argument);

    /* Copy next word and make it lower case */
    for (; *argument > ' '; argument++)
        *first_arg++ = *argument;

    *first_arg = '\0';

    return skip_spaces(argument);
}

/* Copy next word from argument into first_arg and make lowercase*/
char *str_next_word(const char *argument, char *first_arg)
{
    /* Find first non blank */
    argument = skip_spaces(argument);

    /* Copy next word and make it lower case */
    for (; *argument > ' '; argument++)
        *first_arg++ = tolower(*argument);

    *first_arg = '\0';

    return skip_spaces(argument);
}

/*  Find the first sub-argument of a string,
 *  return pointer to first char in primary argument, following the sub-arg
 */
char *one_argument(const char *argument, char *first_arg)
{
    assert(argument && first_arg);

    do
        argument = str_next_word(argument, first_arg);
    while (fill_word(first_arg));

    return (char *)argument;
}

/* determine if 'arg1' is an abbreviation of 'arg2'. */
ubit1 is_abbrev(const char *arg1, const char *arg2)
{
    if (!*arg1)
        return FALSE;

    for (; *arg1; arg1++, arg2++)
        if (tolower(*arg1) != tolower(*arg2))
            return FALSE;

    return TRUE;
}

/* determine if 'arg1' is an abbreviation of 'arg2'.          */
/* The empty string is never an abbrevation of another string */
ubit1 is_multi_abbrev(const char *arg1, const char *arg2)
{
    if (!*arg1)
        return FALSE;

    for (; *arg1 && !isspace(*arg1); arg1++, arg2++)
        if (tolower(*arg1) != tolower(*arg2))
            return FALSE;

    return TRUE;
}

/* Block must end with null pointer, -1 if not found otherwise index */
/* Warning, argument 1 is converted into lowercase!                  */
int search_block_abbrevs(const char *oarg, const char **list, const char **end)
{
    char arg[4096];
    char buf1[MAX_INPUT_LENGTH], buf2[MAX_INPUT_LENGTH];
    char *s, *ps, *bestpos;
    const char *d;
    int i;
    int match, bestidx;

    i = str_lower(skip_spaces(oarg), arg, sizeof(arg));

    bestidx = -1;
    bestpos = NULL;

    for (i = 0; list[i]; i++)
    {
        for (match = 0, s = arg, d = list[i];;)
        {
            ps = s;
            s = str_next_word(s, buf1);
            d = str_next_word(d, buf2);
            if (is_multi_abbrev(buf1, buf2))
            {
                s = skip_spaces(s);
                d = skip_spaces(d);
                match++;
            }
            else
                break;
        }
        if (match && ps > bestpos)
        {
            bestpos = ps;
            bestidx = i;
        }
    }

    if (bestidx != -1)
    {
        *end = oarg + (bestpos - arg);
        return bestidx;
    }

    *end = oarg;

    return -1;
}

/* returns a pointer to a (static) string wich contains s1 and s2 */
char *str_cc(const char *s1, const char *s2)
{
    static char buf[16384];

    strncpy(buf, s1, sizeof(buf) - 1);
    strncat(buf, s2, sizeof(buf) - 1 - strlen(buf));

    return buf;
}

/* =================================================================== */
/* Insert the string "str" at the first position in the "itxt" string. */

void str_insert(char *str, char *itxt)
{
    int nlen, slen;

    nlen = strlen(itxt);
    slen = strlen(str);

    memmove(str + nlen, str, slen + 1);
    memcpy(str, itxt, nlen);
}

/*
   Scan the string 'str' for the string OLD and replace each
   occurence with new. Does not handle memory overrun so str
   must be large enough to hold it.
   */
void str_substitute(const char *old, const char *newstr, char *str)
{
    char *b;
    int olen, nlen, slen;

    if (!str)
        return;

    if (!old)
        return;

    if (!newstr)
        nlen = 0;
    else
        nlen = strlen(newstr);

    olen = strlen(old);
    slen = strlen(str);

    while ((b = str_str(str, old)))
    {
        if (nlen <= olen)
        {
            if (nlen != 0)
            {
                memcpy(b, newstr, nlen);
            }
            memmove(b + nlen, b + olen, slen - (b - str + olen) + 1);
            slen -= olen - nlen;
        }
        else
        {
            memmove(b + nlen, b + olen, slen - (b - str));
            memcpy(b, newstr, nlen);
            slen += nlen - olen;
        }
    }
}

// Search is the string to search for in subject and replace with replace
void str_substitute(const std::string &search, const std::string &replace, std::string &subject)
{
    size_t pos = 0;

    if (search.empty())
        return;

    while ((pos = subject.find(search, pos)) != std::string::npos)
    {
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}

/* Remove all occurences of c in s */
void str_rem(char *s, char c)
{
    int diff;

    if (c == '\0')
        return;

    for (diff = 0; *(s + diff);)
    {
        if (*s == c)
            diff++;
        else
            s++;

        *s = *(s + diff);
    }
}

/* Remove all occurences of control codes in s, keeps \n and \r */
void str_rem_codes(char *s)
{
    int diff;

    for (diff = 0; *(s + diff);)
    {
        if ((*s < ' ') && (*s != '\n') && (*s != '\r'))
            diff++;
        else
            s++;

        *s = *(s + diff);
    }
}

/* Change all \n\r to <br/>\n\r */
void str_amp_html(const char *s, char *d)
{
    for (; *s;)
    {
        if (*s == '&')
        {
            *d++ = '&';
            *d++ = 'a';
            *d++ = 'm';
            *d++ = 'p';
            *d++ = ';';
            s++; // Skip &
        }
        else
            *d++ = *s++;
    }
    *d = 0;
}

/* Change all \n\r to <br/>\n\r */
void str_nr_brnr(const char *s, char *d)
{
    for (; *s;)
    {
        if ((*s == '\n') && (*(s + 1) == '\r'))
        {
            *d++ = '<';
            *d++ = 'b';
            *d++ = 'r';
            *d++ = '/';
            *d++ = '>';
            *d++ = '\n';
            *d++ = '\r';
            s += 2;
        }
        else
            *d++ = *s++;
    }
    *d = 0;
}

/* Replace all occurences of non alfa, digit with space in s */
void str_blank_punct(char *s)
{
    static char c[3] = {' ', ' ', 0};

    for (; *s; s++)
        if (ispunct(*s))
        {
            c[0] = *s;
            *s = ' ';
            str_insert(s + 1, c);
            s += 2;
        }
}

/* Remove all multiple space occurences in s */
void str_remspc(register char *s)
{
    register char *cp;

    while (*s && (*s != ' ' || *(s + 1) != ' '))
        s++;

    if (*s == 0)
        return;

    cp = s;

    while (*cp)
    {
        while (*cp == ' ' && *(cp + 1) == ' ')
            cp++;

        *s++ = *cp++;
    }
    *s = 0;
}

void str_chraround(char *str, char c)
{
    int l;

    l = strlen(str);
    memmove(str + 1, str, l);
    str[0] = c;
    str[l + 1] = c;
    str[l + 2] = 0;
}

/* Names has the format :                                 */
/*     names[0] pointer to a string                       */
/*     names[N] pointer to the last N'th string           */
/*     names[N+1] NIL pointer                             */
/*                                                        */
/*   Assumes  that arg is without leading spaces,         */
/*   no double spaces and contains text                   */
/* Returns NULL if not found, or pointer in arg after     */
/* the found unit                                         */

const char *is_name_raw(const char *arg, char const *const *names) // MS2020 const char *names[])
{
    register int i, j;

    for (i = 0; names[i]; i++)
    {
        for (j = 0; names[i][j]; j++)
            if (tolower(arg[j]) != tolower(names[i][j]))
                break;

        if (!names[i][j])
            if (!arg[j] || isaspace(arg[j]))
                return (arg + j);
    }

    return NULL;
}

/*
 ******** Utility routines for the name lists. *******
 */

/*  Names has the format:
 *     names[0] pointer to a string
 *     names[N] pointer to the last N'th string
 *     names[N+1] NIL pointer
 *  Assumes nothing nice about arg - only about names[]
 */
/* We need to copy to BUF in order to prevent crash when */
/* str_remspc might want to change "constant" strings    */

char *is_name(char *arg, char const *const *names) // MS2020 const char *names[])
{
    for (; isaspace(*arg); arg++)
        ;

    if (!*arg)
        return 0;

    str_remspc(arg);

    return (char *) is_name_raw(arg, names);
}

/* Create an empty namelist */
char **create_namelist(void)
{
    char **list;

    CREATE(list, char *, 1);
    list[0] = NULL;

    return list;
}

/* Add a new name to the end of an existing namelist */
char **add_name(const char *name, char **namelist)
{
    int pos = 0;

    assert(name && namelist);

    while (namelist[pos])
        pos++;

    RECREATE(namelist, char *, pos + 2);

    namelist[pos] = str_dup(name);
    namelist[pos + 1] = NULL;

    return namelist;
}

/* free space allocated to a namelist */
void free_namelist(char **list)
{
    char **original;

    assert(list);

    original = list;

    while (*list)
    {
        free(*list);
        list++;
        /* MS: Well, ugly but we have to do while free macro is in use! */
    }

    FREE(original);
}

/*  Compare if 'next_word' is the next word in buf - if true return pointer
 *  Put no space before or after 'next_word'
 *  Return NIL if not identical, otherwise return pointer to location
 *  just after the match
 */
char *str_ccmp_next_word(const char *buf, const char *next_word)
{
    buf = skip_spaces(buf);

    for (; *next_word; next_word++, buf++)
        if (*next_word != *buf)
            return NULL;

    if (!*buf || isaspace(*buf)) /* Buf must end here or be word separated */
        return (char *)buf;

    return NULL;
}

/*  Must receive a string of the format 'name@zone\0' or
 *  'zone/name\0'. It sabotages the special symbol '@'
 *  or '/' if any. If no special symbol name will
 *  contain the string
 */
void split_fi_ref(const char *str, char *zone, char *name)
{
    char *c, *t;
    int l;

    *zone = 0;
    *name = 0;

    if (!str)
        return;

    str = skip_spaces(str);

    if ((c = (char *)strchr(str, '@')))
    {
        l = MIN(c - str, FI_MAX_UNITNAME);
        strncpy(name, str, l);
        name[l] = '\0';

        l = MIN(strlen(c + 1), FI_MAX_ZONENAME);
        if ((t = strchr(c + 1, ' ')))
            l = MIN(l, t - (c + 1));
        strncpy(zone, c + 1, l);
        zone[l] = 0;
        str_lower(zone);
        str_lower(name);
    }
    else if ((c = (char *)strchr(str, '/')))
    {
        l = MIN(c - str, FI_MAX_ZONENAME);
        strncpy(zone, str, l);
        zone[l] = '\0';

        l = MIN(strlen(c + 1), FI_MAX_UNITNAME);
        if ((t = strchr(c + 1, ' ')))
            l = MIN(l, t - (c + 1));
        strncpy(name, c + 1, l);
        name[l] = 0;
        str_lower(zone);
        str_lower(name);
    }
    else
    {
        if ((c = (char *)strchr(str, ' ')))
        {
            l = MIN(c - str, FI_MAX_UNITNAME);
            strncpy(name, str, l);
            name[l] = '\0';
        }
        else
        {
            l = MIN(strlen(str), FI_MAX_UNITNAME);
            strncpy(name, str, l);
            name[l] = 0;
        }

        *zone = '\0';
        str_lower(zone);
        str_lower(name);
    }
}

char *catnames(char *s, char **names)
{
    char **nam;
    ubit1 ok = FALSE;

    if (names)
    {
        strcpy(s, "{");
        TAIL(s);
        for (nam = names; *nam; nam++)
        {
            ok = TRUE;
            sprintf(s, "\"%s\",", *nam);
            TAIL(s);
        }
        if (ok)
            s--; /* remove the comma */
        strcpy(s, "}");
    }
    else
        sprintf(s, "NULL");

    TAIL(s);

    return s;
}

// Translates C style backslashed escapes to ascii codes.
// never writes more characters than in src.
void str_cescape_format(const char *src, char *dest)
{
    while (*src)
    {
        *dest = *src++;

        if (*dest == '\\')
        {
            switch (*src)
            {
            case 'n':
                *dest = '\n';
                src++;
                break;
            case 'r':
                *dest = '\r';
                src++;
                break;
            case '\\':
                break;
            case 0:
                dest++;    // Save the backslash
                *dest = 0; // terminate dest
                break;
            default:
                dest++;       // Save the backslash
                *dest = *src; // Copy the unknown code
                src++;
                break;
            }
        }
        dest++;
    }
    *dest = 0;
}

// Transforms str in-place with C-style escaping
void str_cescape_transform(char *src)
{
    str_cescape_format(src, src);
}

/* Format the string "str" following the format rules. No longer than */
/* 'destlen' result. Returns resulting length.                        */
int str_escape_format(const char *src, char *dest, int destlen, int formatting)
{
    assert((size_t)destlen >= strlen(src) + 1);
    str_cescape_format(src, dest);
    return strlen(dest);
}

char *str_escape_format(const char *src, int formatting)
{
    char dest[MAX_STRING_LENGTH] = "";
    str_escape_format(src, dest, sizeof(dest), formatting);

    return str_dup(dest);
}


// Adapted from
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

static const uint8_t utf8d[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 00..1f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 20..3f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 40..5f
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 60..7f
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 80..9f
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, // a0..bf
  8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // c0..df
  0xa,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x3,0x4,0x3,0x3, // e0..ef
  0xb,0x6,0x6,0x6,0x5,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8,0x8, // f0..ff
  0x0,0x1,0x2,0x3,0x5,0x8,0x7,0x1,0x1,0x1,0x4,0x6,0x1,0x1,0x1,0x1, // s0..s0
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,0,1,1,1,1,1,1, // s1..s2
  1,2,1,1,1,1,1,2,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1, // s3..s4
  1,2,1,1,1,1,1,1,1,2,1,1,1,1,1,1,1,1,1,1,1,1,1,3,1,3,1,1,1,1,1,1, // s5..s6
  1,3,1,1,1,1,1,3,1,3,1,1,1,1,1,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // s7..s8
};

ubit32 inline utf8_decode(ubit32 *state, ubit32 *codep, ubit8 byte)
{
    ubit32 type = utf8d[byte];

    *codep = (*state != UTF8_ACCEPT) ? (byte & 0x3fu) | (*codep << 6) : (0xff >> type) & (byte);
    *state = utf8d[256 + *state*16 + type];

    return *state;
}


void str_correct_utf8(char *src)
{
    ubit32 codepoint = 0;
    ubit32 prev = 0, current = 0;
    char *s = src;

    for (; *s; prev = current, s++)
    {
        if (utf8_decode(&current, &codepoint, *s) == UTF8_REJECT)
        {
            // The byte is invalid, replace it and restart.
            *s = '?';
            current = UTF8_ACCEPT;
            if (prev != UTF8_ACCEPT)
            {
                s--;
                *s = '?';
                if (s >= src) 
                    s--; // end of for means we'll start checking this '?'
                if (s >= src)
                    s--; // so go back yet another char so we'll check the char before '?'

                // s can become 1 less than src, but for loop above +1 and then it's equal src
                // MS: Code on the website most definitely seemed wrong
                // so if prev was not accept, then because I fear it might 
                // be a 3 character sequence, I go back 
            }
        }
    }
}

void str_correct_utf8(std::string &src)
{
    str_correct_utf8((char *) src.c_str());
}


// This validates a string to be UTF-8 cool
// Illegal characters become question marks
//
void obs_str_correct_utf8(std::string &src)
{
    int nLen = src.length();
    
    for (int pos = 0; pos < nLen; ++pos)
    {
        if ((src[pos] & 0x80) == 0) // lead bit is zero, must be a single ascii
        {
             // ignore all odd/control ascii characters.
            if ((src[pos] < 32) && (src[pos] != '\n') && (src[pos] != '\r'))
                src[pos] = '?';
            continue;
        }
        else if ((src[pos] & 0xE0) == 0xC0) // 110x xxxx
        {
            if (pos + 1 < nLen)
            {
                pos += 1;
                continue;
            }
            src[pos] = '?'; // Must be a UTF8 error of sorts
        }
        else if ((src[pos] & 0xF0) == 0xE0) // 1110 xxxx
        {
            if (pos + 2 < nLen)
            {
                pos += 2;
                continue;
            }
            src[pos] = '?'; // Must be a UTF8 error of sorts
        }
        else if ((src[pos] & 0xF8) == 0xF0) // 1111 0xxx
        {
            if (pos + 3 < nLen)
            {
                pos += 3;
                continue;
            }
            src[pos] = '?'; // Must be a UTF8 error of sorts
        }
        else
            src[pos] = '?'; // Must be a UTF8 error of sorts
    } // end for
}


// This both encodes and prepares string for interpreter.
// Removes all leading and trailing whitespace.
// Ensures only one whitespace between each word.
// HTML encodes and preserves UTF8
// Returns a memory allocated string.
//
char *html_encode_utf8(const char *src)
{
    if (!src)
        return NULL;

    int nLen = strlen(src);
    std::string sBuffer;
    sBuffer.reserve(nLen * 1.2);
    int pos = 0;

    // First skip all leading whitespace
    while (isspace(src[pos]))
        pos++;

    for (; pos < nLen; ++pos)
    {
        if ((src[pos] & 0x80) == 0) // lead bit is zero, must be a single ascii
        {
            if (src[pos] < 32) // ignore all odd/control ascii characters.
                continue;

            switch (src[pos])
            {
            case ' ':
            { // Trim all whitespace to one space
                sBuffer.append(" ");
                while (isspace(src[pos + 1]))
                    pos++;
                continue;
            }
            case '&':
            {
                sBuffer.append("&amp;");
                continue;
            }
            case '\"':
            {
                sBuffer.append("&quot;");
                continue;
            }
            case '<':
            {
                sBuffer.append("&lt;");
                continue;
            }
            case '>':
            {
                sBuffer.append("&gt;");
                continue;
            }
            } //Switch
            sBuffer.append(&src[pos], 1);
            continue;
        }
        else if ((src[pos] & 0xE0) == 0xC0) // 110x xxxx
        {
            if (pos + 1 < nLen)
            {
                sBuffer.append(&src[pos], 2);
                pos += 1;
                continue;
            }
            sBuffer.append(&src[pos], 1); // Must be a UTF8 error of sorts
        }
        else if ((src[pos] & 0xF0) == 0xE0) // 1110 xxxx
        {
            if (pos + 2 < nLen)
            {
                sBuffer.append(&src[pos], 3);
                pos += 2;
                continue;
            }
            sBuffer.append(&src[pos], 1); // Must be a UTF8 error of sorts
        }
        else if ((src[pos] & 0xF8) == 0xF0) // 1111 0xxx
        {
            if (pos + 3 < nLen)
            {
                sBuffer.append(&src[pos], 4);
                pos += 3;
                continue;
            }
            sBuffer.append(&src[pos], 1); // Must be a UTF8 error of sorts
        }
        else
        {
            // printf( "Unrecognized lead byte (%02x)\n", lb );
            sBuffer.append("?"); // Must be a UTF8 error of sorts
        }
    } // end for

    return str_dup(sBuffer.c_str()); // Oh if only we used strings everywhere :))
}

// Helper function to wrap javascript into something. This something might change
// so it was easier to write a helper function. E.g. to onload for image. who knows.
std::string scriptwrap(const char *str)
{
    std::string mystr;

    mystr = "<script>";
    mystr.append(str);
    mystr.append("</script>");

    return mystr;
}

// return 1 as long as we substitute &s
// Potential crash if string is full of many &s99, no OOB check
int my_str_replace_space(char *sbuf)
{
    int i = 0;
    int ln = strlen(sbuf);

    while (i < ln)
    {
        if ((*sbuf == '&') && ((*(sbuf + 1) == 's') || (*(sbuf + 1) == 'S')))
        {
            char Buf[3];
            int rep = 0;
            char *src = sbuf + 2;

            while (isdigit(*src) && rep < (int)sizeof(Buf) - 1)
                Buf[rep++] = *src++;
            Buf[rep] = 0;

            char buf2[10];
            strcpy(buf2, "&s");
            strcat(buf2, Buf);
            str_substitute(buf2, spc(atoi(Buf)), sbuf);
            return 1;
        }
        sbuf++;
        i++;
    }

    return 0;
}

//
// Changes old codes such as &l &c to HTML
// Only used in char conversion of units < v70
// so no bother with mem leaks of strings
char *fix_old_codes_to_html(const char *c)
{
    char buf[2 * MAX_STRING_LENGTH];
    int bPre = 0;

    assert(strlen(c) < sizeof(buf) / 2);
    strcpy(buf, c);

    // Remove all codes (anything less than ' ')
    str_rem_codes(buf);

    // Make sure && is unique by using ESC which we just removed
    str_substitute("&&", "\x1B", buf);

    // Colors
    str_substitute("&cw", "", buf);
    str_substitute("&cr", "", buf);
    str_substitute("&cy", "", buf);
    str_substitute("&cg", "", buf);
    str_substitute("&cb", "", buf);
    str_substitute("&cm", "", buf);
    str_substitute("&cn", "", buf);
    str_substitute("&c+w", "", buf);
    str_substitute("&c+r", "", buf);
    str_substitute("&c+y", "", buf);
    str_substitute("&c+g", "", buf);
    str_substitute("&c+b", "", buf);
    str_substitute("&c+m", "", buf);
    str_substitute("&c+n", "", buf);

    // Specials
    str_substitute("&x", "\n", buf);
    str_substitute("&n", "<br/>", buf);

    if (str_str(buf, "&l"))
    {
        str_substitute("&l", "<pre>", buf);
        bPre = 1;
    }

    str_substitute("&h", "", buf);
    str_substitute("&p", "", buf);
    str_substitute("&f", "", buf);
    str_substitute("&bb", "", buf);
    str_substitute("&bn", "", buf);

    char buf2[2 * MAX_STRING_LENGTH];
    str_amp_html(buf, buf2);
    strcpy(buf, buf2);

    str_substitute("'", "&apos;", buf);
    str_substitute("<", "&lt;", buf);
    str_substitute(">", "&gt;", buf);

    // replace &s[0-9] with spaces
    //sed -i -- 's/&s[0-9][0-9]/ /g' *.zon
    // replace &s[00-99] with spaces
    //sed -i -- 's/&s[0-9]/ /g' *.zon

    str_substitute("&[default]", "</div>", buf);

    while (my_str_replace_space(buf))
        ;

    // I'm hoping this is not necessary to change
    // Replace &[name] with <div class='name'>
    //sed -i -- 's/&\[\([^]]*\)\]/\<div class\=\x27\1\x27\>/g' *.zon

    str_substitute("\x1B", "&amp;", buf);

    if (bPre)
    {
        strcat(buf, "</pre>");
    }

    str_nr_brnr(buf, buf2);
    return str_dup(buf2); // Dont worry about memory leaks it's a one time thing
}

/* Get the contents between <htmltag> and convert it to lowercase
 *    p must point to first <.
 *    Copies the tag between <> into pTag. 
 *    Copies at most nTagMax bytes (incl \0) 
 *    returns position right after > and returns p if no >
 */
const char *getHTMLTag(const char *p, char *pTag, int nTagMax)
{
    const char *c;
    int n;

    *pTag = 0;

    assert(p && (*p == '<'));

    p++; // Skip '<'

    c = strchr(p, '>');
    if (c == NULL)
        return p;

    n = c - p + 1; // How many chars including \0

    n = min(n, nTagMax);

    for (int i = 0; i < n; i++)
        pTag[i] = tolower(*(p + i));

    pTag[n - 1] = 0;

    return c + 1; // return char after '>'
}

/* Gets the value of an atribute from a HTML tag
 *    p points to the html tag contents found between < and >
 *    name is the value to get from, e.g. "class" will get you the value for "class='value"
 *    Copies the value into pTag. Copies at most nTagMax bytes (incl \0)
 *    Returns length of value.
 * Not 100% bullet proof
 */
int getHTMLValue(const char *name, const char *p, char *pTag, int nTagMax)
{
    const char *c;

    *pTag = 0;

    c = strstr(p, name);
    if (c == NULL)
        return 0;

    c += strlen(name);
    c = skip_blanks(c); // skip any whitespace before equal
    if (*c != '=')
        return 0;
    c++;                // skip equal
    c = skip_blanks(c); // skip any whitespace after equal

    if (*c != '\'')
        return 0;

    c++; // Skip '

    const char *ce;
    ce = strchr(c, '\''); // Find the last ' for the value
    if (ce == NULL)
        return 0;

    if (ce - c <= 1) // If the string is empty
        return 0;

    if (ce - c > nTagMax - 1) // Not enough space
        return 0;

    strncpy(pTag, c, ce - c);
    pTag[ce - c] = 0;

    return ce - c;
}

/* 
 * Given an HTML tag pOldTag without <>, change the value of the attribute pAttr to pNewVal 
 * and put the result in pNewTag
 * 
 * pOldTag: the HTML tag, e.g. div class='default'
 * pATtr    the attribute to change, e.g. class
 * pNewVal  the new value to put into the class, e.g. hello
 * pNewTag  result saved in this string
 * nTagMax  write at most nTagMax chars into pNewTag.
 * return 0 is failed, 1 if success
 */
int substHTMLTagClass(const char *pOldTag, const char *pAttr, const char *pNewVal, char *pNewTag, int nTagMax)
{
    const char *c;

    *pNewTag = 0;

    c = strstr(pOldTag, pAttr);
    if (c == NULL)
        return 0;

    c += strlen(pAttr);
    c = skip_blanks(c); // skip any whitespace before equal
    if (*c != '=')
        return 0;
    c++;                // skip equal
    c = skip_blanks(c); // skip any whitespace after equal

    if (*c != '\'')
        return 0;

    c++; // Skip '

    strncpy(pNewTag, pOldTag, c - pOldTag); // Copy up to and including first '
    pNewTag[c - pOldTag] = 0;
    strcat(pNewTag, pNewVal); // Copy in the new value
    strcat(pNewTag, "\'");    // Add the ending '

    const char *ce;

    c = strchr(c, '\''); // Find the last ' for the value
    if (c == NULL)       // Missing ending '
        return 0;

    c++; // Skip the '

    ce = c;
    TAIL(ce); // find the remaineder of the string

    if (ce - c <= 0) // If the string is empty, we are done
        return 1;

    if (strlen(pNewTag) + strlen(c) >= (size_t)nTagMax - 1) // Not enough space
        return 0;

    strcat(pNewTag, c);

    return 1;
}

// given the attribute colorstr return <div class='colorstr'>. Dont make colorstr too insanely long.
const char *divcolor(const char *colorstr)
{
    static char buf[256];

    sprintf(buf, "<div class='%s'>", colorstr);

    return buf;
}


// Encode str to JSON encoding (format X)
std::string str_json_encode(const char *str)
{
    string s;

    s = str;

    str_substitute("\n", "\\n", s);
    str_substitute("\"", "&quot;", s);

    return s;
}

// As str_json_encode but wraps string in quotes
std::string str_json_encode_quote(const char *str)
{
    string s;

    s = "\"";
    s.append(str_json_encode(str));
    s.append("\"");

    return s;
} 

std::string str_json(const char *lbl, ubit64 nInt)
{
    string s;

    s.append("\"");
    s.append(str_json_encode(lbl));
    s.append("\"");
    s.append(": ");
    s.append(ltoa(nInt));

    return s;
}

std::string str_json(const char *lbl, const char *str)
{
    string s;

    s.append("\"");
    s.append(lbl);
    s.append("\"");
    s.append(": ");
    s.append("\"");
    if (str)
        s.append(str_json_encode(str));
    s.append("\"");

    return s;
}

std::string str_json(const char *lbl, const std::string &str)
{
    return str_json(lbl, str.c_str());
}


// Kind of like strncmp except it returns true if the string matches 
// up to the first \0 character. Necessary hack as a bridge between
// old and new password system to both rectify incorrect salt in 
// set pwd <player> and to handle increase in stored password length
// return 0 = match, 1 = differ
int pwdcompare(const char *p1, const char *p2, int nMax)
{
    int i;

    if ((p1 == NULL) || (p2 == NULL))
        return 1;

    for (i=0; i < nMax; i++)
    {
        if (p1[i] != p2[i])
        {
            if ((p1[i] == 0) || (p2[i] == 0))
            {
                if (i < 10)
                    return 1;
                else
                    return 0;                
            }
            else
                return 1;
        }
        else if (p1[i] == 0) // They are both zero
            return 1;
    }

    return 0;
}
