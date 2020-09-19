/*
 $Author: All $
 $RCSfile: modify.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.6 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

/* Per https://sourceforge.net/p/predef/wiki/OperatingSystems/, this identifies
 *  Mac OS X. This is needed since OS X doesn't have crypt.h and instead uses
 *  unistd.h for these mappings. */
#if defined __APPLE__ && __MACH__
#include <unistd.h>
#elif defined LINUX
#include <crypt.h>
#endif

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "handler.h"
#include "db.h"
#include "comm.h"
#include "system.h"
#include "textutil.h"
#include "utility.h"
#include "trie.h"
#include "affect.h"
#include "common.h"
#include "money.h"
#include "constants.h"
#include "skills.h"

#define UT_ROOM (1 << 0)
#define UT_OBJ (1 << 1)
#define UT_PC (1 << 2)
#define UT_NPC (1 << 3)
#define UT_CHAR (UT_NPC | UT_PC)
#define UT_UNIT (UT_CHAR | UT_OBJ | UT_ROOM)

#define AT_VAL 1     /* A value is expected as argument */
#define AT_BIT 2     /* A list of bits is expected as argument */
#define AT_TYP 3     /* A type (char) is expected as argument */
#define AT_STR 4     /* A single line string is expected as argument */
#define AT_DES 5     /* A multi line string is expected as argument */
#define AT_UNT 6     /* A unit-path is expected as argument */
#define AT_KEYDES 7  /* A keyword and a multiline descr. is expected */
#define AT_TYPVAL 8  /* A type followed by a value is expected */
#define AT_DIRBIT 9  /* A direction then a list of bits is expected */
#define AT_TYPDES 10 /* A type then a description is expected */
#define AT_DIRSTR 11 /* A direction then a string is expected */
#define AT_DIRUNT 12 /* A direction then a unit-path is expected */
#define AT_DIRDES 13 /* A direction then a description is expected */

#define MAX_SET_FIELDS 70

struct field_type
{
    ubit8 utype;            /* type of unit to work on           */
    ubit8 atype;            /* type of argument to expect        */
    const char **structure; /* structure of bit/type recognition */
    ubit8 minself;          /* minimum level to modify self      */
    ubit8 minother;         /* minimum level to modify other     */
    ubit8 minplayer;        /* minimum level to modify player    */
};

static const char *unit_field_names[MAX_SET_FIELDS + 1] = {
    "add-name",
    "del-name",
    "title",
    "descr",
    "add-extra",
    "del-extra",
    "manipulate",
    "flags",
    "weight",
    "capacity",
    "max-hit",
    "hit",
    "key",
    "alignment",
    "open-flags",
    "toughness",
    "lights",
    "bright",
    "room-flags",
    "movement",
    "ccinfo",
    "add-dir-name",
    "del-dir-name",
    "dir-flags",
    "dir-key",
    "value0",
    "value1",
    "value2",
    "value3",
    "value4",
    "obj-flags",
    "cost",
    "rent",
    "type",
    "equip",
    "guild-name",
    "pwd",
    "pc-flags",
    "crimes",
    "drunk",
    "full",
    "thirsty",
    "default-pos",
    "npc-flags",
    "hometown",
    "exp",
    "char-flags",
    "mana",
    "endurance",
    "attack-type",
    "hand-quality",
    "size",
    "race",
    "sex",
    "level",
    "position",
    "ability",
    "skill-points",
    "ability-points",
    "remove-affect",
    "add-quest",
    "del-quest",
    "speed",
    "add-info",
    "del-info",
    "access",
    "promptstr",
    "age",
    "lifespan",
    "profession", NULL};

