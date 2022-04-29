#include "npc_data.h"

size_t npc_data::g_world_nonpc = 0; // number of chars in the world

npc_data::npc_data()
    : char_data(UNIT_ST_NPC)
    , m_weapons{0}
    , m_spells{0}
    , m_default_pos{POSITION_STANDING}
    , m_flags{0}
{
    g_world_nonpc++;
}

npc_data::~npc_data()
{
    g_world_nonpc--;
}

sbit16 npc_data::getWeaponSkillAtIndex(size_t index) const
{
    return m_weapons[index];
}

sbit16 *npc_data::getWeaponSkillAtIndexPtr(size_t index)
{
    return &m_weapons[index];
}

std::array<sbit16, WPN_GROUP_MAX> &npc_data::getWeaponSkillArray()
{
    return m_weapons;
}

size_t npc_data::getWeaponsSkillArraySize() const
{
    return m_weapons.size();
}

void npc_data::setWeaponSkillAtIndexTo(size_t index, sbit16 value)
{
    m_weapons[index] = value;
}

void npc_data::increaseWeaponSkillAtIndexBy(size_t index, sbit16 value)
{
    m_weapons[index] += value;
}

void npc_data::decreaseWeaponSkillAtIndexBy(size_t index, sbit16 value)
{
    m_weapons[index] -= value;
}

sbit16 npc_data::getSpellSkillAtIndex(size_t index) const
{
    return m_spells[index];
}

sbit16 *npc_data::getSpellSkillAtIndexPtr(size_t index)
{
    return &m_spells[index];
}

std::array<sbit16, SPL_GROUP_MAX> &npc_data::getSpellSkillArray()
{
    return m_spells;
}

size_t npc_data::getSpellSkillArraySize() const
{
    return m_spells.size();
}

void npc_data::setSpellSkillAtIndexTo(size_t index, sbit16 value)
{
    m_spells[index] = value;
}

void npc_data::increaseSpellSkillAtIndexBy(size_t index, sbit16 value)
{
    m_spells[index] += value;
}

void npc_data::decreaseSpellSkillAtIndexBy(size_t index, sbit16 value)
{
    m_spells[index] += value;
}

ubit8 npc_data::getDefaultPosition() const
{
    return m_default_pos;
}

ubit8 *npc_data::getDefaultPositionPtr()
{
    return &m_default_pos;
}

void npc_data::setDefaultPosition(ubit8 value)
{
    m_default_pos = value;
}

ubit8 npc_data::getAllNPCFlags() const
{
    return m_flags;
}

ubit8 *npc_data::getAllNPCFlagsPtr()
{
    return &m_flags;
}

void npc_data::setAllNPCFlags(ubit8 value)
{
    m_flags = value;
}
