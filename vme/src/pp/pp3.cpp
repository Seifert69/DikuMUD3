#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

/************************************************************************/
/*									*/
/*	File:	pp3.c							*/
/*									*/
/*	General I/O for PP.						*/
/*									*/
/*	Written by:							*/
/*			Gary Oliver					*/
/*			3420 NW Elmwood Dr.				*/
/*			PO Box 826					*/
/*			Corvallis, Oregon 97339				*/
/*			(503)758-5549					*/
/*	Maintained by:							*/
/*			Kirk Bailey					*/
/*			Logical Systems					*/
/*			P.O. Box 1702					*/
/*			Corvallis, OR 97339				*/
/*			(503)753-9051					*/
/*									*/
/*	This program is hereby placed in the public domain.  In		*/
/*	contrast to other claims of "public domain", this means no	*/
/*	copyright is claimed and you may do anything you like with PP,	*/
/*	including selling it!  As a gesture of courtesy, please retain	*/
/*	the authorship information in the source code and		*/
/*	documentation.							*/
/*									*/
/*	Functions contained in this module:				*/
/*									*/
/*		cur_user	Select current disk-user (CPM).		*/
/*		do_line		Issue #line output directive.		*/
/*		doinclude	Process #include directive.		*/
/*		doline		Process #line directive.		*/
/*		gchbuf		Get char from file buffer.		*/
/*		gchfile		Get new buffer from file and return ch.	*/
/*		gchpb		Get char from pushback buffers.		*/
/*		getchn		Get char from buffer, ignoring "\\n"	*/
/*		inc_open	Open file for inclusion.		*/
/*		init_path	Initialize Ipath information.		*/
/*		popfile		Pop to next lower file level.		*/
/*		readline	Read and macro-process a line of text.	*/
/*		scaneol		Scan to eol, leaving EOL as next token.	*/
/*		set_user	Select a disk-user (CPM).		*/
/*		trigraph	Handle ANSI trigraph translations.	*/
/*									*/
/************************************************************************/

#include "pp.h"

#if HOST == H_CPM
/************************************************************************/
/*									*/
/*	cur_user							*/
/*									*/
/*	Restore disk/user to that at program start.			*/
/*									*/
/************************************************************************/

void cur_user()
{
    bdos(BDOS_SELDISK, Orig_disk);
    bdos(BDOS_USER, Orig_user);
}
#endif /* HOST == H_CPM */

/************************************************************************/
/*									*/
/*	do_line								*/
/*									*/
/*	Write #line directive to output file, synching to current	*/
/*	input line number and file name.				*/
/*									*/
/************************************************************************/

/*
 *	The following definition of NL_CHAR is used to minimize the file
 *	storage space required for the output file PP writes.  If the external
 *	file format stores "\n" as a single character (as on the UNIX systems),
 *	then NL_CHAR is set to 1.  Otherwise it is set to 2 to reflect the CR/
 *	LF used for external files.  NLCHAR is only used by this function to
 *	optimize the tradeoff between emitting extra "#line" directives (if
 *	enabled), or to output extra "\n" characters instead.
 */
#if (HOST == H_BSD) OR(HOST == H_MPW) OR(HOST == H_UNIX) OR(HOST == H_XENIX)
    #define NL_CHAR 1
#else /* H_BSD || H_MPW || H_UNIX || H_XENIX */
    #define NL_CHAR 2
#endif /* H_BSD || H_MPW || H_UNIX || H_XENIX */

void do_line(char at_bol)
{
    char buf[TOKENSIZE + 1];
    char filen[FILENAMESIZE + 1 + 3];
    int n;

    n = Tokenline - Outline; /* Difference in line #s */

    sprintf(filen, " \"%s\"", Filestack[Tokenfile]->f_name);
    sprintf(buf,
            "%s#%s %d%s\n",
            !at_bol ? "\n" : "", /* Emit a \n if needed */
            Lineopt == LINE_EXP ? "line" : "",
#if TARGET == T_QC
            Tokenline - 1, /* QC bug */
#else                      /* TARGET != T_QC */
            Tokenline,
#endif                     /* TARGET == T_QC */
            Do_name ? filen : "");

#if (TARGET == T_QC) OR(TARGET == T_QCX)
    if ((!Do_name && (n >= 0) && (((unsigned int)n) < (strlen(buf) / NL_CHAR + 1))) || Do_asm)
#else  /* ! ((TARGET == T_QC) OR (TARGET == T_QCX)) */
    if (!Do_name && (n >= 0) && (((unsigned int)n) < (strlen(buf) / NL_CHAR + 1)))
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) */
    {
        while (n-- > 0)
            if (A_outstr)
                output_addc('\n');
            else
                putc('\n', Output); /* Write newlines to synch */
    }
    else
    {
        if (A_outstr)
            output_adds(buf);
        else
            fprintf(Output, "%s", buf);
    }

    Outline = Tokenline; /* Make them the same */
    Do_name = FALSE;
}