struct field_type unit_field_data[MAX_SET_FIELDS + 1] = {
    {UT_UNIT, AT_STR, 0, 200, 200, 253},                  /* add-name        */
    {UT_UNIT, AT_STR, 0, 200, 200, 253},                  /* del-name        */
    {UT_UNIT, AT_STR, 0, 200, 200, 200},                  /* title           */
    {UT_UNIT, AT_DES, 0, 200, 200, 253},                  /* long-description */
    {UT_UNIT, AT_KEYDES, 0, 200, 200, 200},               /* add-extra       */
    {UT_UNIT, AT_STR, 0, 200, 200, 200},                  /* del-extra       */
    {UT_UNIT, AT_BIT, unit_manipulate, 200, 200, 250},    /* manipulate      */
    {UT_UNIT, AT_BIT, unit_flags, 200, 200, 250},         /* unit-flags      */
    {UT_UNIT, AT_VAL, 0, 200, 200, 230},                  /* weight          */
    {UT_UNIT, AT_VAL, 0, 200, 200, 230},                  /* capacity        */
    {UT_UNIT, AT_VAL, 0, 200, 200, 253},                  /* max-hp          */
    {UT_UNIT, AT_VAL, 0, 200, 200, 253},                  /* hp              */
    {UT_UNIT, AT_STR, 0, 200, 200, 253},                  /* key             */
    {UT_UNIT, AT_VAL, 0, 200, 200, 249},                  /* alignment       */
    {UT_UNIT, AT_BIT, unit_open_flags, 200, 200, 253},    /* open-flags      */
    {UT_UNIT, AT_VAL, 0, 200, 200, 253},                  /* tgh             */
    {UT_UNIT, AT_VAL, 0, 253, 253, 253},                  /* lights          */
    {UT_UNIT, AT_VAL, 0, 200, 200, 253},                  /* bright          */
    {UT_ROOM, AT_BIT, room_flags, 200, 200, 200},         /* room-flags      */
    {UT_ROOM, AT_TYP, room_landscape, 200, 200, 200},     /* movement        */
    {UT_PC, AT_VAL, 0, 255, 255, 255},                    /* cc-info         */
    {UT_ROOM, AT_DIRSTR, 0, 200, 200, 200},               /* add-dir-name    */
    {UT_ROOM, AT_DIRSTR, 0, 200, 200, 200},               /* del-dir-name    */
    {UT_ROOM, AT_DIRBIT, unit_open_flags, 200, 200, 200}, /* dir-flags       */
    {UT_ROOM, AT_DIRSTR, 0, 200, 200, 200},               /* dir-key         */
    {UT_OBJ, AT_VAL, 0, 200, 240, 200},                   /* value0          */
    {UT_OBJ, AT_VAL, 0, 200, 240, 200},                   /* value1          */
    {UT_OBJ, AT_VAL, 0, 200, 240, 200},                   /* value2          */
    {UT_OBJ, AT_VAL, 0, 200, 240, 200},                   /* value3          */
    {UT_OBJ, AT_VAL, 0, 200, 240, 200},                   /* value4          */
    {UT_OBJ, AT_BIT, obj_flags, 200, 200, 200},           /* obj-flags       */
    {UT_OBJ, AT_VAL, 0, 200, 200, 200},                   /* cost            */
    {UT_OBJ, AT_VAL, 0, 200, 200, 200},                   /* rent            */
    {UT_OBJ, AT_TYP, obj_types, 200, 240, 200},           /* type            */
    {UT_OBJ, AT_TYP, obj_pos, 200, 240, 200},             /* equip           */
    {UT_PC, AT_STR, 0, 200, 230, 253},                    /* guild-name      */
    {UT_PC, AT_STR, 0, 240, 253, 230},                    /* pwd             */
    {UT_PC, AT_BIT, pc_flags, 200, 200, 253},             /* pc-flags        */
    {UT_PC, AT_VAL, 0, 200, 200, 253},                    /* crimes          */
    {UT_PC, AT_VAL, 0, 200, 200, 200},                    /* drunk           */
    {UT_PC, AT_VAL, 0, 200, 200, 200},                    /* full            */
    {UT_PC, AT_VAL, 0, 200, 200, 200},                    /* thirsty         */
    {UT_NPC, AT_VAL, 0, 200, 200, 253},                   /* default-pos     */
    {UT_NPC, AT_BIT, npc_flags, 200, 200, 200},           /* npc-flags       */
    {UT_PC, AT_STR, 0, 200, 230, 230},                    /* hometown        */
    {UT_CHAR, AT_VAL, 0, 253, 253, 253},                  /* exp             */
    {UT_CHAR, AT_BIT, char_flags, 200, 200, 253},         /* char-flags      */
    {UT_CHAR, AT_VAL, 0, 200, 200, 240},                  /* mana            */
    {UT_CHAR, AT_VAL, 0, 200, 200, 240},                  /* endurance       */
    {UT_CHAR, AT_TYP, g_WpnColl.text, 200, 200, 253},     /* attack-type     */
    {UT_CHAR, AT_VAL, 0, 200, 200, 253},                  /* hand-quality    */
    {UT_UNIT, AT_VAL, 0, 200, 200, 230},                  /* height          */
    {UT_CHAR, AT_TYP, pc_races, 200, 200, 253},           /* race            */
    {UT_CHAR, AT_TYP, char_sex, 200, 200, 253},           /* sex             */
    {UT_NPC, AT_VAL, 0, 255, 255, 255},                   /* level           */
    {UT_CHAR, AT_TYP, char_pos, 200, 253, 253},           /* position        */
    {UT_CHAR, AT_TYPVAL, g_AbiColl.text, 240, 253, 253},  /* ability         */
    {UT_PC, AT_VAL, 0, 230, 253, 253},                    /* skill-points    */
    {UT_PC, AT_VAL, 0, 230, 253, 253},                    /* ability-points  */
    {UT_UNIT, AT_VAL, 0, 200, 230, 200},                  /* remove affects  */
    {UT_PC, AT_STR, 0, 200, 200, 230},                    /* add-quest       */
    {UT_PC, AT_STR, 0, 200, 200, 230},                    /* del-quest       */
    {UT_CHAR, AT_VAL, 0, 200, 253, 230},                  /* speed           */
    {UT_PC, AT_STR, 0, 255, 254, 254},                    /* add-info        */
    {UT_PC, AT_STR, 0, 255, 254, 254},                    /* del-info        */
    {UT_PC, AT_VAL, 0, 255, 254, 254},                    /* access          */
    {UT_PC, AT_STR, 0, 200, 200, 200},                    /* promptstr       */
    {UT_PC, AT_VAL, 0, 254, 255, 255},                    /* age             */
    {UT_PC, AT_VAL, 0, 254, 255, 255},                    /* lifespan        */
    {UT_PC, AT_VAL, 0, 254, 255, 255},                    /* profession      */
};

// Post-porcessing of adding-extra descriptions.
void edit_extra(class descriptor_data *d)
{
    class extra_descr_data *exd;

    for (exd = UNIT_EXTRA(d->editing).m_pList; exd; exd = exd->next)
        if (exd == d->editref)
            break; // It still exists!

    if (exd)
        exd->descr = (d->localstr);
}

// Post-porcessing of adding-extra descriptions.
void edit_info(class descriptor_data *d)
{
    class extra_descr_data *exd;

    for (exd = PC_INFO(d->editing).m_pList; exd; exd = exd->next)
        if (exd == d->editref)
            break; // It still exists!

    if (exd)
        exd->descr = (d->localstr);
}

void edit_outside_descr(class descriptor_data *d)
{
    UNIT_OUT_DESCR(d->editing) = (d->localstr);
}

void edit_inside_descr(class descriptor_data *d)
{
    UNIT_IN_DESCR(d->editing) = (d->localstr);
}

