/*
 $Author: All $
 $RCSfile: dilshare.cpp,v $
 $Date: 2003/01/03 04:08:47 $
 $Revision: 1.2 $
 */

#include "namelist.h"
#include "intlist.h"
#include "dil.h"
#include "dilrun.h"

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


dilprg::dilprg(class unit_data *owner, int bLink)
{
    g_nDilPrg++;

#ifdef DMSERVER
    this->next = NULL; 
    if (bLink)
    {
        this->next = dil_list;
        dil_list = this;
    }
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

    if (dil_list && this->next)
    {
        if (this == dil_list)
        {
            if (dil_list_nextdude == dil_list)
                dil_list_nextdude = this->next;
            dil_list = this->next;
        }
        else
        {
            int ok = FALSE;
            for (class dilprg *tp = dil_list; tp->next; tp = tp->next)
            {
                if (tp->next == this)
                {
                    if (dil_list_nextdude == tp->next)
                        dil_list_nextdude = this->next;
                    tp->next = this->next;
                    ok = TRUE;
                    break;
                }
            }
            this->next = NULL;
            if (ok == FALSE)
            {
                slog(LOG_ALL, 0, "Not found in dil_list");
                return;
            }
        }
    }

    this->next = NULL;

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
