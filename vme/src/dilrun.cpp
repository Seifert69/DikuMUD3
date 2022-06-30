/*
 $Author: All $
 $RCSfile: dilrun.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.14 $
 */
#include "dilrun.h"

#include "db.h"
#include "dil.h"
#include "dilexp.h"
#include "dilinst.h"
#include "error.h"
#include "essential.h"
#include "handler.h"
#include "interpreter.h"
#include "intlist.h"
#include "main_functions.h"
#include "mobact.h"
#include "szonelog.h"
#include "textutil.h"
#include "unit_fptr.h"
#include "utils.h"

#include <math.h>
#include <sys/time.h>

#include <cstdarg> /* For type_check */
#include <map>

/* *********************************************************************** *
 * Implementation notes:
 *
 *   Internal command structure:
 *	The commands and expressions are all stored in a shared
 *	core. expressions are stored with a leading function id
 *	corresponding to the function array. This method is also
 *	used by instructions, where the return value is not used
 *	(must be NULL!). The core is also used to store static
 *	values of both integers, strings and stringlists. The
 *	values are read with bread_ functions.
 *
 *   The types and their meaning:
 *	unitptr (UP)	   : A pointer to a unit. units not secured,
 *	   are set to NULL after activation.
 *	extraptr (EDP)	   : A pointer to an extra description.
 *	   are set to NULL after activation.
 *	string (SP)	   : A string. When assigned from another
 *	   string, (also static) strdup'ed. (old free'd). Under
 *	   expression
 *	   evaluation strings are only copied if needed, including
 *	   reading from the core (assuming bread_ format is normal
 *	   string format)
 *	integer (INT)	   : A signed 32 bit integer.
 *	NEW IMPLEMENTATION:
 *	stringlist (SLP)   : A stringlist. When assigned from another
 *	   (also static) copied. (old free'd)
 *
 *   Local passed values in expression evaluation:
 *	Values are tagged with the type of allocation used:
 *	   NORM: normal malloc.
 *	   HASH: hashed class.
 *	   EXP:	 temporarily expression eval.
 *	   NONE: not allocated.
 *
 *
 *
 *	########################################################
 *	NOTE! you MUST! mark a field HASH if DIL is supposed to
 *	      use the hash deallocation if field is changed!
 *	########################################################
 *
 *
 *
 *	unitptr reference     : unitptr variable that can be assigned
 *	   new value. Unitptr are only references.
 *	string reference      : string variable that can be assigned
 *	   new value. the 'atyp' variable tells how the string is
 *	   allocated.
 *	stringlist reference  : stringlist that can be assigned a new
 *	   value. the 'atyp' variable tells how the stringlist is
 *	   allocated.
 *	extraptr reference    : extraptr variable that can be
 *	   assigned a new value. Extraptr are only references.
 *	integer reference     : Several types of integer references
 *	   exist to acommodate all the different types of integers.
 *
 *   On activation and combat.
 *	When encountered the boolean expression is passed, and
 *	assumed to have no side-effects. Should expressions have
 *	sideeffects, the commands should have this form:
 *	<cmd> <eadr> <exp> <xadr>
 *	where <eadr> the the end address of the command, so skipping
 *	it is possible without evaluating the expression.
 *
 *   Calls and frameframes.
 *      The dilprg structure contains the data needed for one DIL
 *      process. The process is marked eighter DILP_COPY for
 *      a process that is a direct copy of a previous registered
 *      template, or DILP_INLINE for an inline defined DIL body code.
 *      For backward compatability, the old DIL programs are marked
 *      DIL_INLINE uppon loading, and thus have a local template not
 *      registered in the zone. The structure also includes a frame,
 *      where each frame contains the needed data for the current
 *      proc/func. Among these are variables, secure structures, external
 *      references program counter and variables. When the process is
 *      saved, only some data in the first frameframe is saved.
 *      Only variables are saved, thus starting over the process when
 *      loaded next time.
 *
 * *********************************************************************** */

// class dilprg *dil_list = NULL;
// class dilprg *dil_list_nextdude = NULL;

void dil_edit_done(descriptor_data *d)
{
    send_edit(d->getEditing(), d->getLocalString());
}

void dil_intr_remove(dilprg *p, int idx)
{
    if ((idx >= 0) && (idx < p->fp->intrcount))
    {
        memmove(&(p->fp->intr[idx]), &(p->fp->intr[idx + 1]), sizeof(dilintr) * (p->fp->intrcount - idx - 1));
        p->fp->intr[p->fp->intrcount - 1].flags = 0;
        p->fp->intr[p->fp->intrcount - 1].lab = nullptr;
        p->fp->intr[p->fp->intrcount - 1].elab = nullptr;
    }
}

