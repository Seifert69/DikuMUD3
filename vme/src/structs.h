/*
 $Author: All $
 $RCSfile: structs.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#pragma once

#include "char_data.h"
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

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */

class pc_data : public char_data
{
public:
    pc_data();
    ~pc_data() override;

    void gstate_tomenu(dilprg *pdontstop);
    void gstate_togame(dilprg *pdontstart);

    void disconnect_game();
    void connect_game();
    void reconnect_game(descriptor_data *d);

    /**
     * @name Terminal related code
     * @{
     */
    terminal_setup_type &getTerminalSetupType() { return m_setup; }
    /// @}

    /**
     * @name PC time related code
     * @{
     */
    pc_time_data &getPCTimeInformation() { return m_time; }
    /// @}

    /**
     * @name Accounting related code
     * @{
     */
    pc_account_data &getPCAccountData() { return m_account; }
    /// @}

    /**
     * @name Guild related code
     * @{
     */
    const char *getGuild() const { return m_guild; }
    char **getGuildPtr() { return &m_guild; }
    void freeGuild()
    {
        if (m_guild)
        {
            FREE(m_guild);
            m_guild = nullptr;
        }
    }
    void setGuild(const char *value)
    {
        if (m_guild == nullptr)
        {
            CREATE(m_guild, char, strlen(value) + 1);
        }
        else
        {
            RECREATE(m_guild, char, strlen(value) + 1);
        }
        strcpy(m_guild, value);
    }
    /// @}

    /**
     * @name Bank related code
     * @{
     */
    const char *getBank() const { return m_bank; }
    char **getBankPtr() { return &m_bank; }
    void freeBank()
    {
        if (m_bank)
        {
            FREE(m_bank);
        }
    }
    void setBank(char *value) { m_bank = value; }
    /// @}
private:
    terminal_setup_type m_setup{}; ///<
    pc_time_data m_time{};         ///< PCs time info
    pc_account_data m_account{};   ///< Accounting
    char *m_guild{nullptr};        ///< Player's current default guild (guilds in .info)
    char *m_bank{nullptr};         ///< How much money in bank?
public:
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
