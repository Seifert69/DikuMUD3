/*
 $Author: All $
 $RCSfile: dil.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */

#ifndef _MUD_DIL_H
#define _MUD_DIL_H

#include "namelist.h"
#include "intlist.h"
#include "t_array.hpp"

#define WAITCMD_MAXINST	   32000

#define SKIP     0xffffffff	/* skip label/index defined */

/* The following "dilarg" structs are used only for VMC purposes to allow
   the user to enter the : dilcopy prg@zon(arg, arg, arg); instead of the
   stupid string format */

struct dilargtype
{
    ubit8 type;
    union
    {
        char *string;
        char **stringlist;
        int *intlist;
        ubit32 num;
    }
    data;
};

struct dilargstype
{
    ubit8 no;
    char *name;
    struct dilargtype dilarg[256];
};


/*
 *  The expressions and instructions are as follows:
 *  All expressions are prefixed, and their type defined by the leading
 *  DIL?_* value. Functions and instructions are followed by
 *  the argument expressions. Code pointers are 32-bit pointers point to
 *  other positions in the DIL intruction memory.
 */

#define DILFL_COPY        0x0001	// Program is a --copy--
#define DILFL_RECALL      0x0002	// Recall PC?
#define DILFL_EXECUTING   0x0004	// Program is already executing?
#define DILFL_CMDBLOCK    0x0008	// Block the currently executed command?
#define DILFL_AWARE       0x0010	// To determine when to use SFB_AWARE
#define DILFL_FREEME      0x0020	// If "copy" is just a faked copy.
#define DILFL_DEACTIVATED 0x0040	// True when dil may not be executed
#define DILFL_UNIQUE      0x0100	// No more than one copy of this dil per unit

/* DIL expressions */
#define DIL_ILL		0	/* Illegal (for debugging purpose) */
#define DILE_PLUS	1	/* # + # */
#define DILE_MIN	2	/* # - # */
#define DILE_MUL	3	/* # * # */
#define DILE_DIV	4	/* # / # */
#define DILE_MOD	5	/* # % # */
#define DILE_AND	6	/* # and # */
#define DILE_OR		7	/* # or # */
#define DILE_NOT	8	/* not # */
#define DILE_GT		9	/* # > # */
#define DILE_LT		10	/* # < # */
#define DILE_GE		11	/* # >= # */
#define DILE_LE		12	/* # <= # */
#define DILE_EQ		13	/* # == # */
#define DILE_PE		14	/* # #= # */
#define DILE_NE		15	/* # != # */
#define DILE_IN		16	/* # in # */
#define DILE_UMIN	17	/* - # */
#define DILE_SE		18	/* # ?= # */
#define DILE_NULL	19	/* null */

/* DIL functions */
#define DILE_FLD	20	/* get field + ubit8 DILF_? */
#define DILE_ATOI	21	/* atoi(#) */
#define DILE_ITOA	22	/* itoa(#) */
#define DILE_RND	23	/* rnd(#,#) */
#define DILE_FNDU	24	/* findunit(#,#,#) */
#define DILE_FNDR	25	/* findroom(#,#) */
#define DILE_LOAD	26	/* load(#) */
#define DILE_ISS	27	/* isset(#,#) */
#define DILE_GETW	28	/* getword(#) */
#define DILE_ISA	29	/* isaff(#,#) */
#define DILE_CMDS	30	/* command(#) */
#define DILE_FNDS	31	/* findsymbolic(#) */

/* DIL internal variables */
#define DILE_ACTI	32	/* activator */
#define DILE_ARGM	33	/* argument */
#define DILE_THO	34	/* mudhour */
#define DILE_TDA	35	/* mudday */
#define DILE_TMD	36	/* mudmonth */
#define DILE_TYE	37	/* mudyear */
#define DILE_HRT	38	/* heartbeat */
#define DILE_SELF	39	/* self */

