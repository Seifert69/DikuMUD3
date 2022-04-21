#include "unit_data.h"

#include "affect.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "mobact.h"
#include "slog.h"
#include "textutil.h"
#include "utils.h"
#ifdef VMC
    #include "vmc/vmc_process.h"
#endif

unit_data::unit_data(ubit8 unit_type)
    : m_func{nullptr}
    , m_affected{nullptr}
    , m_fi{nullptr}
    , m_key{nullptr}
    , m_outside{nullptr}
    , m_inside{nullptr}
    , m_next{nullptr}
    , m_gnext{nullptr}
    , m_gprevious{nullptr}
    , m_manipulate{0}
    , m_flags{0}
    , m_base_weight{0}
    , m_weight{0}
    , m_capacity{0}
    , m_size{0}
    , m_status{unit_type}
    , m_open_flags{0}
    , m_open_diff{0}
    , m_light{0}
    , m_bright{0}
    , m_illum{0}
    , m_chars{0}
    , m_minv{0}
    , m_max_hp{0}
    , m_hp{0}
    , m_alignment{0}
{
}

unit_data::~unit_data()
{
    /* Sanity due to wierd bug I saw (MS, 30/05-95) */

#ifdef DMSERVER
    assert(m_gnext == nullptr);
    assert(m_gprevious == nullptr);
    assert(m_next == nullptr);
    assert(g_unit_list != this);
#endif

    if (m_key)
    {
        FREE(m_key);
    }
#ifdef DMSERVER
    while (m_func)
    {
        destroy_fptr(this, m_func); /* Unlinks, no free */
    }

    while (m_affected)
    {
        unlink_affect(m_affected);
    }
#endif

    /* Call functions of the unit which have any data                     */
    /* that they might want to work on.                                   */

    if (IS_OBJ(this))
    {
        delete U_OBJ(this);
    }
    else if (IS_ROOM(this))
    {
        delete U_ROOM(this);
    }
    else if (IS_CHAR(this))
    {
        if (IS_NPC(this))
        {
            delete U_NPC(this);
        }
        else
        {
            delete U_PC(this);
        }

        delete U_CHAR(this);
    }
    else
    {
        assert(FALSE);
    }
}

void unit_data::set_fi(file_index_type *f)
{
    assert(f);

    if (m_fi)
    {
        slog(LOG_ALL, 0, "ERROR: FI was already set. This shouldn't happen");
    }

    m_fi = f;
    m_fi->IncrementNumInMemory();
}

