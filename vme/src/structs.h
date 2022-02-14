/*
 $Author: All $
 $RCSfile: structs.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.9 $
 */
#pragma once

#include "color.h"
#include "combat.h"
#include "destruct.h"
#include "event.h"
#include "extra.h"
#include "hookmud.h"
#include "namelist.h"
#include "protocol.h"
#include "queue.h"

#include <vme.h>

#include <cstring>
#include <forward_list>
#include <map>
#include <vector>

#ifndef MPLEX_COMPILE
    #include <boost/graph/adjacency_list.hpp>
    #include <boost/graph/graph_traits.hpp>
#endif

#define FI_MAX_ZONENAME 30 /* Max length of any zone-name    */
#define FI_MAX_UNITNAME 15 /* Max length of any unit-name    */

#define PC_MAX_PASSWORD 13 /* Max length of any pc-password  */
#define PC_MAX_NAME 15     /* 14 Characters + Null           */

#define MESS_ATTACKER 1
#define MESS_VICTIM 2
#define MESS_ROOM 3

/* For use in spec_assign.c's g_unit_function_array[] */
#define SD_NEVER 0 /* Never save this function           */
#define SD_NULL 1  /* Ignore fptr->data (save as 0 ptr)  */
#define SD_ASCII 2 /* If pointer, then it's ascii char * */

class unit_data;
class diltemplate;
class dilprg;
class file_index_type;
class zone_reset_cmd;

/* ----------------- DATABASE STRUCTURES ----------------------- */
// Used for std::map in place of the old binary search
struct cmp_str
{
    bool operator()(char const *a, char const *b) const { return std::strcmp(a, b) < 0; }
};

/* A linked/sorted list of all the zones in the game */
class zone_type
{
public:
    zone_type();
    ~zone_type();

    cNamelist &getCreators();

    const char *getName() const;
    // Used in one place (dilfld.cpp:2227) that needs a non-const pointer - will be removed at a later date
    // Once whole path can be checked on why modifiable pointer is needed
    char *getNamePtr();
    void setName(char *value);

    const char *getTitle() const;
    char **getTitlePtrPtr();
    void setTitle(char *value);

    const char *getNotes() const;
    char **getNotesPtrPtr();
    void setNotes(char *value);

private:
    cNamelist creators;   /* List of creators of zone         */
    char *name{nullptr};  /* Unique in list                   */
    char *title{nullptr}; /* A nice looking zone title        */
    char *notes{nullptr};    /* Creator notes to zone            */

public:
    char *help;     /* User-Help to zone                */
    char *filename; /* The filename of this file        */

    unit_data *rooms;   // unit pointer to the base rooms, used in vmc really
    unit_data *objects; // unit pointer to the base objects, used in vmc really
    unit_data *npcs;    // unit pointer to the base npcs, used in vmc really

    std::map<const char *, file_index_type *, cmp_str> mmp_fi;

    zone_reset_cmd *zri; /* List of Zone reset commands      */

    std::map<const char *, diltemplate *, cmp_str> mmp_tmpl;

    ubit8 **spmatrix; /* Shortest Path Matrix             */

    ubit16 zone_no;   /* Zone index counter (spmatrix)    */
    ubit16 no_of_fi;  /* Number of fi's in the list       */
    ubit16 zone_time; /* How often to reset zone          */
    ubit16 no_rooms;  /* The number of rooms              */
    ubit16 no_objs;
    ubit16 no_npcs;
    ubit8 reset_mode; /* when/how to reset zone           */
    ubit16 no_tmpl;   /* number of DIL templates          */

    ubit8 access;    /* Access Level 0 = highest (root)  */
    ubit8 loadlevel; /* Level required to load items     */
    ubit8 payonly;   /* TRUE when only 4 paying players  */
    char *dilfilepath;

    struct
    {
        int pressure; /* How is the pressure ( Mb )            */
        int change;   /* How fast and what way does it change. */
        int sky;      /* How is the sky.                       */
        int base;     /* What is the basis pressure like?      */
    } weather;
};

/* ----------------- OTHER STRUCTURES ----------------------- */

/* This structure is purely intended to be an easy way to transfer */
/* and return information about time (real or mudwise).            */
struct time_info_data
{
    long hours, day, month;
    long year;
};

/* --------------------- DESCRIPTOR STRUCTURES -------------------- */

struct snoop_data
{
    unit_data *snooping; /* Who is this char snooping        */
    unit_data *snoop_by; /* And who is snooping on this char */
};

