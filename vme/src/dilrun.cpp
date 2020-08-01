/*
 $Author: All $
 $RCSfile: dilrun.cpp,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.14 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h> /* For type_check */

#include "structs.h"
#include "handler.h"
#include "textutil.h"
#include "interpreter.h"
#include "comm.h"
#include "dil.h"
#include "affect.h"
#include "utility.h"
#include "utils.h"
#include "db.h"
#include "db_file.h"
#include "vmelimits.h"
#include "common.h"
#include "spells.h"
#include "db_file.h"
#include "dilinst.h"
#include "dilexp.h"
#include "dilrun.h"
#include "intlist.h"
#include "essential.h"

extern struct unit_function_array_type unit_function_array[];
extern int mudboot;
extern eventqueue events;
extern void special_event(void *p1, void *p2);
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

class dilprg *dil_list = NULL;
class dilprg *dil_list_nextdude = NULL;

void dil_edit_done(class descriptor_data *d)
{
    send_edit(d->editing, d->localstr);
}

void dil_intr_remove(class dilprg *p, int idx)
{
    if ((idx >= 0) && (idx < p->fp->intrcount))
    {
        memmove(&(p->fp->intr[idx]), &(p->fp->intr[idx + 1]),
                sizeof(struct dilintr) * (p->fp->intrcount - idx - 1));
        p->fp->intr[p->fp->intrcount - 1].flags = 0;
        p->fp->intr[p->fp->intrcount - 1].lab = NULL;
        p->fp->intr[p->fp->intrcount - 1].elab = NULL;
    }
}

