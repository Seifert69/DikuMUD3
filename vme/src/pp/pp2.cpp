#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

/************************************************************************/
/*									*/
/*	File:	pp2.c							*/
/*									*/
/*	Symbol table and macro functions.				*/
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
/*	Functions contained within this module:				*/
/*									*/
/*		docall		Execute macro call.			*/
/*		_docall		Helping function for docall.		*/
/*		dodefine	Process #define directive.		*/
/*		doerror		Process #error directive.		*/
/*		doundef		Process #undef directive.		*/
/*		fbind		A a formal paramter instantiation.	*/
/*		flookup		Lookup a formal parameter.		*/
/*		esc_str		Helper for strize.			*/
/*		getparams	Get parameters for defining macro.	*/
/*		hash		Hash symbol to a value.			*/
/*		lookup		Lookup a symbol in symbol table.	*/
/*		predef		Lookup a directive in directive table.	*/
/*		sbind		Bind a macro body to a symbol.		*/
/*		strize		Handle "#" (stringify) operator.	*/
/*		unfbind		Remove binding of formal parameters.	*/
/*		unparam		Return to free mem a parameter chain.	*/
/*		unsbind		Unbind a symbol from symbol table.	*/
/*									*/
/************************************************************************/

#include "pp.h"

/************************************************************************/
/*									*/
/*	docall								*/
/*									*/
/*	Process a macro call.						*/
/*									*/
/*	The last fetched token was the macro name.  The input		*/
/*	parameter is the symtab pointer to the macro prototype.		*/
/*	Now we must fetch the parameters (if any) and bind the		*/
/*	values to local (new) symbols for processing.			*/
/*									*/
/************************************************************************/

#ifdef MSC_OPT
    #pragma optimize("e", off) /* Disable global reg optimizing */
    #pragma optimize("g", off) /* Disable global common subs */
    #pragma optimize("l", off) /* Disable loop optimizations */
#endif                         /* MSC_OPT */

