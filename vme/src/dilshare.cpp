/*
 $Author: All $
 $RCSfile: dilshare.cpp,v $
 $Date: 2003/01/03 04:08:47 $
 $Revision: 1.2 $
 */

#include "structs.h"
#include "namelist.h"
#include "intlist.h"
#include "dil.h"
#include "dilrun.h"
#include "utils.h"

int g_nDilPrg = 0;
int g_nDilVal = 0;

dilval::dilval(void)
{
    g_nDilVal++;

    type = DILV_ERR;
    val.ptr = NULL;
    ref = NULL;
    atyp = DILA_NONE;
}

/* free generated temporary values */
dilval::~dilval (void)
{
    g_nDilVal--;

    switch (type)
    {
    case DILV_SP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            	slog (LOG_ALL, 0, "DIL: NULL string pointer to FREE().");
            	*/
        }
        else if (atyp == DILA_EXP)
        {
            FREE (val.ptr);
            val.ptr = NULL;
        }
        break;

    case DILV_SLP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            	slog (LOG_ALL, 0, "DIL: NULL string list pointer to FREE().");
            	*/
        }
        else if (atyp == DILA_EXP)
        {
            delete ((class cNamelist *) val.ptr);
            val.ptr = NULL;
        }
        break;

    case DILV_ILP:
        /* Only free if temporary allocated expression */
        if (val.ptr == NULL)
        {
            /*
            				slog (LOG_ALL, 0, "DIL: NULL intlist pointer to FREE().");
            				*/
        }
        else if (atyp == DILA_EXP)
        {
            delete ((class cintlist *) val.ptr);
            val.ptr = NULL;
        }
        break;

    default:
        if (val.ptr && (atyp == DILA_EXP))
        {
            slog(LOG_ALL,0,"value not freed of type %d", atyp);
        }
    }
}


void dilprg::link(diltemplate *tmpl)
{
    assert(this->next == NULL);
    
    this->next = tmpl->prg_list;
    tmpl->prg_list = this;
}

void dilprg::unlink(void)
{
    struct diltemplate *tmpl;

    tmpl = this->frame[0].tmpl;
    assert(tmpl);

    if (this == tmpl->prg_list) // Are we inserted at the head?
    {
        if (tmpl->nextdude == this)
            tmpl->nextdude = this->next;
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
                    tmpl->nextdude = this->next;
                tp->next = this->next;
                ok = TRUE;
                break;
            }
        }

        if (ok == FALSE)
        {
            slog(LOG_ALL, 0, "Not found in dil_list [%s]. Zone [%s]. Owner %s", 
                tmpl->prgname, tmpl->zone ? tmpl->zone->name : "NOZONE", UNIT_FI_NAME(this->owner));
        }
    }

    this->next = NULL;
}

dilprg::dilprg(class unit_data *owner, diltemplate *linktmpl)
{
    g_nDilPrg++;

    this->next = NULL; 

#ifdef DMSERVER
    if (linktmpl)
        this->link(linktmpl);
#endif

    this->flags = 0;       // Recall, copy, etc.
    this->varcrc = 0;		// variable crc from compiler (saved)
    this->corecrc = 0;		// core crc from compiler (saved)
    this->nest = 0;        // How many levels is the call nested 
    // ??? this->stack.init(10);

    this->owner = owner;
    this->sarg = NULL;
    this->waitcmd = WAITCMD_MAXINST - 1;

    // Setup the base frame
    CREATE(this->frame, struct dilframe, 1);
    this->fp = this->frame;
    this->framesz = 1;

    this->frame->tmpl = NULL;
    this->frame->vars = NULL;
    this->frame->intrcount = 0;
    this->frame->intr = NULL;
    this->frame->securecount = 0;
    this->frame->secure = NULL;
    this->frame->pc = NULL;
    this->frame->stacklen = this->stack.length();
}

int dilprg::canfree(void)
{
    if (this->nest <= 0)
        return TRUE;
    else
        return FALSE;
}

dilprg::~dilprg(void)
{
    assert(canfree());

    g_nDilPrg--;

#ifdef DMSERVER
    
    struct diltemplate *tmpl;
    struct dilframe *frm;

    this->unlink();

    tmpl = this->frame[0].tmpl;
    assert(tmpl);

    for (frm = this->frame; frm <= (this->fp); frm++)
        dil_free_frame(frm);

    FREE(this->frame);

    void dil_free_template(struct diltemplate * tmpl, int copy);
    dil_free_template(tmpl, IS_SET(this->flags, DILFL_COPY));

    dilval *v;
    
    while (this->stack.length() > 0)
    {
        v = this->stack.pop();        
        delete v;
    }

#endif

    this->flags = 0;
    this->varcrc = 0;
    this->corecrc = 0;
    this->next = 0;
    this->framesz = 0;
    this->owner = NULL;
    this->sarg = NULL;
    this->fp = NULL;
    this->waitcmd = WAITCMD_DESTROYED;
}
