#include "file_index_type.h"

#include "textutil.h"
#include "utils.h"
#include "vme.h"

unit_data *file_index_type::find_symbolic_instance_ref(unit_data *ref, ubit16 bitvector)
{
    unit_data *u = nullptr;
    unit_data *uu = nullptr;

    if (ref == nullptr)
    {
        return nullptr;
    }

    if (IS_SET(bitvector, FIND_UNIT_EQUIP))
    {
        for (u = ref->getUnitContains(); u; u = u->getNext())
        {
            if ((u->getFileIndex() == this) && UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_INVEN))
    {
        for (u = ref->getUnitContains(); u; u = u->getNext())
        {
            if ((u->getFileIndex() == this) && !UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_SURRO) && ref->getUnitIn())
    {
        if (this == ref->getUnitIn()->getFileIndex())
        {
            return ref->getUnitIn();
        }

        /* Run through units in local environment */
        for (u = ref->getUnitIn()->getUnitContains(); u; u = u->getNext())
        {
            if (u->getFileIndex() == this)
            {
                return u;
            }

            /* check tranparancy */
            if (u->getNumberOfCharactersInsideUnit() && u->isTransparent())
            {
                for (uu = u->getUnitContains(); uu; uu = uu->getNext())
                {
                    if (uu->getFileIndex() == this)
                    {
                        return uu;
                    }
                }
            }
        }

        /* Run through units in local environment if upwards transparent */
        if ((u = ref->getUnitIn()->getUnitIn()) && ref->getUnitIn()->isTransparent())
        {
            for (u = u->getUnitContains(); u; u = u->getNext())
            {
                if (u != ref->getUnitIn())
                {
                    if (this == u->getFileIndex())
                    {
                        return u;
                    }

                    /* check down into transparent unit */
                    if (u->getNumberOfCharactersInsideUnit() && u->isTransparent())
                    {
                        for (uu = u->getUnitContains(); uu; uu = uu->getNext())
                        {
                            if (this == uu->getFileIndex())
                            {
                                return uu;
                            }
                        }
                    }
                }
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_ZONE))
    {
        if (!m_fi_unit_list.empty())
        {
            for (auto it = m_fi_unit_list.begin(); it != m_fi_unit_list.end(); it++)
            {
                if ((*it)->getFileIndex() == this)
                {
                    return u;
                }
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_WORLD))
    {
        if (!m_fi_unit_list.empty())
        {
            return m_fi_unit_list.front();
        }
    }

    return nullptr;
}

unit_data *file_index_type::find_symbolic_instance()
{
    if (!m_fi_unit_list.empty())
    {
        return m_fi_unit_list.front();
    }

    return nullptr;
}

const char *file_index_type::getName() const
{
    return m_name.c_str();
}

long file_index_type::getFilepos() const
{
    return m_filepos;
}

ubit32 file_index_type::getCRC() const
{
    return m_crc;
}

sbit16 file_index_type::getNumInZone() const
{
    return m_no_in_zone;
}

ubit16 file_index_type::getRoomNum() const
{
    return m_room_no;
}

ubit8 file_index_type::getType() const
{
    return m_type;
}

ubit16 file_index_type::getNumInMem() const
{
    return m_no_in_mem;
}

ubit32 file_index_type::getLength() const
{
    return m_length;
}

void file_index_type::IncrementNumInMemory()
{
    ++m_no_in_mem;
}

zone_type *file_index_type::getZone() const
{
    return m_zone;
}

void file_index_type::setType(ubit8 value)
{
    m_type = value;
}

void file_index_type::setZone(zone_type *value)
{
    m_zone = value;
}

void file_index_type::setName(const char *value, bool to_lower)
{
    if (value)
    {
        m_name = std::string(value);
        if (to_lower)
        {
            str_lower(m_name);
        }
    }
}

void file_index_type::DecrementNumInMemory()
{
    /// @todo find out why code is trying to decrement m_no_in_mem below 0
    ///   I tried this but somewhere it goes below zero
    ///     assert(m_no_in_mem > 0);
    if (m_no_in_mem > 0)
    {
        --m_no_in_mem;
    }
}

void file_index_type::IncrementNumInZone()
{
    ++m_no_in_zone;
}

void file_index_type::setNumInZone(sbit16 value)
{
    m_no_in_zone = value;
}

void file_index_type::setLength(ubit32 value)
{
    m_length = value;
}

void file_index_type::setFilepos(long value)
{
    m_filepos = value;
}

void file_index_type::setRoomNum(ubit16 value)
{
    m_room_no = value;
}

void file_index_type::setNumInMemory(sbit16 value)
{
    m_no_in_mem = value;
}

void file_index_type::setCRC(ubit32 value)
{
    m_crc = value;
}

bool file_index_type::Empty() const
{
    return m_fi_unit_list.empty();
}

unit_data *file_index_type::Front() const
{
    return m_fi_unit_list.front();
}

std::forward_list<unit_data *>::iterator file_index_type::Begin()
{
    return m_fi_unit_list.begin();
}

std::forward_list<unit_data *>::iterator file_index_type::End()
{
    return m_fi_unit_list.end();
}

void file_index_type::PushFront(unit_data *value)
{
    m_fi_unit_list.push_front(value);
}

void file_index_type::Remove(unit_data *value)
{
    m_fi_unit_list.remove(value);
}
