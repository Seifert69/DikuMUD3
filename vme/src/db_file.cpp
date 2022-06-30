/*
 $Author: All $
 $RCSfile: db_file.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.15 $
 */
#include "db_file.h"

#include "bytestring.h"
#include "db.h"
#include "dil.h"
#include "dilrun.h"
#include "dilshare.h"
#include "error.h"
#include "formatter.h"
#include "handler.h"
#include "main_functions.h"
#include "slog.h"
#include "spec_assign.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

int g_nCorrupt = 0; /* > 0 when a unit is corrupt       */

CByteBuffer g_FileBuffer(16384);

// int filbuffer_length = 0;             /* The length of filbuffer         */
// ubit8 *filbuffer = 0;                 /* Buffer for read/write unit      */

/// @returns 1 on error, 0 if OK
int bread_extra(CByteBuffer *pBuf, extra_list &cExtra, int unit_version)
{
    extra_descr_data *e = nullptr;
    extra_descr_data *te = nullptr;
    extra_descr_data *first = nullptr;
    ubit32 i = 0; // for 32 bit extra lists
    char *c = nullptr;
    int corrupt = 0;

    assert(cExtra.isempty());
    te = nullptr;

    if (unit_version > 62)
    {
        i = pBuf->ReadU32(&corrupt);
        if (corrupt)
        {
            return 1; /* No of extra descriptions */
        }
    }
    else
    {
        i = pBuf->ReadU8(&corrupt);
        if (corrupt)
        {
            return 1; /* No of extra descriptions */
        }
    }

    first = nullptr;

    /* While description is non null, keep reading */
    for (; i > 0; i--)
    {
        if (pBuf->SkipString(&c))
        {
            return 1;
        }

        if (unit_version < 70)
        {
            c = fix_old_codes_to_html(c);
        }
        e = new (extra_descr_data);
        e->descr = c;
        e->names.ReadBuffer(pBuf, unit_version);
        if (unit_version > 61)
        {
            e->vals.ReadBuffer(pBuf);
        }
        e->next = nullptr;

        /* Insert at END of list to keep the ordering of the names */
        /* as specified in the binary file                         */
        if (first == nullptr)
        {
            first = e;
            te = e;
        }
        else
        {
            te->next = e;
            te = e;
        }
    }

    cExtra.m_pList = first;

    return 0;
}

/**
 * This function reads a DIL template
 */
diltemplate *bread_diltemplate(CByteBuffer *pBuf, int version)
{
#ifdef DMSERVER
    int valid = 0;
#endif
    int i = 0;
    int j = 0;
    diltemplate *tmpl = nullptr;

    /* read a template */
    CREATE(tmpl, diltemplate, 1);

    tmpl->nInstructions = 0;
    tmpl->nTriggers = 0;
    tmpl->fCPU = 0.0;

    pBuf->ReadStringAlloc(&tmpl->prgname);
    if (version < 64)
    {
        tmpl->flags = pBuf->ReadU8();
    }
    else
    {
        tmpl->flags = pBuf->ReadU32();
    }

    if (version >= 70)
    {
        tmpl->priority = pBuf->ReadU8();
    }
    else
    {
        slog(LOG_ALL, 0, "bread_diltemplate() Version < 70");
        tmpl->priority = 42;
    }

    tmpl->intrcount = pBuf->ReadU16();

    REMOVE_BIT(tmpl->flags, DILFL_EXECUTING | DILFL_CMDBLOCK);

    tmpl->varcrc = pBuf->ReadU16();
    tmpl->corecrc = pBuf->ReadU16();
    tmpl->rtnt = pBuf->ReadU8();
    tmpl->argc = pBuf->ReadU8();

    if (tmpl->argc)
    {
        CREATE(tmpl->argt, DilVarType_e, tmpl->argc);

        for (i = 0; i < tmpl->argc; i++)
        {
            tmpl->argt[i] = DilVarTypeIntToEnum(pBuf->ReadU8()); /* argument types */
        }
    }
    else
    {
        tmpl->argt = nullptr;
    }

    pBuf->ReadBlock(&tmpl->core, &tmpl->coresz);

    tmpl->varc = pBuf->ReadU16(); /* number of variables */

    if (tmpl->varc)
    {
        CREATE(tmpl->vart, DilVarType_e, tmpl->varc);

        for (i = 0; i < tmpl->varc; i++)
        {
            tmpl->vart[i] = DilVarTypeIntToEnum(pBuf->ReadU8());
        }
    }
    else
    {
        tmpl->vart = nullptr;
    }

    tmpl->xrefcount = pBuf->ReadU16();

    if (tmpl->xrefcount)
    {
        CREATE(tmpl->xrefs, dilxref, tmpl->xrefcount);

        /* read the symbolic references */
        for (i = 0; i < tmpl->xrefcount; i++)
        {
            pBuf->ReadStringAlloc(&tmpl->xrefs[i].name);
            tmpl->xrefs[i].rtnt = DilVarTypeIntToEnum(pBuf->ReadU8());
            tmpl->xrefs[i].argc = pBuf->ReadU8();

            if (tmpl->xrefs[i].argc)
            {
                CREATE(tmpl->xrefs[i].argt, DilVarType_e, tmpl->xrefs[i].argc);
                for (j = 0; j < tmpl->xrefs[i].argc; j++)
                {
                    tmpl->xrefs[i].argt[j] = DilVarTypeIntToEnum(pBuf->ReadU8());
                }
            }
            else
            {
                tmpl->xrefs[i].argt = nullptr;
            }
        }
    }
    else
    {
        tmpl->xrefs = nullptr;
    }

#ifdef DMSERVER
    /* Resolve the external references runtime */

    if (tmpl->xrefcount)
    {
        CREATE(tmpl->extprg, diltemplate *, tmpl->xrefcount);
    }
    else
    {
        tmpl->extprg = nullptr;
    }

    if (!g_mud_bootzone)
    {
        /*
         * This template not loaded boottime, so resolve
         * and typecheck the external references now
         */
        for (i = 0; i < tmpl->xrefcount; i++)
        {
            tmpl->extprg[i] = find_dil_template(tmpl->xrefs[i].name);
            valid = 1;

            if (tmpl->extprg[i])
            {
                /* check argument count and types */
                if ((tmpl->xrefs[i].rtnt != tmpl->extprg[i]->rtnt) || (tmpl->xrefs[i].argc != tmpl->extprg[i]->argc))
                {
                    valid = 0;
                }
                for (j = 0; j < tmpl->xrefs[i].argc; j++)
                {
                    if (tmpl->xrefs[i].argt[j] != tmpl->extprg[i]->argt[j])
                    {
                        valid = 0;
                    }
                }
            }
            else
            {
                /* ERROR MESSAGE HERE */
                slog(LOG_ALL, 0, "Cannot resolve external reference '%s'", tmpl->xrefs[i].name);
                valid = 0;
            }
            /* Typecheck error ! */
            if (!valid)
            {
                tmpl->extprg[i] = nullptr;
                /* ERROR MESSAGE HERE */
                slog(LOG_ALL, 0, "Error typechecking reference to '%s'", tmpl->xrefs[i].name);
            }
        }
    }
#else
    /* This is compiletime, no resolve done */
    tmpl->extprg = nullptr;
#endif
    tmpl->prg_list = nullptr;

    return tmpl;
}