char *docall(struct symtab *p, char *internal, char *internal_limit)
{
    static char mbomsg[] = "Macro body overflow";
    static char toggle_token[] = {(char)TOGGLE_EXPAND, '\0'};

    int addit;
    char *body;
    char *bodyp;
    char *cp;
    int done;
    int expand;
    int flags;
    struct symtab *formals;
    int had_ws;
    int maclevel;
    int nl_count;
    int pcount;
    struct param *pp;
    int printed;
    struct symtab *sy;
    int t;
    char t_token[TOKENSIZE];
    /*
     *	Crack the formal parameters for the macro.
     */
    formals = NULL; /* No parameter instantiations yet */
    maclevel = 0;

    if ((pp = p->s_params) != NULL)
    {
        /* This macro has parameters: see if present on this call */
        had_ws = FALSE;

        expand = TRUE;
        nl_count = 0;

        while (((t = gettoken(GT_STR)) == '\n') || istype(t, C_W) || (t == TOGGLE_EXPAND) || (t == END_MACRO))
        {
            if (t == '\n')
                ++nl_count;
            else if (t == TOGGLE_EXPAND)
                expand = !expand;
            else if (t == END_MACRO)
                ++maclevel;
            else
                had_ws = TRUE;
        }

        if (expand && (t == '('))
        {
            /* A left parenthesis -- parameters follow */
            printed = done = FALSE;
            cp = t_token;
            pcount = 1;

            /* Special <null> parameter? */
            if (pp->p_name[0] == '\0')
                pp = pp->p_link; /* Skip over it */

            while (!done)
            {
                addit = TRUE; /* Assume to add to string */

                switch (gettoken(GT_STR))
                {
                    case '"':
                        if ((pp != NULL) && ((pp->p_flags & PF_RQUOTES) != 0))
                        {
                            /* Remove surrounding quotes */
                            memmov(Token + 1, Token, (unsigned)strlen(Token));
                            Token[strlen(Token) - 1] = '\0';
                        }
                        break;

                    case '\n':
                        if ((pp != NULL) && ((pp->p_flags & PF_PNLINES) == 0))
                        {
                            /* Remove parameter newlines */
                            Token[0] = ' ';
                            Token[1] = '\0';
                        }
                        break;

                    case '(':
                        ++pcount; /* Count up parenthesis */
                        break;

                    case ')':
                        if (--pcount != 0) /* Count down '()' */
                            break;         /* Continue if not level 0 */

                        pcount = 1;  /* Wrap -- make comma fake */
                        done = TRUE; /* We are finished */

                        /* Fall through for end of param list */

                    case ',':
                        if (pcount == 1)
                        {
                            addit = FALSE; /* Do not add */
                            if (pp == NULL)
                            {
                                if (((!done) || (cp != t_token)) && (!printed))
                                {
                                    warning("Too many arguments for: ", p->s_name);
                                    printed = TRUE;
                                }
                            }
                            else
                            {
                                *cp = '\0';
                                fbind(&formals, pp->p_name, t_token);
                                if ((cp == t_token) && (!printed))
                                {
                                    warning("Missing argument for: ", p->s_name);
                                    printed = TRUE;
                                }
                                cp = t_token;
                                pp = pp->p_link;
                                if ((!done) && (pp == NULL) && (!printed))
                                {
                                    warning("Too many arguments for: ", p->s_name);
                                    printed = TRUE;
                                }
                            }
                        }
                        break;

                    case EOF:
                        end_of_file(); /* Unexpected end of file */

                    default:
                        break;
                }

                if (addit)
                {
                    cp = addstr(cp, &t_token[TOKENSIZE - 1], "Parameter buffer overflow", Token);
                }
            }
            /*
             *	Null bindings for the rest of the parameters, and complain.
             */
            if (pp != NULL)
            {
                warning("Too few arguments for: ", p->s_name);
                for (/* Empty */; pp != NULL; pp = pp->p_link)
                    fbind(&formals, pp->p_name, "");
            }
        }
        else
        {
            /*
             *	Although this looked like a macro call, it wasn't really.  Push back
             *	the non "(" token, together with a whitespace, if one was present.
             *	Then just output the macro name.
             */
            pbstr(Token); /* Put it back */

            while (nl_count-- > 0) /* Dump linefeeds through */
                pushback('\n');

            while (maclevel-- != 0)
                pushback(END_MACRO); /* Keep call lvl OK */

            if (!expand)
                pushback(TOGGLE_EXPAND);

            if (internal == NULL)
            {
                puttoken(p->s_name);
                if (had_ws)
                    puttoken(" ");
            }
            else
            {
                /*
                 *	HACK: Put TOGGLE_TOKENS around the non-macro to "pre-complement" it
                 *	for the later expansion toggle flag put on by the calling copy of
                 *	"docall".
                 */

                internal = addstr(internal, internal_limit, mbomsg, toggle_token);
                internal = addstr(internal, internal_limit, mbomsg, p->s_name);
                internal = addstr(internal, internal_limit, mbomsg, toggle_token);
                if (had_ws)
                {
                    internal = addstr(internal, internal_limit, mbomsg, " ");
                }
            }
            return (internal);
        }
    }
    while (maclevel-- != 0)
        pushback(END_MACRO); /* Keep call level correct */
    /*
     *	Now open the macro body for input and read while processing formal
     *	parameters.
     */
    pushback(END_MACRO);
    pbcstr(p->s_body);

    /* Allocate a place for the macro replacement */
    if ((body = (char *)malloc((unsigned)MACROSIZE)) == NULL)
        out_of_memory();

    flags = (A_astring ? (0) : (GT_STR));
    had_ws = FALSE;
    expand = TRUE;
    bodyp = body; /* Point at start of body */

    /*
     *	The following code does the formal parameter substitution and macro
     *	expansion of formal parameters to ANSI specifications with the ## and
     *	# operators.  To understand this code get a copy of the ANSI spec and
     *	expect to spend some time!  The code within the while loop is a state
     *	machine with "if" clauses either looping back via a "continue" or
     *	falling through to the next test.  The "had_ws" and "expand" flags
     *	are used to remember the fact that whitespace was encountered prior to
     *	the "current" token and the current value of the expand/no-expand
     *	flags prior to the current token (respectively).  The expand/no-expand
     *	flag is used to indicate where macro expansion has already been done
     *	(ie. implement ANSI scopes and the fact that formals get expanded
     *	before the rest of the body).
     */
    while ((t = gettoken(flags)) != END_MACRO)
    {
        if (t == EOF)
            end_of_file();
        /*
         *	Keep track of EXPAND_TOKEN and whitespace occurring before the current
         *	token.
         */
        if (istype(t, C_W))
        {
            had_ws = TRUE;
            continue;
        }
        if (t == TOGGLE_EXPAND)
        {
            expand = !expand;
            continue;
        }
        /*
         *	Handle ## operator.
         */
        if (t == '#')
        {
            if ((t = gettoken(flags)) == '#')
            {
                while (istype((t = gettoken(flags)), C_W) || (t == TOGGLE_EXPAND))
                {
                    if (t == TOGGLE_EXPAND)
                        /* Skip toggle tokens */ expand = !expand;
                }
                if (t == END_MACRO)
                {
                    pushback(END_MACRO);
                    if (had_ws)
                    {
                        had_ws = FALSE;
                        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                    }
                    if (!expand)
                    {
                        expand = TRUE;
                        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                    }
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, "##");
                    continue;
                }
                had_ws = FALSE; /* Ignore whitespace */
                if ((t == LETTER) && ((cp = flookup(formals, Token)) != NULL))
                {
                    pushback(END_MACRO);
                    pbcstr(cp);
                    while (istype((t = gettoken(flags)), C_W) || (t == TOGGLE_EXPAND))
                    {
                        if (t == TOGGLE_EXPAND)
                            /* Skip toggle tokens */ expand = !expand;
                    }
                    if (expand != TRUE)
                    {
                        expand = TRUE;
                        pushback(TOGGLE_EXPAND);
                    }
                    /* Refetch in while loop */ pbstr(Token);
                    while ((t = gettoken(flags)) != END_MACRO)
                    {
                        if (t == EOF)
                            end_of_file();
                        if (istype(t, C_W))
                        {
                            had_ws = TRUE;
                            continue;
                        }
                        if (t == TOGGLE_EXPAND)
                        {
                            expand = !expand;
                            continue;
                        }
                        if (had_ws)
                        {
                            had_ws = FALSE;
                            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                        }
                        if (!expand)
                        {
                            expand = TRUE;
                            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                        }
                        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
                    }
                }
                else
                {
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
                }

                continue;
            }
            else /* Was # operator, look at in a bit */
            {
                pbstr(Token);
                strcpy(Token, "#");
                t = '#';
            }
        }
        /*
         *	All other token types need to have the "had_ws" and "expand" flags
         *	flushed prior to being examined.
         */
        if (had_ws)
        {
            had_ws = FALSE;
            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
        }
        if (!expand)
        {
            expand = TRUE;
            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
        }
        /*
         *	Handle # operator.
         */
        if (t == '#')
        {
            while (istype((t = gettoken(flags)), C_W) || (t == TOGGLE_EXPAND))
            {
                if (istype(t, C_W))
                    had_ws = TRUE; /* Skip whitespace */
                else if (t == TOGGLE_EXPAND)
                    /* Skip toggle tokens */ expand = !expand;
            }
            if ((t == LETTER) && ((cp = flookup(formals, Token)) != NULL))
            {
                had_ws = FALSE;
                bodyp = strize(bodyp, &body[MACROSIZE - 1], mbomsg, cp);
                if (!expand)
                {
                    expand = TRUE;
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                }
            }
            else /* Wasn't formal send # through */
            {
                bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, "#");
                if (had_ws)
                {
                    had_ws = FALSE;
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                }
                if (!expand)
                {
                    expand = TRUE;
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                }
                pbstr(Token); /* Refetch later */
            }
            continue;
        }
        /*
         *	Handle formal parameters.  Note that we need to look ahead to see if
         *	the next operator is ## in order to know whether to perform macro
         *	expansion on the parameter value or not.
         */
        if ((t == LETTER) && ((cp = flookup(formals, Token)) != NULL))
        {
            while (istype((t = gettoken(flags)), C_W) || (t == TOGGLE_EXPAND))
            {
                if (istype(t, C_W))
                    had_ws = TRUE; /* Skip whitespace */
                else if (t == TOGGLE_EXPAND)
                    /* Skip toggle tokens */ expand = !expand;
            }
            /*
             *	See in what form we are going to need the parameter.
             */
            if (t == '#') /* See if ## */
            {
                if ((t = gettoken(flags)) == '#')
                {
                    had_ws = FALSE;
                    pushback(END_MACRO);
                    pbcstr(cp);
                    /*
                     *	Transfer the formal parameter into the result while pruning trailing
                     *	whitespace and TOGGLE_EXPAND's.
                     */
                    while ((t = gettoken(flags)) != END_MACRO)
                    {
                        if (t == EOF)
                            end_of_file();
                        if (istype(t, C_W))
                        {
                            had_ws = TRUE;
                            continue;
                        }
                        if (t == TOGGLE_EXPAND)
                        {
                            expand = !expand;
                            continue;
                        }
                        if (had_ws)
                        {
                            had_ws = FALSE;
                            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                        }
                        if (!expand)
                        {
                            expand = TRUE;
                            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                        }
                        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
                    }
                    pbstr("##"); /* Refetch again */
                    continue;
                }
                else
                {
                    /*
                     *	Was single #, pushback # for later.
                     */
                    pbstr(Token);
                    strcpy(Token, "#");
                }
            }
            pbstr(Token); /* Refetch the last non # token */

            if (had_ws)
            {
                had_ws = FALSE;
                pushback(' ');
            }
            if (!expand)
            {
                expand = TRUE;
                pushback(TOGGLE_EXPAND);
            }
            /*
             *	This macro replaces the entire replacement string for the parameter.
             */
            pushback(END_ARG);
            pbcstr(cp); /* Parameter value */

            while ((t = gettoken(flags)) != END_ARG)
            {
                if (t == EOF)
                    end_of_file();
                if (istype(t, C_W))
                {
                    had_ws = TRUE;
                    continue;
                }
                if (t == TOGGLE_EXPAND)
                {
                    expand = !expand;
                    continue;
                }
                if (had_ws)
                {
                    had_ws = FALSE;
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                }
                if (!expand)
                {
                    expand = TRUE;
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                }
                /*
                 *	Process an individual token in the replacement string for the formal
                 *	parameter.
                 */
                if ((t == LETTER) && ((sy = lookup(Token, NULL)) != NULL) && (!sy->disable))
                {
                    *docall(sy, t_token, &t_token[TOKENSIZE - 1]) = '\0';
                    pushback(END_MACRO);
                    pbcstr(t_token);
                    /* Disable further macro expansion */
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
                    while ((t = gettoken(flags)) != END_MACRO)
                    {
                        if (t == EOF)
                            end_of_file();
                        if (istype(t, C_W))
                        {
                            had_ws = TRUE;
                            continue;
                        }
                        if (had_ws)
                        {
                            had_ws = FALSE;
                            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
                        }
                        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
                    }
                    expand = FALSE;
                }
                else
                {
                    bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
                }
            }
        }
        /*
         *	Handle all other tokens.
         */
        else
            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
    }
    /*
     *	Flush any remaining whitespace or disable tokens.
     */
    if (had_ws)
    {
        had_ws = FALSE;
        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
    }
    if (!expand)
    {
        expand = TRUE;
        bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, toggle_token);
    }
    *bodyp = '\0';     /* Terminate macro body */
    unfbind(formals);  /* Return param instantiation mem */
    p->disable = TRUE; /* No recursive macros in ANSI */
    /*
     *	Now scan the macro body (modified by formal parameter substitution and
     *	formal parameter macro expansion), so that the rest of the macro
     *	expansion occurs.  Do this by buffering a line at a time so that
     *	directive re-scanning will work if the user has requested it.  Note
     *	that rescanning for macro expansion is not performed on any line
     *	beginning with a '#'.
     */
    pushback(END_MACRO);
    pbcstr(body);
    expand = TRUE;

    bodyp = body;
    had_ws = FALSE;
    while ((t = gettoken(GT_STR)) != END_MACRO)
    {
        if (istype(t, C_W))
        {
            had_ws = TRUE;
            continue;
        }
        if (t == EOF)
            end_of_file();
        if (t == TOGGLE_EXPAND)
            expand = !expand;
        else if ((t == LETTER) && expand && ((bodyp > body) ? (body[0] != '#') : TRUE) && ((sy = lookup(Token, NULL)) != NULL) &&
                 (!sy->disable))
        {
            if (had_ws)
            {
                had_ws = FALSE;
                bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
            }
            bodyp = docall(sy, bodyp, &body[MACROSIZE - 1]);
        }
        else if (t == '\n')
        {
            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
            *bodyp = '\0';
            internal = _docall(body, internal, internal_limit);
            had_ws = FALSE;
            bodyp = body;
        }
        else
        {
            if ((t == '#') && (bodyp == body))
                had_ws = FALSE;
            if (had_ws)
            {
                had_ws = FALSE;
                bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, " ");
            }
            bodyp = addstr(bodyp, &body[MACROSIZE - 1], mbomsg, Token);
        }
    }
    /*
     *	Output any trailing stuff.
     */
    *bodyp = '\0';
    internal = _docall(body, internal, internal_limit);

    free(body);         /* Return body buffer memory */
    p->disable = FALSE; /* Macro symbol scoping update */
    return (internal);
}