std::string unit_data::json()
{
    std::string s;
    std::string t;

    t = UNIT_FI_NAME(this);
    t.append("@");
    t.append(UNIT_FI_ZONENAME(this));

    s = "{";
    s.append(str_json("idx", t));
    s.append(",\n");
    s.append(m_names.json());
    s.append(",\n");
    s.append(str_json("title", m_title));
    s.append(",\n");
    s.append(str_json("inside_descr", m_in_descr));
    s.append(",\n");
    s.append(str_json("outside_descr", m_out_descr));
    s.append(",\n");
    s.append(m_extra.json());
    s.append(",\n");

    s.append(str_json("manipulate", m_manipulate));
    s.append(",\n");
    s.append(str_json("flags", m_flags));
    s.append(",\n");
    s.append(str_json("baseweight", m_base_weight));
    s.append(",\n");
    s.append(str_json("capacity", m_capacity));
    s.append(",\n");
    s.append(str_json("size", m_size));
    s.append(",\n");

    s.append(str_json("key", m_key));
    s.append(",\n");
    s.append(str_json("openflags", m_open_flags));
    s.append(",\n");
    s.append(str_json("opendiff", m_open_diff));
    s.append(",\n");

    s.append(str_json("bright", m_bright));
    s.append(",\n");
    s.append(str_json("minv", m_minv));
    s.append(",\n");
    s.append(str_json("maxhp", m_max_hp));
    s.append(",\n");
    s.append(str_json("hp", m_hp));
    s.append(",\n");
    s.append(str_json("alignment", m_alignment));
    s.append(",\n");

    if (m_status == UNIT_ST_ROOM)
    {
        s.append("\"room\": {\n");
        s.append(str_json("roomflags", UROOM(this)->getRoomFlags()));
        s.append(",\n");
        s.append(str_json("movementtype", UROOM(this)->getLandscapeTerrain()));
        s.append(",\n");
        s.append(str_json("resistance", UROOM(this)->getRoomMagicalResistance()));
        s.append(",\n");
        s.append(str_json("mapx", UROOM(this)->getMapXCoordinate()));
        s.append(",\n");
        s.append(str_json("mapy", UROOM(this)->getMapYCoordinate()));

        for (int i = 0; i < MAX_EXIT + 1; i++)
        {
            if (UROOM(this)->getRoomDirectionDataForExit(i))
            {
                s.append(",\n");
                s.append(str_json_encode_quote(g_dirs[i]));
                s.append(": {\n");
                if (UROOM(this)->getRoomDirectionDataForExit(i)->getToRoom())
                {
                    t = UNIT_FI_NAME(ROOM_EXIT(this, i)->getToRoom());
                    t.append("@");
                    t.append(UNIT_FI_ZONENAME(UROOM(this)->getRoomDirectionDataForExit(i)->getToRoom()));
                    s.append(str_json("toroom", t));
                    s.append(",\n");
                }
                s.append(UROOM(this)->getRoomDirectionDataForExit(i)->getOpenName().json());
                s.append(",\n");
                s.append(str_json("difficulty", UROOM(this)->getRoomDirectionDataForExit(i)->getSkillDifficulty()));
                s.append(",\n");
                s.append(str_json("exitflags", UROOM(this)->getRoomDirectionDataForExit(i)->getDoorFlags()));
                s.append(",\n");
                s.append(str_json("key", UROOM(this)->getRoomDirectionDataForExit(i)->getKey()));
                s.append("\n");
                s.append("}\n");
            }
        }

        s.append("}\n");
    }

    s.append("\n}\n");

    return s;
}

