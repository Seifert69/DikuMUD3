#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

#include <ctype.h> /* Char type info			*/
#include <stdio.h>
#include <stdio.h>  /* Standard I/O info			*/
#include <stdlib.h> /* Standard library info		*/
#include <string.h>
#include <time.h>

#ifndef EVALINT
    #define EVALINT long /* Default values in eval are longs	*/
#endif                   /* EVALINT */

#define BUFFERSIZE 512
/*
 *	pp1.c
 */
int pp_main(const char *filename);
char *getnext(char *cp, int *argc, char ***argv, int swvalid);
void init();
void usage(int v);
void output_adds(char *s);
void output_addc(int s);

/*
 *	pp2.c
 */
char *docall(struct symtab *p, char *internal, char *internal_limit);
char *_docall(char *line, char *internal, char *internal_limit);
void dodefine(int mactype, int izxy = 0, const char *izxz = 0);
void doerror(int izxx = 0, int izxy = 0, const char *izxz = 0);
void doundef(int izxx = 0, int izxy = 0, const char *izxz = 0);
char *esc_str(char *old, int c, char *limit);
void fbind(struct symtab **formals, char *name, const char *value);
char *flookup(struct symtab *formals, char *name);
struct param *getparams();
unsigned int pphash(const char *sym);
struct symtab *lookup(char *name, struct symtab **pe);
struct param *makeparam(const char *s, int f);
struct ppdir *predef(char *n, struct ppdir *table);
void sbind(const char *sym, const char *defn, struct param *params);
char *strize(char *result, char *limit, char *msg, char *snew);
void unfbind(struct symtab *formals);
void unparam(struct param *pp);
void unsbind(char *sym);

/*
 *	pp3.c
 */
void cur_user();
void do_line(char at_bol);
void doinclude(int izxx = 0, int izxy = 0, const char *izxz = 0);
void doline(int izxx = 0, int izxy = 0, const char *izxz = 0);
int gchbuf();
int gchfile();
int gchpb();
int getchn();
int inc_open(const char *incfile);
void init_path();
int popfile();
char *readline(char *buf, int bufsize, int flags);
void scaneol();
void set_user();
int trigraph();

/*
 *	pp4.c
 */
char *addstr(char *old, char *limit, const char *msg, const char *snew);
int getnstoken(int f);
int gettoken(int f);
void memmov(char *f, char *t, unsigned l);
void pbcstr(char *s);
void pbstr(const char *in);
void pushback(int c);
void puttoken(const char s[]);
int type(int c);

/*
 *	pp5.c
 */
void doelse(int elif, int izxy = 0, const char *izxz = 0);
void doendif(int izxy, int ixy = 0, const char *izxz = 0);
void doif(int izxy, int ixy = 0, const char *izxz = 0);
void doifs(int t, int izxy = 0, const char *izxz = 0);

/*
 *	pp6.c
 */
void dopragma(int izxy, int izy = 0, const char *izxz = 0);
void pragendm(int izxy = 0, int izy = 0, const char *izxz = 0);
void pragerror(int izxy = 0, int izy = 0, const char *izxz = 0);
void pragmsg(int izxy = 0, int izy = 0, const char *izxz = 0);
void pragopt(int dummy, int no_flag, const char *name);
void pragvalue(int izxy = 0, int izy = 0, const char *izxz = 0);

/*
 *	pp7.c
 */
void end_of_file();
void fatal(const char *s1, const char *s2 = "");
void illegal_symbol();
void non_fatal(const char *s1, const char *s2);
void out_of_memory();
void prmsg(const char *s1, const char *s2, const char *s3);
void warning(const char *s1, const char *s2);

/*
 *	pp8.c
 */
