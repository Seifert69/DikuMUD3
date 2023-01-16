#include "pc_time_data.h"

#include "json_helper.h"

void pc_time_data::readFrom(CByteBuffer &buf, int &errors)
{
    creation = buf.ReadU32(&errors);
    connect = buf.ReadU32(&errors);
    birth = buf.ReadU32(&errors);
    played = buf.ReadU32(&errors);
}

void pc_time_data::writeTo(CByteBuffer &buf) const
{
    buf.Append32((ubit32)creation);
    buf.Append32((ubit32)connect);
    buf.Append32((ubit32)birth);
    buf.Append32(played);
}

time_t pc_time_data::getPlayerCharacterCreationTime() const
{
    return creation;
}

void pc_time_data::setPlayerCharacterCreationTime(time_t value)
{
    creation = value;
}

time_t pc_time_data::getPlayerLastConnectTime() const
{
    return connect;
}

void pc_time_data::setPlayerLastConnectTime(time_t value)
{
    connect = value;
}

time_t pc_time_data::getPlayerBirthday() const
{
    return birth;
}

time_t *pc_time_data::getPlayerBirthdayPtr()
{
    return &birth;
}

void pc_time_data::setPlayerBirthday(time_t value)
{
    birth = value;
}

ubit32 pc_time_data::getTotalTimePlayedInSeconds() const
{
    return played;
}

void pc_time_data::setTotalTimePlayedInSeconds(ubit32 value)
{
    played = value;
}

void pc_time_data::incTotalTimePlayedInSecondsBy(ubit32 seconds)
{
    played += seconds;
}

void pc_time_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_kvp("creation", creation, writer);
        json::write_kvp("connect", connect, writer);
        json::write_kvp("birth", birth, writer);
        json::write_kvp("played", played, writer);
    }
    writer.EndObject();
}