#ifdef MSC_OPT
    #pragma optimize("e", on) /* Enable global reg optimizing */
    #pragma optimize("g", on) /* Enable global common subs */
    #pragma optimize("l", on) /* Enable loop optimizing */
#endif                        /* MSC_OPT */

/************************************************************************/
/*									*/
/*	_docall								*/
/*									*/
/*	Process a macro replaced line to either output it where		*/
/*	appropriate or process any contained directives if desired.	*/
/*									*/
/************************************************************************/

char *_docall(char *line, char *internal, char *internal_limit)
{
    static char mbomsg[] = "Macro body overflow";

    struct ppdir *d;
    int had_ws;
    int t;

    pushback(END_MACRO);
    pbcstr(line);

    Lastnl = TRUE;
    while ((t = gettoken(GT_STR)) != END_MACRO)
    {
        if (Lastnl && (t == DIRECTIVE_CHAR)) /* Process directive */
        {
            had_ws = FALSE;
            while (((t = gettoken(GT_STR)) != END_MACRO) && (t != EOF) && istype(t, C_W))
            {
                had_ws = TRUE;
            }
            if ((t == LETTER) && ((d = predef(Token, pptab)) != NULL) && ((d->pp_func == dopragma) || A_rescan))
            {
                if (d->pp_ifif || (Ifstate == IFTRUE))
                    (void)(*(d->pp_func))(d->pp_arg, 0, 0);
                Lastnl = TRUE;
            }
            else
            {
                if (internal == NULL)
                    puttoken("#");
                else
                {
                    internal = addstr(internal, internal_limit, mbomsg, "#");
                }
                pbstr(Token);
                if (had_ws)
                    pushback(' ');
                Lastnl = FALSE;
            }
        }
        else if (Ifstate == IFTRUE) /* Normal token, just output */
        {
            if (internal == NULL)
                puttoken(Token);
            else
            {
                internal = addstr(internal, internal_limit, mbomsg, Token);
            }
            if (!istype(t, C_W))
                Lastnl = FALSE;
        }
        else /* Toss up to EOL */
        {
            while ((t != '\n') && (t != EOF) && (t != END_MACRO))
                t = gettoken(GT_STR);
        }
        if (t == EOF)
            end_of_file();
        else if (t == '\n')
            Lastnl = TRUE;
        else if (t == END_MACRO)
            break;
    }
    return (internal);
}

