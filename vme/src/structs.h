/*
 $Author: All $
 $RCSfile: structs.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#pragma once

#include "char_point_data.h"
#include "color.h"
#include "combat.h"
#include "destruct.h"
#include "event.h"
#include "extra.h"
#include "hookmud.h"
#include "namelist.h"
#include "pc_account_data.h"
#include "pc_time_data.h"
#include "protocol.h"
#include "queue.h"
#include "room_direction_data.h"
#include "unit_data.h"

#include <vme.h>

#include <cstring>
#include <forward_list>
#include <map>
#include <ostream>
#include <vector>

#ifndef MPLEX_COMPILE
    #include <boost/graph/adjacency_list.hpp>
    #include <boost/graph/graph_traits.hpp>
#endif

#define FI_MAX_ZONENAME 30 /* Max length of any zone-name    */
#define FI_MAX_UNITNAME 15 /* Max length of any unit-name    */

#define PC_MAX_PASSWORD 13 /* Max length of any pc-password  */
#define PC_MAX_NAME 15     /* 14 Characters + Null           */

#define MESS_ATTACKER 1
#define MESS_VICTIM 2
#define MESS_ROOM 3

/* For use in spec_assign.c's g_unit_function_array[] */
#define SD_NEVER 0 /* Never save this function           */
#define SD_NULL 1  /* Ignore fptr->data (save as 0 ptr)  */
#define SD_ASCII 2 /* If pointer, then it's ascii char * */

class char_follow_type;
class descriptor_data;
class dilprg;
class diltemplate;
class file_index_type;
class unit_affected_type;
class unit_data;
class unit_dil_affected_type;
class unit_fptr;
class zone_reset_cmd;

/* ----------------- DATABASE STRUCTURES ----------------------- */

/* ----------------- OTHER STRUCTURES ----------------------- */

/* --------------------- DESCRIPTOR STRUCTURES -------------------- */

/* ----------------- UNIT GENERAL STRUCTURES ----------------------- */

/* ----------------- ROOM SPECIFIC STRUCTURES ----------------------- */

class room_data : public unit_data
{
public:
    room_data();
    ~room_data();

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
    room_direction_data *getRoomDirectionDataForExit(size_t index) { return m_dir_option.at(index); }
    /**
     * Sets data for a room exit
     * @param index Direction of exit
     * @throws std::out_of_range
     * @param value Room Direction Data
     */
    void setRoomDirectionDataForExitTo(size_t index, room_direction_data *value) { m_dir_option.at(index) = value; }
    ///@}

    /**
     * @name Room flags
     * @{
     */
    /**
     * @return All room flags
     */
    ubit8 getRoomFlags() const { return flags; }
    /**
     * @return Pointer to all room flags
     */
    ubit8 *getRoomFlagsPtr() { return &flags; }
    /**
     * Sets all room flags to value
     * @param value NB this is cast down to ubit8
     */
    void setAllRoomFlags(ubit16 value) { flags = static_cast<ubit8>(value); }
    /// @}

    /**
     * @name Room terrain movement type
     * @{
     */
    /**
     * Returns terrain type for the room
     * @return one of SECT_* types
     */
    ubit8 getTerrainMovementType() const { return movement_type; }
    ubit8 *getTerrainMovementTypePtr() { return &movement_type; }
    /**
     * Sets room terrain movement type
     * @param value
     */
    void setTerrainMovementType(ubit8 value) { movement_type = value; }
    /// @}

    /**
     * @name Magical Resistance
     * @{
     */
    ubit8 getRoomMagicalResistance() const { return resistance; }
    ubit8 *getRoomMagicalResistancePtr() { return &resistance; }
    void setRoomMagicalResistance(ubit8 value) { resistance = value; }
    /// @}

    /**
     * @name Graphical Map Coordinates
     * @{
     */
    sbit16 getMapXCoordinate() const { return mapx; }
    void setMapXCoordinate(sbit16 value) { mapx = value; }

    sbit16 getMapYCoordinate() const { return mapy; }
    void setMapYCoordinate(sbit16 value) { mapy = value; }
    /// @}

    /**
     * @name Strong component
     * @{
     */
    int getStrongComponent() const { return sc; }
    void setStrongComponent(int value) { sc = value; }
    /// @}

    /**
     * @name Room Number
     * @{
     */
    int getRoomNumber() const { return num; }
    void setRoomNumber(int value) { num = value; }
    /// @}
private:
    std::array<room_direction_data *, MAX_EXIT + 1> m_dir_option{nullptr}; ///<
    ubit8 flags{0};                                                        ///< Room flags
    ubit8 movement_type{0};                                                ///< The type of movement (city, hills etc.)
    ubit8 resistance{0};                                                   ///< Magic resistance of the room
    sbit16 mapx{0};                                                        ///< Graphical map coordinates
    sbit16 mapy{0};                                                        ///< Graphical map coordinates
    int sc{0};                                                             ///< strong component, used for shortest path
    int num{0};                                                            ///< room number, used for shortest path

#ifndef MPLEX_COMPILE
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
    std::vector<vertex_descriptor> &getPath() { return path; }
    const std::vector<vertex_descriptor> &getPath() const { return path; }
    ///@}

