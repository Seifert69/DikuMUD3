#pragma once

class unit_data;

class snoop_data
{
public:
    const unit_data *getSnooping() const;
    const unit_data *getSnoopBy() const;

    void setSnooping(const unit_data *value);
    void setSnoopBy(const unit_data *value);

private:
    unit_data *snooping{nullptr}; // Who is this char snooping
    unit_data *snoop_by{nullptr}; // And who is snooping on this char
};