/************************************************************************/
/*									*/
/*	dodefine							*/
/*									*/
/*	Define a macro.							*/
/*									*/
/*	There are several types of macro definitions:			*/
/*									*/
/*	#define	macro		string	\* No check for parameters *\	*/
/*	#define	macro()		string	\* check for parameters *\	*/
/*	#define	macro(x)	(x)	\* Process parameters x *\	*/
/*	#pragma macro  name(-one of above three-)			*/
/*	<text>								*/
/*	...								*/
/*	#pragma endm							*/
/*									*/
/*	1) get name token.						*/
/*									*/
/*	2) if next token is a '(' then build a list of strings		*/
/*	   delimited by zero-level commas, until a zero-level		*/
/*	   ')' is found.						*/
/*									*/
/*	3) get EOL (or EOF) terminated string for body.			*/
/*									*/
/*   	sbind(name, body, list_of_parameters (or NO_PARAMS));		*/
/*									*/
/************************************************************************/

void dodefine(int mactype, int ixzy, const char *ixzz)
{
    static char mtlmsg[] = "Macro too long";

    char body[MACROSIZE]; /* Body of def	*/
    char *bodyp;          /* Pointer into body		*/
    int lasttok;          /* Last token fetched in loop	*/
    int macctr;           /* Level ctr for #macro/#endm	*/
    char name[TOKENSIZE]; /* Name buffer	*/
    struct param *pp;     /* Param proto pointer		*/
    struct symtab *sy;    /* Symbol table ptr		*/
    int t;                /* Token type			*/

    if (getnstoken(GT_STR) == LETTER)
    {
        strcpy(name, Token); /* Move name token to save buffer */

#if PPDEBUG
        if (PPDEBUG)
            printf("dodefine: <%s>\n", name);
#endif /* PPDEBUG */

        if (gettoken(GT_STR) == '(')
            pp = getparams(); /* Get list of param protos */
        else
        {
            pp = NULL;    /* No parameters to check for */
            pbstr(Token); /* Will see it again */
        }

        while (istype(t = gettoken(GT_STR), C_W))
        {
            if (t == EOF)
                end_of_file(); /* Unexpected EOF */
        }
        pbstr(Token); /* Push it back */

        if (mactype)
            scaneol(); /* Scan to eol but leave eol */

        for (macctr = 0, bodyp = body, lasttok = '\n'; ((t = gettoken(GT_STR)) != 0);)
        {
            if (mactype)
            {
                if ((lasttok == '\n') && (t == '#'))
                    lasttok = '#';
                else if ((lasttok == '#') && (t == LETTER))
                {
                    if (strcmp(Token, "pragma") == EQUAL)
                    {
                        /************************************************************************/
                        /*
                         *	Possible nesting with "#pragma macro" or ending with
                         *	"#pragma endmacro".
                         */
                        if (getnstoken(GT_STR) == LETTER)
                        {
                            if (strcmp(Token, "endmacro") == EQUAL)
                            {
                                if (macctr-- == 0)
                                {
                                    scaneol();
                                    /* Remove EOL */ (void)gettoken(GT_STR);
                                    break;
                                }
                            }
                            /* Nesting? */ else if (strcmp(Token, "macro") == EQUAL)
                                macctr++;
                        }

                        pbstr(Token);
                        pushback(' ');
                        strcpy(Token, "pragma"); /* Refetch it all */
                        /************************************************************************/
                    }
                }
                else if (!istype(t, C_W))
                    /* Don't count whitespace */ lasttok = t;
            }
            else if (t == '\n')
                break; /* Give up on eol if #define */

            if (t == EOF)
                end_of_file(); /* Unexpected eof */
            else
            {
                bodyp = addstr(bodyp, &body[MACROSIZE - 1], mtlmsg, Token);
            }
        }
        pushback('\n');

        if (mactype)
        {
            while (istype(*--bodyp & 0xFF, C_W))
                ; /* Nuke trailing whitespace */
        }
        else
        {
            while ((bodyp > body) && istype(*(bodyp - 1) & 0xFF, C_W))
                bodyp--; /* Nuke trailing whitespace */
        }
        /*
         *	Because of how the preceeding searches are done the following string
         *	termination also removes the orphan "#" for the case where the last
         *	line was a "#pragma endmacro" termination ("mactype" being TRUE).
         */
        *bodyp = '\0';

        if ((sy = lookup(name, NULL)) != NULL)
        {
            if (!A_stack)
            {
                if (strcmp(sy->s_body, body) != EQUAL)
                {
                    non_fatal("Illegal redefinition of symbol: ", name);
                }
                unparam(pp);
            }
            else
            {
                if (strcmp(sy->s_body, body) != EQUAL)
                    warning("Redefining symbol: ", name);
                sbind(name, body, pp);
            }
        }
        else
            sbind(name, body, pp); /* Bind new symbol */
    }
    else
        illegal_symbol();
}

