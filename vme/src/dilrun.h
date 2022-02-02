/*
 $Author: All $
 $RCSfile: dilrun.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#pragma once

#include "dil.h"
#include "essential.h"
#include "structs.h"

#include <cstddef>

struct dil_func_type
{
    void (*func)(dilprg *);
};

/* Maximum number of consecutive instructions allowed in one tick */
#define WAITCMD_FINISH -100    /* Stop program execution until next event */
#define WAITCMD_STOP -150      /* Suspend execution                       */
#define WAITCMD_QUIT -200      /* Quit the DIL program entirely           */
#define WAITCMD_DESTROYED -500 /* The DIL program was destroyed           */

/* MS: The ONLY reason for having TWO almost identical evals, is that I
   want to initialize the result variables as error by default! */

#define eval_dil_exp(prg, v)                                                                                                               \
    {                                                                                                                                      \
        (prg)->fp->pc++;                                                                                                                   \
        (v)->type = DILV_ERR;                                                                                                              \
        (dilfe_func[*(prg->fp->pc - 1)].func((prg), (v)));                                                                                 \
        assert((prg)->fp->pc <= &((prg)->fp->tmpl->core[(prg)->fp->tmpl->coresz]));                                                        \
    }

#define FAIL_NULL 1
#define TYPEFAIL_NULL 2

void dil_edit_done(descriptor_data *d);
void DeactivateDil(unit_data *pc, dilprg *prg = nullptr);
void ActivateDil(unit_data *pc);
void dil_loadtime_activate(unit_data *u);

int dil_direct_init(spec_arg *sarg);
int dil_init(spec_arg *sarg);

void dil_activate(dilprg *prg);
void dil_activate_cmd(dilprg *prg, command_info *cmd_ptr);
void dil_init_vars(int varc, dilframe *frm);
void dil_free_var(dilvar *var);

void dil_free_frame(dilframe *frame);

char dil_getbool(dilval *v, dilprg *prg);
int dil_getval(dilval *v);
void dil_add_secure(dilprg *prg, unit_data *sup, ubit8 *lab);
int dil_sub_secure(dilframe *frm, unit_data *sup, int bForeach = FALSE);
void dil_clear_extras(dilprg *prg, extra_descr_data *exd);
void dil_clear_non_secured(dilprg *prg);
void dil_clear_lost_reference(dilframe *frm, void *ptr);
void dil_test_secure(dilprg *prg, int bForeach = FALSE);
int dil_destroy(const char *name, unit_data *u);

/* NULL fptr creates one...  */
dilprg *dil_copy_template(diltemplate *tmpl, unit_data *u, unit_fptr **pfptr);
dilprg *dil_copy(char *name, unit_data *u);

unit_fptr *dil_find(const char *name, unit_data *u);

void dil_typeerr(dilprg *p, const char *where);

int dil_type_check(const char *f, dilprg *p, int tot, ...);

void dil_intr_remove(dilprg *p, int idx);
int dil_intr_insert(dilprg *p, ubit8 *lab, ubit8 *elab, ubit16 flags);
int run_dil(spec_arg *sarg);
void dil_free_template(diltemplate *tmpl, int copy);

extern dil_func_type g_dilfe_func[];
