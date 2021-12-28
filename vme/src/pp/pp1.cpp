#define MAIN /* This is the main() module */

#include "pp.h"

#ifdef __TURBOC__
extern unsigned _stklen = 32767;
#endif /* __TURBOC__ */

/************************************************************************/
/*									*/
/*	main								*/
/*									*/
/*	The main program.						*/
/*									*/
/************************************************************************/

#ifdef MSC_OPT
    #pragma optimize("e", off) /* Disable global reg optimizing */
    #pragma optimize("g", off) /* Disable global common subs */
    #pragma optimize("l", off) /* Disable loop optimizations */
#endif                         /* MSC_OPT */
int pp_main(const char *filename)
{
    //	static	char		*one_string = "1";

    register int t;                         /* General holder for token	*/
    register struct symtab *p = 0, *p1 = 0; /* Ptr into symbol table	*/
    register struct ppdir *sp;              /* Ptr to predefined symbol	*/
    // MS2020 int			ifile;
    // MS2020 int			ofile;
    int i;

    init(); /* Initialize preprocessor	*/

    if (g_iInit == 0)
    {
        g_Ipath[g_Ipcnt++] = (char *)malloc(1000);
        strcpy(g_Ipath[g_Ipcnt - 1], ".");
    }
    // MS2020 ifile	= FALSE;		/* No input file specified	*/
    // MS2020 ofile	= TRUE;		/* No output file specified	*/
    if (!inc_open(filename)) /* Open input file */
    {
        fatal("Unable to open input file");
    }
    // MS2020 ifile = TRUE;	/* Got an input file */

    g_Output = stdout;
    g_Nextch = g_A_trigraph ? trigraph : gchbuf; /* Next char source */

    g_Do_name = TRUE; /* Force name output on #line */
    if (g_iInit == 0)
    {
        init_path();             /* Initialize search path */
        g_Ipath[g_Ipcnt] = NULL; /* Terminate last include path */
    }
    for (g_Lastnl = TRUE, t = gettoken(GT_STR); t != EOF; t = gettoken(GT_STR))
    {
        if ((g_Ifstate != IFTRUE) && (t != '\n') && istype(t, C_W))
        {
        }
        else if (g_Lastnl && (t == DIRECTIVE_CHAR))
        {
            t = getnstoken(GT_STR);
            if (t == LETTER)
            {
                if ((sp = predef(g_Token, g_pptab)) != NULL)
                {
                    /*
                     *	If unconditionally do it or if emitting code...
                     */
                    if (sp->pp_ifif || (g_Ifstate == IFTRUE))
                    {
                        /* Do #func */ (void)(*(sp->pp_func))(sp->pp_arg, 0, 0);
                    }
                }
                else if (g_Ifstate == IFTRUE)
                    non_fatal("Illegal directive", "");

                scaneol(); /* Suck till EOL ('\n' next) */
            }
            else if (t != '\n')
            {
                non_fatal("Bad directive", "");
                scaneol();
            }
            else
                pushback('\n'); /* Leave for fetch to get */
        }
        else if ((t != EOF) && (g_Ifstate == IFTRUE))
        {
#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
            if (t == LETTER && g_Macexpand)
#else  /* !((TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX)) */
            if (t == LETTER)
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */
            {
                if ((p = lookup(g_Token, NULL)) != NULL)
                {
                    /* Call macro */ (void)docall(p, NULL, NULL);
                }
                else
                    /* Just output token if nothing */ puttoken(g_Token);
            }
            else
            {
                puttoken(g_Token);
                if (t == '\n')
                    g_Lastnl = TRUE; /* Turn on if '\n' */
                else if (!istype(t, C_W))
                    g_Lastnl = FALSE; /* Turn off if !ws */
            }
        }
        else
        {
            while ((t != '\n') && (t != EOF))
            {
                /* Absorb to EOL if False #ifxx */
                t = gettoken(GT_STR);
            }
            g_Lastnl = TRUE;
        }
    }

    if (g_Iflevel != 0)
    {
        /* Unterminated #if */
        non_fatal("Unterminated conditional", "");
    }

    for (i = 0; i < NUMBUCKETS; i++)
    {
        for (p = g_Macros[i], (p && (p1 = p->s_link)); p; p && (p1 = p->s_link))
        {
            if (p->s_body != NULL)
                free(p->s_body);  /* Free macro body */
            unparam(p->s_params); /* Free parameter cells */
            if (p)
                free((char *)p); /* Free the symbol table entry */
            p = 0;
            p = p1; /* Remove item from list */
            g_Macros[i] = p1;
            g_Nsyms--; /* One less symbol */
        }
    }
    if ((g_Output != stdout) && (fclose(g_Output) == EOF))
        fatal("Unable to close output file: ", g_Outfile);
    if (g_pponly)
    {
        if (g_Errors > 0)
        {
            fprintf(stderr, "Terminating with error count #%d\n", g_Errors);
            exit(1);
        }

        fwrite(g_sOutput, g_sOutput_len, 1, stdout);
        exit(g_Errors);
    }
    g_iInit = 1;

    return (g_Eflag ? 0 : g_Errors);
}