EVALINT eval();
EVALINT evaltern();
EVALINT evallor();
EVALINT evalland();
EVALINT evalbor();
EVALINT evalbxor();
EVALINT evalband();
EVALINT evaleq();
EVALINT evalrel();
EVALINT evalsh();
EVALINT evalsum();
EVALINT evalmdr();
EVALINT evalfuns();
EVALINT evalucom();
EVALINT evalunot();
EVALINT evalumin();
EVALINT evalval();
EVALINT hexbin(char ch);
int ishex(char ch);
int isoct(char ch);
int item(int(fun)(int), int f);
int look(const char *str);
int match(char *tbuf, const char *str);
char *readexpline(char *buf, int bufsize);
int test(const char *str);

#define PP_VERSION "VMC PreProcessor v1.0" /* Version info */
#ifdef MAIN
    #define EXTERN
    #define I_BRZERO = {0}
    #define I_ZERO = 0
#else
    #define EXTERN extern
    #define I_BRZERO
    #define I_ZERO
#endif

/*	Define TRUE and FALSE if not already defined			*/
#ifndef TRUE
    #define TRUE 1
#endif /* TRUE */

#ifndef FALSE
    #define FALSE 0
#endif /* FALSE */

/*
 *	Define AND and OR as logical "and" and "or", or boolean if the
 *	preprocessor being used to compile PP doesn't support "&&" and "||" in
 *	preprocessor directives (at least one version of Lattice for instance).
 */

#define AND &&
#define OR ||

/*
 *	If the value of PP is FALSE, this program may be run through
 *	the original QC compiler without parameterized macro expansion.
 *	After a version is built, recompile with PP as TRUE to make
 *	things run more efficiently (ctype.h, for example.)
 */

#define PP TRUE /* TRUE if to depend upon PP features	*/

#define PPDEBUG FALSE /* TRUE if to include debug code	*/

/*
 *	The host system the preprocessor will be running on.  Note that this
 *	choice implies both the host operating system and the specific "C"
 *	compiler used to compile PP.  Note that not all combinations have
 *	been (recently), tested!
 */

#define H_MSDOS 4 /* MSC or Borland "C" on MS-DOS		*/
#define H_UNIX 5  /* AT+T System 5 UNIX			*/
#define H_XENIX 7 /* Microsoft XENIX			*/

#define HOST H_UNIX

/*
 *	The target system for which the preprocessor is generating code.  Note
 *	that this choice implies both the target "operating system" (if any),
 *	and the	target processor.  In particular, this choice affects which
 *	symbols PP predefines.  Note that not all combinations have been
 *	(recently), tested!
 */

#define T_BSD 1   /* BSD 4.X UNIX				*/
#define T_MPW 2   /* MPW "C" on Mac's			*/
#define T_QC 3    /* QC "C"				*/
#define T_QCX 4   /* QCX "C" cross compiler		*/
#define T_TCX 5   /* TCX "C" cross compiler		*/
#define T_UNIX 6  /* AT+T System 5 UNIX			*/
#define T_VMS 7   /* DEC VAX VMS				*/
#define T_XENIX 8 /* Microsoft XENIX			*/

#define TARGET T_UNIX

#define STDERR stderr

#define VERBOSE FALSE /* TRUE to make verbose by default	*/

#define ENV_PATH "VME_INCLUDE" /* Env. variable for include path */

#define DFLT_PATH "." /* Default include location	*/

#define SLASHCHAR '/'
#define SLASHSTR "/"

#define PATHPUNC ';' /* Include file path separator		*/

#ifdef _WINDOWS
    #define IGNORE_CR /* Defined if to ignore CR's		*/
#endif

/*
 *	Character type.
 */

#define C_L 0x01 /* Letter (A-Zz-z)			*/
#define C_D 0x02 /* Digit (0-9)				*/
#define C_X 0x04 /* Hex digit (0-9 a-f|A-F)		*/
#define C_W 0x08 /* White space (ht, sp, etc.)		*/
/*			0x10	   Not used				*/
#define C_C 0x20 /* Special processing inside gchbuf()	*/
#define C_M 0x40 /* A META token (see gettoken())	*/
#define C_N 0x80 /* A NUL (no output)			*/

#if PP
    #define istype(c, v) ((typetab + 1)[(c)] & (v))
#endif /* PP */

