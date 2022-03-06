/*
 $Author: All $
 $RCSfile: vmc.cpp,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.10 $
 */

#include "vmc.h"

#include "common.h"
#include "db_file.h"
#include "dil.h"
#include "error.h"
#include "pp.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "vmc_process.h"

#include <sys/stat.h>
#include <textutil.h>
#include <unistd.h>
#include <utility.h>
#include <utils.h>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <boost/graph/adjacency_list.hpp>

#define MEMBLOCK (200000)
#define BUFS 30

int g_sunlight = 0;
const sbit8 g_time_light[4] = {-1, 0, 1, 0};

zone_info g_zone;
char g_cur_filename[256], top_filename[256];

void zone_reset(char *default_name);
void dump_zone(char *prefix);
long stat_mtime(char *name);
void dil_free_template(diltemplate *tmpl, int copy, int dil = FALSE);
void dil_free_var(dilvar *var);
void dil_free_frame(dilframe *frame);
void dil_free_prg(dilprg *prg, int dil = FALSE);
void write_dot(char *prefix);
void init_lex(char *str);
int yyparse();

int g_make = 0;           /* cmp mod-times of files before compiling */
int g_nooutput = 0;       /* suppress output */
int g_verbose = 0;        /* be talkative */
int g_fatal_warnings = 0; /* allow warnings */
bool g_quiet_compile = false;

char **ident_names = nullptr; /* Used to check unique ident */

struct mem
{
    char *bufs[BUFS + 1];
    int buf;
    long free;
} mm;
/*
*may not be needed
char *inc_dirs[16];							//Must be enough :-)
int inc_count = 0;
*/

void ShowUsage(char *name)
{
    fprintf(stderr, "Usage: %s [-msvlh] [-Idir ..] zonefile ...\n", name);
    fprintf(stderr, "   -m Compile only changed zones.\n");
    fprintf(stderr, "   -s Suppress output of data files.\n");
    fprintf(stderr, "   -v Verbose mode.\n");
    fprintf(stderr, "   -l Ordinary warnings become fatal warnings.\n");
    fprintf(stderr, "   -h This help.\n");
    fprintf(stderr, "   -d Specifiy the location of the money file\n");
    fprintf(stderr, "   -I Search specified dir for include files.\n");
    fprintf(stderr, "   -p preprocess file only, output to stdout.\n");
    fprintf(stderr, "   -q Quiet compile.\n");
    fprintf(stderr, "Copyright 1994 - 2001 (C) by Valhalla.\n");
}

char tmpfile1[L_tmpnam] = "";
char tmpfile2[L_tmpnam] = "";

void dmc_exit(int n)
{
    if (*tmpfile1)
    {
        remove(tmpfile1);
    }

    if (*tmpfile2)
    {
        remove(tmpfile2);
    }

    exit(n);
}

void fix(char *file)
{
    char *p = nullptr;
    char tmp[500];
    char tmp2[500];
    char filename_prefix[256];
    char filename[256];
    int result = 0;

    /* Examine filename */
    strcpy(filename_prefix, file);
    if (!pponly)
    {
        /* find filename w/o .zon-suffix */
        if (!(p = strrchr(filename_prefix, '.')) || strcmp(p + 1, INPUT_SUFFIX))
        {
            fprintf(stderr, "%s: Filename format error.\n", file);
            exit(1);
        }
        *p = '\0';
    }
    if (g_make)
    {
        sprintf(tmp, "%s.%s", filename_prefix, OUTPUT_WSUFFIX);
        sprintf(tmp2, "%s.%s", filename_prefix, OUTPUT_RSUFFIX);
        result = stat_mtime(file);
        if (result <= stat_mtime(tmp) && result <= stat_mtime(tmp2))
        {
            return;
        }
    }
    /* find filename component of path w/o .zon-suffix */
    if (!(p = strrchr(filename_prefix, '/')))
    {
        p = filename_prefix;
    }
    else
    {
        ++p;
    }
    strcpy(filename, p);
    strcpy(g_cur_filename, file);
    strcpy(top_filename, file);
    /* read & write */
    if (!g_quiet_compile)
    {
        fprintf(stderr, "Compiling '%s'\n", g_cur_filename);
    }
    zone_reset(filename);
    if (pp_main(file) > 0)
    {
        fprintf(stderr, "Fatal error compiling in preprocessor stage in file '%s'.\n", g_cur_filename);
        if (sOutput)
            FREE(sOutput);
        exit(1);
    }

    init_lex(sOutput);
    result = yyparse();
    if (sOutput)
    {
        FREE(sOutput);
        sOutput_len = 0;
        sOutput_mlen = 0;
    }
    if (result)
    {
        fprintf(stderr, "Grave errors in file '%s'.\n", g_cur_filename);
        exit(1);
    }
    else if (g_errcon)
    {
        if (g_fatal_warnings)
        {
            fprintf(stderr, "Warnings have been treated as fatal errors.\n");
        }

        fprintf(stderr, "Compilation aborted.\n");
        exit(1);
    }
    else
    {
        dump_zone(filename_prefix);
    }
}

