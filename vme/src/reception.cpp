/*
 $Author: All $
 $RCSfile: reception.cpp,v $
 $Date: 2004/09/21 08:45:46 $
 $Revision: 2.9 $
 */
#include "affect.h"
#include "comm.h"
#include "db.h"
#include "db_file.h"
#include "files.h"
#include "formatter.h"
#include "handler.h"
#include "interpreter.h"
#include "money.h"
#include "pcsave.h"
#include "slime.h"
#include "slog.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"
#include "zon_basis.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

/* *************************************************************************
 * Routines for calculating rent                                           *
 ************************************************************************* */

static int rent_info;

static void show_items(class unit_data *ch, class unit_data *item, ubit32 price)
{
    if (price > 0)
    {
        price = money_round_up(price, local_currency(ch), 2);

        act("$2t for $3n", A_ALWAYS, ch, money_string(price, local_currency(ch), FALSE), item, TO_CHAR);
        rent_info = TRUE;
    }
}

/* ----------------------------------------------------------------- */

static void subtract_rent(class unit_data *ch, class unit_data *item, ubit32 price)
{
    if (price > 0)
    {
        if (!char_can_afford(ch, price, DEF_CURRENCY))
        {
            act("You couldn't afford to keep $2n.", A_ALWAYS, ch, item, cActParameter(), TO_CHAR);
            extract_unit(item);
        }
        else
        {
            money_transfer(ch, nullptr, price, DEF_CURRENCY);
        }
    }
}

/* ----------------------------------------------------------------- */

static ubit32 subtract_recurse(class unit_data *ch,
                               class unit_data *item,
                               ubit32 seconds,
                               void (*fptr)(class unit_data *ch, class unit_data *obj, ubit32 price))
{
    ubit32 sum = 0;

    if (IS_IMMORTAL(ch))
    {
        return 0;
    }

    if (item == nullptr)
    {
        return 0;
    }

    if (!UNIT_MINV(item))
    {
        sum += subtract_recurse(ch, UNIT_CONTAINS(item), seconds, fptr);
    }

    sum += subtract_recurse(ch, item->next, seconds, fptr);

    if (IS_OBJ(item) && !UNIT_MINV(item))
    {
        ubit32 price = 0;

        if (OBJ_PRICE_DAY(item) > 0)
        {
            price = (int)((float)OBJ_PRICE_DAY(item) * (float)seconds / (float)SECS_PER_REAL_DAY);

            price = (price * g_cServerConfig.getRentModifier()) / 100;

            sum += price;
        }

        if (fptr)
        {
            (*fptr)(ch, item, price);
        }
    }

    return sum;
}

/* ----------------------------------------------------------------- */

ubit32 rent_calc(class unit_data *ch, time_t savetime)
{
    ubit32 sum = 0;

    assert(IS_PC(ch));

    if (CHAR_DESCRIPTOR(ch) == nullptr)
    { /* If loading or similar, dont subtract! */
        return 0;
    }

    if (IS_MORTAL(ch))
    {
        time_t t = time(nullptr);

        if ((t > savetime) && (savetime > 0))
        {
            t -= savetime;

            if (t > SECS_PER_REAL_MIN * 10)
            {
                sum = subtract_recurse(ch, UNIT_CONTAINS(ch), t, subtract_rent);
            }
        }
    }

    return sum;
}