#ifdef MAIN
char typetab[] = {
    (char)(C_C | C_N), /* EOF         */
    0,
    0,
    0,
    0, /* ^@ ^A ^B ^C */
    0,
    0,
    0,
    0, /* ^D ^E ^F ^G */
    0,
    C_W,
    C_C,
    0, /* ^H ^I ^J ^K */
    C_W,
    C_W | C_C,
    0,
    0, /* ^L ^M ^N ^O */
    0,
    0,
    0,
    0, /* ^P ^Q ^R ^S */
    0,
    0,
    0,
    0, /* ^T ^U ^V ^W */
    0,
    0,
    0,
    0, /* ^X ^Y ^Z ESC*/
    0,
    0,
    0,
    0, /* FS GS RS VS */
    C_W,
    0,
    0,
    0, /* Sp !  "  #  */
    0,
    0,
    0,
    0, /* $  %  &  '  */
    0,
    0,
    0,
    0, /* (  )  *  +  */
    0,
    0,
    0,
    0, /* ,  -  .  /  */
    C_D | C_X,
    C_D | C_X,
    C_D | C_X,
    C_D | C_X, /* 0  1  2  3  */
    C_D | C_X,
    C_D | C_X,
    C_D | C_X,
    C_D | C_X, /* 4  5  6  7  */
    C_D | C_X,
    C_D | C_X,
    0,
    0, /* 8  9  :  ;  */
    0,
    0,
    0,
    0, /* <  =  >  ?  */
    0,
    C_L | C_X,
    C_L | C_X,
    C_L | C_X, /* @  A  B  C  */
    C_L | C_X,
    C_L | C_X,
    C_L | C_X,
    C_L, /* D  E  F  G  */
    C_L,
    C_L,
    C_L,
    C_L, /* H  I  J  K  */
    C_L,
    C_L,
    C_L,
    C_L, /* L  M  N  O  */
    C_L,
    C_L,
    C_L,
    C_L, /* P  Q  R  S  */
    C_L,
    C_L,
    C_L,
    C_L, /* T  U  V  W  */
    C_L,
    C_L,
    C_L,
    0, /* X  Y  Z  [  */
    C_C,
    0,
    0,
    C_L, /* \  ]  ^  _  */
    0,
    C_L | C_X,
    C_L | C_X,
    C_L | C_X, /* `  a  b  c  */
    C_L | C_X,
    C_L | C_X,
    C_L | C_X,
    C_L, /* d  e  f  g  */
    C_L,
    C_L,
    C_L,
    C_L, /* h  i  j  k  */
    C_L,
    C_L,
    C_L,
    C_L, /* l  m  n  o  */
    C_L,
    C_L,
    C_L,
    C_L, /* p  q  r  s  */
    C_L,
    C_L,
    C_L,
    C_L, /* t  u  v  w  */
    C_L,
    C_L,
    C_L,
    0, /* x  y  z  {  */
    0,
    0,
    0,
    0, /* |  }  ~  Rub*/
    /* Special token chars 0x80 - 0x??	*/
    (char)(C_M | C_N), /* END_MACRO token */
    (char)(C_M | C_N), /* END_ARG token   */
    (char)(C_M | C_N), /* TOGGLE_EXPAND   */
    (char)(C_M | C_N), /* __LINE__ token  */
    (char)(C_M | C_N), /* __FILE__ token  */
    (char)(C_M | C_N), /* __TIME__ token  */
    (char)(C_M | C_N), /* __DATE__ token  */
    (char)(C_M | C_N), /* __NOW__ token   */
    (char)(C_M | C_N), /* __NEXT__ token  */
    (char)(C_M | C_N)  /* __PREV__ token  */
};
#else
extern char typetab[];
#endif /* MAIN */

#define EQUAL 0 /* Value returned by strcmp if equal	*/

/*
 *	Token types from gettoken()
 */

#define LETTER 'a'         /* (letter | '_') letter* digit*	*/
#define NUMBER '0'         /* (digit | '.' | letter) digit+	*/
#define SPACE ' '          /* Generic whitespace character         */
#define DIRECTIVE_CHAR '#' /* Defines a PP directive		*/