/** Reads DIL interrupt list */
void bread_dilintr(CByteBuffer *pBuf, dilprg *prg, int version)
{
    int i = 0;
    ubit32 lab = 0;

    /* read interrupts */
    prg->frame[0].intrcount = pBuf->ReadU16();

    if (prg->frame[0].intrcount)
    {
        CREATE(prg->frame[0].intr, dilintr, prg->frame[0].intrcount);

        for (i = 0; i < prg->frame[0].intrcount; i++)
        {
            prg->frame[0].intr[i].flags = pBuf->ReadU16();
            lab = pBuf->ReadU32();
            prg->frame[0].intr[i].lab = &(prg->frame[0].tmpl->core[lab]);
            if (version > 64)
            {
                lab = pBuf->ReadU32();
            }
            prg->frame[0].intr[i].elab = &(prg->frame[0].tmpl->core[lab]);
        }
    }
    else
    {
        prg->frame[0].intr = nullptr;
    }
}

void bwrite_dilintr(CByteBuffer *pBuf, dilprg *prg)
{
    ubit16 i = 0;
    ubit32 lab = 0;
    ubit32 elab = 0;

    pBuf->Append16(prg->frame[0].intrcount);

    for (i = 0; i < prg->frame[0].intrcount; i++)
    {
        if (!IS_SET(prg->flags, DILFL_RECALL))
        {
            /* non-recall program: clear all interrupts! */
            pBuf->Append16(0);
            pBuf->Append32(0);
            pBuf->Append32(0);
        }
        else
        {
            if (prg->frame[0].intr[i].lab)
            {
                lab = prg->frame[0].intr[i].lab - prg->frame[0].tmpl->core;
                elab = prg->frame[0].intr[i].elab - prg->frame[0].tmpl->core;
            }
            else
            {
                lab = 0;
                elab = 0;
            }

            pBuf->Append16(prg->frame[0].intr[i].flags);
            pBuf->Append32(lab);
            pBuf->Append32(elab);
        }
    }
}

/**
 * This function reads a DIL program
 *
 * Implementation notes:
 *
 * in VMC:
 *   The DIL programs to be copies (from the zones)
 *   are read as a function, that runtime tries
 *   to initialize the copied function (DILP_COPY).
 *
 *   The 'old' type DIL programs, are inline, and
 *   thus have a local template, saved along with
 *   the program specification. (DILP_INLINE).
 *
 *   The loaded templates from DIL compiler are not
 *   typechecked in VMC compiletime.
 *
 * in DIL:
 *   The compiler saves eihter templates or
 *   dil programs with local templates on demand.
 *   The usages of external programs are typechecked.
 *
 * in SERVER:
 *   Upon loading a template when first reading
 *   a zone, no check of xrefs are made, until
 *   all runtime zones have been loaded.
 *
 *   Other runtime loading of templates, require
 *   lookup and typecheck of loaded template.
 *
 * Really curious what "stspec" is.... (MS2020)
 */

