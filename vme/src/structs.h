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

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

/* ----------------- CHAR SPECIFIC STRUCTURES ----------------------- */

class char_data : public unit_data
{
public:
    explicit char_data(ubit8 unit_type);
    virtual ~char_data();

    /**
     * @name Descriptor related code
     * @{
     */
    const descriptor_data *getDescriptor() const { return m_descriptor; }
    descriptor_data *getDescriptor() { return m_descriptor; }
    void setDescriptor(descriptor_data *value) { m_descriptor = value; }
    ///@}

    /**
     * @name Combat related code
     * @{
     */
    const cCombat *getCombat() const { return m_combat; }
    cCombat *getCombat() { return m_combat; }
    void setCombat(cCombat *value) { m_combat = value; }
    ///@}

    /**
     * @name Master related code
     * @{
     */
    const unit_data *getMaster() const { return m_master; }
    unit_data *getMaster() { return m_master; }
    unit_data **getMasterPtr() { return &m_master; }
    void setMaster(unit_data *value) { m_master = value; }
    ///@}

    /**
     * @name Last Location related code
     * @{
     */
    const unit_data *getLastLocation() const { return m_last_room; }
    unit_data *getLastLocation() { return m_last_room; }
    unit_data **getLastLocationPtr() { return &m_last_room; }
    void setLastLocation(unit_data *value) { m_last_room = value; }
    ///@}

    /**
     * @name Character points related functions
     * @todo This is basically char_point_data's public interface - so in future char_point_data can be just removed
     * @{
     */
    void readFrom(CByteBuffer &buf, ubit8 unit_version, unit_data *unit, int &error)
    {
        return m_points.readFrom(buf, unit_version, unit, error);
    }

    ubit32 *getCharacterFlagsPtr() { return m_points.getCharacterFlagsPtr(); }
    ubit32 getCharacterFlags() const { return m_points.getCharacterFlags(); }
    void setAllCharacterFlags(ubit32 value) { m_points.setAllCharacterFlags(value); }
    void setCharacterFlag(ubit32 value) { m_points.setCharacterFlag(value); }
    void removeCharacterFlag(ubit32 value) { m_points.removeCharacterFlag(value); }
    void toggleCharacterFlag(ubit32 value) { m_points.toggleCharacterFlag(value); }

    sbit32 getPlayerExperience() const { return m_points.getPlayerExperience(); }
    sbit32 *getPlayerExperiencePtr() { return m_points.getPlayerExperiencePtr(); }
    void setPlayerExperience(sbit32 value) { m_points.setPlayerExperience(value); }
    void increasePlayerExperienceBy(sbit32 value) { m_points.increasePlayerExperienceBy(value); }

    ubit16 getRace() const { return m_points.getRace(); }
    ubit16 *getRacePtr() { return m_points.getRacePtr(); }
    void setRace(ubit16 value) { m_points.setRace(value); }

    sbit16 getMana() const { return m_points.getMana(); }
    sbit16 *getManaPtr() { return m_points.getManaPtr(); }
    void setMana(sbit16 value) { m_points.setMana(value); }
    void decreaseManaBy(sbit16 value) { m_points.decreaseManaBy(value); }

    sbit16 getEndurance() const { return m_points.getEndurance(); }
    sbit16 *getEndurancePtr() { return m_points.getEndurancePtr(); }
    void setEndurance(sbit16 value) { m_points.setEndurance(value); }
    void decreaseEnduranceBy(sbit16 value) { m_points.decreaseEnduranceBy(value); }

    sbit16 getOffensiveBonus() const { return m_points.getOffensiveBonus(); }
    void setOffensiveBonus(sbit16 value) { m_points.setOffensiveBonus(value); }
    sbit16 *getOffensiveBonusPtr() { return m_points.getOffensiveBonusPtr(); }

    sbit16 getDefensiveBonus() const { return m_points.getDefensiveBonus(); }
    void setDefensiveBonus(sbit16 value) { m_points.setDefensiveBonus(value); }
    sbit16 *getDefensiveBonusPtr() { return m_points.getDefensiveBonusPtr(); }

    int getNumberOfMeleeAttacks(int naturalAttacks) { return std::max(1, (naturalAttacks * getSpeedPercentage()) / 100); }