int dil_intr_insert(class dilprg *p, ubit8 *lab, ubit8 *elab, ubit16 flags)
{
    int intnum;

    /* find intnum */
    for (intnum = 0; intnum < p->fp->intrcount; intnum++)
        if (p->fp->intr[intnum].lab == lab)
        {
            dil_intr_remove(p, intnum);
            break;
        }

    /* find intnum */
    for (intnum = 0; intnum < p->fp->intrcount; intnum++)
        if (!p->fp->intr[intnum].flags)
            break;

    assert(intnum < p->fp->intrcount);

    p->fp->intr[intnum].flags = flags;
    p->fp->intr[intnum].lab = lab;
    p->fp->intr[intnum].elab = elab;

    return intnum;
}
void dil_free_var(struct dilvar *v)
{
    switch (v->type)
    {
    case DILV_SP:
        if (v->val.string)
        {
            FREE(v->val.string);
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

void dil_free_frame(struct dilframe *frame)
{
    int j;

    /* free variables */
    for (j = 0; j < frame->tmpl->varc; j++)
        dil_free_var(&frame->vars[j]);

    if (frame->vars)
    {
        FREE(frame->vars);
        frame->vars = NULL;
    }

    /* discard secures */
    if (frame->secure)
    {
        FREE(frame->secure);
        frame->secure = NULL;
    }

    /* discard intr */
    if (frame->intr)
    {
        FREE(frame->intr);
        frame->intr = NULL;
    }
}

void dil_free_template(struct diltemplate *tmpl, int copy)
{
    int i;

    if (tmpl->flags == DILFL_FREEME)
    {
        /* free dummy template */
        //FREE(tmpl->prgname); xxx
        tmpl->zone = NULL;
        tmpl->argt = NULL;
        tmpl->xrefs = NULL;
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

        tmpl->zone = NULL;
        tmpl->argt = NULL;
        tmpl->xrefs = NULL;

        FREE(tmpl);
    }
}

/* returns boolean value of DIL value */
char dil_getbool(class dilval *v, class dilprg *prg)
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
    case DILV_UP:
    case DILV_ZP:
    case DILV_CP:
    case DILV_SP:
    case DILV_EDP:
        return (v->val.ptr != NULL); /* return Rvalue */

    case DILV_UPR:
    case DILV_ZPR:
    case DILV_CPR:
    case DILV_SPR:
    case DILV_EDPR:
        return (*((void **)v->ref) != NULL); /* return Lvalue */

    case DILV_SLP:
        return ((cNamelist *)v->val.ptr)->Length() != 0;

    case DILV_SLPR:
        return ((cNamelist *)v->ref)->Length() != 0;

    case DILV_ILP:
        return ((cintlist *)v->val.ptr)->Length() != 0;

    case DILV_ILPR:
        return ((cintlist *)v->ref)->Length() != 0;

    case DILV_HASHSTR:
        /* return Lvalue */
        return ((string *)(v->ref))->empty();

    case DILV_INT:
        return (v->val.num != 0); /* return Rvalue */

    case DILV_SINT1R:
        return (*((sbit8 *)v->ref) != 0); /* return Lvalue */

    case DILV_SINT2R:
        return (*((sbit16 *)v->ref) != 0); /* return Lvalue */

    case DILV_SINT4R:
        return (*((sbit32 *)v->ref) != 0); /* return Lvalue */

    case DILV_UINT1R:
        return (*((ubit8 *)v->ref) != 0); /* return Lvalue */

    case DILV_UINT2R:
        return (*((ubit16 *)v->ref) != 0); /* return Lvalue */

    case DILV_UINT4R:
        return (*((ubit32 *)v->ref) != 0); /* return Lvalue */

    case DILV_FAIL:
    case DILV_NULL:
        return FALSE;

    default:
        slog(LOG_ALL, 0, "DIL getbool error in dil %s on unit %s@%s.",
             prg->frame->tmpl->prgname, UNIT_FI_NAME(prg->sarg->owner),
             UNIT_FI_ZONENAME(prg->sarg->owner));
        return FALSE;
    }
}

/* transform references to values, and returns original type */
int dil_getval(class dilval *v)
{
    /* original type */
    static int orig_type[DILV_MAX + 1] = {
        DILV_ERR,
        DILV_UP, DILV_SP, DILV_SLP, DILV_EDP, DILV_INT,
        DILV_UP, DILV_SP, DILV_SLP, DILV_EDP, DILV_INT,
        DILV_INT, DILV_INT, DILV_INT, DILV_INT, DILV_INT,
        DILV_ERR, DILV_NULL, DILV_FAIL,
        DILV_ERR, DILV_ERR, DILV_ERR, DILV_SP,
        DILV_ZP, DILV_ZP, DILV_ERR, DILV_ERR, DILV_ERR, DILV_ERR,
        DILV_CP, DILV_CP, DILV_ERR, DILV_ERR, DILV_ERR, DILV_ILP, DILV_ILP,
        DILV_ERR};

    switch (v->type)
    {
    case DILV_UP:
    case DILV_SP:
    case DILV_SLP:
    case DILV_ILP:
    case DILV_EDP:
    case DILV_INT:
    case DILV_ZP:
    case DILV_CP:
        v->ref = NULL; /* this is NOT a reference */
        break;
    case DILV_SPR:
        v->val.ptr = *((char **)v->ref); /* get value of ref */
        break;
    case DILV_HASHSTR:
        /* Important! Remember that the HASHSTR may NEVER EVER BE CHANGED! */
        v->val.ptr = (char *)((string *)v->ref)->c_str();
        break;

    case DILV_SLPR:
        v->val.ptr = v->ref;
        break;

    case DILV_ILPR:
        v->val.ptr = v->ref;
        break;

    case DILV_ZPR:
    case DILV_CPR:
    case DILV_UPR:
    case DILV_EDPR:
        v->val.ptr = *((void **)v->ref); /* get value of ref */
        break;

    case DILV_SINT1R:
        v->val.num = *((sbit8 *)v->ref);
        break;
    case DILV_SINT2R:
        v->val.num = *((sbit16 *)v->ref);
        break;
    case DILV_SINT4R:
        v->val.num = *((sbit32 *)v->ref);
        break;
    case DILV_UINT1R:
        v->val.num = *((ubit8 *)v->ref);
        break;
    case DILV_UINT2R:
        v->val.num = *((ubit16 *)v->ref);
        break;
    case DILV_UINT4R:
        v->val.num = *((ubit32 *)v->ref);
        break;
    case DILV_NULL:
    case DILV_FAIL:
        v->val.ptr = NULL;
        v->ref = NULL;
        break;

    default:
        v->type = DILV_ERR; /* illegal type! */
        v->val.num = 0;
        v->ref = NULL;
        break;
    }
    return orig_type[v->type];
}

/* adds exp node to exp, returns node number */
void dil_add_secure(class dilprg *prg, class unit_data *sup, ubit8 *lab)
{
    if (sup == NULL)
        return;

    if (prg->fp->securecount)
    {
        RECREATE(prg->fp->secure, struct dilsecure, prg->fp->securecount + 1);
    }
    else
    {
        CREATE(prg->fp->secure, struct dilsecure, prg->fp->securecount + 1);
    }
    prg->fp->secure[prg->fp->securecount].sup = sup;
    prg->fp->secure[prg->fp->securecount].lab = lab;

    prg->fp->securecount++;
}

/* adds exp node to exp, returns node number */
int dil_sub_secure(struct dilframe *frm, class unit_data *sup, int bForeach)
{
    int i;
    int count;
    count = 0;

    for (i = 0; i < frm->securecount; i++)
        if (frm->secure[i].sup == sup)
        {
            if (bForeach && frm->secure[i].lab)
                continue;

            if (!bForeach && !frm->secure[i].lab)
                continue;

            frm->secure[i] = frm->secure[--(frm->securecount)];
            count++;
            if (frm->securecount)
            {
                RECREATE(frm->secure, struct dilsecure, frm->securecount);
            }
            else
            {
                FREE(frm->secure);
                frm->secure = NULL;
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

/* Clears all variables that are not secured! Called at every activation */
/* of a DIL program (after secures are tested for!).			 */
void dil_clear_non_secured(register class dilprg *prg)
{
    register int i, j;
    struct dilframe *frm;

    if (!prg->frame)
    {
        slog(LOG_ALL, 0, "tried to clean empty frame");
        return;
    }

    for (frm = prg->frame; frm <= prg->fp; frm++)
    {
        for (i = 0; i < frm->tmpl->varc; i++)
        {
            if (frm->vars[i].type == DILV_EDP)
                frm->vars[i].val.extraptr = NULL;
            else if (frm->vars[i].type == DILV_UP)
            {
                for (j = 0; j < frm->securecount; j++)
                    if (frm->secure[j].sup == frm->vars[i].val.unitptr)
                        break;
                if (j >= frm->securecount)
                    frm->vars[i].val.unitptr = NULL;
            }
        }
    }
}

/* If a secure is violated, this routine will clear all local DIL variables */
/* that refer to this secure.						    */
void dil_clear_lost_reference(register struct dilframe *frm, void *ptr)
{
    int i;

    for (i = 0; i < frm->tmpl->varc; i++)
        if (frm->vars[i].val.unitptr == ptr)
            frm->vars[i].val.unitptr = NULL;
}

void dil_test_secure(register class dilprg *prg, int bForeach)
{
    int i;
    int count;
    struct dilframe *frm;

    if (prg->waitcmd <= WAITCMD_STOP)
        return;

    for (frm = prg->frame; frm <= prg->fp; frm++)
        for (i = 0; i < frm->securecount; i++)
        {
            if (bForeach && frm->secure[i].lab)
                continue;
            if (scan4_ref(prg->sarg->owner, frm->secure[i].sup) == NULL)
            {
                if (frm->secure[i].lab)
                {
                    /* This is REALLY important! Imagine a broken secure in a
                       pause; the execution then continues at the label point,
                       however, to get the expected "wait" and execution behaviour,
                       waitcmd must be less than MAXINST (see dilfi_wit) */

                    prg->waitcmd--;
                    frm->pc = frm->secure[i].lab;
                }

                dil_clear_lost_reference(frm, frm->secure[i].sup);
                count = dil_sub_secure(frm, frm->secure[i].sup, bForeach);
                if (count > 0)
                    i--;
                /* Do not return until all have been tested! */
            }
        }
}

/* report error in passed type to function */
void dil_typeerr(class dilprg *p, const char *where)
{
    /* returned value not of expected type */
    /* This is serous! mess-up in the core.. stop the program */

    szonelog(UNIT_FI_ZONE(p->sarg->owner),
             "DIL SERIOUS: on %s@%s %s@%s, Type error in %s\n",
             UNIT_FI_NAME(p->sarg->owner),
             UNIT_FI_ZONENAME(p->sarg->owner), p->fp->tmpl->prgname,
             p->fp->tmpl->zone->name, where);
    p->waitcmd = WAITCMD_QUIT;
}

/*  Within the program p, and within function f,
 *  check the list of variables of the form { v, flag, n, {type} }
 *  where tot is the amount of variables to be checked,
 *  v is a dilval *, flag is one of the macros above,
 *  n is the number of accepted types and type is an int of the form
 *  DILV_xxx.  Returns 0 for success, -1 for failure.
 */
int dil_type_check(const char *f, class dilprg *p, int tot, ...)
{
    va_list args;
    class dilval *v[20];
    int cnt, val, flag, idx = 0;
    bool any, ok_sofar = TRUE;

    va_start(args, tot);

    while (tot--)
    {
        v[idx] = va_arg(args, class dilval *);

        flag = va_arg(args, int);
        cnt = va_arg(args, int);

        val = dil_getval(v[idx]);

        any = FALSE;
        while (cnt--)
            if (val == va_arg(args, int))
                any = TRUE;

        if (!any)
        {
            /* Don't write type error if dil_getval failed or
             * returned DILV_NULL (provided we fail on nulls)
             */
            if ((val != DILV_FAIL) && ((val != DILV_NULL) ||
                                       (flag != FAIL_NULL)))
            {
                dil_typeerr(p, f);
                ok_sofar = FALSE;
            }
        }

        ++idx;
    }

    va_end(args);

    if (ok_sofar)
        return TRUE;

    /* Type error, clean up  They clean themselves up now... :-/
       while (0 < idx--)
       delete v[idx]; */

    p->waitcmd = WAITCMD_DESTROYED;

    return FALSE;
}

/* ************************************************************************ */
/* Evaluating DIL-expressions/instructions				    */
/* ************************************************************************ */

struct dil_func_type dilfe_func[DILI_MAX + 1] = {
    {dilfe_illegal}, /* 0 */
    {dilfe_plus},
    {dilfe_min},
    {dilfe_mul},
    {dilfe_div},
    {dilfe_mod},
    {dilfe_and},
    {dilfe_or},
    {dilfe_not},
    {dilfe_gt},
    {dilfe_lt}, /* 10 */
    {dilfe_ge},
    {dilfe_le},
    {dilfe_eq},
    {dilfe_pe},
    {dilfe_ne},
    {dilfe_in},
    {dilfe_umin},
    {dilfe_se},
    {dilfe_null},

    {dilfe_fld}, /* 20 */
    {dilfe_atoi},
    {dilfe_itoa},
    {dilfe_rnd},
    {dilfe_fndu},
    {dilfe_fndr}, /* 25 */
    {dilfe_load},
    {dilfe_iss},
    {dilfe_getw},
    {dilfe_isa},
    {dilfe_cmds}, /* 30 */
    {dilfe_fnds},

    {dilfe_acti},
    {dilfe_argm},
    {dilfe_tho},
    {dilfe_tda},
    {dilfe_tmd},
    {dilfe_tye},
    {dilfe_hrt},
    {dilfe_self},

    {dilfe_var}, /* 40 */
    {dilfe_fs},
    {dilfe_fsl},
    {dilfe_int},
    {dilfe_len},

    {dilfi_ass},
    {dilfi_lnk},
    {dilfi_exp},
    {dilfi_cast},
    {dilfi_if},
    {dilfi_set}, /* 50 */
    {dilfi_uset},
    {dilfi_adl},
    {dilfi_sul},
    {dilfi_ade},
    {dilfi_sue},
    {dilfi_dst},
    {dilfi_popstk},
    {dilfi_exec},
    {dilfi_wit},
    {dilfi_act}, /* 60 */
    {dilfi_goto},
    {dilfi_sua},
    {dilfi_ada},
    {dilfi_pri},
    {dilfi_npr},
    {dilfi_snd},
    {dilfi_snt},
    {dilfi_sec},
    {dilfi_use},
    {dilfi_foe}, /* 70 */
    {dilfi_fon},
    {dilfi_eqp},
    {dilfi_ueq},
    {dilfe_weat},
    {dilfe_oppo},
    {dilfi_quit},

    {dilfi_blk},
    {dilfi_pup},

    {dilfe_getws},
    {dilfi_snta}, /* 80 */
    {dilfi_log},
    {dilfe_pne},
    {dilfe_sne},
    {dilfi_rfunc},
    {dilfi_rfunc},
    {dilfi_rtf},
    {dilfi_rtf},
    {dilfe_dld},
    {dilfe_dlf},
    {dilfi_dlc}, /* 90 */
    {dilfe_lor},
    {dilfe_land},
    {dilfi_on},
    {dilfi_rsfunc},
    {dilfi_rsfunc},
    {dilfe_intr},
    {dilfi_cli},
    {dilfi_sbt},
    {dilfi_swt},
    {dilfe_fndru}, /* 100 */
    {dilfe_visi},
    {dilfe_atsp},
    {dilfe_purs},
    {dilfi_chas},
    {dilfi_setf}, /* 105 */
    {dilfe_medi},
    {dilfe_targ},
    {dilfe_powe},
    {dilfe_trmo},
    {dilfi_sntadil}, /* 110 */
    {dilfe_cast2},
    {dilfe_mel},
    {dilfe_eqpm},
    {dilfe_cmst},
    {dilfe_opro}, /* 115 */
    {dilfe_delstr},
    {dilfe_delunit},
    {dilfi_amod},
    {dilfi_sete},
    {dilfi_folo}, /* 120 */
    {dilfi_lcri},
    {dilfe_fits},
    {dilfe_cary},
    {dilfe_fnds2},
    {dilfe_path}, /* 125  */
    {dilfe_mons},
    {dilfe_splx},
    {dilfe_spli},
    {dilfe_rti},
    {dilfe_txf}, /* 130  */
    {dilfe_ast},
    {dilfe_pck},
    {dilfe_act},
    {dilfe_islt},
    {dilfe_clr}, /* 135  */
    {dilfe_clradd},
    {dilfe_split},
    {dilfe_ghead},
    {dilfe_replace},
    {dilfe_meldam}, /*140 */
    {dilfi_rslv},
    {dilfi_rsvlv},
    {dilfi_rsrce},
    {dilfi_pgstr},
    {dilfe_clrdel}, /*145 */
    {dilfe_clrchg},
    {dilfe_svstr},
    {dilfe_ldstr},
    {dilfe_flog},
    {dilfe_resta}, /*150 */
    {dilfi_stora},
    {dilfi_stopf},
    {dilfi_edit},
    {dilfe_zhead},
    {dilfe_udir}, //155
    {dilfe_sdir},
    {dilfi_send_done},
    {dilfi_gamestate},
    {dilfi_setpwd},
    {dilfi_delpc},
    {dilfe_ckpwd},
    {dilfe_left},
    {dilfe_right},
    {dilfe_mid},
    {dilfe_sgt}, //165
    {dilfe_slt},
    {dilfe_sge},
    {dilfe_sle},
    {dilfe_isplayer},
    {dilfe_tolower},
    {dilfe_toupper},
    {dilfe_skitxt},
    {dilfe_wpntxt},
    {dilfe_clone},
    {dilfe_chead},
    {dilfe_sendpre},
    {dilfe_excmst},
    {dilfe_fil},
    {dilfi_inslst},
    {dilfi_remlst},
    {dilfi_ade2},
    {dilfe_getcmd},
    {dilfi_reboot},
    {dilfi_kedit},
    {dilfe_gopp},
    {dilfe_excmstc},
    {dilfe_strcmp},
    {dilfe_strncmp},
    {dilfe_wepinfo},
    {dilfe_nhead},
    {dilfe_rhead},
    {dilfe_ohead},
    {dilfe_phead},
    {dilfe_fndu2},
    {dilfe_gfol},
    {dilfe_sact}, // 195
    {dilfe_gint}
};


static int check_interrupt(class dilprg *prg)
{
    int i;
    struct timeval tbegin, tend;
    double ttime;

    for (i = 0; i < prg->fp->intrcount; i++)
    {
        if (IS_SET(prg->fp->intr[i].flags, prg->sarg->mflags | SFB_ACTIVATE))
        {

            ubit32 adr;
            int oldwaitcmd = prg->waitcmd;
            ubit8 *oldpc = prg->fp->pc;

            prg->fp->pc = prg->fp->intr[i].lab;
            prg->fp->stacklen = prg->stack.length();

            gettimeofday(&tbegin, (struct timezone *) 0);

            (prg)->fp->tmpl->nTriggers++;
            while (prg->waitcmd > 0 && (prg->fp->pc < prg->fp->intr[i].elab))
            {
                (prg)->fp->pc++;
                (prg)->fp->tmpl->nInstructions++;
                assert(prg->fp->pc <=
                       &(prg->fp->tmpl->core[prg->fp->tmpl->coresz]));
                (dilfe_func[*(prg->fp->pc - 1)].func(prg));
            }

            gettimeofday(&tend, (struct timezone *) 0);

            ttime = (tend.tv_sec -  tbegin.tv_sec)  * 1000.0 +
                    (tend.tv_usec - tbegin.tv_usec) / 1000.0;
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
                    prg->fp->intr[i].lab = NULL;
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

void ActivateDil(class unit_data *pc)
{
    class unit_fptr *fptr;
    class dilprg *prg;

    for (fptr = UNIT_FUNC(pc); fptr; fptr = fptr->next)
    {
        if (fptr->is_destructed())
        {
            assert(FALSE);
        }
       if (fptr->index == SFUN_DIL_INTERNAL && fptr->data)
        { 
            prg = (class dilprg *)fptr->data;
            REMOVE_BIT(prg->flags, DILFL_DEACTIVATED);
        }
    }
}

/* added active program exclusion for menu dil, we will
 * deactivate all dil's except the running one on a player
 * during a gamestate change.
*/
void DeactivateDil(class unit_data *pc, class dilprg *aprg)
{
    class unit_fptr *fptr;
    class dilprg *prg;

    for (fptr = UNIT_FUNC(pc); fptr; fptr = fptr->next)
    {
        if (fptr->index == SFUN_DIL_INTERNAL && fptr->data)
        {
            if (aprg && (aprg == fptr->data))
                continue;
            prg = (class dilprg *)fptr->data;
            SET_BIT(prg->flags, DILFL_DEACTIVATED);
        }
    }
}

// MS2020. Added dilprg.nest (int) which counts how many times run_dil has
// been called recursively for the same program. nest++ on each entry into run_dil
// and nest-- before you return (if you call dil_free_prg first be sure to decrease first)
// dil_free_prg only frees when nest <= 0.
//
int run_dil(struct spec_arg *sarg)
{
    register class dilprg *prg = (class dilprg *)sarg->fptr->data;
    char *orgarg = (char *)sarg->arg; /* Because fndu may mess with it!!! */

    int i;
    static int activates = 0;
    struct timeval tbegin, tend;
    double ttime;

    if (prg == NULL)
        return SFR_SHARE;

    membug_verify(prg);

    if (prg->owner == NULL)
    {
        slog(LOG_ALL, 0, "run_dil() owner is null :-(");
        return SFR_SHARE;
    }

    if (prg->fp == NULL)
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

            sarg->fptr->data = NULL;

            /* The very very very dirty static template hack... :-) */
            if (IS_SET(prg->fp->tmpl->flags, DILFL_FREEME))
            {
                if (prg->canfree())
                {
                    DELETE(dilprg, prg);
                    prg = NULL;
                }
            }
        }
        else
        {
            sarg->fptr->data = NULL; 
            if (prg->canfree())
            {
                DELETE(dilprg, prg);
                prg = NULL;
            }
        }

        // CMD_AUTO_EXTRACT is only given to run_dil via destroy_fptr
        assert(sarg->fptr->data == NULL);
        assert(sarg->fptr->is_destructed());
        // destroy_fptr(sarg->owner, sarg->fptr);
        return SFR_BLOCK;
    }

    if (IS_SET(prg->flags, DILFL_EXECUTING | DILFL_DEACTIVATED))
    {
        prg->nest--;
        return SFR_SHARE;
    }

    SET_BIT(prg->flags, DILFL_EXECUTING);
    REMOVE_BIT(prg->flags, DILFL_CMDBLOCK);

    /* For evaluating expressions */
    prg->sarg = sarg;

    if (prg->next == NULL)
    {
        slog(LOG_ALL, 0, "already destroyed DIL");
        prg->nest--;
        return SFR_SHARE;
    }


    /* A MEGA HACK! The DIL activated spells will not be tested for
       secures on first call only, to avoid loosing global pointers */
    if (prg->waitcmd == WAITCMD_MAXINST)
        dil_test_secure(prg);

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
    ubit16 OrgHeartBeat = sarg->fptr->heart_beat;

    activates++;

    gettimeofday(&tbegin, (struct timezone *) 0);

    (prg)->fp->tmpl->nTriggers++;
    while (prg->waitcmd > 0)
    {
        (prg)->fp->pc++;

        (prg)->fp->tmpl->nInstructions++;

        assert(prg->fp->pc <= &(prg->fp->tmpl->core[prg->fp->tmpl->coresz]));

        (dilfe_func[*(prg->fp->pc - 1)].func(prg));
    }
    membug_verify(prg);
    gettimeofday(&tend, (struct timezone *) 0);

    ttime = (tend.tv_sec -  tbegin.tv_sec)  * 1000.0 +
            (tend.tv_usec - tbegin.tv_usec) / 1000.0;
    (prg)->fp->tmpl->fCPU += ttime;

    activates--;

    //	assert (!(prg->waitcmd>WAITCMD_FINISH) && (prg->fp==prg->frame) && (prg->stack.length()!=0)));
    sarg->arg = orgarg;

    if (prg->waitcmd <= WAITCMD_DESTROYED)
    { /* Was it destroyed?? */
        prg->nest--;

        int bBlock = IS_SET(prg->flags, DILFL_CMDBLOCK);
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = NULL;
            sarg->fptr->data = NULL;
            destroy_fptr(sarg->owner, sarg->fptr);
        }

        if (bBlock)
            return SFR_BLOCK;
        else
            return SFR_SHARE;
    }
    else if (prg->waitcmd <= WAITCMD_QUIT)
    {
        prg->nest--;

        int bBlock = IS_SET(prg->flags, DILFL_CMDBLOCK);
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = NULL;
            sarg->fptr->data = NULL;
            destroy_fptr(sarg->owner, sarg->fptr);
        }

        if (bBlock)
            return SFR_BLOCK;
        else
            return SFR_SHARE;
    }
    else if (prg->waitcmd <= WAITCMD_STOP)
    {
        /* Just return and let the EXECUTING bit stay turned on, so all
           execution is blocked */
        prg->nest--;
        return SFR_SHARE;
    }
    else if (prg->waitcmd > WAITCMD_FINISH)
    {
        prg->nest--;
        prg->waitcmd = WAITCMD_DESTROYED;
        szonelog(UNIT_FI_ZONE(sarg->owner), "DIL %s in unit %s@%s had "
                                            "endless loop.",
                 prg->fp->tmpl->prgname,
                 UNIT_FI_NAME(sarg->owner), UNIT_FI_ZONENAME(sarg->owner));
        if (prg->canfree())
        {
            DELETE(dilprg, prg);
            prg = NULL;
            sarg->fptr->data = NULL;
            destroy_fptr(sarg->owner, sarg->fptr);
        }
        return SFR_SHARE;
    }

    for (i = 0; i < prg->fp->intrcount; i++)
        SET_BIT(prg->sarg->fptr->flags, prg->fp->intr[i].flags);

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
        sarg->fptr->heart_beat = MAX(PULSE_SEC * 1, sarg->fptr->heart_beat);

        if (IS_SET(prg->sarg->fptr->flags, SFB_TICK))
        {
            void ResetFptrTimer(class unit_data * u, class unit_fptr * fptr);

            /* Purely for optimization purposes! Enqueue / dequeue are HUGE! */
            if ((OrgHeartBeat != sarg->fptr->heart_beat) &&
                (sarg->cmd->no != CMD_AUTO_TICK))
                ResetFptrTimer(sarg->owner, sarg->fptr);
        }
        prg->waitcmd = WAITCMD_MAXINST;

        REMOVE_BIT(prg->flags, DILFL_EXECUTING);

    }

    membug_verify(prg);

    if (IS_SET(prg->flags, DILFL_CMDBLOCK))
        return SFR_BLOCK;
    else
        return SFR_SHARE;
}

int dil_init(struct spec_arg *sarg)
{
    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        if (sarg->fptr->data)
            dil_copy((char *)sarg->fptr->data, sarg->owner);
        else
        {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "Template '(null)' not found: %s@%s",
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner));
        }
        destroy_fptr(sarg->owner, sarg->fptr);
    }

    return SFR_SHARE;
}