#define END_MACRO 0x80     /* Marks end of macro in pushback	*/
#define TOGGLE_EXPAND 0x81 /* Marks edges of previously expanded	*/
#define END_ARG 0x82       /* Marks end of argument macro scope	*/
#define LINE_TOKEN 0x83    /* Return the Line number		*/
#define FILE_TOKEN 0x84    /* Return the File name			*/
#define TIME_TOKEN 0x85    /* Return the system time		*/
#define DATE_TOKEN 0x86    /* Return the system date		*/
#define NOW_TOKEN 0x87     /* Return the last result of NEXT_TOKEN	*/
#define NEXT_TOKEN 0x88    /* Return a unique unsigned number	*/
#define PREV_TOKEN 0x89    /* Return previous unique unsigned #	*/

/*
 *	Flag parameter to "gettoken()".
 */

#define GT_STR 0x0001   /* gettoken() returns whole strings	*/
#define GT_ANGLE 0x0003 /* GT_STR plus returns whole <>'s	*/

#define FILENAMESIZE 300 /* Long enough until we find better	*/

#define FILESTACKSIZE 14         /* Max depth of #include	*/
#define MACROSIZE 4000           /* Max length of #define	*/
#define TOKENSIZE 65438          /* Max token length		*/
#define NUMBUCKETS 64            /* # of buckets (power of 2)	*/
#define IFSTACKSIZE 32           /* #ifxx nesting		*/
#define PUSHBACKSIZE TOKENSIZE   /* Pushback buffer		*/
#define EVALBUFSIZE TOKENSIZE    /* Buffer size in doif()	*/
#define MESSAGEBUFSIZE TOKENSIZE /* Buffer size in #pragma mess	*/
#define PRAGBUFSIZE TOKENSIZE    /* Room for pragma line		*/

/*
 *	Pushback buffer
 */

EXTERN struct pbbuf *Pbbufp I_ZERO;
EXTERN struct pbbuf *Pbbuf I_ZERO;

struct pbbuf
{
    char pb_type;
#define PB_CHAR 0   /* pb_val.pb_char is character		*/
#define PB_STRING 1 /* pb_val.pb_str is ptr to string 	*/
#define PB_TOS 2    /* Top of pushback buffer stack		*/
    union
    {
        char *pb_str;
        int pb_char;
    } pb_val;
};

EXTERN FILE *Output I_ZERO;     /* Output file				*/
EXTERN char *sOutput I_ZERO;    /* Output file			*/
EXTERN int sOutput_len I_ZERO;  /* Output file			*/
EXTERN int sOutput_mlen I_ZERO; /* Output file			*/
EXTERN int pponly I_ZERO;       /* Output file			*/
EXTERN int iInit I_ZERO;
struct file
{
    int f_line;                    /* Line number				*/
#ifdef PP_SYSIO                    /* If to use direct I/O system calls?	*/
    int f_fd;                      /* A file descriptor			*/
#else                              /* !PP_SYSIO */
    FILE *f_file; /* FILE pointer				*/
#endif                             /* PP_SYSIO */
    char *f_bufp;                  /* Pointer into buffer			*/
    int f_bufc;                    /* Number of chars left			*/
    int f_eof;                     /* Flag that's non-zero if in last blk	*/
    char f_lasteol;                /* TRUE if last char was EOL	*/
    char f_buf[BUFFERSIZE];        /* Buffer for file	*/
    char f_name[FILENAMESIZE + 1]; /* File name	*/
};

/*
 *	Define static vars that are a bit more efficient to access.
 */

EXTERN int Bufc I_ZERO;    /* Current file char count		*/
EXTERN char *Bufp I_ZERO;  /* Current file char ptr 		*/
EXTERN int Lasteol I_ZERO; /* True if last char processed was EOL	*/
EXTERN int LLine I_ZERO;   /* Last line number			*/
EXTERN int (*Nextch)();    /* Next char function           */
#define nextch (*Nextch)   /* Macro to rd chars via Nextch	*/