#ifndef VMC_SRC
unit_data *unit_data::copy()
{
    unit_data *u = nullptr;
    int x = 0;

    u = new_unit_data(m_status);

    CByteBuffer abuf;
    CByteBuffer fbuf;
    u->m_names.CopyList(&m_names);
    u->m_chars = m_chars;
    u->m_manipulate = m_manipulate;
    u->m_flags = m_flags;
    u->m_base_weight = m_base_weight;
    u->m_weight = m_weight;
    u->m_capacity = m_capacity;
    u->m_open_flags = m_open_flags;
    u->m_open_diff = m_open_diff;
    u->m_light = m_light;
    u->m_bright = m_bright;
    u->m_illum = m_illum;
    u->m_chars = m_chars;
    u->m_minv = m_minv;
    u->m_max_hp = m_max_hp;
    u->m_hp = m_hp;
    u->m_alignment = m_alignment;
    u->m_key = str_dup(m_key);
    u->set_fi(m_fi);

    bwrite_affect(&abuf, m_affected, 61); // WTF 61?
    bread_affect(&abuf, u, 61);
    bwrite_func(&fbuf, m_func);
    u->setFunctionPointer(bread_func(&fbuf, 61, u, TRUE)); // WTF 61?

    u->m_title = m_title;
    u->m_out_descr = m_out_descr;
    u->m_in_descr = m_in_descr;
    u->getExtraList().copyfrom(m_extra);

    if (IS_ROOM(this))
    {
        room_data *thisroom = UROOM(this);
        room_data *uroom = UROOM(u);

        uroom->setRoomMagicalResistance(thisroom->getRoomMagicalResistance());
        uroom->setLandscapeTerrain(thisroom->getLandscapeTerrain());
        uroom->setAllRoomFlags(thisroom->getRoomFlags());
        for (x = 0; x < MAX_EXIT + 1; x++)
        {
            uroom->getRoomDirectionDataForExit(x)->getOpenName() = thisroom->getRoomDirectionDataForExit(x)->getOpenName();
            uroom->getRoomDirectionDataForExit(x)->setKey(str_dup(thisroom->getRoomDirectionDataForExit(x)->getKey()));
            uroom->getRoomDirectionDataForExit(x)->setDoorFlags(thisroom->getRoomDirectionDataForExit(x)->getDoorFlags());
            uroom->getRoomDirectionDataForExit(x)->setSkillDifficulty(thisroom->getRoomDirectionDataForExit(x)->getSkillDifficulty());
        }
    }
    else if (IS_OBJ(this))
    {
        obj_data *thisobj = UOBJ(this);
        obj_data *uobj = UOBJ(u);

        for (size_t x = 0; x < uobj->getValueArraySize(); x++)
        {
            uobj->setValueAtIndexTo(x, thisobj->getValueAtIndex(x));
        }

        uobj->setPriceInGP(thisobj->getPriceInGP());
        uobj->setPricePerDay(thisobj->getPricePerDay());
        uobj->setAllObjectFlags(thisobj->getObjectFlags());
        uobj->type = thisobj->type;
        uobj->equip_pos = thisobj->equip_pos;
        uobj->resistance = thisobj->resistance;
    }
    else if (IS_CHAR(this))
    {
        /// @todo These should all move down to char_data
        auto *u_downcast = dynamic_cast<char_data *>(u);
        auto *this_downcast = dynamic_cast<char_data *>(this);

        u_downcast->points.setAllCharacterFlags(this_downcast->points.getCharacterFlags());
        u_downcast->points.setPlayerExperience(this_downcast->points.getPlayerExperience());
        u_downcast->points.setMana(this_downcast->points.getMana());
        u_downcast->points.setEndurance(this_downcast->points.getEndurance());
        u_downcast->points.setRace(this_downcast->points.getRace());
        u_downcast->points.setOffensiveBonus(this_downcast->points.getOffensiveBonus());
        u_downcast->points.setDefensiveBonus(this_downcast->points.getDefensiveBonus());
        u_downcast->points.setSpeed(this_downcast->points.getSpeed());
        u_downcast->points.setNaturalArmor(this_downcast->points.getNaturalArmor());
        u_downcast->points.setAttackType(this_downcast->points.getAttackType());
        u_downcast->points.setSex(this_downcast->points.getSex());
        u_downcast->points.setLevel(this_downcast->points.getLevel());
        u_downcast->points.setPosition(this_downcast->points.getPosition());
        for (x = 0; x < ABIL_TREE_MAX; x++)
        {
            u_downcast->points.setAbilityAtIndexTo(x, this_downcast->points.getAbilityAtIndex(x));
        }
        if (IS_PC(this))
        {
            ;
            // Put in PC Copy stuff here
        }
        else
        {
            for (x = 0; x < WPN_GROUP_MAX; x++)
            {
                U_NPC(u)->weapons[x] = U_NPC(this)->weapons[x];
            }
            for (x = 0; x < SPL_GROUP_MAX; x++)
            {
                U_NPC(u)->spells[x] = U_NPC(this)->spells[x];
            }
            U_NPC(u)->default_pos = U_NPC(this)->default_pos;
            U_NPC(u)->flags = U_NPC(this)->flags;
        }
    }
    else
    {
        assert(FALSE);
    }
    insert_in_unit_list(u); /* Put unit into the g_unit_list      */
    apply_affect(u);        /* Set all affects that modify      */
    start_all_special(u);

    return u;
}
#endif

int unit_data::destruct_classindex()
{
    return DR_UNIT;
}

cNamelist &unit_data::getNames()
{
    return m_names;
}

const cNamelist &unit_data::getNames() const
{
    return m_names;
}

unit_fptr *unit_data::getFunctionPointer()
{
    return m_func;
}

void unit_data::setFunctionPointer(unit_fptr *value)
{
    m_func = value;
}

unit_affected_type *unit_data::getUnitAffectedType()
{
    return m_affected;
}

void unit_data::setUnitAffectedType(unit_affected_type *value)
{
    m_affected = value;
}

file_index_type *unit_data::getFileIndex()
{
    return m_fi;
}

const file_index_type *unit_data::getFileIndex() const
{
    return m_fi;
}

