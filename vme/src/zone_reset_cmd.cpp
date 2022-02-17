#include "zone_reset_cmd.h"

void zone_reset_cmd::setCommandNum(ubit8 value)
{
    cmd_no = value;
}

ubit8 zone_reset_cmd::getCommandNum() const
{
    return cmd_no;
}

void zone_reset_cmd::setFileIndexType(size_t index, file_index_type *value)
{
    fi.at(index) = value;
}

file_index_type *zone_reset_cmd::getFileIndexType(size_t index) const
{
    return fi.at(index);
}

sbit16 zone_reset_cmd::getNum(size_t index) const
{
    return num.at(index);
}

void zone_reset_cmd::setNum(size_t index, sbit16 value)
{
    num.at(index) = value;
}

void zone_reset_cmd::setCompleteFlag(ubit8 value)
{
    cmpl = value;
}

void zone_reset_cmd::setNextPtr(zone_reset_cmd *value)
{
    next = value;
}

void zone_reset_cmd::setNestedPtr(zone_reset_cmd *value)
{
    nested = value;
}

ubit8 zone_reset_cmd::getCompleteFlag() const
{
    return cmpl;
}

zone_reset_cmd *zone_reset_cmd::getNested() const
{
    return nested;
}

zone_reset_cmd *zone_reset_cmd::getNext() const
{
    return next;
}