void *bread_dil(CByteBuffer *pBuf, unit_data *owner, ubit8 version, unit_fptr *fptr, int stspec)
{
#ifdef DMSERVER
    dilprg *prg = nullptr;
    diltemplate *tmpl = nullptr;
    ubit32 recallpc = 0;
    int i = 0;
    int novar = 0;
    char name[255];
    int bNameRead = 0;

    // prg = new EMPLACE(dilprg) dilprg(owner, stspec);
    // We will create an NOT link program here, link down below
    prg = new EMPLACE(dilprg) dilprg(owner, nullptr);

    /* read new version */
    if (version < 64)
    {
        prg->flags = pBuf->ReadU8();
    }
    else
    {
        prg->flags = pBuf->ReadU32();
    }

    REMOVE_BIT(prg->flags, DILFL_EXECUTING | DILFL_CMDBLOCK);

    if (fptr && IS_SET(prg->flags, DILFL_AWARE))
    {
        fptr->setActivateOnEventFlag(SFB_AWARE);
    }

    if (!IS_SET(prg->flags, DILFL_COPY))
    {
        /* read local program, and convert to global template */
        tmpl = bread_diltemplate(pBuf, version);
        snprintf(name, sizeof(name), "%s", tmpl->prgname);
        dil_free_template(tmpl, IS_SET(prg->flags, DILFL_COPY));
        SET_BIT(prg->flags, DILFL_COPY);
        slog(LOG_ALL, 0, "hula hop");
        bNameRead = 0;
    }
    else
    {
        bNameRead = 1;
        /* lookup template, only runtime */
        pBuf->ReadStringCopy(name, sizeof(name)); /* prg/template name */
    }
    tmpl = find_dil_template(name);

    if (!tmpl)
    {
        /* Make static template containing just the name */
        CREATE(tmpl, diltemplate, 1);
        tmpl->nInstructions = 0;
        tmpl->nTriggers = 0;
        tmpl->fCPU = 0.0;
        tmpl->prgname = str_dup(name);
        tmpl->zone = nullptr;
        tmpl->prg_list = nullptr;
        /* Prevent all execution */
        SET_BIT(prg->flags, DILFL_EXECUTING);
        tmpl->flags |= DILFL_FREEME;
        if (owner->getFileIndex())
        {
            slog(LOG_ALL,
                 0,
                 "bread_dil(): Unit %s@%s DIL template [%s] no longer exists. bNameRead = %d.",
                 owner->getFileIndex()->getName(),
                 owner->getFileIndex()->getZone(),
                 name,
                 bNameRead);
        }
        else
        {
            slog(LOG_ALL, 0, "bread_dil(): Unit has no file index DIL template [%s] no longer exists. bNameRead = %d.", name, bNameRead);
        }
    }

    prg->waitcmd = WAITCMD_MAXINST - 1; /* Command countdown          */
    prg->varcrc = pBuf->ReadU16();      /* variable crc from compiler */
    prg->corecrc = pBuf->ReadU16();     /* core crc from compiler     */
    recallpc = pBuf->ReadU32();         /* recalled PC                */

    if (!IS_SET(prg->flags, DILFL_RECALL))
    {
        recallpc = 0;
    }

    /* read stackframe #0 */

    prg->fp->tmpl = tmpl; /* template                    */
    if (stspec)
    {
        prg->link(tmpl);
    }

    novar = pBuf->ReadU16(); /* the SAVED #vars             */

    if (novar)
    {
        CREATE(prg->fp->vars, dilvar, novar);
    } /* saved variables */
    else
    {
        prg->fp->vars = nullptr;
    }

    for (i = 0; i < novar; i++)
    {
        prg->fp->vars[i].type = DilVarTypeIntToEnum(pBuf->ReadU8());

        switch (prg->fp->vars[i].type)
        {
            case DilVarType_e::DILV_SLP:
                prg->fp->vars[i].val.namelist = new cNamelist;
                prg->fp->vars[i].val.namelist->ReadBuffer(pBuf, version);
                break;
            case DilVarType_e::DILV_ILP:
                prg->fp->vars[i].val.intlist = new cintlist;
                prg->fp->vars[i].val.intlist->ReadBuffer(pBuf);
                break;
            case DilVarType_e::DILV_SP:
                pBuf->ReadStringAlloc(&prg->fp->vars[i].val.string);
                break;
            case DilVarType_e::DILV_INT:
                prg->fp->vars[i].val.integer = pBuf->ReadS32();
                break;
            default:
                prg->fp->vars[i].val.integer = 0;
        }
    }

    /* read interrupt array */
    bread_dilintr(pBuf, prg, version);

    prg->frame[0].securecount = 0;  /* number of secures */
    prg->frame[0].secure = nullptr; /* secured vars */

    /* The static template ends here.... */
    if (tmpl->flags == DILFL_FREEME)
    {
        tmpl->varc = novar;
        tmpl->intrcount = prg->frame[0].intrcount;
        return prg;
    }

    if ((prg->varcrc != tmpl->varcrc) || (novar != tmpl->varc))
    {
        /* state of variables have changed */
        /* s printf(buf,"Var CRC mismatch. prg:%d, tmpl:%d",
           prg->varcrc,tmpl->varcrc);
           slog(LOG_ALL,0,buf); */

        for (i = 0; i < novar; i++)
        {
            dil_free_var(&prg->fp->vars[i]);

            prg->fp->vars[i].val.string = nullptr;
            prg->fp->vars[i].val.integer = 0;
        }

        if (prg->fp->vars)
            FREE(prg->fp->vars);

        if (tmpl->varc)
        {
            CREATE(prg->fp->vars, dilvar, tmpl->varc);
        }
        else
        {
            prg->fp->vars = nullptr;
        }
        for (i = 0; i < tmpl->varc; i++)
        {
            prg->fp->vars[i].type = tmpl->vart[i];
            if (tmpl->vart[i] == DilVarType_e::DILV_SLP)
            {
                prg->fp->vars[i].val.namelist = new cNamelist;
            }
            else if (tmpl->vart[i] == DilVarType_e::DILV_ILP)
            {
                prg->fp->vars[i].val.intlist = new cintlist;
            }
            else
            {
                prg->fp->vars[i].val.string = nullptr;
            }
        }
        prg->varcrc = tmpl->varcrc; /* variables updated! */

        /* free interrupts */
        if (prg->fp->intr)
            FREE(prg->fp->intr);

        if (tmpl->intrcount)
        {
            CREATE(prg->fp->intr, dilintr, tmpl->intrcount);
        }
        else
        {
            prg->fp->intr = nullptr;
        }
        prg->fp->intrcount = tmpl->intrcount;

        /* clear interrupts */
        for (i = 0; i < prg->fp->intrcount; i++)
        {
            prg->fp->intr[i].flags = 0;
            prg->fp->intr[i].lab = nullptr;
        }

        recallpc = 0;
    }

    if ((prg->corecrc) != (tmpl->corecrc))
    {
        // slog(LOG_ALL, 0, "Core CRC mismatch. prg:%d, tmpl:%d", prg->corecrc, tmpl->corecrc);
        recallpc = 0;
        prg->corecrc = tmpl->corecrc;

        /* free interrupts */
        if (prg->fp->intr)
            FREE(prg->fp->intr);

        if (tmpl->intrcount)
        {
            CREATE(prg->fp->intr, dilintr, tmpl->intrcount);
        }
        else
        {
            prg->fp->intr = nullptr;
        }
        prg->fp->intrcount = tmpl->intrcount;

        /* clear interrupts */
        for (i = 0; i < prg->fp->intrcount; i++)
        {
            prg->fp->intr[i].flags = 0;
            prg->fp->intr[i].lab = nullptr;
        }
    }

    prg->frame[0].pc = &(tmpl->core[recallpc]); /* program counter */

    return prg;
#else
    return nullptr;
#endif
}

