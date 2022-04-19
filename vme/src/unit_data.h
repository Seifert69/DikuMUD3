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
    explicit unit_data(ubit8 unit_type);
    virtual ~unit_data();
    unit_data *copy();
    void set_fi(file_index_type *f);

    cNamelist &getNames();
    const cNamelist &getNames() const;

    unit_fptr *getFunctionPointer();
    void setFunctionPointer(unit_fptr *value);

    unit_affected_type *getUnitAffectedType();
    void setUnitAffectedType(unit_affected_type *value);

    file_index_type *getFileIndex();
    const file_index_type *getFileIndex() const;
    void setFileIndex(file_index_type *value);

    const char *getKey() const;
    char **getKeyPtr();
    void setKey(char *value);

    unit_data *getMyContainer();
    const unit_data *getMyContainer() const;
    void setMyContainerTo(unit_data *value);

    const unit_data *getContainedUnits() const;
    unit_data *getContainedUnits();
    void setContainedUnit(unit_data *value);

    const unit_data *getNext() const;
    unit_data *getNext();
    void setNext(unit_data *value);

    unit_data *getGlobalNext();
    const unit_data *getGlobalNext() const;
    void setGlobalNext(unit_data *value);

    unit_data *getGlobalPrevious();
    const unit_data *getGlobalPrevious() const;
    void setGlobalPrevious(unit_data *value);

    ubit32 getManipulate() const;
    ubit32 *getManipulatePtr();
    void setAllManipulateFlags(ubit32 value);
    void setManipulateFlag(ubit32 value);

    ubit16 getUnitFlags() const;
    ubit16 *getUnitFlagsPtr();
    void setAllUnitFlags(ubit16 value);
    void setUnitFlag(ubit16 value);
    void removeUnitFlag(ubit16 value);

    sbit32 getBaseWeight() const;
    void setBaseWeight(sbit32 value);

    sbit32 getWeight() const;
    void reduceWeightBy(sbit32 value);
    void increaseWeightBy(sbit32 value);
    void setWeight(sbit32 value);

    sbit16 getCapacity() const;
    sbit16 *getCapacityPtr();
    void setCapacity(sbit16 value);

    ubit16 getSize() const;
    ubit16 *getSizePtr();
    void setSize(ubit16 value);
    void increaseSizeBy(ubit16 value);

    ubit8 getUnitType() const;

    ubit8 getOpenFlags() const;
    ubit8 *getOpenFlagsPtr();
    void setAllOpenFlags(ubit8 value);
    void setOpenFlag(ubit8 value);

    ubit8 getOpenDifficulty() const;
    ubit8 *getOpenDifficultyPtr();
    void setOpenDifficulty(ubit8 value);

    sbit16 getNumberOfActiveLightSources() const;
    void changeNumberOfActiveLightSourcesBy(sbit16 value);
    void setNumberOfActiveLightSources(sbit16 value);

    sbit16 getLightOutput() const;
    void setLightOutput(sbit16 value);
    void changeLightOutputBy(sbit16 value);

    sbit16 getTransparentLightOutput() const;
    void setTransparentLightOutput(sbit16 value);
    void changeTransparentLightOutputBy(sbit16 value);

    ubit8 getNumberOfCharactersInsideUnit() const;
    void decrementNumberOfCharactersInsideUnit();
    void incrementNumberOfCharactersInsideUnit();
    void setNumberOfCharactersInsideUnit(ubit8 value);

    ubit8 getLevelOfWizardInvisibility() const;
    ubit8 *getLevelOfWizardInvisibilityPtr();
    void setLevelOfWizardInvisibility(ubit8 value);

    sbit32 getMaximumHitpoints() const;
    sbit32 *getMaximumHitpointsPtr();
    void setMaximumHitpoints(sbit32 value);

    sbit32 getCurrentHitpoints() const;
    sbit32 *getCurrentHitpointsPtr();
    void setCurrentHitpoints(sbit32 value);
    void changeCurrentHitpointsBy(sbit32 value);

    sbit16 getAlignment() const;
    sbit16 *getAlignmentPtr();
    void increaseAlignmentBy(sbit16 value);
    void decreaseAlignmentBy(sbit16 value);
    void setAlignment(sbit16 value);

    const std::string &getTitle() const;
    std::string *getTitlePtr();
    void setTitle(std::string value);

    const std::string &getDescriptionOfOutside() const;
    std::string *getDescriptionOfOutsidePtr();
    void setDescriptionOfOutside(std::string value);

    const std::string &getDescriptionOfInside() const;
    std::string *getDescriptionOfInsidePtr();
    void setDescriptionOfInside(std::string value);

    const extra_list &getExtraList() const;
    extra_list &getExtraList();

    int destruct_classindex();
    std::string json();

private:
    cNamelist names;                            ///< Name Keyword list for get, enter, etc.
    unit_fptr *func{nullptr};                   ///< Function pointer type
    unit_dil_affected_type *dilaffect{nullptr}; ///<
    unit_affected_type *affected{nullptr};      ///<
    file_index_type *fi{nullptr};               ///< Unit file-index
    char *key{nullptr};                         ///< Pointer to fileindex to Unit which is the key
    unit_data *outside{nullptr};                ///< Pointer out of the unit, ie. from an object out to the char carrying it
    unit_data *inside{nullptr};                 ///< Linked list of chars,rooms & objs
    unit_data *next{nullptr};                   ///< For next unit in 'inside' linked list
    unit_data *gnext{nullptr};                  ///< global l-list of objects, chars & rooms
    unit_data *gprevious{nullptr};              ///< global l-list of objects, chars & rooms
    ubit32 manipulate{0};                       ///< WEAR_XXX macros
    ubit16 flags{0};                            ///< Invisible, can_bury, burried...
    sbit32 base_weight{0};                      ///< The "empty" weight of a room/char/obj (lbs)
    sbit32 weight{0};                           ///< Current weight of a room/obj/char
    sbit16 capacity{0};                         ///< Capacity of obj/char/room, -1 => any
    ubit16 size{0};                             ///< (cm) MOBs height, weapons size, ropes length
    ubit8 status{0};                            ///< IS_ROOM, IS_OBJ, IS_PC, IS_NPC
    ubit8 open_flags{0};                        ///< In general OPEN will mean can "enter"?
    ubit8 open_diff{0};                         ///< Open difficulty
    sbit16 light{0};                            ///< Number of active light sources in unit
    sbit16 bright{0};                           ///< How much the unit shines
    sbit16 illum{0};                            ///< how much bright is by transparency
    ubit8 chars{0};                             ///< How many chars is inside the unit
    ubit8 minv{0};                              ///< Level of wizard invisible
    sbit32 max_hp{0};                           ///< The maximum number of hitpoints
    sbit32 hp{0};                               ///< The actual amount of hitpoints left
    sbit16 alignment{0};                        ///< +-1000 for alignments
    std::string title;                          ///< Room title, Char title, Obj "the barrel", NPC "the Beastly Fido"
    std::string out_descr;                      ///< The outside description of a unit
    std::string in_descr;                       ///< The inside description of a unit
    extra_list extra;                           ///< All the look 'at' stuff
};