static void dil_free_dilargs(struct dilargstype *dilargs)
{
    if (dilargs->name)
        FREE(dilargs->name);

    for (int i = 0; i < dilargs->no; i++)
        switch (dilargs->dilarg[i].type)
        {
        case DILV_SP:
            if (dilargs->dilarg[i].data.string)
                FREE(dilargs->dilarg[i].data.string);
            break;

        case DILV_SLP:
            if (dilargs->dilarg[i].data.stringlist)
                free_namelist(dilargs->dilarg[i].data.stringlist);
            break;
        case DILV_ILP:
            if (dilargs->dilarg[i].data.intlist)
                FREE(dilargs->dilarg[i].data.intlist);
            break;
        }

    FREE(dilargs);
}

int dil_direct_init(struct spec_arg *sarg)
{
    struct dilargstype *dilargs = (struct dilargstype *)sarg->fptr->data;

    assert(dilargs);

    if (sarg->cmd->no != CMD_AUTO_EXTRACT)
    {
        class dilprg *prg;
        struct diltemplate *tmpl;

        tmpl = find_dil_template(dilargs->name);

        if (tmpl == NULL)
        {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "Template '%s' not found: %s@%s",
                     dilargs->name,
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner));
        }
        else if (tmpl->argc != dilargs->no)
        {
            szonelog(UNIT_FI_ZONE(sarg->owner),
                     "Template '%s' had mismatching argument count %d: %s@%s",
                     dilargs->name,
                     dilargs->no,
                     UNIT_FI_NAME(sarg->owner),
                     UNIT_FI_ZONENAME(sarg->owner));
        }
        else
        {
            int i;
            for (i = 0; i < dilargs->no; i++)
                if (tmpl->argt[i] != dilargs->dilarg[i].type)
                    break;

            if (i < dilargs->no)
            {
                szonelog(UNIT_FI_ZONE(sarg->owner),
                         "Template '%s' had argument "
                         "mismatch for argument %d: %s@%s",
                         dilargs->name, i, UNIT_FI_NAME(sarg->owner),
                         UNIT_FI_ZONENAME(sarg->owner));
            }
            else
            {
                prg = dil_copy_template(tmpl, sarg->owner, NULL);

                if (prg && dilargs->no > 0)
                {
                    for (i = 0; i < dilargs->no; i++)
                    {
                        if (tmpl->argt[i] == DILV_SP)
                        {
                            prg->fp->vars[i].val.string =
                                dilargs->dilarg[i].data.string;
                            dilargs->dilarg[i].data.string = NULL;
                        }
                        else if (tmpl->argt[i] == DILV_SLP)
                        {
                            if (prg->fp->vars[i].val.namelist)
                                delete (cNamelist *)prg->fp->vars[i].val.namelist;
                            prg->fp->vars[i].val.namelist =
                                new cNamelist((const char **)dilargs->dilarg[i].data.stringlist);
                        }
                        else if (tmpl->argt[i] == DILV_ILP)
                        {
                            if (prg->fp->vars[i].val.intlist)
                                delete (cintlist *)prg->fp->vars[i].val.intlist;
                            prg->fp->vars[i].val.intlist = new cintlist();
                            int m;
                            for (m = 1; m <= dilargs->dilarg[i].data.intlist[0];
                                 m++)
                                prg->fp->vars[i].val.intlist->Append((int)dilargs->dilarg[i].data.intlist[m]);
                        }
                        else if (tmpl->argt[i] == DILV_INT)
                        {
                            prg->fp->vars[i].val.integer =
                                dilargs->dilarg[i].data.num;
                        }
                    }
                }
            }
        }


        destroy_fptr(sarg->owner, sarg->fptr);
    }
    else
        dil_free_dilargs(dilargs);

    sarg->fptr->data = NULL;

    return SFR_SHARE;
}

