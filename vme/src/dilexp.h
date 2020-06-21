/*
 $Author: tperry $
 $RCSfile: dilexp.h,v $
 $Date: 2002/06/09 23:27:40 $
 $Revision: 2.3 $
 */

#ifndef _MUD_DILEXP_H
#define _MUD_DILEXP_H

#include "dil.h"

void dilfe_rti (struct dilprg *);
void dilfe_txf (struct dilprg *);
void dilfe_ast (struct dilprg *);

void dilfe_splx (struct dilprg *);
void dilfe_spli (struct dilprg *);
void dilfe_mons (struct dilprg *);
void dilfe_path (struct dilprg *);
void dilfe_cary (struct dilprg *);
void dilfe_fits (struct dilprg *);
void dilfe_rest (struct dilprg *);
void dilfe_resta (struct dilprg *);
void dilfe_opro (struct dilprg *);
void dilfe_eqpm (struct dilprg *);
void dilfe_mel (struct dilprg *);
void dilfe_cast2 (struct dilprg *);
void dilfe_atsp (struct dilprg *);
void dilfe_weat (register struct dilprg *p);
void dilfe_illegal (struct dilprg *);
void dilfe_plus (struct dilprg *);
void dilfe_min (struct dilprg *);
void dilfe_mul (struct dilprg *);
void dilfe_div (struct dilprg *);
void dilfe_mod (struct dilprg *);
void dilfe_and (struct dilprg *);
void dilfe_or (struct dilprg *);
void dilfe_not (struct dilprg *);
void dilfe_atoi (struct dilprg *);
void dilfe_rnd (struct dilprg *);
void dilfe_fndu (struct dilprg *);
void dilfe_fndu2 (struct dilprg *);
void dilfe_fndru (struct dilprg *);
void dilfe_fndr (struct dilprg *);
void dilfe_load (struct dilprg *);
void dilfe_gt (struct dilprg *);
void dilfe_lt (struct dilprg *);
void dilfe_ge (struct dilprg *);
void dilfe_le (struct dilprg *);
void dilfe_eq (struct dilprg *);
void dilfe_pe (struct dilprg *);
void dilfe_ne (struct dilprg *);
void dilfe_iss (struct dilprg *);
void dilfe_in (struct dilprg *);
void dilfe_getw (struct dilprg *);
void dilfe_getws (struct dilprg *);
void dilfe_fld (struct dilprg *);
void dilfe_var (struct dilprg *);
void dilfe_fs (struct dilprg *);
void dilfe_int (struct dilprg *);
void dilfe_umin (struct dilprg *);
void dilfe_self (struct dilprg *);
void dilfe_acti (struct dilprg *);
void dilfe_argm (struct dilprg *);
void dilfe_cmst (struct dilprg *);
void dilfe_excmst (struct dilprg *);
void dilfe_excmstc (struct dilprg *);
void dilfe_itoa (struct dilprg *);
void dilfe_len (struct dilprg *);
void dilfe_se (struct dilprg *);
void dilfe_fsl (struct dilprg *);
void dilfe_fil (struct dilprg *);
void dilfe_hrt (struct dilprg *);
void dilfe_isa (struct dilprg *);
void dilfe_tho (struct dilprg *);
void dilfe_tda (struct dilprg *);
void dilfe_tmd (struct dilprg *);
void dilfe_tye (struct dilprg *);
void dilfe_cmds (struct dilprg *);
void dilfe_fnds (struct dilprg *);
void dilfe_fnds2 (struct dilprg *);
void dilfe_sne (struct dilprg *);
void dilfe_pne (struct dilprg *);
void dilfe_null (struct dilprg *);
void dilfe_dlf (struct dilprg *);
void dilfe_dld (struct dilprg *);
void dilfe_lor (struct dilprg *);
void dilfe_land (struct dilprg *);
void dilfe_intr (struct dilprg *);
void dilfe_visi (struct dilprg *);
void dilfe_oppo (struct dilprg *);
void dilfe_purs (struct dilprg *);
void dilfe_medi (struct dilprg *);
void dilfe_targ (struct dilprg *);
void dilfe_powe (struct dilprg *);
void dilfe_trmo (struct dilprg *);
void dilfe_pck (struct dilprg *);
void dilfe_act (struct dilprg *);
void dilfe_islt (struct dilprg *);
void dilfe_clr (struct dilprg *p);
void dilfe_split (struct dilprg *p);
void dilfe_ghead (struct dilprg *p);
void dilfe_phead (struct dilprg *p);
void dilfe_rhead (struct dilprg *p);
void dilfe_nhead (struct dilprg *p);
void dilfe_ohead (struct dilprg *p);
void dilfe_zhead (struct dilprg *p);
void dilfe_chead (struct dilprg *p);
void dilfe_replace (struct dilprg *);
void dilfe_meldam (struct dilprg *);
void dilfe_clradd (struct dilprg *);
void dilfe_clrdel (struct dilprg *);
void dilfe_clrchg (struct dilprg *);
void dilfe_svstr (struct dilprg *);
void dilfe_flog (struct dilprg *);
void dilfe_ldstr (struct dilprg *);
void dilfe_udir (struct dilprg *);
void dilfe_sdir (struct dilprg *);
void dilfe_delstr (struct dilprg *);
void dilfe_delunit (struct dilprg *);
void dilfe_ckpwd (struct dilprg *);
void dilfe_filesz (struct dilprg *);
void dilfe_left (struct dilprg *);
void dilfe_right (struct dilprg *);
void dilfe_mid (struct dilprg *);
void dilfe_sgt (struct dilprg *);
void dilfe_slt (struct dilprg *);
void dilfe_sge (struct dilprg *);
void dilfe_sle (struct dilprg *);
void dilfe_isplayer (struct dilprg *);
void dilfe_tolower (struct dilprg *);
void dilfe_toupper (struct dilprg *);
void dilfe_skitxt (struct dilprg *);
void dilfe_wpntxt (struct dilprg *);
void dilfe_clone (struct dilprg *);
void dilfe_sendpre (struct dilprg *);
void dilfe_getcmd (struct dilprg *);
void dilfe_gopp (struct dilprg *);
void dilfe_strcmp (struct dilprg *);
void dilfe_strncmp (struct dilprg *);
void dilfe_wepinfo (struct dilprg *);
void dilfe_gfol (struct dilprg *);

#endif
