/*
 $Author: tperry $
 $RCSfile: affect.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */
#pragma once

#include "essential.h"
#include "structs.h"

/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
    const char *descr;

    ubit1 (*func)(unit_affected_type *af, unit_data *unit, ubit1 set);
};

/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
    const char *descr;
    void (*func)(unit_affected_type *af, unit_data *unit);
};

void affect_beat(void *, void *);
void affect_clear_unit(unit_data *unit);
unit_affected_type *affected_by_spell(const unit_data *unit, sbit16 id);
void apply_affect(unit_data *unit);
void create_affect(unit_data *unit, unit_affected_type *orgaf);
void destroy_affect(unit_affected_type *af);
unit_affected_type *link_alloc_affect(unit_data *unit, unit_affected_type *orgaf);
void start_affect(unit_data *unit);
void stop_affect(unit_data *unit);
void unlink_affect(unit_affected_type *af);
void get_affects(const unit_data *unit, cNamelist *sl);
