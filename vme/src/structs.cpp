/*
 $Author: All $
 $RCSfile: structs.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.8 $
 */

#include "structs.h"

#include "affect.h"
#include "color.h"
#include "db.h"
#include "db_file.h"
#include "handler.h"
#include "mobact.h"
#include "slog.h"
#include "textutil.h"
#include "utils.h"
#include "zone_reset_cmd.h"

int g_world_norooms = 0;   /* number of rooms in the world   */
int g_world_noobjects = 0; /* number of objects in the world */
int g_world_nochars = 0;   /* number of chars in the world   */
int g_world_nonpc = 0;     /* number of chars in the world   */
int g_world_nopc = 0;      /* number of chars in the world   */
int g_world_nozones = 0;   /* number of zones in the world   */

/* Descriptor stuff is in system.c */

char_data::char_data(ubit8 unit_type)
    : unit_data(unit_type)
{
    g_world_nochars++;

    money = nullptr;
    descriptor = nullptr;
    Combat = nullptr;
    followers = nullptr;
    master = nullptr;
    last_room = nullptr;
    last_attacker = nullptr;
    last_attacker_type = 0;

    // MS2020 memset(&points, 0, sizeof (points));
    // Removed because class constructor already initialize class variables.
}

char_data::~char_data()
{
#ifdef DMSERVER
    if (money)
    {
        FREE(money);
    }
    if (last_attacker)
    {
        FREE(last_attacker);
    }
#endif
    g_world_nochars--;
}

room_data::room_data()
    : unit_data(UNIT_ST_ROOM)
{
    g_world_norooms++;
    mapx = -1;
    mapy = -1;
#ifdef DMSERVER
    waiting_dijkstra = FALSE;
#endif
}

room_data::~room_data()
{
    g_world_norooms--;

    for (int i = 0; i < MAX_EXIT + 1; i++)
    {
        if (dir_option[i])
        {
            delete dir_option[i];
        }
    }
}

obj_data::obj_data()
    : unit_data(UNIT_ST_OBJ)
{
    g_world_noobjects++;

    memset(value, 0, sizeof(value));
    cost = 0;
    cost_per_day = 0;
    flags = 0;
    type = 0;
    equip_pos = 0;
    resistance = 0;
}

obj_data::~obj_data()
{
    g_world_noobjects--;
}

pc_data::pc_data()
    : char_data(UNIT_ST_PC)
{
    g_world_nopc++;

    bank = nullptr;
    guild = nullptr;
    hometown = nullptr;
    promptstr = nullptr;

    memset(&setup, 0, sizeof(setup));

    profession = -1;
    vlvl = 0;
    id = -1;

    skill_points = 0;
    ability_points = 0;
    flags = 0;

    nr_of_crimes = 0;
    crack_attempts = 0;
    memset(spells, 0, sizeof(spells));
    memset(spell_lvl, 0, sizeof(spell_lvl));
    memset(skills, 0, sizeof(skills));
    memset(skill_lvl, 0, sizeof(skill_lvl));
    memset(weapons, 0, sizeof(weapons));
    memset(weapon_lvl, 0, sizeof(weapon_lvl));
    memset(ability_lvl, 0, sizeof(ability_lvl));

    memset(conditions, 0, sizeof(conditions));
#ifdef DMSERVER
    memset(lasthosts, 0, sizeof(lasthosts));
#endif
    nAccessLevel = 0;
    pwd[0] = 0;
    filename[0] = 0;
}

pc_data::~pc_data()
{
    g_world_nopc--;

    if (guild)
        FREE(guild);

    if (hometown)
        FREE(hometown);

    if (bank)
        FREE(bank);

    if (promptstr)
        FREE(promptstr);
}

npc_data::npc_data()
    : char_data(UNIT_ST_NPC)
{
    g_world_nonpc++;

    memset(weapons.data(), 0, weapons.size());
    memset(spells.data(), 0, spells.size());
    default_pos = POSITION_STANDING;
    flags = 0;
}

