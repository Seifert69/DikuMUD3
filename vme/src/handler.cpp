/*
 $Author: All $
 $RCSfile: handler.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#include "handler.h"

#include "affect.h"
#include "char_follow_type.h"
#include "comm.h"
#include "db.h"
#include "dilrun.h"
#include "files.h"
#include "interpreter.h"
#include "main_functions.h"
#include "mobact.h"
#include "money.h"
#include "nanny.h"
#include "slog.h"
#include "spec_assign.h"
#include "unit_affected_type.h"
#include "unit_fptr.h"
#include "utils.h"
#include "zon_basis.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

descriptor_data *unit_is_edited(unit_data *u)
{
    descriptor_data *d = nullptr;

    for (d = g_descriptor_list; d; d = d->getNext())
    {
        if (d->cgetEditing() == u)
        {
            return d;
        }
    }

    return nullptr;
}

/* By using this, we can easily sort the list if ever needed */
void insert_in_unit_list(unit_data *u)
{
    assert(u->getGlobalNext() == nullptr && u->getGlobalPrevious() == nullptr && g_unit_list != u);

    if (u->getFileIndex())
    {
        u->getFileIndex()->PushFront(u);
    }

    unit_data *tmp_u = nullptr;

    if (!g_unit_list)
    {
        u->setGlobalNext(nullptr);
        u->setGlobalPrevious(nullptr);
        g_unit_list = u;
        g_npc_head = u;
        g_room_head = u;
        g_obj_head = u;
        return;
    }

    switch (u->getUnitType())
    {
        case UNIT_ST_PC:
        {
            tmp_u = g_unit_list;
            u->setGlobalNext(tmp_u);
            u->setGlobalPrevious(tmp_u->getGlobalPrevious());
            tmp_u->setGlobalPrevious(u);

            if (tmp_u == g_unit_list)
            {
                g_unit_list = u;
            }
            break;
        }
        case UNIT_ST_NPC:
        {
            if (g_npc_head->getUnitType() != UNIT_ST_NPC)
            {
                tmp_u = g_unit_list;
                for (; tmp_u && tmp_u->isPC(); tmp_u = tmp_u->getGlobalNext())
                {
                    ;
                }
            }
            else
            {
                tmp_u = g_npc_head;
            }

            u->setGlobalNext(tmp_u);
            u->setGlobalPrevious(tmp_u->getGlobalPrevious());
            if (tmp_u->getGlobalPrevious())
            {
                tmp_u->getGlobalPrevious()->setGlobalNext(u);
            }
            tmp_u->setGlobalPrevious(u);

            if (tmp_u == g_unit_list)
            {
                g_unit_list = u;
            }
            g_npc_head = u;
            break;
        }
        case UNIT_ST_OBJ:
        {
            if (g_obj_head->getUnitType() != UNIT_ST_OBJ)
            {
                tmp_u = g_unit_list;
                for (; tmp_u && tmp_u->isChar(); tmp_u = tmp_u->getGlobalNext())
                {
                    ;
                }
            }
            else
            {
                tmp_u = g_obj_head;
            }

            u->setGlobalNext(tmp_u);
            u->setGlobalPrevious(tmp_u->getGlobalPrevious());
            if (tmp_u->getGlobalPrevious())
            {
                tmp_u->getGlobalPrevious()->setGlobalNext(u);
            }
            tmp_u->setGlobalPrevious(u);

            if (tmp_u == g_unit_list)
            {
                g_unit_list = u;
            }
            g_obj_head = u;
            break;
        }
        case UNIT_ST_ROOM:
        {
            if (g_room_head->getUnitType() != UNIT_ST_ROOM)
            {
                tmp_u = g_unit_list;
                for (; tmp_u && (tmp_u->isChar() || tmp_u->isObj()); tmp_u = tmp_u->getGlobalNext())
                {
                    ;
                }
            }
            else
            {
                tmp_u = g_room_head;
            }

            u->setGlobalNext(tmp_u);
            u->setGlobalPrevious(tmp_u->getGlobalPrevious());
            if (tmp_u->getGlobalPrevious())
            {
                tmp_u->getGlobalPrevious()->setGlobalNext(u);
            }
            tmp_u->setGlobalPrevious(u);

            if (tmp_u == g_unit_list)
            {
                g_unit_list = u;
            }
            g_room_head = u;
            break;
        }
    }
}