/* DIL static references */
#define DILE_VAR	40	/* variable (ubit16) */
#define DILE_FS	        41	/* fixed string (char[]) */
#define DILE_FSL	42	/* fixed stringlist (char[][]) */
#define DILE_INT	43	/* fixed integer (sbit32) */
#define DILE_LEN	44	/* length(#) */

/* DIL instructions */
#define DILI_ASS	45	/* # = # */
#define DILI_LNK	46	/* link(#,#) */
#define DILI_EXP	47	/* experience(#) */
#define DILI_CAST	48	/* cast_spell */
#define DILI_IF         49	/* if */
#define DILI_SET	50	/* set (#,#) */
#define DILI_USET	51	/* unset (#,#) */
#define DILI_ADL	52	/* addlist (#,#) */
#define DILI_SUL	53	/* sublist (#,#) */
#define DILI_ADE	54	/* addextra (#,#,#) */
#define DILI_SUE	55	/* subextra (#,#) */
#define DILI_DST	56	/* destroy (#) */
#define DILI_POPSTK   57      /* pop the stack and thow it away*/
#define DILI_EXEC	58	/* exec (#,#) */
#define DILI_WIT	59	/* wait (#,#) */
#define DILI_ACT	60	/* act (#,#,#,#,#,#) */
#define DILI_GOTO	61	/* goto label */
#define DILI_SUA	62	/* subaff (#,#) */
#define DILI_ADA	63	/* addaff (#,#,#) */
#define DILI_PRI	64	/* priority */
#define DILI_NPR	65	/* nopriority */
#define DILI_SND	66	/* send (#) */
#define DILI_SNT	67	/* sendto (#,#) */
#define DILI_SEC	68	/* secure (#,@) */
#define DILI_USE	69	/* unsecure (#) */
#define DILI_FOE	70	/* foreach - clear / build list */
#define DILI_FON	71	/* foreach - get next in environment */
#define DILI_EQP	72	/* addequip (#,#) */
#define DILI_UEQ	73	/* unequip (#) */
#define DILE_WEAT	74	/* weather */
#define DILE_OPPO	75	/* opponent(#,#) */
#define DILI_QUIT	76	/* quit */
#define DILI_BLK	77	/* block */
#define DILI_PUP	78	/* position_update */

/* Extensions */
#define DILE_GETWS	79	/* getwords(#) */
#define DILI_SNTA	80	/* sendtoall */
#define DILI_LOG	81	/* log */
#define DILE_PNEQ	82	/* not # #= # */
#define DILE_SNEQ	83	/* not # $= # */
#define DILI_RPC        84	/* remote procedure call */
#define DILI_RFC        85	/* remote function call */
#define DILI_RTS        86	/* return from subroutine */
#define DILI_RTF        87	/* return from function */
#define DILE_DLD        88	/* dil destroy (#,#) */
#define DILE_DLF        89	/* dil find (#,#) */
#define DILI_DLC        90	/* dil copy (#,#) */
#define DILE_LOR        91	/* logical or */
#define DILE_LAND       92	/* logical and */
#define DILI_ON 	93	/* on # @ @ @ @ */
#define DILI_SPC	94	/* symbolic procedure call */
#define DILI_SFC	95	/* symbolic function call */
#define DILE_INTR	96	/* interrupt */
#define DILI_CLI        97	/* clear interrupt */
#define DILI_SBT	98	/* setbright (#,#) */
#define DILI_SWT	99	/* setweight (#,#) */
#define DILE_FNDRU     100	/* findunit(#,#,#) */
#define DILE_VISI      101	/* visible(#,#)   */
#define DILE_ATSP      102	/* attack_spell(#,#,#,#,#) */
#define DILE_PURS      103	/* purse(#,#) */
#define DILI_CHAS      104	/* change_speed(#,#) */
#define DILI_SETF      105	/* set_fighting(#,#) */
#define DILE_MEDI      106	/* medium */
#define DILE_TARG      107	/* target */
#define DILE_POWE      108	/* power */
#define DILE_TRMO      109	/* transfermoney(#,#,#) */
#define DILI_SNTADIL   110	/* sendtoalldil(#,#,#) */
#define DILE_CAST2     111	/* int cast_spell(#,#,#,#,#) */
#define DILE_MEL       112	/* int meleeattack(#,#,#) */
#define DILE_EQPM      113	/* unitptr equipment(#,#) */
#define DILE_CMST      114	/* cmdstr */
#define DILE_OPRO      115	/* openroll(#,#) */

