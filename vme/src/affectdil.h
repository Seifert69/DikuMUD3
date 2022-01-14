/*
 $Author: tperry $
 $RCSfile: affectdil.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"

/* affect a units values                                           */
struct apply_function_type
{
    char *descr;

    ubit1 (*func)(class unit_affected_type *af, class unit_data *unit, ubit1 set);
};

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
    char *descr;
    void (*func)(class unit_affected_type *af, class unit_data *unit);
};
