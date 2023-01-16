#include "room_data.h"

#include "json_helper.h"
#include "utility.h"

size_t room_data::g_world_norooms = 0; // number of rooms in the world

room_data::room_data(file_index_type *fi)
    : unit_data(UNIT_ST_ROOM, fi)
    , m_mapx{-1}
    , m_mapy{-1}
#ifndef MPLEX_COMPILE
    , m_waiting_dijkstra{false}
#endif
{
    g_world_norooms++;
}

room_data::~room_data()
{
    g_world_norooms--;

    for (int i = 0; i < MAX_EXIT + 1; i++)
    {
        if (m_dir_option[i])
        {
            delete m_dir_option[i];
        }
    }
}

room_direction_data *room_data::getRoomDirectionDataForExit(size_t index)
{
    return m_dir_option.at(index);
}

void room_data::setRoomDirectionDataForExitTo(size_t index, room_direction_data *value)
{
    m_dir_option.at(index) = value;
}

ubit8 room_data::getRoomFlags() const
{
    return m_flags;
}

ubit8 *room_data::getRoomFlagsPtr()
{
    return &m_flags;
}

void room_data::setAllRoomFlags(ubit16 value)
{
    m_flags = static_cast<ubit8>(value);
}

ubit8 room_data::getLandscapeTerrain() const
{
    return m_movement_type;
}

ubit8 *room_data::getLandscapeTerrainPtr()
{
    return &m_movement_type;
}

void room_data::setLandscapeTerrain(ubit8 value)
{
    m_movement_type = value;
}

ubit8 room_data::getRoomMagicalResistance() const
{
    return m_resistance;
}

ubit8 *room_data::getRoomMagicalResistancePtr()
{
    return &m_resistance;
}

void room_data::setRoomMagicalResistance(ubit8 value)
{
    m_resistance = value;
}

sbit16 room_data::getMapXCoordinate() const
{
    return m_mapx;
}

void room_data::setMapXCoordinate(sbit16 value)
{
    m_mapx = value;
}

sbit16 room_data::getMapYCoordinate() const
{
    return m_mapy;
}

void room_data::setMapYCoordinate(sbit16 value)
{
    m_mapy = value;
}

int room_data::getStrongComponent() const
{
    return m_sc;
}

void room_data::setStrongComponent(int value)
{
    m_sc = value;
}

int room_data::getRoomNumber() const
{
    return m_num;
}

void room_data::setRoomNumber(int value)
{
    m_num = value;
}

std::vector<room_data::vertex_descriptor> &room_data::getPath()
{
    return m_path;
}

const std::vector<room_data::vertex_descriptor> &room_data::getPath() const
{
    return m_path;
}

std::vector<room_data::vertex_descriptor> &room_data::getDistance()
{
    return m_distance;
}

const std::vector<room_data::vertex_descriptor> &room_data::getDistance() const
{
    return m_distance;
}

bool room_data::getWaitingDijkstra() const
{
    return m_waiting_dijkstra;
}

void room_data::setWaitingDijkstra(bool value)
{
    m_waiting_dijkstra = value;
}

void room_data::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_unit_id_kvp("id", this, writer);

        writer.String("room_data");
        writer.StartObject();
        {
            writer.String("dir_option");
            writer.StartArray();
            for (auto &direction : m_dir_option)
            {
                if (direction)
                {
                    direction->toJSON(writer);
                }
                else
                {
                    writer.Null();
                }
            }
            writer.EndArray();

            std::string bits;
            json::write_kvp("flags", sprintbit(bits, m_flags, g_room_flags), writer);
            json::write_kvp("movement_type", m_movement_type, writer);
            json::write_kvp("resistance", m_resistance, writer);
            json::write_kvp("mapx", m_mapx, writer);
            json::write_kvp("mapy", m_mapy, writer);
            json::write_kvp("sc", m_sc, writer);
            json::write_kvp("num", m_num, writer);
        }
        writer.EndObject();
        writer.String("unit_data");
        unit_data::toJSON(writer);
    }
    writer.EndObject();
}
