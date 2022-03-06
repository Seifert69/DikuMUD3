#pragma once

#include "bytestring.h"
#include "destruct.h"
#include "essential.h"
#include "event.h"
#include "vme.h"

class unit_fptr : public basedestruct
{
public:
    unit_fptr();
    ~unit_fptr();

    ubit16 getFunctionPointerIndex() const;
    void readFunctionPointerIndexFrom(CByteBuffer &buf, int &nError);
    void setFunctionPointerIndex(ubit16 value);

    ubit8 getFunctionPriority() const;
    void readFunctionPriorityFrom(CByteBuffer &buf, int &nError);
    void setFunctionPriority(ubit8 value);

    ubit16 getHeartBeat() const;
    ubit16 *getHeartBeatPtr();
    void readHeartBeatFrom(CByteBuffer &buf, int &nError);
    void setHeartBeat(ubit16 value);

    bool isActivateOnEventFlagSet(ubit16 value);
    void setActivateOnEventFlag(ubit16 value);
    void setAllActivateOnEventFlags(ubit16 value);
    void removeActivateOnEventFlag(ubit16 value);
    void readActivateOnEventFlagsFrom(CByteBuffer &buf, int &nError);
    ubit16 getAllActivateOnEventFlags() const;

    void *getData();
    void setData(void *value);
    void readDataFrom(CByteBuffer &buf);

    unit_fptr *getNext();
    void setNext(unit_fptr *value);

    eventq_elem *getEventQueue();
    void setEventQueue(eventq_elem *value);

    int destruct_classindex();

private:
    ubit16 index{0};                  // Index to function pointer array
    ubit8 priority{FN_PRI_CHORES};    // Order to insert ftpr on unit (2020)
    ubit16 heart_beat{PULSE_SEC * 5}; // in 1/4 of a sec
    ubit16 flags{0};                  // When to override next function (boolean)
    void *data{nullptr};              // Pointer to data local for this unit
    unit_fptr *next{nullptr};         // Next in linked list
    eventq_elem *event{nullptr};      // pointer to eventq for quick removing
};
