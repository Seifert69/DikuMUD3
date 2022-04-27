#include "snoop_data.h"

unit_data *snoop_data::getSnooping()
{
    return snooping;
}
const unit_data *snoop_data::getSnooping() const
{
    return snooping;
}

const unit_data *snoop_data::getSnoopBy() const
{
    return snoop_by;
}

unit_data *snoop_data::getSnoopBy()
{
    return snoop_by;
}

void snoop_data::setSnooping(const unit_data *value)
{
    snooping = const_cast<unit_data *>(value);
}

void snoop_data::setSnoopBy(const unit_data *value)
{
    snoop_by = const_cast<unit_data *>(value);
}
