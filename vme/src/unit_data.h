#pragma once

#include "essential.h"
#include "extra.h"
#include "file_index_type.h"
#include "namelist.h"
#include "unit_affected_type.h"
#include "unit_dil_affected_type.h"
#include "unit_fptr.h"

class unit_data : public basedestruct
{
public:
    static constexpr sbit16 MinAlignment = -1000; ///< Minimum possible value for alignment
    static constexpr sbit16 MaxAlignment = 1000;  ///< Maximum possible value for alignment

public:
    /**
     * @name Constructors/Destructor
     * @{
     */

    /**
     * @param unit_type One of UNIT_ST_NPC, UNIT_ST_PC, UNIT_ST_ROOM, UNIT_ST_OBJ
     */
    explicit unit_data(ubit8 unit_type);
    unit_data(const unit_data &) = delete;
    unit_data(unit_data &&) = delete;
    unit_data &operator=(const unit_data &) = delete;
    unit_data &operator=(unit_data &&) = delete;
    ~unit_data() override;
    /// @}

    unit_data *copy();
    void set_fi(file_index_type *f);

    ubit8 getUnitType() const;

    /**
     * @name Name related code
     * @{
     */
    cNamelist &getNames();
    const cNamelist &getNames() const;
    /// @}

    /**
     * @name Function Pointer related code
     * @{
     */
    unit_fptr *getFunctionPointer();
    void setFunctionPointer(unit_fptr *value);
    /// @}

    /**
     * @name Unit affected type related code
     * @{
     */
    unit_affected_type *getUnitAffectedType();
    void setUnitAffectedType(unit_affected_type *value);
    /// @}

    /**
     * @name File index related code
     * @{
     */
    file_index_type *getFileIndex();
    const file_index_type *getFileIndex() const;
    void setFileIndex(file_index_type *value);
    /// @}

    /**
     * @name Key related code
     * @{
     */
    const char *getKey() const;
    char **getKeyPtr();
    void setKey(char *value);
    /// @}

    /**
     * @name Contains / Contained related code
     * @{
     */
    unit_data *getMyContainer();
    const unit_data *getMyContainer() const;
    void setMyContainerTo(unit_data *value);

    const unit_data *getContainedUnits() const;
    unit_data *getContainedUnits();
    void setContainedUnit(unit_data *value);
    /// @}

    /**
     * @name Linked List related code
     * @{
     */
    const unit_data *getNext() const;
    unit_data *getNext();
    void setNext(unit_data *value);

    unit_data *getGlobalNext();
    const unit_data *getGlobalNext() const;
    void setGlobalNext(unit_data *value);

    unit_data *getGlobalPrevious();
    const unit_data *getGlobalPrevious() const;
    void setGlobalPrevious(unit_data *value);
    /// @}

    /**
     * @name Flag related code
     * @{
     */
    ubit32 getManipulate() const;
    ubit32 *getManipulatePtr();
    void setAllManipulateFlags(ubit32 value);
    void setManipulateFlag(ubit32 value);

    ubit16 getUnitFlags() const;
    ubit16 *getUnitFlagsPtr();
    void setAllUnitFlags(ubit16 value);
    void setUnitFlag(ubit16 value);
    void removeUnitFlag(ubit16 value);
    /// @}

    /**
     * @name Weight related code
     * @{
     */
    sbit32 getBaseWeight() const;
    void setBaseWeight(sbit32 value);

    sbit32 getWeight() const;
    void reduceWeightBy(sbit32 value);
    void increaseWeightBy(sbit32 value);
    void setWeight(sbit32 value);
    /// @}

    /**
     * @name Capacity related code
     * @{
     */
    sbit16 getCapacity() const;
    sbit16 *getCapacityPtr();
    void setCapacity(sbit16 value);
    /// @}

    /**
     * @name Size related code
     * @{
     */
    ubit16 getSize() const;
    ubit16 *getSizePtr();
    void setSize(ubit16 value);
    void increaseSizeBy(ubit16 value);
    /// @}

    /**
     * @name Open object related code
     * @{
     */
    ubit8 getOpenFlags() const;
    ubit8 *getOpenFlagsPtr();
    void setAllOpenFlags(ubit8 value);
    void setOpenFlag(ubit8 value);

    ubit8 getOpenDifficulty() const;
    ubit8 *getOpenDifficultyPtr();
    void setOpenDifficulty(ubit8 value);
    /// @}

    /**
     * @name Light related code
     * @{
     */
    sbit16 getNumberOfActiveLightSources() const;
    void changeNumberOfActiveLightSourcesBy(sbit16 value);
    void setNumberOfActiveLightSources(sbit16 value);

    sbit16 getLightOutput() const;
    void setLightOutput(sbit16 value);
    void changeLightOutputBy(sbit16 value);

    sbit16 getTransparentLightOutput() const;
    void setTransparentLightOutput(sbit16 value);
    void changeTransparentLightOutputBy(sbit16 value);
    ///@}

    /**
     * @name
     */
    ubit8 getNumberOfCharactersInsideUnit() const;
    void decrementNumberOfCharactersInsideUnit();
    void incrementNumberOfCharactersInsideUnit();
    void setNumberOfCharactersInsideUnit(ubit8 value);
    /// @}