EXTERN struct file *Filestack[FILESTACKSIZE + 1] I_BRZERO;
EXTERN int Filelevel I_ZERO; /* Include level	*/
EXTERN int Do_name I_ZERO;   /* True to put name on #line	*/

EXTERN char Outfile[FILENAMESIZE + 1] I_BRZERO;
EXTERN char Token[TOKENSIZE + 1] I_BRZERO; /* Token buffer	*/
EXTERN unsigned int Tokenline I_ZERO;      /* Line # token began	*/
EXTERN unsigned int Tokenfile I_ZERO;      /* File # token began	*/
EXTERN int Lastnl I_ZERO;                  /* Last token processed was \n	*/
EXTERN int inquote I_ZERO;
/*
 *	Macro proto pointers.
 */

struct symtab
{
    struct symtab *s_link;  /* Next in list for this chain		*/
    char disable;           /* TRUE to disable recognition for now	*/
    char *s_body;           /* Body of definition			*/
    struct param *s_params; /* List of parameters		*/
    char s_name[1];         /* Name is appended to structure*/
};

struct param
{
    struct param *p_link; /* Next in list				*/
    char p_flags;         /* Flags:				*/
#define PF_RQUOTES 0x01   /* Remove "" chars from parameter	*/
#define PF_PNLINES 0x02   /* Preserve '\n' char in parameter	*/
    char p_name[1];       /* Name is appended to struct	*/
};

/*
 *	Predefined symbol entry.
 */

struct ppdir
{
    const char *pp_name;                     /* #function name		*/
    char pp_ifif;                            /* FALSE if ! to do on false #if*/
    void (*pp_func)(int, int, const char *); /* Address of function		*/
    int pp_arg;                              /* Argument to function		*/
};

/*
 *	Predefined symbols.
 */

#define YES TRUE
#define NO FALSE
#define EMPTY 0 /* Catch-all for non-used field		*/

/*
 *	Alphabetically ordered.
 */

#ifdef MAIN /* If in main() module			*/
struct ppdir pptab[] = {
/*	 Directive	Do within	Procedure	Arg to	  */
/*	   name		FALSE #ifxx	name		function  */
/* --------------	-----------	----------	--------  */
    #if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
    {"asm", NO, doasm, TRUE},
    #endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */
    {"define", NO, dodefine, FALSE},
    {"elif", YES, doelse, TRUE},
    {"else", YES, doelse, FALSE},
    #if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
    {"endasm", NO, doasm, FALSE},
    #endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */
    {"endif", YES, doendif, EMPTY},
    {"error", NO, doerror, EMPTY},
    {"if", YES, doif, EMPTY},
    {"ifdef", YES, doifs, TRUE},
    {"ifndef", YES, doifs, FALSE},
    {"include", NO, doinclude, EMPTY},
    {"line", NO, doline, EMPTY},
    {"undef", NO, doundef, EMPTY},
    {"pragma", YES, dopragma, EMPTY},
    {NULL} /* The end */
};

struct ppdir pragtab[] = {
    /*	 Keyword	Do within	Procedure	Arg to	  */
    /*	   name		FALSE #ifxx	name		function  */
    /* ------------------	-----------	----------	--------  */
    {"arg_string", NO, pragopt, EMPTY},
    {"comment_recurse", NO, pragopt, EMPTY},
    {"define", NO, dodefine, FALSE},
    {"elif", YES, doelse, TRUE},
    {"else", YES, doelse, FALSE},
    {"endif", YES, doendif, EMPTY},
    {"endmacro", NO, pragendm, EMPTY},
    {"error", NO, pragerror, FALSE},
    {"if", YES, doif, EMPTY},
    {"ifdef", YES, doifs, TRUE},
    {"ifndef", YES, doifs, FALSE},
    {"macro", NO, dodefine, TRUE},
    {"macro_rescan", NO, pragopt, EMPTY},
    {"macro_stack", NO, pragopt, EMPTY},
    {"message", NO, pragmsg, EMPTY},
    {"trigraph", NO, pragopt, EMPTY},
    {"undef", NO, doundef, EMPTY},
    {"value", NO, pragvalue, EMPTY},
    {NULL} /* The end */
};