/************************************************************************/
/*									*/
/*	doerror								*/
/*									*/
/*	Process #error statement.					*/
/*									*/
/************************************************************************/

void doerror(int aaa, int bbb, const char *ccc)
{
    char *cptr1;
    char *cptr2;
    char msgbuf[MESSAGEBUFSIZE];

    cptr1 = readline(msgbuf, MESSAGEBUFSIZE, GT_STR);

    if ((*cptr1 == '"') && ((cptr2 = strrchr(++cptr1, '"')) != NULL))
    {
        *cptr2 = '\0'; /* Terminate for display */
        non_fatal(cptr1, "");
    }
    else
        non_fatal("Bad parameter to \"#error\"", "");
}

/************************************************************************/
/*									*/
/*	doundef								*/
/*									*/
/*	Process #undef statement.					*/
/*									*/
/************************************************************************/

void doundef(int aaa, int bbb, const char *ccc)
{
    if (getnstoken(GT_STR) == LETTER)
    {
        if (lookup(Token, NULL) != NULL) /* OK if symbol not defined */
            unsbind(Token);              /* Remove symbol from table */
    }
    else
        illegal_symbol();
}

/************************************************************************/
/*									*/
/*	esc_str								*/
/*									*/
/*	Helper function for strize to escape special string characters.	*/
/*									*/
/************************************************************************/