void zone_reset(char *default_name)
{
    mem_reset();
    g_zone.z_rooms = nullptr;
    g_zone.z_mobiles = nullptr;
    g_zone.z_objects = nullptr;
    g_zone.z_table = nullptr;
    g_zone.z_zone.name = default_name;
    g_zone.z_zone.lifespan = 60;
    g_zone.z_zone.reset_mode = 0;
    g_zone.z_zone.creators = nullptr;
    g_zone.z_zone.title = nullptr;
    g_zone.z_zone.notes = nullptr;
    g_zone.z_zone.help = nullptr;
    g_zone.z_zone.weather = 1000;
    g_zone.z_tmpl = nullptr;
}

void mem_init()
{
    int i = 0;

    ident_names = nullptr;
    g_tmplnames = create_namelist();
    CREATE(mm.bufs[0], char, MEMBLOCK);
    mm.buf = 0;
    for (i = 1; i <= BUFS; i++)
    {
        mm.bufs[i] = nullptr;
    }
    mm.free = 0;
}

void mem_reset()
{
    if (ident_names)
    {
        free_namelist(ident_names);
    }

    ident_names = create_namelist();

    mm.buf = 0;
    mm.free = 0;
    mm.buf = 0;
}

void *mmalloc(int size)
{
    long ret = 0;

    if (size > MEMBLOCK)
    {
        fprintf(stderr, "trying to allocate %d bytes which is above our threshold of %d.\n", size, MEMBLOCK);
        exit(1);
    }
    ret = mm.free;

    mm.free += size + (sizeof(int) - (size % sizeof(int)));
    if (mm.free > (MEMBLOCK - 1))
    {
        if (++mm.buf >= BUFS)
        {
            fprintf(stderr, "Out of memory.\n");
            exit(1);
        }
        if (!mm.bufs[mm.buf])
            CREATE(mm.bufs[mm.buf], char, MEMBLOCK);
        mm.free = size + (sizeof(int) - (size % sizeof(int)));
        ret = 0;
    }
    memset(mm.bufs[mm.buf] + ret, '\0', size);
    return mm.bufs[mm.buf] + ret;
}

unit_data *mcreate_unit(int type)
{
    unit_data *rslt = nullptr;

    // rslt = new (class unit_data) (type);
    rslt = new_unit_data(type);

    init_unit(rslt);
    return rslt;
}

unit_fptr *mcreate_func()
{
    return new EMPLACE(unit_fptr) unit_fptr;
}

room_direction_data *mcreate_exit()
{
    room_direction_data *rslt = nullptr;

    rslt = new (room_direction_data);

    //   rslt->open_name = NULL;
    rslt->exit_info = 0;
    rslt->key = nullptr;
    rslt->to_room = nullptr;
    return rslt;
}

unit_affected_type *mcreate_affect()
{
    unit_affected_type *rs = new EMPLACE(unit_affected_type) unit_affected_type;
    rs->setNext(nullptr);
    rs->setID(0);
    rs->setDuration(0);
    rs->setDataAtIndex(0, 0);
    rs->setDataAtIndex(1, 0);
    rs->setDataAtIndex(2, 0);
    rs->setFirstFI(0);
    rs->setTickFI(0);
    rs->setLastFI(0);
    rs->setApplyFI(0);
    return rs;
}

