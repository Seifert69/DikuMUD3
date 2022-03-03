#include "unit_affected_type.h"

#include "structs.h"

int unit_affected_type::destruct_classindex()
{
    return DR_AFFECT;
}

sbit16 unit_affected_type::getID() const
{
    return id;
}

void unit_affected_type::setID(sbit16 value)
{
    id = value;
}

int unit_affected_type::readIDFrom(CByteBuffer &buf)
{
    return buf.Read16(&id);
}

ubit16 unit_affected_type::getBeat() const
{
    return beat;
}

void unit_affected_type::setBeat(ubit16 value)
{
    beat = value;
}

int unit_affected_type::readBeatFrom(CByteBuffer &buf)
{
    return buf.Read16(&beat);
}

sbit16 unit_affected_type::getDuration() const
{
    return duration;
}

void unit_affected_type::setDuration(sbit16 value)
{
    duration = value;
}

void unit_affected_type::decrementDuration()
{
    duration--;
}

int unit_affected_type::readDurationFrom(CByteBuffer &buf)
{
    return buf.Read16(&duration);
}

int unit_affected_type::getDataAtIndex(size_t index) const
{
    return data[index];
}

void unit_affected_type::setDataAtIndex(size_t index, int value)
{
    data[index] = value;
}

void unit_affected_type::incrementDataAtIndex(size_t index)
{
    data[index]++;
}

int unit_affected_type::readFromIntoDataAtIndex(CByteBuffer &buf, size_t index)
{
    return buf.Read32(&data[index]);
}

sbit16 unit_affected_type::getFirstFI() const
{
    return firstf_i;
}

void unit_affected_type::setFirstFI(sbit16 value)
{
    firstf_i = value;
}

int unit_affected_type::readFirstFIFrom(CByteBuffer &buf)
{
    return buf.Read16(&firstf_i);
}

sbit16 unit_affected_type::getTickFI() const
{
    return tickf_i;
}

void unit_affected_type::setTickFI(sbit16 value)
{
    tickf_i = value;
}

int unit_affected_type::readTickFIFrom(CByteBuffer &buf)
{
    return buf.Read16(&tickf_i);
}

sbit16 unit_affected_type::getLastFI() const
{
    return lastf_i;
}

void unit_affected_type::setLastFI(sbit16 value)
{
    lastf_i = value;
}

int unit_affected_type::readLastFIFrom(CByteBuffer &buf)
{
    return buf.Read16(&lastf_i);
}

sbit16 unit_affected_type::getApplyFI() const
{
    return applyf_i;
}

void unit_affected_type::setApplyFI(sbit16 value)
{
    applyf_i = value;
}

int unit_affected_type::readApplyFIFrom(CByteBuffer &buf)
{
    return buf.Read16(&applyf_i);
}

const eventq_elem *unit_affected_type::cgetEventQueueElement() const
{
    return event;
}

eventq_elem *unit_affected_type::getEventQueueElement()
{
    return event;
}

void unit_affected_type::setEventQueueElement(eventq_elem *value)
{
    event = value;
}

const unit_data *unit_affected_type::cgetOwner() const
{
    return owner;
}

unit_data *unit_affected_type::getOwner()
{
    return owner;
}

void unit_affected_type::setOwner(unit_data *value)
{
    owner = value;
}

unit_affected_type *unit_affected_type::getNext()
{
    return next;
}

void unit_affected_type::setNext(unit_affected_type *value)
{
    next = value;
}

unit_affected_type *unit_affected_type::getG_Next()
{
    return gnext;
}

void unit_affected_type::setG_Next(unit_affected_type *value)
{
    gnext = value;
}

unit_affected_type *unit_affected_type::getG_Previous()
{
    return gprevious;
}

void unit_affected_type::setG_Previous(unit_affected_type *value)
{
    gprevious = value;
}
