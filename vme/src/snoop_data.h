#pragma once

#include <rapidjson/prettywriter.h>

class unit_data;

class snoop_data
{
public:
    unit_data *getSnooping();
    const unit_data *getSnooping() const;
    unit_data *getSnoopBy();
    const unit_data *getSnoopBy() const;

    void setSnooping(const unit_data *value);
    void setSnoopBy(const unit_data *value);

    void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

private:
    unit_data *snooping{nullptr}; ///< Who is this char snooping
    unit_data *snoop_by{nullptr}; ///< And who is snooping on this char
};