#define DILE_DELSTR    116	/* delstr(filename) */
#define DILE_DELUNIT   117	/* delunit(filename)     */
#define DILI_AMOD      118	/* acc_modify(#,#)  */
#define DILI_SETE      119	/* sendtext(#,#)  */

#define DILI_FOLO      120	/* follow(#,#)       */
#define DILI_LCRI      121	/* logcrime(#,#,#)   */
#define DILE_FIT       122	/* fits(#,#,#)       */
#define DILE_CARY      123	/* can_carry(#,#)    */
#define DILE_FNDS2     124	/* findsymbolic(#,#) */
#define DILE_PATH      125	/* pathto(#,#)       */
#define DILE_MONS      126	/* moneystring(#,#)  */
#define DILE_SPLX      127	/* spellindex(#)     */
#define DILE_SPLI      128	/* spellinfo(#,#)    */

#define DILE_RTI       129	/* realtime()        */
#define DILE_TXF       130	/* textformat(#)     */
#define DILE_AST       131	/* asctime(#)        */
#define DILE_PCK       132	/* paycheck(#,#)   */
#define DILE_ACT       133	/* act(...) expression */
#define DILE_ISLT      134	/* islight(#) */
#define DILE_GETCLR    135	/* getcolor(#,#) */
#define DILE_ADDCLR    136	/* addcolor(#,#,#) */
#define DILE_SPLIT     137	/* split(#,#) */
#define DILE_GHEAD     138	/* ghead() */
#define DILE_REPLACE   139	/* replace(#,#,#) */
#define DILE_MELDAM    140	/* int meleedamage(#,#,#) */
#define DILI_RSLV      141	/* reset_level(#) */
#define DILI_RSVLV     142	/* reset_vlevel(#) */
#define DILI_RSRCE     143	/* reset_race(#) */
#define DILI_PGSTR     144	/* reset_race(#) */
#define DILE_DELCLR    145	/* delcolor(#,#) */
#define DILE_CHGCLR    146	/* changecolor(#,#, #) */
#define DILE_SVSTR     147	/* int = savestr(filename, string) */
#define DILE_LDSTR     148	/* int = loadstr(filename, string) */
#define DILE_FLOG      149	/* int = flog(filename, string) */
#define DILE_RESTA     150	/* restoreall(#,#) */
#define DILI_STORA     151	/* storeall(#)     */
#define DILI_STOPF     152	/* stopfighting(#)     */
#define DILI_EDIT      153	/* beginedit     */
#define DILE_ZHEAD     154
#define DILE_UDIR      155
#define DILE_SDIR      156
#define DILI_SNDDONE   157
#define DILI_GMSTATE   158
#define DILI_SETPWD    159
#define DILI_DELPC     160
#define DILE_CKPWD     161
#define DILE_LEFT      162
#define DILE_RIGHT     163
#define DILE_MID       164
#define DILE_SGT		165	/* # > # */
#define DILE_SLT		166	/* # < # */
#define DILE_SGE		167	/* # >= # */
#define DILE_SLE		168	/* # <= # */
#define DILE_ISPLAYER		169	/* # <= # */
#define DILE_TOLOWER   170
#define DILE_TOUPPER   171
#define DILE_SKITXT    172
#define DILE_WPNTXT    173
#define DILE_CLONE     174
#define DILE_CHEAD     175
#define DILE_SENDPRE   176
#define DILE_EXCMST    177
#define DILE_FIL       178
#define DILI_INSLST    179
#define DILI_REMLST    180
#define DILI_ADE2	     181	/* addextra (#,#,#,#) */
#define DILE_GETCMD	  182	/* getcmd (#) */
#define DILI_REBOOT    183	/*Reboot the server */
#define DILI_KEDIT    184	/*Reboot the server */
#define DILE_GOPP	185
#define DILE_EXCMSTC    186
#define DILE_STRCMP    187
#define DILE_STRNCMP   188
#define DILE_WEPINFO   189
#define DILE_NHEAD     190
#define DILE_RHEAD     191
#define DILE_OHEAD     192
#define DILE_PHEAD     193
#define DILE_FNDU2     194
#define DILE_GFOL      195
#define DILE_SACT	   196	/* sact (#,#,#,#,#,#) */
#define DILE_GINT	   197	/* getinteger(unit, idx) */

