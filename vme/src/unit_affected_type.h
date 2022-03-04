#pragma once

#include "bytestring.h"
#include "destruct.h"
#include "essential.h"
#include "event.h"

class unit_data;

class unit_affected_type : public basedestruct
{
public:
    sbit16 getID() const;
    void setID(sbit16 value);

    void bread(CByteBuffer *pBuf, int *nError);

    ubit16 getBeat() const;
    void setBeat(ubit16 value);

    sbit16 getDuration() const;
    void setDuration(sbit16 value);
    void decrementDuration();

    int getDataAtIndex(size_t index) const;
    void setDataAtIndex(size_t index, int value);
    void incrementDataAtIndex(size_t index);
    int readFromIntoDataAtIndex(CByteBuffer &buf, size_t index);

    sbit16 getFirstFI() const;
    void setFirstFI(sbit16 value);

    sbit16 getTickFI() const;
    void setTickFI(sbit16 value);

    sbit16 getLastFI() const;
    void setLastFI(sbit16 value);

    sbit16 getApplyFI() const;
    void setApplyFI(sbit16 value);

    const eventq_elem *cgetEventQueueElement() const;
    eventq_elem *getEventQueueElement();
    void setEventQueueElement(eventq_elem *value);

    const unit_data *cgetOwner() const;
    unit_data *getOwner();
    void setOwner(unit_data *value);

    unit_affected_type *getNext();
    void setNext(unit_affected_type *value);

    unit_affected_type *getG_Next();
    void setG_Next(unit_affected_type *value);

    unit_affected_type *getG_Previous();
    void setG_Previous(unit_affected_type *value);

    int destruct_classindex();

private:
    sbit16 id{0};                           //
    ubit16 beat{0};                         // Beat in 1/4 of secs, 0 = None
    sbit16 duration{0};                     // How many beats until end
    sbit32 data[3]{0};                      //
    sbit16 firstf_i{0};                     //
    sbit16 tickf_i{0};                      //
    sbit16 lastf_i{0};                      //
    sbit16 applyf_i{0};                     //
    eventq_elem *event{nullptr};            // pointer to eventq for quick removing
    unit_data *owner{nullptr};              //
    unit_affected_type *next{nullptr};      //
    unit_affected_type *gnext{nullptr};     //
    unit_affected_type *gprevious{nullptr}; //
};