void do_rent(class unit_data *ch, char *arg, const struct command_info *cmd)
{
    ubit32 sum;

    rent_info = FALSE;

    sum = subtract_recurse(ch, UNIT_CONTAINS(ch), SECS_PER_REAL_DAY, show_items);

    if (!rent_info)
    {
        send_to_char("You are charged no rent.<br/>", ch);
    }
    else
    {
        act("Your inventory costs $2t per day to rent.",
            A_ALWAYS,
            ch,
            money_string(sum, local_currency(ch), FALSE),
            cActParameter(),
            TO_CHAR);
    }
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

/* Old Pre-object header */
struct objheaderold
{
    sbit16 length; /* length of data */
    char zone[FI_MAX_ZONENAME + 1];
    char unit[FI_MAX_UNITNAME + 1];
    ubit8 level;      /* level of 'containment' (depth) */
    ubit8 equip;      /* equipment position */
    ubit8 compressed; /* compressed? */
    ubit8 type;       /* NPC or OBJ? */
};

/* Per-object header */
struct objheadernew
{
    ubit32 length;  /* length of data */
    ubit8 nVersion; // Version number
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
class file_index_type *g_slime_fi = nullptr;

/* save object */
void enlist(CByteBuffer *pBuf, class unit_data *unit, int level, int fast)
{
    ubit32 len;
    struct objheaderold ho;
    struct objheadernew hn;
    CByteBuffer TmpBuf;

    /* On 64 bit Linux at least, the struct is padded and 1 byte larger than its data */
    /* So I memset to zero to avoid valgrind reporting a memory error                 */
    memset(&ho, 0, sizeof(ho));
    memset(&hn, 0, sizeof(hn));

    if (!IS_SET(UNIT_TYPE(unit), UNIT_ST_NPC | UNIT_ST_OBJ))
    {
        slog(LOG_ALL, 0, "MAJOR ERROR - enlist a non-NPC or non-OBJ is being saved. Aborted");
        return;
    }
    if (unit->is_destructed())
    {
        slog(LOG_ALL, 0, "MAJOR ERROR - enlist a destructed unit is being saved. Aborted");
        return;
    }

    len = write_unit_string(&TmpBuf, unit);

    if (fast || !UNIT_FILE_INDEX(unit))
    {
        hn.compressed = 0;
    }
    else
    {
        assert(FALSE);
    }

    strcpy(hn.zone, UNIT_FI_ZONENAME(unit));
    strcpy(hn.unit, UNIT_FI_NAME(unit));

    hn.type = UNIT_TYPE(unit);
    hn.level = level;

    if (IS_OBJ(unit))
    {
        hn.equip = OBJ_EQP_POS(unit);
    }
    else
    {
        hn.equip = 0;
    }

    hn.nVersion = 2;
    hn.length = len;

    ho.length = 0;
    strcpy(ho.zone, "_obsoleted");
    strcpy(ho.unit, "_obsoleted");
    ho.level = 0;
    ho.equip = 0;
    ho.compressed = 0;
    ho.type = 0;

    pBuf->Append((ubit8 *)&ho, sizeof(ho)); // This is effectively a marker to read the new header
    pBuf->Append((ubit8 *)&hn, sizeof(hn));
    pBuf->Append(&TmpBuf);
}

/* Arguments:                                                    */
/*    unit   - The container to be saved (with contents)         */
/*    level  - 0 for contents only, 1 for contents & container   */
/*    fast   - TRUE for compression, FALSE for no compression.   */

void add_units(CByteBuffer *pBuf, class unit_data *parent, class unit_data *unit, int level, int fast)
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
        {
            add_units(pBuf, parent, tmp_u, level + 1, fast);
        }

        unit_to_unit(tmp_u, unit);

        if (IS_OBJ(tmp_u) && tmp_i)
        {
            OBJ_EQP_POS(tmp_u) = 0;
            equip_char(unit, tmp_u, tmp_i);
        }
    }
    else
    {
        /* UNIT CONTAINS NOTHING */
        if ((level != 0) && (IS_OBJ(unit) || IS_NPC(unit)) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
        {
            enlist(pBuf, unit, level, fast);
        }
    }
}

void send_saves(class unit_data *parent, class unit_data *unit)
{
    if (!unit)
    {
        return;
    }

    send_saves(parent, UNIT_CONTAINS(unit));
    send_saves(parent, unit->next);

    if ((IS_OBJ(unit) || IS_NPC(unit)) && !IS_SET(UNIT_FLAGS(unit), UNIT_FL_NOSAVE))
    {
        send_save_to(parent, unit);
    }
}

const char *ContentsFileName(const char *pName)
{
    static std::string Buf;

    Buf = PlayerFileName(pName) + ".inv";

    return Buf.c_str();
}

/* Save all units inside 'unit' in the blk_file 'bf' as uncompressed  */
/* if fast == 1 or compressed if fast == 0. Only OBJ's and NPC's will */
/* be saved!                                                          */
/* Container = 1 if container should be saved also                    */
void basic_save_contents(const char *pFileName, class unit_data *unit, int fast, int bContainer)
{
    class descriptor_data *tmp_descr = nullptr;
    FILE *pFile;
    char TmpName[MAX_INPUT_LENGTH + 1];

    fast = 1; /* MAJOR BUG IN DIFF CAUSING BAD PLAYERS! WITH TOO MUCH */
    /* INVENTORY, THE PLAYER INDEX WILL GET FUCKED UP!      */

    if (IS_CHAR(unit))
    {
        tmp_descr = CHAR_DESCRIPTOR(unit);
        CHAR_DESCRIPTOR(unit) = nullptr;
    }

    CByteBuffer *pBuf = &g_FileBuffer;
    pBuf->Clear();

    if (bContainer)
    {
        send_save_to(unit, unit);
    }

    send_saves(unit, UNIT_CONTAINS(unit));

    add_units(pBuf, unit, unit, bContainer ? 1 : 0, fast);

    if (IS_CHAR(unit))
    {
        CHAR_DESCRIPTOR(unit) = tmp_descr;
    }

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
int save_contents(const char *pFileName, class unit_data *unit, int fast, int bContainer)
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

    return subtract_recurse(unit, UNIT_CONTAINS(unit), SECS_PER_REAL_DAY, nullptr);
}

/* From the block_file 'bf' at index 'blk_idx' load the objects    */
/* and place them inside 'unit' by unit_to_unit and possibly equip */
/* Return the top level unit loaded                                */

class unit_data *base_load_contents(const char *pFileName, const class unit_data *unit)
{
    struct objheaderold ho;
    struct objheadernew hn;
    class file_index_type *fi;
    class unit_data *pnew, *pnew_tmp, *pstack[25];
    int len, init;
    int frame, n;
    class descriptor_data *tmp_descr = nullptr;
    int equip_ok;
    FILE *pFile;
    class unit_data *topu = nullptr;

    CByteBuffer InvBuf;
    InvBuf.Clear();

    assert(g_slime_fi != nullptr);

    pFile = fopen(pFileName, "rb");

    if (pFile == nullptr)
    {
        return nullptr;
    }

    len = fsize(pFile);
    if (len == 0)
    {
        fclose(pFile);
        return nullptr;
    }

    n = InvBuf.FileRead(pFile, len);
    fclose(pFile);

    if (n != len)
    {
        slog(LOG_ALL, 0, "Corrupted inventory: %s", pFileName);
        return nullptr;
    }

    frame = 0;
    pstack[frame] = (class unit_data *)unit;

    if (unit && IS_CHAR(unit))
    {
        tmp_descr = CHAR_DESCRIPTOR(unit);
        CHAR_DESCRIPTOR(unit) = nullptr;
    }

    for (init = TRUE; InvBuf.GetReadPosition() < InvBuf.GetLength();)
    {
        if (InvBuf.Read((ubit8 *)&ho, sizeof(ho)))
        {
            break;
        }

        // It's the new version if both are equal to "obsoleted"
        if ((strcmp(ho.zone, "_obsoleted") == 0) && (strcmp(ho.unit, "_obsoleted") == 0))
        {
            // It's a new version
            if (InvBuf.Read((ubit8 *)&hn, sizeof(hn)))
            {
                break;
            }
        }
        else
        {
            // It's the old version, copy it to the new version
            hn.nVersion = 1; // Old version
            hn.compressed = ho.compressed;
            hn.equip = ho.equip;
            hn.length = ho.length;
            hn.level = ho.level;
            hn.type = ho.type;
            strcpy(hn.unit, ho.unit);
            strcpy(hn.zone, ho.zone);
        }

        fi = find_file_index(hn.zone, hn.unit);

        pnew = nullptr;

        equip_ok = TRUE;

        if (hn.compressed)
        {
            slog(LOG_ALL, 0, "Corrupted inventory: %s", pFileName);
            if (unit && IS_CHAR(unit))
            {
                CHAR_DESCRIPTOR(unit) = tmp_descr;
                send_to_char("Your inventory was corrupt, please contact the Admin.<br/>", unit);
            }

            return topu;
        }
        else /* uncompressed */
        {
            if ((fi == nullptr) || is_slimed(fi))
            {
                slog(LOG_ALL, 0, "Sliming %s@%s for %s@%s", hn.unit, hn.zone, UNIT_FI_NAME(unit), UNIT_FI_ZONENAME(unit));
                pnew = read_unit(g_slime_fi); // Inserts unit into glist
                pnew_tmp = read_unit_string(&InvBuf, hn.type, hn.length, "preslime", FALSE);

                if (g_nCorrupt)
                {
                    slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                    break;
                }

                if (pnew->fi == nullptr)
                {
                    pnew->set_fi(g_slime_fi);
                }
            }
            else
            {
                pnew_tmp = nullptr;
                pnew = read_unit_string(&InvBuf, hn.type, hn.length, str_cc(fi->name, fi->zone->name));
                if (g_nCorrupt)
                {
                    slog(LOG_ALL, 0, "Inventory UNIT corrupt!");
                    break;
                }
                pnew->set_fi(fi);
                insert_in_unit_list(pnew);
            }

            if (pnew_tmp && pnew)
            {
                auto str = diku::format_to_str("The slimy remains of %s", TITLENAME(pnew_tmp));
                UNIT_OUT_DESCR(pnew) = str;
                UNIT_TITLE(pnew) = str;
                UNIT_NAMES(pnew).PrependName(str_cc("slime of ", UNIT_NAME(pnew_tmp)));
                delete pnew_tmp;
            }
        }

        if (init)
        {
            topu = pnew;
            init = FALSE;
        }
        if (pstack[frame] == nullptr)
        {
            if (UNIT_IN(pnew))
            {
                pstack[frame] = UNIT_IN(pnew);
            }
            else
            {
                pstack[frame] = g_void_room;
            }
        }

        UNIT_IN(pnew) = nullptr;
        if (pnew == pstack[frame])
        {
            slog(LOG_ALL, 0, "ERROR. Loading inventory, recursive linking. Please report.");
            send_to_char("Your inventory was corrupt, please contact the Admin. Recursive Linking<br/>", unit);
            return topu;
        }
        else
        {
            if (hn.level > frame)
            {
                unit_to_unit(pnew, pstack[frame]);
                frame = hn.level;
            }
            else
            {
                frame = hn.level;
                unit_to_unit(pnew, UNIT_IN(pstack[frame]));
            }

            /* IS_CHAR() needed, since a potential char may have been slimed! */
            if (hn.equip && equip_ok && IS_CHAR(UNIT_IN(pnew)))
            {
                equip_char(UNIT_IN(pnew), pnew, hn.equip);
            }

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
    g_slime_fi = find_file_index("basis", "slime");
    assert(g_slime_fi);
}

/* ************************************************************************
 * Low-level routines for handling diff/patch                              *
 ************************************************************************* */

/* Create difference-data (patch can reconstruct obj based on ref & dif) */
int diff(char *ref, ubit32 reflen, char *obj, int objlen, char *dif, int diflen, ubit32 crc)
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
    {
        if (*(ref + dstart) != *(obj + dstart))
        {
            break;
        }
    }

    /* find end of difference */
    for (dend = 0; len && rlen; dend++, len--, rlen--)
    {
        if (*(rend - dend) != *(oend - dend))
        {
            break;
        }
    }

    if ((int)(sizeof(head) + len) > diflen)
    {
        return -1;
    }
    else
    {
        diflen = sizeof(head) + len;
    }
    head.start = dstart;

    head.end = reflen - dend;
    head.reflen = reflen;
    head.crc = crc;

    memcpy(dif, (char *)&head, sizeof(head));
    dif += sizeof(head);
    if (len)
    {
        memcpy(dif, (char *)obj + dstart, len);
    }
    return (diflen);
}

/* reconstruct obj based on ref and diff */
int patch(char *ref, ubit32 reflen, char *dif, int diflen, char *res, int reslen, ubit32 crc)
{
    struct diffhead head;

    if (diflen < (int)sizeof(head))
    {
        return -1;
    }

    memcpy((char *)&head, dif, sizeof(head));
    dif += sizeof(head);
    diflen -= sizeof(head);

    if ((int)(head.start + diflen + reflen - head.end) > reslen)
    {
        return -1;
    }

    if (head.reflen != reflen)
    {
        return -1;
    }

    if (head.crc != crc)
    {
        return -1;
    }

    if (head.start)
    {
        memcpy(res, ref, head.start);
    }
    if (diflen)
    {
        memcpy(res + head.start, dif, diflen);
    }
    if (head.end < (int)reflen)
    {
        memcpy(res + head.start + diflen, ref + head.end, reflen - head.end);
    }

    return (head.start + diflen + reflen - head.end);
}

/* ========================= DIL STORE / RESTORE ======================= */

void store_all_unit(class unit_data *u, char *fname, int svcont)
{
    /*fuck  if (!UNIT_FILE_INDEX (u))
        return;*/

    basic_save_contents(fname, u, FALSE, svcont);
}

class unit_data *restore_all_unit(char *filename, unit_data *udest)
{
    class unit_data *u;
    u = base_load_contents(filename, udest);
    return u;
}
