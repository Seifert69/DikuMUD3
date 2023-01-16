#pragma once

#include "room_direction_data.h"
#include "unit_data.h"

#include <array>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

class room_data : public unit_data
{
public:
    static size_t g_world_norooms; ///< number of rooms in the world
public:
    /**
     * @name Constructors/Destructor
     * @{
     */
    room_data(file_index_type *fi);
    room_data(const room_data &) = delete;
    room_data(room_data &&) = delete;
    room_data &operator=(const room_data &) = delete;
    room_data &operator=(room_data &&) = delete;
    ~room_data() override;
    ///@}

    /**
     * @name Room Exit Information
     * @{
     */
    /**
     * Gets information about room exit
     * @param index Direction of exit
     * @throws std::out_of_range
     * @return room_direction_data * with information
     */
    room_direction_data *getRoomDirectionDataForExit(size_t index);
    /**
     * Sets data for a room exit
     * @param index Direction of exit
     * @throws std::out_of_range
     * @param value Room Direction Data
     */
    void setRoomDirectionDataForExitTo(size_t index, room_direction_data *value);
    ///@}

    /**
     * @name Room flags
     * @{
     */
    /**
     * @return All room flags
     */
    ubit8 getRoomFlags() const;
    /**
     * @return Pointer to all room flags
     */
    ubit8 *getRoomFlagsPtr();
    /**
     * Sets all room flags to value
     * @param value NB this is cast down to ubit8
     */
    void setAllRoomFlags(ubit16 value);
    /// @}

    /**
     * @name Room terrain movement type
     * @{
     */
    /**
     * Returns terrain type for the room
     * @return one of SECT_* types
     */
    ubit8 getLandscapeTerrain() const;
    ubit8 *getLandscapeTerrainPtr();
    /**
     * Sets room terrain movement type
     * @param value
     */
    void setLandscapeTerrain(ubit8 value);
    /// @}

    /**
     * @name Magical Resistance
     * @{
     */
    ubit8 getRoomMagicalResistance() const;
    ubit8 *getRoomMagicalResistancePtr();
    void setRoomMagicalResistance(ubit8 value);
    /// @}

    /**
     * @name Graphical Map Coordinates
     * @{
     */
    sbit16 getMapXCoordinate() const;
    void setMapXCoordinate(sbit16 value);

    sbit16 getMapYCoordinate() const;
    void setMapYCoordinate(sbit16 value);
    /// @}

    /**
     * @name Strong component
     * @{
     */
    int getStrongComponent() const;
    void setStrongComponent(int value);
    /// @}

    /**
     * @name Room Number
     * @{
     */
    int getRoomNumber() const;
    void setRoomNumber(int value);
    /// @}

    virtual void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

private:
    std::array<room_direction_data *, MAX_EXIT + 1> m_dir_option{nullptr}; ///<
    ubit8 m_flags{0};                                                      ///< Room flags
    ubit8 m_movement_type{0};                                              ///< The type of movement (city, hills etc.)
    ubit8 m_resistance{0};                                                 ///< Magic resistance of the room
    sbit16 m_mapx{-1};                                                     ///< Graphical map coordinates
    sbit16 m_mapy{-1};                                                     ///< Graphical map coordinates
    int m_sc{0};                                                           ///< strong component, used for shortest path
    int m_num{0};                                                          ///< room number, used for shortest path

public:
    enum edge_dir_t
    {
        edge_dir = 101
    };

    using graph_t = boost::adjacency_list<boost::vecS,
                                          boost::vecS,
                                          boost::directedS,
                                          boost::no_property,
                                          boost::property<boost::edge_weight_t, int, boost::property<edge_dir_t, int>>>;

    using vertex_descriptor = boost::graph_traits<graph_t>::vertex_descriptor;

    /**
     * @name Path
     * @{
     */
    std::vector<vertex_descriptor> &getPath();
    const std::vector<vertex_descriptor> &getPath() const;
    ///@}

    /**
     * @name Distance
     * @{
     */
    std::vector<vertex_descriptor> &getDistance();
    const std::vector<vertex_descriptor> &getDistance() const;
    /// @}

    /**
     * @name
     * @{
     */
    bool getWaitingDijkstra() const;
    void setWaitingDijkstra(bool value);
    /// @}
private:
    std::vector<vertex_descriptor> m_path;     ///<
    std::vector<vertex_descriptor> m_distance; ///<
    bool m_waiting_dijkstra{false};            ///<
};