unit_fptr *bread_func(CByteBuffer *pBuf, ubit8 version, unit_data *owner, int stspec)
{
    unit_fptr *fptr = nullptr;
    unit_fptr *head = nullptr;
    int i = 0;

    if (version < 66)
    {
        i = pBuf->ReadU8(&g_nCorrupt);
    }
    else
    {
        i = pBuf->ReadU16(&g_nCorrupt);
    }

    for (; i > 0; i--)
    {
        if (fptr)
        {
            fptr->setNext(new EMPLACE(unit_fptr) unit_fptr);
            fptr = fptr->getNext();
        }
        else
        {
            head = new EMPLACE(unit_fptr) unit_fptr;
            fptr = head;
        }

        fptr->readFunctionPointerIndexFrom(*pBuf, g_nCorrupt);

        if (fptr->getFunctionPointerIndex() > SFUN_TOP_IDX)
        {
            slog(LOG_ALL, 0, "Illegal func index in bread_func index - corrupt.");
            g_nCorrupt = TRUE;
            return nullptr;
        }

        if (version >= 70)
        {
            fptr->readFunctionPriorityFrom(*pBuf, g_nCorrupt); // MS2020 added
        }
        else
        {
            fptr->setFunctionPriority(43);
        }

        fptr->readHeartBeatFrom(*pBuf, g_nCorrupt);
        fptr->readActivateOnEventFlagsFrom(*pBuf, g_nCorrupt);

        if (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL)
        {
            fptr->setData(bread_dil(pBuf, owner, version, fptr, stspec));
        }
        else if (fptr->getFunctionPointerIndex() == SFUN_DILCOPY_INTERNAL)
        {
            char name[256];
            char *c = nullptr;

            dilargstype *dilargs = nullptr;

            CREATE(dilargs, dilargstype, 1);

            pBuf->ReadStringCopy(name, sizeof(name));
            strcat(name, "/");
            pBuf->SkipString(&c);
            strcat(name, c);

            dilargs->name = str_dup(name);

            dilargs->no = pBuf->ReadU8();

            for (int j = 0; j < dilargs->no; j++)
            {
                dilargs->dilarg[j].type = DilVarTypeIntToEnum(pBuf->ReadU8());

                switch (dilargs->dilarg[j].type)
                {
                    case DilVarType_e::DILV_SLP:
                        pBuf->ReadNames(&dilargs->dilarg[j].data.stringlist, version <= 73 ? 1 : 0);
                        break;

                    case DilVarType_e::DILV_SP:
                        pBuf->ReadStringAlloc(&dilargs->dilarg[j].data.string);
                        break;

                    case DilVarType_e::DILV_INT:
                        dilargs->dilarg[j].data.num = pBuf->ReadS32();
                        break;

                    case DilVarType_e::DILV_ILP:
                        pBuf->ReadIntList(&dilargs->dilarg[j].data.intlist);
                        break;

                    default:
                        error(HERE, "Uhadada");
                }
            }

            fptr->setData(dilargs);
        }
        else
        {
#ifdef DMSERVER
            fptr->removeActivateOnEventFlag(SFB_ALL);
            fptr->setActivateOnEventFlag(g_unit_function_array[fptr->getFunctionPointerIndex()].sfb);
#endif
            fptr->readDataFrom(*pBuf);
        }

        if ((fptr->getHeartBeat()) && (fptr->getHeartBeat() < WAIT_SEC))
        {
            if (fptr->getFunctionPointerIndex() != SFUN_DIL_INTERNAL)
            {
                slog(LOG_ALL, 0, "WARNING: HEARTBEAT LOW (%d) SAVE on %s\n", fptr->getHeartBeat(), owner->getFileIndexSymName());
            }
            else
            {
                slog(LOG_ALL,
                     0,
                     "WARNING: DIL (%s@%s) HEARTBEAT LOW (%d) ON LOAD",
                     (((dilprg *)fptr->getData())->fp->tmpl->prgname ? ((dilprg *)fptr->getData())->fp->tmpl->prgname : "NO NAME"),
                     (((dilprg *)fptr->getData())->fp->tmpl->zone ? ((dilprg *)fptr->getData())->fp->tmpl->zone->getName() : "NO ZONE"),
                     fptr->getHeartBeat());
            }
        }

        fptr->setNext(nullptr);
    }

    return head;
}

void bread_block(FILE *datafile, long file_pos, int length, void *buffer)
{
    /* Search from beginning of file (binary file) */
    if (fseek(datafile, file_pos, SEEK_SET))
    {
        assert(FALSE);
    }

    if (!((int)fread(buffer, sizeof(ubit8), length, datafile) == length))
    {
        assert(FALSE);
    }
}

