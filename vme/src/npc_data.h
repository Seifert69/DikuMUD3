#pragma once

#include "char_data.h"

#include <rapidjson/prettywriter.h>

#include <array>

class npc_data : public char_data
{
public:
    static size_t g_world_nonpc; // number of chars in the world
public:
    npc_data(file_index_type *fi);
    ~npc_data() override;

    /**
     * @name Weapons
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getWeaponSkillAtIndex(size_t index) const;
    sbit16 *getWeaponSkillAtIndexPtr(size_t index);
    std::array<sbit16, WPN_GROUP_MAX> &getWeaponSkillArray();
    size_t getWeaponsSkillArraySize() const;
    void setWeaponSkillAtIndexTo(size_t index, sbit16 value);
    void increaseWeaponSkillAtIndexBy(size_t index, sbit16 value);
    void decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value);
    /// @}

    /**
     * Spells
     * @todo Add range checking to indexes
     * @{
     */
    sbit16 getSpellSkillAtIndex(size_t index) const;
    sbit16 *getSpellSkillAtIndexPtr(size_t index);
    std::array<sbit16, SPL_GROUP_MAX> &getSpellSkillArray();
    size_t getSpellSkillArraySize() const;
    void setSpellSkillAtIndexTo(size_t index, sbit16 value);
    void increaseSpellSkillAtIndexBy(size_t index, sbit16 value);
    void decreaseSpellSkillAtIndexBy(size_t index, sbit16 value);
    /// @}

    /**
     * @name Default Position
     * @{
     */
    ubit8 getDefaultPosition() const;
    ubit8 *getDefaultPositionPtr();
    void setDefaultPosition(ubit8 value);
    /// @}

    /**
     * @name NPC Flags
     * @{
     */
    ubit8 getAllNPCFlags() const;
    ubit8 *getAllNPCFlagsPtr();
    void setAllNPCFlags(ubit8 value);
    /// @}

    virtual void toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const;

private:
    std::array<sbit16, WPN_GROUP_MAX> m_weapons{0}; ///<
    std::array<sbit16, SPL_GROUP_MAX> m_spells{0};  ///<
    ubit8 m_default_pos{POSITION_STANDING};         ///< Default position for NPC
    ubit8 m_flags{0};                               ///< flags for NPC behavior
};