/* Remove a unit from the g_unit_list */
void remove_from_unit_list(unit_data *unit)
{
    assert(unit->getGlobalPrevious() || unit->getGlobalNext() || (g_unit_list == unit));

    if (unit->getFileIndex())
    {
        unit->getFileIndex()->Remove(unit);
    }

    if (g_npc_head == unit)
    {
        g_npc_head = unit->getGlobalNext();
    }
    if (g_obj_head == unit)
    {
        g_obj_head = unit->getGlobalNext();
    }
    if (g_room_head == unit)
    {
        g_room_head = unit->getGlobalNext();
    }
    if (g_unit_list == unit)
    {
        g_unit_list = unit->getGlobalNext();
    }
    else
    { /* Then this is always true 'if (unit->gprevious)'  */
        unit->getGlobalPrevious()->setGlobalNext(unit->getGlobalNext());
    }

    if (unit->getGlobalNext())
    {
        unit->getGlobalNext()->setGlobalPrevious(unit->getGlobalPrevious());
    }

    unit->setGlobalNext(nullptr);
    unit->setGlobalPrevious(nullptr);
}

unit_fptr *find_fptr(unit_data *u, ubit16 idx)
{
    unit_fptr *tf = nullptr;

    for (tf = u->getFunctionPointer(); tf; tf = tf->getNext())
    {
        if (tf->getFunctionPointerIndex() == idx)
        {
            return tf;
        }
    }

    return nullptr;
}

// 2020: Add it prioritized
void insert_fptr(unit_data *u, unit_fptr *f)
{
    if (f->getFunctionPriority() == 0)
    {
        slog(LOG_ALL, 0, "fptr_priotity not set, setting to chores");
        f->setFunctionPriority(FN_PRI_CHORES);
    }

    // If there are no funcs, just add it.
    if (u->getFunctionPointer() == nullptr)
    {
        f->setNext(u->getFunctionPointer());
        u->setFunctionPointer(f);
        return;
    }

    // See if we are higher priority than head element
    if (f->getFunctionPriority() < u->getFunctionPointer()->getFunctionPriority())
    {
        f->setNext(u->getFunctionPointer());
        u->setFunctionPointer(f);
        return;
    }

    unit_fptr *p = nullptr;
    unit_fptr *prev = nullptr;

    // Find location to insert
    prev = u->getFunctionPointer();
    for (p = u->getFunctionPointer()->getNext(); p; p = p->getNext())
    {
        if (f->getFunctionPriority() < p->getFunctionPriority())
        {
            f->setNext(p);
            prev->setNext(f);
            return;
        }
        prev = p;
    }

    // We are the lowest priority, insert at the
    assert(prev->getNext() == nullptr);
    prev->setNext(f);
    f->setNext(nullptr);
}

unit_fptr *create_fptr(unit_data *u, ubit16 index, ubit16 priority, ubit16 beat, ubit16 flags, void *data)
{
    unit_fptr *f = nullptr;
    f = new EMPLACE(unit_fptr) unit_fptr;

    assert(f);
    assert(!f->is_destructed());

    f->setFunctionPointerIndex(index);
    f->setFunctionPriority(priority);
    f->setHeartBeat(beat);
    f->setAllActivateOnEventFlags(flags);
    f->setData(data);
    f->setEventQueue(nullptr);

    membug_verify(f->data);
    insert_fptr(u, f);

    start_special(u, f);

    membug_verify(f);
    membug_verify(f->data);

    return f;
}

/* Does not free 'f' - it is done by clear_destruct by comm.c */
void destroy_fptr(unit_data *u, unit_fptr *f)
{
    unit_fptr *tf = nullptr;
    spec_arg sarg;

    assert(f);
    if (f->is_destructed())
    {
        return;
    }

    assert(!f->is_destructed());

    f->register_destruct();

#ifdef DEBUG_HISTORY
    add_func_history(u, f->getFunctionPointerIndex(), 0);
#endif

    sarg.owner = u;
    sarg.activator = nullptr;
    sarg.medium = nullptr;
    sarg.target = nullptr;
    sarg.pInt = nullptr;
    sarg.fptr = f;
    sarg.cmd = &g_cmd_auto_extract;
    sarg.arg = "";
    sarg.mflags = ((ubit16)0);

    (*(g_unit_function_array[f->getFunctionPointerIndex()].func))(&sarg);

    /* Data is free'ed in destruct() if it is not NULL now */

    membug_verify(f);
    stop_special(u, f);
    membug_verify(f);

    /* Only unlink function, do not free it! */
    if (u->getFunctionPointer() == f)
    {
        u->setFunctionPointer(f->getNext());
    }
    else
    {
        for (tf = u->getFunctionPointer(); tf && (tf->getNext() != f); tf = tf->getNext())
        {
            ;
        }
        if (tf)
        {
            assert(tf->getNext() == f);
            tf->setNext(f->getNext());
        }
    }

    f->setNext(nullptr);
    assert(f->getEventQueue() == nullptr);
}