#define DILI_MAX       197	/* The maximum node number */

/* DIL Field references */
#define DILF_NMS	0	/* .names */
#define DILF_ODES	1	/* .descr */
#define DILF_NXT	2	/* .next */
#define DILF_NAM	3	/* .name */
#define DILF_TIT	4	/* .title */
#define DILF_EXT	5	/* .extra */
#define DILF_OUT	6	/* .outside */
#define DILF_INS	7	/* .inside */
#define DILF_GNX	8	/* .gnext */
#define DILF_GPR	9	/* .gprevious */
#define DILF_FUN	10	/* .hasfun */
#define DILF_MHP	11	/* .max_hp */
#define DILF_CHP	12	/* .hp */
#define DILF_MAN	13	/* .manipulate */
#define DILF_FL		14	/* .flags */
#define DILF_BWT	15	/* .baseweight */
#define DILF_WGT	16	/* .weight */
#define DILF_CAP	17	/* .capacity */
#define DILF_ALG	18	/* .alignment */
#define DILF_FGT	19	/* .fighting */
#define DILF_OFL	20	/* .openflags */
#define DILF_LGT	21	/* .light */
#define DILF_BGT	22	/* .bright */
#define DILF_ILL	23	/* .illum */
#define DILF_SPL	24	/* .spell */
#define DILF_VAL	25	/* .values */
#define DILF_EFL	26	/* .objectflags */
#define DILF_CST	27	/* .cost */
#define DILF_RNT	28	/* .rent */
#define DILF_OTY	29	/* .objecttype */
#define DILF_EQP	30	/* .equip */
#define DILF_MOV	31	/* .movement */
#define DILF_ONM	32	/* .exit_name */
#define DILF_XNF	33	/* .exit_info */
#define DILF_TOR	34	/* .exit_to */
#define DILF_RFL	35	/* .roomflags */
#define DILF_ZON	36	/* .zone */

#define DILF_ACT	37	/* .behavior */
#define DILF_TIM	38	/* .time */
#define DILF_EXP	39	/* .exp */
#define DILF_AFF	40	/* .affected */
#define DILF_MNA	41	/* .mana */
#define DILF_END	42	/* .endurance */
#define DILF_ATY	43	/* .attack_type */
#define DILF_DEF	44	/* .defaultpos */
#define DILF_HGT	45	/* .height */
#define DILF_RCE	46	/* .race */
#define DILF_DRE	47	/* .dex_red */
#define DILF_SEX	48	/* .sex */
#define DILF_LVL	49	/* .level */
#define DILF_POS	50	/* .position */
#define DILF_ABL	51	/* .abilities */
#define DILF_WPN	52	/* .weapons */