/************************************************************************/
/*									*/
/*	doinclude							*/
/*									*/
/*	Process #include file commands.					*/
/*									*/
/*	The file name is parsed and the file is opened.  A stack of	*/
/*	current files and file names is pushed, with the new one	*/
/*	being placed upon the top of the other, currently opened,	*/
/*	files.  Upon return, the next character read is from the	*/
/*	newly opened file.						*/
/*									*/
/*									*/
/************************************************************************/

void doinclude(int aaa, int bbb, const char *ccc)
{
    char buf[TOKENSIZE];
    int c;
    int d = 0;
    char *incf;
    char incfile[FILENAMESIZE + 1];
#if HOST != H_CPM
    char filename[FILENAMESIZE + 1];
    char **ip;
#endif /* HOST != H_CPM */
    int ok;
#if HOST == H_CPM
    int disk;
    char *p;
    char **ip;
    int user;
#endif /* HOST == H_CPM */

#if PPDEBUG
    if (PPDEBUG)
        printf("doinclude: include level:%d\n", Filelevel);
#endif /* PPDEBUG */
    if (Filelevel >= FILESTACKSIZE)
    {
        non_fatal("Include file stack overflow", "");
        return;
    }
    /*
     *	Read line and process any macros encountered.
     */
    pbcstr(readline(buf, TOKENSIZE, GT_ANGLE));

    while (istype((c = getchn()), C_W))
        ;

    if (c == EOF)
        end_of_file();
    else if (c == '<')
        d = '>';
    else if (c == '"')
        d = '"';
    else
    {
        non_fatal("Bad include argument", "");
        if (c == '\n')
            pushback('\n');
        return;
    }

    for (incf = incfile; c != EOF && c != '\n';)
    {
        if ((c = getchn()) == d)
            break;

        if (incf >= &incfile[FILENAMESIZE])
        {
            non_fatal("Include file name too long", "");
            return;
        }
        else
            *incf++ = c;
    }

    while ((c != '\n') && (c != EOF))
        c = getchn(); /* Slurp trailing \n */

    if (incf != incfile)
        *incf = '\0';
    else
    {
        non_fatal("Illegal file name", "");
        return;
    }

    if (Lineopt)
        do_line(TRUE); /* Catch up before inc file switch */

#if PPDEBUG
    if (PPDEBUG)
        printf("doinclude: <%s>\n", incfile);
#endif /* PPDEBUG */

    if (Verbose)
        printf("*** Include %s\n", incfile);

    ok = FALSE;

#if HOST == H_CPM /* The following stuff works only under CP/M */

    /*
     *	Determine the type of include: if d == '<' then search the path
     *	indicated by the -i parameter (or the default.)  If the delimiter is
     *	'"' then include the current directory as well.
     */
    if (d == '"')
        ok = inc_open(incfile, -1, 0); /* The current location */

    /* Try all path options in -i list (including DFLT_PATH stuff) */
    for (ip = &Ipath[0]; (*ip != NULL) && !ok; ip++)
    {
        p = *ip;

        while (istype(*p & 0xFF, C_W))
            ++p; /* Skip white space */

        disk = *p++; /* Get drive letter */

        if (isupper(disk))
            disk = tolower(disk);

        if (disk == '*')
            user = -1; /* Use the default */
        else if ((disk >= 'a') && (disk <= 'p'))
        {
            disk -= 'a'; /* Drive */

            if (!isdigit(*p))
                user = -1; /* Default */
            else
            {
                user = atoi(p);
                if ((user < 0) || (user > 31))
                {
                    non_fatal("Illegal user number", "");
                    return;
                }
            }
        }
        else
        {
            non_fatal("Illegal disk drive specifier", "");
            return;
        }

        ok = inc_open(incfile, user, disk);
    }

#else /* HOST != H_CPM */

    if (d == '"')
    {
        /* Look in current directory */
        strcpy(filename, Filestack[Filelevel]->f_name);
        if (strrchr(filename, SLASHCHAR))
            strcpy(strrchr(filename, SLASHCHAR) + 1, incfile);
        else
            strcpy(filename, incfile);

        ok = inc_open(filename);
    }

    /* Look through all paths for an existance of the file */

    for (ip = &Ipath[0]; *ip != NULL && !ok; ip++)
    {
        strcpy(filename, *ip);      /* Copy path name */
        strcat(filename, SLASHSTR); /* Append / for directory */
        strcat(filename, incfile);  /* Append local file name */
        ok = inc_open(filename);    /* Attempt an open */
    }

#endif /* (HOST != H_CPM) */

#if HOST == H_CPM
    cur_user(); /* Restore current user/disk */
#endif          /* HOST == H_CPM */

    if (!ok)
        non_fatal("Unable to open include file: ", incfile);
    pushback('\n');

    /* Let token scanner see first things on line */
    Lastnl = TRUE;
}

