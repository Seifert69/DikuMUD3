/*
 $Author: All $
 $RCSfile: reception.cpp,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.9 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "structs.h"
#include "utils.h"
#include "textutil.h"
#include "comm.h"
#include "handler.h"
#include "db.h"
#include "interpreter.h"
#include "affect.h"
#include "utility.h"
#include "spells.h"
#include "money.h"
#include "files.h"
#include "db_file.h"

int write_unit_string(ubit8 *b, class unit_data *u);

/* *************************************************************************
 * Routines for calculating rent                                           *
 ************************************************************************* */

static int rent_info;

static void
show_items(class unit_data *ch, class unit_data *item, ubit32 price)
{
    if (price > 0)
    {
        price = money_round_up(price, local_currency(ch), 2);

        act("$2t for $3n", A_ALWAYS, ch,
            money_string(price, local_currency(ch), FALSE), item, TO_CHAR);
        rent_info = TRUE;
    }
}

/* ----------------------------------------------------------------- */

static void
subtract_rent(class unit_data *ch, class unit_data *item, ubit32 price)
{
    if (price > 0)
    {
        if (!char_can_afford(ch, price, DEF_CURRENCY))
        {
            act("You couldn't afford to keep $2n.", A_ALWAYS, ch, item, 0, TO_CHAR);
            extract_unit(item);
        }
        else
            money_transfer(ch, NULL, price, DEF_CURRENCY);
    }
}

/* ----------------------------------------------------------------- */

static ubit32
subtract_recurse(class unit_data *ch,
                 class unit_data *item,
                 ubit32 seconds,
                 void (*fptr)(class unit_data *ch,
                              class unit_data *obj, ubit32 price))
{
    ubit32 sum = 0;

    if (IS_IMMORTAL(ch))
        return 0;

    if (item == NULL)
        return 0;

    if (!UNIT_MINV(item))
        sum += subtract_recurse(ch, UNIT_CONTAINS(item), seconds, fptr);

    sum += subtract_recurse(ch, item->next, seconds, fptr);

    if (IS_OBJ(item) && !UNIT_MINV(item))
    {
        ubit32 price = 0;

        if (OBJ_PRICE_DAY(item) > 0)
        {
            price = (int)((float)OBJ_PRICE_DAY(item) * (float)seconds /
                          (float)SECS_PER_REAL_DAY);

            price = (price * g_cServerConfig.m_nRentModifier) / 100;

            sum += price;
        }

        if (fptr)
            (*fptr)(ch, item, price);
    }

    return sum;
}

/* ----------------------------------------------------------------- */

ubit32 rent_calc(class unit_data *ch, time_t savetime)
{
    ubit32 sum = 0;

    assert(IS_PC(ch));

    if (CHAR_DESCRIPTOR(ch) == NULL) /* If loading or similar, dont subtract! */
        return 0;

    if (IS_MORTAL(ch))
    {
        time_t t = time(0);

        if ((t > savetime) && (savetime > 0))
        {
            t -= savetime;

            if (t > SECS_PER_REAL_MIN * 10)
                sum = subtract_recurse(ch, UNIT_CONTAINS(ch), t, subtract_rent);
        }
    }

    return sum;
}

void do_rent(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    ubit32 sum;

    rent_info = FALSE;

    sum = subtract_recurse(ch, UNIT_CONTAINS(ch),
                           SECS_PER_REAL_DAY, show_items);

    if (!rent_info)
        send_to_char("You are charged no rent.<br/>", ch);
    else
        act("Your inventory costs $2t per day to rent.",
            A_ALWAYS, ch, money_string(sum, local_currency(ch), FALSE),
            NULL, TO_CHAR);
}

/* *************************************************************************
 * Routines for loading and saving inventory                               *
 ************************************************************************* */

/* Header of compressed byte-string
   note: sice the length of the entire diff-set is often needed even when
   the head-struct may not be available, the diff-length is not stored in
   the header. */
struct diffhead
{
    ubit32 reflen;   /* length of reference (for checking) */
    ubit32 crc;      /* extra check - maybe one enough? */
    short int start; /* offset to first different byte */
    short int end;   /* offset to first similar byte after start */
};

/* Per-object header */
struct objheader
{
    sbit16 length; /* length of data */
    char zone[FI_MAX_ZONENAME + 1];
    char unit[FI_MAX_UNITNAME + 1];
    ubit8 level;      /* level of 'containment' (depth) */
    ubit8 equip;      /* equipment position */
    ubit8 compressed; /* compressed? */
    ubit8 type;       /* NPC or OBJ? */
};

/* Local global variables */

/*
static char *membuf = 0;
static int membuflen = 0, mempos;
*/

/* Global variables */
class file_index_type *slime_fi = NULL;