/* Stop the 'ch' from following his master    */
/* Call die_follower if a person dies         */
void stop_following(unit_data *ch)
{
    char_follow_type *j = nullptr;
    char_follow_type *k = nullptr;

    assert(CHAR_MASTER(ch));

    if (CHAR_FOLLOWERS(CHAR_MASTER(ch))->getFollower() == ch) /* Head of list? */
    {
        k = CHAR_FOLLOWERS(CHAR_MASTER(ch));
        UCHAR(CHAR_MASTER(ch))->setFollowers(k->getNext());
        DELETE(char_follow_type, k);
    }
    else
    { /* locate follower who is not head of list */
        for (k = CHAR_FOLLOWERS(CHAR_MASTER(ch)); k->getNext()->getFollower() != ch; k = k->getNext())
        {
            ;
        }
        j = k->getNext();
        k->setNext(j->getNext());
        DELETE(char_follow_type, j);
    }

    UCHAR(ch)->setMaster(nullptr);

    send_done(ch, nullptr, nullptr, 0, g_cmd_follow, "");
}

/* Set 'ch' to follow leader. Circles allowed. */
void start_following(unit_data *ch, unit_data *leader)
{
    char_follow_type *k = nullptr;

    assert(!leader->is_destructed());
    assert(!ch->is_destructed());

    UCHAR(ch)->removeCharacterFlag(CHAR_GROUP);
    if (CHAR_MASTER(ch))
    {
        stop_following(ch);
    }
    UCHAR(ch)->setMaster(leader);
    k = new EMPLACE(char_follow_type) char_follow_type;
    k->setFollower(ch);
    k->setNext(CHAR_FOLLOWERS(leader));
    UCHAR(leader)->setFollowers(k);

    send_done(ch, nullptr, leader, 0, g_cmd_follow, "");
}

/* Called by extract_unit when a character that follows/is followed dies */
void die_follower(unit_data *ch)
{
    char_follow_type *j = nullptr;
    char_follow_type *k = nullptr;

    if (CHAR_MASTER(ch))
    {
        stop_following(ch);
    }

    for (k = CHAR_FOLLOWERS(ch); k; k = j)
    {
        j = k->getNext();
        stop_following(k->getFollower());
    }
}

/*
   An NPC can see if the sum of this is >= 0

           Lights of the unit itself
    plus   Light if it is daylight (depends on time of day)
    plus   Lights of the unit it is inside

ubit1 unit_islight(class unit_data *u)
{
    if (!u)
        return 0;

    return (UNIT_LIGHTS(unit) + UNIT_OUTSIDE_LIGHT(unit) + ((UNIT_IN(unit)?UNIT_LIGHTS(UNIT_IN(unit)):0)) >= 0;
}
*/

