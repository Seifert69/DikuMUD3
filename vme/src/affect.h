/*
 $Author: tperry $
 $RCSfile: affect.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"

/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
    const char *descr;

    ubit1 (*func)(class unit_affected_type *af, class unit_data *unit, ubit1 set);
};

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
    const char *descr;
    void (*func)(class unit_affected_type *af, class unit_data *unit);
};

void affect_beat(void *, void *);
void affect_clear_unit(class unit_data *unit);
unit_affected_type *affected_by_spell(const class unit_data *unit, sbit16 id);
void apply_affect(class unit_data *unit);
void create_affect(class unit_data *unit, class unit_affected_type *orgaf);
void destroy_affect(class unit_affected_type *af);
unit_affected_type *link_alloc_affect(class unit_data *unit, class unit_affected_type *orgaf);
void start_affect(class unit_data *unit);
void stop_affect(class unit_data *unit);
void unlink_affect(class unit_affected_type *af);
