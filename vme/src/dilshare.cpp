/*
 $Author: All $
 $RCSfile: dilshare.cpp,v $
 $Date: 2003/01/03 04:08:47 $
 $Revision: 1.2 $
 */

#include "dil.h"
#include "dilrun.h"
#include "intlist.h"
#include "namelist.h"
#include "slog.h"
#include "structs.h"
#include "utils.h"
#include "zone_type.h"

int g_nDilPrg = 0;
int g_nDilVal = 0;

dilval::dilval()
{
    g_nDilVal++;

    type = DILV_ERR;
    val.ptr = nullptr;
    ref = nullptr;
    atyp = DILA_NONE;
}

/* free generated temporary values */
dilval::~dilval()
{
    g_nDilVal--;

    switch (type)
    {
        case DILV_SP:
            /* Only free if temporary allocated expression */
            if (val.ptr == nullptr)
            {
                /*
                    slog (LOG_ALL, 0, "DIL: NULL string pointer to FREE().");
                    */
            }
            else if (atyp == DILA_EXP)
            {
                FREE(val.ptr);
                val.ptr = nullptr;
            }
            break;

        case DILV_SLP:
            /* Only free if temporary allocated expression */
            if (val.ptr == nullptr)
            {
                /*
                    slog (LOG_ALL, 0, "DIL: NULL string list pointer to FREE().");
                    */
            }
            else if (atyp == DILA_EXP)
            {
                delete ((cNamelist *)val.ptr);
                val.ptr = nullptr;
            }
            break;

        case DILV_ILP:
            /* Only free if temporary allocated expression */
            if (val.ptr == nullptr)
            {
                /*
                                            slog (LOG_ALL, 0, "DIL: NULL intlist pointer to FREE().");
                                            */
            }
            else if (atyp == DILA_EXP)
            {
                delete ((cintlist *)val.ptr);
                val.ptr = nullptr;
            }
            break;

        default:
            if (val.ptr && (atyp == DILA_EXP))
            {
                slog(LOG_ALL, 0, "value not freed of type %d", atyp);
            }
    }
}

void dilprg::link(diltemplate *tmpl)
{
    assert(this->next == nullptr);

    this->next = tmpl->prg_list;
    tmpl->prg_list = this;
}

void dilprg::unlink()
{
    diltemplate *tmpl = nullptr;

    tmpl = this->frame[0].tmpl;
    assert(tmpl);

    if (this == tmpl->prg_list) // Are we inserted at the head?
    {
        if (tmpl->nextdude == this)
        {
            tmpl->nextdude = this->next;
        }
        tmpl->prg_list = this->next;
    }
    else
    {
        int ok = FALSE;
        for (dilprg *tp = tmpl->prg_list; tp; tp = tp->next)
        {
            if (tp->next == this)
            {
                // I guess there's a wierdo scenario where two calls to sendtoalldil()
                // by two different DILs, which both remove an item that affects netdude
                // would cause the DIL traversing list A to suddenly traverse list B
                // (a different template). But I guess no real harm done if we remember
                // to check in DIL. Potentially we could do a glocal nextdude per template
                // that would at least be a little less shaky and less likely.
                //
                if (tmpl->nextdude == this)
                {
                    tmpl->nextdude = this->next;
                }
                tp->next = this->next;
                ok = TRUE;
                break;
            }
        }

        if (ok == FALSE)
        {
            slog(LOG_ALL,
                 0,
                 "Not found in dil_list [%s]. Zone [%s]. Owner %s",
                 tmpl->prgname,
                 tmpl->zone ? tmpl->zone->getName() : "NOZONE",
                 UNIT_FI_NAME(this->owner));
        }
    }

    this->next = nullptr;
}

dilprg::dilprg(unit_data *owner, diltemplate *linktmpl)
{
    g_nDilPrg++;

    this->next = nullptr;

#ifdef DMSERVER
    if (linktmpl)
    {
        this->link(linktmpl);
    }
#endif

    this->flags = 0;   // Recall, copy, etc.
    this->varcrc = 0;  // variable crc from compiler (saved)
    this->corecrc = 0; // core crc from compiler (saved)
    this->nest = 0;    // How many levels is the call nested
    // ??? this->stack.init(10);

    this->owner = owner;
    this->sarg = nullptr;
    this->waitcmd = WAITCMD_MAXINST - 1;

    // Setup the base frame
    CREATE(this->frame, dilframe, 1);
    this->fp = this->frame;
    this->framesz = 1;

    this->frame->tmpl = nullptr;
    this->frame->vars = nullptr;
    this->frame->intrcount = 0;
    this->frame->intr = nullptr;
    this->frame->securecount = 0;
    this->frame->secure = nullptr;
    this->frame->pc = nullptr;
    this->frame->stacklen = this->stack.length();
}

int dilprg::canfree()
{
    if (this->nest <= 0)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

dilprg::~dilprg()
{
    assert(canfree());

    g_nDilPrg--;

#ifdef DMSERVER

    diltemplate *tmpl = nullptr;
    dilframe *frm = nullptr;

    this->unlink();

    tmpl = this->frame[0].tmpl;
    assert(tmpl);

    for (frm = this->frame; frm <= (this->fp); frm++)
    {
        dil_free_frame(frm);
    }

    FREE(this->frame);

    dil_free_template(tmpl, IS_SET(this->flags, DILFL_COPY));

    dilval *v = nullptr;

    while (this->stack.length() > 0)
    {
        v = this->stack.pop();
        delete v;
    }

#endif

    this->flags = 0;
    this->varcrc = 0;
    this->corecrc = 0;
    this->next = nullptr;
    this->framesz = 0;
    this->owner = nullptr;
    this->sarg = nullptr;
    this->fp = nullptr;
    this->waitcmd = WAITCMD_DESTROYED;
}