/************************************************************************/
/*									*/
/*	doline								*/
/*									*/
/*	Set current line number and file identifier to that specified.	*/
/*									*/
/************************************************************************/

void doline(int aaa, int bbb, const char *ccc)
{
    char buf[TOKENSIZE];
    int c;
    int l;
    char *p;
    /*
     *	Read line and process any macros encountered.
     */
    pbcstr(readline(buf, TOKENSIZE, GT_STR));

    while (istype((c = getchn()), C_W))
        ;

    if (istype(c, C_D))
    {
        for (l = 0; istype(c, C_D); c = getchn())
            l = l * 10 + c - '0';

        LLine = l - 1; /* Set line number */

        pushback(c);
        c = getnstoken(GT_STR);

        if ((c != '\n') && (c != EOF))
        {
            if (c == '"')
            {
                p = strrchr(Token, '"'); /* Find ending " */

                /* Allow for first " */
                if (p - Token > FILENAMESIZE)
                    p = &Token[FILENAMESIZE + 1];

                *p = '\0'; /* Terminate it */
                strcpy(Filestack[Filelevel]->f_name, Token + 1);
                /* Need filename on "output" #line */
                Do_name = TRUE;
            }
            else
            {
                pushback(c);
                c = '\0';
            }
        }
    }
    else
    {
        pushback(c);
        c = '\0';
    }

    if (c == '\0')
        non_fatal("\"#line\" argument error", "");

    while ((c != '\n') && (c != EOF))
        c = getnstoken(GT_STR); /* Slurp trailing \n */
    pushback('\n');
}

/************************************************************************/
/*									*/
/*	gchbuf								*/
/*									*/
/*	Get char from input buffer.  If out of chars in buffer, read	*/
/*	another buffer.							*/
/*									*/
/************************************************************************/

int gchbuf()
{
    int c;

    for (;;)
    {
        if (Lasteol)
        {
            Lasteol = FALSE;
            LLine++;
        }

        if (!istype(c = (Bufc-- ? *Bufp++ : gchfile()), C_C))
            break; /* If no need to examine closely */

#ifdef IGNORE_CR
        if (c == '\r')
            continue;
#endif /* IGNORE_CR */

        if (c == '\n')
            Lasteol = TRUE; /* Inc line number next time */
#ifdef PP_SYSIO
    #if HOST == H_CPM
        else if (c == ENDFILE)
        {
            Bufc = 0;
            continue; /* Try to get next file */
        }
    #endif /* HOST == H_CPM */
#endif     /* PP_SYSIO */
        break;
    }
    return (c); /* And exit */
}

/************************************************************************/
/*									*/
/*	gchfile								*/
/*									*/
/*	Get next buffer of chars from input file.  If end of file	*/
/*	found, pop file level and read from lower file.  If out of	*/
/*	files, return EOF.						*/
/*									*/
/************************************************************************/

