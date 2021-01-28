/*
 $Author: All $
 $RCSfile: vmc.cpp,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.10 $
 */

#include <boost/config.hpp>
#include <algorithm>
#include <iostream>
#include <boost/graph/adjacency_list.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <sstream>


using namespace std;

#ifdef WINDOWS

#else
#include <unistd.h>
#endif
#undef WRITE_TEST
#include <sys/types.h>
#include <sys/stat.h>
#include "pp.h"
#include <utils.h>
#include "vmc.h"
#include <dil.h>
#include <textutil.h>
#include <utility.h>
#include <db_file.h>
#include <common.h>

#define MEMBLOCK 65536
#define BUFS 30

int sunlight = 0;
const sbit8 time_light[4] = {
    -1,
        0,
        1,
        0
    };

struct zone_info zone;
char cur_filename[256], top_filename[256];

void boot_money (char *moneyfile);
void fix (char *file);
void zone_reset (char *default_name);
void init_unit (class unit_data * u);
void dump_zone (char *prefix);
long stat_mtime (char *name);
void dil_free_template (struct diltemplate *tmpl, int copy, int dil = FALSE);
void dil_free_var (struct dilvar *var);
void dil_free_frame (struct dilframe *frame);
void dil_free_prg (struct dilprg *prg, int dil = FALSE);
void write_dot (char *prefix);

int make = 0,										/* cmp mod-times of files before compiling */
    nooutput = 0,									/* suppress output */
    verbose = 0,									/* be talkative */
    fatal_warnings = 0;						/* allow warnings */

extern int errcon;
extern char *error_zone_name;

char **ident_names = NULL;			/* Used to check unique ident */

struct mem
{
    char *bufs[BUFS + 1];
    int buf;
    long free;
}
mm;
/*
*may not be needed
char *inc_dirs[16];							//Must be enough :-)
int inc_count = 0;
*/

void ShowUsage (char *name)
{
    fprintf (stderr, "Usage: %s [-msvlh] [-Idir ..] zonefile ...\n", name);
    fprintf (stderr, "   -m Compile only changed zones.\n");
    fprintf (stderr, "   -s Suppress output of data files.\n");
    fprintf (stderr, "   -v Verbose mode.\n");
    fprintf (stderr, "   -l Ordinary warnings become fatal warnings.\n");
    fprintf (stderr, "   -h This help.\n");
    fprintf (stderr, "   -d Specifiy the location of the money file\n");
    fprintf (stderr, "   -I Search specified dir for include files.\n");
    fprintf (stderr, "   -p preprocess file only, output to stdout.\n");
    fprintf (stderr, "Copyright 1994 - 2001 (C) by Valhalla.\n");
}


int main(int argc, char **argv)
{
    int pos;
    char moneyfile[512], *money;

    mem_init ();

    money = getenv ("VME_MONEY");
    if (money)
        strcpy (moneyfile, money);
    else
        strcpy (moneyfile, "../etc/");
    if (argc == 1)
    {
        ShowUsage (argv[0]);
        exit (1);
    }

    /*
    * not sure why this is here anymore.
    inc_dirs[inc_count++] = CPPI;
    */

    fprintf (stderr, "VMC %s Copyright (C) 2001 by Valhalla [%s]\n",
             VERSION, __DATE__);

    for (pos = 1; pos < argc; pos++)
        if (*argv[pos] == '-')
            switch (*(argv[pos] + 1))
            {
            case 'I':
                Ipath[Ipcnt++] = argv[pos] + 2;;
                break;
            case 'm':
                make = 1;
                break;
            case 's':
                nooutput = 1;
                break;
            case 'v':
                verbose = 1;
                break;
            case 'p':
                pponly = 1;
                break;
            case 'l':
                fatal_warnings = 1;
                break;
            case 'd':
                if (*(argv[pos] + 2))
                    strcpy (moneyfile, argv[pos] + 2);
                else if (++pos < argc)
                    strcpy (moneyfile, argv[pos]);
                else
                {
                    slog (LOG_OFF, 0, "Full path of the money file expected.");
                    exit (1);
                }
                break;

            case '?':
                ShowUsage (argv[0]);
                exit (0);
            case 'h':
                ShowUsage (argv[0]);
                exit (0);

            default:
                fprintf (stderr, "Unknown option '%c'.\n", *(argv[pos] + 1));
                ShowUsage (argv[0]);
                exit (0);
            }
        else
        {
#ifndef WINDOWS
            alarm (15 * 60);				/* If not done in 5 minutes, abort */
#endif
            boot_money(moneyfile);	/* I guess it was inevitable... /gnort */
            fix (argv[pos]);
        }

    fprintf (stderr, "VMC Done.\n");

    return 0;
}