void write_resetcom(FILE *fl, reset_command *c)
{
    char *t = nullptr;
    static char nul[] = {0, 0};

    fwrite(&c->cmd_no, sizeof(c->cmd_no), 1, fl);
    if (c->ref1)
    {
        t = c->ref1;
        fwrite(t, 1, strlen(t) + 1, fl);
        t += strlen(t) + 1;
        fwrite(t, 1, strlen(t) + 1, fl);
    }
    else
    {
        fwrite(nul, 1, 2, fl);
    }
    if (c->ref2)
    {
        t = c->ref2;
        fwrite(t, 1, strlen(t) + 1, fl);
        t += strlen(t) + 1;
        fwrite(t, 1, strlen(t) + 1, fl);
    }
    else
    {
        fwrite(nul, 1, 2, fl);
    }
    fwrite(&c->num1, sizeof(c->num1), 1, fl);
    fwrite(&c->num2, sizeof(c->num2), 1, fl);
    fwrite(&c->num3, sizeof(c->num3), 1, fl);
    fwrite(&c->cmpl, sizeof(c->cmpl), 1, fl);
    fwrite(&c->direction, sizeof(c->direction), 1, fl);
}

void check_unique_ident(unit_data *u)
{
    if (is_name(UNIT_IDENT(u), ident_names))
    {
        fprintf(stderr, "Error: Duplicate symbolic reference '%s'\n", UNIT_IDENT(u));
        exit(1);
    }

    ident_names = add_name(UNIT_IDENT(u), ident_names);
}

/*
   #define write_unit(x,y,z) fprintf(stderr, "Writing: %s\n", z)
   #define write_diltemplate(x,y) fprintf(stderr, "Writing tmpl: %s\n", y->prgname)
   */
void dump_zone(char *prefix)
{
    FILE *fl = nullptr;
    char filename[256];
    char **creators = nullptr;
    unit_data *u = nullptr;
    unit_data *v = nullptr;
    reset_command *c = nullptr;
    int no_rooms = 0;
    diltemplate *tmpl = nullptr;
    diltemplate *ut = nullptr;
    ubit32 dummy = 0;

    /* Quinn, I do this to get all the sematic errors and info */
    /* appear when nooutput = TRUE - it didn't before!         */

    g_error_zone_name = prefix;

    /* Purge the error file */
    sprintf(filename, "%s.err", g_error_zone_name);
    fl = fopen(filename, "w");
    assert(fl);
    fclose(fl);

    if (!is_in(g_zone.z_zone.weather, 960, 1040))
    {
        dmc_error(TRUE,
                  "Error: Zone weather pressure of %d is outside the "
                  "valid range of [960..1040]. (Latter bad, former best).",
                  g_zone.z_zone.weather);
    }

    for (u = g_zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM(u))
        {
            no_rooms++;
        }
        check_unique_ident(u);
        process_unit(u);
    }

    if (no_rooms > 300)
    {
        dmc_error(FALSE,
                  "Warning: %3d rooms in zone [100 maximum suggested "
                  "due to the O(n^2)\n         nature of the shortest path "
                  "algorithm].\n",
                  no_rooms);
    }

    for (u = g_zone.z_objects; u; u = u->next)
    {
        check_unique_ident(u);
        process_unit(u);
    }

    for (u = g_zone.z_mobiles; u; u = u->next)
    {
        check_unique_ident(u);
        process_unit(u);
    }

    if (g_errcon)
    {
        dmc_error(FALSE, "Fatal errors in zone.");
        exit(1);
    }

    if (g_nooutput)
    {
        return;
    }
    sprintf(filename, "%s.%s", prefix, OUTPUT_WSUFFIX);
    if (!(fl = fopen(filename, "w")))
    {
        perror(filename);
        exit(1);
    }