/* MS2020. Trying to remember how this worked:

    BRIGHT: How much a unit shines wtih light/dark afecting what it is in
    LIGHTS: How much light is inside a unit (not how much it shines out)
    ILLUM:

Example: (Lights, Bright, Illum)
    Room: (-1,0,0)
    NPC in room: (0,0,0)
    Lantern in room: (0,0,0)
    Coffin in room (0,0,0)
    Lantern is lit (B:+2, 0, 0)
       Room now has L:1, B:0
    NPC takes lantern. Room: L-1. NPC: L:2, B:0
    NPC equips lantern Room: L+1. NPC: L:2, B:2

    What about NPC in coffin when it is open & closed?
    What about room in room when it is open & closed? (barrel)

    coffin@midgaard & horse@midgaard are both transparent

    Coffin (0,0,0)
       Cast light coffin (0,B1,0)
    If you cast light on a coffin, the coffin shines (bright).
    But if you crawl inside and close the lid it is still dark.


*/
/* Call this routine if you modify the brightness of a unit */
/* in order to correctly update the environment of the unit */
void modify_bright(unit_data *unit, int bright)
{
    unit_data *ext = nullptr;
    unit_data *in = nullptr;

    unit->changeLightOutputBy(bright);

    if ((in = unit->getUnitIn()))
    { /* Light up what the unit is inside */
        in->changeNumberOfActiveLightSourcesBy(bright);
    }

    if (unit->isObj() && OBJ_EQP_POS(unit))
    {
        /* The char holding the torch light up the SAME way the torch does! */
        /* this works with the equib/unequib functions. This is NOT a case  */
        /* of transparancy.  */
        modify_bright(in, bright);
    }
    else if (in && in->isTransparent())
    {
        /* the unit is inside a transperant unit, so it lights up too */
        /* this works with actions in unit-up/down                    */
        in->changeLightOutputBy(bright);
        if ((ext = in->getUnitIn()))
        {
            ext->changeNumberOfActiveLightSourcesBy(bright);
            in->changeTransparentLightOutputBy(bright);
        }
    }
}

void trans_set(unit_data *u)
{
    unit_data *u2 = nullptr;
    int sum = 0;

    for (u2 = u->getUnitContains(); u2; u2 = u2->getNext())
    {
        sum += u2->getLightOutput();
    }

    u->setTransparentLightOutput(sum);
    u->changeLightOutputBy(sum);

    if (u->getUnitIn())
    {
        u->getUnitIn()->changeNumberOfActiveLightSourcesBy(sum);
    }
}

void trans_unset(unit_data *u)
{
    u->changeLightOutputBy(-1 * u->getTransparentLightOutput());

    if (u->getUnitIn())
    {
        u->getUnitIn()->changeNumberOfActiveLightSourcesBy(-1 * u->getTransparentLightOutput());
    }

    u->setTransparentLightOutput(0);
}

unit_data *equipment(unit_data *ch, ubit8 pos)
{
    unit_data *u = nullptr;

    assert(ch->isChar());

    for (u = ch->getUnitContains(); u; u = u->getNext())
    {
        if (u->isObj() && pos == OBJ_EQP_POS(u))
        {
            return u;
        }
    }

    return nullptr;
}

/* The following functions find armor / weapons on a person with     */
/* type checks (i.e. trash does not protect!).                       */
unit_data *equipment_type(unit_data *ch, int pos, ubit8 type)
{
    unit_data *obj = nullptr;

    obj = equipment(ch, pos);

    if (obj && OBJ_TYPE(obj) == type)
    {
        return obj;
    }
    else
    {
        return nullptr;
    }
}

void equip_char(unit_data *ch, unit_data *obj, ubit8 pos)
{
    unit_affected_type *af = nullptr;
    unit_affected_type newaf;

    assert(pos > 0 && obj->isObj() && ch->isChar());
    assert(!equipment(ch, pos));
    assert(obj->getUnitIn() == ch); /* Must carry object in inventory */

    UOBJ(obj)->setEquipmentPosition(pos);
    modify_bright(ch, obj->getLightOutput()); /* Update light sources */

    for (af = obj->getUnitAffected(); af; af = af->getNext())
    {
        if (af->getID() < 0) /* It is a transfer affect! */
        {
            newaf = *af;
            newaf.setID(newaf.getID() * -1); /* No longer a transfer    */
            newaf.setDuration(-1);           /* Permanent until unequip */
            create_affect(ch, &newaf);
        }
    }
}

unit_data *unequip_object(unit_data *obj)
{
    unit_data *ch = nullptr;
    unit_affected_type *af = nullptr;
    unit_affected_type *caf = nullptr;

    ch = obj->getUnitIn();

    assert(obj->isObj() && OBJ_EQP_POS(obj));
    assert(ch->isChar());

    UOBJ(obj)->setEquipmentPosition(0);
    modify_bright(ch, -obj->getLightOutput()); /* Update light sources */

    for (af = obj->getUnitAffected(); af; af = af->getNext())
    {
        if (af->getID() < 0) /* It is a transfer affect! */
        {
            for (caf = ch->getUnitAffected(); caf; caf = caf->getNext())
            {
                if ((-caf->getID() == af->getID()) && (caf->getDuration() == -1) && (caf->getDataAtIndex(0) == af->getDataAtIndex(0)) &&
                    (caf->getDataAtIndex(1) == af->getDataAtIndex(1)) &&
                    // THIS IS NOT TESTED! (caf->data[2] == af->data[2]) &&
                    (caf->getApplyFI() == af->getApplyFI()) && (caf->getFirstFI() == af->getFirstFI()) &&
                    (caf->getLastFI() == af->getLastFI()) && (caf->getTickFI() == af->getTickFI()))
                {
                    destroy_affect(caf);
                    break; /* Skip inner for loop since we found the affect */
                }
            }
        }
    }

    return obj;
}