int gchfile()
{
#ifdef PP_SYSIO
    extern int read();
#endif /* PP_SYSIO */

    struct file *f;

    if (Filelevel < 0)
    {
        Bufc = 0;
        return (EOF);
    }
    else if ((Filestack[Filelevel]->f_eof) && popfile())
        return (A_trigraph ? trigraph() : gchbuf());

    if (Filelevel < 0)
    {
        Bufc = 0;
        return (EOF);
    }
    f = Filestack[Filelevel];

#if HOST == H_CPM
    set_user();
#endif /* HOST == H_CPM */

    Bufp = f->f_buf; /* Set buffer address */

#ifdef PP_SYSIO
    if ((Bufc = read(f->f_fd, Bufp, BUFFERSIZE)) == 0)
#else  /* !PP_SYSIO */
    if ((Bufc = fread(Bufp, 1, BUFFERSIZE, f->f_file)) == 0)
#endif /* PP_SYSIO */
    {
        f->f_eof = TRUE;
        return ('\n'); /* Fake '\n' before EOF */
    }
#if HOST == H_CPM
    cur_user();
#endif /* HOST == H_CPM */

    Bufc--;
    return (*Bufp++ & 0xFF); /* Return the char */
}

/************************************************************************/
/*									*/
/*	gchpb								*/
/*									*/
/*	Get character from push-back buffer.  If out of chars, return	*/
/*	next char routine to get chars from file.			*/
/*									*/
/************************************************************************/

int gchpb()
{
    int c;

    for (;;)
    {
        if (Pbbufp->pb_type == PB_CHAR)
            c = (Pbbufp--)->pb_val.pb_char; /* Pop the char */
        else if (Pbbufp->pb_type == PB_STRING)
        {
            /* Get next char from string */
            if ((c = *(Pbbufp->pb_val.pb_str++) & 0xFF) == '\0')
            {
                /*
                 *	End of the string.  Pop the stack to get the next pushback buffer
                 *	which contains the starting address for the string.
                 */
                Pbbufp--;
                /* Free up the memory */ free(Pbbufp->pb_val.pb_str);
                /* Get nxt real pushback buf */ Pbbufp--;
                /* Try again to get input */ continue;
            }
        }
        else
        {
            /* Assume PB_TOS and pop to file input */
            /* Next char source */ Nextch = A_trigraph ? trigraph : gchbuf;
            return (A_trigraph ? trigraph() : gchbuf());
        }
        return (c);
    }
}

/************************************************************************/
/*									*/
/*	getchn								*/
/*									*/
/*	Get next char from nextch() routine, ignoring \ followed	*/
/*	by a newline character.						*/
/*									*/
/************************************************************************/

int getchn()
{
    int c;
    int c2;

    for (;;)
    {
        if ((c = nextch()) == '\\')
        {
            c2 = nextch();
            if (c2 == '\n')
                continue; /* Ignore char and get next */

            pushback(c2); /* We'll get to this char later */
        }
        return (c);
    }
}

/************************************************************************/
/*									*/
/*	inc_open							*/
/*									*/
/*	Open an include file for the selected path.			*/
/*									*/
/************************************************************************/

#if HOST == H_CPM
int inc_open(incfile, u, d)
char *incfile;
int u;
int d;
#else  /* HOST != H_CPM */
int inc_open(const char *incfile)
#endif /* HOST == H_CPM */
{
#ifdef PP_SYSIO
    extern int open();
#endif /* PP_SYSIO */

    int v;
    struct file *f;
    struct file *fold;

#if HOST == H_CPM

    #if PPDEBUG
    if (PPDEBUG)
        printf("inc_open: %s on %c%d\n", incfile, d + 'A', u);
    #endif /* PPDEBUG */

    if (u >= 0)
    {
        bdos(BDOS_USER, u);
        bdos(BDOS_SELDISK, d);
    }

#else /* HOST != H_CPM */

    #if PPDEBUG
    if (PPDEBUG)
        printf("inc_open: %s\n", incfile);
    #endif /* PPDEBUG */

#endif /* HOST == H_CPM */

    f = Filestack[Filelevel + 1] = (struct file *)malloc(sizeof(struct file));

    if (f == NULL)
        out_of_memory();

#ifdef PP_SYSIO
    if ((v = ((f->f_fd = open(incfile, 0)) != -1)) != 0)
#else  /* !PP_SYSIO */
    if ((v = (int)((f->f_file = fopen(incfile, "r")) != NULL)) != 0)
#endif /* PP_SYSIO */
    {
        if (Filelevel >= 0) /* Don't do if first time thru */
        {
#if PPDEBUG
            if (PPDEBUG)
            {
                printf("inc_open pushing: Bufc=%d, Bufp=%p, Lasteol=%d, Line=%d\n", Bufc, Bufp, Lasteol, LLine);
            }
#endif /* PPDEBUG */

            fold = Filestack[Filelevel];
            fold->f_bufp = Bufp;       /* Save current buf ptr */
            fold->f_bufc = Bufc;       /* Save current buffer count */
            fold->f_lasteol = Lasteol; /* Save last char */
            fold->f_line = LLine;      /* Save current line # */
        }
        Filelevel++;
        strcpy(f->f_name, incfile);
        LLine = 1;           /* Initial line number		*/
        Bufc = 0;            /* No chars in buffer		*/
        f->f_eof =           /* Not at eof yet		*/
            Lasteol = FALSE; /* Last char was not EOL	*/
#if HOST == H_CPM
        f->f_disk = d;
        f->f_user = u;
#endif /* HOST == H_CPM */
    }
    else
        free((char *)f); /* Return the memory used */

#if HOST == H_CPM
    if (u >= 0)
        cur_user(); /* Restore current user/disk */
#endif              /* HOST == H_CPM */

    if (v)
    {
        Do_name = TRUE;
        return (TRUE);
    }
    else
        return (FALSE);
}