char *esc_str(char *old, int c, char *limit)
{
    if (((c == '"') || (c == '\\')) && (old < limit))
        *old++ = '\\';
    if (old < limit)
        *old++ = c;
    return (old);
}

/************************************************************************/
/*									*/
/*	fbind								*/
/*									*/
/*	Add a formal parameter to the list.				*/
/*									*/
/************************************************************************/

void fbind(struct symtab **formals, char *name, const char *value)
{
    struct symtab *temp;

    if ((temp = (struct symtab *)malloc(sizeof(struct symtab) + (unsigned)strlen(name))) == NULL)
    {
        out_of_memory();
    }
    temp->s_link = *formals;
    *formals = temp;
    strcpy(temp->s_name, name);

    if ((temp->s_body = (char *)malloc(strlen(value) + 1)) == NULL)
        out_of_memory();

    strcpy(temp->s_body, value);
}

/************************************************************************/
/*									*/
/*	flookup								*/
/*									*/
/*	Find body associated with a formal parameter.			*/
/*									*/
/************************************************************************/

char *flookup(struct symtab *formals, char *name)
{
    while (formals != NULL)
    {
        if (strcmp(formals->s_name, name) == EQUAL)
            return (formals->s_body);
        formals = formals->s_link;
    }
    return (NULL);
}

/************************************************************************/
/*									*/
/*	getparams							*/
/*									*/
/*	Get list of parameters, terminated by zero-level ')'		*/
/*	and return list as value of function.  If no parameters are	*/
/*	found, return a null parameter (NOT a null pointer.)		*/
/*									*/
/*	Parameters within [] are processed in an extended form:		*/
/*	The first LETTER token within the [] is the parameter name.	*/
/*	Subsequent LETTER tokens are taken as flag symbols that		*/
/*	define flag bits for the parameter.  Currently these are:	*/
/*									*/
/*		PN	- Preserve newline chars when substituted.	*/
/*		RQ	- Remove quote chars when substituted.		*/
/*									*/
/************************************************************************/

struct param *getparams()
{
    static const char *iffmsg = "Invalid formal parameter flag: ";

    int flags;
    struct param *lh;
    struct param *lt;
    struct param *p;
    char pname[TOKENSIZE];
    int t;

    lh = NULL;
    lt = NULL; /* For lint */