#define DILF_MAS	53	/* .master */
#define DILF_FOL	54	/* .follower */
#define DILF_MIV	55	/* .minv */
#define DILF_SPT	56	/* .skill_points */
#define DILF_APT	57	/* .ability_points */
#define DILF_SKL	58	/* .skills */
#define DILF_GLD	59	/* .guild */
#define DILF_CRM	60	/* .crime */
#define DILF_FLL	61	/* .full */
#define DILF_THR	62	/* .thirst */
#define DILF_DRK	63	/* .drunk */
#define DILF_QST	64	/* .quests */
#define DILF_IDX	65	/* .[] */
#define DILF_NMI	66	/* .nameidx */
#define DILF_ZOI	67	/* .zoneidx */
#define DILF_TYP	68	/* .objecttype */

#define DILF_BIR        69	/* .birth */
#define DILF_PTI        70	/* .playtime */
#define DILF_PCF        71	/* .pcflags */
#define DILF_HOME       72	/* .home */
#define DILF_IDES	73	/* .inside_descr */
#define DILF_DES	74	/* .descr */
#define DILF_LCN        75	/* .loadcount */
#define DILF_SID        76	/* .idx (symbolic index as integer) */
#define DILF_SPD        77	/* .speed */
#define DILF_ABAL       78	/* .acc_balance */
#define DILF_ATOT       79	/* .acc_total   */
#define DILF_MMA	80	/* .max_mana */
#define DILF_LSA	81	/* .lifespan */
#define DILF_INFO	82	/* .info */
#define DILF_MED	83	/* .max_endurance */
#define DILF_EXPTOL	84	/* .exptol */
#define DILF_VLVL	85	/* .vlevel */
#define DILF_PROMPT 86		/* .prompt */
#define DILF_ROOMS 87		/*.rooms */
#define DILF_OBJS 88		/*.objects */
#define DILF_NPCS 89		/*.npcs */
#define DILF_NROOMS 90		/*roomcount */
#define DILF_NOBJS 91		/*objcount */
#define DILF_NNPCS 92		/*npccount */
#define DILF_RSTMD 93		/*resetmode */
#define DILF_RSTTM 94		/*resettime */
#define DILF_ACCESS 95		/*access */
#define DILF_LDLVL 96		/*loadlevel */
#define DILF_FNAME 97		/*filename */
#define DILF_NOTES 98		/*notes */
#define DILF_HELP 99		/*help */
#define DILF_CREATORS 100	/*creators */
#define DILF_PAYONLY 101	/*payonly */
#define DILF_EDT  102		/*editing */
#define DILF_SWT  103		/*switched */
#define DILF_LOGLVL 104
#define DILF_VALS 105
#define DILF_NARM 106
#define DILF_OPPCT 107
#define DILF_KEY 108
#define DILF_LASTROOM 109
#define DILF_EXITKEY 110
#define DILF_FOLCT 111
#define DILF_OFFEN 112
#define DILF_DEFEN 113
#define DILF_ABL_L	114	/* .ability_levels */
#define DILF_ABL_C	115	/* .ability_costs */
#define DILF_SPL_L	116	/* .spell_levels */
#define DILF_SPL_C	117	/* .spell_costs */
#define DILF_SKL_L	118	/* .skill_levels */
#define DILF_SKL_C	119	/* .skill_costs */
#define DILF_WPN_L	120	/* .weapon_levels */
#define DILF_WPN_C 121	/* .weapon_costs */
#define DILF_MAPX  122  /* room.mapx */
#define DILF_MAPY  123  /* room.mapy */
#define DILF_XDIFF 124  /*.exit_diff[UP] (room) */
#define DILF_ODI   125  /*.opendiff (unit) */
#define DILF_PROF  126	/* .profession */

#define DILF_MAX 	126	/* The maximum field number */

/* Legal variable values */
#define DILV_UP         1	/* unit pointer Rexpr Var */
#define DILV_SP         2	/* string pointer Rexpr Var */
#define DILV_SLP        3	/* string list pointer Var */
#define DILV_EDP        4	/* extra description pointer Rexpr Var */
#define DILV_INT        5	/* integer Rexpr Var */