    /**
     * @name Distance
     * @{
     */
    std::vector<vertex_descriptor> &getDistance() { return distance; }
    const std::vector<vertex_descriptor> &getDistance() const { return distance; }
    /// @}
private:
    std::vector<vertex_descriptor> path;     ///<
    std::vector<vertex_descriptor> distance; ///<
public:
    int waiting_dijkstra;
#endif
};

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

class obj_data : public unit_data
{
public:
    obj_data();
    ~obj_data();

    sbit32 value[5];     ///< Values of the item (see list)
    ubit32 cost;         ///< Value when sold (gp.)
    ubit32 cost_per_day; ///< Cost to keep pr. real day

    ubit8 flags;      ///< Various special object flags
    ubit8 type;       ///< Type of item (ITEM_XXX)
    ubit8 equip_pos;  ///< 0 or position of item in equipment
    ubit8 resistance; ///< Magic resistance
};

/* ----------------- CHAR SPECIFIC STRUCTURES ----------------------- */

class char_data : public unit_data
{
public:
    explicit char_data(ubit8 unit_type);
    virtual ~char_data();

    descriptor_data *descriptor;
    cCombat *Combat;
    unit_data *master;    ///< Must be a char
    unit_data *last_room; ///< Last location of character
    char_point_data points;

    char_follow_type *followers;

    char *last_attacker; ///< Last attacker of character
    char *money;         ///<  Money transfer from db-files.

    ubit8 last_attacker_type; ///< Last attacker type of character
};

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */

class pc_data : public char_data
{
public:
    pc_data();
    ~pc_data();

    void gstate_tomenu(dilprg *pdontstop);
    void gstate_togame(dilprg *pdontstart);

    void disconnect_game();
    void connect_game();
    void reconnect_game(descriptor_data *d);

    terminal_setup_type setup;

    pc_time_data m_time{};     ///< PCs time info
    pc_account_data account{}; ///< Accounting

    char *guild;     ///< Player's current default guild (guilds in .info)
    char *bank;      ///< How much money in bank?
    char *hometown;  ///< PCs Hometown (symbolic reference)
    char *promptstr; ///< A PC's Prompt

    extra_list info;  ///< For saving Admin information
    extra_list quest; ///< For saving QUEST information

    sbit8 profession; ///< The player's chosen profession, -1 means unknown
    ubit16 vlvl;      ///< Virtual Level for player

    sbit32 id;             ///< Unique identifier for each player (-1 guest)
    sbit32 skill_points;   ///< No of practice points left
    sbit32 ability_points; ///< No of practice points left

    ubit16 flags;          ///< flags for PC setup (brief, noshout...)
    ubit16 nr_of_crimes;   ///< Number of crimes committed
    ubit16 crack_attempts; ///< Number of wrong passwords entered
    ubit16 lifespan{};     ///< How many year to live....

    sbit16 spells[SPL_TREE_MAX];   ///< The spells learned
    ubit8 spell_lvl[SPL_TREE_MAX]; ///< Practiced within that level

    sbit16 skills[SKI_TREE_MAX];   ///< The skills learned
    ubit8 skill_lvl[SKI_TREE_MAX]; ///< The skills practiced within level

    sbit16 weapons[WPN_TREE_MAX];   ///< The weapons learned
    ubit8 weapon_lvl[WPN_TREE_MAX]; ///< The weapons learned

    ubit8 ability_lvl[ABIL_TREE_MAX]; ///< The abilities learned

    sbit8 conditions[3]; ///< Drunk full etc.
    ubit8 nAccessLevel;  ///< Access Level for BBS use

    char pwd[PC_MAX_PASSWORD];  ///< Needed when loaded w/o descriptor
    char filename[PC_MAX_NAME]; ///< The name on disk...
    ubit32 lasthosts[5];        ///< last 5 different IPs
    color_type color;           ///< Players default colors
};

/* ------------------ NPC SPECIFIC STRUCTURES ----------------------- */

class npc_data : public char_data
{
public:
    npc_data();
    ~npc_data();

    std::array<sbit16, WPN_GROUP_MAX> weapons;
    std::array<sbit16, SPL_GROUP_MAX> spells;

    ubit8 default_pos; ///< Default position for NPC
    ubit8 flags;       ///< flags for NPC behavior
};

/* ----------------- Destructed decalrations ----------------------- */

#define DR_UNIT 0
#define DR_AFFECT 1
#define DR_FUNC 2

unit_data *new_unit_data(ubit8 type);

extern int g_world_nochars;
extern int g_world_nonpc;
extern int g_world_noobjects;
extern int g_world_norooms;
extern int g_world_nopc;
extern int g_world_nozones;