char tmpfile1[L_tmpnam] = "";
char tmpfile2[L_tmpnam] = "";

void
dmc_exit (int n)
{
    if (*tmpfile1)
        remove (tmpfile1);

    if (*tmpfile2)
        remove (tmpfile2);

    exit (n);
}

void fix(char *file)
{
    char *p, tmp[500], tmp2[500], filename_prefix[256], filename[256];
    int  result;
    void init_lex (char *str);
    int yyparse (void);

    /* Examine filename */
    strcpy (filename_prefix, file);
    if (!pponly)
    {
        /* find filename w/o .zon-suffix */
        if (!(p = strrchr (filename_prefix, '.'))
                || strcmp (p + 1, INPUT_SUFFIX))
        {
            fprintf (stderr, "%s: Filename format error.\n", file);
            exit (1);
        }
        *p = '\0';
    }
    if (make)
    {
        sprintf (tmp, "%s.%s", filename_prefix, OUTPUT_WSUFFIX);
        sprintf (tmp2, "%s.%s", filename_prefix, OUTPUT_RSUFFIX);
        result = stat_mtime (file);
        if (result <= stat_mtime (tmp) && result <= stat_mtime (tmp2))
            return;
    }
    /* find filename component of path w/o .zon-suffix */
    if (!(p = strrchr (filename_prefix, '/')))
        p = filename_prefix;
    else
        ++p;
    strcpy (filename, p);
    strcpy (cur_filename, file);
    strcpy (top_filename, file);
    /* read & write */
    fprintf (stderr, "Compiling '%s'\n",cur_filename);
    zone_reset (filename);
    if (pp_main (file) > 0)
    {
        fprintf (stderr,
                 "Fatal error compiling in preprocessor stage in file '%s'.\n",
                 cur_filename);
        if (sOutput)
            FREE (sOutput);
        exit (1);
    }



    init_lex (sOutput);
    result = yyparse ();
    if (sOutput)
    {
        FREE (sOutput);
        sOutput_len = 0;
        sOutput_mlen = 0;
    }
    if (result)
    {
        fprintf (stderr, "Grave errors in file '%s'.\n", cur_filename);
        exit (1);
    }
    else if (errcon)
    {
        if (fatal_warnings)
            fprintf (stderr, "Warnings have been treated as fatal errors.\n");

        fprintf (stderr, "Compilation aborted.\n");
        exit (1);
    }
    else
        dump_zone (filename_prefix);
}


void
zone_reset (char *default_name)
{

    mem_reset ();
    zone.z_rooms = 0;
    zone.z_mobiles = 0;
    zone.z_objects = 0;
    zone.z_table = 0;
    zone.z_zone.name = default_name;
    zone.z_zone.lifespan = 60;
    zone.z_zone.reset_mode = 0;
    zone.z_zone.creators = 0;
    zone.z_zone.title = 0;
    zone.z_zone.notes = 0;
    zone.z_zone.help = 0;
    zone.z_zone.weather = 1000;
    zone.z_tmpl = NULL;
}


void
mem_init ()
{
    extern char **tmplnames;
    int i;

    ident_names = NULL;
    tmplnames = create_namelist ();
    CREATE (mm.bufs[0], char, MEMBLOCK);
    mm.buf = 0;
    for (i = 1; i <= BUFS; i++)
        mm.bufs[i] = 0;
    mm.free = 0;
}

void mem_reset()
{
    if (ident_names)
        free_namelist (ident_names);

    ident_names = create_namelist ();

    mm.buf = 0;
    mm.free = 0;
    mm.buf = 0;
}


void *
mmalloc (int size)
{
    long ret;

    if (size > MEMBLOCK)
    {
        fprintf (stderr, "trying to allocate too-large chunk.\n");
        exit (1);
    }
    ret = mm.free;

    mm.free += size + (sizeof (int) - (size % sizeof (int)));
    if (mm.free > (MEMBLOCK - 1))
    {
        if (++mm.buf >= BUFS)
        {
            fprintf (stderr, "Out of memory.\n");
            exit (1);
        }
        if (!mm.bufs[mm.buf])
            CREATE (mm.bufs[mm.buf], char, MEMBLOCK);
        mm.free = size + (sizeof (int) - (size % sizeof (int)));
        ret = 0;
    }
    memset (mm.bufs[mm.buf] + ret, '\0', size);
    return mm.bufs[mm.buf] + ret;
}