/* save object */
void enlist(CByteBuffer *pBuf, class unit_data *unit, int level, int fast)
{
    int len;
    struct objheader h;
    CByteBuffer TmpBuf;

    /* On 64 bit Linux at least, the struct is padded and 1 byte larger than its data */
    /* So I memset to zero to avoid valgrind reporting a memory error                 */
    memset(&h, 0, sizeof(h)); 

    int diff(char *ref, ubit32 reflen, char *obj, int objlen, char *dif,
             int diflen, ubit32 crc);

    if (!IS_SET(UNIT_TYPE(unit), UNIT_ST_NPC | UNIT_ST_OBJ))
    {
        slog(LOG_ALL, 0,
             "MAJOR ERROR - enlist a non-NPC or non-OBJ is being saved. Aborted");
        return;
    }
    if (unit->is_destructed())
    {
        slog(LOG_ALL, 0,
             "MAJOR ERROR - enlist a destructed unit is being saved. Aborted");
        return;
    }

    len = write_unit_string(&TmpBuf, unit);

    if (fast || !UNIT_FILE_INDEX(unit))
    {
        h.compressed = 0;
    }
    else
    {
        assert(FALSE);
    }

    strcpy(h.zone, UNIT_FI_ZONENAME(unit));
    strcpy(h.unit, UNIT_FI_NAME(unit));

    h.type = UNIT_TYPE(unit);
    h.level = level;

    if (IS_OBJ(unit))
        h.equip = OBJ_EQP_POS(unit);
    else
        h.equip = 0;

    h.length = len;

    pBuf->Append((ubit8 *)&h, sizeof(h));
    pBuf->Append(&TmpBuf);
}

/* Arguments:                                                    */
/*    unit   - The container to be saved (with contents)         */
/*    level  - 0 for contents only, 1 for contents & container   */
/*    fast   - TRUE for compression, FALSE for no compression.   */

void add_units(CByteBuffer *pBuf, class unit_data *parent,
               class unit_data *unit, int level, int fast)
{
    int tmp_i = 0;
    class unit_data *tmp_u;

    if (IS_ROOM(unit))
    {
        slog(LOG_ALL, 0, "BAD ERROR: Room attempted saved as inventory!");
        return;
    }

