#include "snoop_data.h"

#include "json_helper.h"

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

void snoop_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_pointer_kvp("snooping", snooping, writer);
        json::write_pointer_kvp("snoop_by", snoop_by, writer);
    }
    writer.EndObject();
}