void bwrite_affect(CByteBuffer *pBuf, unit_affected_type *af)
{
    int i = 0;
    ubit32 nPos = 0;
    ubit32 nOrgPos = pBuf->GetLength();

    pBuf->Append16(0); /* Assume no affects by default */

    for (; af; af = af->getNext())
    {
        i++;
        pBuf->Append16(af->getDuration());
        pBuf->Append16(af->getID());
        pBuf->Append16(af->getBeat());

        pBuf->Append32(af->getDataAtIndex(0));
        pBuf->Append32(af->getDataAtIndex(1));
        pBuf->Append32(af->getDataAtIndex(2));

        pBuf->Append16(af->getFirstFI());
        pBuf->Append16(af->getTickFI());
        pBuf->Append16(af->getLastFI());
        pBuf->Append16(af->getApplyFI());
    }

    assert(i <= 5000);

    if (i > 0)
    {
        nPos = pBuf->GetLength();
        pBuf->SetLength(nOrgPos);

        pBuf->Append16(i);

        pBuf->SetLength(nPos);
    }
}

/**
 * This function writes a DIL template
 */
void bwrite_diltemplate(CByteBuffer *pBuf, diltemplate *tmpl)
{
    int i = 0;
    int j = 0;

    /* write a template */

    /* slog(LOG_ALL, 0, "Write Template: Var %d, Core %d.", tmpl->varcrc, tmpl->corecrc); */

    // fprintf(stderr, "bwrite dil template: %s : corecrc=%d   varcrc=%d\n\r", tmpl->prgname, tmpl->corecrc, tmpl->varcrc);

    if (tmpl->zone)
    {
        auto str = diku::format_to_str("%s@%s", tmpl->prgname, tmpl->zone->getName());
        pBuf->AppendString(str.c_str());
    }
    else
    {
        pBuf->AppendString(tmpl->prgname); /* program name @ zone */
    }

    //      pBuf->Append8 (tmpl->flags);    /* recall, etc */
    pBuf->Append32(tmpl->flags);     /* recall, etc */
    pBuf->Append8(tmpl->priority);   /* priority */
    pBuf->Append16(tmpl->intrcount); /* max number of intr */
    pBuf->Append16(tmpl->varcrc);    /* variable crc from compiler */
    pBuf->Append16(tmpl->corecrc);   /* core crc from compiler */
    pBuf->Append8(tmpl->rtnt);       /* return type */
    pBuf->Append8(tmpl->argc);       /* number of arguments */

    for (i = 0; i < tmpl->argc; i++)
    {
        pBuf->Append8(tmpl->argt[i]); /* argument types */
    }

    /* fprintf(stderr,"Writing core: %d\n",tmpl->coresz); */

    pBuf->AppendBlock(tmpl->core, tmpl->coresz); /* core */

    pBuf->Append16(tmpl->varc); /* number of variables */
    for (i = 0; i < tmpl->varc; i++)
    {
        pBuf->Append8(tmpl->vart[i]); /* variable types */
    }

    pBuf->Append16(tmpl->xrefcount); /* number of external references */

    /* read the symbolic references */
    for (i = 0; i < tmpl->xrefcount; i++)
    {
        pBuf->AppendString(tmpl->xrefs[i].name);
        pBuf->Append8(tmpl->xrefs[i].rtnt);
        pBuf->Append8(tmpl->xrefs[i].argc);

        for (j = 0; j < tmpl->xrefs[i].argc; j++)
        {
            pBuf->Append8(tmpl->xrefs[i].argt[j]);
        }
    }
    /* resolved references are runtime */
}

/**
 * This function writes a DIL program
 */
void bwrite_dil(CByteBuffer *pBuf, dilprg *prg)
{
    int i = 0;
    diltemplate *tmpl = nullptr;

    /* write new version */
    pBuf->Append32(prg->flags); /* from other template? */

    tmpl = prg->frame[0].tmpl;

    if (!IS_SET(prg->flags, DILFL_COPY))
    {
        /* write local template */
        bwrite_diltemplate(pBuf, tmpl);
    }
    else
    {
        if (tmpl->zone)
        {
            auto buf = diku::format_to_str("%s@%s", tmpl->prgname, tmpl->zone->getName());
            pBuf->AppendString(buf.c_str());
        }
        else
        {
            pBuf->AppendString(tmpl->prgname);
        }
    }

    /* slog(LOG_ALL, 0, "Write Program: Var %d, Core %d.",
       prg->varcrc, prg->corecrc); */

    /* variable crc from compiler */
    pBuf->Append16(prg->varcrc);
    // fprintf(stderr, "bwrite dil: %s : corecrc=%d   varcrc=%d\n\r", prg->frame[0].tmpl->prgname, prg->corecrc, prg->varcrc);
    /* core crc from compiler */
    pBuf->Append16(prg->corecrc);

    /* save state of frame 0 ONLY!!!!! */
    pBuf->Append32(prg->frame[0].pc - prg->frame[0].tmpl->core);

    /* write stackframe #0 */
    pBuf->Append16(tmpl->varc); /* the SAVED #vars */

    for (i = 0; i < tmpl->varc; i++)
    {
        pBuf->Append8(prg->frame[0].vars[i].type);

        switch (prg->frame[0].vars[i].type)
        {
            case DilVarType_e::DILV_SLP:
                if (prg->frame[0].vars[i].val.namelist)
                {
                    prg->frame[0].vars[i].val.namelist->AppendBuffer(pBuf);
                }
                else
                {
                    pBuf->Append32(0);
                }
                break;

            case DilVarType_e::DILV_ILP:
                if (prg->frame[0].vars[i].val.intlist)
                {
                    prg->frame[0].vars[i].val.intlist->AppendBuffer(pBuf);
                }
                else
                {
                    pBuf->Append32(0);
                }
                break;

            case DilVarType_e::DILV_SP:
                pBuf->AppendString(prg->frame[0].vars[i].val.string);
                break;

            case DilVarType_e::DILV_INT:
                pBuf->Append32(prg->frame[0].vars[i].val.integer);
                break;

            default:
                break;
        }
    }

    /* write intr */
    bwrite_dilintr(pBuf, prg);
}