/************************************************************************/
/*									*/
/*	getnext								*/
/*									*/
/*	Get next parameter from parameter line and return its address.	*/
/*	If the character following the current parameter (cp) is non-	*/
/*	null, then return that parameter, otherwise look ahead in the	*/
/*	argv list to fetch the next one.  Give a usage() error if the	*/
/*	parameter list is empty or if the fetched parameter's first	*/
/*	character is a '-' and the <swvalid> parameter is NO.		*/
/*									*/
/************************************************************************/
char *getnext(char *cp, int *argc, char ***argv, int swvalid)
{
    if (*++cp == '\0')
    {
        if (*argc != 0)
        {
            /* Parameters remain -- use next one */
            --*argc;       /* Count it down */
            cp = *++*argv; /* Return its address */
        }
        else
            usage(TRUE); /* Otherwise give usage error */
    }

    if (!swvalid && (*cp == '-'))
        usage(TRUE); /* Complain if switch starts w/'-' */

    return (cp);
}

/************************************************************************/
/*									*/
/*	init								*/
/*									*/
/*	Initialize the preprocessor.					*/
/*									*/
/************************************************************************/

void init()
{
    static const char *one_string = "1";
    if (g_sOutput)
        free(g_sOutput);
    g_sOutput = 0;
    g_sOutput_len = 0;
    const char *fromptr;
    int i;
#if (HOST != H_CPM) AND(HOST != H_MPW)
    time_t long_time; /* time_t def'd in "time.h" */
#endif                /* (HOST != H_CPM) AND (HOST != H_MPW) */
    char str[TOKENSIZE + 1];
    char *toptr;

#if PPDEBUG
    PPDEBUG = FALSE;
#endif /* PPDEBUG */

    g_Verbose = FALSE; /* Set verbose state */

    g_Eflag = FALSE;      /* Say to abort on errors */
    g_Lineopt = LINE_EXP; /* Default to "long" #line form */

#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
    g_Do_asm = FALSE;    /* Not inside #pragma asm/endasm */
    g_Macexpand = TRUE;  /* Macro expansion enabled */
    g_Asmexpand = FALSE; /* Disabled inside asm/endasm */
#endif                   /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */

    g_Outline = 1; /* Line number of next output line */

    g_Filelevel = -1; /* Current file level */
    if (g_iInit == 0)
        g_Pbbuf = g_Pbbufp = (struct pbbuf *)malloc(sizeof(struct pbbuf) * PUSHBACKSIZE);

    if (g_Pbbufp == NULL)
        out_of_memory();

    g_Pbbufp->pb_type = PB_TOS; /* Top of stack marker */

    g_A_astring =             /* Replace args within strings	*/
        g_A_crecurse =        /* No recursive comments	*/
        g_A_rescan =          /* No macro gen'd directives	*/
        g_A_trigraph = FALSE; /* No Trigraph translation	*/

    g_Nsyms =         /* Number of symbols generated	*/
        g_Errors =    /* Zero the error counter	*/
        g_Iflevel =   /* #if stack pointer		*/
        g_Unique = 0; /* Zero unique # counter	*/

    g_A_stack = TRUE;                          /* No stacking of macro def's	*/
    g_A_eolcomment = TRUE;                     /* No eol comments		*/
    g_A_outstr = TRUE;                         /* No eol comments		*/
    g_Ifstack[0].i_state = g_Ifstate = IFTRUE; /* Currently TRUE #ifxxx assumed */

#if HOST == H_CPM
    g_Orig_user = bdos1(BDOS_USER, 0xFF); /* Save user and disk */
    g_Orig_disk = bdos1(BDOS_GETDISK, 0);
#endif /* HOST == H_CPM */

    /*
     *	Initialize the Time and Date variables to hold the ANSI strings that
     *	each will print out in response to __TIME__ and __DATE__ respectively.
     */
#if (HOST == H_CPM) OR(HOST == H_MPW)
    strcpy(g_Time, "HH:MM:SS"); /* Fake a time */
    strcpy(Date, "Mmm DD YYYY");
#else  /* !((HOST == H_CPM) OR (HOST == H_MPW)) */
    (void)time(&long_time);                          /* Seconds since whenever... */
    memcpy(str, asctime(localtime(&long_time)), 26); /* Get time/date */

    memcpy(g_Time, &str[11], 8); /* Pull time portion out of string */
    g_Time[8] = '\0';

    memcpy(g_Date, &str[4], 7);      /* Pull month and day out of string */
    memcpy(&g_Date[7], &str[20], 4); /* Pull year out of string */
    g_Date[11] = '\0';
#endif /* (HOST == H_CPM) OR (HOST == H_MPW) */

    /************************************/
    /* Define the automatic definitions */
    /************************************/

#if TARGET == T_UNIX
    sbind("unix", one_string, NO_PARAMS); /* #define unix 1 */
#endif                                    /* TARGET == T_UNIX */

#if TARGET == T_BSD
    sbind("unix", one_string, NO_PARAMS); /* #define unix 1 */
    sbind("BSD", one_string, NO_PARAMS);  /* #define BSD 1 */
#endif                                    /* TARGET == T_BSD */

#if TARGET == T_VMS
    sbind("VMS", one_string, NO_PARAMS); /* #define VMS 1 */
#endif                                   /* TARGET == T_VMS */

#if (TARGET == T_QC) OR(TARGET == T_QCX)
    sbind("CPM", one_string, NO_PARAMS); /* #define CPM 1 */
    sbind("QC", one_string, NO_PARAMS);  /* #define QC 1 */
    /* Generate asm() macro */
    sbind("asm", ";\n#pragma asm\n_PARAM_\n#pragma endasm\n", makeparam("_PARAM_", PF_RQUOTES));
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) */