class unit_data *mcreate_unit (int type)
{
    class unit_data *rslt;

    // rslt = new (class unit_data) (type);
    rslt = new_unit_data(type);

    init_unit (rslt);
    return rslt;
}


struct unit_fptr *
mcreate_func (void)
{
    return (struct unit_fptr *) mmalloc (sizeof (struct unit_fptr));
}


class room_direction_data *
mcreate_exit (void)
{
    class room_direction_data *rslt;

    rslt = new (class room_direction_data);

//   rslt->open_name = NULL;
    rslt->exit_info = 0;
    rslt->key = 0;
    rslt->to_room = 0;
    return rslt;
}


struct unit_affected_type *
mcreate_affect (void)
{
    struct unit_affected_type *rs;

    MCREATE (rs, struct unit_affected_type, 1);
    rs->next = 0;
    rs->id = 0;
    rs->duration = 0;
    rs->data[0] = rs->data[1] = rs->data[2] = 0;
    rs->firstf_i = rs->tickf_i = rs->lastf_i = rs->applyf_i = 0;
    return rs;
}

void
write_resetcom (FILE * fl, struct reset_command *c)
{
    char *t;
    static char nul[] = { 0, 0 };

    fwrite (&c->cmd_no, sizeof (c->cmd_no), 1, fl);
    if (c->ref1)
    {
        t = c->ref1;
        fwrite (t, 1, strlen (t) + 1, fl);
        t += strlen (t) + 1;
        fwrite (t, 1, strlen (t) + 1, fl);
    }
    else
        fwrite (nul, 1, 2, fl);
    if (c->ref2)
    {
        t = c->ref2;
        fwrite (t, 1, strlen (t) + 1, fl);
        t += strlen (t) + 1;
        fwrite (t, 1, strlen (t) + 1, fl);
    }
    else
        fwrite (nul, 1, 2, fl);
    fwrite (&c->num1, sizeof (c->num1), 1, fl);
    fwrite (&c->num2, sizeof (c->num2), 1, fl);
    fwrite (&c->num3, sizeof (c->num3), 1, fl);
    fwrite (&c->cmpl, sizeof (c->cmpl), 1, fl);
    fwrite (&c->direction, sizeof (c->direction), 1, fl);
}


void check_unique_ident (class unit_data * u)
{
    if (is_name(UNIT_IDENT (u), ident_names))
    {
        fprintf (stderr, "Error: Duplicate symbolic reference '%s'\n",
                 UNIT_IDENT (u));
        exit (1);
    }

    ident_names = add_name (UNIT_IDENT (u), ident_names);
}


/*
   #define write_unit(x,y,z) fprintf(stderr, "Writing: %s\n", z)
   #define write_diltemplate(x,y) fprintf(stderr, "Writing tmpl: %s\n", y->prgname)
   */
void dump_zone (char *prefix)
{
    FILE *fl;
    char filename[256];
    char **creators;
    class unit_data *u, *v;
    struct reset_command *c;
    int no_rooms = 0;
    struct diltemplate *tmpl, *ut;
    ubit32 dummy;
    void dmc_error (int fatal, const char *fmt, ...);

    /* Quinn, I do this to get all the sematic errors and info */
    /* appear when nooutput = TRUE - it didn't before!         */

    error_zone_name = prefix;

    /* Purge the error file */
    sprintf (filename, "%s.err", error_zone_name);
    fl = fopen (filename, "w");
    assert (fl);
    fclose (fl);

    if (!is_in (zone.z_zone.weather, 960, 1040))
    {
        dmc_error (TRUE, "Error: Zone weather pressure of %d is outside the "
                   "valid range of [960..1040]. (Latter bad, former best).",
                   zone.z_zone.weather);
    }

    for (u = zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM (u))
            no_rooms++;
        check_unique_ident (u);
        process_unit (u);
    }

    if (no_rooms > 150)
    {
        dmc_error (FALSE, "Warning: %3d rooms in zone [100 maximum suggested "
                   "due to the O(n^2)\n         nature of the shortest path "
                   "algorithm].\n", no_rooms);
    }

    for (u = zone.z_objects; u; u = u->next)
    {
        check_unique_ident (u);
        process_unit (u);
    }

    for (u = zone.z_mobiles; u; u = u->next)
    {
        check_unique_ident (u);
        process_unit (u);
    }

    if (errcon)
    {
        dmc_error (FALSE, "Fatal errors in zone.");
        exit (1);
    }

    if (nooutput)
        return;
    sprintf (filename, "%s.%s", prefix, OUTPUT_WSUFFIX);
    if (!(fl = fopen (filename, "w")))
    {
        perror (filename);
        exit (1);
    }