void unit_data::setFileIndex(file_index_type *value)
{
    m_fi = value;
}

const char *unit_data::getKey() const
{
    return m_key;
}

char **unit_data::getKeyPtr()
{
    return &m_key;
}

void unit_data::setKey(char *value)
{
    m_key = value;
}

unit_data *unit_data::getMyContainer()
{
    return m_outside;
}

const unit_data *unit_data::getMyContainer() const
{
    return m_outside;
}

void unit_data::setMyContainerTo(unit_data *value)
{
    m_outside = value;
}

const unit_data *unit_data::getContainedUnits() const
{
    return m_inside;
}

unit_data *unit_data::getContainedUnits()
{
    return m_inside;
}

void unit_data::setContainedUnit(unit_data *value)
{
    m_inside = value;
}

const unit_data *unit_data::getNext() const
{
    return m_next;
}

unit_data *unit_data::getNext()
{
    return m_next;
}

void unit_data::setNext(unit_data *value)
{
    m_next = value;
}

unit_data *unit_data::getGlobalNext()
{
    return m_gnext;
}

const unit_data *unit_data::getGlobalNext() const
{
    return m_gnext;
}

void unit_data::setGlobalNext(unit_data *value)
{
    m_gnext = value;
}

unit_data *unit_data::getGlobalPrevious()
{
    return m_gprevious;
}

const unit_data *unit_data::getGlobalPrevious() const
{
    return m_gprevious;
}

void unit_data::setGlobalPrevious(unit_data *value)
{
    m_gprevious = value;
}

ubit32 unit_data::getManipulate() const
{
    return m_manipulate;
}

ubit32 *unit_data::getManipulatePtr()
{
    return &m_manipulate;
}

void unit_data::setAllManipulateFlags(ubit32 value)
{
    m_manipulate = value;
}

void unit_data::setManipulateFlag(ubit32 value)
{
    m_manipulate |= value;
}

ubit16 unit_data::getUnitFlags() const
{
    return m_flags;
}

ubit16 *unit_data::getUnitFlagsPtr()
{
    return &m_flags;
}

void unit_data::setAllUnitFlags(ubit16 value)
{
    m_flags = value;
}

void unit_data::setUnitFlag(ubit16 value)
{
    m_flags |= value;
}

void unit_data::removeUnitFlag(ubit16 value)
{
    m_flags &= ~value;
}

sbit32 unit_data::getBaseWeight() const
{
    return m_base_weight;
}

void unit_data::setBaseWeight(sbit32 value)
{
    m_base_weight = value;
}

sbit32 unit_data::getWeight() const
{
    return m_weight;
}

void unit_data::reduceWeightBy(sbit32 value)
{
    m_weight -= value;
}

void unit_data::increaseWeightBy(sbit32 value)
{
    m_weight += value;
}

void unit_data::setWeight(sbit32 value)
{
    m_weight = value;
}

sbit16 unit_data::getCapacity() const
{
    return m_capacity;
}

sbit16 *unit_data::getCapacityPtr()
{
    return &m_capacity;
}

void unit_data::setCapacity(sbit16 value)
{
    m_capacity = value;
}

ubit16 unit_data::getSize() const
{
    return m_size;
}

ubit16 *unit_data::getSizePtr()
{
    return &m_size;
}

void unit_data::setSize(ubit16 value)
{
    m_size = value;
}

void unit_data::increaseSizeBy(ubit16 value)
{
    m_size += value;
}

ubit8 unit_data::getUnitType() const
{
    return m_status;
}

ubit8 unit_data::getOpenFlags() const
{
    return m_open_flags;
}

ubit8 *unit_data::getOpenFlagsPtr()
{
    return &m_open_flags;
}

void unit_data::setAllOpenFlags(ubit8 value)
{
    m_open_flags = value;
}

void unit_data::setOpenFlag(ubit8 value)
{
    m_open_flags |= value;
}

ubit8 unit_data::getOpenDifficulty() const
{
    return m_open_diff;
}

ubit8 *unit_data::getOpenDifficultyPtr()
{
    return &m_open_diff;
}

