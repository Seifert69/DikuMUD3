#include "unit_data.h"

#include "affect.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "mobact.h"
#include "slog.h"
#include "textutil.h"
#include "utils.h"

unit_data::unit_data(ubit8 unit_type)
    : func{nullptr}
    , affected{nullptr}
    , fi{nullptr}
    , key{nullptr}
    , outside{nullptr}
    , inside{nullptr}
    , next{nullptr}
    , gnext{nullptr}
    , gprevious{nullptr}
    , manipulate{0}
    , flags{0}
    , base_weight{0}
    , weight{0}
    , capacity{0}
    , size{0}
    , status{unit_type}
    , open_flags{0}
    , open_diff{0}
    , light{0}
    , bright{0}
    , illum{0}
    , chars{0}
    , minv{0}
    , max_hp{0}
    , hp{0}
    , alignment{0}
{
}

unit_data::~unit_data()
{
    /* Sanity due to wierd bug I saw (MS, 30/05-95) */

#ifdef DMSERVER
    assert(gnext == nullptr);
    assert(gprevious == nullptr);
    assert(next == nullptr);
    assert(g_unit_list != this);
#endif

    if (key)
        FREE(key);
#ifdef DMSERVER
    while (UNIT_FUNC(this))
    {
        destroy_fptr(this, UNIT_FUNC(this)); /* Unlinks, no free */
    }

    while (UNIT_AFFECTED(this))
    {
        unlink_affect(UNIT_AFFECTED(this));
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

    if (this->fi)
    {
        slog(LOG_ALL, 0, "ERROR: FI was already set. This shouldn't happen");
    }

    this->fi = f;
    this->fi->IncrementNumInMemory();
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
    s.append(this->names.json());
    s.append(",\n");
    s.append(str_json("title", this->title));
    s.append(",\n");
    s.append(str_json("inside_descr", this->in_descr));
    s.append(",\n");
    s.append(str_json("outside_descr", this->out_descr));
    s.append(",\n");
    s.append(this->extra.json());
    s.append(",\n");

    s.append(str_json("manipulate", this->manipulate));
    s.append(",\n");
    s.append(str_json("flags", this->flags));
    s.append(",\n");
    s.append(str_json("baseweight", this->base_weight));
    s.append(",\n");
    s.append(str_json("capacity", this->capacity));
    s.append(",\n");
    s.append(str_json("size", this->size));
    s.append(",\n");

    s.append(str_json("key", this->key));
    s.append(",\n");
    s.append(str_json("openflags", this->open_flags));
    s.append(",\n");
    s.append(str_json("opendiff", this->open_diff));
    s.append(",\n");

    s.append(str_json("bright", this->bright));
    s.append(",\n");
    s.append(str_json("minv", this->minv));
    s.append(",\n");
    s.append(str_json("maxhp", this->max_hp));
    s.append(",\n");
    s.append(str_json("hp", this->hp));
    s.append(",\n");
    s.append(str_json("alignment", this->alignment));
    s.append(",\n");

    if (UNIT_TYPE(this) == UNIT_ST_ROOM)
    {
        s.append("\"room\": {\n");
        s.append(str_json("roomflags", UROOM(this)->flags));
        s.append(",\n");
        s.append(str_json("movementtype", UROOM(this)->movement_type));
        s.append(",\n");
        s.append(str_json("resistance", UROOM(this)->resistance));
        s.append(",\n");
        s.append(str_json("mapx", UROOM(this)->mapx));
        s.append(",\n");
        s.append(str_json("mapy", UROOM(this)->mapy));

        for (int i = 0; i < MAX_EXIT + 1; i++)
        {
            if (UROOM(this)->dir_option[i])
            {
                s.append(",\n");
                s.append(str_json_encode_quote(g_dirs[i]));
                s.append(": {\n");
                if (UROOM(this)->dir_option[i]->getToRoom())
                {
                    t = UNIT_FI_NAME(ROOM_EXIT(this, i)->getToRoom());
                    t.append("@");
                    t.append(UNIT_FI_ZONENAME(UROOM(this)->dir_option[i]->getToRoom()));
                    s.append(str_json("toroom", t));
                    s.append(",\n");
                }
                s.append(UROOM(this)->dir_option[i]->getOpenName().json());
                s.append(",\n");
                s.append(str_json("difficulty", UROOM(this)->dir_option[i]->getSkillDifficulty()));
                s.append(",\n");
                s.append(str_json("exitflags", UROOM(this)->dir_option[i]->getDoorFlags()));
                s.append(",\n");
                s.append(str_json("key", UROOM(this)->dir_option[i]->getKey()));
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

    u = new_unit_data(status);

    CByteBuffer abuf;
    CByteBuffer fbuf;
    u->names.CopyList(&names);
    u->chars = chars;
    u->manipulate = manipulate;
    u->flags = flags;
    u->base_weight = base_weight;
    u->weight = weight;
    u->capacity = capacity;
    u->open_flags = open_flags;
    u->open_diff = open_diff;
    u->light = light;
    u->bright = bright;
    u->illum = illum;
    u->chars = chars;
    u->minv = minv;
    u->max_hp = max_hp;
    u->hp = hp;
    u->alignment = alignment;
    u->key = str_dup(key);
    u->set_fi(fi);

    bwrite_affect(&abuf, UNIT_AFFECTED(this), 61); // WTF 61?
    bread_affect(&abuf, u, 61);
    bwrite_func(&fbuf, UNIT_FUNC(this));
    u->setFunctionPointer(bread_func(&fbuf, 61, u, TRUE)); // WTF 61?

    u->title = title;
    u->out_descr = out_descr;
    u->in_descr = in_descr;
    UNIT_EXTRA(u).copyfrom(UNIT_EXTRA(this));

    if (IS_ROOM(this))
    {
        room_data *thisroom = UROOM(this);
        room_data *uroom = UROOM(u);

        uroom->resistance = thisroom->resistance;
        uroom->movement_type = thisroom->movement_type;
        uroom->flags = thisroom->flags;
        for (x = 0; x < MAX_EXIT + 1; x++)
        {
            uroom->dir_option[x]->getOpenName() = thisroom->dir_option[x]->getOpenName();
            uroom->dir_option[x]->setKey(str_dup(thisroom->dir_option[x]->getKey()));
            uroom->dir_option[x]->setDoorFlags(thisroom->dir_option[x]->getDoorFlags());
            uroom->dir_option[x]->setSkillDifficulty(thisroom->dir_option[x]->getSkillDifficulty());
        }
    }
    else if (IS_OBJ(this))
    {
        obj_data *thisobj = UOBJ(this);
        obj_data *uobj = UOBJ(u);

        for (x = 0; x < 5; x++)
        {
            uobj->value[x] = thisobj->value[x];
        }

        uobj->cost = thisobj->cost;
        uobj->cost_per_day = thisobj->cost_per_day;
        uobj->flags = thisobj->flags;
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
    return names;
}

const cNamelist &unit_data::getNames() const
{
    return names;
}

unit_fptr *unit_data::getFunctionPointer()
{
    return func;
}

void unit_data::setFunctionPointer(unit_fptr *value)
{
    func = value;
}

unit_affected_type *unit_data::getUnitAffectedType()
{
    return affected;
}

void unit_data::setUnitAffectedType(unit_affected_type *value)
{
    affected = value;
}

file_index_type *unit_data::getFileIndex()
{
    return fi;
}

const file_index_type *unit_data::getFileIndex() const
{
    return fi;
}

void unit_data::setFileIndex(file_index_type *value)
{
    fi = value;
}

const char *unit_data::getKey() const
{
    return key;
}

char **unit_data::getKeyPtr()
{
    return &key;
}

void unit_data::setKey(char *value)
{
    key = value;
}

unit_data *unit_data::getMyContainer()
{
    return outside;
}

const unit_data *unit_data::getMyContainer() const
{
    return outside;
}

void unit_data::setMyContainerTo(unit_data *value)
{
    outside = value;
}

const unit_data *unit_data::getContainedUnits() const
{
    return inside;
}

unit_data *unit_data::getContainedUnits()
{
    return inside;
}

void unit_data::setContainedUnit(unit_data *value)
{
    inside = value;
}

const unit_data *unit_data::getNext() const
{
    return next;
}

unit_data *unit_data::getNext()
{
    return next;
}

void unit_data::setNext(unit_data *value)
{
    next = value;
}

unit_data *unit_data::getGlobalNext()
{
    return gnext;
}

const unit_data *unit_data::getGlobalNext() const
{
    return gnext;
}

void unit_data::setGlobalNext(unit_data *value)
{
    gnext = value;
}

unit_data *unit_data::getGlobalPrevious()
{
    return gprevious;
}

const unit_data *unit_data::getGlobalPrevious() const
{
    return gprevious;
}

void unit_data::setGlobalPrevious(unit_data *value)
{
    gprevious = value;
}

ubit32 unit_data::getManipulate() const
{
    return manipulate;
}

ubit32 *unit_data::getManipulatePtr()
{
    return &manipulate;
}

void unit_data::setAllManipulateFlags(ubit32 value)
{
    manipulate = value;
}

void unit_data::setManipulateFlag(ubit32 value)
{
    manipulate |= value;
}

ubit16 unit_data::getUnitFlags() const
{
    return flags;
}

ubit16 *unit_data::getUnitFlagsPtr()
{
    return &flags;
}

void unit_data::setAllUnitFlags(ubit16 value)
{
    flags = value;
}

void unit_data::setUnitFlag(ubit16 value)
{
    flags |= value;
}

void unit_data::removeUnitFlag(ubit16 value)
{
    flags &= ~value;
}

sbit32 unit_data::getBaseWeight() const
{
    return base_weight;
}

void unit_data::setBaseWeight(sbit32 value)
{
    base_weight = value;
}

sbit32 unit_data::getWeight() const
{
    return weight;
}

void unit_data::reduceWeightBy(sbit32 value)
{
    weight -= value;
}

void unit_data::increaseWeightBy(sbit32 value)
{
    weight += value;
}

void unit_data::setWeight(sbit32 value)
{
    weight = value;
}

sbit16 unit_data::getCapacity() const
{
    return capacity;
}

sbit16 *unit_data::getCapacityPtr()
{
    return &capacity;
}

void unit_data::setCapacity(sbit16 value)
{
    capacity = value;
}

ubit16 unit_data::getSize() const
{
    return size;
}

ubit16 *unit_data::getSizePtr()
{
    return &size;
}

void unit_data::setSize(ubit16 value)
{
    size = value;
}

void unit_data::increaseSizeBy(ubit16 value)
{
    size += value;
}

ubit8 unit_data::getUnitType() const
{
    return status;
}

ubit8 unit_data::getOpenFlags() const
{
    return open_flags;
}

ubit8 *unit_data::getOpenFlagsPtr()
{
    return &open_flags;
}

void unit_data::setAllOpenFlags(ubit8 value)
{
    open_flags = value;
}

void unit_data::setOpenFlag(ubit8 value)
{
    open_flags |= value;
}

ubit8 unit_data::getOpenDifficulty() const
{
    return open_diff;
}

ubit8 *unit_data::getOpenDifficultyPtr()
{
    return &open_diff;
}

void unit_data::setOpenDifficulty(ubit8 value)
{
    open_diff = value;
}

sbit16 unit_data::getNumberOfActiveLightSources() const
{
    return light;
}

void unit_data::changeNumberOfActiveLightSourcesBy(sbit16 value)
{
    light += value;
}

void unit_data::setNumberOfActiveLightSources(sbit16 value)
{
    light = value;
}

sbit16 unit_data::getLightOutput() const
{
    return bright;
}

void unit_data::setLightOutput(sbit16 value)
{
    bright = value;
}

void unit_data::changeLightOutputBy(sbit16 value)
{
    bright += value;
}

sbit16 unit_data::getTransparentLightOutput() const
{
    return illum;
}

void unit_data::setTransparentLightOutput(sbit16 value)
{
    illum = value;
}

void unit_data::changeTransparentLightOutputBy(sbit16 value)
{
    illum += value;
}

ubit8 unit_data::getNumberOfCharactersInsideUnit() const
{
    return chars;
}

void unit_data::decrementNumberOfCharactersInsideUnit()
{
    --chars;
}

void unit_data::incrementNumberOfCharactersInsideUnit()
{
    ++chars;
}

void unit_data::setNumberOfCharactersInsideUnit(ubit8 value)
{
    chars = value;
}

ubit8 unit_data::getLevelOfWizardInvisibility() const
{
    return minv;
}

ubit8 *unit_data::getLevelOfWizardInvisibilityPtr()
{
    return &minv;
}

void unit_data::setLevelOfWizardInvisibility(ubit8 value)
{
    minv = value;
}

sbit32 unit_data::getMaximumHitpoints() const
{
    return max_hp;
}

sbit32 *unit_data::getMaximumHitpointsPtr()
{
    return &max_hp;
}

void unit_data::setMaximumHitpoints(sbit32 value)
{
    max_hp = value;
}

sbit32 unit_data::getCurrentHitpoints() const
{
    return hp;
}

sbit32 *unit_data::getCurrentHitpointsPtr()
{
    return &hp;
}

void unit_data::setCurrentHitpoints(sbit32 value)
{
    hp = value;
}

void unit_data::changeCurrentHitpointsBy(sbit32 value)
{
    hp += value;
}

sbit16 unit_data::getAlignment() const
{
    return alignment;
}

sbit16 *unit_data::getAlignmentPtr()
{
    return &alignment;
}

void unit_data::increaseAlignmentBy(sbit16 value)
{
    alignment += value;
}

void unit_data::decreaseAlignmentBy(sbit16 value)
{
    alignment -= value;
}

void unit_data::setAlignment(sbit16 value)
{
    alignment = value;
}

const std::string &unit_data::getTitle() const
{
    return title;
}

std::string *unit_data::getTitlePtr()
{
    return &title;
}

void unit_data::setTitle(std::string value)
{
    title = std::move(value);
}

const std::string &unit_data::getDescriptionOfOutside() const
{
    return out_descr;
}

std::string *unit_data::getDescriptionOfOutsidePtr()
{
    return &out_descr;
}

void unit_data::setDescriptionOfOutside(std::string value)
{
    out_descr = std::move(value);
}

const std::string &unit_data::getDescriptionOfInside() const
{
    return in_descr;
}

std::string *unit_data::getDescriptionOfInsidePtr()
{
    return &in_descr;
}

void unit_data::setDescriptionOfInside(std::string value)
{
    in_descr = std::move(value);
}

const extra_list &unit_data::getExtraList() const
{
    return extra;
}

extra_list &unit_data::getExtraList()
{
    return extra;
}