void bwrite_func(CByteBuffer *pBuf, unit_fptr *fptr)
{
    char *data = nullptr;
    int i = 0;
    ubit32 nPos = 0;
    ubit32 nOrgPos = pBuf->GetLength();
    pBuf->Append16(0); /* Assume no affects by default */

    for (; fptr; fptr = fptr->getNext())
    {
        assert(fptr->getFunctionPointerIndex() <= SFUN_TOP_IDX);

        data = (char *)fptr->getData();

#ifdef DMSERVER
        if (g_unit_function_array[fptr->getFunctionPointerIndex()].save_w_d == SD_NEVER)
        {
            continue; /* DONT SAVE THIS FROM INSIDE THE GAME! */
        }

        if (fptr->getData() && g_unit_function_array[fptr->getFunctionPointerIndex()].save_w_d == SD_NULL)
        {
            data = nullptr;
        }

        /* Else this is SD_ASCII and we can save anything we like ... :-) */
#endif
        i++;
        pBuf->Append16(fptr->getFunctionPointerIndex());

        if ((fptr->getHeartBeat()) && (fptr->getHeartBeat() < WAIT_SEC))
        {
            if (fptr->getFunctionPointerIndex() != SFUN_DIL_INTERNAL)
            {
                slog(LOG_ALL, 0, "WARNING: HEARTBEAT LOW (%d)\n", fptr->getHeartBeat());
            }
            else
            {
                slog(LOG_ALL,
                     0,
                     "WARNING: DIL (%s@%s) HEARTBEAT LOW (%d) ON SAVE",
                     (((dilprg *)fptr->getData())->fp->tmpl->prgname ? ((dilprg *)fptr->getData())->fp->tmpl->prgname : "NO NAME"),
                     (((dilprg *)fptr->getData())->fp->tmpl->zone ? ((dilprg *)fptr->getData())->fp->tmpl->zone->getName() : "NO ZONE"),
                     fptr->getHeartBeat());
            }
        }

        pBuf->Append8(fptr->getFunctionPriority()); // MS2020 added priority, version 70+
        pBuf->Append16(fptr->getHeartBeat());
        pBuf->Append16(fptr->getAllActivateOnEventFlags());

        if (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL)
        {
            assert(fptr->getData());
            bwrite_dil(pBuf, (dilprg *)fptr->getData());
        }
        else if (fptr->getFunctionPointerIndex() == SFUN_DILCOPY_INTERNAL)
        {
#ifdef VMC_SRC
            assert(fptr->getData());
            dilargstype *dilargs = (dilargstype *)fptr->getData();

            pBuf->AppendDoubleString(dilargs->name);

            pBuf->Append8(dilargs->no);

            for (int j = 0; j < dilargs->no; j++)
            {
                pBuf->Append8(dilargs->dilarg[j].type);

                switch (dilargs->dilarg[j].type)
                {
                    case DilVarType_e::DILV_SLP:
                        pBuf->AppendNames((const char **)dilargs->dilarg[j].data.stringlist, 0);
                        break;
                    case DilVarType_e::DILV_ILP:
                    {
                        int myi = 0;
                        for (myi = 0; myi <= ((int *)dilargs->dilarg[j].data.intlist)[0]; myi++)
                        {
                            pBuf->Append32(((int *)dilargs->dilarg[j].data.intlist)[myi]);
                        }
                    }
                    break;
                    case DilVarType_e::DILV_SP:
                        pBuf->AppendString(dilargs->dilarg[j].data.string);
                        break;
                    case DilVarType_e::DILV_INT:
                        pBuf->Append32(dilargs->dilarg[j].data.num);
                        break;
                    default:
                        error(HERE, "Not possible");
                }
            }
#else
            error(HERE, "Not possible!");
#endif
        }
        else
        {
            if (data)
            {
                pBuf->AppendString((char *)data);
            }
            else
            {
                pBuf->AppendString("");
            }
        }
    }

    assert(i <= 0xFFFF);

    if (i > 0)
    {
        nPos = pBuf->GetLength();
        pBuf->SetLength(nOrgPos);
        pBuf->Append16(i);
        pBuf->SetLength(nPos);
    }
}

void bwrite_block(FILE *datafile, int length, void *buffer)
{
    if (fwrite(buffer, sizeof(ubit8), length, datafile) != (size_t)length)
    {
        assert(FALSE);
    }
}