/* Checks if a unit which is to be put inside another unit will pass */
/* through itself. For example if Papi carries a coffin and God is   */
/* inside the coffin, when God types trans Papi this function will   */
/* return TRUE                                                       */
int unit_recursive(unit_data *from, unit_data *to)
{
    unit_data *u = nullptr;

    for (u = to; u; u = u->getUnitIn())
    {
        if (u == from)
        {
            return TRUE;
        }
    }

    return FALSE;
}

zone_type *unit_zone(const unit_data *unit)
{
    unit_data *org = (unit_data *)unit;

    for (; unit; unit = unit->getUnitIn())
    {
        if (!unit->getUnitIn())
        {
            //      assert(IS_ROOM(unit));
            if (!unit->isRoom())
            {
                slog(LOG_ALL, 0, "ZONE: FATAL(1): %s IN NO ROOMS WHILE NOT A ROOM!!", org->getFileIndexSymName());
                return nullptr;
            }
            return unit->getFileIndex()->getZone();
        }
    }

    slog(LOG_ALL, 0, "ZONE: FATAL(2): %s IN NO ROOMS WHILE NOT A ROOM!!", org->getFileIndexSymName());
    return nullptr;
}

//
// Returns a symname text string of all the units a unit is in
//
std::string unit_trace_up(unit_data *unit)
{
    unit_data *u = nullptr;
    std::string s = unit->getFileIndexSymName();
    std::string t;

    for (u = unit->getUnitIn(); u; u = u->getUnitIn())
    {
        t = " in ";
        t.append(u->getFileIndexSymName());
        s.append(t);
    }

    return s;
}

unit_data *unit_room(unit_data *unit)
{
    if (unit == nullptr)
    {
        return nullptr;
    }

    unit_data *org = unit;

    for (; unit; unit = unit->getUnitIn())
    {
        if (unit->isRoom())
        {
            return unit;
        }
    }

    slog(LOG_ALL, 0, "ROOM: FATAL(3): %s IN NO ROOMS WHILE NOT A ROOM!!", org->getFileIndexSymName());
    return nullptr;
}

void intern_unit_up(unit_data *unit, ubit1 pile)
{
    unit_data *u = nullptr;
    unit_data *in = nullptr;
    unit_data *toin = nullptr;
    unit_data *extin = nullptr;
    sbit8 bright = 0;
    sbit8 selfb = 0;

    assert(unit->getUnitIn());

    /* resolve *ALL* light!!! */
    in = unit->getUnitIn();                             /* where to move unit up to */
    toin = in->getUnitIn();                             /* unit around in           */
    extin = toin ? toin->getUnitIn() : nullptr;         /* unit around toin         */
    bright = unit->getLightOutput();                    /* brightness inc. trans    */
    selfb = bright - unit->getTransparentLightOutput(); /* brightness excl. trans   */

    in->changeNumberOfActiveLightSourcesBy(-1 * bright); // Subtract Light
    if (in->isTransparent())
    {
        in->changeTransparentLightOutputBy(-1 * selfb);
        in->changeLightOutputBy(-1 * selfb);
    }
    else if (toin)
    {
        toin->changeNumberOfActiveLightSourcesBy(bright);
    }

    if (toin && toin->isTransparent())
    {
        toin->changeLightOutputBy(selfb);
        toin->changeTransparentLightOutputBy(selfb);
        if (extin)
        {
            extin->changeNumberOfActiveLightSourcesBy(selfb);
        }
    }

    if (unit->isChar())
    {
        unit->getUnitIn()->decrementNumberOfCharactersInsideUnit();
    }
    /*fuck*/
    unit->getUnitIn()->reduceWeightBy(unit->getWeight());

    if (unit == unit->getUnitIn()->getUnitContains())
    {
        unit->getUnitIn()->setUnitContains(unit->getNext());
    }
    else
    {
        for (u = unit->getUnitIn()->getUnitContains(); u->getNext() != unit; u = u->getNext())
        {
            ;
        }
        u->setNext(unit->getNext());
    }

    unit->setNext(nullptr);

    unit->setUnitIn(unit->getUnitIn()->getUnitIn());
    if (unit->getUnitIn())
    {
        unit->setNext(unit->getUnitIn()->getUnitContains());
        unit->getUnitIn()->setUnitContains(unit);
        if (unit->isChar())
        {
            unit->getUnitIn()->incrementNumberOfCharactersInsideUnit();
        }
    }

    if (pile && IS_MONEY(unit) && unit->getUnitIn())
    {
        pile_money(unit);
    }
}

