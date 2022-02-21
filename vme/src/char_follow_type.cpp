#include "char_follow_type.h"

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
