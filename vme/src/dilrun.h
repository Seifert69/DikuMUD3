/*
 $Author: All $
 $RCSfile: dilrun.h,v $
 $Date: 2004/03/20 06:13:21 $
 $Revision: 2.3 $
 */
#ifndef _MUD_DILRUN_H
#define _MUD_DILRUN_H

struct dil_func_type
{
    void (*func) (class dilprg *);
};

extern struct dil_func_type dilfe_func[];

/* Maximum number of consecutive instructions allowed in one tick */
#define WAITCMD_FINISH	  -100	/* Stop program execution until next event */
#define WAITCMD_STOP      -150	/* Suspend execution                       */
#define WAITCMD_QUIT	  -200	/* Quit the DIL program entirely           */
#define WAITCMD_DESTROYED -500	/* The DIL program was destroyed           */

/* MS: The ONLY reason for having TWO almost identical evals, is that I
   want to initialize the result variables as error by default! */

#define eval_dil_exp(prg, v) \
   { (prg)->fp->pc++; \
   (v)->type = DILV_ERR; \
   (dilfe_func[*(prg->fp->pc-1)].func ((prg), (v))); \
   assert((prg)->fp->pc <= \
     &((prg)->fp->tmpl->core[(prg)->fp->tmpl->coresz])); }

void dil_edit_done (class descriptor_data *d);
void DeactivateDil (class unit_data * pc, class dilprg *prg = NULL);
void ActivateDil (class unit_data * pc);
void dil_loadtime_activate (class unit_data * u);

int dil_direct_init (struct spec_arg *sarg);
int dil_init (struct spec_arg *sarg);

void dil_activate (class dilprg *prg);
void dil_activate_cmd (class dilprg *prg, struct command_info *cmd_ptr);
void dil_init_vars (int varc, struct dilframe *frm);
void dil_free_var (struct dilvar *var);

void dil_free_frame (struct dilframe *frame);

char dil_getbool (class dilval *v, class dilprg *prg);
int dil_getval (class dilval *v);
void dil_add_secure (class dilprg *prg, class unit_data * sup, ubit8 * lab);
#ifdef __cplusplus
int dil_sub_secure (struct dilframe *frm, class unit_data * sup,
                    int bForeach = FALSE);
#endif
void dil_secure_test (register class dilprg *prg);
int same_environment (class unit_data * u1, class unit_data * u2);
void unhash_str (char **s);
void dil_clear_extras(register class dilprg *prg, class extra_descr_data *exd);
void dil_clear_non_secured (register class dilprg *prg);
void dil_clear_lost_reference (register struct dilframe *frm, void *ptr);
void dil_test_secure (register class dilprg *prg, int bForeach = FALSE);
int dil_destroy (const char *name, class unit_data * u);

/* NULL fptr creates one...  */
class dilprg *dil_copy_template (struct diltemplate *tmpl,
                                  class unit_data * u,
                                  class unit_fptr **pfptr);
class dilprg *dil_copy (char *name, class unit_data * u);


class unit_fptr *dil_find (const char *name, class unit_data * u);

void dil_typeerr (class dilprg *p, const  char *where);

#define FAIL_NULL   1
#define TYPEFAIL_NULL 2

int dil_type_check (const char *f, class dilprg *p, int tot, ...);
void dil_free_prg (class dilprg *prg);

void dil_intr_remove (class dilprg *p, int idx);
int dil_intr_insert (class dilprg *p, ubit8 * lab, ubit8 * elab, ubit16 flags);



#endif