void unit_up(unit_data *unit)
{
    intern_unit_up(unit, TRUE);
}

void unit_from_unit(unit_data *unit)
{
    while (unit->getUnitIn())
    {
        intern_unit_up(unit, FALSE);
    }
}

void intern_unit_down(unit_data *unit, unit_data *to, ubit1 pile)
{
    unit_data *u = nullptr;
    unit_data *in = nullptr;
    unit_data *extin = nullptr;
    sbit8 bright = 0;
    sbit8 selfb = 0;

    assert(unit->getUnitIn() == to->getUnitIn());
    assert(unit != to);

    /* do *ALL* light here!!!! */
    in = unit->getUnitIn();
    extin = in ? in->getUnitIn() : nullptr;
    bright = unit->getLightOutput();
    selfb = bright - unit->getTransparentLightOutput();

    to->changeNumberOfActiveLightSourcesBy(bright);
    if (to->isTransparent())
    {
        to->changeLightOutputBy(selfb);
        to->changeTransparentLightOutputBy(selfb);
    }
    else if (in)
    {
        in->changeNumberOfActiveLightSourcesBy(-1 * bright);
    }

    if (in && in->isTransparent())
    {
        in->changeLightOutputBy(-1 * selfb);
        in->changeTransparentLightOutputBy(-1 * selfb);
        if (extin)
        {
            extin->changeNumberOfActiveLightSourcesBy(-1 * selfb);
        }
    }

    if (unit->getUnitIn())
    {
        if (unit->isChar())
        {
            unit->getUnitIn()->decrementNumberOfCharactersInsideUnit();
        }
        if (unit == unit->getUnitIn()->getUnitContains())
        {
            unit->getUnitIn()->setUnitContains(unit->getNext());
        }
        else
        {
            for (u = unit->getUnitIn()->getUnitContains(); u->getNext() != unit; u = u->getNext())
            {
                ;
            }
            u->setNext(unit->getNext());
        }
    }

    unit->setUnitIn(to);
    unit->setNext(to->getUnitContains());
    to->setUnitContains(unit);

    if (unit->isChar())
    {
        unit->getUnitIn()->incrementNumberOfCharactersInsideUnit();
    }
    to->increaseWeightBy(unit->getWeight());

    if (pile && IS_MONEY(unit))
    {
        pile_money(unit);
    }
}

void unit_down(unit_data *unit, unit_data *to)
{
    intern_unit_down(unit, to, TRUE);
}

void intern_unit_to_unit(unit_data *unit, unit_data *to, ubit1 pile)
{
    assert(to);
    if (unit == to)
    {
        slog(LOG_ALL, 0, "ERROR.  Unit tried to link into its self, please report.");
        assert(to != unit);
    }

    if (to->getUnitIn())
    {
        intern_unit_to_unit(unit, to->getUnitIn(), FALSE);
    }

    intern_unit_down(unit, to, FALSE);

    if (pile && IS_MONEY(unit))
    {
        pile_money(unit);
    }
}

void unit_to_unit(unit_data *unit, unit_data *to)
{
    intern_unit_to_unit(unit, to, TRUE);
}

