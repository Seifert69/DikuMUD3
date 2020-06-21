/*
 $Author: tperry $
 $RCSfile: dilinst.h,v $
 $Date: 2002/06/09 23:27:40 $
 $Revision: 2.2 $
 */
#ifndef _MUD_DILINST_H
#define _MUD_DILINST_H

void dil_insterr (struct dilprg *p, char *where);

void dilfi_folo (struct dilprg *);
void dilfi_lcri (struct dilprg *);
void dilfi_sete (struct dilprg *);
void dilfi_amod (struct dilprg *);
void dilfi_edit (struct dilprg *);
void dilfi_stora (struct dilprg *);
void dilfi_chas (struct dilprg *);
void dilfi_setf (struct dilprg *);
void dilfi_stopf (struct dilprg *);
void dilfi_ass (struct dilprg *);
void dilfi_lnk (struct dilprg *);
void dilfi_exp (struct dilprg *);
void dilfi_if (struct dilprg *);
void dilfi_set (struct dilprg *);
void dilfi_uset (struct dilprg *);
void dilfi_adl (struct dilprg *);
void dilfi_sul (struct dilprg *);
void dilfi_ade (struct dilprg *);
void dilfi_ade2 (struct dilprg *);
void dilfi_sue (struct dilprg *);
void dilfi_dst (struct dilprg *);
void dilfi_popstk (struct dilprg *);
void dilfi_exec (struct dilprg *);
void dilfi_wit (struct dilprg *);
void dilfi_act (struct dilprg *);
void dilfi_goto (struct dilprg *);
void dilfi_sua (struct dilprg *);
void dilfi_ada (struct dilprg *);
void dilfi_pri (struct dilprg *);
void dilfi_npr (struct dilprg *);
void dilfi_snd (struct dilprg *);
void dilfi_snt (struct dilprg *);
void dilfi_snta (struct dilprg *);
void dilfi_log (struct dilprg *);
void dilfi_sec (struct dilprg *);
void dilfi_use (struct dilprg *);
void dilfi_eqp (struct dilprg *);
void dilfi_ueq (struct dilprg *);
void dilfi_quit (struct dilprg *);
void dilfi_blk (struct dilprg *);
void dilfi_pup (struct dilprg *);
void dilfi_cast (struct dilprg *);
void dilfi_rproc (struct dilprg *);
void dilfi_rfunc (struct dilprg *);
void dilfi_rts (struct dilprg *);
void dilfi_rtf (struct dilprg *);
void dilfi_dlc (struct dilprg *);
void dilfi_on (struct dilprg *);
void dilfi_rsproc (struct dilprg *);
void dilfi_rsfunc (struct dilprg *);
void dilfi_swt (struct dilprg *);
void dilfi_sbt (struct dilprg *);
void dilfi_cli (struct dilprg *);
void dilfi_foe (struct dilprg *);
void dilfi_fon (struct dilprg *);
void dilfi_sntadil (struct dilprg *);
void dilfi_rslv (struct dilprg *);
void dilfi_rsvlv (struct dilprg *);
void dilfi_rsrce (struct dilprg *);
void dilfi_pgstr (struct dilprg *);
void dilfi_send_done (struct dilprg *);
void dilfi_gamestate (struct dilprg *);
void dilfi_setpwd (struct dilprg *);
void dilfi_delpc (struct dilprg *);
void dilfi_inslst (struct dilprg *);
void dilfi_remlst (struct dilprg *);
void dilfi_reboot (struct dilprg *);
void dilfi_kedit (struct dilprg *);
void dilfe_sact (struct dilprg *);

#endif
