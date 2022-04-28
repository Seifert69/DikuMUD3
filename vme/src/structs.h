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

    /**
     * @name Hometown related code
     * @{
     */
    const char *getHometown() const { return m_hometown; }
    char **getHometownPtr() { return &m_hometown; }
    void setHometown(char *value)
    {
        if (m_hometown == nullptr)
        {
            CREATE(m_hometown, char, strlen(value) + 1);
        }
        else
        {
            RECREATE(m_hometown, char, strlen(value) + 1);
        }
        strcpy(m_hometown, value);
    }
    /// @}

    /** @name Prompt String related code
     * @{
     */
    const char *getPromptString() const { return m_promptstr; }
    char **getPromptStringPtr() { return &m_promptstr; }
    void setPromptString(char *value) { m_promptstr = value; }
    void freePromptString()
    {
        if (m_promptstr)
        {
            FREE(m_promptstr);
        }
    }
    /// @}

    /**
     * @name Admin related code
     * @{
     */
    extra_list &getAdministrationInformation() { return m_info; }
    /// @}

    /**
     * @name Quest related code
     * @{
     */
    extra_list &getQuestInformation() { return m_quest; }
    /// @}

    /**
     * @name Profession related code
     * @{
     */
    sbit8 getProfession() const { return m_profession; }
    sbit8 *getProfessionPtr() { return &m_profession; }
    void setProfession(sbit8 value) { m_profession = value; }
    /// @}

    /**
     * @name Virtual Player level code
     * @{
     */
    ubit16 getVirtualPlayerLevel() const { return m_vlvl; }
    ubit16 *getVirtualPlayerLevelPtr() { return &m_vlvl; }
    void setVirtualPlayerLevel(ubit16 value) { m_vlvl = value; }
    void incrementVirtualPlayerLevel() { m_vlvl++; }
    /// @}

    /**
     * @name UID for players
     * @{
     */
    sbit32 getPlayerUID() const { return m_id; }
    void setPlayerUID(sbit32 value) { m_id = value; }
    /// @}

    /**
     * @name Skill points
     * @{
     */
    sbit32 getSkillPoints() const { return m_skill_points; }
    sbit32 *getSkillPointsPtr() { return &m_skill_points; }
    void setSkillPoints(sbit32 value) { m_skill_points = value; }
    void increaseSkillPointsBy(sbit32 value) { m_skill_points += value; }
    /// @}

    /**
     * @name Ability Points
     * @{
     */
    sbit32 getAbilityPoints() const { return m_ability_points; }
    sbit32 *getAbilityPointsPtr() { return &m_ability_points; }
    void setAbilityPoints(sbit32 value) { m_ability_points = value; }
    void increaseAbilityPointsBy(sbit32 value) { m_ability_points += value; }
    /// @}

    /**
     * @name Flags
     * @{
     */
    ubit16 getAllPCFlags() const { return m_flags; }
    ubit16 *getAllPCFlagsPtr() { return &m_flags; }
    void setPCFlag(ubit16 value) { m_flags |= value; }
    void setAllPCFlags(ubit16 value) { m_flags = value; }
    void togglePCFlag(ubit16 value) { m_flags ^= value; }
    void removePCFlag(ubit16 value) { m_flags &= ~value; }
    /// @}

    /**
     * @name Crimes Committed
     * @{
     */
    ubit16 getNumberOfCrimesCommitted() const { return m_nr_of_crimes; }
    ubit16 *getNumberOfCrimesCommittedPtr() { return &m_nr_of_crimes; }
    void setNumberOfCrimesCommitted(ubit16 value) { m_nr_of_crimes = value; }
    /// @}

    /**
     * @name Crack Attempts
     * @{
     */
    ubit16 getNumberOfCrackAttempts() const { return m_crack_attempts; }
    void setNumberOfCrackAttempts(ubit16 value) { m_crack_attempts = value; }
    void incrementNumberOfCrackAttempts() { m_crack_attempts++; }
    /// @}

    /**
     * @name Lifespan
     * @{
     */
    ubit16 getLifespan() const { return m_lifespan; }
    ubit16 *getLifespanPtr() { return &m_lifespan; }
    void setLifespan(ubit16 value) { m_lifespan = value; }
    /// @}

    /**
     * @name Spells skills Learned
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSpellSkillAtIndex(size_t index) const { return m_spells[index]; }
    sbit16 *getSpellSkillArrayPtr() { return &m_spells[0]; }
    sbit16 *getSpellSkillAtIndexPtr(size_t index) { return &m_spells[index]; }
    void setSpellSKillAtIndexTo(size_t index, sbit16 value) { m_spells[index] = value; }
    void increaseSpellSkillAtIndexBy(size_t index, sbit16 value) { m_spells[index] += value; }
    void decreaseSpellSkillAtIndexBy(size_t index, sbit16 value) { m_spells[index] -= value; }
    /// @}

    /**
     * @name Spell level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getSpellLevelAtIndex(size_t index) const { return m_spell_lvl[index]; }
    ubit8 *getSpellLevelAtIndexPtr(size_t index) { return &m_spell_lvl[index]; }
    ubit8 *getSpellLevelArrayPtr() { return &m_spell_lvl[0]; }
    void setSpellLevelAtIndexTo(size_t index, ubit8 value) { m_spell_lvl[index] = value; }
    /// @}

    /**
     * @name Skills
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSkillAtIndex(size_t index) const { return m_skills[index]; }
    sbit16 *getSkillAtIndexPtr(size_t index) { return &m_skills[index]; }
    sbit16 *getSkillArrayPtr() { return &m_skills[0]; }
    void setSkillAtIndexTo(size_t index, sbit16 value) { m_skills[index] = value; }
    void increaseSkillAtIndexBy(size_t index, sbit16 value) { m_skills[index] += value; }
    void decreaseSkillAtIndexBy(size_t index, sbit16 value) { m_skills[index] -= value; }
    /// @}

    /**
     * @name Skill Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getSkillLevelAtIndex(size_t index) const { return m_skill_lvl[index]; }
    ubit8 *getSkillLevelAtIndexPtr(size_t index) { return &m_skill_lvl[index]; }
    ubit8 *getSkillLevelArrayPtr() { return &m_skill_lvl[0]; }
    void setSkillLevelAtIndexTo(size_t index, ubit8 value) { m_skill_lvl[index] = value; }
    /// @}

    /**
     * @name Weapon skill
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getWeaponSkillAtIndex(size_t index) const { return m_weapons[index]; }
    sbit16 *getWeaponSkillArrayPtr() { return &m_weapons[0]; }
    sbit16 *getWeaponSkillAtIndexPtr(size_t index) { return &m_weapons[index]; }
    void setWeaponSkillAtIndexTo(size_t index, sbit16 value) { m_weapons[index] = value; }
    void increaseWeaponSkillAtIndexBy(size_t index, sbit16 value) { m_weapons[index] += value; }
    void decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value) { m_weapons[index] -= value; }
    /// @}

    /**
     * @name Weapon Skill Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getWeaponSkillLevelAtIndex(size_t index) const { return m_weapon_lvl[index]; }
    ubit8 *getWeaponSkillLevelAtIndexPtr(size_t index) { return &m_weapon_lvl[index]; }
    ubit8 *getWeaponSkillLevelArrayPtr() { return &m_weapon_lvl[0]; }
    void setWeaponSkillLevelAtIndexTo(size_t index, ubit8 value) { m_weapon_lvl[index] = value; }
    /// @}

    /**
     * @name Ability Level
     * @todo Add range checking to indexes
     * @{
     */
    ubit8 getAbilityLevelAtIndex(size_t index) const { return m_ability_lvl[index]; }
    ubit8 *getAbilityLevelAtIndexPtr(size_t index) { return &m_ability_lvl[index]; }
    ubit8 *getAbilityLevelArrayPtr() { return &m_ability_lvl[0]; }
    void setAbilityLevelAtIndexTo(size_t index, ubit8 value) { m_ability_lvl[index] = value; }
    /// @}

    /**
     * @name Conditions
     * @todo Add range checking to indexes
     * @{
     */
    sbit8 getConditionAtIndex(size_t index) const { return m_conditions[index]; }
    sbit8 *getConditionAtIndexPtr(size_t index) { return &m_conditions[index]; }
    void setConditionAtIndexTo(size_t index, sbit8 value) { m_conditions[index] = value; }
    void increaseConditionAtIndexBy(size_t index, sbit8 value) { m_conditions[index] += value; }
    void decreaseConditionAtIndexBy(size_t index, sbit8 value) { m_conditions[index] -= value; }
    /// @}

    /**
     * @name Access Level
     * @{
     */
    ubit8 getAccessLevel() const { return m_nAccessLevel; }
    void setAccessLevel(ubit8 value) { m_nAccessLevel = value; }
    /// @}