int dil_destroy(const char *name, class unit_data *u)
{
    class unit_fptr *fptr;
    class dilprg *prg;
    struct spec_arg sarg;

    fptr = dil_find(name, u);
    if (fptr)
    {
        assert(fptr->data); /* MUST or ged! */
        prg = ((class dilprg *)fptr->data);
        //  This is to send the dildestroy SFB,
        sarg.owner = prg->owner;
        sarg.activator = NULL;
        sarg.fptr = fptr;
        sarg.cmd = &cmd_auto_tick;
        sarg.arg = "";
        sarg.mflags = SFB_DILDESTROY;
        sarg.medium = NULL;
        sarg.target = NULL;
        sarg.pInt = NULL;

        run_dil(&sarg);
        //  We finished the on_dildestroy part, now lets really destroy it.
        if (!fptr->is_destructed() && fptr && fptr->data)
        {
            prg->waitcmd = WAITCMD_QUIT;
            dil_activate(prg);
        }
        return TRUE;
    }

    return FALSE;
}

void dil_init_vars(int varc, struct dilframe *frm)
{
    for (int i = 0; i < varc; i++)
    {
        switch (frm->vars[i].type)
        {
        case DILV_SLP:
            frm->vars[i].val.namelist = new cNamelist;
            break;

        case DILV_ILP:
            frm->vars[i].val.intlist = new cintlist;
            break;

        case DILV_SP:
            frm->vars[i].val.string = NULL;
            break;

        default:
            frm->vars[i].val.integer = 0;
        }
    }
}

