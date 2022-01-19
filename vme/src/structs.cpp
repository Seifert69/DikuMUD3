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

int g_world_norooms = 0;   /* number of rooms in the world   */
int g_world_noobjects = 0; /* number of objects in the world */
int g_world_nochars = 0;   /* number of chars in the world   */
int g_world_nonpc = 0;     /* number of chars in the world   */
int g_world_nopc = 0;      /* number of chars in the world   */
int g_world_nozones = 0;   /* number of zones in the world   */

/* Descriptor stuff is in system.c */

room_direction_data::room_direction_data(void)
{
    key = nullptr;
    to_room = nullptr;
    exit_info = 0;
    difficulty = 0;
}

room_direction_data::~room_direction_data(void)
{
}

char_point_data::char_point_data(void)
{
    flags = 0;          /* Char flags                               */
    exp = 0;            /* The experience of the player             */
    race = 0;           /* PC/NPC race, Humanoid, Animal, etc.     */
    mana = 0;           /* How many mana points are left?           */
    endurance = 0;      /* How many endurance points are left?      */
    offensive = 0;      /* The OB of a character.                   */
    defensive = 0;      /* The DB of a character.                   */
    speed = 0;          /* The default speed for natural combat     */
    natural_armour = 0; /* The natural built-in armour (ARM_)       */
    attack_type = 0;    /* PC/NPC Attack Type for bare hands (WPN_) */
    dex_reduction = 0;  /* For speed of armour calculations only    */
    sex = 0;            /* PC / NPC s sex                           */
    level = 0;          /* PC / NPC s level                         */
    position = 0;       /* Standing, sitting, fighting...           */
    memset(abilities, 0, sizeof(abilities));
}

char_data::char_data(void)
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

char_data::~char_data(void)
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

room_data::room_data(void)
{
    status = UNIT_ST_ROOM;

    g_world_norooms++;
    mapx = -1;
    mapy = -1;
#ifdef DMSERVER
    waiting_dijkstra = FALSE;
#endif
}

room_data::~room_data(void)
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

obj_data::obj_data(void)
{
    status = UNIT_ST_OBJ;

    g_world_noobjects++;

    memset(value, 0, sizeof(value));
    cost = 0;
    cost_per_day = 0;
    flags = 0;
    type = 0;
    equip_pos = 0;
    resistance = 0;
}

obj_data::~obj_data(void)
{
    g_world_noobjects--;
}