    /**
     * @name
     * @{
     */
    ubit8 getLevelOfWizardInvisibility() const;
    ubit8 *getLevelOfWizardInvisibilityPtr();
    void setLevelOfWizardInvisibility(ubit8 value);
    /// @}

    /**
     * @name Hitpoint related code
     * Code for Max and Current hitpoints
     * @{
     */
    sbit32 getMaximumHitpoints() const;
    sbit32 *getMaximumHitpointsPtr();
    void setMaximumHitpoints(sbit32 value);

    sbit32 getCurrentHitpoints() const;
    sbit32 *getCurrentHitpointsPtr();
    void setCurrentHitpoints(sbit32 value);
    void changeCurrentHitpointsBy(sbit32 value);
    ///@}

    /**
     * @name Alignment related code
     * @{
     */
    sbit16 getAlignment() const;
    sbit16 *getAlignmentPtr();

    /**
     * Modify alignment by value - negative numbers are subtracted, positive added.
     *
     * If the change exceeds alignment limits, alignment is set to either min or max
     * value and false is returned.
     *
     * eg current alignment = -900 and value = -200
     * result alignment = -1000 and false is returned.
     *
     * @param value Amount to vary alignment by.
     * @return true if the change was accepted as is, false if the change overflowed limits and was truncated
     */
    bool changeAlignmentBy(int64_t value);

    /**
     * Set alignment to value
     *
     * For VME
     *
     * If the change exceeds alignment limits, alignment is set to either min or max
     * value and false is returned.
     * eg. current alignment = -900 and value = -200
     * result alignment = -1000 and false is returned.
     *
     * For VMC (#define VMC is set)
     *
     * If the change exceeds alignment limits, alignment is set to 0 and dmc_error is called
     *
     * @param value Amount to set alignment to.
     * @return true if the change was accepted as is, false if the change overflowed limits and was truncated
     */
    bool setAlignment(int64_t value);
    ///@}

    /**
     * @name Name / Description related code
     * @{
     */
    const std::string &getTitle() const;
    std::string *getTitlePtr();
    void setTitle(std::string value);

    const std::string &getDescriptionOfOutside() const;
    std::string *getDescriptionOfOutsidePtr();
    void setDescriptionOfOutside(std::string value);

    const std::string &getDescriptionOfInside() const;
    std::string *getDescriptionOfInsidePtr();
    void setDescriptionOfInside(std::string value);
    /// @}

    /**
     * @name
     * @{
     */
    const extra_list &getExtraList() const;
    extra_list &getExtraList();
    /// @}

    int destruct_classindex();
    std::string json();

private:
    cNamelist m_names;                       ///< Name Keyword list for get, enter, etc.
    unit_fptr *m_func{nullptr};              ///< Function pointer type
    unit_affected_type *m_affected{nullptr}; ///<
    file_index_type *m_fi{nullptr};          ///< Unit file-index
    char *m_key{nullptr};                    ///< Pointer to fileindex to Unit which is the key
    unit_data *m_outside{nullptr};           ///< Pointer out of the unit, ie. from an object out to the char carrying it
    unit_data *m_inside{nullptr};            ///< Linked list of chars,rooms & objs
    unit_data *m_next{nullptr};              ///< For next unit in 'inside' linked list
    unit_data *m_gnext{nullptr};             ///< global l-list of objects, chars & rooms
    unit_data *m_gprevious{nullptr};         ///< global l-list of objects, chars & rooms
    ubit32 m_manipulate{0};                  ///< WEAR_XXX macros
    ubit16 m_flags{0};                       ///< Invisible, can_bury, burried...
    sbit32 m_base_weight{0};                 ///< The "empty" weight of a room/char/obj (lbs)
    sbit32 m_weight{0};                      ///< Current weight of a room/obj/char
    sbit16 m_capacity{0};                    ///< Capacity of obj/char/room, -1 => any
    ubit16 m_size{0};                        ///< (cm) MOBs height, weapons size, ropes length
    ubit8 m_status{0};                       ///< IS_ROOM, IS_OBJ, IS_PC, IS_NPC
    ubit8 m_open_flags{0};                   ///< In general OPEN will mean can "enter"?
    ubit8 m_open_diff{0};                    ///< Open difficulty
    sbit16 m_light{0};                       ///< Number of active light sources in unit
    sbit16 m_bright{0};                      ///< How much the unit shines
    sbit16 m_illum{0};                       ///< how much bright is by transparency
    ubit8 m_chars{0};                        ///< How many chars is inside the unit
    ubit8 m_minv{0};                         ///< Level of wizard invisible
    sbit32 m_max_hp{0};                      ///< The maximum number of hitpoints
    sbit32 m_hp{0};                          ///< The actual amount of hitpoints left
    sbit16 m_alignment{0};                   ///< +-1000 for alignments
    std::string m_title;                     ///< Room title, Char title, Obj "the barrel", NPC "the Beastly Fido"
    std::string m_out_descr;                 ///< The outside description of a unit
    std::string m_in_descr;                  ///< The inside description of a unit
    extra_list m_extra;                      ///< All the look 'at' stuff
};
