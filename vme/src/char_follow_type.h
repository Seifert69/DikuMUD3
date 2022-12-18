#pragma once

#include "unit_data.h"

class char_follow_type
{
public:
    char_follow_type() = default;
    ~char_follow_type() = default;

    unit_data *getFollower();
    void setFollower(unit_data *value);
    unit_data **getFollowerPtr();

    char_follow_type *getNext();
    void setNext(char_follow_type *value);

    void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer);

private:
    unit_data *follower{nullptr}; ///< Must be a char
    char_follow_type *next{nullptr};
};