    for (;;)
    {
        if ((t = getnstoken(GT_STR)) == '[')
        {
            if (getnstoken(GT_STR) != LETTER)
            {
                p = NULL; /* For lint */
                non_fatal("Invalid formal parameter", "");
            }
            else
            {
                strcpy(pname, Token);
                flags = 0;

                for (t = getnstoken(GT_STR); t != ']'; t = getnstoken(GT_STR))
                {
                    if (t == EOF)
                        end_of_file();
                    /*
                     *	Ignore commas and whitespace.
                     */
                    if ((t == ',') || istype(t, C_W))
                        continue;

                    if (t == LETTER)
                    {
                        if (strcmp(Token, "RQ") == EQUAL)
                            flags |= PF_RQUOTES;
                        else if (strcmp(Token, "PN") == EQUAL)
                        {
                            flags |= PF_PNLINES;
                        }
                        else
                        {
                            non_fatal(iffmsg, Token);
                        }
                    }
                    else
                        non_fatal(iffmsg, Token);
                }
                p = makeparam(pname, flags);
            }
        }
        else if (t == LETTER)
            p = makeparam(Token, 0);
        else
        {
            p = NULL; /* For lint */
            break;
        }

        if (lh == NULL)
            lh = lt = p; /* Head and tail of list */
        else
        {
            lt->p_link = p; /* New tail of list */
            lt = p;         /* Mark it as end */
        }

        if ((t = getnstoken(GT_STR)) != ',')
            break;
    }

    if (t != ')')
        non_fatal("Illegal or out of place token: ", Token);

    if (lh == NULL)
        lh = makeparam("", 0); /* Make a null parameter */

    return (lh); /* Return the list */
}

/************************************************************************/
/*									*/
/*	hash								*/
/*									*/
/*	Calculate a hash function for the character string.		*/
/*									*/
/*	There is probably a better function to do this, but I am	*/
/*	simply too lazy to search for one now.  This one sums		*/
/*	the character values together.  The caller is responsible	*/
/*	for taking the sum modulo the value range desired.		*/
/*									*/
/************************************************************************/

unsigned int pphash(const char *sym)
{
    unsigned int s;
    const char *str;

    for (str = sym, s = 0; *str != '\0'; s += *str++)
        ; /* Compute sum of chars */

    return (s);
}

/************************************************************************/
/*									*/
/*	lookup								*/
/*									*/
/*	Locate name and return pointer to symtab entry.			*/
/*									*/
/************************************************************************/

struct symtab *lookup(char *name, struct symtab **pe)
{
    struct symtab *c;
    struct symtab *p;

#if PPDEBUG
    if (PPDEBUG)
        printf("lookup: %s - ", name);
#endif /* PPDEBUG */
    p = (struct symtab *)&Macros[pphash(name) & (NUMBUCKETS - 1)];
    c = p->s_link;

    while (c != NULL)
    {
        if (strcmp(c->s_name, name) == EQUAL)
        {
#if PPDEBUG
            if (PPDEBUG)
                printf("found!\n");
#endif /* PPDEBUG */
            if (pe != NULL)
                *pe = p;
            return (c);
        }
        p = c;
        c = c->s_link;
    }
#if PPDEBUG
    if (PPDEBUG)
        printf("NOT found.\n");
#endif /* PPDEBUG */

    return (NULL); /* Didn't find it */
}

/************************************************************************/
/*									*/
/*	makeparam							*/
/*									*/
/*	Generate one parameter cell and return its pointer.		*/
/*									*/
/************************************************************************/

struct param *makeparam(const char *s, int f)
{
    struct param *p;

    p = (struct param *)malloc(sizeof(struct param) + strlen(s));

    if (p == NULL)
        out_of_memory();

    strcpy(p->p_name, s); /* Copy name to parameter cell	*/
    p->p_link = NULL;     /* End of list so far		*/
    p->p_flags = f;       /* Set flags			*/
    return (p);
}

/************************************************************************/
/*									*/
/*	predef								*/
/*									*/
/*	Search through predefined table.  Return pointer to entry	*/
/*	if found, else return NULL.					*/
/*									*/
/************************************************************************/

struct ppdir *predef(char *n, struct ppdir *table)
{
    char *name;
    struct ppdir *p;

    for (name = n, p = table; p->pp_name != NULL; p++)
    {
#if PPDEBUG
        if (PPDEBUG)
            printf("predef: %s : %s\n", name, p->pp_name);
#endif /* PPDEBUG */
        if (strcmp(name, p->pp_name) == EQUAL)
        {
#if PPDEBUG
            if (PPDEBUG)
                printf("predef: found! %p\n", p->pp_func);
#endif                  /* PPDEBUG */
            return (p); /* Return table address */
        }
    }
#if PPDEBUG
    if (PPDEBUG)
        printf("predef: not found.\n");
#endif /* PPDEBUG */
    return (NULL);
}

/************************************************************************/
/*									*/
/*	sbind								*/
/*									*/
/*	Add symbol and definitions to symbol table.			*/
/*									*/
/************************************************************************/