    if ((tmp_u = UNIT_CONTAINS(unit)))
    {
        if (IS_OBJ(tmp_u) && (tmp_i = OBJ_EQP_POS(tmp_u)))
        {
            unequip_object(tmp_u);
            OBJ_EQP_POS(tmp_u) = tmp_i;
        }

        unit_from_unit(tmp_u);

        add_units(pBuf, parent, unit, level, fast);

        if (IS_OBJ(tmp_u) || IS_NPC(tmp_u))
            add_units(pBuf, parent, tmp_u, level + 1, fast);

        unit_to_unit(tmp_u, unit);

        if (IS_OBJ(tmp_u) && tmp_i)
        {
            OBJ_EQP_POS(tmp_u) = 0;
            equip_char(unit, tmp_u, tmp_i);
        }
    }
    else /* UNIT CONTAINS NOTHING */ if ((level != 0) && (IS_OBJ(unit) || IS_NPC(unit)) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
        enlist(pBuf, unit, level, fast);
}

void send_saves(class unit_data *parent, class unit_data *unit)
{
    if (!unit)
        return;

    send_saves(parent, UNIT_CONTAINS(unit));
    send_saves(parent, unit->next);

    if ((IS_OBJ(unit) || IS_NPC(unit)) &&
        !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
        send_save_to(parent, unit);
}

char *
ContentsFileName(const char *pName)
{
    static char Buf[MAX_INPUT_LENGTH + 1];

    char *PlayerFileName(const char *);

    sprintf(Buf, "%s.inv", PlayerFileName(pName));

    return Buf;
}

/* Save all units inside 'unit' in the blk_file 'bf' as uncompressed  */
/* if fast == 1 or compressed if fast == 0. Only OBJ's and NPC's will */
/* be saved!                                                          */
/* Container = 1 if container should be saved also                    */
void basic_save_contents(const char *pFileName, class unit_data *unit,
                         int fast, int bContainer)
{
    class descriptor_data *tmp_descr = NULL;
    FILE *pFile;
    char TmpName[MAX_INPUT_LENGTH + 1];

    fast = 1; /* MAJOR BUG IN DIFF CAUSING BAD PLAYERS! WITH TOO MUCH */
    /* INVENTORY, THE PLAYER INDEX WILL GET FUCKED UP!      */

    if (IS_CHAR(unit))
    {
        tmp_descr = CHAR_DESCRIPTOR(unit);
        CHAR_DESCRIPTOR(unit) = NULL;
    }

    CByteBuffer *pBuf = &g_FileBuffer;
    pBuf->Clear();

    if (bContainer)
        send_save_to(unit, unit);

    send_saves(unit, UNIT_CONTAINS(unit));

    add_units(pBuf, unit, unit, bContainer ? 1 : 0, fast);

    if (IS_CHAR(unit))
        CHAR_DESCRIPTOR(unit) = tmp_descr;

    if (pBuf->GetLength() > 0)
    {
        strcpy(TmpName, ContentsFileName("aaa-inv.tmp"));
        pFile = fopen(TmpName, "wb");
        assert(pFile);
        pBuf->FileWrite(pFile);
        fclose(pFile);

        if (rename(TmpName, pFileName) != 0)
        {
            perror("rename:");
            exit(2);
        }
    }
}

/* Save all units inside 'unit' in the blk_file 'bf' as uncompressed  */
/* if fast == 1 or compressed if fast == 0. Only OBJ's and NPC's will */
/* be saved!                                                          */
/* Container = 1 if container should be saved also                    */
int save_contents(const char *pFileName, class unit_data *unit,
                  int fast, int bContainer)
{
    char name[MAX_INPUT_LENGTH + 1];

    fast = 1; /* MAJOR BUG IN DIFF CAUSING BAD PLAYERS! WITH TOO MUCH */
    /* INVENTORY, THE PLAYER INDEX WILL GET FUCKED UP!      */

    strcpy(name, ContentsFileName(pFileName));

    if (!UNIT_CONTAINS(unit))
    {
        remove(name);
        return 0;
    }

    basic_save_contents(name, unit, fast, bContainer);

    return subtract_recurse(unit, UNIT_CONTAINS(unit),
                            SECS_PER_REAL_DAY, NULL);
}

/* From the block_file 'bf' at index 'blk_idx' load the objects    */
/* and place them inside 'unit' by unit_to_unit and possibly equip */
/* Return the top level unit loaded                                */

class unit_data *
base_load_contents(const char *pFileName, const class unit_data *unit)
{
    struct objheader h;
    class file_index_type *fi;
    class unit_data *pnew, *pnew_tmp, *pstack[25];
    int len, init;
    int frame, n;
    class descriptor_data *tmp_descr = NULL;
    int equip_ok;
    FILE *pFile;
    class unit_data *topu = NULL;

    CByteBuffer InvBuf;
    InvBuf.Clear();

    extern class unit_data *void_room;

    int is_slimed(class file_index_type * sp);
    int patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res,
              int reslen, ubit32 crc);

    assert(slime_fi != NULL);

    pFile = fopen(pFileName, "rb");

    if (pFile == NULL)
        return NULL;

    len = fsize(pFile);
    if (len == 0)
    {
        fclose(pFile);
        return NULL;
    }

    n = InvBuf.FileRead(pFile, len);
    fclose(pFile);

    if (n != len)
    {
        slog(LOG_ALL, 0, "Corrupted inventory: %s", pFileName);
        return NULL;
    }

    frame = 0;
    pstack[frame] = (class unit_data *)unit;

    if (unit && IS_CHAR(unit))
    {
        tmp_descr = CHAR_DESCRIPTOR(unit);
        CHAR_DESCRIPTOR(unit) = NULL;
    }

    for (init = TRUE; InvBuf.GetReadPosition() < InvBuf.GetLength();)
    {
        if (InvBuf.Read((ubit8 *)&h, sizeof(h)))
            break;

        fi = find_file_index(h.zone, h.unit);

        pnew = NULL;

        equip_ok = TRUE;

        if (h.compressed)
        {
            slog(LOG_ALL, 0, "Corrupted inventory: %s", pFileName);
            if (unit && IS_CHAR(unit))
            {
                CHAR_DESCRIPTOR(unit) = tmp_descr;
                send_to_char("Your inventory was corrupt, please contact the Admin.<br/>",
                             unit);
            }

            return topu;
        }
        else /* uncompressed */
        {
            if ((fi == NULL) || is_slimed(fi))
            {
                pnew = read_unit(slime_fi);
                pnew_tmp =
                    read_unit_string(&InvBuf, h.type, h.length, "preslime",
                                     FALSE);
                if (g_nCorrupt)
                {
                    slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                    break;
                }
            }
            else
            {
                pnew_tmp = NULL;
                pnew = read_unit_string(&InvBuf, h.type, h.length,
                                        str_cc(fi->name, fi->zone->name));
                if (g_nCorrupt)
                {
                    slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                    break;
                }
                insert_in_unit_list(pnew);
                UNIT_FILE_INDEX(pnew) = fi;
                fi->no_in_mem++;
            }

            if (pnew_tmp && pnew)
            {
                char buf[MAX_STRING_LENGTH];
                sprintf(buf, "The slimy remains of %s", TITLENAME(pnew_tmp));
                UNIT_OUT_DESCR(pnew) = (buf);
                UNIT_TITLE(pnew) = (buf);
                UNIT_NAMES(pnew).PrependName(str_cc("slime of ",
                                                    UNIT_NAME(pnew_tmp)));
                delete pnew_tmp;
            }
        }

        if (init)
        {
            topu = pnew;
            init = FALSE;
        }
        if (pstack[frame] == NULL)
        {
            if (UNIT_IN(pnew))
                pstack[frame] = UNIT_IN(pnew);
            else
                pstack[frame] = void_room;
        }

        UNIT_IN(pnew) = NULL;
        if (pnew == pstack[frame])
        {
            slog(LOG_ALL, 0, "ERROR. Loading inventory, recursive linking. Please report.");
            send_to_char("Your inventory was corrupt, please contact the Admin. Recursive Linking<br/>",
                         unit);
            return topu;
        }
        else
        {

            if (h.level > frame)
            {
                unit_to_unit(pnew, pstack[frame]);
                frame = h.level;
            }
            else
            {
                frame = h.level;
                unit_to_unit(pnew, UNIT_IN(pstack[frame]));
            }

            /* IS_CHAR() needed, since a potential char may have been slimed! */
            if (h.equip && equip_ok && IS_CHAR(UNIT_IN(pnew)))
                equip_char(UNIT_IN(pnew), pnew, h.equip);

            pstack[frame] = pnew;
        }
    }

    if (unit && IS_CHAR(unit))
    {
        CHAR_DESCRIPTOR(unit) = tmp_descr;
    }

    return topu;
}

/* From the block_file 'bf' at index 'blk_idx' load the objects    */
/* and place them inside 'unit' by unit_to_unit and possibly equip */
/* Return the daily cost                                           */
void load_contents(const char *pFileName, class unit_data *unit)
{
    base_load_contents(ContentsFileName(pFileName), unit);
}

void reception_boot(void)
{
    slime_fi = find_file_index("basis", "slime");
    assert(slime_fi);
}

/* ************************************************************************
 * Low-level routines for handling diff/patch                              *
 ************************************************************************* */

/* Create difference-data (patch can reconstruct obj based on ref & dif) */
int diff(char *ref, ubit32 reflen, char *obj, int objlen, char *dif,
         int diflen, ubit32 crc)
{
    int dstart, dend, len, rlen;
    char *oend, *rend;
    struct diffhead head;

    rend = ref + reflen - 1;
    oend = obj + objlen - 1;
    len = objlen;
    rlen = reflen;

    /* find start of difference */
    for (dstart = 0; len && rlen; dstart++, len--, rlen--)
        if (*(ref + dstart) != *(obj + dstart))
            break;

    /* find end of difference */
    for (dend = 0; len && rlen; dend++, len--, rlen--)
        if (*(rend - dend) != *(oend - dend))
            break;

    if ((int)(sizeof(head) + len) > diflen)
        return -1;
    else
        diflen = sizeof(head) + len;
    head.start = dstart;

    head.end = reflen - dend;
    head.reflen = reflen;
    head.crc = crc;

    memcpy(dif, (char *)&head, sizeof(head));
    dif += sizeof(head);
    if (len)
        memcpy(dif, (char *)obj + dstart, len);
    return (diflen);
}

/* reconstruct obj based on ref and diff */
int patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res,
          int reslen, ubit32 crc)
{
    struct diffhead head;

    if (diflen < (int)sizeof(head))
        return -1;

    memcpy((char *)&head, dif, sizeof(head));
    dif += sizeof(head);
    diflen -= sizeof(head);

    if ((int)(head.start + diflen + reflen - head.end) > reslen)
        return -1;

    if (head.reflen != reflen)
        return -1;

    if (head.crc != crc)
        return -1;

    if (head.start)
        memcpy(res, ref, head.start);
    if (diflen)
        memcpy(res + head.start, dif, diflen);
    if (head.end < (int)reflen)
        memcpy(res + head.start + diflen, ref + head.end, reflen - head.end);

    return (head.start + diflen + reflen - head.end);
}

/* ========================= DIL STORE / RESTORE ======================= */

void store_all_unit(class unit_data *u, char *fname, int svcont)
{

    /*fuck  if (!UNIT_FILE_INDEX (u))
        return;*/

    basic_save_contents(fname, u, FALSE, svcont);
}

class unit_data *
restore_all_unit(char *filename, unit_data *udest)
{
    class unit_data *u;
    u = base_load_contents(filename, udest);
    return u;
}
