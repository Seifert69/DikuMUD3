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

/* ------------------ NPC SPECIFIC STRUCTURES ----------------------- */

class npc_data : public char_data
{
public:
    npc_data();
    ~npc_data() override;

    /**
     * @name Weapons
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getWeaponSkillAtIndex(size_t index) const { return m_weapons[index]; }
    sbit16 *getWeaponSkillAtIndexPtr(size_t index) { return &m_weapons[index]; }
    std::array<sbit16, WPN_GROUP_MAX> &getWeaponSkillArray() { return m_weapons; }
    size_t getWeaponsSkillArraySize() const { return m_weapons.size(); }
    void setWeaponSkillAtIndexTo(size_t index, sbit16 value) { m_weapons[index] = value; }
    void increaseWeaponSkillAtIndexBy(size_t index, sbit16 value) { m_weapons[index] += value; }
    void decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value) { m_weapons[index] -= value; }
    /// @}

    /**
     * Spells
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSpellSkillAtIndex(size_t index) const { return m_spells[index]; }
    sbit16 *getSpellSkillAtIndexPtr(size_t index) { return &m_spells[index]; }
    std::array<sbit16, SPL_GROUP_MAX> &getSpellSkillArray() { return m_spells; }
    size_t getSpellSkillArraySize() const { return m_spells.size(); }
    void setSpellSkillAtIndexTo(size_t index, sbit16 value) { m_spells[index] = value; }
    void increaseSpellSkillAtIndexBy(size_t index, sbit16 value) { m_spells[index] += value; }
    void decreaseSpellSkillAtIndexBy(size_t index, sbit16 value) { m_spells[index] += value; }
    /// @}

    /**
     * @name Default Position
     * @{
     */
    ubit8 getDefaultPosition() const { return m_default_pos; }
    ubit8 *getDefaultPositionPtr() { return &m_default_pos; }
    void setDefaultPosition(ubit8 value) { m_default_pos = value; }
    /// @}
private:
    std::array<sbit16, WPN_GROUP_MAX> m_weapons{0}; ///<
    std::array<sbit16, SPL_GROUP_MAX> m_spells{0};  ///<
    ubit8 m_default_pos{POSITION_STANDING};         ///< Default position for NPC
public:
    ubit8 flags; ///< flags for NPC behavior
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