class dilprg *dil_copy_template(struct diltemplate *tmpl,
                                 class unit_data *u, class unit_fptr **pfptr)
{
    class dilprg *prg;
    class unit_fptr *fptr;

    if (IS_SET(tmpl->flags, DILFL_UNIQUE))
    {
        class unit_fptr *f;
        for (f = UNIT_FUNC(u); f; f = f->next)
        {
            if (f->index == SFUN_DIL_INTERNAL)
            {
                class dilprg *uprg;
                if ((uprg = (class dilprg *)f->data))
                    if (uprg->frame[0].tmpl == tmpl)
                        return (NULL);
            }
        }
    }

    prg = new EMPLACE(dilprg) dilprg(u,true);
    membug_verify(prg);

    prg->fp->tmpl = tmpl;
    prg->varcrc = tmpl->varcrc;
    prg->corecrc = tmpl->corecrc;
    prg->flags = DILFL_COPY | REMOVE_BIT(tmpl->flags, DILFL_EXECUTING | DILFL_CMDBLOCK);
    prg->frame->pc = tmpl->core;

    if (tmpl->varc)
    {
        CREATE(prg->frame->vars, struct dilvar, tmpl->varc);
    }

    for (int i = 0; i < tmpl->varc; i++)
        prg->frame->vars[i].type = tmpl->vart[i];