    sbit16 getSpeed() const { return m_points.getSpeed(); }
    void setSpeed(sbit16 value) { m_points.setSpeed(value); }
    void modifySpeedBy(sbit16 delta) { m_points.modifySpeedBy(delta); }
    /**
     * Calculate the speed percentage. 200% max for players, 300% max for NPCs. 25% is the worst possible (4x slowed)
     * @return speed percentage
     */
    int getSpeedPercentage()
    {
        sbit16 s = m_points.getSpeed();

        if (s < SPEED_MIN)
        {
            s = SPEED_MIN;
        }

        if (s == SPEED_DEFAULT)
        {
            return 100;
        }

        if (getUnitType() == UNIT_ST_PC)
        {
            return std::max(std::min(200, (100 * SPEED_DEFAULT) / s), 25);
        }
        return std::max(std::min(300, (100 * SPEED_DEFAULT) / s), 25);
    }

    ubit8 getNaturalArmor() const { return m_points.getNaturalArmor(); }
    void setNaturalArmor(ubit8 value) { m_points.setNaturalArmor(value); }
    ubit8 *getNaturalArmorPtr() { return m_points.getNaturalArmorPtr(); }

    ubit8 getAttackType() const { return m_points.getAttackType(); }
    void setAttackType(ubit8 value) { m_points.setAttackType(value); }
    ubit8 *getAttackTypePtr() { return m_points.getAttackTypePtr(); }

    ubit8 getSex() const { return m_points.getSex(); }
    void setSex(ubit8 value) { m_points.setSex(value); }
    ubit8 *getSexPtr() { return m_points.getSexPtr(); }

    ubit8 getLevel() const { return m_points.getLevel(); }
    void setLevel(ubit8 value) { m_points.setLevel(value); }
    ubit8 *getLevelPtr() { return m_points.getLevelPtr(); }
    void incrementLevel() { m_points.incrementLevel(); }

    ubit8 getPosition() const { return m_points.getPosition(); }
    void setPosition(ubit8 value) { m_points.setPosition(value); }
    ubit8 *getPositionPtr() { return m_points.getPositionPtr(); }

    sbit16 getAbilityAtIndex(size_t index) const { return m_points.getAbilityAtIndex(index); }
    sbit16 *getAbilityAtIndexPtr(size_t index) { return m_points.getAbilityAtIndexPtr(index); }
    sbit16 getSTR() const { return m_points.getSTR(); }
    sbit16 getDEX() const { return m_points.getDEX(); }
    sbit16 getCON() const { return m_points.getCON(); }
    sbit16 getCHA() const { return m_points.getCHA(); }
    sbit16 getBRA() const { return m_points.getBRA(); }
    sbit16 getMAG() const { return m_points.getMAG(); }
    sbit16 getDIV() const { return m_points.getDIV(); }
    sbit16 getHPP() const { return m_points.getHPP(); }
    void setSTR(sbit16 value) { m_points.setSTR(value); }
    void setDEX(sbit16 value) { m_points.setDEX(value); }
    void setCON(sbit16 value) { m_points.setCON(value); }
    void setCHA(sbit16 value) { m_points.setCHA(value); }
    void setBRA(sbit16 value) { m_points.setBRA(value); }
    void setMAG(sbit16 value) { m_points.setMAG(value); }
    void setDIV(sbit16 value) { m_points.setDIV(value); }
    void setHPP(sbit16 value) { m_points.setHPP(value); }
    void setAbilityAtIndexTo(size_t index, sbit16 value) { m_points.setAbilityAtIndexTo(index, value); }
    void increaseAbilityAtIndexBy(size_t index, sbit16 value) { m_points.increaseAbilityAtIndexBy(index, value); }
    void decreaseAbilityAtIndexBy(size_t index, sbit16 value) { m_points.decreaseAbilityAtIndexBy(index, value); }
    std::array<sbit16, ABIL_TREE_MAX> &getAbilitiesArray() { return m_points.getAbilitiesArray(); }
    /// @}
private:
    descriptor_data *m_descriptor{nullptr}; ///<
    cCombat *m_combat{nullptr};             ///<
    unit_data *m_master{nullptr};           ///< Must be a char
    unit_data *m_last_room{nullptr};        ///< Last location of character
    char_point_data m_points;               ///<
public:
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