/* Other return values */
#define DILV_UPR        6	/* unit pointer ref Lexpr */
#define DILV_SPR        7	/* string pointer Rexpr */
#define DILV_SLPR       8	/* string list pointer */
#define DILV_EDPR       9	/* extra descr pointer ref Lexpr */

/* Different pointers for different size fields */
#define DILV_SINT1R     10	/* integer ref, 8 bit, sgn Lexpr */
#define DILV_SINT2R     11	/* integer ref, 16 bit, sgn Lexpr */
#define DILV_SINT4R     12	/* integer ref, 32 bit, sgn Lexpr */
#define DILV_UINT1R     13	/* integer ref, 8 bit, usgn Lexpr */
#define DILV_UINT2R     14	/* integer ref, 16 bit, usgn Lexpr */
#define DILV_UINT4R     15	/* integer ref, 32 bit, usgn Lexpr */

#define DILV_ERR	16	/* fatal error value */
#define DILV_NULL       17	/* value is null */
#define DILV_FAIL       18	/* value is failed */

#define DILV_UEDP       19	/* FOR COMPILER ONLY! (shared fields) */
#define DILV_FUNC       20	/* FOR COMPILER ONLY! functions */
#define DILV_PROC       21	/* FOR COMPILER ONLY! procedures */
#define DILV_HASHSTR    22	/* Hashed String */

#define DILV_ZP         23	/* Zone Pointer Rexpr */
#define DILV_ZPR        24	/* zone pointer Lexpr */
#define DILV_UZP        25	/* FOR COMPILER ONLY! unit/zone shared */
#define DILV_EZP        26	/* FOR COMPILER ONLY! extra/zone shared */
#define DILV_UEZP       27	/* FOR COMPILER ONLY! all shared */
#define DILV_SP_SLP     28	/* FOR COMPILER ONLY! all shared */
#define DILV_CP         29	/* Command Pointer Rexpr */
#define DILV_CPR        30	/* Command pointer Lexpr */
#define DILV_UEZCP      31	/* FOR COMPILER ONLY! all shared */
#define DILV_UCP        32	/* FOR COMPILER ONLY! all shared */
#define DILV_ESLP       33	/* FOR COMPILER ONLY! all shared */
#define DILV_ILP         34	/* intlist pointer Rexpr */
#define DILV_ILPR        35	/* intlist pointer Lexpr */
#define DILV_ESLIP      36	/* For compiler shared e sl il */
#define DILV_ALL        37	/* sym functions can return anything at runtim */
#define DILV_MAX        37	/* Max number */

#define DILT_REG   1
#define DILT_CONST 2
#define DILT_ARG   3

/* DIL variable structure */
struct dilvar
{
    ubit8 type;			/* variable type - string, integer, ...*/
    ubit8 itype;    /* Regular, constant, argument */
    char *name;     /* variable name */
    union
    {
        class unit_data *unitptr;
        class zone_type *zoneptr;
        struct command_info *cmdptr;
        sbit32 integer;
        class extra_descr_data *extraptr;
        char *string;
        class cStringInstance *pHash;
        class cNamelist *namelist;
        class cintlist *intlist;
    }
    val;
};

/* allocation strategy */
#define DILA_NONE	0	/* not malloc (int) */
#define DILA_NORM	1	/* normal malloc */
#define DILA_EXP	2	/* temp. expression malloc */

/* DIL evaluation result. */
class dilval
{
public:
    dilval (void);
    ~dilval (void);

    ubit8 type;			/* result type     */
    ubit8 atyp;			/* allocation type */
    union
    {
        void *ptr;			/* result pointer  */
        sbit32 num;			/* result integer  */
    }
    val;
    void *ref;			/* result reference (NULL=Rexpr) */
};