    dil_init_vars(tmpl->varc, prg->frame);

    if (tmpl->intrcount)
    {
        CREATE(prg->frame->intr, struct dilintr, tmpl->intrcount);
    }

    prg->frame->intrcount = tmpl->intrcount;

    /* activate on tick SOON! */
    if (IS_SET(tmpl->flags, DILFL_AWARE))
        fptr = create_fptr(u, SFUN_DIL_INTERNAL, tmpl->priority , 1, SFB_ALL | SFB_AWARE, prg);
    else
        fptr = create_fptr(u, SFUN_DIL_INTERNAL,  tmpl->priority, 1, SFB_ALL, prg);

    if (pfptr)
        *pfptr = fptr;

    membug_verify(fptr->data);
    membug_verify(prg);

    return prg;
}

void dil_activate(class dilprg *prg)
{
    struct spec_arg sarg;
    class unit_fptr *fptr;

    assert(prg);

    for (fptr = UNIT_FUNC(prg->owner); fptr; fptr = fptr->next)
        if (fptr->data == prg)
            break;

    assert(fptr);

#ifdef DEBUG_HISTORY
    void add_func_history(class unit_data * u, ubit16, ubit16);
    add_func_history(prg->owner, SFUN_DIL_INTERNAL, SFB_TICK);
#endif

    sarg.owner = prg->owner;
    sarg.activator = NULL;
    sarg.fptr = fptr;
    sarg.cmd = &cmd_auto_tick;
    sarg.arg = "";
    sarg.mflags = SFB_TICK;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;

    run_dil(&sarg);
}

