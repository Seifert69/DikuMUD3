/*
 $Author: tperry $
 $RCSfile: dilinst.h,v $
 $Date: 2002/06/09 23:27:40 $
 $Revision: 2.2 $
 */
#ifndef _MUD_DILINST_H
#define _MUD_DILINST_H

void dil_insterr (class dilprg *p, char *where);

void dilfi_folo (class dilprg *);
void dilfi_lcri (class dilprg *);
void dilfi_sete (class dilprg *);
void dilfi_amod (class dilprg *);
void dilfi_edit (class dilprg *);
void dilfi_stora (class dilprg *);
void dilfi_chas (class dilprg *);
void dilfi_setf (class dilprg *);
void dilfi_stopf (class dilprg *);
void dilfi_ass (class dilprg *);
void dilfi_lnk (class dilprg *);
void dilfi_exp (class dilprg *);
void dilfi_if (class dilprg *);
void dilfi_set (class dilprg *);
void dilfi_uset (class dilprg *);
void dilfi_adl (class dilprg *);
void dilfi_sul (class dilprg *);
void dilfi_ade (class dilprg *);
void dilfi_ade2 (class dilprg *);
void dilfi_sue (class dilprg *);
void dilfi_dst (class dilprg *);
void dilfi_popstk (class dilprg *);
void dilfi_exec (class dilprg *);
void dilfi_wit (class dilprg *);
void dilfi_act (class dilprg *);
void dilfi_goto (class dilprg *);
void dilfi_sua (class dilprg *);
void dilfi_ada (class dilprg *);
void dilfi_pri (class dilprg *);
void dilfi_npr (class dilprg *);
void dilfi_snd (class dilprg *);
void dilfi_snt (class dilprg *);
void dilfi_snta (class dilprg *);
void dilfi_log (class dilprg *);
void dilfi_sec (class dilprg *);
void dilfi_use (class dilprg *);
void dilfi_eqp (class dilprg *);
void dilfi_ueq (class dilprg *);
void dilfi_quit (class dilprg *);
void dilfi_blk (class dilprg *);
void dilfi_pup (class dilprg *);
void dilfi_cast (class dilprg *);
void dilfi_rproc (class dilprg *);
void dilfi_rfunc (class dilprg *);
void dilfi_rts (class dilprg *);
void dilfi_rtf (class dilprg *);
void dilfi_dlc (class dilprg *);
void dilfi_on (class dilprg *);
void dilfi_rsproc (class dilprg *);
void dilfi_rsfunc (class dilprg *);
void dilfi_set_weight(class dilprg *p);
void dilfi_set_weight_base(class dilprg *p);
void dilfi_sbt (class dilprg *);
void dilfi_cli (class dilprg *);
void dilfi_foe (class dilprg *);
void dilfi_fon (class dilprg *);
void dilfi_sntadil (class dilprg *);
void dilfi_rslv (class dilprg *);
void dilfi_rsvlv (class dilprg *);
void dilfi_rsrce (class dilprg *);
void dilfi_pgstr (class dilprg *);
void dilfi_send_done (class dilprg *);
void dilfi_gamestate (class dilprg *);
void dilfi_setpwd (class dilprg *);
void dilfi_delpc (class dilprg *);
void dilfi_inslst (class dilprg *);
void dilfi_remlst (class dilprg *);
void dilfi_reboot (class dilprg *);
void dilfi_kedit (class dilprg *);
void dilfe_sact (class dilprg *);
void dilfe_gint (class dilprg *);
void dilfe_shell (class dilprg *);
void dilfi_dispatch(class dilprg *);
#endif