void unit_data::setOpenDifficulty(ubit8 value)
{
    m_open_diff = value;
}

sbit16 unit_data::getNumberOfActiveLightSources() const
{
    return m_light;
}

void unit_data::changeNumberOfActiveLightSourcesBy(sbit16 value)
{
    m_light += value;
}

void unit_data::setNumberOfActiveLightSources(sbit16 value)
{
    m_light = value;
}

sbit16 unit_data::getLightOutput() const
{
    return m_bright;
}

void unit_data::setLightOutput(sbit16 value)
{
    m_bright = value;
}

void unit_data::changeLightOutputBy(sbit16 value)
{
    m_bright += value;
}

sbit16 unit_data::getTransparentLightOutput() const
{
    return m_illum;
}

void unit_data::setTransparentLightOutput(sbit16 value)
{
    m_illum = value;
}

void unit_data::changeTransparentLightOutputBy(sbit16 value)
{
    m_illum += value;
}

ubit8 unit_data::getNumberOfCharactersInsideUnit() const
{
    return m_chars;
}

void unit_data::decrementNumberOfCharactersInsideUnit()
{
    --m_chars;
}

void unit_data::incrementNumberOfCharactersInsideUnit()
{
    ++m_chars;
}

void unit_data::setNumberOfCharactersInsideUnit(ubit8 value)
{
    m_chars = value;
}

ubit8 unit_data::getLevelOfWizardInvisibility() const
{
    return m_minv;
}

ubit8 *unit_data::getLevelOfWizardInvisibilityPtr()
{
    return &m_minv;
}

void unit_data::setLevelOfWizardInvisibility(ubit8 value)
{
    m_minv = value;
}

sbit32 unit_data::getMaximumHitpoints() const
{
    return m_max_hp;
}

sbit32 *unit_data::getMaximumHitpointsPtr()
{
    return &m_max_hp;
}

void unit_data::setMaximumHitpoints(sbit32 value)
{
    m_max_hp = value;
}

sbit32 unit_data::getCurrentHitpoints() const
{
    return m_hp;
}

sbit32 *unit_data::getCurrentHitpointsPtr()
{
    return &m_hp;
}

void unit_data::setCurrentHitpoints(sbit32 value)
{
    m_hp = value;
}

void unit_data::changeCurrentHitpointsBy(sbit32 value)
{
    m_hp += value;
}

sbit16 unit_data::getAlignment() const
{
    return m_alignment;
}

sbit16 *unit_data::getAlignmentPtr()
{
    return &m_alignment;
}

void unit_data::changeAlignmentBy(int64_t value)
{
    setAlignment(m_alignment + value);
}

bool unit_data::setAlignment(int64_t value)
{
    bool change_is_ok = true;

    m_alignment = static_cast<sbit16>(value);

    if (m_alignment > MaxAlignment)
    {
        m_alignment = MaxAlignment;
        change_is_ok = false;
    }
    else if (m_alignment < MinAlignment)
    {
        m_alignment = MinAlignment;
        change_is_ok = false;
    }

    return change_is_ok;
}

const std::string &unit_data::getTitle() const
{
    return m_title;
}

std::string *unit_data::getTitlePtr()
{
    return &m_title;
}

void unit_data::setTitle(std::string value)
{
    m_title = std::move(value);
}

const std::string &unit_data::getDescriptionOfOutside() const
{
    return m_out_descr;
}

std::string *unit_data::getDescriptionOfOutsidePtr()
{
    return &m_out_descr;
}

void unit_data::setDescriptionOfOutside(std::string value)
{
    m_out_descr = std::move(value);
}

const std::string &unit_data::getDescriptionOfInside() const
{
    return m_in_descr;
}

std::string *unit_data::getDescriptionOfInsidePtr()
{
    return &m_in_descr;
}

void unit_data::setDescriptionOfInside(std::string value)
{
    m_in_descr = std::move(value);
}

const extra_list &unit_data::getExtraList() const
{
    return m_extra;
}

extra_list &unit_data::getExtraList()
{
    return m_extra;
}
