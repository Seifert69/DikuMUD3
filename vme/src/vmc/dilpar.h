/*
 $Author: tperry $
 $RCSfile: dilpar.h,v $
 $Date: 2002/06/09 23:27:41 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"
#include "dil.h"

#define DIL_VERSION "v5.10"

#define SYMSIZE 40

/* This is for the compiler */

#define CODESIZE 16768 /* maximum expression size */

struct exptype
{
    ubit8 dsl; ///< static/dynamic/lvalue, type, req type
    DilVarType_e typ; ///< static/dynamic/lvalue, type, req type
    DilVarType_e rtyp; ///< static/dynamic/lvalue, type, req type
    ubit8 boolean;        ///< if expression contains boolean op
    sbit32 num;           ///< static value INT or field type
    ubit8 *code;          ///< code / static value SP
    ubit8 *codep;         ///< code write pointer
};

struct dilref
{
    char *zname; ///< func/proc name
    char *name;  ///< zone name
    DilVarType_e rtnt;  ///< return type
    ubit8 argc;  ///< number of arguments (min 1)
    DilVarType_e *argt; ///< argument types
    char **argv; ///< argument names
};