#ifndef DEMO_VERSION
#ifdef WRITE_TEST
    write_unit (fl, zone.z_rooms, UNIT_IDENT (zone.z_rooms));
    exit (10);
#endif
    fwrite (zone.z_zone.name, sizeof (char), strlen (zone.z_zone.name) + 1, fl);
    fwrite (&zone.z_zone.weather, sizeof (int), 1, fl);
    /* More data inserted here */
    if (zone.z_zone.notes)
        fwrite (zone.z_zone.notes, sizeof (char), strlen (zone.z_zone.notes) + 1,
                fl);
    else
        fwrite ("", sizeof (char), 1, fl);

    if (zone.z_zone.help)
        fwrite (zone.z_zone.help, sizeof (char), strlen (zone.z_zone.help) + 1,
                fl);
    else
        fwrite ("No help for this zone.", sizeof (char), 23, fl);
    if (zone.z_zone.creators)
    {
        // MS2020 for (creators = zone.z_zone.creators; (ubit32)*creators; creators++)
        for (creators = zone.z_zone.creators; *creators; creators++)
        {
            fwrite (*creators, sizeof (char), strlen (*creators) + 1, fl);
        }
    }
    fwrite ("", sizeof (char), 1, fl);

    if (zone.z_zone.title)
    {
        fwrite (zone.z_zone.title, sizeof (char),
                strlen (zone.z_zone.title) + 1, fl);
    }
    else
        fwrite ("", sizeof (char), 1, fl);

    /* write DIL templates */
    for (tmpl = zone.z_tmpl; tmpl; tmpl = tmpl->vmcnext)
        write_diltemplate (fl, tmpl);


    /* end of DIL templates marker */
    dummy = 0;
    if (fwrite (&dummy, sizeof (dummy), 1, fl) != 1)
        error (HERE, "Failed to fwrite() end of DIL templates");

    write_dot (prefix);
    for (u = zone.z_rooms; u; u = u->next)
    {
        write_unit (fl, u, UNIT_IDENT (u));
    }

    u = zone.z_rooms;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    for (u = zone.z_objects; u; u = u->next)
        write_unit (fl, u, UNIT_IDENT (u));

    u = zone.z_objects;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    for (u = zone.z_mobiles; u; u = u->next)
        write_unit (fl, u, UNIT_IDENT (u));

    u = zone.z_mobiles;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    tmpl = zone.z_tmpl;
    while (tmpl)
    {
        ut = tmpl->vmcnext;
        dil_free_template (tmpl, 0);
        tmpl = ut;
    }

    fwrite ("VMC", sizeof (char), 3, fl);
    fclose (fl);

    sprintf (filename, "%s.%s", prefix, OUTPUT_RSUFFIX);
    if (!(fl = fopen (filename, "w")))
    {
        perror (filename);
        exit (1);
    }
    fwrite (&zone.z_zone.lifespan, sizeof (unsigned short), 1, fl);
    fwrite (&zone.z_zone.reset_mode, sizeof (unsigned char), 1, fl);

    for (c = zone.z_table; c; c = c->next)
        write_resetcom (fl, c);

    fwrite ("VMC", sizeof (char), 3, fl);
    fclose (fl);
#endif
}

long
stat_mtime (char *name)
{
    struct stat buf;

    if (stat (name, &buf) < 0)
        return 0;
    return buf.st_mtime;
}

void szonelog (struct zone_type *zone, const char *fmt, ...)
{
    /* Not pretty, but it's this or linking with handler.c ;) */
    fprintf (stderr, "%s\n", fmt);
}

void
dil_free_prg (struct dilprg *prg, int dil)
{
    struct diltemplate *tmpl;
    struct dilframe *frm;

    tmpl = prg->frame[0].tmpl;

    for (frm = prg->frame; frm <= (prg->fp); frm++)
        dil_free_frame (frm);
    if (!dil)
        FREE (prg->frame);

    dil_free_template (tmpl, IS_SET (prg->flags, DILFL_COPY), dil);
    if (!dil)
        FREE (prg);
}


void
dil_free_var (struct dilvar *v)
{
    switch (v->type)
    {
    case DILV_SP:
        if (v->val.string)
        {
            FREE (v->val.string);
            v->val.string = NULL;
        }
        break;

    case DILV_SLP:
        if (v->val.namelist)
        {
            delete v->val.namelist;
            v->val.namelist = NULL;
        }
        break;

    case DILV_ILP:
        /* Only free if temporary allocated expression */
        if (v->val.intlist)
        {
            delete v->val.intlist;
            v->val.intlist = NULL;
        }
        break;
    }
}