int dil_intr_insert(dilprg *p, ubit8 *lab, ubit8 *elab, ubit16 flags)
{
    int intnum = 0;

    /* find intnum */
    for (intnum = 0; intnum < p->fp->intrcount; intnum++)
    {
        if (p->fp->intr[intnum].lab == lab)
        {
            dil_intr_remove(p, intnum);
            break;
        }
    }

    /* find intnum */
    for (intnum = 0; intnum < p->fp->intrcount; intnum++)
    {
        if (!p->fp->intr[intnum].flags)
        {
            break;
        }
    }

    assert(intnum < p->fp->intrcount);

    p->fp->intr[intnum].flags = flags;
    p->fp->intr[intnum].lab = lab;
    p->fp->intr[intnum].elab = elab;

    return intnum;
}
void dil_free_var(dilvar *v)
{
    switch (v->type)
    {
        case DilVarType_e::DILV_SP:
            if (v->val.string)
            {
                FREE(v->val.string);
                v->val.string = nullptr;
            }
            break;

        case DilVarType_e::DILV_SLP:
            if (v->val.namelist)
            {
                delete v->val.namelist;
                v->val.namelist = nullptr;
            }
            break;

        case DilVarType_e::DILV_ILP:
            /* Only free if temporary allocated expression */
            if (v->val.intlist)
            {
                delete v->val.intlist;
                v->val.intlist = nullptr;
            }
            break;

        default:
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

void dil_free_template(diltemplate *tmpl, int copy)
{
    int i = 0;

    if (tmpl->flags == DILFL_FREEME)
    {
        /* free dummy template */
        // FREE(tmpl->prgname); xxx
        tmpl->zone = nullptr;
        tmpl->argt = nullptr;
        tmpl->xrefs = nullptr;
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

            if (tmpl->xrefs[i].argt)
                FREE(tmpl->xrefs[i].argt);
        }

        if (tmpl->xrefs)
            FREE(tmpl->xrefs);

        tmpl->zone = nullptr;
        tmpl->argt = nullptr;
        tmpl->xrefs = nullptr;

        FREE(tmpl);
    }
}

/* returns boolean value of DIL value */
char dil_getbool(dilval *v, dilprg *prg)
{
    /*
       Please! no 'smart' stuff here, the older NICE version
       did NOT work correctly on leftvalue pointers, as there
       is a difference between a pointer and the address of
       the pointer variable! The new value type is very
       simplified, and thus more effective.
     */
    switch (v->type)
    {
        case DilVarType_e::DILV_UP:
        case DilVarType_e::DILV_ZP:
        case DilVarType_e::DILV_CP:
        case DilVarType_e::DILV_SP:
        case DilVarType_e::DILV_EDP:
            return (v->val.ptr != nullptr); /* return Rvalue */

        case DilVarType_e::DILV_UPR:
        case DilVarType_e::DILV_ZPR:
        case DilVarType_e::DILV_CPR:
        case DilVarType_e::DILV_SPR:
        case DilVarType_e::DILV_EDPR:
            return (*((void **)v->ref) != nullptr); /* return Lvalue */

        case DilVarType_e::DILV_SLP:
            return ((cNamelist *)v->val.ptr)->Length() != 0;

        case DilVarType_e::DILV_SLPR:
            return ((cNamelist *)v->ref)->Length() != 0;

        case DilVarType_e::DILV_ILP:
            return ((cintlist *)v->val.ptr)->Length() != 0;

        case DilVarType_e::DILV_ILPR:
            return ((cintlist *)v->ref)->Length() != 0;

        case DilVarType_e::DILV_HASHSTR:
            /* return Lvalue */
            return ((std::string *)(v->ref))->empty();

        case DilVarType_e::DILV_INT:
            return (v->val.num != 0); /* return Rvalue */

        case DilVarType_e::DILV_SINT1R:
            return (*((sbit8 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_SINT2R:
            return (*((sbit16 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_SINT4R:
            return (*((sbit32 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_UINT1R:
            return (*((ubit8 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_UINT2R:
            return (*((ubit16 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_UINT4R:
            return (*((ubit32 *)v->ref) != 0); /* return Lvalue */

        case DilVarType_e::DILV_FAIL:
        case DilVarType_e::DILV_NULL:
            return FALSE;

        default:
            slog(LOG_ALL, 0, "DIL getbool error in dil %s on unit %s.", prg->frame->tmpl->prgname, prg->sarg->owner->getFileIndexSymName());
            return FALSE;
    }
}

/* transform references to values, and returns original type */
int dil_getval(dilval *v)
{
    /* original type */
    static int orig_type[DILV_MAX + 1] = {
        DilVarType_e::DILV_ERR, DilVarType_e::DILV_UP,  DilVarType_e::DILV_SP,  DilVarType_e::DILV_SLP, DilVarType_e::DILV_EDP,  DilVarType_e::DILV_INT,  DilVarType_e::DILV_UP,  DilVarType_e::DILV_SP,  DilVarType_e::DILV_SLP, DilVarType_e::DILV_EDP, DilVarType_e::DILV_INT, DilVarType_e::DILV_INT, DilVarType_e::DILV_INT,
        DilVarType_e::DILV_INT, DilVarType_e::DILV_INT, DilVarType_e::DILV_INT, DilVarType_e::DILV_ERR, DilVarType_e::DILV_NULL, DilVarType_e::DILV_FAIL, DilVarType_e::DILV_ERR, DilVarType_e::DILV_ERR, DilVarType_e::DILV_ERR, DilVarType_e::DILV_SP,  DilVarType_e::DILV_ZP,  DilVarType_e::DILV_ZP,  DilVarType_e::DILV_ERR,
        DilVarType_e::DILV_ERR, DilVarType_e::DILV_ERR, DilVarType_e::DILV_ERR, DilVarType_e::DILV_CP,  DilVarType_e::DILV_CP,   DilVarType_e::DILV_ERR,  DilVarType_e::DILV_ERR, DilVarType_e::DILV_ERR, DilVarType_e::DILV_ILP, DilVarType_e::DILV_ILP, DilVarType_e::DILV_ERR};

    switch (v->type)
    {
        case DilVarType_e::DILV_UP:
        case DilVarType_e::DILV_SP:
        case DilVarType_e::DILV_SLP:
        case DilVarType_e::DILV_ILP:
        case DilVarType_e::DILV_EDP:
        case DilVarType_e::DILV_INT:
        case DilVarType_e::DILV_ZP:
        case DilVarType_e::DILV_CP:
            v->ref = nullptr; /* this is NOT a reference */
            break;
        case DilVarType_e::DILV_SPR:
            v->val.ptr = *((char **)v->ref); /* get value of ref */
            break;
        case DilVarType_e::DILV_HASHSTR:
            /* Important! Remember that the HASHSTR may NEVER EVER BE CHANGED! */
            v->val.ptr = (char *)((std::string *)v->ref)->c_str();
            break;

        case DilVarType_e::DILV_SLPR:
            v->val.ptr = v->ref;
            break;

        case DilVarType_e::DILV_ILPR:
            v->val.ptr = v->ref;
            break;

        case DilVarType_e::DILV_ZPR:
        case DilVarType_e::DILV_CPR:
        case DilVarType_e::DILV_UPR:
        case DilVarType_e::DILV_EDPR:
            v->val.ptr = *((void **)v->ref); /* get value of ref */
            break;

        case DilVarType_e::DILV_SINT1R:
            v->val.num = *((sbit8 *)v->ref);
            break;
        case DilVarType_e::DILV_SINT2R:
            v->val.num = *((sbit16 *)v->ref);
            break;
        case DilVarType_e::DILV_SINT4R:
            v->val.num = *((sbit32 *)v->ref);
            break;
        case DilVarType_e::DILV_UINT1R:
            v->val.num = *((ubit8 *)v->ref);
            break;
        case DilVarType_e::DILV_UINT2R:
            v->val.num = *((ubit16 *)v->ref);
            break;
        case DilVarType_e::DILV_UINT4R:
            v->val.num = *((ubit32 *)v->ref);
            break;
        case DilVarType_e::DILV_NULL:
        case DilVarType_e::DILV_FAIL:
            v->val.ptr = nullptr;
            v->ref = nullptr;
            break;

        default:
            v->type = DilVarType_e::DILV_ERR; /* illegal type! */
            v->val.num = 0;
            v->ref = nullptr;
            break;
    }
    return orig_type[v->type];
}

/* adds exp node to exp, returns node number */
void dil_add_secure(dilprg *prg, unit_data *sup, ubit8 *lab)
{
    if (sup == nullptr)
    {
        return;
    }

    if (prg->fp->securecount)
    {
        RECREATE(prg->fp->secure, dilsecure, prg->fp->securecount + 1);
    }
    else
    {
        CREATE(prg->fp->secure, dilsecure, prg->fp->securecount + 1);
    }
    prg->fp->secure[prg->fp->securecount].sup = sup;
    prg->fp->secure[prg->fp->securecount].lab = lab;

    prg->fp->securecount++;
}

/* adds exp node to exp, returns node number */
int dil_sub_secure(dilframe *frm, unit_data *sup, int bForeach)
{
    int i = 0;
    int count = 0;
    count = 0;

    for (i = 0; i < frm->securecount; i++)
    {
        if (frm->secure[i].sup == sup)
        {
            if (bForeach && frm->secure[i].lab)
            {
                continue;
            }

            if (!bForeach && !frm->secure[i].lab)
            {
                continue;
            }

            frm->secure[i] = frm->secure[--(frm->securecount)];
            count++;
            if (frm->securecount)
            {
                RECREATE(frm->secure, dilsecure, frm->securecount);
            }
            else
            {
                FREE(frm->secure);
                frm->secure = nullptr;
            }
        }
    }
    return count;
}

/* *********************************************************************
   This function tests if secures if DIL's are triggered.
   The function must be called just BEFORE a dil program is activated,
   or reactivated after external calls such as addequip(), unequip(),
   exec(), send() and sendto().
   ********************************************************************* */

/* Clears all extra pointers equal to a removed extra            */
void dil_clear_extras(dilprg *prg, extra_descr_data *exd)
{
    int i = 0;
    dilframe *frm = nullptr;

    if (!prg->frame)
    {
        slog(LOG_ALL, 0, "tried to clean empty frame");
        return;
    }

    for (frm = prg->frame; frm <= prg->fp; frm++)
    {
        for (i = 0; i < frm->tmpl->varc; i++)
        {
            if (frm->vars[i].type == DilVarType_e::DILV_EDP)
            {
                if (frm->vars[i].val.extraptr == exd)
                {
                    frm->vars[i].val.extraptr = nullptr;
                }
            }
        }
    }

    // Guess this might be needed when calling remote routines. Not tested... :-/
    for (i = 0; i < prg->stack.length(); i++)
    {
        if (prg->stack[i]->atyp == DilVarType_e::DILV_EDPR)
        {
            if (*((extra_descr_data **)prg->stack[i]->ref) == exd)
            {
                prg->stack[i]->ref = nullptr;
            }
        }
        else if (prg->stack[i]->atyp == DilVarType_e::DILV_EDP)
        {
            if (prg->stack[i]->val.ptr == exd)
            {
                prg->stack[i]->val.ptr = nullptr;
            }
        }
    }
}

/* Clears all variables that are not secured! Called at every activation */
/* of a DIL program (after secures are tested for!).			 */
void dil_clear_non_secured(dilprg *prg)
{
    int i = 0;
    int j = 0;
    dilframe *frm = nullptr;

    if (!prg->frame)
    {
        slog(LOG_ALL, 0, "tried to clean empty frame");
        return;
    }

    for (frm = prg->frame; frm <= prg->fp; frm++)
    {
        for (i = 0; i < frm->tmpl->varc; i++)
        {
            if (frm->vars[i].type == DilVarType_e::DILV_EDP)
            {
                frm->vars[i].val.extraptr = nullptr;
            }
            else if (frm->vars[i].type == DilVarType_e::DILV_UP)
            {
                for (j = 0; j < frm->securecount; j++)
                {
                    if (frm->secure[j].sup == frm->vars[i].val.unitptr)
                    {
                        break;
                    }
                }
                if (j >= frm->securecount)
                {
                    frm->vars[i].val.unitptr = nullptr;
                }
            }
        }
    }
}

/* If a secure is violated, this routine will clear all local DIL variables */
/* that refer to this secure.						    */
void dil_clear_lost_reference(dilframe *frm, void *ptr)
{
    int i = 0;

    for (i = 0; i < frm->tmpl->varc; i++)
    {
        if (frm->vars[i].val.unitptr == ptr)
        {
            frm->vars[i].val.unitptr = nullptr;
        }
    }
}

/*
    MS 2020-10-21

    There was a major issue with dil_test_secure() that I had to fix.
    And there's no perfect fix for the problem.

    Imagine that a dil program has a secured variable then then calls
    another secured dil program that returns a value which also has a
    secured variable.

    When the secure is broken, the first program exits, and returns a value,
    the second (original) program, should then have received the value which
    was placed on the stack, but because the dil_test_secure() moved the program
    pointer for both programs, the second program is no longer at the original
    point and doesn't know there are values to pop from the stack.

    Since the last program can't know if it is used in a stack sensitive context,
    the only option seems to be to not change the program point of any program
    except the currently executing DIL program.

    Thus the code has been modified to change only the active program's execution
    point (e.g. to secure(pc, lostpc); -> :lostpc: // new exec point). It will still
    null out the variable for all programs that secured them.

    It's impossible to know how big an impact this will have, if any. If it is an
    issue, then my next plan would be to mark the new execution point in a variable
    in the frame of the other programs. Once they get to execute, they should jump
    to this point. However, it's also non-trivial. We can't just do that on the first
    instruction when returning from the second program, or we'd run into the same issue
    as far as I can tell. So it would have to be at a "valid point". Since I don't have
    a great idea for what defines a valid point, I'm punting to see if we can live with
    only the currently executing program triggering the secure().

*/

void dil_test_secure(dilprg *prg, int bForeach)
{
    int i = 0;
    int count = 0;
    dilframe *frm = nullptr;

    if (prg->waitcmd <= WAITCMD_STOP)
    {
        return;
    }

    for (frm = prg->frame; frm <= prg->fp; frm++)
    {
        for (i = 0; i < frm->securecount; i++)
        {
            if (bForeach && frm->secure[i].lab)
            {
                continue;
            }
            if (scan4_ref(prg->sarg->owner, frm->secure[i].sup) == nullptr)
            {
                if (frm->secure[i].lab)
                {
                    /* This is REALLY important! Imagine a broken secure in a
                       pause; the execution then continues at the label point,
                       however, to get the expected "wait" and execution behaviour,
                       waitcmd must be less than MAXINST (see dilfi_wit) */

                    prg->waitcmd--;

                    if (frm == prg->fp)
                    { // See long comment above. Only the active frame changes execution point
                        frm->pc = frm->secure[i].lab;
                    }
                }

                dil_clear_lost_reference(frm, frm->secure[i].sup);
                count = dil_sub_secure(frm, frm->secure[i].sup, bForeach);
                if (count > 0)
                {
                    i--;
                }
                /* Do not return until all have been tested! */
            }
        }
    }
}

/* report error in passed type to function */
void dil_typeerr(dilprg *p, const char *where)
{
    /* returned value not of expected type */
    /* This is serous! mess-up in the core.. stop the program */

    szonelog(p->sarg->owner->getFileIndex()->getZone(),
             "DIL SERIOUS: on %s %s@%s, Type error in %s\n",
             p->sarg->owner->getFileIndexSymName(),
             p->fp->tmpl->prgname,
             p->fp->tmpl->zone->getName(),
             where);
    p->waitcmd = WAITCMD_QUIT;
}

/*  Within the program p, and within function f,
 *  check the list of variables of the form { v, flag, n, {type} }
 *  where tot is the amount of variables to be checked,
 *  v is a dilval *, flag is one of the macros above,
 *  n is the number of accepted types and type is an int of the form
 *  DilVarType_e::DILV_xxx.  Returns 0 for success, -1 for failure.
 */
int dil_type_check(const char *f, dilprg *p, int tot, ...)
{
    va_list args;
    dilval *v[20];
    int cnt = 0;
    int val = 0;
    int flag = 0;
    int idx = 0;
    bool any = 0;
    bool ok_sofar = TRUE;

    va_start(args, tot);

    while (tot--)
    {
        v[idx] = va_arg(args, dilval *);

        flag = va_arg(args, int);
        cnt = va_arg(args, int);

        val = dil_getval(v[idx]);

        any = FALSE;
        while (cnt--)
        {
            if (val == va_arg(args, int))
            {
                any = TRUE;
            }
        }

        if (!any)
        {
            /* Don't write type error if dil_getval failed or
             * returned DilVarType_e::DILV_NULL (provided we fail on nulls)
             */
            if ((val != DilVarType_e::DILV_FAIL) && ((val != DilVarType_e::DILV_NULL) || (flag != FAIL_NULL)))
            {
                dil_typeerr(p, f);
                ok_sofar = FALSE;
            }
        }

        ++idx;
    }

    va_end(args);

    if (ok_sofar)
    {
        return TRUE;
    }

    /* Type error, clean up  They clean themselves up now... :-/
       while (0 < idx--)
       delete v[idx]; */

    p->waitcmd = WAITCMD_DESTROYED;

    return FALSE;
}

/**
 * Evaluating DIL-expressions/instructions
 *
 * Papi had this great idea for optimization. The map gets flattened into an array at runtime to reduce lookup
 * times for the functions.
 *
 * The map gets copied into an array at runtime.
 **/
using dil_func_ptr = void (*)(dilprg *);
static std::unique_ptr<dil_func_ptr[]> g_dil_runtime_function_table;
void dil_function_table_setup()
{
    // We use gcc branch hinting here, because this will only ever be null on the first call
    if (__builtin_expect(g_dil_runtime_function_table != nullptr, 1))
    {
        return;
    }
    static std::map<size_t, dil_func_ptr> function_table{
        /* DIL expressions */
        {DIL_ILL, dilfe_illegal}, /* Illegal (for debugging purpose) */
        {DILE_PLUS, dilfe_plus},  /* # + # */
        {DILE_MIN, dilfe_min},    /* # - # */
        {DILE_MUL, dilfe_mul},    /* # * # */
        {DILE_DIV, dilfe_div},    /* # / # */
        {DILE_MOD, dilfe_mod},    /* # % # */
        {DILE_AND, dilfe_and},    /* # and # */
        {DILE_OR, dilfe_or},      /* # or # */
        {DILE_NOT, dilfe_not},    /* not # */
        {DILE_GT, dilfe_gt},      /* # > # */
        {DILE_LT, dilfe_lt},      /* # < # */
        {DILE_GE, dilfe_ge},      /* # >= # */
        {DILE_LE, dilfe_le},      /* # <= # */
        {DILE_EQ, dilfe_eq},      /* # == # */
        {DILE_PE, dilfe_pe},      /* # #= # */
        {DILE_NE, dilfe_ne},      /* # != # */
        {DILE_IN, dilfe_in},      /* # in # */
        {DILE_UMIN, dilfe_umin},  /* - # */
        {DILE_SE, dilfe_se},      /* # ?= # */
        {DILE_NULL, dilfe_null},  /* null */

        /* DIL functions */
        {DILE_FLD, dilfe_fld},   /* get field + ubit8 DILF_? */
        {DILE_ATOI, dilfe_atoi}, /* atoi(#) */
        {DILE_ITOA, dilfe_itoa}, /* itoa(#) */
        {DILE_RND, dilfe_rnd},   /* rnd(#,#) */
        {DILE_FNDU, dilfe_fndu}, /* findunit(#,#,#) */
        {DILE_FNDR, dilfe_fndr}, /* findroom(#,#) */
        {DILE_LOAD, dilfe_load}, /* load(#) */
        {DILE_ISS, dilfe_iss},   /* isset(#,#) */
        {DILE_GETW, dilfe_getw}, /* getword(#) */
        {DILE_ISA, dilfe_isa},   /* isaff(#,#) */
        {DILE_CMDS, dilfe_cmds}, /* command(#) */
        {DILE_FNDS, dilfe_fnds}, /* findsymbolic(#) */

        /* DIL internal variables */
        {DILE_ACTI, dilfe_acti}, /* activator */
        {DILE_ARGM, dilfe_argm}, /* argument */
        {DILE_THO, dilfe_tho},   /* mudhour */
        {DILE_TDA, dilfe_tda},   /* mudday */
        {DILE_TMD, dilfe_tmd},   /* mudmonth */
        {DILE_TYE, dilfe_tye},   /* mudyear */
        {DILE_HRT, dilfe_hrt},   /* heartbeat */
        {DILE_SELF, dilfe_self}, /* self */

        /* DIL static references */
        {DILE_VAR, dilfe_var}, /* variable (ubit16) */
        {DILE_FS, dilfe_fs},   /* fixed string (char[]) */
        {DILE_FSL, dilfe_fsl}, /* fixed stringlist (char[][]) */
        {DILE_INT, dilfe_int}, /* fixed integer (sbit32) */
        {DILE_LEN, dilfe_len}, /* length(#) */

        /* DIL instructions */
        {DILI_ASS, dilfi_ass},       /* # = # */
        {DILI_LNK, dilfi_lnk},       /* link(#,#) */
        {DILI_EXP, dilfi_exp},       /* experience(#) */
        {DILI_CAST, dilfi_cast},     /* cast_spell */
        {DILI_IF, dilfi_if},         /* if */
        {DILI_SET, dilfi_set},       /* set (#,#) */
        {DILI_USET, dilfi_uset},     /* unset (#,#) */
        {DILI_ADL, dilfi_adl},       /* addlist (#,#) */
        {DILI_SUL, dilfi_sul},       /* sublist (#,#) */
        {DILI_ADE, dilfi_ade},       /* addextra (#,#,#) */
        {DILI_SUE, dilfi_sue},       /* subextra (#,#) */
        {DILI_DST, dilfi_dst},       /* destroy (#) */
        {DILI_POPSTK, dilfi_popstk}, /* pop the stack and thow it away*/
        {DILI_EXEC, dilfi_exec},     /* exec (#,#) */
        {DILI_WIT, dilfi_wit},       /* wait (#,#) */
        {DILI_ACT, dilfi_act},       /* act (#,#,#,#,#,#) */
        {DILI_GOTO, dilfi_goto},     /* goto label */
        {DILI_SUA, dilfi_sua},       /* subaff (#,#) */
        {DILI_ADA, dilfi_ada},       /* addaff (#,#,#) */
        {DILI_PRI, dilfi_pri},       /* priority */
        {DILI_NPR, dilfi_npr},       /* nopriority */
        {DILI_SND, dilfi_snd},       /* send (#) */
        {DILI_SNT, dilfi_snt},       /* sendto (#,#) */
        {DILI_SEC, dilfi_sec},       /* secure (#,@) */
        {DILI_USE, dilfi_use},       /* unsecure (#) */
        {DILI_FOE, dilfi_foe},       /* foreach - clear / build list */
        {DILI_FON, dilfi_fon},       /* foreach - get next in environment */
        {DILI_EQP, dilfi_eqp},       /* addequip (#,#) */
        {DILI_UEQ, dilfi_ueq},       /* unequip (#) */
        {DILE_WEAT, dilfe_weat},     /* weather */
        {DILE_OPPO, dilfe_oppo},     /* opponent(#,#) */
        {DILI_QUIT, dilfi_quit},     /* quit */

        {DILI_BLK, dilfi_blk}, /* block */
        {DILI_PUP, dilfi_pup}, /* position_update */

        /* Extensions */
        {DILE_GETWS, dilfe_getws},                /* getwords(#) */
        {DILI_SNTA, dilfi_snta},                  /* sendtoall */
        {DILI_LOG, dilfi_log},                    /* log */
        {DILE_PNEQ, dilfe_pne},                   /* not # #= # */
        {DILE_SNEQ, dilfe_sne},                   /* not # $= # */
        {DILI_RPC, dilfi_rfunc},                  /* remote procedure call */
        {DILI_RFC, dilfi_rfunc},                  /* remote function call */
        {DILI_RTS, dilfi_rtf},                    /* return from subroutine */
        {DILI_RTF, dilfi_rtf},                    /* return from function */
        {DILE_DLD, dilfe_dld},                    /* dil destroy (#,#) */
        {DILE_DLF, dilfe_dlf},                    /* dil find (#,#) */
        {DILI_DLC, dilfi_dlc},                    /* dil copy (#,#) */
        {DILE_LOR, dilfe_lor},                    /* logical or */
        {DILE_LAND, dilfe_land},                  /* logical and */
        {DILI_ON, dilfi_on},                      /* on # @ @ @ @ */
        {DILI_SPC, dilfi_rsfunc},                 /* symbolic procedure call */
        {DILI_SFC, dilfi_rsfunc},                 /* symbolic function call */
        {DILE_INTR, dilfe_intr},                  /* interrupt */
        {DILI_CLI, dilfi_cli},                    /* clear interrupt */
        {DILI_SBT, dilfi_sbt},                    /* setbright (#,#) */
        {DILI_SET_W_BASE, dilfi_set_weight_base}, /* set_weight_base (#,#) */
        {DILE_FNDRU, dilfe_fndru},                /* findunit(#,#,#) */
        {DILE_VISI, dilfe_visi},                  /* visible(#,#)   */
        {DILE_ATSP, dilfe_atsp},                  /* attack_spell(#,#,#,#,#) */
        {DILE_PURS, dilfe_purs},                  /* purse(#,#) */
        {DILI_CHAS, dilfi_chas},                  /* change_speed(#,#) */
        {DILI_SETF, dilfi_setf},                  /* set_fighting(#,#) */
        {DILE_MEDI, dilfe_medi},                  /* medium */
        {DILE_TARG, dilfe_targ},                  /* target */
        {DILE_POWE, dilfe_powe},                  /* power */
        {DILE_TRMO, dilfe_trmo},                  /* transfermoney(#,#,#) */
        {DILI_SNTADIL, dilfi_sntadil},            /* sendtoalldil(#,#,#) */
        {DILE_CAST2, dilfe_cast2},                /* int cast_spell(#,#,#,#,#) */
        {DILE_MEL, dilfe_mel},                    /* int meleeattack(#,#,#) */
        {DILE_EQPM, dilfe_eqpm},                  /* unitptr equipment(#,#) */
        {DILE_CMST, dilfe_cmst},                  /* cmdstr */
        {DILE_OPRO, dilfe_opro},                  /* openroll(#,#) */

        {DILE_DELSTR, dilfe_delstr},   /* delstr(filename) */
        {DILE_DELUNIT, dilfe_delunit}, /* delunit(filename)     */
        {DILI_AMOD, dilfi_amod},       /* acc_modify(#,#)  */
        {DILI_SETE, dilfi_sete},       /* sendtext(#,#)  */

        {DILI_FOLO, dilfi_folo},   /* follow(#,#)       */
        {DILI_LCRI, dilfi_lcri},   /* logcrime(#,#,#)   */
        {DILE_FIT, dilfe_fits},    /* fits(#,#,#)       */
        {DILE_CARY, dilfe_cary},   /* can_carry(#,#)    */
        {DILE_FNDS2, dilfe_fnds2}, /* findsymbolic(#,#) */
        {DILE_PATH, dilfe_path},   /* pathto(#,#)       */
        {DILE_MONS, dilfe_mons},   /* moneystring(#,#)  */
        {DILE_SPLX, dilfe_splx},   /* spellindex(#)     */
        {DILE_SPLI, dilfe_spli},   /* spellinfo(#,#)    */

        {DILE_RTI, dilfe_rti},              /* realtime()        */
        {DILE_TXF, dilfe_txf},              /* textformat(#)     */
        {DILE_AST, dilfe_ast},              /* asctime(#)        */
        {DILE_PCK, dilfe_pck},              /* paycheck(#,#)   */
        {DILE_ACT, dilfe_act},              /* act(...) expression */
        {DILE_ISLT, dilfe_islt},            /* islight(#) */
        {DILE_GETCLR, dilfe_clr},           /* getcolor(#,#) */
        {DILE_ADDCLR, dilfe_clradd},        /* addcolor(#,#,#) */
        {DILE_SPLIT, dilfe_split},          /* split(#,#) */
        {DILE_GHEAD, dilfe_ghead},          /* ghead() */
        {DILE_REPLACE, dilfe_replace},      /* replace(#,#,#) */
        {DILE_MELDAM, dilfe_meldam},        /* int meleedamage(#,#,#) */
        {DILI_RSLV, dilfi_rslv},            /* reset_level(#) */
        {DILI_RSVLV, dilfi_rsvlv},          /* reset_vlevel(#) */
        {DILI_RSRCE, dilfi_rsrce},          /* reset_race(#) */
        {DILI_PGSTR, dilfi_pgstr},          /* reset_race(#) */
        {DILE_DELCLR, dilfe_clrdel},        /* delcolor(#,#) */
        {DILE_CHGCLR, dilfe_clrchg},        /* changecolor(#,#, #) */
        {DILE_SVSTR, dilfe_svstr},          /* int = savestr(filename, string) */
        {DILE_LDSTR, dilfe_ldstr},          /* int = loadstr(filename, string) */
        {DILE_FLOG, dilfe_flog},            /* int = flog(filename, string) */
        {DILE_RESTA, dilfe_resta},          /* restoreall(#,#) */
        {DILI_STORA, dilfi_stora},          /* storeall(#)     */
        {DILI_STOPF, dilfi_stopf},          /* stopfighting(#)     */
        {DILI_EDIT, dilfi_edit},            /* beginedit     */
        {DILE_ZHEAD, dilfe_zhead},          /* */
        {DILE_UDIR, dilfe_udir},            /* */
        {DILE_SDIR, dilfe_sdir},            /* */
        {DILI_SNDDONE, dilfi_send_done},    /* */
        {DILI_GMSTATE, dilfi_gamestate},    /* */
        {DILI_SETPWD, dilfi_setpwd},        /* */
        {DILI_DELPC, dilfi_delpc},          /* */
        {DILE_CKPWD, dilfe_ckpwd},          /* */
        {DILE_LEFT, dilfe_left},            /* */
        {DILE_RIGHT, dilfe_right},          /* */
        {DILE_MID, dilfe_mid},              /* */
        {DILE_SGT, dilfe_sgt},              /* # > # */
        {DILE_SLT, dilfe_slt},              /* # < # */
        {DILE_SGE, dilfe_sge},              /* # >= # */
        {DILE_SLE, dilfe_sle},              /* # <= # */
        {DILE_ISPLAYER, dilfe_isplayer},    /* */
        {DILE_TOLOWER, dilfe_tolower},      /* */
        {DILE_TOUPPER, dilfe_toupper},      /* */
        {DILE_SKITXT, dilfe_skitxt},        /* */
        {DILE_WPNTXT, dilfe_wpntxt},        /* */
        {DILE_CLONE, dilfe_clone},          /* */
        {DILE_CHEAD, dilfe_chead},          /* */
        {DILE_SENDPRE, dilfe_sendpre},      /* */
        {DILE_EXCMST, dilfe_excmst},        /* */
        {DILE_FIL, dilfe_fil},              /* */
        {DILI_INSLST, dilfi_inslst},        /* */
        {DILI_REMLST, dilfi_remlst},        /* */
        {DILI_ADE2, dilfi_ade2},            /* addextra (#,#,#,#) */
        {DILE_GETCMD, dilfe_getcmd},        /* getcmd (#) */
        {DILI_REBOOT, dilfi_reboot},        /* Reboot the server */
        {DILI_KEDIT, dilfi_kedit},          /* Reboot the server */
        {DILE_GOPP, dilfe_gopp},            /* */
        {DILE_EXCMSTC, dilfe_excmstc},      /* */
        {DILE_STRCMP, dilfe_strcmp},        /* */
        {DILE_STRNCMP, dilfe_strncmp},      /* */
        {DILE_WEPINFO, dilfe_wepinfo},      /* */
        {DILE_NHEAD, dilfe_nhead},          /* */
        {DILE_RHEAD, dilfe_rhead},          /* */
        {DILE_OHEAD, dilfe_ohead},          /* */
        {DILE_PHEAD, dilfe_phead},          /* */
        {DILE_FNDU2, dilfe_fndu2},          /* */
        {DILE_GFOL, dilfe_gfol},            /* */
        {DILE_SACT, dilfe_sact},            /* sact (#,#,#,#,#,#) */
        {DILE_GINT, dilfe_gint},            /* getinteger(unit, idx) */
        {DILE_PLAYERID, dilfe_shell},       /* */
        {DILI_SET_W, dilfi_set_weight},     /* set_weight(#,#) */
        {DILI_DISPATCH, dilfi_dispatch},    /* dispatch(message) */
        {DILE_FNDZ, dilfe_fndz},            /* findroom(#,#) */
        {DILE_FNDSIDX, dilfe_fndsidx},      /* */
        {DILE_CALL, dilfe_call},            /* dilcall(#)(#,#,#) */
        {DILE_GETAFFECTS, dilfe_getaffects} /* getaffects(#) */
    };

    // Find the largest function key for array total size
    const auto function_with_largest_key = std::max_element(function_table.begin(), function_table.end(), function_table.value_comp());
    const auto table_size = function_with_largest_key->first + 1;

    g_dil_runtime_function_table = std::make_unique<dil_func_ptr[]>(table_size);
    bool missing_functions = false;
    for (auto i = 0ul; i < table_size; ++i)
    {
        if (auto it = function_table.find(i); it != function_table.end())
        {
            g_dil_runtime_function_table[i] = it->second;
        }
        else
        {
            missing_functions = true;
            slog(LOG_ALL, 0, "Missing function id in DIL function map [%d]", i);
        }
    }

    if (missing_functions)
    {
        std::terminate();
    }

    slog(LOG_ALL, 0, "Initialized dil function table with %d functions", table_size);
}

static int check_interrupt(dilprg *prg)
{
    int i = 0;
    timeval tbegin;
    timeval tend;
    double ttime = NAN;

    for (i = 0; i < prg->fp->intrcount; i++)
    {
        if (IS_SET(prg->fp->intr[i].flags, prg->sarg->mflags | SFB_ACTIVATE))
        {
            ubit32 adr = 0;
            int oldwaitcmd = prg->waitcmd;
            ubit8 *oldpc = prg->fp->pc;

            prg->fp->pc = prg->fp->intr[i].lab;
            prg->fp->stacklen = prg->stack.length();

            gettimeofday(&tbegin, nullptr);

            (prg)->fp->tmpl->nTriggers++;
            while (prg->waitcmd > 0 && (prg->fp->pc < prg->fp->intr[i].elab))
            {
                (prg)->fp->pc++;
                (prg)->fp->tmpl->nInstructions++;
                assert(prg->fp->pc <= &(prg->fp->tmpl->core[prg->fp->tmpl->coresz]));
                g_dil_runtime_function_table[*(prg->fp->pc - 1)](prg);
            }

            gettimeofday(&tend, nullptr);

            ttime = (tend.tv_sec - tbegin.tv_sec) * 1000.0 + (tend.tv_usec - tbegin.tv_usec) / 1000.0;
            (prg)->fp->tmpl->fCPU += ttime;

            assert((prg->fp->stacklen + 1) == prg->stack.length());

            dilval *v1 = prg->stack.pop();

            adr = bread_ubit32(&(prg->fp->pc));

            prg->waitcmd = oldwaitcmd;

            if (dil_getbool(v1, prg))
            {
                if (adr == SKIP)
                {
                    prg->fp->pc = oldpc;
                    delete v1;
                    return 1;
                }

                prg->waitcmd--;
                prg->fp->pc = &(prg->fp->tmpl->core[adr]);

                if (IS_SET(prg->fp->intr[i].flags, SFB_ACTIVATE))
                {
                    prg->fp->intr[i].flags = 0;
                    prg->fp->intr[i].lab = nullptr;
                }
                delete v1;
                return 0;
            }
            else
            {
                prg->fp->pc = oldpc;
            }
            delete v1;
        }
    }
    return 0;
}

/* ************************************************************************ */
/* Running a DIL-program */
/* ************************************************************************ */

void ActivateDil(unit_data *pc)
{
    unit_fptr *fptr = nullptr;
    dilprg *prg = nullptr;

    for (fptr = pc->getFunctionPointer(); fptr; fptr = fptr->getNext())
    {
        if (fptr->is_destructed())
        {
            assert(FALSE);
        }
        if (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL && fptr->getData())
        {
            prg = (dilprg *)fptr->getData();
            REMOVE_BIT(prg->flags, DILFL_DEACTIVATED);
        }
    }
}

/* added active program exclusion for menu dil, we will
 * deactivate all dil's except the running one on a player
 * during a gamestate change.
 */
void DeactivateDil(unit_data *pc, dilprg *aprg)
{
    unit_fptr *fptr = nullptr;
    dilprg *prg = nullptr;

    for (fptr = pc->getFunctionPointer(); fptr; fptr = fptr->getNext())
    {
        if (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL && fptr->getData())
        {
            if (aprg && (aprg == fptr->getData()))
            {
                continue;
            }
            prg = (dilprg *)fptr->getData();
            SET_BIT(prg->flags, DILFL_DEACTIVATED);
        }
    }
}

// MS2020. Added dilprg.nest (int) which counts how many times run_dil has
// been called recursively for the same program. nest++ on each entry into run_dil
// and nest-- before you return (if you call dil_free_prg first be sure to decrease first)
// dil_free_prg only frees when nest <= 0.
//
int run_dil(spec_arg *sarg)
{
    dilprg *prg = (dilprg *)sarg->fptr->getData();
    char *orgarg = (char *)sarg->arg; /* Because fndu may mess with it!!! */

    int i = 0;
    static int activates = 0;
    timeval tbegin;
    timeval tend;
    double ttime = NAN;

    if (prg == nullptr)
    {
        return SFR_SHARE;
    }

    membug_verify_class(prg);

    if (prg->owner == nullptr)
    {
        slog(LOG_ALL, 0, "run_dil() owner is null :-(");
        return SFR_SHARE;
    }

    if (prg->fp == nullptr)
    {
        slog(LOG_ALL, 0, "run_dil() has empty frame?! :-(");
        return SFR_SHARE;
    }

    prg->nest++; // How many recursive calls does this program have?

    if (prg->nest > 2)
    {
        slog(LOG_ALL, 0, "run_dil() %s has nest %d", prg->fp->tmpl->prgname, prg->nest);
    }

    if (sarg->cmd->no == CMD_AUTO_EXTRACT)
    {
        prg->nest--; // We return below, get to zero before free
        prg->waitcmd = WAITCMD_DESTROYED;

        if (IS_SET(prg->flags, DILFL_EXECUTING))
        {
            /* Set so as to notify command loop! Extracted recursively! */

            sarg->fptr->setData(nullptr);

            /* The very very very dirty static template hack... :-) */
            if (IS_SET(prg->fp->tmpl->flags, DILFL_FREEME))
            {
                if (prg->canfree())
                {
                    DELETE(dilprg, prg);
                    prg = nullptr;
                }
            }
        }
        else
        {
            sarg->fptr->setData(nullptr);
            if (prg->canfree())
            {
                DELETE(dilprg, prg);
                prg = nullptr;
            }
        }

        // CMD_AUTO_EXTRACT is only given to run_dil via destroy_fptr
        assert(sarg->fptr->getData() == nullptr);
        assert(sarg->fptr->is_destructed());
        // destroy_fptr(sarg->owner, sarg->fptr);
        return SFR_BLOCK;
    }

    if ((prg->waitcmd > 0) && IS_SET(prg->flags, DILFL_EXECUTING | DILFL_DEACTIVATED))
    {
        prg->nest--;
        return SFR_SHARE;
    }

    SET_BIT(prg->flags, DILFL_EXECUTING);
    REMOVE_BIT(prg->flags, DILFL_CMDBLOCK);

    /* For evaluating expressions */
    prg->sarg = sarg;

    /* This is not correct. If it's the last element in the list it is also NULL.
    if (prg->next == NULL)
    {
        slog(LOG_ALL, 0, "already destroyed DIL");
        prg->nest--;
        return SFR_SHARE;
    }*/

    /* A MEGA HACK! The DIL activated spells will not be tested for
       secures on first call only, to avoid loosing global pointers */
    if (prg->waitcmd == WAITCMD_MAXINST)
    {
        dil_test_secure(prg);
    }

    dil_clear_non_secured(prg);

    /* Test if the ON_ACTIVATION should be triggered */
    if ((prg->waitcmd > 0) && check_interrupt(prg) == 1)
    {
        REMOVE_BIT(prg->flags, DILFL_EXECUTING);
        sarg->arg = orgarg;
        prg->nest--;
        return SFR_SHARE;
    }

    if (activates + 1 > 5000)
    {
        slog(LOG_ALL, 0, "RECURSIVE DIL ERROR IN %s", prg->fp->tmpl->prgname);
        prg->nest--;
        return SFR_SHARE;
    }

    /* For optimization purposes */
    ubit16 OrgHeartBeat = sarg->fptr->getHeartBeat();

    activates++;

    gettimeofday(&tbegin, nullptr);

    (prg)->fp->tmpl->nTriggers++;
    while (prg->waitcmd > 0)
    {
        (prg)->fp->pc++;

        (prg)->fp->tmpl->nInstructions++;

        assert(prg->fp->pc <= &(prg->fp->tmpl->core[prg->fp->tmpl->coresz]));

        g_dil_runtime_function_table[*(prg->fp->pc - 1)](prg);
    }
    membug_verify(prg);
    gettimeofday(&tend, nullptr);

    ttime = (tend.tv_sec - tbegin.tv_sec) * 1000.0 + (tend.tv_usec - tbegin.tv_usec) / 1000.0;
    (prg)->fp->tmpl->fCPU += ttime;

    activates--;

    //	assert (!(prg->waitcmd>WAITCMD_FINISH) && (prg->fp==prg->frame) && (prg->stack.length()!=0)));
    sarg->arg = orgarg;

    if (prg->waitcmd <= WAITCMD_DESTROYED) // Maybe also destroy if fptr or owner is destroyed?
    {                                      /* Was it destroyed?? */
        prg->nest--;

        int bBlock = IS_SET(prg->flags, DILFL_CMDBLOCK);
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = nullptr;
            sarg->fptr->setData(nullptr);
            destroy_fptr(sarg->owner, sarg->fptr);
        }

        if (bBlock)
        {
            return SFR_BLOCK;
        }
        else
        {
            return SFR_SHARE;
        }
    }
    else if (prg->waitcmd <= WAITCMD_QUIT)
    {
        prg->nest--;

        int bBlock = IS_SET(prg->flags, DILFL_CMDBLOCK);
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = nullptr;
            sarg->fptr->setData(nullptr);
            destroy_fptr(sarg->owner, sarg->fptr);
        }

        if (bBlock)
        {
            return SFR_BLOCK;
        }
        else
        {
            return SFR_SHARE;
        }
    }
    else if (prg->waitcmd <= WAITCMD_STOP)
    {
        /* Just return and let the EXECUTING bit stay turned on, so all execution is blocked */
        // Not sure when this might happen, logging
        slog(LOG_ALL, 0, "DIL program [%s] stopped on unit %s.", prg->fp->tmpl->prgname, sarg->owner->getFileIndexSymName());
        prg->nest--;

        //
        // MS2020 It seems we should delete the program to avoid having it hang around broken.
        //
        int bBlock = IS_SET(prg->flags, DILFL_CMDBLOCK);
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = nullptr;
            sarg->fptr->setData(nullptr);
            destroy_fptr(sarg->owner, sarg->fptr);
        }

        if (bBlock)
        {
            return SFR_BLOCK;
        }
        else
        {
            return SFR_SHARE;
        }
    }
    else if (prg->waitcmd > WAITCMD_FINISH)
    {
        szonelog(sarg->owner->getFileIndex()->getZone(),
                 "DIL %s in unit %s had endless loop.",
                 prg->fp->tmpl->prgname,
                 sarg->owner->getFileIndexSymName());

        prg->nest--;
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = nullptr;
            sarg->fptr->setData(nullptr);
            destroy_fptr(sarg->owner, sarg->fptr);
        }
        return SFR_SHARE;
    }

    for (i = 0; i < prg->fp->intrcount; i++)
    {
        prg->sarg->fptr->setActivateOnEventFlag(prg->fp->intr[i].flags);
    }

    /* Okay this is the problem:
       Imagine this:

       heartbeat := 30 * PULSE_SEC;
       wait(SFB_CMD | SFB_TICK, TRUE);
       heartbeat := PULSE_SEC;
       pause;
       goto loop;

       It will only work, if the "SFB_TICK" is what triggers the execution.
       Therefore, I am unfortunately forced to do the follow dequeue and
       enqueue.
     */

    prg->nest--;

    if (prg->nest <= 0)
    {
        sarg->fptr->setHeartBeat(MAX(PULSE_SEC * 1, sarg->fptr->getHeartBeat()));

        if (prg->sarg->fptr->isActivateOnEventFlagSet(SFB_TICK))
        {
            /* Purely for optimization purposes! Enqueue / dequeue are HUGE! */
            if ((OrgHeartBeat != sarg->fptr->getHeartBeat()) && (sarg->cmd->no != CMD_AUTO_TICK))
            {
                ResetFptrTimer(sarg->owner, sarg->fptr);
            }
        }
        prg->waitcmd = WAITCMD_MAXINST;

        REMOVE_BIT(prg->flags, DILFL_EXECUTING);
    }

    membug_verify(prg);

    if (IS_SET(prg->flags, DILFL_CMDBLOCK))
    {
        return SFR_BLOCK;
    }
    else
    {
        return SFR_SHARE;
    }
}

int dil_init(spec_arg *sarg)
{
    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        if (sarg->fptr->getData())
        {
            dil_copy((char *)sarg->fptr->getData(), sarg->owner);
        }
        else
        {
            szonelog(sarg->owner->getFileIndex()->getZone(), "Template '(null)' not found: %s", sarg->owner->getFileIndexSymName());
        }
        destroy_fptr(sarg->owner, sarg->fptr);
    }

    return SFR_SHARE;
}

static void dil_free_dilargs(dilargstype *dilargs)
{
    if (dilargs->name)
        FREE(dilargs->name);

    for (int i = 0; i < dilargs->no; i++)
    {
        switch (dilargs->dilarg[i].type)
        {
            case DilVarType_e::DILV_SP:
                if (dilargs->dilarg[i].data.string)
                    FREE(dilargs->dilarg[i].data.string);
                break;

            case DilVarType_e::DILV_SLP:
                if (dilargs->dilarg[i].data.stringlist)
                {
                    free_namelist(dilargs->dilarg[i].data.stringlist);
                }
                break;
            case DilVarType_e::DILV_ILP:
                if (dilargs->dilarg[i].data.intlist)
                    FREE(dilargs->dilarg[i].data.intlist);
                break;
            default:
                break;
        }
    }

    FREE(dilargs);
}

int dil_direct_init(spec_arg *sarg)
{
    dilargstype *dilargs = (dilargstype *)sarg->fptr->getData();

    assert(dilargs);

    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        dilprg *prg = nullptr;
        diltemplate *tmpl = nullptr;

        tmpl = find_dil_template(dilargs->name);

        if (tmpl == nullptr)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "Template '%s' not found: %s",
                     dilargs->name,
                     sarg->owner->getFileIndexSymName());
        }
        else if (tmpl->argc != dilargs->no)
        {
            szonelog(sarg->owner->getFileIndex()->getZone(),
                     "Template '%s' had mismatching argument count %d: %s",
                     dilargs->name,
                     dilargs->no,
                     sarg->owner->getFileIndexSymName());
        }
        else
        {
            int i = 0;
            for (i = 0; i < dilargs->no; i++)
            {
                if (tmpl->argt[i] != dilargs->dilarg[i].type)
                {
                    break;
                }
            }

            if (i < dilargs->no)
            {
                szonelog(sarg->owner->getFileIndex()->getZone(),
                         "Template '%s' had argument "
                         "mismatch for argument %d: %s",
                         dilargs->name,
                         i,
                         sarg->owner->getFileIndexSymName());
            }
            else
            {
                prg = dil_copy_template(tmpl, sarg->owner, nullptr);

                if (prg && dilargs->no > 0)
                {
                    for (i = 0; i < dilargs->no; i++)
                    {
                        if (tmpl->argt[i] == DilVarType_e::DILV_SP)
                        {
                            prg->fp->vars[i].val.string = dilargs->dilarg[i].data.string;
                            dilargs->dilarg[i].data.string = nullptr;
                        }
                        else if (tmpl->argt[i] == DilVarType_e::DILV_SLP)
                        {
                            if (prg->fp->vars[i].val.namelist)
                            {
                                delete (cNamelist *)prg->fp->vars[i].val.namelist;
                            }
                            prg->fp->vars[i].val.namelist = new cNamelist((const char **)dilargs->dilarg[i].data.stringlist);
                        }
                        else if (tmpl->argt[i] == DilVarType_e::DILV_ILP)
                        {
                            if (prg->fp->vars[i].val.intlist)
                            {
                                delete (cintlist *)prg->fp->vars[i].val.intlist;
                            }
                            prg->fp->vars[i].val.intlist = new cintlist();
                            int m = 0;
                            for (m = 1; m <= dilargs->dilarg[i].data.intlist[0]; m++)
                            {
                                prg->fp->vars[i].val.intlist->Append((int)dilargs->dilarg[i].data.intlist[m]);
                            }
                        }
                        else if (tmpl->argt[i] == DilVarType_e::DILV_INT)
                        {
                            prg->fp->vars[i].val.integer = dilargs->dilarg[i].data.num;
                        }
                    }
                }
            }
        }

        destroy_fptr(sarg->owner, sarg->fptr);
    }
    else
    {
        dil_free_dilargs(dilargs);
    }

    sarg->fptr->setData(nullptr);

    return SFR_SHARE;
}

int dil_destroy(const char *name, unit_data *u)
{
    unit_fptr *fptr = nullptr;
    dilprg *prg = nullptr;
    spec_arg sarg;

    fptr = dil_find(name, u);
    if (fptr && !fptr->is_destructed())
    {
        assert(fptr->getData()); /* MUST or ged! */
        prg = ((dilprg *)fptr->getData());

        //  This is to send the dildestroy SFB to any listening DILs
        sarg.owner = prg->owner;
        sarg.activator = nullptr;
        sarg.fptr = fptr;
        sarg.cmd = &g_cmd_auto_tick;
        sarg.arg = "";
        sarg.mflags = SFB_DILDESTROY;
        sarg.medium = nullptr;
        sarg.target = nullptr;
        sarg.pInt = nullptr;

        run_dil(&sarg);

        //  We finished the signalling, now lets really destroy it.
        if (!fptr->is_destructed() && fptr && fptr->getData())
        {
            REMOVE_BIT(prg->flags, DILFL_DEACTIVATED); // We're going to destroy it
            prg->waitcmd = WAITCMD_QUIT;
            dil_activate(prg);
        }
        return TRUE;
    }

    return FALSE;
}

void dil_init_vars(int varc, dilframe *frm)
{
    for (int i = 0; i < varc; i++)
    {
        switch (frm->vars[i].type)
        {
            case DilVarType_e::DILV_SLP:
                frm->vars[i].val.namelist = new cNamelist;
                break;

            case DilVarType_e::DILV_ILP:
                frm->vars[i].val.intlist = new cintlist;
                break;

            case DilVarType_e::DILV_SP:
                frm->vars[i].val.string = nullptr;
                break;

            default:
                frm->vars[i].val.integer = 0;
        }
    }
}

dilprg *dil_copy_template(diltemplate *tmpl, unit_data *u, unit_fptr **pfptr)
{
    dilprg *prg = nullptr;
    unit_fptr *fptr = nullptr;

    if (IS_SET(tmpl->flags, DILFL_UNIQUE))
    {
        unit_fptr *f = nullptr;
        for (f = u->getFunctionPointer(); f; f = f->getNext())
        {
            if (f->getFunctionPointerIndex() == SFUN_DIL_INTERNAL)
            {
                dilprg *uprg = nullptr;
                if ((uprg = (dilprg *)f->getData()))
                {
                    if (uprg->frame[0].tmpl == tmpl)
                    {
                        return (nullptr);
                    }
                }
            }
        }
    }

    prg = new EMPLACE(dilprg) dilprg(u, tmpl);
    membug_verify(prg);

    prg->fp->tmpl = tmpl;
    prg->varcrc = tmpl->varcrc;
    prg->corecrc = tmpl->corecrc;
    prg->flags = DILFL_COPY | REMOVE_BIT(tmpl->flags, DILFL_EXECUTING | DILFL_CMDBLOCK);
    prg->frame->pc = tmpl->core;

    if (tmpl->varc)
    {
        CREATE(prg->frame->vars, dilvar, tmpl->varc);
    }

    for (int i = 0; i < tmpl->varc; i++)
    {
        prg->frame->vars[i].type = tmpl->vart[i];
    }

    dil_init_vars(tmpl->varc, prg->frame);

    if (tmpl->intrcount)
    {
        CREATE(prg->frame->intr, dilintr, tmpl->intrcount);
    }

    prg->frame->intrcount = tmpl->intrcount;

    /* activate on tick SOON! */
    if (IS_SET(tmpl->flags, DILFL_AWARE))
    {
        fptr = create_fptr(u, SFUN_DIL_INTERNAL, tmpl->priority, PULSE_SEC, SFB_ALL | SFB_AWARE, prg);
    }
    else
    {
        fptr = create_fptr(u, SFUN_DIL_INTERNAL, tmpl->priority, PULSE_SEC, SFB_ALL, prg);
    }

    if (pfptr)
    {
        *pfptr = fptr;
    }

    membug_verify(fptr->data);
    membug_verify(prg);

    return prg;
}

void dil_activate(dilprg *prg)
{
    spec_arg sarg;
    unit_fptr *fptr = nullptr;

    assert(prg);

    for (fptr = prg->owner->getFunctionPointer(); fptr; fptr = fptr->getNext())
    {
        if (fptr->getData() == prg)
        {
            break;
        }
    }

    assert(fptr);

#ifdef DEBUG_HISTORY
    void add_func_history(unit_data * u, ubit16, ubit16);
    add_func_history(prg->owner, SFUN_DIL_INTERNAL, SFB_TICK);
#endif

    sarg.owner = prg->owner;
    sarg.activator = nullptr;
    sarg.fptr = fptr;
    sarg.cmd = &g_cmd_auto_tick;
    sarg.arg = "";
    sarg.mflags = SFB_TICK;
    sarg.medium = nullptr;
    sarg.target = nullptr;
    sarg.pInt = nullptr;

    run_dil(&sarg);
}

void dil_activate_cmd(dilprg *prg, command_info *cmd_ptr)
{
    spec_arg sarg;
    unit_fptr *fptr = nullptr;

    assert(prg);

    for (fptr = prg->owner->getFunctionPointer(); fptr; fptr = fptr->getNext())
    {
        if (fptr->getData() == prg)
        {
            break;
        }
    }

    assert(fptr);

#ifdef DEBUG_HISTORY
    void add_func_history(unit_data * u, ubit16, ubit16);
    add_func_history(prg->owner, SFUN_DIL_INTERNAL, SFB_TICK);
#endif

    sarg.owner = prg->owner;
    sarg.activator = nullptr;
    sarg.fptr = fptr;
    sarg.cmd = cmd_ptr;
    sarg.arg = "";
    sarg.mflags = SFB_TICK;
    sarg.medium = nullptr;
    sarg.target = nullptr;
    sarg.pInt = nullptr;

    run_dil(&sarg);
}

void dil_loadtime_activate(unit_data *u)
{
    unit_fptr *f = nullptr;
    unit_fptr *fnext = nullptr;
    for (f = u->getFunctionPointer(); f; f = fnext)
    {
        fnext = f->getNext();
        if (f->getFunctionPointerIndex() == SFUN_DILCOPY_INTERNAL)
        {
            g_events.remove(special_event, u, f);
            special_event(u, f);
        }
        else if ((f->getFunctionPointerIndex() == SFUN_DIL_INTERNAL) && f->getData())
        {
            g_events.remove(special_event, u, f);
            special_event(u, f);
            //          dil_activate((class dilprg *) f->data);
        }
    }
    //      if (x>0)
    //        dil_loadtime_activate(u);
}

dilprg *dil_copy(char *name, unit_data *u)
{
    char buf[MAX_STRING_LENGTH];
    dilprg *prg = nullptr;
    diltemplate *tmpl = nullptr;
    char *tmplname = nullptr;
    char *farg = nullptr;
    char *targ = nullptr;
    int narg = 0;
    int i = 0;
    char *args[256];

    assert(name);
    assert(strlen(name) < sizeof(buf));

    strcpy(buf, name);

    tmplname = strtok(buf, " (");
    farg = strtok(nullptr, ")");
    targ = nullptr;
    narg = 0;

    if (farg)
    {
        /* To avoid "(,25) giving no arg as 1 - do -1 and set to ' '. */
        farg--;
        *farg = ' ';
        farg = str_dup(farg);
        targ = farg;
        args[0] = strtok(farg, ",");

        if (args[0])
        {
            args[0]++; /* Skip the space we just generated! */

            for (narg = 1; narg < (int)sizeof(args); narg++)
            {
                args[narg] = strtok(nullptr, ",");

                if (args[narg] == nullptr)
                {
                    break;
                }
            }
        }
    }

    tmpl = find_dil_template(tmplname);

    if (!tmpl)
    {
        szonelog(u->getFileIndex()->getZone(), "Template '%s' not found: %s", tmplname, u->getFileIndexSymName());
        if (targ)
        {
            FREE(targ);
        }
        return nullptr;
    }

    /* check argument count and types */
    if (tmpl->argc != narg)
    {
        szonelog(u->getFileIndex()->getZone(),
                 "Template '%s' had mismatching argument count %d: %s",
                 tmplname,
                 narg,
                 u->getFileIndexSymName());
        if (targ)
        {
            FREE(targ);
        }
        return nullptr;
    }

    for (i = 0; i < narg; i++)
    {
        if (tmpl->argt[i] == DilVarType_e::DILV_SP)
        {
            continue;
        }
        else if (tmpl->argt[i] == DilVarType_e::DILV_INT)
        {
            args[i] = (char *)skip_spaces(args[i]);
            strip_trailing_spaces(args[i]);

            if (str_is_number(args[i]))
            {
                continue;
            }
        }

        szonelog(u->getFileIndex()->getZone(),
                 "Template '%s' had mismatching argument mismatch for %d: %s",
                 tmplname,
                 i,
                 u->getFileIndexSymName());
        if (targ)
        {
            FREE(targ);
        }
        return nullptr;
    }

    prg = dil_copy_template(tmpl, u, nullptr);

    if (prg && narg > 0)
    {
        for (i = 0; i < narg; i++)
        {
            if (tmpl->argt[i] == DilVarType_e::DILV_SP)
            {
                prg->fp->vars[i].val.string = str_dup(args[i]);
            }
            else if (tmpl->argt[i] == DilVarType_e::DILV_INT)
            {
                prg->fp->vars[i].val.integer = atoi(args[i]);
            }
            else
            {
                error(HERE, "DIL COPY Arg parse");
            }
        }
    }
    if (targ)
        FREE(targ);
    return prg;
}

unit_fptr *dil_find(const char *name, unit_data *u)
{
    unit_fptr *fptr = nullptr;
    diltemplate *tmpl = nullptr;

    if ((tmpl = find_dil_template(name)))
    {
        for (fptr = u->getFunctionPointer(); fptr; fptr = fptr->getNext())
        {
            if ((!fptr->is_destructed()) && (fptr->getFunctionPointerIndex() == SFUN_DIL_INTERNAL))
            {
                if ((((dilprg *)fptr->getData())->frame[0].tmpl == tmpl) && ((dilprg *)fptr->getData())->waitcmd > WAITCMD_QUIT)
                {
                    return fptr;
                }
            }
        }
    }
    return nullptr;
}
