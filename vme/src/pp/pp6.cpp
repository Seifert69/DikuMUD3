#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wredundant-tags"

/************************************************************************/
/*									*/
/*	File:	pp6.c							*/
/*									*/
/*	Pragma directives.						*/
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
/*		doasm		Process #asm/#endasm directives.	*/
/*		dopragma	Main #pragma directive processing.	*/
/*		pragasm		Process #pragma asm/endasm directives.	*/
/*		pragerror	Process #pragma error directive.	*/
/*		pragmsg		Process #pragma message directive.	*/
/*		pragopt		Process #pragma <option> directives.	*/
/*		pragvalue	Process #pragma value directive.	*/
/*									*/
/************************************************************************/

#include "pp.h"

#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
/************************************************************************/
/*									*/
/*	doasm								*/
/*									*/
/*	Copy "#asm" or "#endasm" to the output.  This is basically a	*/
/*	function to clean up for the "#asm"/"#endasm" previously pushed	*/
/*	by the "pragasm" function.					*/
/*									*/
/************************************************************************/

void doasm(int asmtype)
/* True if #asm; False if #endasm */
{
    puttoken(asmtype ? "#asm" : "#endasm");
}
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */

/************************************************************************/
/*									*/
/*	dopragma							*/
/*									*/
/*	Execute the #pragma directive.					*/
/*									*/
/*	Any unrecognized #pragma lines are passed on to the output file	*/
/*	unless in the FALSE clause of an #if (or equivalent).		*/
/*									*/
/************************************************************************/

void dopragma(int aaa, int bbb, const char *ccc)
{
    int ltr_flag;
    int no_flag;
    struct ppdir *pp;
    char pragbuf[PRAGBUFSIZE];

    no_flag = FALSE; /* "no" not encountered */

    if ((ltr_flag = (getnstoken(GT_STR) == LETTER)) != 0)
    {
        if (strcmp(Token, "no") == EQUAL)
        {
            no_flag = TRUE;
            ltr_flag = (getnstoken(GT_STR) == LETTER);
        }
        if (ltr_flag && ((pp = predef(Token, pragtab)) != NULL))
        {
            /* If unconditionally do it or if emitting code */
            if (pp->pp_ifif || (Ifstate == IFTRUE))
            {
                (void)(*(pp->pp_func))(pp->pp_arg, no_flag, pp->pp_name);
            }
            return;
        }
    }
    /*
     *	We do not understand: print original (or similar) #pragma to output
     *	if we are emitting code.
     */
    if (Ifstate == IFTRUE)
    {
        puttoken("#pragma "); /* Write #pragma directive */
        if (no_flag)
            puttoken("no ");
        puttoken(Token); /* Unrecognized token */
        puttoken(" ");   /* Whitespace separating */
        /*
         *	Expand macros...
         */
        puttoken(readline(pragbuf, PRAGBUFSIZE, GT_STR));
    }
}

#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
/************************************************************************/
/*									*/
/*	pragasm								*/
/*									*/
/*	Copy code verbatum to output file until a line beginning with	*/
/*	the token "#pragma endasm" is read.				*/
/*									*/
/************************************************************************/

void pragasm(int asmtype, int bbb, char *ccc)
{
    if (Do_asm == asmtype)
    {
        if (Do_asm)
            non_fatal("Already within \"#pragma asm\"", "");
        else
            non_fatal("Not within \"#pragma asm\"", "");
    }
    else
    {
        pbstr(Do_asm ? "#endasm" : "#asm");     /* For 2nd pass */
        pushback('\n');                         /* So scaneol finds the end */
        Do_asm = asmtype;                       /* Set flag and let main line handle */
        Macexpand = asmtype ? Asmexpand : TRUE; /* Set expand mode */
    }
}
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */

/************************************************************************/
/*									*/
/*	pragendm							*/
/*									*/
/*	Process "#pragma endmacro" statement as error outside macro	*/
/*									*/
/************************************************************************/