void
dil_free_frame (struct dilframe *frame)
{
    int j;

    /* free variables */
    for (j = 0; j < frame->tmpl->varc; j++)
        dil_free_var (&frame->vars[j]);

    if (frame->vars)
    {
        FREE (frame->vars);
        frame->vars = NULL;
    }

    /* discard secures */
    if (frame->secure)
    {
        FREE (frame->secure);
        frame->secure = NULL;
    }

    /* discard intr */
    if (frame->intr)
    {
        FREE (frame->intr);
        frame->intr = NULL;
    }
}

void dil_free_template (struct diltemplate *tmpl, int copy, int dil)
{
    int i;

    if (tmpl->flags == DILFL_FREEME)
    {
        /* free dummy template */
        FREE (tmpl->prgname);
        FREE (tmpl);
    }
    else if (!copy)
    {
        /* free entire inline template */
        FREE (tmpl->prgname);

        if (tmpl->argt)
            FREE (tmpl->argt);

        if (tmpl->vart)
            FREE (tmpl->vart);

        if (tmpl->core)
            FREE (tmpl->core);

        if (tmpl->extprg)
            FREE (tmpl->extprg);

        for (i = 0; i < tmpl->xrefcount; i++)
        {
            if (tmpl->xrefs[i].name)
                FREE (tmpl->xrefs[i].name);
            if (!dil)
                if (tmpl->xrefs[i].argt)
                    FREE (tmpl->xrefs[i].argt);
        }

        if (tmpl->xrefs)
            FREE (tmpl->xrefs);
        if (!dil)
            FREE (tmpl);
    }
}

void
graph_sc( char *prefix )
{
    using namespace boost;
    class unit_data *u;
    int x;
    for (x=0,u=zone.z_rooms; u; u=u->next,x++)
        if (IS_ROOM(u))
            ROOM_NUM(u)=x;

    typedef adjacency_list<> ZoneGraph;

    ZoneGraph G(x);
    for (u=zone.z_rooms; u; u=u->next)
        if (IS_ROOM(u))
        {
            add_edge(ROOM_NUM(u), 0, G);

        }

}

void
write_dot (char *prefix)
{
    char dotfilename[256];
    ostringstream interconnect;
    class unit_data *u;
    sprintf (dotfilename, "%s.%s", prefix, "dot");
    ofstream dotfl (dotfilename);
    if (!(dotfl))
    {
        perror (dotfilename);
        exit (1);
    }

    dotfl << "subgraph \"cluster@" << zone.z_zone.name << "\" {" << endl <<
          "  label=\"" << zone.z_zone.name << "\";" << endl;

    dotfl << endl << "  /* Room Labels */" << endl;

    for (u = zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM (u))
            dotfl << "\"" << UNIT_IDENT (u) << "@" << zone.z_zone.name << "\" "
                  << "[label=\"" << UNIT_IDENT (u) << "\"];" << endl;
    }

    dotfl << endl << "/* Room Interconnects */" << endl;
    for (u = zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM (u))
            for (int i = 0; i < MAX_EXIT; i++)
            {
                if (ROOM_EXIT (u, i))
                {
                    const char *c1 = "";
                    const char *c2 = "";

                    if (ROOM_EXIT (u, i) && ROOM_EXIT (u, i)->to_room)
                    {
                        c1 = (char *) ROOM_EXIT (u, i)->to_room;
                        c2 = c1;
                        TAIL (c2);
                        c2++;
                    }

                    if (strcmp (c1, zone.z_zone.name) == 0)
                    {
                        dotfl << "\"" << UNIT_IDENT (u) << "@" << zone.z_zone.name
                              << "\"->\"" << c2 << "@" << c1 << "\";" << endl;
                    }
                    else
                    {
                        interconnect << "\"" << UNIT_IDENT (u) << "@"
                                     << zone.z_zone.name
                                     << "\"->\"" << c2 << "@" << c1 << "\";" << endl;
                    }
                }
            }
    }
    dotfl <<  "}" << endl;
    dotfl << endl << "/*  Zone Interconnect Points */" << endl << endl;
    interconnect << endl << "/*  End of subgraph "<< zone.z_zone.name << " */" << endl << ends;
    dotfl << interconnect.str() << endl;
    dotfl.close();
}
