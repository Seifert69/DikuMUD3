/*
 $Author: All $
 $RCSfile: ban.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.2 $
 */

#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "textutil.h"
#include "interpreter.h"
#include "utility.h"
#include "db_file.h"
#include "db.h"
#include "ban.h"
#include "files.h"

#ifndef DOS
/* Kindly left out of time.h by <Insert_Unix_Vendor> */
size_t strftime (char *s, size_t smax, const char *fmt, const struct tm *tp);
#endif

#define BAN_SAVE str_cc(g_cServerConfig.m_libdir, BAN_FILE)

struct ban_t
{
    char *site;
    char type;
    time_t until;
    char *textfile;
    struct ban_t *next;
}
*ban_list = NULL;


void
save_ban (void)
{
    struct ban_t *tmp;
    FILE *bf = fopen (BAN_SAVE, "w");
    assert (bf);

    for (tmp = ban_list; tmp; tmp = tmp->next)
        fprintf (bf, "%s %c %ld %s\n",
                 tmp->site, tmp->type, tmp->until, tmp->textfile);

    fclose (bf);
}


void
load_ban (void)
{
    FILE *bf;
    struct ban_t *tmp;
    char buf[256], site[256], textfile[256];

    touch_file (BAN_SAVE);

    bf = fopen (BAN_SAVE, "r");
    assert (bf);

    while (fgets (buf, sizeof buf, bf) != NULL)
    {
        CREATE (tmp, struct ban_t, 1);
        sscanf (buf, "%s %c %ld %s\n", site, &tmp->type, &tmp->until, textfile);
        tmp->site = str_dup (site);
        tmp->textfile = str_dup (textfile);
        tmp->next = ban_list;
        ban_list = tmp;
    }

    fclose (bf);
}


time_t ban_timer (char *arg)
{
    time_t now = time (0);

    while (*arg)
    {
        int mult = 0;

        arg = skip_spaces (arg);
        while (isdigit (*arg))
            mult = 10 * mult + (*arg++ - '0');
        arg = skip_spaces (arg);
        switch (*arg++)
        {
        case 'd':
            mult *= SECS_PER_REAL_DAY;
            break;
        case 'h':
            mult *= SECS_PER_REAL_HOUR;
            break;
        case 'm':
            mult *= SECS_PER_REAL_MIN;
            break;
        default:
            return 0;
        }
        arg = skip_spaces (arg);
        now += mult;
    }

    return now;
}


void
add_ban (class unit_data * ch, char *site, char type, time_t * until,
         char *textfile)
{
    struct ban_t *entry;
    char d[50], buf[MAX_STRING_LENGTH];

    for (entry = ban_list; entry; entry = entry->next)
        if (!str_ccmp (entry->site, site))
            break;

    if (!entry)
    {
        CREATE (entry, struct ban_t, 1);
        entry->site = str_dup (site);
        if (!str_is_empty (textfile))
            entry->textfile = str_dup (textfile);
        else
        {
            if (type == BAN_TOTAL)
                entry->textfile = str_dup ("bantotal.txt");
            else
                entry->textfile = str_dup ("bannew.txt");
        }
        entry->until = 0;
        entry->next = ban_list;
        ban_list = entry;
    }

    entry->type = type;
    entry->until = *until;

    if (*until)
        strftime (d, 50, "Lasting 'til %a %b %d %H:%M.", localtime (until));
    else
        strcpy (d, "");

    sprintf (buf, "Ban on %s, type %s, textfile %s. %s<br/>",
             entry->site,
             type == BAN_TOTAL ? "TOTAL" : "NEW CHARS", entry->textfile, d);
    send_to_char (buf, ch);
    save_ban ();
}


void
kill_entry (struct ban_t *entry)
{
    if (entry == ban_list)
        ban_list = entry->next;
    else
    {
        struct ban_t *tmp;

        for (tmp = ban_list; tmp; tmp = tmp->next)
            if (tmp->next == entry)
                break;
        tmp->next = entry->next;
    }

    FREE (entry->site);
    if (entry->textfile)
        FREE (entry->textfile);
    FREE (entry);

    save_ban ();
}