#else  /* !MAIN */
extern struct ppdir pptab[];
extern struct ppdir pragtab[];
#endif /* MAIN */

EXTERN int A_astring I_ZERO;    /* TRUE/args in strings	*/
EXTERN int A_c99 I_ZERO;        /* TRUE/configure for C99       */
EXTERN int A_outstr I_ZERO;     /* output is sOutput	*/
EXTERN int A_crecurse I_ZERO;   /* TRUE/comments nest	*/
EXTERN int A_eolcomment I_ZERO; /* TRUE/eol comments OK	*/
EXTERN int A_rescan I_ZERO;     /* TRUE/direct. from macro's	*/
EXTERN int A_stack I_ZERO;      /* TRUE/macro def's stack	*/
EXTERN int A_trigraph I_ZERO;   /* TRUE/trigraphs active*/

EXTERN struct symtab *Macros[NUMBUCKETS] I_BRZERO; /* Ptr/macro chains*/
EXTERN int Nsyms I_ZERO;                           /* Number of symbols in table	*/
EXTERN int Maxsyms I_ZERO;                         /* Max number of symbols used	*/

#define NO_PARAMS (struct param *)NULL /* For sbind of 0 params*/

struct ifstk
{
    char i_state; /* The ifstack state:			*/
    char i_else;  /* True if encountered an #else		*/
};

#ifdef MAIN
struct ifstk Ifstack[IFSTACKSIZE + 1] = {{0}};
#else             /* !MAIN */
extern struct ifstk Ifstack[IFSTACKSIZE + 1];
#endif            /* MAIN */
#define IFTRUE 0  /* True - include code within #ifxx	*/
#define IFFALSE 1 /* False - no code within #ifxx		*/
#define IFNEVER 2 /* Treat as false forever after		*/

EXTERN int Iflevel I_ZERO; /* Index into Ifstack		*/
EXTERN int Ifstate I_ZERO; /* Current state of #if		*/

#if PPDEBUG
EXTERN int PPDEBUG I_ZERO; /* -z flag			*/
#endif                     /* PPDEBUG */

EXTERN int Lineopt; /* True if producing #line directives	*/
#define LINE_EXP 1  /* If to expand to #line directives	*/
#define LINE_ABR 2  /* If to abbreviate to # n "file"	*/

EXTERN int Outline I_ZERO; /* Line # of current output file*/
EXTERN int Errors I_ZERO;  /* Total errors detected	*/
EXTERN int Eflag I_ZERO;   /* True if to ignore errors	*/
#if PPDEBUG
EXTERN int Stats I_ZERO;           /* True to print stats at end	*/
#endif                             /* PPDEBUG */
EXTERN char Date[12] I_BRZERO;     /* Date str for __DATE__*/
EXTERN char _Time[9] I_BRZERO;     /* Time str for __TIME__*/
EXTERN unsigned int Unique I_ZERO; /* Unique # for __NOW__/__NEXT__*/
EXTERN int Verbose I_ZERO;         /* True to print verbose mess	*/
#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
EXTERN int Do_asm I_ZERO;    /* True if in asm/endasm body	*/
EXTERN int Macexpand I_ZERO; /* True/macro expand on	*/
EXTERN int Asmexpand I_ZERO; /* Set Macexpand in asm	*/
#endif                       /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */

#define NIPATHS 10                        /* Up to 10 -i pathnames		*/
EXTERN char *Ipath[NIPATHS + 1] I_BRZERO; /* -i path list	*/
EXTERN int Ipcnt I_ZERO;                  /* Count of -i parameters	*/

#if HOST == H_CPM
EXTERN int Orig_user I_ZERO; /* Original user	*/
EXTERN int Orig_disk I_ZERO; /* Original disk	*/
#endif                       /* HOST == H_CPM */

/*
 *	End of pp.h
 */

#pragma GCC diagnostic pop