npc_data::~npc_data()
{
    g_world_nonpc--;
}

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
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
        {
            if ((UNIT_FILE_INDEX(u) == this) && UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_INVEN))
    {
        for (u = UNIT_CONTAINS(ref); u; u = u->next)
        {
            if ((UNIT_FILE_INDEX(u) == this) && !UNIT_IS_EQUIPPED(u))
            {
                return u;
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_SURRO) && UNIT_IN(ref))
    {
        if (this == UNIT_FILE_INDEX(UNIT_IN(ref)))
        {
            return UNIT_IN(ref);
        }

        /* Run through units in local environment */
        for (u = UNIT_CONTAINS(UNIT_IN(ref)); u; u = u->next)
        {
            if (UNIT_FILE_INDEX(u) == this)
            {
                return u;
            }

            /* check tranparancy */
            if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
            {
                for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                {
                    if (UNIT_FILE_INDEX(uu) == this)
                    {
                        return uu;
                    }
                }
            }
        }

        /* Run through units in local environment if upwards transparent */
        if ((u = UNIT_IN(UNIT_IN(ref))) && UNIT_IS_TRANSPARENT(UNIT_IN(ref)))
        {
            for (u = UNIT_CONTAINS(u); u; u = u->next)
            {
                if (u != UNIT_IN(ref))
                {
                    if (this == UNIT_FILE_INDEX(u))
                    {
                        return u;
                    }

                    /* check down into transparent unit */
                    if (UNIT_CHARS(u) && UNIT_IS_TRANSPARENT(u))
                    {
                        for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                        {
                            if (this == UNIT_FILE_INDEX(uu))
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
        if (!fi_unit_list.empty())
        {
            for (auto it = fi_unit_list.begin(); it != fi_unit_list.end(); it++)
            {
                if (UNIT_FILE_INDEX(*it) == this)
                {
                    return u;
                }
            }
        }
    }

    if (IS_SET(bitvector, FIND_UNIT_WORLD))
    {
        if (!fi_unit_list.empty())
        {
            return fi_unit_list.front();
        }
    }

    return nullptr;
}

unit_data *file_index_type::find_symbolic_instance()
{
    if (!fi_unit_list.empty())
    {
        return fi_unit_list.front();
    }

    return nullptr;
}

const char *file_index_type::getName() const
{
    return name.c_str();
}

long file_index_type::getFilepos() const
{
    return filepos;
}

ubit32 file_index_type::getCRC() const
{
    return crc;
}

sbit16 file_index_type::getNumInZone() const
{
    return no_in_zone;
}

ubit16 file_index_type::getRoomNum() const
{
    return room_no;
}

ubit8 file_index_type::getType() const
{
    return type;
}

ubit16 file_index_type::getNumInMem() const
{
    return no_in_mem;
}

ubit32 file_index_type::getLength() const
{
    return length;
}

void file_index_type::IncrementNumInMemory()
{
    ++no_in_mem;
}

zone_type *file_index_type::getZone() const
{
    return zone;
}

void file_index_type::setType(ubit8 value)
{
    type = value;
}

void file_index_type::setZone(zone_type *value)
{
    zone = value;
}

void file_index_type::setName(const char *value, bool to_lower)
{
    if (value)
    {
        name = std::string(value);
        if (to_lower)
        {
            str_lower(name);
        }
    }
}

void file_index_type::DecrementNumInMemory()
{
    // TODO find out why code is trying to decrement no_in_mem below 0
    // I tried this but somewhere it goes below zero
    //    assert(no_in_mem > 0);
    if (no_in_mem > 0)
    {
        --no_in_mem;
    }
}

void file_index_type::IncrementNumInZone()
{
    ++no_in_zone;
}

void file_index_type::setNumInZone(sbit16 value)
{
    no_in_zone = value;
}

void file_index_type::setLength(ubit32 value)
{
    length = value;
}

void file_index_type::setFilepos(long value)
{
    filepos = value;
}

void file_index_type::setRoomNum(ubit16 value)
{
    room_no = value;
}

void file_index_type::setNumInMemory(sbit16 value)
{
    no_in_mem = value;
}

void file_index_type::setCRC(ubit32 value)
{
    crc = value;
}

bool file_index_type::Empty() const
{
    return fi_unit_list.empty();
}

unit_data *file_index_type::Front() const
{
    return fi_unit_list.front();
}

std::forward_list<unit_data *>::iterator file_index_type::Begin()
{
    return fi_unit_list.begin();
}

std::forward_list<unit_data *>::iterator file_index_type::End()
{
    return fi_unit_list.end();
}

void file_index_type::PushFront(unit_data *value)
{
    fi_unit_list.push_front(value);
}

void file_index_type::Remove(unit_data *value)
{
    fi_unit_list.remove(value);
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
    UNIT_FUNC(u) = bread_func(&fbuf, 61, u, TRUE); // WTF 61?

    u->title = title;
    UNIT_OUT_DESCR(u) = UNIT_OUT_DESCR(this);
    UNIT_IN_DESCR(u) = UNIT_IN_DESCR(this);
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
        // TODO These should all move down to char_data
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

unit_data *new_unit_data(ubit8 type)
{
    if (type == UNIT_ST_ROOM)
    {
        return new EMPLACE(room_data) room_data;
    }
    else if (type == UNIT_ST_OBJ)
    {
        return new EMPLACE(obj_data) obj_data;
    }
    else if (type == UNIT_ST_PC)
    {
        return new EMPLACE(pc_data) pc_data;
    }
    else if (type == UNIT_ST_NPC)
    {
        return new EMPLACE(npc_data) npc_data;
    }
    else
    {
        assert(FALSE);
        return nullptr; // Need to avoid warning on Git actions.
    }
}

unit_data::unit_data()
{
    func = nullptr;
    affected = nullptr;
    fi = nullptr;
    key = nullptr;
    outside = nullptr;
    inside = nullptr;
    next = nullptr;
    gnext = nullptr;
    gprevious = nullptr;
    chars = 0;
    manipulate = 0;
    flags = 0;
    base_weight = 0;
    weight = 0;
    capacity = 0;
    open_flags = 0;
    open_diff = 0;
    light = 0;
    bright = 0;
    illum = 0;
    chars = 0;
    minv = 0;
    max_hp = 0;
    hp = 0;
    alignment = 0;
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
