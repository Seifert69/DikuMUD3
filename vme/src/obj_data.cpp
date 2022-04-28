#include "obj_data.h"

size_t obj_data::g_world_noobjects; // number of objects in the world

obj_data::obj_data()
    : unit_data(UNIT_ST_OBJ)
    , m_value{0}
    , m_cost{0}
    , m_cost_per_day{0}
    , m_flags{0}
    , m_type{ITEM_TRASH}
    , m_equip_pos{0}
    , m_resistance{0}
{
    g_world_noobjects++;
}

obj_data::~obj_data()
{
    g_world_noobjects--;
}

sbit32 obj_data::getValueAtIndex(size_t index) const
{
    return m_value.at(index);
}

sbit32 *obj_data::getValueAtIndexPtr(size_t index)
{
    return &m_value.at(index);
}

size_t obj_data::getValueArraySize() const
{
    return m_value.size();
}

void obj_data::setValueAtIndexTo(size_t index, sbit32 value)
{
    m_value.at(index) = value;
}

ubit32 obj_data::getPriceInGP() const
{
    return m_cost;
}

ubit32 *obj_data::getPriceInGPPtr()
{
    return &m_cost;
}

void obj_data::setPriceInGP(ubit32 value)
{
    m_cost = value;
}

ubit32 obj_data::getPricePerDay() const
{
    return m_cost_per_day;
}

ubit32 *obj_data::getPricePerDayPtr()
{
    return &m_cost_per_day;
}

void obj_data::setPricePerDay(ubit32 value)
{
    m_cost_per_day = value;
}

ubit8 obj_data::getObjectFlags() const
{
    return m_flags;
}

ubit8 *obj_data::getObjectFlagsPtr()
{
    return &m_flags;
}

void obj_data::setObjectFlag(ubit8 value)
{
    m_flags |= value;
}

void obj_data::removeObjectFlag(ubit8 value)
{
    m_flags &= ~value;
}

void obj_data::setAllObjectFlags(ubit32 value)
{
    m_flags = static_cast<ubit8>(value);
}

ubit8 obj_data::getObjectItemType() const
{
    return m_type;
}

ubit8 *obj_data::getObjectItemTypePtr()
{
    return &m_type;
}

void obj_data::setObjectItemType(ubit8 value)
{
    m_type = value;
}

ubit8 obj_data::getEquipmentPosition() const
{
    return m_equip_pos;
}

void obj_data::setEquipmentPosition(ubit8 value)
{
    m_equip_pos = value;
}

ubit8 obj_data::getMagicResistance() const
{
    return m_resistance;
}

ubit8 *obj_data::getMagicResistancePtr()
{
    return &m_resistance;
}

void obj_data::setMagicResistance(ubit8 value)
{
    m_resistance = value;
}