pc_data::pc_data(void)
{
    status = UNIT_ST_PC;

    g_world_nopc++;

    bank = nullptr;
    guild = nullptr;
    hometown = nullptr;
    promptstr = nullptr;

    memset(&setup, 0, sizeof(setup));
    memset(&m_time, 0, sizeof(setup));
    memset(&account, 0, sizeof(setup));

    profession = -1;
    guild_time = 0;
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

pc_data::~pc_data(void)
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

npc_data::npc_data(void)
{
    status = UNIT_ST_NPC;

    g_world_nonpc++;

    memset(weapons, 0, sizeof(weapons));
    memset(spells, 0, sizeof(spells));
    default_pos = POSITION_STANDING;
    flags = 0;
}

npc_data::~npc_data(void)
{
    g_world_nonpc--;
}

zone_type::zone_type(void)
{
    name = nullptr;
    notes = nullptr;
    help = nullptr;
    filename = nullptr;
    rooms = nullptr;
    objects = nullptr;
    npcs = nullptr;

    zri = nullptr;

    spmatrix = nullptr;
    no_rooms = 0;
    no_objs = 0;
    no_npcs = 0;
    access = 255;
}

zone_type::~zone_type(void)
{
    if (name)
        FREE(name);

    if (title)
        FREE(title);

    if (notes)
        FREE(notes);

    if (help)
        FREE(help);

    if (filename)
        FREE(filename);
    class unit_data *ut, *nextut;

    for (ut = objects; ut; ut = nextut)
    {
        nextut = ut->next;
        ut->next = nullptr;
        ut->gnext = nullptr;
        ut->gprevious = nullptr;
        delete ut;
    }

    for (ut = npcs; ut; ut = nextut)
    {
        nextut = ut->next;
        ut->next = nullptr;
        ut->gnext = nullptr;
        ut->gprevious = nullptr;
        delete ut;
    }

    auto nextfi = mmp_fi.begin();
    for (auto p = mmp_fi.begin(); p != mmp_fi.end(); p = nextfi)
    {
        nextfi = p;
        nextfi++;

        delete p->second;
    }

    struct zone_reset_cmd *pzri, *nextzri;

    for (pzri = zri; pzri; pzri = nextzri)
    {
        nextzri = pzri->next;
        FREE(pzri);
    }

    auto nextpt = mmp_tmpl.begin();

    for (auto pt = mmp_tmpl.begin(); pt != mmp_tmpl.end(); pt = nextpt)
    {
        nextpt = pt;
        nextpt++;

        if (pt->second->prgname)
            FREE(pt->second->prgname);
        if (pt->second->argt)
            FREE(pt->second->argt);
        if (pt->second->core)
            FREE(pt->second->core);
        if (pt->second->vart)
            FREE(pt->second->vart);

        FREE(pt->second);
    }

    // struct bin_search_type *ba;    /* Pointer to binarray of type      */
    // struct diltemplate *tmpl;      /* DIL templates in zone            */
    // struct bin_search_type *tmplba;/* Pointer to binarray of type      */
    if (spmatrix)
        FREE(spmatrix);
}

file_index_type::file_index_type(void)
{
    name = nullptr;
    zone = nullptr;
    // next = NULL;
    // unit = NULL;

    filepos = 0;
    length = 0;
    crc = 0;

    no_in_zone = 0;
    no_in_mem = 0;
    room_no = 0;
    type = 0;

#ifdef DMSERVER
    no_in_zone = 0;
#endif
}

file_index_type::~file_index_type(void)
{
    if (name)
        FREE(name);
}

unit_fptr::unit_fptr(void)
{
    index = 0;
    priority = FN_PRI_CHORES;
    heart_beat = PULSE_SEC;
    flags = 0;
    data = nullptr;
    next = nullptr;
    event = nullptr;
}

unit_fptr::~unit_fptr()
{
    data = nullptr;
}

#ifndef VMC_SRC
unit_data *unit_data::copy()
{
    unit_data *u;
    int x;

    u = new_unit_data(status);

    CByteBuffer abuf, fbuf;
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

    UNIT_TITLE(u) = UNIT_TITLE(this);
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
            uroom->dir_option[x]->open_name = thisroom->dir_option[x]->open_name;
            uroom->dir_option[x]->key = str_dup(thisroom->dir_option[x]->key);
            uroom->dir_option[x]->exit_info = thisroom->dir_option[x]->exit_info;
            uroom->dir_option[x]->difficulty = thisroom->dir_option[x]->difficulty;
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
        CHAR_FLAGS(u) = CHAR_FLAGS(this);
        CHAR_EXP(u) = CHAR_EXP(this);
        CHAR_MANA(u) = CHAR_MANA(this);
        CHAR_ENDURANCE(u) = CHAR_ENDURANCE(this);
        CHAR_RACE(u) = CHAR_RACE(this);
        CHAR_OFFENSIVE(u) = CHAR_OFFENSIVE(this);
        CHAR_DEFENSIVE(u) = CHAR_DEFENSIVE(this);
        CHAR_SPEED(u) = CHAR_SPEED(this);
        CHAR_NATURAL_ARMOUR(u) = CHAR_NATURAL_ARMOUR(this);
        CHAR_ATTACK_TYPE(u) = CHAR_ATTACK_TYPE(this);
        CHAR_SEX(u) = CHAR_SEX(this);
        CHAR_LEVEL(u) = CHAR_LEVEL(this);
        CHAR_POS(u) = CHAR_POS(this);
        for (x = 0; x < ABIL_TREE_MAX; x++)
        {
            CHAR_ABILITY(u, x) = CHAR_ABILITY(this, x);
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

unit_data::unit_data(void)
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

unit_data::~unit_data(void)
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

void unit_data::set_fi(class file_index_type *f)
{
    assert(f);

    if (this->fi)
    {
        slog(LOG_ALL, 0, "ERROR: FI was already set. This shouldn't happen");
    }

    this->fi = f;
    this->fi->no_in_mem++;
}

std::string unit_data::json(void)
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
                if (UROOM(this)->dir_option[i]->to_room)
                {
                    t = UNIT_FI_NAME(ROOM_EXIT(this, i)->to_room);
                    t.append("@");
                    t.append(UNIT_FI_ZONENAME(UROOM(this)->dir_option[i]->to_room));
                    s.append(str_json("toroom", t));
                    s.append(",\n");
                }
                s.append(UROOM(this)->dir_option[i]->open_name.json());
                s.append(",\n");
                s.append(str_json("difficulty", UROOM(this)->dir_option[i]->difficulty));
                s.append(",\n");
                s.append(str_json("exitflags", UROOM(this)->dir_option[i]->exit_info));
                s.append(",\n");
                s.append(str_json("key", UROOM(this)->dir_option[i]->key));
                s.append("\n");
                s.append("}\n");
            }
        }

        s.append("}\n");
    }

    s.append("\n}\n");

    return s;
}
