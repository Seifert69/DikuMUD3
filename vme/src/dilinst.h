/*
 $Author: tperry $
 $RCSfile: dilinst.h,v $
 $Date: 2002/06/09 23:27:40 $
 $Revision: 2.2 $
 */
#pragma once

#include "dil.h"

void dil_insterr(dilprg *p, char *where);

void dilfi_folo(dilprg *);
void dilfi_lcri(dilprg *);
void dilfi_sete(dilprg *);
void dilfi_amod(dilprg *);
void dilfi_edit(dilprg *);
void dilfi_stora(dilprg *);
void dilfi_chas(dilprg *);
void dilfi_setf(dilprg *);
void dilfi_stopf(dilprg *);
void dilfi_ass(dilprg *);
void dilfi_lnk(dilprg *);
void dilfi_exp(dilprg *);
void dilfi_if(dilprg *);
void dilfi_set(dilprg *);
void dilfi_uset(dilprg *);
void dilfi_adl(dilprg *);
void dilfi_sul(dilprg *);
void dilfi_ade(dilprg *);
void dilfi_ade2(dilprg *);
void dilfi_sue(dilprg *);
void dilfi_dst(dilprg *);
void dilfi_popstk(dilprg *);
void dilfi_exec(dilprg *);
void dilfi_wit(dilprg *);
void dilfi_act(dilprg *);
void dilfi_goto(dilprg *);
void dilfi_sua(dilprg *);
void dilfi_ada(dilprg *);
void dilfi_pri(dilprg *);
void dilfi_npr(dilprg *);
void dilfi_snd(dilprg *);
void dilfi_snt(dilprg *);
void dilfi_snta(dilprg *);
void dilfi_log(dilprg *);
void dilfi_sec(dilprg *);
void dilfi_use(dilprg *);
void dilfi_eqp(dilprg *);
void dilfi_ueq(dilprg *);
void dilfi_quit(dilprg *);
void dilfi_blk(dilprg *);
void dilfi_pup(dilprg *);
void dilfi_cast(dilprg *);
void dilfi_rproc(dilprg *);
void dilfi_rfunc(dilprg *);
void dilfi_rts(dilprg *);
void dilfi_rtf(dilprg *);
void dilfi_dlc(dilprg *);
void dilfi_on(dilprg *);
void dilfi_rsproc(dilprg *);
void dilfi_rsfunc(dilprg *);
void dilfi_set_weight(dilprg *p);
void dilfi_set_weight_base(dilprg *p);
void dilfi_sbt(dilprg *);
void dilfi_cli(dilprg *);
void dilfi_foe(dilprg *);
void dilfi_fon(dilprg *);
void dilfi_sntadil(dilprg *);
void dilfi_rslv(dilprg *);
void dilfi_rsvlv(dilprg *);
void dilfi_rsrce(dilprg *);
void dilfi_pgstr(dilprg *);
void dilfi_send_done(dilprg *);
void dilfi_gamestate(dilprg *);
void dilfi_setpwd(dilprg *);
void dilfi_delpc(dilprg *);
void dilfi_inslst(dilprg *);
void dilfi_remlst(dilprg *);
void dilfi_reboot(dilprg *);
void dilfi_kedit(dilprg *);
void dilfe_sact(dilprg *);
void dilfe_gint(dilprg *);
void dilfe_shell(dilprg *);
void dilfi_dispatch(dilprg *);
void dil_push_frame(dilprg *p, diltemplate *rtmpl);
void dil_start_special(unit_data *unt, dilprg *aprg);
void dil_stop_special(unit_data *unt, dilprg *aprg);
unit_data *hometown_unit(char *str);