private:
    terminal_setup_type m_setup{0, 0, 0, 0, 0, 0, 0, 0}; ///<
    pc_time_data m_time{};                               ///< PCs time info
    pc_account_data m_account{};                         ///< Accounting
    char *m_guild{nullptr};                              ///< Player's current default guild (guilds in .info)
    char *m_bank{nullptr};                               ///< How much money in bank?
    char *m_hometown{nullptr};                           ///< PCs Hometown (symbolic reference)
    char *m_promptstr{nullptr};                          ///< A PC's Prompt
    extra_list m_info;                                   ///< For saving Admin information
    extra_list m_quest;                                  ///< For saving QUEST information
    sbit8 m_profession{-1};                              ///< The player's chosen profession, -1 means unknown
    ubit16 m_vlvl{0};                                    ///< Virtual Level for player
    sbit32 m_id{-1};                                     ///< Unique identifier for each player (-1 guest)
    sbit32 m_skill_points{0};                            ///< No of practice points left
    sbit32 m_ability_points{0};                          ///< No of practice points left
    ubit16 m_flags{0};                                   ///< flags for PC setup (brief, noshout...)
    ubit16 m_nr_of_crimes{0};                            ///< Number of crimes committed
    ubit16 m_crack_attempts{0};                          ///< Number of wrong passwords entered
    ubit16 m_lifespan{0};                                ///< How many year to live....
    sbit16 m_spells[SPL_TREE_MAX]{0};                    ///< The spells learned
    ubit8 m_spell_lvl[SPL_TREE_MAX]{0};                  ///< Practiced within that level
    sbit16 m_skills[SKI_TREE_MAX]{0};                    ///< The skills learned
    ubit8 m_skill_lvl[SKI_TREE_MAX]{0};                  ///< The skills practiced within level
    sbit16 m_weapons[WPN_TREE_MAX]{0};                   ///< The weapons learned
    ubit8 m_weapon_lvl[WPN_TREE_MAX]{0};                 ///< The weapons learned
    ubit8 m_ability_lvl[ABIL_TREE_MAX]{0};               ///< The abilities learned
    sbit8 m_conditions[3]{0};                            ///< Drunk full etc.
    ubit8 m_nAccessLevel{0};                             ///< Access Level for BBS use
public:
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