void
del_ban (class unit_data * ch, char *site)
{
    struct ban_t *entry;

    for (entry = ban_list; entry; entry = entry->next)
        if (!str_ccmp (entry->site, site))
            break;

    if (entry)
    {
        kill_entry (entry);
        act ("$2t taken succesfully off ban list.",
             A_ALWAYS, ch, site, cActParameter(), TO_CHAR);
    }
    else
        act ("No entry $2t in ban list.", A_ALWAYS, ch, site, cActParameter(), TO_CHAR);
}


void
show_site (class unit_data * ch, struct ban_t *entry)
{
    char buf[200], d[40];

    if (entry->until)
        strftime (d, 40, "until %a %b %d %H:%M", localtime (&entry->until));
    else
        strcpy (d, " ");
    sprintf (buf, " %-30s : %-9s %s %s<br/>", entry->site,
             (entry->type == BAN_TOTAL) ? "TOTAL" : "NEW CHARS",
             entry->textfile, d);
    send_to_char (buf, ch);
}


void
do_ban (class unit_data * ch, char *arg, const struct command_info *cmd)
{
    struct ban_t *tmp;
    char site[MAX_INPUT_LENGTH], textfile[MAX_INPUT_LENGTH], mode;
    char type;
    time_t until = 0;

    if (str_is_empty (arg))
    {
        if (ban_list)
        {
            send_to_char ("Sites banned:<br/>-------------<br/>", ch);
            for (tmp = ban_list; tmp; tmp = tmp->next)
                show_site (ch, tmp);
        }
        else
            send_to_char ("No sites banned.<br/>", ch);

        return;
    }

    mode = tolower (*arg);
    ++arg;

    if (!str_is_empty (arg))
    {
        arg = one_argument (arg, site);
        arg = one_argument (arg, textfile);

        switch (mode)
        {
        case 'n':
        case 't':
            type = (mode == 't') ? BAN_TOTAL : BAN_NEW_CHARS;

            if (!str_is_empty (arg) && (until = ban_timer (arg)) == 0)
                send_to_char ("Wrong syntax in time. Not banned.<br/>.", ch);
            else
                add_ban (ch, site, type, &until, textfile);
            return;

        case 'd':
            del_ban (ch, site);
            return;
        }
    }

    send_to_char ("Usage:<br/>  >ban <t|n> <site> [textfile] [<period>]<br/>"
                  "  >ban d <site><br/>  >ban<br/>", ch);
}


/* Complex little recursive bugger */
bool ban_check (char *ban, char *site)	/* TRUE, if banned */
{
    if (*ban == '\0' && *site == '\0')
        return TRUE;
    else if (*ban == '*')
    {
        if (*site == '\0' || *++ban == '\0')
            return TRUE;
        for (;;)
        {
            if (ban_check (ban, site))
                return TRUE;
            if (*++site == '\0')
                return FALSE;
        }
    }
    else if (*site == '\0')
        return FALSE;
    else if (*ban == '?' || *ban == tolower (*site))
        return ban_check (++ban, ++site);
    else
        return FALSE;
}


char
site_banned (char *cur_site)
{
    struct ban_t *entry, *next_entry;
    time_t now = time (0);

    for (entry = ban_list; entry; entry = next_entry)
    {
        next_entry = entry->next;

        if (entry->until && entry->until < now)
            kill_entry (entry);
        else if (ban_check (entry->site, cur_site))
            return entry->type;
    }

    return NO_BAN;
}


void
show_ban_text (char *site, class descriptor_data *d)
{
    struct ban_t *entry;
    char bantext[MAX_STRING_LENGTH];
    char formtext[MAX_STRING_LENGTH];

    for (entry = ban_list; entry; entry = entry->next)
    {
        if (ban_check (entry->site, site))
        {
            if (file_to_string
                    (str_cc (g_cServerConfig.m_etcdir, entry->textfile), bantext,
                     sizeof (bantext) - 1) == -1)
                send_to_descriptor ("Your site has been banned.<br/>", d);
            else
            {
                str_escape_format (bantext, formtext, sizeof (formtext));
                send_to_descriptor (formtext, d);
            }
            return;
        }
    }
}