void pragendm(int aaa, int bbb, const char *ccc)
{
    non_fatal("\"#pragma endmacro\" illegal outside macro", "");
}

/************************************************************************/
/*									*/
/*	pragerror							*/
/*									*/
/*	Process "#pragma error" statement.				*/
/*									*/
/************************************************************************/

void pragerror(int aaa, int bbb, const char *ccc)
{
    char msgbuf[MESSAGEBUFSIZE];

    non_fatal(readline(msgbuf, MESSAGEBUFSIZE, GT_STR), "");
}

/************************************************************************/
/*									*/
/*	pragmsg								*/
/*									*/
/*	Process "#pragma message" statement.				*/
/*									*/
/*	Note:	If the file stack is < 0, then all files have been	*/
/*		popped off the stack.  Use 0 (the original file name.)	*/
/*									*/
/************************************************************************/

void pragmsg(int aaa, int bbb, const char *ccc)
{
    char msgbuf[MESSAGEBUFSIZE];

    printf("<%s> @ %u: MESSAGE: %s\n", Filestack[Filelevel >= 0 ? Filelevel : 0]->f_name, LLine, readline(msgbuf, MESSAGEBUFSIZE, GT_STR));
}

/************************************************************************/
/*									*/
/*	pragopt								*/
/*									*/
/*	Process "#pragma [no] <option>"			 statements.	*/
/*									*/
/************************************************************************/

#ifdef __TURBOC__
    #pragma warn - par
#endif /* __TURBOC__ */

void pragopt(int dummy, int no_flag, const char *name)
{
    char buf[TOKENSIZE + 1];
    struct symtab *sym;
    char *toptr;

    if (strcmp(name, "arg_string") == EQUAL)
        A_astring = !no_flag;
#if (TARGET == T_QC) OR(TARGET == T_QCX) OR(TARGET == T_TCX)
    else if (strcmp(name, "asm_expand") == EQUAL)
        Asmexpand = !no_flag;
#endif /* (TARGET == T_QC) OR (TARGET == T_QCX) OR (TARGET == T_TCX) */
    else if (strcmp(name, "comment_recurse") == EQUAL)
        A_crecurse = !no_flag;
    else if (strcmp(name, "eol_comment") == EQUAL)
        A_eolcomment = !no_flag;
    else if (strcmp(name, "macro_rescan") == EQUAL)
        A_rescan = !no_flag;
    else if (strcmp(name, "macro_stack") == EQUAL)
        A_stack = !no_flag;
    else if (strcmp(name, "trigraph") == EQUAL)
        A_trigraph = !no_flag;
    /*
     *	We need to keep track of the current setting of the options via
     *	the appropriate "__<uppercase option name>__" macro.  First we
     *	synthesize the macro name.
     */
    strcpy(buf, "__");
    for (toptr = &buf[2]; *name; name++)
        *toptr++ = (islower(*name) ? toupper(*name) : *name);
    *toptr = '\0';
    strcat(buf, "__");
    /*
     *	Now see if it still exists and if its value hasn't been altered
     *	beyond use by the user doing a undef/define sequence on it.
     */
    if ((sym = lookup(buf, NULL)) != NULL)
    {
        if ((sym->s_body != NULL) && ((strcmp(sym->s_body, "0") == EQUAL) || (strcmp(sym->s_body, "1") == EQUAL)))
        {
            if (!no_flag)
                *(sym->s_body) = '1';
            else
                *(sym->s_body) = '0';
        }
    }
}

#ifdef __TURBOC__
    #pragma warn.par
#endif /* __TURBOC__ */

/************************************************************************/
/*									*/
/*	pragvalue							*/
/*									*/
/*	Process "#pragma value" statement.				*/
/*									*/
/************************************************************************/

void pragvalue(int aaa, int bbb, const char *ccc)
{
    char buf[TOKENSIZE + 1];

    sprintf(buf, "%ld", (long)eval());
    pbstr(buf);     /* Push back into "input" stream */
    pushback('\n'); /* So scaneol finds the end */
}

#pragma GCC diagnostic pop