class descriptor_data
{
public:
    descriptor_data(cMultiHook *pe);
    ~descriptor_data();

    void CreateBBS();
    void RemoveBBS();

    time_t logon;      /* Time of last connect              */
    cMultiHook *multi; /* Multi element pointer             */
    ubit16 id;         /* The ID for the multi              */
    void (*fptr)(descriptor_data *, char *);
    int state;      /* Locally used in each fptr         */
    char host[50];  /* hostname                          */
    ubit16 nPort;   /* Mplex port                        */
    ubit8 nLine;    /* Serial Line                       */
    int wait;       /* wait for how many loops           */
    ubit16 timer;   /* num of hours idleness for mortals */
    ubit32 replyid; /* Used for 'tell reply'             */

    /* For the 'modify-string' system.       */
    char *localstr; /* This string is expanded while editing */

    void (*postedit)(descriptor_data *);
    unit_data *editing;
    void *editref; /* pointer to "where we are editing"     */
    /* when using (volatile) extras + boards */

    int prompt_mode;                     /* control of prompt-printing       */
    char last_cmd[MAX_INPUT_LENGTH + 1]; /* the last entered cmd_str         */
    char history[MAX_INPUT_LENGTH + 1];  /* simple command history           */
    cQueue qInput;                       /* q of unprocessed input           */
    unit_data *character;                /* linked to char                   */
    unit_data *original;                 /* original char                    */
    snoop_data snoop;                    /* to snoop people.                 */

    descriptor_data *next; /* link to next descriptor          */
};

/* ----------------- UNIT GENERAL STRUCTURES ----------------------- */

struct unit_dil_affected_type
{
    ubit16 beat;
    ubit16 duration;
    ubit16 transfer;
    cNamelist data;
    char *apply;  /* Apply DIL  */
    char *remove; /* Remove DIL */
    unit_dil_affected_type *next;
};

class unit_affected_type : public basedestruct
{
public:
    sbit16 id;
    ubit16 beat;     /* Beat in 1/4 of secs, 0 = None */
    sbit16 duration; /* How many beats until end      */

    int data[3];

    sbit16 firstf_i;
    sbit16 tickf_i;
    sbit16 lastf_i;
    sbit16 applyf_i;
    eventq_elem *event; /*pointer to eventq for quick removing      */
    unit_data *owner;
    unit_affected_type *next, *gnext, *gprevious;

    int destruct_classindex();
};

class unit_fptr : public basedestruct
{
public:
    unit_fptr();
    ~unit_fptr();

    ubit16 index;       /* Index to function pointer array             */
    ubit8 priority;     /* Order to insert ftpr on unit (2020)         */
    ubit16 heart_beat;  /* in 1/4 of a sec                             */
    ubit16 flags;       /* When to override next function (boolean)    */
    void *data;         /* Pointer to data local for this unit         */
    unit_fptr *next;    /* Next in linked list                         */
    eventq_elem *event; /* pointer to eventq for quick removing        */

    int destruct_classindex();
};

class unit_data : public basedestruct
{
public:
    unit_data();
    virtual ~unit_data();
    unit_data *copy();
    void set_fi(file_index_type *f);

    cNamelist names; /* Name Keyword list for get, enter, etc.      */

    unit_fptr /* Function pointer type                      */
        *func;

    unit_dil_affected_type *dilaffect;

    unit_affected_type *affected;

    file_index_type *fi; /* Unit file-index                               */

    char *key; /* Pointer to fileindex to Unit which is the key */

    unit_data *outside; /* Pointer out of the unit, ei. from an object   */
    /* out to the char carrying it                   */
    unit_data *inside; /* Linked list of chars,rooms & objs             */

    unit_data /* For next unit in 'inside' linked list         */
        *next;

    unit_data /* global l-list of objects, chars & rooms       */
        *gnext,
        *gprevious;

    ubit32 manipulate;  /* WEAR_XXX macros                               */
    ubit16 flags;       /* Invisible, can_bury, burried...               */
    sbit32 base_weight; /* The "empty" weight of a room/char/obj (lbs)   */
    sbit32 weight;      /* Current weight of a room/obj/char             */
    sbit16 capacity;    /* Capacity of obj/char/room, -1 => any          */
    ubit16 size;        /* (cm) MOBs height, weapons size, ropes length  */