void sbind(const char *sym, const char *defn, struct param *params)
{
    int i;
    struct symtab *p;

#if PPDEBUG
    if (PPDEBUG)
        printf("sbind: name <%s> with <%s>\n", sym, defn);
#endif /* PPDEBUG */

    p = (struct symtab *)malloc(sizeof(struct symtab) + (unsigned)strlen(sym));

    if ((p == NULL) || ((p->s_body = (char *)malloc((unsigned)strlen(defn) + 1)) == NULL))
    {
        out_of_memory();
    }

    strcpy(p->s_name, sym);
    strcpy(p->s_body, defn);
    p->s_params = params;
    p->disable = FALSE; /* Initially legal to expand */

    /* Add to symbol table */

    i = pphash(sym) & (NUMBUCKETS - 1); /* Hash value for symbol */

    p->s_link = Macros[i];
    Macros[i] = p; /* Link in to list */

    if (++Nsyms > Maxsyms)
        Maxsyms = Nsyms; /* One more symbol counted */
}

/************************************************************************/
/*									*/
/*	strize								*/
/*									*/
/*	Handle '#' (stringify) operator.				*/
/*									*/
/************************************************************************/

char *strize(char *result, char *limit, char *msg, char *snew)
{
    int c;
    int had_ws;

    while (TRUE) /* Skip whitespace */
    {
        if ((c = *snew) == EOF)
            end_of_file();
        c &= 0xFF;
        if (!(istype(c, C_W) || (c == '\n')))
            break;
        snew++;
    }

    *result++ = '"'; /* Leading " for string */

    had_ws = FALSE; /* TRUE if accumulating whitespace */
    while ((c = *snew++) != '\0')
    {
        if (c == EOF)
            end_of_file();
        c &= 0xFF;
        if (c == TOGGLE_EXPAND) /* Ignore scoping flags */
            continue;
        if (istype(c, C_W) || (c == '\n')) /* Skip inner ws */
            had_ws = TRUE;
        else if ((c == '"') || (c == '\'')) /* Str/char const? */
        {
            if (had_ws)
            {
                had_ws = FALSE;
                result = esc_str(result, ' ', limit);
            }
            result = esc_str(result, c, limit);
            while ((*snew != '\0') && (*snew != c))
            {
                if ((*snew & 0xFF) == TOGGLE_EXPAND)
                    continue;
                if (*snew == EOF)
                    end_of_file();
                if (*snew == '\\')
                    result = esc_str(result, *snew++, limit);
                if (*snew != '\0')
                    result = esc_str(result, *snew++, limit);
            }
            if (*snew != '\0')
                snew++;
            result = esc_str(result, c, limit);
        }
        else /* Everything else ... */
        {
            if (had_ws)
            {
                had_ws = FALSE;
                result = esc_str(result, ' ', limit);
            }
            if (result < limit)
                *result++ = c;
        }
    }
    if (result < limit)
        *result++ = '"';
    else
        non_fatal(msg, "");

    return (result);
}

/************************************************************************/
/*									*/
/*	unfbind								*/
/*									*/
/*	Return the formal parameters to a macro to free storage.	*/
/*									*/
/************************************************************************/

void unfbind(struct symtab *formals)
{
    struct symtab *temp;

    while (formals != NULL)
    {
        free(formals->s_body);  /* Free up parameter instantiation */
        temp = formals->s_link; /* Point at next paramter */
        free((char *)formals);  /* Free up symbol block & name */
        formals = temp;
    }
}

/************************************************************************/
/*									*/
/*	unparam								*/
/*									*/
/*	Return to free storage a macro parameter prototype chain.	*/
/*									*/
/************************************************************************/

void unparam(struct param *pp)
{
    struct param *npp;

    for (; pp != NULL; pp = npp)
    {
        npp = pp->p_link; /* Save next address */
        free((char *)pp); /* Free this memory */
    }
}

/************************************************************************/
/*									*/
/*	unsbind								*/
/*									*/
/*	Look up the symbol and remove its entry from the symbol table.	*/
/*									*/
/************************************************************************/

void unsbind(char *sym)
{
    struct symtab *p;
    struct symtab *s;

#if PPDEBUG
    if (PPDEBUG)
        printf("unsbind: %s\n", sym);
#endif /* PPDEBUG */

    if ((s = lookup(sym, &p)) == NULL)
        non_fatal("Symbol not defined: ", sym);
    else
    {
        if (s->s_body != NULL)
            free(s->s_body);   /* Free macro body */
        unparam(s->s_params);  /* Free parameter cells */
        p->s_link = s->s_link; /* Remove item from list */
        free((char *)s);       /* Free the symbol table entry */

        Nsyms--; /* One less symbol */
    }
}

#pragma GCC diagnostic pop
