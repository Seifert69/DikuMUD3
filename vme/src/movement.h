/*
 $Author: All $
 $RCSfile: movement.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.3 $
 */

#ifndef _MUD_MOVEMENT_H
#define _MUD_MOVEMENT_H
int move_to(unit_data *from, unit_data *to);

#include "interpreter.h"

#define ROOM_DOOR_NAME(room, dir) (ROOM_EXIT((room), (dir))->open_name.Name() ? ROOM_EXIT((room), (dir))->open_name.Name() : "UNDEFINED")

#define MOVE_GOAL 0   /* The NPC is now at it's destination         */
#define MOVE_CLOSER 1 /* The NPC was moved closer to destination    */
#define MOVE_DOOR 2   /* The NPC is working on a door/lock/room     */
#define MOVE_BUSY 3   /* The NPC is busy and can't do anything yet  */
#define MOVE_FAILED 4 /* It is currently impossible to reach desti. */
#define MOVE_DEAD 5   /* The NPC is dead, abort now!                */

#define DESTROY_ME 12

struct door_data
{
    class unit_data *thing;
    class unit_data *room;
    class unit_data *reverse; /* Reverse Room or Inside thing */
    char *key;
    ubit8 direction; /* For rooms, which direction was picked? */
    ubit8 *flags;
    ubit8 *rev_flags;
    const char *name;
};

struct visit_data
{
    int state;
    class unit_data *go_to;

    class unit_data *start_room;
    class unit_data *dest_room;

    /* Return DESTROY_ME to destroy moving function        */
    /*        SFR_SHARE to allow lower functions to handle */
    /*        SFR_SUPREME to not allow lower functions     */
    int (*what_now)(const class unit_data *, struct visit_data *);

    void *data;
    int non_tick_return; /* What to return upon non-ticks (SFR_...) */
};

void npc_set_visit(class unit_data *npc,
                   class unit_data *dest_room,
                   int what_now(const class unit_data *, struct visit_data *),
                   void *data,
                   int non_tick_return);

#endif /* _MUD_MOVEMENT_H */