void snoop(unit_data *ch, unit_data *victim)
{
    assert(!victim->is_destructed());
    assert(!ch->is_destructed());

    assert(ch != victim);
    /*   assert(IS_PC(ch) && IS_PC(victim)); */
    assert(CHAR_DESCRIPTOR(ch) && CHAR_DESCRIPTOR(victim));
    assert(!CHAR_IS_SNOOPING(ch) && !CHAR_IS_SNOOPED(victim));
    assert(CHAR_LEVEL(CHAR_ORIGINAL(victim)) < CHAR_LEVEL(CHAR_ORIGINAL(ch)));
    /*   assert(!CHAR_IS_SWITCHED(victim)); */

    CHAR_DESCRIPTOR(ch)->getSnoopData().setSnooping(victim);
    CHAR_DESCRIPTOR(victim)->getSnoopData().setSnoopBy(ch);
}

/* Mode 0: Stop ch from snooping a person       */
/* Mode 1: Mode 0 + stop any person snooping ch */
void unsnoop(unit_data *ch, int mode)
{
    assert(CHAR_DESCRIPTOR(ch));
    assert(CHAR_IS_SNOOPING(ch) || CHAR_IS_SNOOPED(ch));

    if (CHAR_IS_SNOOPING(ch))
    {
        act("You no longer snoop $3n.", A_SOMEONE, ch, cActParameter(), CHAR_DESCRIPTOR(ch)->cgetSnoopData().getSnooping(), TO_CHAR);
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->getSnoopData().getSnooping())->getSnoopData().setSnoopBy(nullptr);
        CHAR_DESCRIPTOR(ch)->getSnoopData().setSnooping(nullptr);
    }

    if (CHAR_IS_SNOOPED(ch) && mode)
    {
        act("You no longer snoop $3n, $3e was extracted.",
            A_SOMEONE,
            CHAR_DESCRIPTOR(ch)->cgetSnoopData().getSnoopBy(),
            cActParameter(),
            ch,
            TO_CHAR);
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->getSnoopData().getSnoopBy())->getSnoopData().setSnooping(nullptr);
        CHAR_DESCRIPTOR(ch)->getSnoopData().setSnoopBy(nullptr);
    }
}

void switchbody(unit_data *ch, unit_data *vict)
{
    assert(CHAR_DESCRIPTOR(ch) && vict->isNPC());
    assert(!CHAR_DESCRIPTOR(vict));
    assert(!CHAR_IS_SNOOPING(ch) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(ch)));
    assert(!CHAR_IS_SNOOPED(ch) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(ch)));
    assert(!vict->is_destructed());
    assert(!ch->is_destructed());

    CHAR_DESCRIPTOR(ch)->setCharacter(vict);

    if (ch->isPC())
    {
        CHAR_DESCRIPTOR(ch)->setOriginalCharacter(ch);
    }
    if (CHAR_IS_SNOOPING(ch))
    {
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->getSnoopData().getSnooping())->getSnoopData().setSnoopBy(vict);
    }
    if (CHAR_IS_SNOOPED(ch))
    {
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(ch)->getSnoopData().getSnoopBy())->getSnoopData().setSnooping(vict);
    }

    UCHAR(vict)->setDescriptor(CHAR_DESCRIPTOR(ch));
    UCHAR(ch)->setDescriptor(nullptr);
    UCHAR(vict)->setLastLocation(nullptr);
}

void unswitchbody(unit_data *npc)
{
    assert(npc->isNPC() && CHAR_DESCRIPTOR(npc));
    assert(CHAR_IS_SWITCHED(npc));
    assert(!CHAR_IS_SNOOPING(npc) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPING(npc)));
    assert(!CHAR_IS_SNOOPED(npc) || CHAR_DESCRIPTOR(CHAR_IS_SNOOPED(npc)));

    send_to_char("You return to your original body.<br/>", npc);

    if (CHAR_IS_SNOOPING(npc))
    {
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->getSnoopData().getSnooping())->getSnoopData().setSnoopBy(CHAR_ORIGINAL(npc));
    }

    if (CHAR_IS_SNOOPED(npc))
    {
        CHAR_DESCRIPTOR(CHAR_DESCRIPTOR(npc)->getSnoopData().getSnoopBy())->getSnoopData().setSnooping(CHAR_ORIGINAL(npc));
    }

    CHAR_DESCRIPTOR(npc)->setCharacter(CHAR_ORIGINAL(npc));
    CHAR_DESCRIPTOR(npc)->setOriginalCharacter(nullptr);

    UCHAR(CHAR_DESCRIPTOR(npc)->cgetCharacter())->setDescriptor(CHAR_DESCRIPTOR(npc));
    UCHAR(npc)->setDescriptor(nullptr);
}