#if TARGET == T_TCX
    sbind("TC", one_string, NO_PARAMS); /* #define TC 1 */
    /* Generate asm() macro */
    sbind("asm", ";\n#pragma asm\n_PARAM_\n#pragma endasm\n", makeparam("_PARAM_", PF_RQUOTES));
#endif /* TARGET == T_TCX */

    /*	Now bind the automatic line/file generators, etc.	*/

    str[1] = '\0';
    str[0] = (char)LINE_TOKEN;
    sbind("__LINE__", str, NO_PARAMS);
    str[0] = (char)FILE_TOKEN;
    sbind("__FILE__", str, NO_PARAMS);
    str[0] = (char)TIME_TOKEN;
    sbind("__TIME__", str, NO_PARAMS);
    str[0] = (char)DATE_TOKEN;
    sbind("__DATE__", str, NO_PARAMS);
    str[0] = (char)NOW_TOKEN;
    sbind("__NOW__", str, NO_PARAMS);
    str[0] = (char)NEXT_TOKEN;
    sbind("__NEXT__", str, NO_PARAMS);
    str[0] = (char)PREV_TOKEN;
    sbind("__PREV__", str, NO_PARAMS);

    /*	Bind macro symbols for the configuration setting variables	*/

    for (i = 0; g_pragtab[i].pp_name != NULL; i++)
    {
        if ((void *)g_pragtab[i].pp_func == (void *)pragopt)
        {
            strcpy(str, "__");
            for (toptr = &str[2], fromptr = g_pragtab[i].pp_name; *fromptr; fromptr++)
            {
                *toptr++ = (islower(*fromptr) ? toupper(*fromptr) : *fromptr);
            }
            *toptr = '\0';
            strcat(str, "__");
            sbind(str, "0", NO_PARAMS);
        }
    }
}

