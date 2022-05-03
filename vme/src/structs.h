/*
 $Author: All $
 $RCSfile: structs.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#pragma once

#include "char_data.h"

#include <array>

class npc_data : public char_data
{
public:
    static size_t g_world_nonpc; // number of chars in the world
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

    /**
     * @name NPC Flags
     * @{
     */
    ubit8 getAllNPCFlags() const { return m_flags; }
    ubit8 *getAllNPCFlagsPtr() { return &m_flags; }
    void setAllNPCFlags(ubit8 value) { m_flags = value; }
    /// @}
private:
    std::array<sbit16, WPN_GROUP_MAX> m_weapons{0}; ///<
    std::array<sbit16, SPL_GROUP_MAX> m_spells{0};  ///<
    ubit8 m_default_pos{POSITION_STANDING};         ///< Default position for NPC
    ubit8 m_flags{0};                               ///< flags for NPC behavior
};

unit_data *new_unit_data(ubit8 type);