void stop_fightfollow(unit_data *unit)
{
    if (unit->isChar())
    {
        if (CHAR_FOLLOWERS(unit) || CHAR_MASTER(unit))
        {
            die_follower(unit);
        }

        stop_fighting(unit);
    }
}

void stop_snoopwrite(unit_data *unit)
{
    // removed this statement: if (!IS_PC(unit) || UNIT_IN(unit))
    //

    descriptor_data *d = nullptr;
    while ((d = unit_is_edited(unit)))
    {
        send_to_char("<br/>Unit was extracted, sorry.<br/>", d->cgetCharacter());
        set_descriptor_fptr(d, descriptor_interpreter, FALSE);
    }

    if (unit->isChar())
    {
        if (CHAR_IS_SWITCHED(unit))
        {
            unswitchbody(unit);
        }

        /* If the PC which is switched is extracted, then unswitch */
        if (unit->isPC() && !CHAR_DESCRIPTOR(unit))
        {
            for (d = g_descriptor_list; d; d = d->getNext())
            {
                if (d->cgetOriginalCharacter() == unit)
                {
                    unswitchbody(d->getCharacter());
                    break;
                }
            }
        }

        if (CHAR_IS_SNOOPING(unit) || CHAR_IS_SNOOPED(unit))
        {
            unsnoop(unit, 1); /* Remove all snoopers */
        }
    }
}

/* Used when a unit is to be extracted from the game */
/* Extracts recursively                              */
void extract_unit(unit_data *unit)
{
    /* Prevent recursive calling on extracted units. */
    /* This happens on for example corpses. When the */
    /* destruct_affect is called inside extract we   */
    /* got a recursive call.                         */

    if (unit->is_destructed())
    {
        return;
    }

    /* We can't extract rooms! Sanity, MS 300595, wierd bug... */
    assert(!unit->isRoom());

    if (unit->isPC())
    {
        // slog(LOG_ALL, 0, "DEBUG: Extracting player %s", UNIT_NAME(unit));
        UPC(unit)->gstate_tomenu(nullptr);
    }

    DeactivateDil(unit);

    /*if (IS_PC(unit))
    {
        slog(LOG_ALL, 0, "DEBUG: Extracting player step 2: %s", UNIT_NAME(unit));
    }*/

    unit->register_destruct();

    if (UNIT_IS_EQUIPPED(unit))
    {
        unequip_object(unit);
    }

    stop_all_special(unit);
    stop_affect(unit);

    while (unit->getUnitContains())
    {
        extract_unit(unit->getUnitContains());
    }

    /*	void unlink_affect(class unit_affected_type *af);
          while (UNIT_FUNC(unit))
        destroy_fptr(unit, UNIT_FUNC(unit));

          while (UNIT_AFFECTED(unit))
             unlink_affect(UNIT_AFFECTED(unit));
    */

    stop_fightfollow(unit);
    stop_snoopwrite(unit);

    if (unit->isPC() && CHAR_DESCRIPTOR(unit))
    {
        UPC(unit)->disconnect_game();
    }

    /*if (!IS_PC(unit) || UNIT_IN(unit))
    {
      if (UNIT_IN(unit))
         unit_from_unit(unit);

      unit_to_unit(unit, g_destroy_room);

      // Otherwise find_unit will find it AFTER it has been extracted!!
      remove_from_unit_list(unit);
    }*/

    if (unit->getUnitIn())
    {
        unit_from_unit(unit);
    }

    if (!unit->isPC())
    {
        unit_to_unit(unit, g_destroy_room); // Apparently dont place PCs in the destroy room

        // Otherwise find_unit will find it AFTER it has been extracted!!
        // Players are already removed from the list in gstate_tomenu()
        remove_from_unit_list(unit);
    }
}

/* ***********************************************************************
   Here follows high-level versions of some earlier routines, ie functions
   which incorporate the actual player-data.
   *********************************************************************** */

/* Add weight to the unit and change everything it is in */
/* (It will not change the -basic- weight of a player)   */
void weight_change_unit(unit_data *unit, int weight)
{
    for (; unit; unit = unit->getUnitIn())
    {
        unit->increaseWeightBy(weight);
    }
}

extra_descr_data *quest_add(unit_data *ch, const char *name, const char *descr)
{
    assert(name != nullptr);
    assert(name[0]);

    return PC_QUEST(ch).add(name, descr);
}