int search_block_set(char *arg, const char **list, bool exact)
{
    register int i, l;

    /* Substitute '_' and get length of string */
    for (l = 0; arg[l]; l++)
        if (arg[l] == '_')
            arg[l] = ' ';

    if (exact)
    {
        for (i = 0; list[i]; i++)
            if (!strcmp(arg, list[i]))
                return i;
    }
    else
    {
        if (l == 0)
            l = 1; /* Avoid "" to match the first available string */

        for (i = 0; list[i]; i++)
            if (!str_nccmp(arg, list[i], l)) /* no regard of case */
                return i;
    }

    return -1;
}

/* show possible fields */
#define GET_FIELD_UT(c) \
    (c) == UT_CHAR ? "Char" : (c) == UT_PC ? "Pc" : (c) == UT_NPC ? "Npc" : (c) == UT_OBJ ? "Obj" : (c) == UT_UNIT ? "Unit" : (c) == UT_ROOM ? "Room" : "Not Used"

#define GET_FIELD_AT(c) \
    (c) == AT_VAL ? "&lt;value&gt;" : (c) == AT_BIT ? "&lt;bitlist&gt;" : (c) == AT_TYP ? "&lt;type&gt;" : (c) == AT_STR ? "&lt;string&gt;" : (c) == AT_DES ? "(enter description)" : (c) == AT_UNT ? "&lt;unitpath&gt;" : (c) == AT_KEYDES ? "&lt;keyword&gt; (enter description)" : (c) == AT_TYPVAL ? "&lt;type&gt; <value&gt;" : (c) == AT_DIRBIT ? "&lt;direction&gt; <bitlist&gt;" : (c) == AT_TYPDES ? "&lt;type&gt; (enter description)" : (c) == AT_DIRSTR ? "&lt;direction&gt; <string&gt;" : (c) == AT_DIRUNT ? "&lt;direction&gt; <unitpath&gt;" : (c) == AT_DIRDES ? "&lt;direction&gt; (enter description)" : "Not usable"

void show_fields(class unit_data *ch)
{
    char string[4 * MAX_STRING_LENGTH], *c;
    int i;

    *string = '\0';
    c = string;
    sprintf(c, "<table class='colh3'>");
    TAIL(c);

    for (i = 0; i < MAX_SET_FIELDS; i++)
    {
        sprintf(c, "<tr><td>%s :</td><td> on %s. :</td><td>%s</td></tr>",
                unit_field_names[i],
                GET_FIELD_UT(unit_field_data[i].utype),
                GET_FIELD_AT(unit_field_data[i].atype));
        TAIL(c);
    }
    sprintf(c, "</table>");
    send_to_char(string, ch);
}

#undef GET_FIELDT_UT
#undef GET_FIELD_AT

void show_structure(const char *structure[], class unit_data *ch)
{
    char **c, *cc, buf[MAX_STRING_LENGTH];

    strcpy(buf, "Not defined yet.<br/>");

    for (cc = buf, c = (char **)structure; *c; c++)
    {
        sprintf(cc, "%s<br/>", *c);
        TAIL(cc);
    }
    send_to_char(buf, ch);
}

long int get_bit(char *bitlst, const char *structure[])
{
    char *l, *s, bit[MAX_STRING_LENGTH];
    long int bitval = 0, tmpval;

    /* walk through bitlist */
    for (l = bitlst; *l;)
    {
        /* copy bitname */
        for (s = bit; *l && *l != '|'; *(s++) = *(l++))
            ;

        *s = 0;
        if (*l)
            l++;

        if (!str_is_empty(bit))
        {
            if ((tmpval = search_block_set(bit, structure, FALSE)) == -1)
                return -1;
            SET_BIT(bitval, 1 << tmpval);
        }
    }
    return bitval;
}

int get_type(char *typdef, const char *structure[])
{
    return search_block_set(typdef, structure, FALSE);
}