    ubit8 status;     /* IS_ROOM, IS_OBJ, IS_PC, IS_NPC                */
    ubit8 open_flags; /* In general OPEN will mean can "enter"?        */
    ubit8 open_diff;  /* Open dificulty                                */
    sbit16 light;     /* Number of active light sources in unit        */
    sbit16 bright;    /* How much the unit shines                      */
    sbit16 illum;     /* how much bright is by transparency            */
    ubit8 chars;      /* How many chars is inside the unit             */
    ubit8 minv;       /* Level of wizard invisible                     */
    sbit32 max_hp;    /* The maximum number of hitpoint                */
    sbit32 hp;        /* The actual amount of hitpoints left           */

    sbit16 alignment; /* +-1000 for alignments                         */

    /* Room title, Char title, Obj "the barrel", NPC "the Beastly Fido" */
    std::string title;

    /* The outside description of a unit           */
    std::string out_descr;

    /* The inside description of a unit            */
    std::string in_descr;

    extra_list extra; /* All the look 'at' stuff                     */

    int destruct_classindex();
    std::string json();
};

/* ----------------- ROOM SPECIFIC STRUCTURES ----------------------- */

class room_direction_data
{
public:
    room_direction_data();
    ~room_direction_data();

    cNamelist open_name; // For Open & Enter

    char *key;
    unit_data *to_room;
    ubit8 difficulty; // Skill needed for swim, climb, search, pick-lock
    int weight;       // Used for shortest path algorithm

    ubit8 exit_info; // Door info flags
};

class room_data : public unit_data
{
public:
    room_data();
    ~room_data();

    room_direction_data *dir_option[MAX_EXIT + 1]; // Why 11? Why not MAX_EXIT+1?

    ubit8 flags;         /* Room flags                              */
    ubit8 movement_type; /* The type of movement (city, hills etc.) */
    ubit8 resistance;    /* Magic resistance of the room            */

    sbit16 mapx, mapy; /* Graphical map coordinates */

    int sc;  /*strong component, used for shortest path */
    int num; /*room number, used for shortest path */
#ifndef MPLEX_COMPILE
    enum edge_dir_t
    {
        edge_dir = 101
    };

    typedef boost::adjacency_list<boost::vecS,
                                  boost::vecS,
                                  boost::directedS,
                                  boost::no_property,
                                  boost::property<boost::edge_weight_t, int, boost::property<edge_dir_t, int>>>
        graph_t;

    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    std::vector<vertex_descriptor> path;
    std::vector<vertex_descriptor> distance;
    int waiting_dijkstra;
#endif
};

/* ------------------ OBJ SPECIFIC STRUCTURES ----------------------- */

class obj_data : public unit_data
{
public:
    obj_data();
    ~obj_data();

    sbit32 value[5];     /* Values of the item (see list)       */
    ubit32 cost;         /* Value when sold (gp.)               */
    ubit32 cost_per_day; /* Cost to keep pr. real day           */

    ubit8 flags;      /* Various special object flags        */
    ubit8 type;       /* Type of item (ITEM_XXX)             */
    ubit8 equip_pos;  /* 0 or position of item in equipment  */
    ubit8 resistance; /* Magic resistance                    */
};

/* ----------------- CHAR SPECIFIC STRUCTURES ----------------------- */

class char_point_data
{
public:
    char_point_data();
    /*~char_point_data(void); not needed yet all base types they destroy themselves*/

    ubit32 flags; /* Char flags                               */

    sbit32 exp; /* The experience of the player             */

    ubit16 race; /* PC/NPC race, Humanoid, Animal, etc.     */

    sbit16 mana;      /* How many mana points are left?           */
    sbit16 endurance; /* How many endurance points are left?      */
    sbit16 offensive; /* The OB of a character.                   */
    sbit16 defensive; /* The DB of a character.                   */

    ubit8 speed;                     /* The default speed for natural combat     */
    ubit8 natural_armour;            /* The natural built-in armour (ARM_)       */
    ubit8 attack_type;               /* PC/NPC Attack Type for bare hands (WPN_) */
    ubit8 dex_reduction;             /* For speed of armour calculations only    */
    ubit8 sex;                       /* PC / NPC s sex                           */
    ubit8 level;                     /* PC / NPC s level                         */
    ubit8 position;                  /* Standing, sitting, fighting...           */
    sbit16 abilities[ABIL_TREE_MAX]; /* Str/dex etc.                 */
};

struct char_follow_type
{
    unit_data *follower; /* Must be a char */
    char_follow_type *next;
};

class char_data : public unit_data
{
public:
    char_data();
    virtual ~char_data();