/************************************************************************/
/*									*/
/*	init_path							*/
/*									*/
/*	Initialize path searching information.  Try to open a file	*/
/*	defined as PATHFILE on A0:.  If available, read in the path	*/
/*	default information, otherwise use CPM_PATH as the default.	*/
/*									*/
/*	If initializing for non-CPM, initialize Ipath to whatever is	*/
/*	specified by the PPINC environment variable or DFLT_PATH if no	*/
/*	matching environment variable is found.				*/
/*									*/
/************************************************************************/

void init_path()
{
#if HOST == H_CPM
    int inum;
    char pb[TOKENSIZE];
    FILE *pf;

    bdos(BDOS_USER, 0);
    bdos(BDOS_SELDISK, 0); /* Select A0 */

    if (pf = fopen(PATHFILE, "r"))
    {
        /* Found the file -- read lines and use as paths */
        for (inum = Ipcnt; inum < NIPATHS; inum++)
        {
            if (fgets(pb, TOKENSIZE, pf) != NULL)
            {
                if ((Ipath[inum] = malloc((unsigned)(strlen(pb) + 1))) == NULL)
                {
                    out_of_memory();
                }
                else
                    /* Copy the default path */ strcpy(Ipath[inum], pb);
            }
            else
            {
                if (inum == Ipcnt)
                {
                    /* Didn't find any -- give error msg */
                    warning("Bad format on include path file: ", PATHFILE);
                    /* Use default path */ Ipath[inum++] = DFLT_PATH;
                }
                break;
            }
        }

        Ipcnt = inum; /* Keep counter correct	*/

        if (fclose(pf) == EOF)
        {
            non_fatal("Unable to close include path file: ", PATHFILE);
        }
    }
    else
        Ipath[Ipcnt++] = DFLT_PATH; /* The default path list */

    cur_user(); /* Restore user/disk defaults */
#endif          /* HOST == H_CPM */

#if HOST != H_CPM

    char *cptr1;
    char *cptr2;
    char pb[TOKENSIZE];
    char msbuf[20] = {0};
    /*
     *	See if there is an environment variable for the default search path.
     */
    if ((cptr1 = getenv(ENV_PATH)) == NULL)
        cptr1 = strcpy(pb, DFLT_PATH); /* Nope, use default path */
    else
        cptr1 = strcpy(pb, cptr1);

    for (; (*cptr1 != '\0') && (Ipcnt < NIPATHS); cptr1 = cptr2)
    {
        if ((cptr2 = strchr(cptr1, PATHPUNC)) != NULL)
            *cptr2++ = '\0';
        else
            cptr2 = msbuf;
        // MS2020 cptr2 = "\0";

        if ((Ipath[Ipcnt] = (char *)malloc((unsigned)(strlen(cptr1) + 1))) == NULL)
        {
            out_of_memory();
        }
        else
            strcpy(Ipath[Ipcnt++], cptr1);
    }
#endif /* HOST != H_CPM */
}

/************************************************************************/
/*									*/
/*	popfile								*/
/*									*/
/*	Pop file stack to previous file and return FALSE if no more.	*/
/*									*/
/************************************************************************/