void dil_activate_cmd(class dilprg *prg, struct command_info *cmd_ptr)
{
    struct spec_arg sarg;
    class unit_fptr *fptr;

    assert(prg);

    for (fptr = UNIT_FUNC(prg->owner); fptr; fptr = fptr->next)
        if (fptr->data == prg)
            break;

    assert(fptr);

#ifdef DEBUG_HISTORY
    void add_func_history(class unit_data * u, ubit16, ubit16);
    add_func_history(prg->owner, SFUN_DIL_INTERNAL, SFB_TICK);
#endif

    sarg.owner = prg->owner;
    sarg.activator = NULL;
    sarg.fptr = fptr;
    sarg.cmd = cmd_ptr;
    sarg.arg = "";
    sarg.mflags = SFB_TICK;
    sarg.medium = NULL;
    sarg.target = NULL;
    sarg.pInt = NULL;

    run_dil(&sarg);
}

void dil_loadtime_activate(class unit_data *u)
{
    class unit_fptr *f, *fnext;
    for (f = UNIT_FUNC(u); f; f = fnext)
    {
        fnext = f->next;
        if (f->index == SFUN_DILCOPY_INTERNAL)
        {
            events.remove(special_event, u, f);
            special_event(u, f);
        }
        else if ((f->index == SFUN_DIL_INTERNAL) && f->data)
        {
            events.remove(special_event, u, f);
            special_event(u, f);
            //          dil_activate((class dilprg *) f->data);
        }
    }
    //      if (x>0)
    //        dil_loadtime_activate(u);
}