/** Write unit to string. */
int write_unit_string(CByteBuffer *pBuf, unit_data *u)
{
    int i = 0;

#ifdef DMSERVER
    char zone[FI_MAX_ZONENAME + 1];
    char name[FI_MAX_UNITNAME + 1];
#endif

    ubit32 nPos = pBuf->GetLength();

    pBuf->Append8(UNIT_VERSION); /* Version Number! */

    u->getNames().AppendBuffer(pBuf);

    pBuf->AppendString(u->getTitle().c_str());
    pBuf->AppendString(u->getDescriptionOfOutside().c_str());
    pBuf->AppendString(u->getDescriptionOfInside().c_str());

    if (u->getExtraList().isempty())
    { // MS2020, nasty bug, exd could be NULL and class was called.
        pBuf->Append32(0);
    }
    else
    {
        u->getExtraList().AppendBuffer(pBuf);
    }

    /*      if (IS_PC (u))
      {
    UNIT_EXTRA_ABILITY (u)->AppendBuffer (pBuf);
  UNIT_EXTRA_SPELL (u)->AppendBuffer (pBuf);
  UNIT_EXTRA_SKILL (u)->AppendBuffer (pBuf);
  UNIT_EXTRA_WEAPON (u)->AppendBuffer (pBuf);
       }*/

#ifdef VMC_SRC
    pBuf->AppendDoubleString((char *)u->getKey());
#else
    if (u->getKey())
    {
        split_fi_ref(u->getKey(), zone, name);
        pBuf->AppendString(zone);
        pBuf->AppendString(name);
    }
    else
    {
        pBuf->AppendString("");
        pBuf->AppendString("");
    }
#endif

    pBuf->Append32(u->getManipulate());
    pBuf->Append16(u->getUnitFlags());
    pBuf->Append16(u->getBaseWeight());
    pBuf->Append16(u->getWeight());
    pBuf->Append16(u->getCapacity());

    pBuf->Append32((ubit32)u->getMaximumHitpoints());
    pBuf->Append32((ubit32)u->getCurrentHitpoints());

    pBuf->Append16((ubit16)u->getAlignment());

    pBuf->Append8(u->getOpenFlags());
    pBuf->Append8(u->getOpenDifficulty());
    pBuf->Append8(u->getNumberOfActiveLightSources());
    pBuf->Append8(u->getLightOutput());
    pBuf->Append8(u->getTransparentLightOutput());
    pBuf->Append8(u->getNumberOfCharactersInsideUnit());
    pBuf->Append8(u->getLevelOfWizardInvisibility());

    pBuf->Append16(u->getSize());

    if (u->getUnitType() == UNIT_ST_ROOM)
    {
        /* See if room is to be placed inside another room! */
        pBuf->AppendDoubleString((char *)u->getUnitIn());
    }
    else
    {
        unit_data *inu = nullptr;

        if (u->isPC())
        {
            inu = CHAR_LAST_ROOM(u);
        }
        else if (u->getUnitIn())
        {
#ifdef DMSERVER
            inu = unit_room(u);
#else
            assert(inu == nullptr);
#endif
        }

        if (inu && inu->getFileIndex())
        {
            pBuf->AppendString(inu->getFileIndexZoneName());
            pBuf->AppendString(inu->getFileIndexName());
        }
        else
        {
            pBuf->AppendString("");
            pBuf->AppendString("");
        }
    }

    switch (u->getUnitType())
    {
        case UNIT_ST_NPC:
            pBuf->AppendString(CHAR_MONEY(u) ? CHAR_MONEY(u) : "");
            /* fallthru */

        case UNIT_ST_PC:
            /* fallthru from NPC above */
            pBuf->Append32(CHAR_EXP(u));
            pBuf->Append32(CHAR_FLAGS(u));

            pBuf->Append16((ubit16)CHAR_MANA(u));
            pBuf->Append16((ubit16)CHAR_ENDURANCE(u));

            pBuf->Append8(CHAR_NATURAL_ARMOUR(u));
            pBuf->Append16(CHAR_SPEED(u));

            pBuf->Append16(CHAR_ATTACK_TYPE(u));
            pBuf->Append16(CHAR_RACE(u));

            pBuf->Append16(CHAR_OFFENSIVE(u));
            pBuf->Append16(CHAR_DEFENSIVE(u));

            pBuf->Append8(u->getSex());
            pBuf->Append8(CHAR_LEVEL(u));
            pBuf->Append8(CHAR_POS(u));

            pBuf->Append8(ABIL_TREE_MAX);
            for (i = 0; i < ABIL_TREE_MAX; i++)
            {
                pBuf->Append16(CHAR_ABILITY(u, i));
                if (u->isPC())
                {
                    pBuf->Append8(PC_ABI_LVL(u, i));
                }
            }

            if (u->isPC())
            {
                pBuf->Append8(PC_PROFESSION(u));
                PC_ACCOUNT(u).writeTo(*pBuf);

                pBuf->Append16(PC_LIFESPAN(u));

                pBuf->Append8(PC_SETUP_ECHO(u));
                pBuf->Append8(PC_SETUP_REDRAW(u));
                pBuf->Append8(PC_SETUP_WIDTH(u));
                pBuf->Append8(PC_SETUP_HEIGHT(u));
                pBuf->Append8(PC_SETUP_EMULATION(u));
                pBuf->Append8(PC_SETUP_TELNET(u));
                pBuf->Append8(PC_SETUP_COLOUR(u));
#ifdef DMSERVER
                auto temp = UPC(u)->getColor().save_string();
                pBuf->AppendString(temp.c_str());
#endif
                pBuf->AppendString(UPC(u)->getPromptString());
                pBuf->AppendString(PC_FILENAME(u));
                pBuf->AppendString(PC_PWD(u));

                for (i = 0; i < 5; i++)
                {
                    pBuf->Append32(UPC(u)->getLastHostAtIndex(i));
                }

                pBuf->Append32((ubit32)PC_ID(u));
                pBuf->Append16(PC_CRACK_ATTEMPTS(u));

                pBuf->AppendString(PC_HOME(u));
                pBuf->AppendString(PC_GUILD(u));

                pBuf->Append32((ubit32)0); // OBSOLETE pBuf->Append32((ubit32)PC_GUILD_TIME(u));
                pBuf->Append16(PC_VIRTUAL_LEVEL(u));

                PC_TIME(u).writeTo(*pBuf);

                pBuf->AppendString(PC_BANK(u));
                pBuf->Append32(PC_SKILL_POINTS(u));
                pBuf->Append32(PC_ABILITY_POINTS(u));
                pBuf->Append16(PC_FLAGS(u));

                pBuf->Append16(SPL_TREE_MAX);
                for (i = 0; i < SPL_TREE_MAX; i++)
                {
                    pBuf->Append16(PC_SPL_SKILL(u, i));
                    pBuf->Append8(PC_SPL_LVL(u, i));
                }

                pBuf->Append16(SKI_TREE_MAX);
                for (i = 0; i < SKI_TREE_MAX; i++)
                {
                    pBuf->Append16(PC_SKI_SKILL(u, i));
                    pBuf->Append8(PC_SKI_LVL(u, i));
                }

                pBuf->Append8(WPN_TREE_MAX);
                for (i = 0; i < WPN_TREE_MAX; i++)
                {
                    pBuf->Append16(PC_WPN_SKILL(u, i));
                    pBuf->Append8(PC_WPN_LVL(u, i));
                }

                pBuf->Append16(PC_CRIMES(u));

                pBuf->Append8(3);
                for (i = 0; i < 3; i++)
                {
                    pBuf->Append8(PC_COND(u, i));
                }

                pBuf->Append8(PC_ACCESS_LEVEL(u));

                PC_QUEST(u).AppendBuffer(pBuf);
                PC_INFO(u).AppendBuffer(pBuf);
            }
            else
            {
                for (i = 0; i < WPN_GROUP_MAX; i++)
                {
                    pBuf->Append16(NPC_WPN_SKILL(u, i));
                }

                for (i = 0; i < SPL_GROUP_MAX; i++)
                {
                    pBuf->Append16(NPC_SPL_SKILL(u, i));
                }

                pBuf->Append8(NPC_DEFAULT(u));
                pBuf->Append8(NPC_FLAGS(u));
            }
            break;

        case UNIT_ST_OBJ:
            pBuf->Append32(OBJ_VALUE(u, 0));
            pBuf->Append32(OBJ_VALUE(u, 1));
            pBuf->Append32(OBJ_VALUE(u, 2));
            pBuf->Append32(OBJ_VALUE(u, 3));
            pBuf->Append32(OBJ_VALUE(u, 4));
            pBuf->Append32(OBJ_FLAGS(u));
            pBuf->Append32(OBJ_PRICE(u));
            pBuf->Append32(OBJ_PRICE_DAY(u));

            pBuf->Append8(OBJ_TYPE(u));
            pBuf->Append8(OBJ_RESISTANCE(u));
            break;

        case UNIT_ST_ROOM:
            /* Read N,S,E,W,U and D directions */
            for (i = 0; i <= MAX_EXIT; i++)
            {
                char *c1 = nullptr;
                char *c2 = nullptr;

                if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->getToRoom())
                {
                    c1 = (char *)ROOM_EXIT(u, i)->getToRoom();
                    c2 = c1;
                    TAIL(c2);
                    c2++;
                }
                if (ROOM_EXIT(u, i) && c1 && c2)
                {
                    pBuf->AppendDoubleString((char *)ROOM_EXIT(u, i)->getToRoom());
                    ROOM_EXIT(u, i)->getOpenName().AppendBuffer(pBuf);
                    pBuf->Append16(ROOM_EXIT(u, i)->getDoorFlags());
                    pBuf->Append8(ROOM_EXIT(u, i)->getSkillDifficulty()); // V71 MS2020
                    pBuf->AppendDoubleString((char *)ROOM_EXIT(u, i)->getKey());
                }
                else
                {
                    pBuf->AppendString(""); /* Null Zone name */
                    pBuf->AppendString(""); /* Null name      */
                }
            }
            pBuf->Append16(ROOM_FLAGS(u));
            pBuf->Append8(ROOM_LANDSCAPE(u));
            pBuf->Append8(ROOM_RESISTANCE(u));
            pBuf->Append16(UROOM(u)->getMapXCoordinate()); // Version 70
            pBuf->Append16(UROOM(u)->getMapYCoordinate());
            break;

        default:
            assert(FALSE);
            break;
    }

    bwrite_affect(pBuf, u->getUnitAffected());

    bwrite_func(pBuf, u->getFunctionPointer());

    return pBuf->GetLength() - nPos;
}