#ifndef DEMO_VERSION
    #ifdef WRITE_TEST
    write_unit(fl, zone.z_rooms, UNIT_IDENT(zone.z_rooms));
    exit(10);
    #endif
    fwrite(g_zone.z_zone.name, sizeof(char), strlen(g_zone.z_zone.name) + 1, fl);
    fwrite(&g_zone.z_zone.weather, sizeof(int), 1, fl);
    /* More data inserted here */
    if (g_zone.z_zone.notes)
    {
        fwrite(g_zone.z_zone.notes, sizeof(char), strlen(g_zone.z_zone.notes) + 1, fl);
    }
    else
    {
        fwrite("", sizeof(char), 1, fl);
    }

    if (g_zone.z_zone.help)
    {
        fwrite(g_zone.z_zone.help, sizeof(char), strlen(g_zone.z_zone.help) + 1, fl);
    }
    else
    {
        fwrite("No help for this zone.", sizeof(char), 23, fl);
    }
    if (g_zone.z_zone.creators)
    {
        // MS2020 for (creators = zone.z_zone.creators; (ubit32)*creators; creators++)
        for (creators = g_zone.z_zone.creators; *creators; creators++)
        {
            fwrite(*creators, sizeof(char), strlen(*creators) + 1, fl);
        }
    }
    fwrite("", sizeof(char), 1, fl);

    if (g_zone.z_zone.title)
    {
        fwrite(g_zone.z_zone.title, sizeof(char), strlen(g_zone.z_zone.title) + 1, fl);
    }
    else
    {
        fwrite("", sizeof(char), 1, fl);
    }

    /* write DIL templates */
    for (tmpl = g_zone.z_tmpl; tmpl; tmpl = tmpl->vmcnext)
    {
        write_diltemplate(fl, tmpl);
    }

    /* end of DIL templates marker */
    dummy = 0;
    if (fwrite(&dummy, sizeof(dummy), 1, fl) != 1)
    {
        error(HERE, "Failed to fwrite() end of DIL templates");
    }

    write_dot(prefix);
    for (u = g_zone.z_rooms; u; u = u->next)
    {
        write_unit(fl, u, UNIT_IDENT(u));
    }

    u = g_zone.z_rooms;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    for (u = g_zone.z_objects; u; u = u->next)
    {
        write_unit(fl, u, UNIT_IDENT(u));
    }

    u = g_zone.z_objects;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    for (u = g_zone.z_mobiles; u; u = u->next)
    {
        write_unit(fl, u, UNIT_IDENT(u));
    }

    u = g_zone.z_mobiles;
    while (u)
    {
        v = u->next;
        delete u;
        u = v;
    }

    tmpl = g_zone.z_tmpl;
    while (tmpl)
    {
        ut = tmpl->vmcnext;
        dil_free_template(tmpl, 0);
        tmpl = ut;
    }

    fwrite("VMC", sizeof(char), 3, fl);
    fclose(fl);

    sprintf(filename, "%s.%s", prefix, OUTPUT_RSUFFIX);
    if (!(fl = fopen(filename, "w")))
    {
        perror(filename);
        exit(1);
    }
    fwrite(&g_zone.z_zone.lifespan, sizeof(unsigned short), 1, fl);
    fwrite(&g_zone.z_zone.reset_mode, sizeof(unsigned char), 1, fl);

    for (c = g_zone.z_table; c; c = c->next)
    {
        write_resetcom(fl, c);
    }

    fwrite("VMC", sizeof(char), 3, fl);
    fclose(fl);
#endif
}

long stat_mtime(char *name)
{
    struct stat buf;

    if (stat(name, &buf) < 0)
    {
        return 0;
    }
    return buf.st_mtime;
}

void szonelog(zone_type *zone, const char *fmt, ...)
{
    /* Not pretty, but it's this or linking with handler.c ;) */
    fprintf(stderr, "%s\n", fmt);
}

void dil_free_prg(dilprg *prg, int dil)
{
    diltemplate *tmpl = nullptr;
    dilframe *frm = nullptr;

    tmpl = prg->frame[0].tmpl;

    for (frm = prg->frame; frm <= (prg->fp); frm++)
    {
        dil_free_frame(frm);
    }
    if (!dil)
        FREE(prg->frame);

    dil_free_template(tmpl, IS_SET(prg->flags, DILFL_COPY), dil);
    if (!dil)
        FREE(prg);
}

void dil_free_var(dilvar *v)
{
    switch (v->type)
    {
        case DILV_SP:
            if (v->val.string)
            {
                FREE(v->val.string);
                v->val.string = nullptr;
            }
            break;

        case DILV_SLP:
            if (v->val.namelist)
            {
                delete v->val.namelist;
                v->val.namelist = nullptr;
            }
            break;

        case DILV_ILP:
            /* Only free if temporary allocated expression */
            if (v->val.intlist)
            {
                delete v->val.intlist;
                v->val.intlist = nullptr;
            }
            break;
    }
}