int popfile()
{
#ifdef PP_SYSIO
    extern int close();
#endif /* PP_SYSIO */
    struct file *f;

#if HOST == H_CPM
    set_user();
#endif /* HOST == H_CPM */

#ifdef PP_SYSIO
    if (close((f = Filestack[Filelevel])->f_fd) == -1)
#else  /* !PP_SYSIO */
    if (fclose((f = Filestack[Filelevel])->f_file) == EOF)
#endif /* PP_SYSIO */
        non_fatal("Unable to close input/include file: ", f->f_name);

    free((char *)f); /* Free the entry */

#if HOST == H_CPM
    cur_user();
#endif /* HOST == H_CPM */

    if (Filelevel-- == 0)
        return (FALSE); /* At bottom level, real EOF */

    f = Filestack[Filelevel];

    if (Verbose)
        printf("*** Resume  %s\n", f->f_name);

    Do_name = TRUE; /* Next time do_line called, name it */
    Bufc = f->f_bufc;
    Bufp = f->f_bufp;
    Lasteol = f->f_lasteol;
    LLine = f->f_line;

#if PPDEBUG
    if (PPDEBUG)
        printf("popfile: Bufc=%d, Bufp=%p, Lasteol=%d, Line=%d\n", Bufc, Bufp, Lasteol, LLine);
#endif /* PPDEBUG */

    return (TRUE); /* All is ok -- return success */
}

/************************************************************************/
/*									*/
/*	readline							*/
/*									*/
/*	Read and edit a line into buffer with macro expansion.		*/
/*									*/
/************************************************************************/

char *readline(char *buf, int bufsize, int flags)
{
    static char rbo[] = "Read buffer overflow";

    char *bufp;
    struct symtab *sy;
    int t;

    for (bufp = buf; (t = gettoken(flags)) != '\n';)
    {
        if (t == EOF)
            end_of_file();
        if ((t == LETTER) && ((sy = lookup(Token, NULL)) != NULL) && (sy->disable != TRUE))
        {
            bufp = docall(sy, bufp, &buf[bufsize - 1]);
        }
        else
            bufp = addstr(bufp, &buf[bufsize - 1], rbo, Token);
    }
    pushback('\n');
    *bufp = '\0';

    for (bufp = buf; istype(*bufp & 0xFF, C_W); ++bufp)
        ; /* Skip leading blanks */

    return (bufp);
}

/************************************************************************/
/*									*/
/*	scaneol								*/
/*									*/
/*	Scan for end of current line.					*/
/*									*/
/************************************************************************/

void scaneol()
{
    int t;

    while ((t = gettoken(GT_STR)) != '\n')
    {
        if (t == EOF)
            return; /* Absorb chars */
    }
    pushback('\n'); /* So the newline is seen */
}

#if HOST == H_CPM
/************************************************************************/
/*									*/
/*	set_user							*/
/*									*/
/*	Set disk/user to that of current input file.			*/
/*									*/
/************************************************************************/

void set_user()
{
    /* Don't change if < 0 */
    if (Filestack[Filelevel]->f_user >= 0)
    {
        bdos(BDOS_SELDISK, Filestack[Filelevel]->f_disk);
        bdos(BDOS_USER, Filestack[Filelevel]->f_user);
    }
}
#endif /* HOST == H_CPM */

/************************************************************************/
/*									*/
/*	trigraph							*/
/*									*/
/*	Handles ANSI trigraph substitutions (alternate to "gchbuf").	*/
/*									*/
/************************************************************************/

int trigraph()
{
    int c;
    int q_count;

    if ((c = gchbuf()) == '?')
    {
        if ((c = gchbuf()) == '?')
        {
            q_count = 0; /* Extra '?' encountered */
            for (;;)
            {
                switch ((int)(c = gchbuf()))
                {
                    case '=':
                        c = '#';
                        break;
                    case '(':
                        c = '[';
                        break;
                    case '/':
                        c = '\\';
                        break;
                    case ')':
                        c = ']';
                        break;
                    case '\'':
                        c = '^';
                        break;
                    case '<':
                        c = '{';
                        break;
                    case '!':
                        c = '|';
                        break;
                    case '>':
                        c = '}';
                        break;
                    case '-':
                        c = '~';
                        break;
                    case '?':
                        q_count++;
                        continue;
                    default:
                        pushback(c);
                        pushback('?');
                        c = '?';
                        break;
                }
                /*
                 *	Handle stuff like "????????????#" correctly.
                 */
                if (q_count > 0)
                {
                    pushback(c);
                    while (q_count-- > 1)
                        pushback('?');
                    c = '?';
                }
                break;
            }
        }
        else
        {
            pushback(c);
            c = '?';
        }
    }
    return (c);
}

#pragma GCC diagnostic pop