/**
 * Appends unit 'u' to file 'f'. Name is the unique name
 * Used only by dmc.
 */
void write_unit(FILE *f, unit_data *u, char *fname)
{
    CByteBuffer *pBuf = nullptr;
    ubit32 nSizeStart = 0;
    ubit32 nStart = 0;
    ubit32 nPos = 0;
    ubit32 length = 0;
    ubit32 crc = 0;

    pBuf = &g_FileBuffer;
    pBuf->Clear();

    pBuf->AppendString(fname);       /* Write unique name  */
    pBuf->Append8(u->getUnitType()); /* Write unit type    */

    nSizeStart = pBuf->GetLength();
    pBuf->Append32(0); /* Write dummy length */
    pBuf->Append32(0); /* Write dummy CRC    */

    nStart = pBuf->GetLength();

    length = write_unit_string(pBuf, u);

    /* Calculate the CRC */
    crc = length;

    for (ubit32 i = 0; i < length; i++)
    {
        crc += (pBuf->GetData()[nStart + i] << (i % 16));
    }

    nPos = pBuf->GetLength();
    pBuf->SetLength(nSizeStart);

    /* Lets write the calculated length of the unit itself */
    pBuf->Append32(length);

    /* Lets write the calculated length of the unit itself */
    pBuf->Append32(crc);

    pBuf->SetLength(nPos);

    /* Lets write the entire block, including name, type and length info */
    pBuf->FileWrite(f);
}

/**
 * Append template 'tmpl' to file 'f'
 * Used only by dmc. for writing zones
 */
void write_diltemplate(FILE *f, diltemplate *tmpl)
{
    CByteBuffer *pBuf = nullptr;
    ubit32 length = 0;
    ubit32 nStart = 0;
    ubit32 nPos = 0;

    pBuf = &g_FileBuffer;
    pBuf->Clear();

    pBuf->Append32(0); /* Write dummy length */
    nStart = pBuf->GetLength();

    bwrite_diltemplate(pBuf, tmpl);

    /* We are now finished, and are positioned just beyond last data byte */
    length = pBuf->GetLength() - nStart;

    nPos = pBuf->GetLength();

    pBuf->SetLength(0);
    pBuf->Append32(length);
    pBuf->SetLength(nPos);

    pBuf->FileWrite(f);
}