/* structure for securing unit pointers */
struct dilsecure
{
    class unit_data *sup;		/* A direct reference to the variabel! */
    ubit8 *lab;			/* address to jump to, NULL=foreach */
};

/*
 *  An external reference.
 *  For each external reference, the name and cooresponding
 *  argument and return types will be saved
 */
struct dilxref
{
    char *name;			/* func/proc name [@ zone] */
    ubit8 rtnt;			/* return type */
    ubit8 argc;			/* number of arguments (min 1) */
    ubit8 *argt;			/* argument types */
};

/*
 *  A DIL template for registering DIL programs/functions/procedures.
 *  Inline code is registered as local instances.
 *  Uppon loading old dil programs, an unlinked template is created.
 */
struct diltemplate
{
    char *prgname;		/* program name @ zone */
    class zone_type *zone;	/* Pointer to owner of structure    */

    ubit32 flags;			/* recall, etc. */
    ubit8  priority;
    ubit16 intrcount;		/* max number of interrupts */
    ubit16 varcrc;		/* variable crc from compiler */
    ubit16 corecrc;		/* core crc from compiler */
    ubit8 rtnt;			/* return type */
    ubit8 argc;			/* number of arguments */
    ubit8 *argt;			/* argument types */

    ubit32 coresz;		/* size of coreblock */
    ubit8 *core;			/* instructions, expressions and statics */

    ubit16 varc;			/* number of variables */
    ubit8 *vart;			/* variable types */

    ubit16 xrefcount;		/* number of external references   */
    struct diltemplate **extprg;	/* external programs (SERVER only) */
    struct dilxref *xrefs;	/* external references (VMC only)  */

    ubit32 nInstructions;		/* Number of instructions          */
    ubit32 nTriggers;	        /* Number of triggers of the DIL   */
    double fCPU;	            /* CPU usage (miliseconds)         */

    struct diltemplate *next;	/* for zone templates              */
};

struct dilintr
{
    ubit16 flags;			/* what message types to react on 0=off */
    ubit8 *lab;			/* where to perform check */
    ubit8 *elab;			/* where the end of the check evals to*/
};

/*
 *  A stack frame for a DIL call of function or procedure.
 *  The frame contains runtime values for proc/func execution,
 *  including the return variable number for calling proc/func.
 *  Uppon call, a new stackframe is created from the called
 *  template. The needed memory is allocated in one chunk.
 */
struct dilframe
{
    struct diltemplate *tmpl;	/* current template */
    struct dilvar *vars;		/* variables */

    ubit8 *pc;			/* program counter */

    ubit16 securecount;		/* number of secures (not saved) */
    struct dilsecure *secure;	/* secured vars (not saved) */

    ubit16 intrcount;		/* number of interrupts */
    struct dilintr *intr;		/* interrupts */
    int stacklen;
};

/*
 *   A dil process.
 *   The program is the runtime values of the process execution.
 */

#define DIL_FRAMEINC    8	/* # of stackframes to inc stack with */

class dilprg
{
public:
    dilprg(class unit_data *owner, int bLink);
    ~dilprg(void);

    ubit32 flags;       // Recall, copy, etc.
    ubit16 varcrc;		// variable crc from compiler (saved)
    ubit16 corecrc;		// core crc from compiler (saved)
    ubit16 nest;        // How many levels is the call nested 

    ubit16 framesz;		/* stack size */
    struct dilframe *fp;		/* stack and pointer */
    struct dilframe *frame;	/* stack frames, #0 saved */

    t_array < dilval > stack;

    struct spec_arg *sarg;
    class unit_data *owner;

    sbit16 waitcmd;		/* Command countdown */

    class dilprg *next;		/* For global dilprg list (sendtoalldil) */

    int canfree(void);
};

extern class dilprg *dil_list;
extern class dilprg *dil_list_nextdude;
extern int g_nDilPrg;
extern int g_nDilVal;

#endif /* _MUD_DIL_H */