void dil_free_frame(dilframe *frame)
{
    int j = 0;

    /* free variables */
    for (j = 0; j < frame->tmpl->varc; j++)
    {
        dil_free_var(&frame->vars[j]);
    }

    if (frame->vars)
    {
        FREE(frame->vars);
        frame->vars = nullptr;
    }

    /* discard secures */
    if (frame->secure)
    {
        FREE(frame->secure);
        frame->secure = nullptr;
    }

    /* discard intr */
    if (frame->intr)
    {
        FREE(frame->intr);
        frame->intr = nullptr;
    }
}

void dil_free_template(diltemplate *tmpl, int copy, int dil)
{
    int i = 0;

    if (tmpl->flags == DILFL_FREEME)
    {
        /* free dummy template */
        FREE(tmpl->prgname);
        FREE(tmpl);
    }
    else if (!copy)
    {
        /* free entire inline template */
        FREE(tmpl->prgname);

        if (tmpl->argt)
            FREE(tmpl->argt);

        if (tmpl->vart)
            FREE(tmpl->vart);

        if (tmpl->core)
            FREE(tmpl->core);

        if (tmpl->extprg)
            FREE(tmpl->extprg);

        for (i = 0; i < tmpl->xrefcount; i++)
        {
            if (tmpl->xrefs[i].name)
                FREE(tmpl->xrefs[i].name);
            if (!dil)
            {
                if (tmpl->xrefs[i].argt)
                    FREE(tmpl->xrefs[i].argt);
            }
        }

        if (tmpl->xrefs)
            FREE(tmpl->xrefs);
        if (!dil)
            FREE(tmpl);
    }
}

void graph_sc(char *prefix)
{
    unit_data *u = nullptr;
    int x = 0;
    for (x = 0, u = g_zone.z_rooms; u; u = u->next, x++)
    {
        if (IS_ROOM(u))
        {
            ROOM_NUM(u) = x;
        }
    }

    typedef boost::adjacency_list<> ZoneGraph;

    ZoneGraph G(x);
    for (u = g_zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM(u))
        {
            add_edge(ROOM_NUM(u), 0, G);
        }
    }
}

void write_dot(char *prefix)
{
    char dotfilename[256];
    std::ostringstream interconnect;
    unit_data *u = nullptr;
    sprintf(dotfilename, "%s.%s", prefix, "dot");
    std::ofstream dotfl(dotfilename);
    if (!(dotfl))
    {
        perror(dotfilename);
        exit(1);
    }

    dotfl << "subgraph \"cluster@" << g_zone.z_zone.name << "\" {" << std::endl << "  label=\"" << g_zone.z_zone.name << "\";" << std::endl;

    dotfl << std::endl << "  /* Room Labels */" << std::endl;

    for (u = g_zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM(u))
        {
            dotfl << "\"" << UNIT_IDENT(u) << "@" << g_zone.z_zone.name << "\" "
                  << "[label=\"" << UNIT_IDENT(u) << "\"];" << std::endl;
        }
    }

    dotfl << std::endl << "/* Room Interconnects */" << std::endl;
    for (u = g_zone.z_rooms; u; u = u->next)
    {
        if (IS_ROOM(u))
        {
            for (int i = 0; i < MAX_EXIT; i++)
            {
                if (ROOM_EXIT(u, i))
                {
                    const char *c1 = "";
                    const char *c2 = "";

                    if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->to_room)
                    {
                        c1 = (char *)ROOM_EXIT(u, i)->to_room;
                        c2 = c1;
                        TAIL(c2);
                        c2++;
                    }

                    if (strcmp(c1, g_zone.z_zone.name) == 0)
                    {
                        dotfl << "\"" << UNIT_IDENT(u) << "@" << g_zone.z_zone.name << "\"->\"" << c2 << "@" << c1 << "\";" << std::endl;
                    }
                    else
                    {
                        interconnect << "\"" << UNIT_IDENT(u) << "@" << g_zone.z_zone.name << "\"->\"" << c2 << "@" << c1 << "\";"
                                     << std::endl;
                    }
                }
            }
        }
    }
    dotfl << "}" << std::endl;
    dotfl << std::endl << "/*  Zone Interconnect Points */" << std::endl << std::endl;
    interconnect << std::endl << "/*  End of subgraph " << g_zone.z_zone.name << " */" << std::endl << std::ends;
    dotfl << interconnect.str() << std::endl;
    dotfl.close();
}
