// AUTO-GENERATED – DO NOT EDIT
// Generated from dilpar.y and diltok.h on 2025-11-23 11:52
#pragma once

struct dil_type_map { const char *keyword; const char *type; const char *params; };

static const struct dil_type_map dil_types[] = {
    {"integer", "int", ""},  // DILST_INT → DILV_INT
    {"string", "string", ""},  // DILST_SP → DILV_SP
    {"stringlist", "stringlist", ""},  // DILST_SLP → DILV_SLP
    {"extraptr", "extraptr", ""},  // DILST_EDP → DILV_EDP
    {"zoneptr", "zoneptr", ""},  // DILST_ZP → DILV_ZP
    {"cmdptr", "cmdptr", ""},  // DILST_CP → DILV_CP
    {"self", "unitptr", ""},  // DILSE_SELF → DILV_UP
    {"activator", "unitptr", ""},  // DILSE_ACTI → DILV_UP
    {"medium", "unitptr", ""},  // DILSE_MEDI → DILV_UP
    {"target", "unitptr", ""},  // DILSE_TARG → DILV_UP
    {"power", "int", ""},  // DILSE_POWE → DILV_INT
    {"cmdstr", "string", ""},  // DILSE_CMST → DILV_SP
    {"excmdstr", "string", ""},  // DILSE_EXCMST → DILV_SP
    {"excmdstr_case", "string", ""},  // DILSE_EXCMSTC → DILV_SP
    {"argument", "string", ""},  // DILSE_ARGM → DILV_SP
    {"heartbeat", "int", ""},  // DILSE_HRT → DILV_INT
    {"weather", "int", ""},  // DILSE_WEAT → DILV_INT
    {"mudhour", "int", ""},  // DILSE_THO → DILV_INT
    {"mudday", "int", ""},  // DILSE_TDA → DILV_INT
    {"mudmonth", "int", ""},  // DILSE_TMD → DILV_INT
    {"mudyear", "int", ""},  // DILSE_TYE → DILV_INT
    {"birth", "int", ""},  // DILSF_BIR → DILV_INT
    {"playtime", "int", ""},  // DILSF_PTI → DILV_INT
    {"pcflags", "int", ""},  // DILSF_PCF → DILV_INT
    {"hometown", "string", ""},  // DILSF_HOME → DILV_SP
    {"master", "unitptr", ""},  // DILSF_MAS → DILV_UP
    {"lastroom", "unitptr", ""},  // DILSF_LASTROOM → DILV_UP
    {"follower", "unitptr", ""},  // DILSF_FOL → DILV_UP
    {"idx", "int", ""},  // DILSF_IDX → DILV_INT
    {"zoneidx", "string", ""},  // DILSF_ZOI → DILV_SP
    {"nameidx", "string", ""},  // DILSF_NMI → DILV_SP
    {"symname", "string", ""},  // DILSF_SYMNAME → DILV_SP
    {"key", "string", ""},  // DILSF_KEY → DILV_SP
    {"offensive", "int", ""},  // DILSF_OFFEN → DILV_INT
    {"defensive", "int", ""},  // DILSF_DEFEN → DILV_INT
    {"type", "int", ""},  // DILSF_TYP → DILV_INT
    {"next", "dilv_uezcp", ""},  // DILSF_NXT → DILV_UEZCP
    {"names", "stringlist", ""},  // DILSF_NMS → DILV_SLP
    {"creators", "stringlist", ""},  // DILSF_CREATORS → DILV_SLP
    {"name", "string", ""},  // DILSF_NAM → DILV_SP
    {"descr", "string", ""},  // DILSF_DES → DILV_SP
    {"vals", "intlist", ""},  // DILSF_VALS → DILV_ILP
    {"outside_descr", "string", ""},  // DILSF_ODES → DILV_SP
    {"inside_descr", "string", ""},  // DILSF_IDES → DILV_SP
    {"title", "string", ""},  // DILSF_TIT → DILV_SP
    {"extra", "extraptr", ""},  // DILSF_EXT → DILV_EDP
    {"outside", "unitptr", ""},  // DILSF_OUT → DILV_UP
    {"inside", "unitptr", ""},  // DILSF_INS → DILV_UP
    {"switched", "unitptr", ""},  // DILSF_SWT → DILV_UP
    {"rooms", "unitptr", ""},  // DILSF_ROOMS → DILV_UP
    {"objs", "unitptr", ""},  // DILSF_OBJS → DILV_UP
    {"npcs", "unitptr", ""},  // DILSF_NPCS → DILV_UP
    {"resetmode", "int", ""},  // DILSF_RSTMD → DILV_INT
    {"resettime", "int", ""},  // DILSF_RSTTM → DILV_INT
    {"access", "int", ""},  // DILSF_ACCESS → DILV_INT
    {"loadlevel", "int", ""},  // DILSF_LDLVL → DILV_INT
    {"payonly", "int", ""},  // DILSF_PAYONLY → DILV_INT
    {"roomcount", "int", ""},  // DILSF_NROOMS → DILV_INT
    {"objcount", "int", ""},  // DILSF_NOBJS → DILV_INT
    {"npccount", "int", ""},  // DILSF_NNPCS → DILV_INT
    {"gnext", "unitptr", ""},  // DILSF_GNX → DILV_UP
    {"loadcount", "int", ""},  // DILSF_LCN → DILV_INT
    {"gprevious", "unitptr", ""},  // DILSF_GPR → DILV_UP
    {"light", "int", ""},  // DILSF_LGT → DILV_INT
    {"bright", "int", ""},  // DILSF_BGT → DILV_INT
    {"minv", "int", ""},  // DILSF_MIV → DILV_INT
    {"illum", "int", ""},  // DILSF_ILL → DILV_INT
    {"flags", "int", ""},  // DILSF_FL → DILV_INT
    {"manipulate", "int", ""},  // DILSF_MAN → DILV_INT
    {"openflags", "int", ""},  // DILSF_OFL → DILV_INT
    {"opendiff", "int", ""},  // DILSF_OPENDIFF → DILV_INT
    {"max_hp", "int", ""},  // DILSF_MHP → DILV_INT
    {"info", "extraptr", ""},  // DILSF_INFO → DILV_EDP
    {"lifespan", "int", ""},  // DILSF_LSA → DILV_INT
    {"max_endurance", "int", ""},  // DILSF_MED → DILV_INT
    {"max_mana", "int", ""},  // DILSF_MMA → DILV_INT
    {"hp", "int", ""},  // DILSF_CHP → DILV_INT
    {"baseweight", "int", ""},  // DILSF_BWT → DILV_INT
    {"weight", "int", ""},  // DILSF_WGT → DILV_INT
    {"editing", "int", ""},  // DILSF_EDT → DILV_INT
    {"capacity", "int", ""},  // DILSF_CAP → DILV_INT
    {"alignment", "int", ""},  // DILSF_ALG → DILV_INT
    {"spells", "int", ""},  // DILSF_SPL → DILV_INT
    {"hasfunc", "int", ""},  // DILSF_FUN → DILV_INT
    {"objecttype", "int", ""},  // DILSF_OTY → DILV_INT
    {"value", "int", ""},  // DILSF_VAL → DILV_INT
    {"objectflags", "int", ""},  // DILSF_EFL → DILV_INT
    {"cost", "int", ""},  // DILSF_CST → DILV_INT
    {"rent", "int", ""},  // DILSF_RNT → DILV_INT
    {"exit_names", "stringlist", ""},  // DILSF_ONM → DILV_SLP
    {"exit_info", "int", ""},  // DILSF_XNF → DILV_INT
    {"exit_diff", "int", ""},  // DILSF_EXITDIFF → DILV_INT
    {"exit_key", "string", ""},  // DILSF_EXITKEY → DILV_SP
    {"exit_to", "unitptr", ""},  // DILSF_TOR → DILV_UP
    {"mapx", "int", ""},  // DILSF_MAPX → DILV_INT
    {"mapy", "int", ""},  // DILSF_MAPY → DILV_INT
    {"roomflags", "int", ""},  // DILSF_RFL → DILV_INT
    {"acc_balance", "int", ""},  // DILSF_ABAL → DILV_INT
    {"acc_total", "int", ""},  // DILSF_ATOT → DILV_INT
    {"speed", "int", ""},  // DILSF_SPD → DILV_INT
    {"opponentcount", "int", ""},  // DILSF_OPPCT → DILV_INT
    {"followercount", "int", ""},  // DILSF_FOLCT → DILV_INT
    {"loglevel", "int", ""},  // DILSF_LOGLVL → DILV_INT
    {"sex", "int", ""},  // DILSF_SEX → DILV_INT
    {"race", "int", ""},  // DILSF_RCE → DILV_INT
    {"abilities", "int", ""},  // DILSF_ABL → DILV_INT
    {"ability_levels", "int", ""},  // DILSF_ABL_L → DILV_INT
    {"ability_costs", "int", ""},  // DILSF_ABL_C → DILV_INT
    {"spell_levels", "int", ""},  // DILSF_SPL_L → DILV_INT
    {"spell_costs", "int", ""},  // DILSF_SPL_C → DILV_INT
    {"skill_levels", "int", ""},  // DILSF_SKL_L → DILV_INT
    {"skill_costs", "int", ""},  // DILSF_SKL_C → DILV_INT
    {"weapon_levels", "int", ""},  // DILSF_WPN_L → DILV_INT
    {"weapon_costs", "int", ""},  // DILSF_WPN_C → DILV_INT
    {"exp", "int", ""},  // DILSF_EXP → DILV_INT
    {"exptol", "int", ""},  // DILSF_EXPTOL → DILV_INT
    {"profession", "int", ""},  // DILSF_PROF → DILV_INT
    {"level", "int", ""},  // DILSF_LVL → DILV_INT
    {"vlevel", "int", ""},  // DILSF_VLVL → DILV_INT
    {"height", "int", ""},  // DILSF_HGT → DILV_INT
    {"position", "int", ""},  // DILSF_POS → DILV_INT
    {"natural_armour", "int", ""},  // DILSF_NARM → DILV_INT
    {"attack_type", "int", ""},  // DILSF_ATY → DILV_INT
    {"mana", "int", ""},  // DILSF_MNA → DILV_INT
    {"endurance", "int", ""},  // DILSF_END → DILV_INT
    {"charflags", "int", ""},  // DILSF_AFF → DILV_INT
    {"dex_red", "int", ""},  // DILSF_DRE → DILV_INT
    {"fighting", "unitptr", ""},  // DILSF_FGT → DILV_UP
    {"defaultpos", "int", ""},  // DILSF_DEF → DILV_INT
    {"crimes", "int", ""},  // DILSF_CRM → DILV_INT
    {"full", "int", ""},  // DILSF_FLL → DILV_INT
    {"thirst", "int", ""},  // DILSF_THR → DILV_INT
    {"drunk", "int", ""},  // DILSF_DRK → DILV_INT
    {"skill_points", "int", ""},  // DILSF_SPT → DILV_INT
    {"ability_points", "int", ""},  // DILSF_APT → DILV_INT
    {"guild", "string", ""},  // DILSF_GLD → DILV_SP
    {"prompt", "string", ""},  // DILSF_PROMPT → DILV_SP
    {"fname", "string", ""},  // DILSF_FNAME → DILV_SP
    {"notes", "string", ""},  // DILSF_NOTES → DILV_SP
    {"help", "string", ""},  // DILSF_HELP → DILV_SP
    {"quests", "extraptr", ""},  // DILSF_QST → DILV_EDP
    {"skills", "int", ""},  // DILSF_SKL → DILV_INT
    {"null", "null", ""},  // DILSE_NULL → DILV_NULL
    {"not", "int", ""},  // DILSE_NOT → DILV_INT
    {"atoi", "int", "string"},  // DILSE_ATOI → DILV_INT
    {"isplayer", "int", "string"},  // DILSE_ISPLAYER → DILV_INT
    {"shell", "int", "string"},  // DILSE_SHELL → DILV_INT
    {"dildestroy", "int", "string, unitptr"},  // DILSE_DLD → DILV_INT
    {"dilcall", "int", "string, unknown, unknown, unknown"},  // DILSE_CALL → DILV_INT
    {"dilfind", "int", "string, unitptr"},  // DILSE_DLF → DILV_INT
    {"length", "int", "intlist|string|stringlist"},  // DILSE_LEN → DILV_INT
    {"itoa", "string", "int"},  // DILSE_ITOA → DILV_SP
    {"weapon_name", "string", "int"},  // DILSE_WPNTXT → DILV_SP
    {"skill_name", "string", "int"},  // DILSE_SKITXT → DILV_SP
    {"act", "string", "string, int, null|string|unitptr, null|string|unitptr, null|string|unitptr, int"},  // DILSI_ACT → DILV_SP
    {"rnd", "int", "int, int"},  // DILSE_RND → DILV_INT
    {"isset", "int", "int, int"},  // DILSE_ISS → DILV_INT
    {"strcmp", "int", "string, string"},  // DILSE_STRCMP → DILV_INT
    {"strncmp", "int", "string, string, int"},  // DILSE_STRNCMP → DILV_INT
    {"isaff", "int", "unitptr, int"},  // DILSE_ISA → DILV_INT
    {"findunit", "unitptr", "unitptr, string, int, null|unitptr, int"},  // DILSE_FNDU → DILV_UP
    {"findunit", "unitptr", "unitptr, string, int, null|unitptr"},  // DILSE_FNDU → DILV_UP
    {"global_head", "unitptr", ""},  // DILSE_GHEAD → DILV_UP
    {"pc_head", "unitptr", ""},  // DILSE_PHEAD → DILV_UP
    {"room_head", "unitptr", ""},  // DILSE_RHEAD → DILV_UP
    {"npc_head", "unitptr", ""},  // DILSE_NHEAD → DILV_UP
    {"obj_head", "unitptr", ""},  // DILSE_OHEAD → DILV_UP
    {"zone_head", "zoneptr", ""},  // DILSE_ZHEAD → DILV_ZP
    {"chead", "cmdptr", ""},  // DILSE_CHEAD → DILV_CP
    {"findrndunit", "unitptr", "unitptr, int, int"},  // DILSE_FNDRU → DILV_UP
    {"weapon_info", "intlist", "int"},  // DILSE_WEPINFO → DILV_ILP
    {"findroom", "unitptr", "string"},  // DILSE_FNDR → DILV_UP
    {"findzone", "zoneptr", "string"},  // DILSE_FNDZ → DILV_ZP
    {"findsymbolic", "unitptr", "string"},  // DILSE_FNDS → DILV_UP
    {"findsymbolic", "unitptr", "string, int"},  // DILSE_FNDS → DILV_UP
    {"findsymbolic", "unitptr", "unitptr, string, int"},  // DILSE_FNDS → DILV_UP
    {"visible", "int", "unitptr, unitptr"},  // DILSE_VISI → DILV_INT
    {"paycheck", "int", "unitptr, unitptr"},  // DILSE_PCK → DILV_INT
    {"opponent", "int", "unitptr, unitptr"},  // DILSE_OPPO → DILV_INT
    {"getopponent", "unitptr", "unitptr, int"},  // DILSE_GOPP → DILV_UP
    {"getfollower", "unitptr", "unitptr, int"},  // DILSE_GFOL → DILV_UP
    {"spellindex", "int", "string"},  // DILSE_SPLX → DILV_INT
    {"moneystring", "string", "int, int"},  // DILSE_MONS → DILV_SP
    {"pathto", "int", "unitptr, unitptr"},  // DILSE_PATH → DILV_INT
    {"can_carry", "int", "unitptr, unitptr, int"},  // DILSE_CARY → DILV_INT
    {"getcolor", "string", "string"},  // DILSE_GETCLR → DILV_SP
    {"send_pre", "int", "string, null|unitptr, null|unitptr, null|unitptr, int, string, null|unitptr"},  // DILSE_SENDPRE → DILV_INT
    {"addcolor", "int", "unitptr, string, string"},  // DILSE_ADDCLR → DILV_INT
    {"changecolor", "int", "unitptr, string, string"},  // DILSE_CHGCLR → DILV_INT
    {"delcolor", "int", "unitptr, string"},  // DILSE_DELCLR → DILV_INT
    {"check_password", "int", "unitptr, string"},  // DILSE_CKPWD → DILV_INT
    {"islight", "int", "unitptr"},  // DILSE_ISLT → DILV_INT
    {"replace", "string", "string, string, string"},  // DILSE_REPLACE → DILV_SP
    {"mid", "string", "string, int, int"},  // DILSE_MID → DILV_SP
    {"tolower", "string", "string"},  // DILSE_TOLOWER → DILV_SP
    {"toupper", "string", "string"},  // DILSE_TOUPPER → DILV_SP
    {"left", "string", "string, int"},  // DILSE_LEFT → DILV_SP
    {"right", "string", "string, int"},  // DILSE_RIGHT → DILV_SP
    {"purse", "int", "unitptr, string"},  // DILSE_PURS → DILV_INT
    {"transfermoney", "int", "null|unitptr, null|unitptr, int"},  // DILSE_TRMO → DILV_INT
    {"getcmd", "cmdptr", "string"},  // DILSE_GETCMD → DILV_CP
    {"load", "unitptr", "string"},  // DILSE_LOAD → DILV_UP
    {"clone", "unitptr", "unitptr"},  // DILSE_CLONE → DILV_UP
    {"attack_spell", "int", "int, unitptr, unitptr, unitptr, int, null|string"},  // DILSE_ATSP → DILV_INT
    {"cast_spell", "int", "int, unitptr, unitptr, unitptr, string"},  // DILSE_CST → DILV_INT
    {"fits", "string", "unitptr, unitptr, int"},  // DILSE_FIT → DILV_SP
    {"restore", "unitptr", "string, null|unitptr"},  // DILSE_RESTA → DILV_UP
    {"delstr", "int", "string"},  // DILSE_DELSTR → DILV_INT
    {"delunit", "int", "string"},  // DILSE_DELUNIT → DILV_INT
    {"openroll", "int", "int, int"},  // DILSE_OPRO → DILV_INT
    {"equipment", "unitptr", "unitptr, int"},  // DILSE_EQPM → DILV_UP
    {"meleeattack", "int", "unitptr, unitptr, int, int, int"},  // DILSE_MEL → DILV_INT
    {"meleedamage", "int", "unitptr, unitptr, int, int"},  // DILSE_MELDAM → DILV_INT
    {"textformat", "string", "string"},  // DILSE_TXF → DILV_SP
    {"sact", "string", "string, int, null|string|unitptr, null|string|unitptr, null|string|unitptr, int"},  // DILSE_SACT → DILV_SP
    {"getinteger", "int", "int, null|unitptr, int"},  // DILSE_GINT → DILV_INT
    {"asctime", "string", "int"},  // DILSE_AST → DILV_SP
    {"getword", "string", "string"},  // DILSE_GETW → DILV_SP
    {"getwords", "stringlist", "string"},  // DILSE_GETWS → DILV_SLP
    {"split", "stringlist", "string, string"},  // DILSE_SPLIT → DILV_SLP
    {"getaffects", "stringlist", "unitptr"},  // DILSE_GETAFFECTS → DILV_SLP
    {"strdir", "stringlist", "string"},  // DILSE_SDIR → DILV_SLP
    {"unitdir", "stringlist", "string"},  // DILSE_UDIR → DILV_SLP
    {"savestr", "int", "string, string, string"},  // DILSE_SVSTR → DILV_INT
    {"loadstr", "int", "string, string"},  // DILSE_LDSTR → DILV_INT
    {"flog", "int", "string, string, string"},  // DILSE_FLOG → DILV_INT
    {"command", "int", "unknown"},  // DILSE_CMDS → DILV_INT
    {nullptr, nullptr, nullptr}
};
