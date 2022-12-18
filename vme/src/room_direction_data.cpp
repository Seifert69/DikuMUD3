#include "room_direction_data.h"

#include "json_helper.h"

room_direction_data::room_direction_data()
{
    key = nullptr;
    to_room = nullptr;
    exit_info = 0;
    difficulty = 0;
}

room_direction_data::~room_direction_data()
{
}

const cNamelist &room_direction_data::getOpenName() const
{
    return open_name;
}

cNamelist &room_direction_data::getOpenName()
{
    return open_name;
}

const char *room_direction_data::getKey() const
{
    return key;
}

char **room_direction_data::getKeyPtr()
{
    return &key;
}

void room_direction_data::setKey(char *value)
{
    FREE(key);
    key = value;
}

const unit_data *room_direction_data::getToRoom() const
{
    return to_room;
}

unit_data *room_direction_data::getToRoom()
{
    return to_room;
}

void room_direction_data::setToRoom(unit_data *value)
{
    to_room = value;
}

ubit8 room_direction_data::getSkillDifficulty() const
{
    return difficulty;
}

void room_direction_data::setSkillDifficulty(ubit8 value)
{
    difficulty = value;
}

ubit8 *room_direction_data::getSkillDifficultyPtr()
{
    return &difficulty;
}

ubit8 room_direction_data::getDoorFlags() const
{
    return exit_info;
}

bool room_direction_data::isDoorFlagSet(ubit8 value) const
{
    return exit_info & value;
}

void room_direction_data::setDoorFlags(ubit8 value)
{
    exit_info = value;
}
ubit8 *room_direction_data::getDoorFlagsPtr()
{
    return &exit_info;
}

void room_direction_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    //    writer.String("room_direction_data");

    writer.String("open_name");
    open_name.toJSON(writer);

    writer.String("key");
    if (key)
    {
        writer.String(key);
    }
    else
    {
        writer.Null();
    }

    json::write_unit_id_kvp("to_room", to_room, writer);

    writer.String("difficulty");
    writer.Uint(difficulty);

    writer.String("weight");
    writer.Int(weight);

    writer.String("exit_info");
    writer.Uint(exit_info);

    writer.EndObject();
}
