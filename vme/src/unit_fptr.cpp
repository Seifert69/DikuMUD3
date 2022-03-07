#include "unit_fptr.h"

#include "structs.h"

#include <cstdlib>

unit_fptr::unit_fptr()
{
    index = 0;
    priority = FN_PRI_CHORES;
    heart_beat = PULSE_SEC * 5;
    flags = 0;
    data = nullptr;
    next = nullptr;
    event = nullptr;
}

unit_fptr::~unit_fptr()
{
    data = nullptr;
}

int unit_fptr::destruct_classindex()
{
    return DR_FUNC;
}

ubit16 unit_fptr::getFunctionPointerIndex() const
{
    return index;
}

void unit_fptr::readFunctionPointerIndexFrom(CByteBuffer &buf, int &nError)
{
    index = buf.ReadU16(&nError);
}

void unit_fptr::setFunctionPointerIndex(ubit16 value)
{
    index = value;
}

ubit8 unit_fptr::getFunctionPriority() const
{
    return priority;
}

void unit_fptr::readFunctionPriorityFrom(CByteBuffer &buf, int &nError)
{
    priority = buf.ReadU8(&nError);
}

void unit_fptr::setFunctionPriority(ubit8 value)
{
    priority = value;
}

ubit16 unit_fptr::getHeartBeat() const
{
    return heart_beat;
}

ubit16 *unit_fptr::getHeartBeatPtr()
{
    return &heart_beat;
}

void unit_fptr::readHeartBeatFrom(CByteBuffer &buf, int &nError)
{
    heart_beat = buf.ReadU16(&nError);
}

void unit_fptr::setHeartBeat(ubit16 value)
{
    heart_beat = value;
}

bool unit_fptr::isActivateOnEventFlagSet(ubit16 value)
{
    return flags & value;
}

void unit_fptr::setActivateOnEventFlag(ubit16 value)
{
    flags |= value;
}

void unit_fptr::setAllActivateOnEventFlags(ubit16 value)
{
    flags = value;
}

void unit_fptr::removeActivateOnEventFlag(ubit16 value)
{
    flags &= ~value;
}

void unit_fptr::readActivateOnEventFlagsFrom(CByteBuffer &buf, int &nError)
{
    flags = buf.ReadU16(&nError);
}

ubit16 unit_fptr::getAllActivateOnEventFlags() const
{
    return flags;
}

void *unit_fptr::getData()
{
    return data;
}

void unit_fptr::setData(void *value)
{
    data = value;
}

void unit_fptr::readDataFrom(CByteBuffer &buf)
{
    buf.ReadStringAlloc((char **)&data);
}

unit_fptr *unit_fptr::getNext()
{
    return next;
}

void unit_fptr::setNext(unit_fptr *value)
{
    next = value;
}

eventq_elem *unit_fptr::getEventQueue()
{
    return event;
}

void unit_fptr::setEventQueue(eventq_elem *value)
{
    event = value;
}