/* modification of anything in units */
void do_set(class unit_data *ch, char *argument, const struct command_info *cmd)
{
    char arg[MAX_STRING_LENGTH], buf[MAX_STRING_LENGTH];
    int type;

    char strarg[MAX_STRING_LENGTH];
    int typarg = 0;
    long int valarg = 0;
    long int bitarg = 0;

    class file_index_type *untarg = NULL;
    class extra_descr_data *ed;
    class unit_data *unt;
    class unit_affected_type *aff;

    int required_xp(int level);

    if (!CHAR_DESCRIPTOR(ch))
        return;

    /* Check argument */
    if (str_is_empty(argument))
    {
        send_to_char("Syntax: set &lt;name&gt; &lt;field&gt; &lt;arguments&gt;.<br/>", ch);
        return;
    }

    str_next_word(argument, arg);

    if (str_ccmp(arg, "room") == 0)
    {
        argument = str_next_word(argument, arg);
        unt = UNIT_IN(ch);
    }
    else
    {
        unt = find_unit(ch, &argument, 0, FIND_UNIT_WORLD | FIND_UNIT_SURRO | FIND_UNIT_INVEN | FIND_UNIT_EQUIP);
        if (unt == NULL)
        {
            send_to_char("No such thing.<br/>", ch);
            return;
        }
        /* Temporary fix, as modifying money CAN crash the game!   /gnort */
        else if (IS_MONEY(unt) && CHAR_LEVEL(ch) < 255)
        {
            send_to_char("You can't modify money yet, sorry.<br/>", ch);
            return;
        }
    }

    /* find field to change */
    argument = str_next_word(argument, arg);

    /* Partial match on fields */
    if ((type = search_block(arg, unit_field_names, FALSE)) == -1)
    {
        send_to_char("Invalid field.<br/>", ch);
        show_fields(ch);
        return;
    }

    /* check if level of users is ok */
    if (CHAR_LEVEL(ch) < unit_field_data[type].minplayer && unt != ch && IS_PC(unt))
    {
        send_to_char("Authority to set field for OTHER PLAYERS denied!<br/>",
                     ch);
        return;
    }

    /* check if level of users is ok */
    if ((CHAR_LEVEL(ch) < unit_field_data[type].minother && unt != ch && IS_PC(unt)) || (CHAR_LEVEL(ch) < unit_field_data[type].minself && unt == ch))
    {
        send_to_char("Authority to set field denied!<br/>", ch);
        return;
    }

    /* see if field is valid for unit */
    if ((unit_field_data[type].utype == UT_ROOM && !IS_ROOM(unt)) ||
        (unit_field_data[type].utype == UT_OBJ && !IS_OBJ(unt)) ||
        (unit_field_data[type].utype == UT_NPC && !IS_NPC(unt)) ||
        (unit_field_data[type].utype == UT_PC && !IS_PC(unt)) ||
        (unit_field_data[type].utype == UT_CHAR && !IS_CHAR(unt)))
    {
        send_to_char("Field invalid for type of unit.<br/>", ch);
        return;
    }

    argument = skip_spaces(argument);

    /* read in field parameters */
    switch (unit_field_data[type].atype)
    {
    case AT_STR:
        if (str_is_empty(argument))
        {
            send_to_char("Argument expected.<br/>", ch);
            return;
        }
        break;

    case AT_DES:
        if (unit_is_edited(unt))
        {
            send_to_char("Unit is already being edited.<br/>", ch);
            return;
        }
        CHAR_DESCRIPTOR(ch)->editing = unt;
        /* handle this later */
        break;

    case AT_VAL:
        send_to_char("Arg:&lt;value&gt;<br/>", ch);
        argument = str_next_word(argument, arg);
        if (str_is_empty(arg))
        {
            send_to_char("Numeric argument expected.<br/>", ch);
            return;
        }
        valarg = atoi(arg);
        sprintf(buf, "Value is %ld<br/>", valarg);
        send_to_char(buf, ch);
        break;

    case AT_BIT:
        send_to_char("Arg:&lt;bitlist&gt;<br/>", ch);
        argument = str_next_word(argument, arg);
        if ((bitarg = get_bit(arg, unit_field_data[type].structure)) == -1)
        {
            send_to_char("Invalid or missing bit for field<br/>", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
        }
        sprintf(buf, "Bit found is %ld<br/>", bitarg);
        send_to_char(buf, ch);
        break;

    case AT_TYP:
        send_to_char("Arg:&lt;type&gt;<br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, unit_field_data[type].structure)) == -1)
        {
            send_to_char("Invalid or missing type for field.<br/>", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
        }
        sprintf(buf, "Type found is %s [%d]<br/>",
                unit_field_data[type].structure[typarg], typarg);
        send_to_char(buf, ch);
        break;

    case AT_UNT:
        send_to_char("Arg:&lt;unitpath&gt;<br/>", ch);
        argument = str_next_word(argument, strarg);
        /* find unit by 'path' name in arg */
        if ((untarg = str_to_file_index(strarg)) == NULL)
        {
            send_to_char("Invalid or missing unit path for field.<br/>", ch);
            return;
        }
        sprintf(buf, "Unit pointer is [%s@%s]<br/>",
                untarg->name, untarg->zone->name);
        send_to_char(buf, ch);
        break;

    case AT_KEYDES:
        send_to_char("Arg:&lt;string&gt; (description)<br/>", ch);
        if (str_is_empty(argument))
        {
            send_to_char("Missing string argument.<br/>", ch);
            return;
        }
        if (unit_is_edited(unt))
        {
            send_to_char("Unit is already being edited.<br/>", ch);
            return;
        }
        CHAR_DESCRIPTOR(ch)->editing = unt;
        /* handle takes care of the rest */
        break;

    case AT_TYPVAL:
        send_to_char("Arg:&lt;type&gt; <value><br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, unit_field_data[type].structure)) == -1)
        {
            send_to_char("Invalid or missing type for field.<br/>", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
        }
        sprintf(buf, "Type found is %s [%d]<br/>",
                unit_field_data[type].structure[typarg], typarg);
        send_to_char(buf, ch);
        argument = str_next_word(argument, arg);
        if (str_is_empty(arg))
        {
            send_to_char("Numeric argument expected.<br/>", ch);
            return;
        }
        valarg = atoi(arg);
        sprintf(buf, "Value is %ld<br/>", valarg);
        send_to_char(buf, ch);
        break;

    case AT_DIRBIT:
        send_to_char("Arg:&lt;direction&gt; <bitlist><br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, dirs)) == -1)
        {
            send_to_char("Invalid direction.<br/>", ch);
            show_structure(dirs, ch);
            return;
        }
        sprintf(buf, "Direction found is %s [%d]<br/>", dirs[typarg], typarg);
        send_to_char(buf, ch);
        argument = str_next_word(argument, arg);
        if ((bitarg = get_bit(arg, unit_field_data[type].structure)) == -1)
        {
            send_to_char("Invalid or missing bit for field.<br/>", ch);
            show_structure(unit_field_data[type].structure, ch);
            return;
        }
        sprintf(buf, "Bit found is %ld<br/>", bitarg);
        send_to_char(buf, ch);
        break;

    case AT_DIRSTR:
        send_to_char("Arg:&lt;direction&gt; <string><br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, dirs)) == -1)
        {
            send_to_char("Invalid direction.<br/>", ch);
            show_structure(dirs, ch);
            return;
        }
        sprintf(buf, "Direction found is %s [%d]<br/>", dirs[typarg], typarg);
        send_to_char(buf, ch);
        if (str_is_empty(argument))
        {
            send_to_char("Missing string argument.<br/>", ch);
            return;
        }
        argument = skip_spaces(argument);
        break;

    case AT_DIRUNT:
        send_to_char("Arg:&lt;direction&gt; <unitpath><br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, dirs)) == -1)
        {
            send_to_char("Invalid or missing direction.<br/>", ch);
            show_structure(dirs, ch);
            return;
        }
        sprintf(buf, "Direction found is %s [%d]<br/>", dirs[typarg], typarg);
        send_to_char(buf, ch);
        argument = str_next_word(argument, arg);
        if ((untarg = str_to_file_index(arg)) == NULL)
        {
            send_to_char("Invalid or missing unit path for field.<br/>", ch);
            return;
        }
        sprintf(buf, "Unit pointer is [%s@%s]<br/>",
                untarg->name, untarg->zone->name);
        send_to_char(buf, ch);
        break;

    case AT_DIRDES:
        send_to_char("Arg:&lt;direction&gt; (description)<br/>", ch);
        argument = str_next_word(argument, arg);
        if ((typarg = get_type(arg, dirs)) == -1)
        {
            send_to_char("Invalid or missing direction.<br/>", ch);
            show_structure(dirs, ch);
            return;
        }
        sprintf(buf, "Direction found is %s [%d]<br/>", dirs[typarg], typarg);
        send_to_char(buf, ch);
        if (unit_is_edited(unt))
        {
            send_to_char("Unit is already being edited.<br/>", ch);
            return;
        }
        CHAR_DESCRIPTOR(ch)->editing = unt;
        /* handle rest later */
        break;

    default:
        send_to_char("Forbidden argument type for field, please "
                     "contact implementators.<br/>",
                     ch);
        return;
    }

    argument = skip_spaces(argument);
    strip_trailing_spaces(argument);

    /* insert data read in argument */
    switch (type)
    {
    case 0: /* "add-name" */
        if (IS_PC(unt) && CHAR_LEVEL(ch) < 255)
        {
            send_to_char("Not allowed to modify PC's.<br/>", ch);
            return;
        }
        argument = skip_spaces(argument);
        strip_trailing_blanks(argument);
        str_remspc(argument);

        UNIT_NAMES(unt).AppendName(argument);
        send_to_char("The extra name was added.<br/>", ch);
        return;

    case 1: /* "del-name" */
        if (!UNIT_NAMES(unt).Name(0) || !UNIT_NAMES(unt).Name(1))
        {
            send_to_char("Must have minimum of two names<br/>", ch);
            return;
        }
        argument = skip_spaces(argument);
        UNIT_NAMES(unt).RemoveName(argument);
        send_to_char("Name may have been deleted.<br/>", ch);
        return;

    case 2: /* "title",   */
        UNIT_TITLE(unt) = (argument);

        send_to_char("Title modified.<br/>", ch);
        return;

    case 3: /* "outside description" */
        send_to_char("Modifying long description.<br/>", ch);
        CHAR_DESCRIPTOR(ch)->postedit = edit_outside_descr;
        set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add,
                            TRUE);
        return;

    case 4: /* "add-extra" */
        argument = str_next_word(argument, strarg);
        act("Searching for $2t.", A_ALWAYS, ch, strarg, cActParameter(), TO_CHAR);

        if ((ed = unit_find_extra(strarg, unt)) == NULL)
        {
            /* the field was not found. create a new one. */
            ed = new (class extra_descr_data);
            while (*strarg) /* insert names */
            {
                ed->names.AppendName(strarg);
                argument = str_next_word(argument, strarg);
            }
            UNIT_EXTRA(unt).add(ed);

            send_to_char("New field.<br/>", ch);
        }
        else
        {
            /* add the rest of the names if they do not exist */
            argument = str_next_word(argument, strarg);
            while (*strarg)
            {
                if (ed->names.IsName(strarg))
                    ed->names.AppendName(strarg);

                argument = str_next_word(argument, strarg);
            }

            send_to_char("Modifying existing description.<br/>", ch);
        }

        CHAR_DESCRIPTOR(ch)->editref = ed;
        CHAR_DESCRIPTOR(ch)->postedit = edit_extra;
        CHAR_DESCRIPTOR(ch)->editing = unt;
        set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add,
                            TRUE);
        return;

    case 5: /* "del-extra" */
        if (str_is_empty(argument))
        {
            send_to_char("You must supply a field name.<br/>", ch);
            return;
        }

        UNIT_EXTRA(unt).remove(argument);
        send_to_char("Trying to delete field.<br/>", ch);
        return;

    case 6: /* "manipulate-flags" */
        UNIT_MANIPULATE(unt) = bitarg;
        return;

    case 7: /* "unit-flags" */
        if (IS_SET(UNIT_FLAGS(unt), UNIT_FL_TRANS) &&
            !IS_SET(bitarg, UNIT_FL_TRANS))
            trans_unset(unt);
        else if (!IS_SET(UNIT_FLAGS(unt), UNIT_FL_TRANS) &&
                 IS_SET(bitarg, UNIT_FL_TRANS))
            trans_set(unt);

        UNIT_FLAGS(unt) = bitarg;
        return;

    case 8: /* "weight" */
        UNIT_BASE_WEIGHT(unt) = UNIT_WEIGHT(unt) = valarg;
        return;

    case 9: /* "capacity" */
        UNIT_CAPACITY(unt) = valarg;
        return;

    case 10: /* "max_hp" */
        UNIT_MAX_HIT(unt) = valarg;
        return;

    case 11: /* "hp" */
        UNIT_HIT(unt) = valarg;
        return;

    case 12: /* "key" */
        argument = str_next_word(argument, strarg);
        UNIT_KEY(unt) = str_dup(strarg);
        return;

    case 13: /* "alignment" */
        if (is_in(valarg, -1000, 1000))
            UNIT_ALIGNMENT(unt) = valarg;
        else
            send_to_char("Shame on you: Value must be in -1000..+1000!<br/>", ch);
        return;

    case 14: /* "open_flags" */
        UNIT_OPEN_FLAGS(unt) = bitarg;
        return;

    case 15: /* "tgh" OBSOLETE */
        /* UNIT_TGH(unt) = valarg; */
        return;

    case 16: /* "lights" */
        UNIT_LIGHTS(unt) = valarg;
        UNIT_BRIGHT(unt) = valarg;
        UNIT_ILLUM(unt) = valarg;
        send_to_char("WARNING: This value is absolute and will cause 'darkness' "
                     "bugs if not used properly! Only use this to fix darkness "
                     "- use bright for changing the illumination!<br/>",
                     ch);
        return;

    case 17: /* "bright" */
        if (!is_in(valarg, -6, 6))
        {
            send_to_char("Expected -6..+6<br/>", ch);
            return;
        }
        modify_bright(unt, valarg - UNIT_BRIGHT(unt));
        return;

    case 18: /* "room_flags" */
        ROOM_FLAGS(unt) = bitarg;
        return;

    case 19: /* "movement" */
        ROOM_LANDSCAPE(unt) = typarg;
        return;

    case 20: /* "ccinfo" */
        if (valarg == -1)
        {
            send_to_char("Erasing CC information.<br/>", ch);
            PC_ACCOUNT(unt).last4 = -1;
            PC_ACCOUNT(unt).cracks = 0;
        }
        else
        {
            if (is_in(valarg, 0, 9999))
            {
                send_to_char("Setting CC information.<br/>", ch);
                PC_ACCOUNT(unt).last4 = valarg;
                PC_ACCOUNT(unt).cracks = 0;
            }
            else
                send_to_char("Illegal value, expected -1 or 0..9999.<br/>", ch);
        }
        return;

    case 21: /* "add-dir-name" */
        if (!ROOM_EXIT(unt, typarg))
        {
            send_to_char("No such exit.<br/>", ch);
            return;
        }

        argument = str_next_word(argument, strarg);
        ROOM_EXIT(unt, typarg)->open_name.AppendName(strarg);
        return;

    case 22: /* "del-dir-name" */
        if (!ROOM_EXIT(unt, typarg))
        {
            send_to_char("No such exit.<br/>", ch);
            return;
        }

        argument = str_next_word(argument, strarg);
        ROOM_EXIT(unt, typarg)->open_name.AppendName(strarg);
        return;

    case 23: /* "dir-flags" */
        if (!ROOM_EXIT(unt, typarg))
        {
            send_to_char("No such exit.<br/>", ch);
            return;
        }

        ROOM_EXIT(unt, typarg)->exit_info = bitarg;
        return;

    case 24: /* "dir-key" */
        if (!ROOM_EXIT(unt, typarg))
        {
            send_to_char("No such exit.<br/>", ch);
            return;
        }

        argument = str_next_word(argument, strarg);
        ROOM_EXIT(unt, typarg)->key = str_dup(strarg);
        return;

    case 25: /* "value0" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }

        /* Should be expanded to handle the different object-types */
        OBJ_VALUE(unt, 0) = valarg;
        return;

    case 26: /* "value1" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }
        /* Should be expanded to handle the different object-types */
        OBJ_VALUE(unt, 1) = valarg;
        return;

    case 27: /* "value2" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }
        /* Should be expanded to handle the different object-types */
        OBJ_VALUE(unt, 2) = valarg;
        return;

    case 28: /* "value3" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }
        /* Should be expanded to handle the different object-types */
        OBJ_VALUE(unt, 3) = valarg;
        return;

    case 29: /* "value4" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }
        /* Should be expanded to handle the different object-types */
        OBJ_VALUE(unt, 4) = valarg;
        return;

    case 30: /* "obj-flags" */
        OBJ_FLAGS(unt) = bitarg;
        return;

    case 31: /* "cost" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }

        OBJ_PRICE(unt) = valarg;
        return;

    case 32: /* "rent" */
        OBJ_PRICE_DAY(unt) = valarg;
        return;

    case 33: /* "type" */
        switch (OBJ_TYPE(unt))
        {
        case ITEM_MONEY:
            send_to_char("Operation not allowed on this type of object.<br/>",
                         ch);
            return;
        }

        if (typarg == ITEM_MONEY)
        {
            send_to_char("Operation not allowed to that kind of object.<br/>",
                         ch);
            return;
        }

        OBJ_TYPE(unt) = typarg;
        return;

    case 34: /* "equip" */
        OBJ_EQP_POS(unt) = typarg;
        return;

    case 35: /* "guild-name" */
        if (str_ccmp_next_word(argument, "none"))
        {
            if (PC_GUILD(unt))
            {
                FREE(PC_GUILD(unt));
                PC_GUILD(unt) = NULL;
            }
            send_to_char("Changed.<br/>", ch);
            return;
        }
        if (PC_GUILD(unt) == NULL)
        {
            CREATE(PC_GUILD(unt), char, strlen(argument) + 1);
        }
        else
        {
            RECREATE(PC_GUILD(unt), char, strlen(argument) + 1);
        }
        strcpy(PC_GUILD(unt), argument);
        send_to_char("Changed.<br/>", ch);
        return;

    case 36: /* "pwd" */
        argument = str_next_word(argument, strarg);
        if (CHAR_LEVEL(ch) < CHAR_LEVEL(unt))
        {
            slog(LOG_ALL, 0, "WARNING: %s attempted to set %s password",
                 UNIT_NAME(ch), UNIT_NAME(unt));
            send_to_char("You can not change a password of a higher level immortal", ch);
        }
        else
        {
            slog(LOG_ALL, 0, "PASSWORD: %s changed %s's password.", UNIT_NAME(ch), UNIT_NAME(unt));
            strncpy(PC_PWD(unt), crypt(strarg, UNIT_NAME(unt)), 10);
            send_to_char("The password has been set.<br/>", ch);
        }
        return;

    case 37: /* "setup-flags" */
        PC_FLAGS(unt) = bitarg;
        return;

    case 38: /* "crimes" */
        PC_CRIMES(unt) = valarg;
        return;

    case 39: /* "drunk" */
        PC_COND(unt, DRUNK) = valarg;
        return;

    case 40: /* "full" */
        PC_COND(unt, FULL) = valarg;
        return;

    case 41: /* "thirsty" */
        PC_COND(unt, THIRST) = valarg;
        return;

    case 42: /* "default-pos" */
        NPC_DEFAULT(unt) = valarg;
        return;

    case 43: /* "act-flags" */
        NPC_FLAGS(unt) = bitarg;
        return;

    case 44: /* hometown */
        if (PC_HOME(unt) == NULL)
        {
            CREATE(PC_HOME(unt), char, strlen(argument) + 1);
        }
        else
        {
            RECREATE(PC_HOME(unt), char, strlen(argument) + 1);
        }
        strcpy(PC_HOME(unt), argument);
        send_to_char("Changed.<br/>", ch);
        return;

    case 45: /* "exp" */
        if (CHAR_LEVEL(unt) < MORTAL_MAX_LEVEL && valarg > required_xp(1000))
            send_to_char("You are not allowed to set exp that high for "
                         "mortals<br/>",
                         ch);
        else
            CHAR_EXP(unt) = valarg;
        return;

    case 46: /* "affected-by" */
        CHAR_FLAGS(unt) = bitarg;
        return;

    case 47: /* "mana" */
        CHAR_MANA(unt) = valarg;
        return;

    case 48: /* "endurance" */
        CHAR_ENDURANCE(unt) = valarg;
        return;

    case 49: /* "attack-type" */
        CHAR_ATTACK_TYPE(unt) = typarg;
        return;

    case 50: /* "hand-quality" OBSOLETE */
        /*
           if (is_in(valarg, 0, 200))
           CHAR_HAND_QUALITY(unt) = valarg;
           else
           send_to_char("Shame on you: Value must be in 0%..200%!<br/>", ch); */
        return;

    case 51: /* "height" */
        UNIT_SIZE(unt) = valarg;
        return;

    case 52: /* "race" */
        CHAR_RACE(unt) = typarg;
        return;

    case 53: /* "sex" */
        CHAR_SEX(unt) = typarg;
        return;

    case 54: /* "level" */
        if (is_in(valarg, 0, 199))
            CHAR_LEVEL(unt) = valarg;
        else
            send_to_char("Shame on you: Value must be in 0..199!<br/>", ch);
        return;

    case 55: /* "position" */
        CHAR_POS(unt) = typarg;
        return;

    case 56: /* "ability" */
        if (is_in(valarg, 0, 250))
            CHAR_ABILITY(unt, typarg) = valarg;
        else
            send_to_char("Shame on you: Value must be in 0%..250%!<br/>", ch);
        return;

    case 57: /* "skill-points" */
        PC_SKILL_POINTS(unt) = valarg;
        return;

    case 58: /* "ability-points" */
        PC_ABILITY_POINTS(unt) = valarg;
        return;

    case 59: /* "remove-affect" */
        if ((aff = affected_by_spell(unt, valarg)))
        {
            destroy_affect(aff);
            send_to_char("Affect attempted removed.<br/>", ch);
        }
        else
            send_to_char("No such affect.<br/>", ch);
        return;

    case 60: /* "add-quest" */
        quest_add(unt, argument, "");
        send_to_char("New quest.<br/>", ch);
        return;

    case 61: /* "del-quest" */
        if (str_is_empty(argument))
        {
            send_to_char("You must supply a field name.<br/>", ch);
            return;
        }

        send_to_char("Attempting to remove such a quest.<br/>", ch);

        PC_QUEST(unt).remove(argument);
        return;

    case 62: /* "speed" */
        if (is_in(valarg, SPEED_MIN, SPEED_MAX))
            CHAR_SPEED(unt) = valarg;
        else
            send_to_char("Speed must be in [12..200]!<br/>", ch);
        return;

    case 63: /* "add-info" */
        argument = str_next_word(argument, strarg);
        act("Searching for $2t.", A_ALWAYS, ch, strarg, cActParameter(), TO_CHAR);

        if ((ed = PC_INFO(unt).find_raw(strarg)) == NULL)
        {
            /* the field was not found. create a new one. */
            ed = new (class extra_descr_data);
            while (*strarg) /* insert names */
            {
                ed->names.AppendName(strarg);
                argument = str_next_word(argument, strarg);
            }
            PC_INFO(unt).add(ed);

            send_to_char("New field.<br/>", ch);
        }
        else
        {
            /* add the rest of the names if they do not exist */
            argument = str_next_word(argument, strarg);
            while (*strarg)
            {
                if (ed->names.IsName(strarg))
                    ed->names.AppendName(strarg);

                argument = str_next_word(argument, strarg);
            }

            send_to_char("Modifying existing description.<br/>", ch);
        }

        CHAR_DESCRIPTOR(ch)->editref = ed;
        CHAR_DESCRIPTOR(ch)->postedit = edit_info;
        CHAR_DESCRIPTOR(ch)->editing = unt;
        set_descriptor_fptr(CHAR_DESCRIPTOR(ch), interpreter_string_add,
                            TRUE);
        return;

    case 64: /* "del-info" */
        if (str_is_empty(argument))
        {
            send_to_char("You must supply a field name.<br/>", ch);
            return;
        }

        send_to_char("Attempting to remove such an info.<br/>", ch);

        PC_INFO(unt).remove(argument);
        return;

    case 65: /* "access" */
        PC_ACCESS_LEVEL(unt) = valarg;
        return;

    case 66: /* "promptstr" */
        if (UPC(unt)->promptstr != NULL)
            free(UPC(unt)->promptstr);
        UPC(unt)->promptstr = str_dup(argument);
        send_to_char("Prompt String Modified.<br/>", ch);
        return;

    case 67: /* "age" */
        /* This sets a time_t so that the PC's age
         * will be as many years as valarg
         */
        if (is_in(valarg, 10, 1000))
        {
            slog(LOG_BRIEF, CHAR_LEVEL(ch),
                 "SET %s set %s's age from %.2f to %d",
                 UNIT_NAME(ch), UNIT_NAME(unt),
                 PC_TIME(unt).birth / (1.0 * SECS_PER_MUD_YEAR), valarg);

            PC_TIME(unt).birth = time(NULL) - (valarg * SECS_PER_MUD_YEAR);
        }
        else
            send_to_char("Value must be in 10..1000!<br/>", ch);
        return;

    case 68: /* "lifespan" */
        /* This sets a time_t so that the PC's age
         * will be as many years as valarg
         */
        if (is_in(valarg, 10, 1000))
        {
            slog(LOG_BRIEF, CHAR_LEVEL(ch),
                 "SET %s set %s's lifespan from %d to %d",
                 UNIT_NAME(ch), UNIT_NAME(unt), PC_LIFESPAN(unt), valarg);
            PC_LIFESPAN(unt) = valarg;
        }
        else
            send_to_char("Value must be in 10..1000!<br/>", ch);
        return;

    case 69: /* "profession" */
        if (is_in(valarg, 0, PROFESSION_MAX - 1))
        {
            slog(LOG_BRIEF, CHAR_LEVEL(ch),
                 "SET %s set %s's profession from %d to %d",
                 UNIT_NAME(ch), UNIT_NAME(unt), PC_PROFESSION(unt), valarg);
            PC_PROFESSION(unt) = valarg;
        }
        else
            send_to_char("Value must be in 0..PROFESION MAX<br/>", ch);
        return;

    default: /* undefined field type */
        send_to_char("Field-type is undefined, please contact implementor<br/>",
                     ch);
        return;
    }
}

/*
 *  Modification of character skills
 */

#define SET_SKILL 0
#define SET_SPELL 1
#define SET_WEAPON 2

static const char *skill_field_names[] = {
    "skill",
    "spell",
    "weapon",
    NULL};

void do_setskill(class unit_data *ch, char *argument,
                 const struct command_info *cmd)
{
    int type, skillarg, valarg = 0;
    char buf[MAX_STRING_LENGTH], arg[MAX_STRING_LENGTH];
    class unit_data *unt;

    if (!CHAR_DESCRIPTOR(ch))
        return;

    /* Check argument */
    if (str_is_empty(argument))
    {
        send_to_char("Syntax: setskill &lt;name&gt; (skill|spell|weapon)"
                     " &lt;field&gt; &lt;value&gt;.<br/>",
                     ch);
        return;
    }

    /* find unit to set */
    if ((unt = find_unit(ch, &argument, 0, FIND_UNIT_WORLD)) == NULL)
    {
        send_to_char("No such person or thing<br/>", ch);
        return;
    }

    if (!IS_CHAR(unt))
    {
        send_to_char("Unit-type must be char<br/>", ch);
        return;
    }

    /* find field to change */
    argument = str_next_word(argument, arg);
    /* Partial match on fields */
    if ((type = search_block(arg, skill_field_names, TRUE)) == -1)
    {
        send_to_char("Missing or invalid skill-type, <br/>"
                     "Use 'skill' 'spell' or 'weapon'.<br/>",
                     ch);
        return;
    }

    argument = str_next_word(argument, arg);

    switch (type)
    {
    case SET_SKILL:
        if (!IS_PC(unt))
        {
            send_to_char("Skills are only for PC's<br/>", ch);
            return;
        }
        if ((skillarg = get_type(arg, g_SkiColl.text)) == -1)
        {
            send_to_char("Invalid or missing skill<br/>", ch);
            show_structure(g_SkiColl.text, ch);
            return;
        }
        argument = str_next_word(argument, arg);
        valarg = atoi(arg);
        PC_SKI_SKILL(unt, skillarg) = valarg;
        break;

    case SET_SPELL:
        if ((skillarg = get_type(arg, g_SplColl.text)) == -1)
        {
            send_to_char("Invalid or missing spell<br/>", ch);
            show_structure(g_SplColl.text, ch);
            return;
        }
        if (skillarg > SPL_EXTERNAL && IS_NPC(unt))
        {
            send_to_char("Only spell-groups for NPC<br/>", ch);
            return;
        }
        argument = str_next_word(argument, arg);
        valarg = atoi(arg);

        if (IS_PC(unt))
            PC_SPL_SKILL(unt, skillarg) = valarg;
        else
            NPC_SPL_SKILL(unt, skillarg) = valarg;
        break;

    case SET_WEAPON:
        if ((skillarg = get_type(arg, g_WpnColl.text)) == -1)
        {
            send_to_char("Invalid or missing weaponskill<br/>", ch);
            show_structure(g_WpnColl.text, ch);
            return;
        }
        if (skillarg > WPN_SPECIAL && IS_NPC(unt))
        {
            send_to_char("Only weapon-groups for NPC<br/>", ch);
            return;
        }
        argument = str_next_word(argument, arg);
        valarg = atoi(arg);

        if (IS_PC(unt))
            PC_WPN_SKILL(unt, skillarg) = valarg;
        else
            NPC_WPN_SKILL(unt, skillarg) = valarg;
    }

    sprintf(buf, "New value: %d<br/>Ok.<br/>", valarg);
    send_to_char(buf, ch);
}
