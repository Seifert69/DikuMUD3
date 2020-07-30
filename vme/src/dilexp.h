/*
 $Author: tperry $
 $RCSfile: dilexp.h,v $
 $Date: 2002/06/09 23:27:40 $
 $Revision: 2.3 $
 */

#ifndef _MUD_DILEXP_H
#define _MUD_DILEXP_H

#include "dil.h"

void dilfe_rti (class dilprg *);
void dilfe_txf (class dilprg *);
void dilfe_ast (class dilprg *);

void dilfe_splx (class dilprg *);
void dilfe_spli (class dilprg *);
void dilfe_mons (class dilprg *);
void dilfe_path (class dilprg *);
void dilfe_cary (class dilprg *);
void dilfe_fits (class dilprg *);
void dilfe_rest (class dilprg *);
void dilfe_resta (class dilprg *);
void dilfe_opro (class dilprg *);
void dilfe_eqpm (class dilprg *);
void dilfe_mel (class dilprg *);
void dilfe_cast2 (class dilprg *);
void dilfe_atsp (class dilprg *);
void dilfe_weat (register class dilprg *p);
void dilfe_illegal (class dilprg *);
void dilfe_plus (class dilprg *);
void dilfe_min (class dilprg *);
void dilfe_mul (class dilprg *);
void dilfe_div (class dilprg *);
void dilfe_mod (class dilprg *);
void dilfe_and (class dilprg *);
void dilfe_or (class dilprg *);
void dilfe_not (class dilprg *);
void dilfe_atoi (class dilprg *);
void dilfe_rnd (class dilprg *);
void dilfe_fndu (class dilprg *);
void dilfe_fndu2 (class dilprg *);
void dilfe_fndru (class dilprg *);
void dilfe_fndr (class dilprg *);
void dilfe_load (class dilprg *);
void dilfe_gt (class dilprg *);
void dilfe_lt (class dilprg *);
void dilfe_ge (class dilprg *);
void dilfe_le (class dilprg *);
void dilfe_eq (class dilprg *);
void dilfe_pe (class dilprg *);
void dilfe_ne (class dilprg *);
void dilfe_iss (class dilprg *);
void dilfe_in (class dilprg *);
void dilfe_getw (class dilprg *);
void dilfe_getws (class dilprg *);
void dilfe_fld (class dilprg *);
void dilfe_var (class dilprg *);
void dilfe_fs (class dilprg *);
void dilfe_int (class dilprg *);
void dilfe_umin (class dilprg *);
void dilfe_self (class dilprg *);
void dilfe_acti (class dilprg *);
void dilfe_argm (class dilprg *);
void dilfe_cmst (class dilprg *);
void dilfe_excmst (class dilprg *);
void dilfe_excmstc (class dilprg *);
void dilfe_itoa (class dilprg *);
void dilfe_len (class dilprg *);
void dilfe_se (class dilprg *);
void dilfe_fsl (class dilprg *);
void dilfe_fil (class dilprg *);
void dilfe_hrt (class dilprg *);
void dilfe_isa (class dilprg *);
void dilfe_tho (class dilprg *);
void dilfe_tda (class dilprg *);
void dilfe_tmd (class dilprg *);
void dilfe_tye (class dilprg *);
void dilfe_cmds (class dilprg *);
void dilfe_fnds (class dilprg *);
void dilfe_fnds2 (class dilprg *);
void dilfe_sne (class dilprg *);
void dilfe_pne (class dilprg *);
void dilfe_null (class dilprg *);
void dilfe_dlf (class dilprg *);
void dilfe_dld (class dilprg *);
void dilfe_lor (class dilprg *);
void dilfe_land (class dilprg *);
void dilfe_intr (class dilprg *);
void dilfe_visi (class dilprg *);
void dilfe_oppo (class dilprg *);
void dilfe_purs (class dilprg *);
void dilfe_medi (class dilprg *);
void dilfe_targ (class dilprg *);
void dilfe_powe (class dilprg *);
void dilfe_trmo (class dilprg *);
void dilfe_pck (class dilprg *);
void dilfe_act (class dilprg *);
void dilfe_islt (class dilprg *);
void dilfe_clr (class dilprg *p);
void dilfe_split (class dilprg *p);
void dilfe_ghead (class dilprg *p);
void dilfe_phead (class dilprg *p);
void dilfe_rhead (class dilprg *p);
void dilfe_nhead (class dilprg *p);
void dilfe_ohead (class dilprg *p);
void dilfe_zhead (class dilprg *p);
void dilfe_chead (class dilprg *p);
void dilfe_replace (class dilprg *);
void dilfe_meldam (class dilprg *);
void dilfe_clradd (class dilprg *);
void dilfe_clrdel (class dilprg *);
void dilfe_clrchg (class dilprg *);
void dilfe_svstr (class dilprg *);
void dilfe_flog (class dilprg *);
void dilfe_ldstr (class dilprg *);
void dilfe_udir (class dilprg *);
void dilfe_sdir (class dilprg *);
void dilfe_delstr (class dilprg *);
void dilfe_delunit (class dilprg *);
void dilfe_ckpwd (class dilprg *);
void dilfe_filesz (class dilprg *);
void dilfe_left (class dilprg *);
void dilfe_right (class dilprg *);
void dilfe_mid (class dilprg *);
void dilfe_sgt (class dilprg *);
void dilfe_slt (class dilprg *);
void dilfe_sge (class dilprg *);
void dilfe_sle (class dilprg *);
void dilfe_isplayer (class dilprg *);
void dilfe_tolower (class dilprg *);
void dilfe_toupper (class dilprg *);
void dilfe_skitxt (class dilprg *);
void dilfe_wpntxt (class dilprg *);
void dilfe_clone (class dilprg *);
void dilfe_sendpre (class dilprg *);
void dilfe_getcmd (class dilprg *);
void dilfe_gopp (class dilprg *);
void dilfe_strcmp (class dilprg *);
void dilfe_strncmp (class dilprg *);
void dilfe_wepinfo (class dilprg *);
void dilfe_gfol (class dilprg *);

#endif
