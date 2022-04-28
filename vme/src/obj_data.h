#pragma once

#include "unit_data.h"

#include <array>

class obj_data : public unit_data
{
public:
    static size_t g_world_noobjects; ///< number of objects in the world

public:
    /**
     * @name Constructors / Destructor
     * @{
     */
    obj_data();                                     ///< Constructor
    obj_data(const obj_data &) = delete;            ///< Delete copy constructor
    obj_data(obj_data &&) = delete;                 ///< Delete move constructor
    obj_data &operator=(const obj_data &) = delete; ///< Delete assignment operator
    obj_data &operator=(obj_data &&) = delete;      ///< Delete move assignment operator
    ~obj_data() override;                           ///< Destructor
    /// @}

    /**
     * @name Value related functions
     * @{
     */

    /**
     * Gets the value at index for the object
     *
     * WEAPONS
     *     * index = 0 is weapon category
     *     * index = 1 is weapon material bonus
     *     * index = 2 is magic bonus
     *     * index = 3 is slaying race
     *
     * ARMOURS
     *     * index = 0 is armour category
     *     * index = 1 is armour material bonus
     *     * index = 2 is magic bonus
     *
     * SHIELDS
     *     * index = 0 is shield category
     *     * index = 1 is shield material bonus
     *     * index = 2 is magic bonus
     *
     * @throws std::out_of_range if index is greater than array size
     * @param index
     * @return Bonus/Value at index
     */
    sbit32 getValueAtIndex(size_t index) const;

    /**
     * @throws std::out_of_range if index is greater than array size
     * @param index Array index see getValueAtIndex() for index values
     * @return Pointer to value at index - its a DIL thing
     */
    sbit32 *getValueAtIndexPtr(size_t index);

    /**
     * @return Upper bound of array size (5)
     */
    size_t getValueArraySize() const;

    /**
     * @throws std::out_of_range if index is greater than array size
     * @param index Array index see getValueAtIndex() for index values
     * @param value Value to set
     */
    void setValueAtIndexTo(size_t index, sbit32 value);
    /// @}

    /**
     * @name Price related functions
     * @{
     */

    /**
     * @return Price in GP
     */
    ubit32 getPriceInGP() const;
    /**
     * @return Pointer to price in GP
     */
    ubit32 *getPriceInGPPtr();

    /**
     * @param value Price in GP
     */
    void setPriceInGP(ubit32 value);
    /// @}

    /**
     * @name Price per day functions
     * @{
     */
    /**
     * @return Cost to keep per real day
     */
    ubit32 getPricePerDay() const;
    /**
     * @return Pointer to cost per real day
     */
    ubit32 *getPricePerDayPtr();
    /**
     * @param value Price to keep per real day
     */
    void setPricePerDay(ubit32 value);
    /// @}

    /**
     * @name Object Flag related functions
     * @{
     */
    /**
     * @return All object flags
     */
    ubit8 getObjectFlags() const;
    /**
     * @return pointer to object flags for DIL
     */
    ubit8 *getObjectFlagsPtr();
    /**
     * @todo Find out why a 32bit value is being passed to this function and the flags are 8bit
     * @param value Sets bits from value in object flags
     */
    void setObjectFlag(ubit8 value);
    /**
     * @todo Find out why a 32bit value is being passed to this function and the flags are 8bit
     * @param value Unsets bit from value in object flags
     */
    void removeObjectFlag(ubit8 value);
    /**
     * @todo Find out why a 32bit value is being passed to this function and the flags are 8bit
     * @param value Overwrites all flags with value
     */
    void setAllObjectFlags(ubit32 value);
    /// @}

    /**
     * @name Item type related functions
     * @todo Create ENUM for ITEM_* types
     * @{
     */

    /**
     * @return the ITEM_* type
     */
    ubit8 getObjectItemType() const;
    /**
     * @return pointer to the item type
     */
    ubit8 *getObjectItemTypePtr();
    /**
     * @param value ITEM_* value (see vme.h)
     */
    void setObjectItemType(ubit8 value);
    /// @}

    /**
     * @name Equipment position related functions
     * @{
     */
    /**
     * @return 0 or position of item in equipment
     */
    ubit8 getEquipmentPosition() const;
    /**
     * @param value 0 or position of item in equipment
     */
    void setEquipmentPosition(ubit8 value);
    /// @}
    /**
     * @name Magic Resistance related functions
     * @{
     */

    /**
     * @return magic resistance
     */
    ubit8 getMagicResistance() const;
    /**
     * @return pointer to magic resistance
     */
    ubit8 *getMagicResistancePtr();
    /**
     * @param value Set magic resistance to
     */
    void setMagicResistance(ubit8 value);
    /// @}
private:
    std::array<sbit32, 5> m_value{0}; ///< Values of the item (see list)
    ubit32 m_cost{0};                 ///< Value when sold (gp.)
    ubit32 m_cost_per_day{0};         ///< Cost to keep pr. real day
    ubit8 m_flags{0};                 ///< Various special object flags
    ubit8 m_type{ITEM_TRASH};         ///< Type of item (ITEM_XXX)
    ubit8 m_equip_pos{0};             ///< 0 or position of item in equipment
    ubit8 m_resistance{0};            ///< Magic resistance
};
