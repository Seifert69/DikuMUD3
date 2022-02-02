#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

#include "../compile_defines.h"
#include "pp.h"

#include <cstdio>

//
// MS copied in from the original project. But doesn't seem to work...
//

int original_main(int argc, char *argv[])
{
    static const char *one_string = "1";
#if C99_WCHAR
    static const char *iso_date = "200301L";
#endif /* C99_WCHAR */
    static const char *iso_version = "199901L";
#if EMBEDDED_EXT
    static const char *zero_string = "0";
#endif /* EMBEDDED_EXT */

    int t;            /* General holder for token	*/
    struct symtab *p; /* Ptr into symbol table	*/
    struct ppdir *sp; /* Ptr to predefined symbol	*/
    int ifile;
    int ofile;
    char *s;
    const char *s2;
    int i = 0;
#if DEBUG
    int n;
#endif        /* DEBUG */
    int skip; /* Boolean for option loop	*/

    init(); /* Initialize preprocessor	*/

    ifile =            /* No input file specified	*/
        ofile = FALSE; /* No output file specified	*/

    while (--argc != 0)
    {
        s = *++argv;
        if (s[0] == '-')
        {
            skip = FALSE;
            while ((!skip) && (*++s != '\0'))
            {
                switch ((int)*s)
                {
                    /* -[c 0|1|2|3|4|5|6|8|9] */
                    case 'C':
                    case 'c':
                        s2 = getnext(s, &argc, &argv, NO);
                        switch ((int)*s2)
                        {
                            case '0':
                                s2 = "arg_string";
                                break;

                            case '1':
                                s2 = "asm_expand";
                                break;

                            case '2':
                                s2 = "comment_recurse";
                                break;

                            case '3':
                                s2 = "macro_rescan";
                                break;

                            case '4':
                                s2 = "macro_stack";
                                break;

                            case '5':
                                s2 = "trigraph";
                                break;

                            case '6':
                                s2 = "eol_comment";
                                break;

                            case '8':
                                sbind("__STDC__", one_string, NO_PARAMS);
                                s2 = "trigraph";
                                break;

                            case '9':
                                A_c99 = TRUE;
                                sbind("__STDC__", one_string, NO_PARAMS);
#if EMBEDDED_EXT
                                sbind("__STDC_HOSTED__", zero_string, NO_PARAMS);
#else  /* !EMBEDDED_EXT */
                                sbind("__STDC_HOSTED__", one_string, NO_PARAMS);
#endif /* EMBEDDED_EXT */
                                sbind("__STDC_VERSION__", iso_version, NO_PARAMS);
#if C99_FP
                                sbind("__STDC_IEC_559__", one_string, NO_PARAMS);
#endif /* C99_FP */
#if C99_COMPLEX
                                sbind("__STDC_IEC_559_COMPLEX__", one_string, NO_PARAMS);
#endif /* C99_COMPLEX */
#if C99_WCHAR
                                sbind("__STDC_ISO_10646__", iso_date, NO_PARAMS);
#endif /* C99_WCHAR */
                                pragopt(EMPTY, FALSE, "trigraph");
                                s2 = "eol_comment";
                                break;

                            default:
                                usage(TRUE);
                        }
                        pragopt(EMPTY, FALSE, s2);
                        skip = TRUE;
                        break;

                    /* -[d symbol [= value]] */
                    case 'D':
                    case 'd':
                        s = getnext(s, &argc, &argv, NO);
                        s2 = strchr(s, '='); /* Location of val */
                        if (s2)
                        {
                            // MS2020 Hack bad code.
                            *((char *)s2) = '\0';
                            s2++;
                        }
                        else
                            s2 = one_string; /* Default */

                        if (lookup(s, NULL) != NULL)
                            warning("Symbol already defined: ", s);
                        else
                            sbind(s, s2, NO_PARAMS);

                        skip = TRUE; /* Skip to next param */
                        break;

                    /* -[e] don't abort on errors */
                    case 'E':
                    case 'e':
                        Eflag = TRUE;
                        break;

                    /* -iI <#include search path> */
                    case 'I':
                    case 'i':
                        if (Ipcnt > NIPATHS)
                            fatal("Too many pathnames", "");
                        Ipath[Ipcnt++] = getnext(s, &argc, &argv, NO);
                        skip = TRUE; /* Skip to next param */
                        break;

                    /* -[l a|l|n] Spec #line output mode */
                    case 'L':
                    case 'l':
                        s2 = getnext(s, &argc, &argv, NO);
                        switch ((int)*s2)
                        {
                            case 'A':
                            case 'a':
                                Lineopt = LINE_ABR;
                                break;

                            case 'L':
                            case 'l':
                                Lineopt = LINE_EXP;
                                break;

                            case 'N':
                            case 'n':
                                Lineopt = FALSE;
                                break;

                            default:
                                usage(TRUE);
                        }
                        skip = TRUE; /* Skip to next param */
                        break;

                    /* -[o file] spec output file name */
                    case 'O':
                    case 'o':
                        s2 = getnext(s, &argc, &argv, NO);
                        strcpy(Outfile, s2); /* Copy filename */
                        ofile = TRUE;
                        skip = TRUE; /* Skip to next param */
                        break;

#if DEBUG
                    /* -[s] give statistics at end */
                    case 'S':
                    case 's':
                        Stats = Verbose = TRUE; /* Implies Verbose */
                        break;
#endif /* DEBUG */
                        /* -[t Astr|Rstr] Add or delete chars from LETTER class */
                    case 'T':
                    case 't':
                        s2 = getnext(s, &argc, &argv, NO);
                        switch ((int)*s2++)
                        {
                            case 'a':
                            case 'A':
                                i = TRUE;
                                break;

                            case 'r':
                            case 'R':
                                i = FALSE;
                                break;

                            default:
                                usage(TRUE);
                        }

                        for (; *s2 != '\0'; s2++)
                        {
                            if (i)
                                typetab[*s2 + 1] |= C_L;
                            else
                                typetab[*s2 + 1] &= ~C_L;
                        }

                        skip = TRUE; /* Skip to next param */
                        break;

                    /* -[u symbol] */
                    case 'U':
                    case 'u':
                        s = getnext(s, &argc, &argv, NO);

                        if (lookup(s, NULL) == NULL)
                            warning("Symbol not defined: ", s);
                        else
                            unsbind(s);
                        skip = TRUE; /* Skip to next param */
                        break;

                    /* -[v] verbose mode toggle */
                    case 'V':
                    case 'v':
                        Verbose = !Verbose;
                        break;

#if DEBUG
                    /* -[z] enable debug */
                    case 'Z':
                    case 'z':
                        Debug = TRUE;
                        printf("Debug is on\n");
                        break;
#endif /* DEBUG */
                    case '?':
                        usage(FALSE); /* Give usage info and quit */

                    default:
                        fprintf(STDERR, "FATAL: Bad option: %s\n", s);
                        usage(TRUE);
                }
            }
        }
        else if (!ifile)
        {
            /* Try to get input file */
#if HOST == H_CPM
            if (!inc_open(s, -1, 0)) /* Open file here */
#else                                /* !H_CPM */
            if (!inc_open(s)) /* Open input file */
#endif                               /* H_CPM */
            {
                fatal("Unable to open input file: ", s);
            }
            ifile = TRUE; /* Got an input file */
        }
        else
        {
            /* Too many file names given */
            usage(TRUE);
        }
    } // end while

    //
    // MS2020
    // Here we should be done parsing input parameters
    // I presume the next bit of code is pre-processing the file
    //

    Nextch = gchbuf; /* Next char source */

    if (!ifile)
    {
        /* Must have at least an input file name */
        usage(TRUE);
    }

    if (!ofile)
    {
        /* No output name given; use input name and modify it */
        strcpy(Outfile, Filestack[0]->f_name);
        /* terminate the file name before any extension */
        if ((s = strrchr(Outfile, '.')) != NULL)
            *s = '\0';
        strcat(Outfile, ".pp");
    }

    if (strcmp(Outfile, Filestack[0]->f_name) == EQUAL)
        fatal("Input and output filenames are the same: ", Outfile);
    else if ((Output = fopen(Outfile, "w")) == NULL)
        fatal("Unable to create output file: ", Outfile);

#if HOST == H_CPM
    /* Create a bigger than average buffer */
    if ((s = malloc(OUTBUFSIZE)) == NULL)
        out_of_memory();
    setbuf(Output, s);
    setbsize(Output, OUTBUFSIZE);
#endif /* H_CPM */

    if (Verbose)
    {
        printf("%s%s\n\n", "PP Preprocessor, ", PP_VERSION);
        printf("Output will be on <%s>\n", Outfile);
        printf("*** Read    %s\n", Filestack[Filelevel]->f_name);
    }

    Do_name = TRUE; /* Force name output on #line */

    init_path();         /* Initialize search path */
    Ipath[Ipcnt] = NULL; /* Terminate last include path */
                         /*
                          *	If we are in C99 mode then we need to prepend a definition for the
                          *	"_pragma" macro definition.  We also need to enable \uxxxx and
                          *	\Uxxxxxxxx "universal characters".
                          */
    if (A_c99)
    {
        pbstr("#define _Pragma([line,RQ]) #pragma line\n");
        typetab['\\' + 1] |= C_L;
    }
    /*
     *	Time to start processing the input file(s)!
     */

    Lastnl = TRUE; /* We are at the file beginning */
    while ((t = gettoken(GT_STR)) != EOF)
    {
        if ((t == DIRECTIVE_CHAR) && Lastnl)
        {
            t = getnstoken(GT_STR);
            if (t == LETTER)
            {
                if ((sp = predef(Token, pptab)) != NULL)
                {
                    /*
                     *	If unconditionally do it or if emitting code...
                     */
                    if (sp->pp_ifif || (Ifstate == IFTRUE))
                    {
                        /* Do #func */ (void)(*(sp->pp_func))(sp->pp_arg, 0, ""); // MS2020 add missing params??? Totally odd.
                    }
                }
                else if (Ifstate == IFTRUE)
                    non_fatal("Illegal directive", "");
            }
            else if (t != '\n')
                non_fatal("Bad directive", "");
            /*
             *	After processing directive toss the remainder of the input
             *	line and just output the linefeed.
             */
            while ((t != '\n') && (t != EOF))
                t = gettoken(GT_STR);
            if (t == '\n')
                puttoken(Token);
            Lastnl = TRUE;
        }
        else if (Ifstate == IFTRUE)
        {
/*
 *	We need to process this line.
 */
#if EMBEDDED_EXT
            if (t == LETTER && Macexpand)
#else  /* !EMBEDDED_EXT */
            if (t == LETTER)
#endif /* EMBEDDED_EXT */
            {
                if ((p = lookup(Token, NULL)) != NULL)
                    /* Call macro */ (void)docall(p, NULL, NULL);
                else
                {
                    /* Not a macro, just output */ puttoken(Token);
                    Lastnl = FALSE;
                }
            }
            else
            {
                puttoken(Token);
                if (t == '\n')
                    Lastnl = TRUE; /* Turn on if '\n' */
                else if (t != SPACE)
                    Lastnl = FALSE; /* Turn off if !ws */
            }
        }
        else
        /*
         *	Ifstate is FALSE, so if it is anything but whitespace, toss it.
         *	We need to allow whitespace since it might be leading a #else or
         *	some such on a line that will be processed the next time around.
         */
        {
            if (t != SPACE) /* Generic whitespace? */
            {
                while ((t != '\n') && (t != EOF))
                    t = gettoken(GT_STR);
                Lastnl = TRUE;
            }
        }
    } // end while

    //
    // Done pre processing the file
    //

    if (Iflevel != 0)
    {
        /* Unterminated #if */
        non_fatal("Unterminated conditional", "");
    }

    if (Verbose)
    {
        printf("\nActive symbols at end:\t%d\tMaximum symbols used:\t%d\n", Nsyms, Maxsyms);
#if HOST == H_CPM
        printf("Free memory: %5u\n", maxsbrk());
#endif /* H_CPM */
    }

    if (Verbose || Errors)
    {
        if (Errors)
            printf("\n%d errors detected\n", Errors);
        else
            printf("\nNo errors detected\n");
    }

#if DEBUG
    if (Stats)
    {
        printf("\nSymbol table bucket statistics:");
        for (i = 0; i < NUMBUCKETS; i++)
        {
            if ((i % 8) == 0)
                printf("\n"); /* New line */
            for (n = 0, p = Macros[i]; p != 0; p = p->s_link, n++)
                ; /* Count items in list */
            printf("  %3d:%-3d", i, n);
        }
        printf("\n\n");
    }
#endif /* DEBUG */

    // MS2020 looks like an error that the output is not written
    // I added this to write the output, copied over from the custom
    // ppmain() by the Perry's
    //

    if (Errors > 0)
    {
        fprintf(stderr, "Terminating with error count #%d\n", Errors);
        exit(1);
    }

    fwrite(sOutput, sOutput_len, 1, stdout);

    //
    // Before it just closed the output file

    if ((Output != stdout) && (fclose(Output) == EOF))
        fatal("Unable to close output file: ", Outfile);

    exit(Eflag ? 0 : Errors);
}

#ifdef MSC_OPT
    #pragma optimize("e", on) /* Enable global reg optimizing */
    #pragma optimize("g", on) /* Enable global common subs */
    #pragma optimize("l", on) /* Enable loop optimizing */
#endif                        /* MSC_OPT */

int main(int argc, char *argv[])
{
    fprintf(stderr, "PP Compiled with [%s]\n", get_compiled_hash_defines().c_str());
    original_main(argc, argv);
    return 1;

    pponly = 1;

    char path[200];

    if (argc < 2)
    {
        fprintf(stderr, "please specify a filename to input as the first parameter.\n");
        return 1;
    }

    strcpy(path, "..");
    Ipath[Ipcnt++] = path;
    pp_main(argv[1]);
}

#pragma GCC diagnostic pop
