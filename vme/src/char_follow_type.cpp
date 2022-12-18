#include "char_follow_type.h"

#include "json_helper.h"
unit_data *char_follow_type::getFollower()
{
    return follower;
}

void char_follow_type::setFollower(unit_data *value)
{
    follower = value;
}

unit_data **char_follow_type::getFollowerPtr()
{
    return &follower;
}

char_follow_type *char_follow_type::getNext()
{
    return next;
}

void char_follow_type::setNext(char_follow_type *value)
{
    next = value;
}

void char_follow_type::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer)
{
    json::write_unit_id_kvp("follower", follower, writer);
}