class dilprg *dil_copy(char *name, class unit_data *u)
{
    char buf[MAX_STRING_LENGTH];
    class dilprg *prg;
    struct diltemplate *tmpl;
    char *tmplname, *farg, *targ;
    int narg, i;
    char *args[256];

    assert(name);
    assert(strlen(name) < sizeof(buf));

    strcpy(buf, name);

    tmplname = strtok(buf, " (");
    farg = strtok(NULL, ")");
    targ = NULL;
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
                args[narg] = strtok(NULL, ",");

                if (args[narg] == NULL)
                    break;
            }
        }
    }

    tmpl = find_dil_template(tmplname);

    if (!tmpl)
    {
        szonelog(UNIT_FI_ZONE(u),
                 "Template '%s' not found: %s@%s",
                 tmplname, UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
        if (targ)
            FREE(targ)
        return NULL;
    }

    /* check argument count and types */
    if (tmpl->argc != narg)
    {
        szonelog(UNIT_FI_ZONE(u),
                 "Template '%s' had mismatching argument count %d: %s@%s",
                 tmplname, narg, UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
        if (targ)
            FREE(targ)
        return NULL;
    }

    for (i = 0; i < narg; i++)
    {
        if (tmpl->argt[i] == DILV_SP)
        {
            continue;
        }
        else if (tmpl->argt[i] == DILV_INT)
        {
            args[i] = skip_spaces(args[i]);
            strip_trailing_spaces(args[i]);

            if (str_is_number(args[i]))
                continue;
        }

        szonelog(UNIT_FI_ZONE(u),
                 "Template '%s' had mismatching argument mismatch for %d: %s@%s",
                 tmplname, i, UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u));
        if (targ)
            FREE(targ)
        return NULL;
    }

    prg = dil_copy_template(tmpl, u, NULL);

    if (prg && narg > 0)
    {
        for (i = 0; i < narg; i++)
        {
            if (tmpl->argt[i] == DILV_SP)
            {
                prg->fp->vars[i].val.string = str_dup(args[i]);
            }
            else if (tmpl->argt[i] == DILV_INT)
            {
                prg->fp->vars[i].val.integer = atoi(args[i]);
            }
            else
                error(HERE, "DIL COPY Arg parse");
        }
    }
    if (targ)
        FREE(targ);
    return prg;
}

class unit_fptr *dil_find(const char *name, class unit_data *u)
{
    class unit_fptr *fptr;
    struct diltemplate *tmpl;

    if ((tmpl = find_dil_template(name)))
    {
        for (fptr = UNIT_FUNC(u); fptr; fptr = fptr->next)
            if ((!fptr->is_destructed()) && (fptr->index == SFUN_DIL_INTERNAL))
                if ((((class dilprg *)fptr->data)->frame[0].tmpl == tmpl) && (((class dilprg *)fptr->data)->waitcmd != WAITCMD_QUIT))
                    return fptr;
    }
    return NULL;
}