    descriptor_data *descriptor;
    cCombat *Combat;
    unit_data *master;    /* Must be a char */
    unit_data *last_room; /* Last location of character */
    char_point_data points;

    char_follow_type *followers;

    char *last_attacker; /* Last attacker of character */
    char *money;         /*  Money transfer from db-files. */

    ubit8 last_attacker_type; /* Last attacker type of character */
};

/* ------------------  PC SPECIFIC STRUCTURES ------------------------ */
struct pc_time_data
{
    time_t creation; /* This represents time when the pc was created.     */
    time_t connect;  /* This is the last time that the pc connected.      */
    time_t birth;    /* This represents the characters age                */
    ubit32 played;   /* This is the total accumulated time played in secs */
};

struct pc_account_data
{
    float credit;        /* How many coin units are left on account?       */
    ubit32 credit_limit; /* In coin units (i.e. cents / oerer)             */
    ubit32 total_credit; /* Accumulated credit to date (coin units)        */
    sbit16 last4;        /* The last four digits of his credit card, or -1 */
    ubit8 cracks;        /* Crack-attempts on CC last4                     */
    ubit8 discount;      /* 0 - 100% discount                              */
    ubit32 flatrate;     /* The expiration date of a flat rate service     */
};

class pc_data : public char_data
{
public:
    pc_data();
    ~pc_data();

    void gstate_tomenu(dilprg *pdontstop);
    void gstate_togame(dilprg *pdontstart);

    void disconnect_game();
    void connect_game();
    void reconnect_game(descriptor_data *d);

    terminal_setup_type setup;

    pc_time_data m_time;     /* PCs time info  */
    pc_account_data account; /* Accounting     */

    char *guild;     /* Which guild is the player a member of?  */
    char *bank;      /* How much money in bank?                 */
    char *hometown;  /* PCs Hometown (symbolic reference)       */
    char *promptstr; /* A PC's Prompt                           */

    extra_list info;  /* For saving Admin information             */
    extra_list quest; /* For saving QUEST information            */

    sbit8 profession;  // The player's chosen profession, -1 means unknown
    ubit32 guild_time; /* When (playing secs) player entered      */
    ubit16 vlvl;       /* Virtual Level for player                */

    sbit32 id;             /* Unique identifier for each player (-1 guest) */
    sbit32 skill_points;   /* No of practice points left              */
    sbit32 ability_points; /* No of practice points left              */

    ubit16 flags;          /* flags for PC setup (brief, noshout...)  */
    ubit16 nr_of_crimes;   /* Number of crimes committed              */
    ubit16 crack_attempts; /* Number of wrong passwords entered       */
    ubit16 lifespan;       /* How many year to live....               */

    sbit16 spells[SPL_TREE_MAX];   /* The spells learned                  */
    ubit8 spell_lvl[SPL_TREE_MAX]; /* Practiced within that level         */

    sbit16 skills[SKI_TREE_MAX];   /* The skills learned                  */
    ubit8 skill_lvl[SKI_TREE_MAX]; /* The skills practiced within level   */

    sbit16 weapons[WPN_TREE_MAX];   /* The weapons learned                 */
    ubit8 weapon_lvl[WPN_TREE_MAX]; /* The weapons learned                  */

    ubit8 ability_lvl[ABIL_TREE_MAX]; /* The abilities learned                  */

    sbit8 conditions[3]; /* Drunk full etc.                     */
    ubit8 nAccessLevel;  /* Access Level for BBS use            */

    char pwd[PC_MAX_PASSWORD];  /* Needed when loaded w/o descriptor   */
    char filename[PC_MAX_NAME]; /* The name on disk...                 */
    ubit32 lasthosts[5];        /* last 5 different IPs                */
    color_type color;           /* Players default colors              */
};

/* ------------------ NPC SPECIFIC STRUCTURES ----------------------- */

class npc_data : public char_data
{
public:
    npc_data();
    ~npc_data();

    sbit16 weapons[WPN_GROUP_MAX];
    sbit16 spells[SPL_GROUP_MAX];

    ubit8 default_pos; /* Default position for NPC               */
    ubit8 flags;       /* flags for NPC behavior                 */
};

/* ----------------- Destructed decalrations ----------------------- */

#define DR_UNIT 0
#define DR_AFFECT 1
#define DR_FUNC 2

unit_data *new_unit_data(ubit8 type);

extern int g_world_nochars;
extern int g_world_nonpc;
extern int g_world_noobjects;
extern int g_world_norooms;
extern int g_world_nopc;
extern int g_world_nozones;