/************************************************************************/
/*									*/
/*	usage								*/
/*									*/
/*	Print usage information and exit with argument passed.		*/
/*									*/
/************************************************************************/

void usage(int v)
{
    printf(
#if PPDEBUG
        "Usage: pp <input> -[cdeilostuvz?]\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
#else  /* !PPDEBUG */
        "Usage: pp <input> -[cdeilotuv?]\n%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
#endif /* PPDEBUG */
        "   -c 0|1|2|3|4|5|6 Enable the desired configuration option:\n",
        "              0 -> Replace macro arguments in strings.\n",
        "              1 -> Expand macros inside #pragma asm.\n",
        "              2 -> Allow recursive comments.\n",
        "              3 -> Rescan macro expansions for directives.\n",
        "              4 -> Allow macro defs and undefs to stack/unstack.\n",
        "              5 -> Perform Trigraph input character translation.\n",
        "              6 -> Permit C++ style \"//\" eol comments.\n",
        "   -d s[=v] Define symbol <s> to have value <v> (default 1).\n",
        "   -e       Don't abort on error.\n",
        "   -i       Set path for #include files.\n",
        "   -l a|l|n Specify #line output mode (abbrev/long/none).\n",
        "   -o file  Specify output file name.\n",
#if PPDEBUG
        "   -s       Generate statistics summary at end.\n",
#endif /* PPDEBUG */
        "   -t str   Add/remove LETTER chars (Accc or Rccc).\n",
        "   -u s     Undefine an initial symbol.\n",
        "   -v       Verbose mode toggle.\n",
#if PPDEBUG
        "   -z       Output PPDEBUG messages.\n",
#endif /* PPDEBUG */
        "   -?       Output this message.\n",
        " Output file, if not specified, is <input>.pp\n");

    exit(v);
}

void output_adds(char *s)
{
    char *p;

    g_sOutput_len = g_sOutput_len + strlen(s);
    if (g_sOutput_mlen < (g_sOutput_len + 5))
    {
        if ((g_sOutput_len + 1) < (g_sOutput_mlen + 32768))
        {
            g_sOutput = (char *)realloc(g_sOutput, (g_sOutput_mlen + 32768));
            g_sOutput_mlen = g_sOutput_mlen + 32768;
        }
        else
        {
            g_sOutput = (char *)realloc(g_sOutput, (g_sOutput_len + 32768));
            g_sOutput_mlen = g_sOutput_len + 32768;
        }
    }
    p = g_sOutput + (g_sOutput_len - strlen(s));
    strcpy(p, s);
}

void output_addc(int s)
{
    char *p;

    g_sOutput_len = g_sOutput_len + 1;
    if (g_sOutput_mlen < (g_sOutput_len + 5))
    {
        if ((g_sOutput_len + 1) < (g_sOutput_mlen + 32768))
        {
            g_sOutput = (char *)realloc(g_sOutput, (g_sOutput_mlen + 32768));
            g_sOutput_mlen = g_sOutput_mlen + 32768;
        }
        else
        {
            g_sOutput = (char *)realloc(g_sOutput, (g_sOutput_len + 32768));
            g_sOutput_mlen = g_sOutput_len + 32768;
        }
    }
    p = g_sOutput + (g_sOutput_len - 1);
    *p = s;
    *++p = '\0';
}
