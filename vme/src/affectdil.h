/*
 $Author: tperry $
 $RCSfile: affectdil.h,v $
 $Date: 2001/04/29 03:46:05 $
 $Revision: 2.1 $
 */

#ifndef _MUD_AFFECT_H
#define _MUD_AFFECT_H

void apply_affect (class unit_data * unit);
void start_affect (class unit_data * unit);
void stop_affect (class unit_data * unit);
class unit_affected_type *affected_by_spell (const class unit_data * unit,
        sbit16 id);

void create_affect (class unit_data * unit, class unit_affected_type *orgaf);
void destroy_affect (class unit_affected_type *af);
void affect_clear_unit (class unit_data * unit);


/* These functions may not send messages - nor destroy units. Only */
/* affect a units values                                           */
struct apply_function_type
{
    char *descr;


    ubit1 (*func) (class unit_affected_type * af, class unit_data * unit, ubit1 set);
};


/* These functions may send messages and destroy any unit. */
struct tick_function_type
{
    char *descr;
    void (*func) (class unit_affected_type * af, class unit_data * unit);
};

#endif /* _MUD_AFFECT_H */
