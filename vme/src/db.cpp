/*
 $Author: All $
 $RCSfile: db.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.16 $
 */
#include "db.h"

#include "account.h"
#include "act_other.h"
#include "affect.h"
#include "ban.h"
#include "common.h"
#include "convert.h"
#include "db_file.h"
#include "dbfind.h"
#include "dil.h"
#include "dilrun.h"
#include "error.h"
#include "files.h"
#include "handler.h"
#include "interpreter.h"
#include "main_functions.h"
#include "mobact.h"
#include "money.h"
#include "path.h"
#include "pcsave.h"
#include "reception.h"
#include "sector.h"
#include "skills.h"
#include "slime.h"
#include "spell_parser.h"
#include "szonelog.h"
#include "textutil.h"
#include "unit_affected_type.h"
#include "utils.h"
#include "weather.h"
#include "zon_basis.h"
#include "zone_reset.h"
#include "zone_reset_cmd.h"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

const char *g_player_zone = "_players";

int g_room_number;                ///< For counting numbers in rooms
unit_data *g_unit_list = nullptr; ///< The global unit_list
unit_data *g_npc_head = nullptr;
unit_data *g_obj_head = nullptr;
unit_data *g_room_head = nullptr;

cSector g_sector_dat;
/**
 * Global permanent element of zone info
 */
size_t zone_info_type::g_world_nozones = 0; // number of zones in the world
zone_info_type g_zone_info = {0, nullptr};

/* By using this, we can easily sort the list if ever needed
void insert_unit_in_zone_list(zone_type *zp, class unit_data *u)
{
   class unit_data *tmp_u;

   if (IS_ROOM(u))
   {
      if (zp->rooms == NULL)
      {
         zp->rooms = u;
      }
      else
      {
         for (tmp_u = zp->rooms; tmp_u->next; tmp_u = tmp_u->next)
            ;
         tmp_u->next = u;
      }
   }
   else if (IS_OBJ(u))
   {
      zp->no_objs++;
      if (zp->objects == NULL)
      {
         zp->objects = u;
      }
      else
      {
         for (tmp_u = zp->objects; tmp_u->next; tmp_u = tmp_u->next)
            ;
         tmp_u->next = u;
      }
   }
   else if (IS_NPC(u))
   {
      zp->no_npcs++;
      if (zp->npcs == NULL)
      {
         zp->npcs = u;
      }
      else
      {
         for (tmp_u = zp->npcs; tmp_u->next; tmp_u = tmp_u->next)
            ;
         tmp_u->next = u;
      }
   }
   else
      return;
}*/

/**
 * Generate array of bin_search_type for the zone_list, and for each
 * zone's file_index's.
 */
void generate_bin_arrays()
{
    // class file_index_type *fi;
    // class zone_type *z;
    // int i;

    /* Generate array for zones
    CREATE(g_zone_info.ba, struct bin_search_type, g_zone_info.no_of_zones);
    for (i = 0, z = g_zone_info.zone_list; z; z = z->next, i++)
    {
       g_zone_info.ba[i].block = z;
       g_zone_info.ba[i].compare = z->name;
    }*/

    /* Generate array for zones
    for (i = 0, z = g_zone_info.zone_list; z; z = z->next, i++)
       g_zone_info.mmp.insert(make_pair("Barry Bonds", z));
    {
       g_zone_info.ba[i].block = z;
       g_zone_info.ba[i].compare = z->name;
    }*/

    /* Generate array for file indexes for each zone
    for (z = g_zone_info.zone_list; z; z = z->next)
    {
       if (z->no_of_fi)
       {
          CREATE(z->ba, struct bin_search_type, z->no_of_fi);
          for (fi = z->fi, i = 0; fi; fi = fi->next, i++)
          {
             z->ba[i].block = fi;
             z->ba[i].compare = fi->name;
          }
       }

       if (z->no_tmpl)
       {
          struct diltemplate *tmpl;

          CREATE(z->tmplba, struct bin_search_type, z->no_tmpl);
          for (tmpl = z->tmpl, i = 0; tmpl; tmpl = tmpl->next, i++)
          {
             z->tmplba[i].block = tmpl;
             z->tmplba[i].compare = tmpl->prgname;
          }
       }
    }*/
}

/**
 * Resolves DIL templates loaded boottime
 */
void resolve_templates()
{
    int i = 0;
    int j = 0;
    int valid = 0;

    /* all zones */
    for (auto z = g_zone_info.mmp.begin(); z != g_zone_info.mmp.end(); z++)
    {
        /* all templates in zone */
        for (auto tmpl = z->second->cgetDILTemplate().begin(); tmpl != z->second->cgetDILTemplate().end(); tmpl++)
        {
            /* all external references */
            for (i = 0; i < tmpl->second->xrefcount; i++)
            {
                tmpl->second->extprg[i] = find_dil_template(tmpl->second->xrefs[i].name);
                valid = 1;

                if (tmpl->second->extprg[i])
                {
                    /* check argument count and types */
                    if ((tmpl->second->xrefs[i].rtnt != tmpl->second->extprg[i]->rtnt) ||
                        (tmpl->second->xrefs[i].argc != tmpl->second->extprg[i]->argc))
                    {
                        valid = 0;
                    }
                    for (j = 0; j < tmpl->second->xrefs[i].argc; j++)
                    {
                        if (tmpl->second->xrefs[i].argt[j] != tmpl->second->extprg[i]->argt[j])
                        {
                            valid = 0;
                        }
                    }
                }
                else
                {
                    /* ERROR MESSAGE HERE */
                    szonelog(z->second, "Cannot resolve external reference '%s'", tmpl->second->xrefs[i].name);
                }
                /* Typecheck error ! */
                if (!valid)
                {
                    tmpl->second->extprg[i] = nullptr;
                    /* ERROR MESSAGE HERE */
                    szonelog(z->second, "Error typechecking reference to '%s'", tmpl->second->xrefs[i].name);
                }
            }
        }
    }
}

/**
 * Generate and read DIL templates
 */
diltemplate *generate_templates(FILE *f, zone_type *zone)
{
    diltemplate *tmpllist = nullptr;
    diltemplate *tmpl = nullptr;
    CByteBuffer Buf;
    ubit32 tmplsize = 0;
    char nBuf[256];
    char zBuf[256];

    tmpllist = nullptr;

    /*
     * The global templates are preceded with their length
     * written by write_template() in db_file.c
     */

    if (fread(&(tmplsize), sizeof(ubit32), 1, f) != 1)
    {
        error(HERE, "Failed to fread() tmplsize");
    }

    while (tmplsize && !feof(f))
    {
        Buf.FileRead(f, tmplsize);

        tmpl = bread_diltemplate(&Buf, UNIT_VERSION);

        if (tmpl)
        {
            tmpl->zone = zone;

            split_fi_ref(tmpl->prgname, zBuf, nBuf);

            FREE(tmpl->prgname);

            tmpl->prgname = str_dup(nBuf);
            str_lower(tmpl->prgname);
            zone->getDILTemplate().insert(std::make_pair(tmpl->prgname, tmpl));

            /* Link into list of indexes */

            zone->incrementNumOfDILTemplates();
        }
        /* next size */
        if (fread(&(tmplsize), sizeof(ubit32), 1, f) != 1)
        {
            error(HERE, "Failed to fread() tmplsize");
        }
    }

    return tmpllist;
}

/**
 * Generate index's for each unit in the file 'f', zone 'zone'
 */
void generate_file_indexes(FILE *f, zone_type *zone)
{
    file_index_type *fi = nullptr;
    file_index_type *temp_index = nullptr;
    static int object_num = 0;
    static int npc_num = 0;

    static int room_num = 0;

    CByteBuffer cBuf(100);

    g_room_number = 0;

    for (;;)
    {
        fstrcpy(&cBuf, f);

        if (feof(f))
        {
            break;
        }

        temp_index = new file_index_type();
        temp_index->setName(reinterpret_cast<const char *>(cBuf.GetData()), true);

        temp_index->setZone(zone);
        // temp_index->unit = NULL;
        temp_index->setCRC(0);

        ubit8 temp_8{};
        if (fread(&temp_8, sizeof(ubit8), 1, f) != 1)
        {
            error(HERE, "Failed to fread() temp_index->type");
        }
        temp_index->setType(temp_8);

        ubit32 temp_32{};
        if (fread(&temp_32, sizeof(ubit32), 1, f) != 1)
        {
            error(HERE, "Failed to fread() temp_index->length");
        }
        temp_index->setLength(temp_32);

        temp_32 = 0;
        if (fread(&temp_32, sizeof(ubit32), 1, f) != 1)
        {
            error(HERE, "Failed to fread() temp_index->crc");
        }
        temp_index->setCRC(temp_32);

        zone->getFileIndexMap().insert(std::make_pair(temp_index->getName(), temp_index));
        fi = temp_index;
        zone->incrementNumOfFileIndexes();
        fi->setFilepos(ftell(f));
        if (fi->getType() == UNIT_ST_OBJ)
        {
            zone->incrementNumOfObjects();
            object_num++;
        }

        if (fi->getType() == UNIT_ST_NPC)
        {
            zone->incrementNumOfNPCs();
            npc_num++;
        }

        if (fi->getType() == UNIT_ST_ROOM)
        {
            zone->incrementNumOfRooms();
        }

        if (fi->getType() == UNIT_ST_ROOM)
        {
            fi->setRoomNum(g_room_number++);
            room_num++;
        }
        else
        {
            fi->setRoomNum(0);
        }

        /* We are now positioned at first data byte */
        fi->setFilepos(ftell(f));
        fi->setNumInMemory(0);

        /* Seek forward to next index, so we are ready */
        fseek(f, fi->getFilepos() + fi->getLength(), SEEK_SET);
    }
}

/**
 * Call this routine at boot time, to index all zones
 */
void generate_zone_indexes()
{
    zone_type *z = nullptr;
    char zone[82];
    char tmpbuf[82];
    char filename[82 + 41];
    char buf[MAX_STRING_LENGTH];
    char dilfilepath[255];
    CByteBuffer cBuf(MAX_STRING_LENGTH);
    FILE *f = nullptr;
    FILE *zone_file = nullptr;
    char *c = nullptr;
    ubit8 access = 0;
    ubit8 loadlevel = 0;
    ubit8 payonly = 0;

    g_zone_info.no_of_zones = 0;

    if ((zone_file = fopen(g_cServerConfig.getFileInEtcDir(ZONE_FILE_LIST).c_str(), "r")) == nullptr)
    {
        slog(LOG_OFF, 0, "Could not open file containing filenames of zones: %s", ZONE_FILE_LIST);
        exit(0);
    }

    // Insert a virtual zone _players
    z = new (zone_type);
    g_zone_info.no_of_zones++;
    z->setZoneNumber(g_zone_info.no_of_zones - 1);
    z->setName(str_dup("_players"));
    z->setFilename(str_dup("ply"));
    z->setTitle(str_dup("Reserved zone for player file_indexes"));
    z->setHelp(str_dup(""));
    z->setNotes(str_dup("This zone is only here to allow us to use playername@_plaeyrs as with all "
                        "other indexes such as mayor@midgaard. It's not actually a zone, and it's not a represenation "
                        "of player files on disk\n"));
    g_zone_info.mmp.insert(std::make_pair(z->getName(), z));
    z = nullptr;

    for (;;)
    {
        /* Get name of next zone-file */
        if (fgets(buf, 200, zone_file) == nullptr)
        {
            break;
        }

        if (*skip_blanks(buf) == '#')
        {
            continue;
        }

        c = str_next_word_copy(buf, zone);

        if (str_is_empty(zone))
        {
            break;
        }

        snprintf(filename, sizeof(filename), "%s%s.data", g_cServerConfig.getZoneDir().c_str(), zone);

        /* Skip password */
        c = str_next_word_copy(c, tmpbuf);

        /* Skip priority level for DIL compiler */
        c = str_next_word_copy(c, tmpbuf);

        /* Read priority level */
        c = str_next_word_copy(c, tmpbuf);

        if (str_is_number(tmpbuf))
        {
            access = atoi(tmpbuf);
        }
        else
        {
            access = 255;
        }

        /* Read load level */
        c = str_next_word_copy(c, tmpbuf);

        if (str_is_number(tmpbuf))
        {
            loadlevel = MAX(200, atoi(tmpbuf));
        }
        else
        {
            loadlevel = 200;
        }

        /* Read pay only */
        c = str_next_word_copy(c, tmpbuf);

        if (str_is_number(tmpbuf))
        {
            payonly = atoi(tmpbuf);
        }
        else
        {
            payonly = FALSE;
        }

        c = str_next_word_copy(c, dilfilepath);

        if ((f = fopen_cache(filename, "rb")) == nullptr)
        {
            slog(LOG_OFF, 0, "Could not open data file: %s", filename);
            continue; /* Next file, please */
        }

        if (fsize(f) <= 3)
        {
            slog(LOG_OFF, 0, "Data file empty: %s", filename);
            continue; /* Next file, please */
        }

        slog(LOG_ALL, 0, "Indexing %s AC[%3d] LL[%d] PO[%d]", filename, access, loadlevel, payonly);

        z = new (zone_type);
        g_zone_info.no_of_zones++;

        z->setZoneNumber(g_zone_info.no_of_zones - 1);
        z->setFilename(str_dup(zone));

        if (*dilfilepath)
        {
            z->setDILFilePath(str_dup(dilfilepath));
        }
        else
        {
            z->setDILFilePath(nullptr);
        }

        fstrcpy(&cBuf, f);
        auto name = str_dup((char *)cBuf.GetData());
        str_lower(name);
        z->setName(name);

        if (find_zone(z->getName()))
        {
            slog(LOG_ALL, 0, "ZONE BOOT: Duplicate zone name [%s] not allowed", z->getName());
            exit(42);
        }

        // Insert zone into sorted list
        g_zone_info.mmp.insert(std::make_pair(z->getName(), z));

        int temp1{0};
        int mstmp = fread(&temp1, sizeof(int), 1, f);
        if (mstmp < 1)
        {
            slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in db.cpp", mstmp);
            assert(FALSE);
        }
        z->getWeather().setBase(temp1);

        z->setAccessLevel(access);
        z->setLevelRequiredToLoadItems(loadlevel);
        z->setPayOnly(payonly);

        /* More data read here */
        fstrcpy(&cBuf, f);
        z->setNotes(str_dup((char *)cBuf.GetData()));

        fstrcpy(&cBuf, f);
        z->setHelp(str_dup((char *)cBuf.GetData()));

        for (;;)
        {
            fstrcpy(&cBuf, f);

            if (cBuf.GetData()[0] == 0)
            {
                break;
            }

            z->getCreators().AppendName((char *)cBuf.GetData());
        }

        fstrcpy(&cBuf, f);

        if (cBuf.GetData()[0] != 0)
        {
            z->setTitle(str_dup((char *)cBuf.GetData()));
        }
        else
        {
            z->setTitle(str_dup(""));
        }

        /* read templates */
        z->setNumOfDILTemplates(0);
        generate_templates(f, z);

        z->setNumOfFileIndexes(0);
        z->setZoneResetCommands(nullptr);
        generate_file_indexes(f, z);
        z->setNumOfRooms(g_room_number); /* Number of rooms in the zone */

        fflush(f); /* Don't fclose(f); since we are using _cache */
    }
    fclose(zone_file);

    // _players always there, so that plus basis is the minimum
    if (g_zone_info.mmp.empty() || g_zone_info.no_of_zones <= 1)
    {
        slog(LOG_ALL, 0, "Basis zone is minimum reqired environment!");
        exit(0);
    }

    generate_bin_arrays();

    resolve_templates();
    g_mud_bootzone = 0;

    /* Allocate memory for the largest possible file-buffer */
    /* filbuffer_length = MAX(filbuffer_length + 1, 16384); */
    // CREATE(filbuffer, ubit8, filbuffer_length + 1);
    // slog(LOG_OFF, 0, "Max length for filebuffer is %d bytes.", filbuffer_length);
}

/**
 * Reason why here, and not in db_file.c: Dmc.c would then need affect.c
 *
 * Stored As:
 *   <no of affects (0..255)>
 *   `<duration>`
 *   `<id>`
 *   `<beat>`
 *   `<data[3]>`
 *   `<firstf_i>`
 *   `<tickf_i>`
 *   `<lastf_i>`
 *   `<applyf_i>`
 *
 *  Will only link the affect since it is used by both players and by
 *  other units. If the affect should also have an actual effect, then it
 *  must be followed by the function call 'apply_affects'.
 */
int bread_affect(CByteBuffer *pBuf, unit_data *u, ubit8 nVersion)
{
    unit_affected_type af;
    int i = 0;
    int corrupt = 0;

    if (nVersion <= 56)
    {
        i = pBuf->ReadU8(&corrupt);
        if (corrupt)
        {
            return 1;
        }
    }
    else
    {
        i = pBuf->ReadU16(&corrupt);
        if (corrupt)
        {
            return 1;
        }
    }

    int nError = 0;

    for (; 0 < i; i--)
    {
        af.bread(pBuf, &nError);

        if (nError)
        {
            return 1;
        }

        /* Don't call, don't apply and don't set up tick for this affect (yet) */
        af.setEventQueueElement(nullptr);
        link_alloc_affect(u, &af);
    }

    return 0;
}

zone_type *unit_error_zone = nullptr;

/**
 * After a unit has been read, this is an opportunity to do stuff on it
 */
void post_read_unit(unit_data *u)
{
    // Add regenerate to NPCs
    if (u->isNPC())
    {
        static diltemplate *regen = nullptr;

        if (regen == nullptr)
        {
            regen = find_dil_template("regenerate@update");
        }

        if (regen)
        {
            dilprg *prg = dil_copy_template(regen, u, nullptr);
            if (prg)
            {
                prg->waitcmd = WAITCMD_MAXINST - 1;
                dil_activate(prg);
            }
        }
        else
        {
            slog(LOG_ALL, 0, "SERIOUS: Couldn't find NPC regenerate@update DIL.");
        }
    }
}

/**
 * Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 *  Unit is NOT inserted in g_unit_list
 *
 * whom is an error message to be printed when something goes wrong.
 */
unit_data *read_unit_string(CByteBuffer *pBuf, int type, int len, const char *whom, int stspec)
{
    unit_data *u = nullptr;
    file_index_type *fi = nullptr;
    char zone[FI_MAX_ZONENAME + 1];
    char name[FI_MAX_UNITNAME + 1];
    int i = 0;
    int j = 0;
    ubit8 unit_version = 0;
    ubit32 nStart = 0;
    char tmpbuf[2 * MAX_STRING_LENGTH];

    g_nCorrupt = 0;

    if (type != UNIT_ST_NPC && type != UNIT_ST_PC && type != UNIT_ST_ROOM && type != UNIT_ST_OBJ)
    {
        g_nCorrupt = TRUE;
        return nullptr;
    }

    // u = new EMPLACE(unit_data) unit_data(type);
    u = new_unit_data(type);

    nStart = pBuf->GetReadPosition();
    unit_version = pBuf->ReadU8(&g_nCorrupt);

    assert(unit_version >= 37);

    if (unit_version > UNIT_VERSION)
    {
        slog(LOG_EXTENSIVE,
             0,
             "FATAL: Attempted to read '%s' but found "
             "version number %d which was NEWER than this implementation "
             "can handle! Aborting server.",
             whom,
             unit_version);
        exit(0);
    }

    g_nCorrupt += u->getNames().ReadBuffer(pBuf, unit_version);

    if (unit_version < 47 && type == UNIT_ST_PC)
    {
        char buf[256];

        strcpy(buf, u->getNames().Name());
        CAP(buf);
        u->getNames().Substitute(0, buf);
    }
    char *c = nullptr;
    if (pBuf->SkipString(&c))
    {
        g_nCorrupt++;
    }
    u->setTitle(c);
    if (unit_version < 70)
    {
        u->setTitle(fix_old_codes_to_html(u->getTitle()));
    }

    if (pBuf->SkipString(&c))
    {
        g_nCorrupt++;
    }
    u->setDescriptionOfOutside(c);
    if (unit_version < 70)
    {
        u->setDescriptionOfOutside(fix_old_codes_to_html(u->getDescriptionOfOutside()));
    }

    if (pBuf->SkipString(&c))
    {
        g_nCorrupt++;
    }
    u->setDescriptionOfInside(c);
    if (unit_version < 70)
    {
        u->setDescriptionOfInside(fix_old_codes_to_html(u->getDescriptionOfInside()));
    }

    g_nCorrupt += bread_extra(pBuf, u->getExtraList(), unit_version);

    /* Read Key Zone, Name */
    g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
    g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

    if (!str_is_empty(name))
    {
        snprintf(tmpbuf, sizeof(tmpbuf), "%s@%s", name, zone);
        u->setKey(str_dup(tmpbuf));
    }
    else
    {
        u->setKey(nullptr);
    }

    if (unit_version < 46)
    {
        u->setManipulateFlag(pBuf->ReadU16(&g_nCorrupt));
    }
    else
    {
        u->setManipulateFlag(pBuf->ReadU32(&g_nCorrupt));
    }

    u->setAllUnitFlags(pBuf->ReadU16(&g_nCorrupt));
    u->setBaseWeight(pBuf->ReadS16(&g_nCorrupt));
    u->setWeight(pBuf->ReadS16(&g_nCorrupt));
    u->setCapacity(pBuf->ReadS16(&g_nCorrupt));

    u->setMaximumHitpoints(pBuf->ReadS32(&g_nCorrupt));
    u->setCurrentHitpoints(pBuf->ReadS32(&g_nCorrupt));

    if (unit_version <= 54)
    {
        if (u->getMaximumHitpoints() <= 0)
        {
            u->setCurrentHitpoints(1000);
            u->setMaximumHitpoints(1000);
        }
    }

    u->setAlignment(pBuf->ReadS16(&g_nCorrupt));

    u->setAllOpenFlags(pBuf->ReadU8(&g_nCorrupt));
    if (unit_version >= 71)
    {
        u->setOpenDifficulty(pBuf->ReadU8(&g_nCorrupt));
    }

    u->setNumberOfActiveLightSources(pBuf->ReadS8(&g_nCorrupt));
    u->setLightOutput(pBuf->ReadS8(&g_nCorrupt));
    u->setTransparentLightOutput(pBuf->ReadS8(&g_nCorrupt));

    u->setNumberOfCharactersInsideUnit(pBuf->ReadU8(&g_nCorrupt));
    u->setLevelOfWizardInvisibility(pBuf->ReadU8(&g_nCorrupt));

    if (unit_version >= 53)
    {
        u->setSize(pBuf->ReadU16(&g_nCorrupt));
    }
    else
    {
        u->setSize(180);
    }

    if (unit_version >= 51) // Get the unit the unit is in
    {
        g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
        g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

        file_index_type *tmpfi = find_file_index(zone, name);

        if (tmpfi)
        {
            if (u->getUnitType() == UNIT_ST_ROOM)
            {
                u->setUnitIn(reinterpret_cast<unit_data *>(tmpfi)); // To be normalized!
            }
            else
            {
                if (u->isPC())
                {
                    UCHAR(u)->setLastLocation(tmpfi->Front());
                }
                else
                {
                    u->setUnitIn(tmpfi->Front());
                }
            }
        }
    }

    switch (u->getUnitType())
    {
        case UNIT_ST_NPC:
            g_nCorrupt += pBuf->ReadStringAlloc(UCHAR(u)->getMoneyPtr());
            [[fallthrough]];

        case UNIT_ST_PC:
        {
            UCHAR(u)->readFrom(*pBuf, unit_version, u, g_nCorrupt);

            if (u->isPC())
            {
                if (unit_version >= 72)
                {
                    UPC(u)->setProfession(pBuf->ReadS8(&g_nCorrupt));
                }

                PC_ACCOUNT(u).readFrom(*pBuf, unit_version, g_nCorrupt);

                if (unit_version >= 48)
                {
                    UPC(u)->setLifespan(pBuf->ReadU16(&g_nCorrupt));
                }
                else
                {
                    /// @todo worse than spooky - why decrement a race?? Also could be really bad if race==0
                    UCHAR(u)->setRace(UCHAR(u)->getRace() - 1);

                    base_race_info_type *sex_race = nullptr;

                    if (u->getSex() == SEX_MALE)
                    {
                        sex_race = &g_race_info[CHAR_RACE(u)].male;
                    }
                    else
                    {
                        sex_race = &g_race_info[CHAR_RACE(u)].female;
                    }

                    UPC(u)->setLifespan(sex_race->lifespan + dice(sex_race->lifespan_dice.reps, sex_race->lifespan_dice.size));
                }

                if (unit_version < 50)
                {
                    g_nCorrupt += pBuf->SkipString();
                }

                UPC(u)->getTerminalSetupType().echo = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().redraw = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().width = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().height = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().emulation = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().telnet = pBuf->ReadU8(&g_nCorrupt);
                UPC(u)->getTerminalSetupType().telnet = TRUE; // 2020 we shouldn't allow this to change (BBS support)
                UPC(u)->getTerminalSetupType().colour_convert = pBuf->ReadU8(&g_nCorrupt);

                if (unit_version > 59)
                {
                    g_nCorrupt += pBuf->ReadStringCopy(tmpbuf, MAX_STRING_LENGTH * 2);
                    UPC(u)->getColor().create(tmpbuf);
                }
                else if (unit_version > 57)
                {
                    if (unit_version > 58)
                    {
                        for (i = 0; i < 44; i++)
                        {
                            tmpbuf[0] = '\0';
                            g_nCorrupt += pBuf->ReadStringCopy(tmpbuf, 8);
                        }
                    }
                    else
                    {
                        for (i = 0; i < 35; i++)
                        {
                            tmpbuf[0] = '\0';
                            g_nCorrupt += pBuf->ReadStringCopy(tmpbuf, 8);
                        }
                    }
                }

                if (unit_version > 57)
                {
                    tmpbuf[0] = '\0';
                    g_nCorrupt += pBuf->ReadStringCopy(tmpbuf, MAX_STRING_LENGTH);
                    if (str_is_empty(tmpbuf))
                    {
                        UPC(u)->setPromptString(nullptr);
                    }
                    else
                    {
                        UPC(u)->setPromptString(str_dup(tmpbuf));
                    }
                }

                if (unit_version < 51)
                {
                    g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
                    g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

                    file_index_type *fi = nullptr;

                    if ((fi = find_file_index(zone, name)))
                    {
                        UCHAR(u)->setLastLocation(fi->Front());
                    }
                }

                if (unit_version >= 42)
                {
                    g_nCorrupt += UPC(u)->readFilenameFrom(*pBuf);
                }
                else
                {
                    char temp[pc_data::PC_MAX_NAME];
                    memcpy(temp, u->getNames().Name(), pc_data::PC_MAX_NAME);
                    temp[pc_data::PC_MAX_NAME - 1] = 0;
                    str_lower(temp);
                    UPC(u)->setFilename(temp);
                }

                g_nCorrupt += UPC(u)->readPasswordFrom(*pBuf);

                if (unit_version <= 72)
                {
                    UPC(u)->truncatePassword(); // This will allow me to later extend the password length
                }

                if (unit_version >= 54)
                {
                    for (i = 0; i < 5; i++)
                    {
                        UPC(u)->setLastHostAtIndexTo(i, pBuf->ReadU32(&g_nCorrupt));
                    }
                }
                else
                {
                    for (i = 0; i < 5; i++)
                    {
                        UPC(u)->setLastHostAtIndexTo(i, 0);
                    }
                }

                UPC(u)->setPlayerUID(pBuf->ReadS32(&g_nCorrupt));

                if (unit_version >= 40)
                {
                    UPC(u)->setNumberOfCrackAttempts(pBuf->ReadU16(&g_nCorrupt));
                }

                g_nCorrupt += pBuf->ReadStringAlloc(UPC(u)->getHometownPtr());
                g_nCorrupt += pBuf->ReadStringAlloc(UPC(u)->getGuildPtr());

                g_nCorrupt += pBuf->Skip32(); // skip value, was: PC_GUILD_TIME(u) obsolete

                if (unit_version >= 38)
                {
                    UPC(u)->setVirtualPlayerLevel(pBuf->ReadU16(&g_nCorrupt));
                }
                else
                {
                    UPC(u)->setVirtualPlayerLevel(CHAR_LEVEL(u));
                }

                if (unit_version <= 72)
                {
                    // Catch up XP to new hockey stick
                    int xpl = required_xp(PC_VIRTUAL_LEVEL(u));
                    int xpfloor = xpl + (required_xp(PC_VIRTUAL_LEVEL(u) + 1) - xpl) / 2;
                    if (CHAR_EXP(u) < xpfloor)
                    {
                        slog(LOG_ALL, 0, "ADJUST: Player %s XP increased from %d to %d", u->getNames().Name(), CHAR_EXP(u), xpfloor);
                        UCHAR(u)->setPlayerExperience(xpfloor);
                        // xxx
                    }
                }

                PC_TIME(u).readFrom(*pBuf, g_nCorrupt);

                if (unit_version < 44)
                {
                    race_adjust(u);
                }

                g_nCorrupt += pBuf->ReadStringAlloc(UPC(u)->getBankPtr());

                UPC(u)->setSkillPoints(pBuf->ReadS32(&g_nCorrupt));
                UPC(u)->setAbilityPoints(pBuf->ReadS32(&g_nCorrupt));

                UPC(u)->setAllPCFlags(pBuf->ReadU16(&g_nCorrupt));

                if (unit_version < 61)
                {
                    j = pBuf->ReadU8(&g_nCorrupt);
                }
                else
                {
                    j = pBuf->ReadU16(&g_nCorrupt);
                }

                for (i = 0; i < j; i++)
                {
                    if (unit_version < 69)
                    {
                        UPC(u)->setSpellSKillAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    }
                    else
                    {
                        UPC(u)->setSpellSKillAtIndexTo(i, pBuf->ReadS16(&g_nCorrupt));
                    }
                    UPC(u)->setSpellLevelAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));

                    if (unit_version < 72)
                    {
                        g_nCorrupt += pBuf->Skip8();
                    }

                    if (unit_version < 46)
                    {
                        if ((i < SPL_GROUP_MAX) && (PC_SPL_SKILL(u, i) == 0))
                        {
                            UPC(u)->setSpellSKillAtIndexTo(i, 1);
                        }
                    }
                }

                if (unit_version < 61)
                {
                    j = pBuf->ReadU8(&g_nCorrupt);
                }
                else
                {
                    j = pBuf->ReadU16(&g_nCorrupt);
                }

                for (i = 0; i < j; i++)
                {
                    if (unit_version < 69)
                    {
                        UPC(u)->setSkillAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    }
                    else
                    {
                        UPC(u)->setSkillAtIndexTo(i, pBuf->ReadS16(&g_nCorrupt));
                    }

                    UPC(u)->setSkillLevelAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    if (unit_version < 72)
                    {
                        g_nCorrupt += pBuf->Skip8();
                    }
                }

                j = pBuf->ReadU8(&g_nCorrupt);

                for (i = 0; i < j; i++)
                {
                    if (unit_version < 69)
                    {
                        UPC(u)->setWeaponSkillAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    }
                    else
                    {
                        UPC(u)->setWeaponSkillAtIndexTo(i, pBuf->ReadS16(&g_nCorrupt));
                    }

                    UPC(u)->setWeaponSkillLevelAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    if (unit_version < 72)
                    {
                        g_nCorrupt += pBuf->Skip8();
                    }
                    if (unit_version < 46)
                    {
                        if ((i < WPN_GROUP_MAX) && (PC_WPN_SKILL(u, i) == 0))
                        {
                            UPC(u)->setWeaponSkillAtIndexTo(i, 1);
                        }
                    }
                }

                if (unit_version < 47)
                {
                    UPC(u)->setWeaponSkillAtIndexTo(WPN_KICK, PC_SKI_SKILL(u, SKI_KICK));
                    UPC(u)->setSkillAtIndexTo(SKI_KICK, 0);
                }

                UPC(u)->setNumberOfCrimesCommitted(pBuf->ReadU16(&g_nCorrupt));

                j = pBuf->ReadU8(&g_nCorrupt);

                for (i = 0; i < j; i++)
                {
                    UPC(u)->setConditionAtIndexTo(i, pBuf->ReadS8(&g_nCorrupt));
                }

                if (unit_version >= 56)
                {
                    UPC(u)->setAccessLevel(pBuf->ReadU8(&g_nCorrupt));
                }
                else
                {
                    UPC(u)->setAccessLevel(0);
                }

                g_nCorrupt += bread_extra(pBuf, PC_QUEST(u), unit_version);

                if (unit_version >= 50)
                {
                    g_nCorrupt += bread_extra(pBuf, PC_INFO(u), unit_version);
                }
            }
            else
            {
                for (i = 0; i < WPN_GROUP_MAX; i++)
                {
                    if (unit_version < 69)
                    {
                        UNPC(u)->setWeaponSkillAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    }
                    else
                    {
                        UNPC(u)->setWeaponSkillAtIndexTo(i, pBuf->ReadS16(&g_nCorrupt));
                    }
                }

                for (i = 0; i < SPL_GROUP_MAX; i++)
                {
                    if (unit_version < 69)
                    {
                        UNPC(u)->setSpellSkillAtIndexTo(i, pBuf->ReadU8(&g_nCorrupt));
                    }
                    else
                    {
                        UNPC(u)->setSpellSkillAtIndexTo(i, pBuf->ReadS16(&g_nCorrupt));
                    }
                }

                UNPC(u)->setDefaultPosition(pBuf->ReadU8(&g_nCorrupt));
                UNPC(u)->setAllNPCFlags(pBuf->ReadU8(&g_nCorrupt));
            }
        }
        break;

        case UNIT_ST_OBJ:
            UOBJ(u)->setValueAtIndexTo(0, pBuf->ReadS32(&g_nCorrupt));
            UOBJ(u)->setValueAtIndexTo(1, pBuf->ReadS32(&g_nCorrupt));
            UOBJ(u)->setValueAtIndexTo(2, pBuf->ReadS32(&g_nCorrupt));
            UOBJ(u)->setValueAtIndexTo(3, pBuf->ReadS32(&g_nCorrupt));
            UOBJ(u)->setValueAtIndexTo(4, pBuf->ReadS32(&g_nCorrupt));

            UOBJ(u)->setAllObjectFlags(pBuf->ReadU32(&g_nCorrupt));
            UOBJ(u)->setPriceInGP(pBuf->ReadU32(&g_nCorrupt));
            UOBJ(u)->setPricePerDay(pBuf->ReadU32(&g_nCorrupt));

            UOBJ(u)->setObjectItemType(pBuf->ReadU8(&g_nCorrupt));
            UOBJ(u)->setEquipmentPosition(0);

            UOBJ(u)->setMagicResistance(pBuf->ReadU8(&g_nCorrupt));
            if (unit_version < 49)
            {
                if (OBJ_TYPE(u) == ITEM_WEAPON && (OBJ_VALUE(u, 3) == 0))
                {
                    UOBJ(u)->setValueAtIndexTo(3, RACE_DO_NOT_USE);
                }
            }
            break;

        case UNIT_ST_ROOM:
            if (unit_version < 51)
            {
                /* See if room is to be placed inside another room! */
                g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
                g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
                if ((fi = find_file_index(zone, name)))
                {
                    u->setUnitIn(reinterpret_cast<unit_data *>(fi)); // A file index
                }
                else
                {
                    u->setUnitIn(nullptr);
                }
            }

            /* Read N, S, E, W, U and D directions */
            for (i = 0; i <= MAX_EXIT; i++)
            {
                UROOM(u)->setRoomDirectionDataForExitTo(i, nullptr);
                g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
                g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
                str_lower(zone);
                str_lower(name);

                if (*zone && *name)
                {
                    if ((fi = find_file_index(zone, name)))
                    {
                        UROOM(u)->setRoomDirectionDataForExitTo(i, new (room_direction_data));
                        g_nCorrupt += ROOM_EXIT(u, i)->getOpenName().ReadBuffer(pBuf, unit_version);

                        ROOM_EXIT(u, i)->setDoorFlags(pBuf->ReadU16(&g_nCorrupt));
                        if (unit_version >= 71)
                        {
                            ROOM_EXIT(u, i)->setSkillDifficulty(pBuf->ReadU8(&g_nCorrupt)); // V71
                        }

                        g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
                        g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
                        str_lower(zone);
                        str_lower(name);

                        if (!str_is_empty(name))
                        {
                            snprintf(tmpbuf, sizeof(tmpbuf), "%s@%s", name, zone);
                            ROOM_EXIT(u, i)->setKey(str_dup(tmpbuf));
                        }
                        else
                        {
                            ROOM_EXIT(u, i)->setKey(nullptr);
                        }

                        /* NOT fi->unit! Done later */
                        ROOM_EXIT(u, i)->setToRoom((unit_data *)fi);
                    }
                    else
                    { /* Exit not existing, skip the junk info! */

                        szonelog(unit_error_zone, "Unknown room direction '%s@%s'", name, zone);
                        g_nCorrupt += pBuf->SkipNames();
                        g_nCorrupt += pBuf->Skip16();
                        if (unit_version >= 71)
                        {
                            g_nCorrupt += pBuf->Skip8();
                        }
                        g_nCorrupt += pBuf->SkipString();
                        g_nCorrupt += pBuf->SkipString();
                    }
                }
            }

            UROOM(u)->setAllRoomFlags(pBuf->ReadU16(&g_nCorrupt));
            UROOM(u)->setLandscapeTerrain(pBuf->ReadU8(&g_nCorrupt));
            UROOM(u)->setRoomMagicalResistance(pBuf->ReadU8(&g_nCorrupt));
            if (unit_version >= 70)
            {
                UROOM(u)->setMapXCoordinate(pBuf->ReadS16(&g_nCorrupt));
                UROOM(u)->setMapYCoordinate(pBuf->ReadS16(&g_nCorrupt));
            }
            break;

        default:
            assert(FALSE);
            break;
    }

    g_nCorrupt += bread_affect(pBuf, u, unit_version);

    u->setFunctionPointer(bread_func(pBuf, unit_version, u, stspec));

    if (len != (int)(pBuf->GetReadPosition() - nStart))
    {
        slog(LOG_ALL, 0, "FATAL: Length read mismatch (%d / %d)", len, pBuf->GetReadPosition());
        g_nCorrupt++;
    }

    if (g_nCorrupt == 0)
    {
        if (u->isChar() && CHAR_MONEY(u) && stspec)
        {
            long int val1 = 0;
            long int val2 = 0;
            char *c = nullptr;
            char *prev = CHAR_MONEY(u);

            while ((c = strchr(prev, '~')))
            {
                sscanf(prev, "%ld %ld", &val1, &val2);
                if (val1 > 0 && is_in(val2, DEF_CURRENCY, MAX_MONEY))
                {
                    coins_to_unit(u, val1, val2);
                }

                prev = c + 1;
            }

            UCHAR(u)->freeMoney();
        }

        /* We dare not start if unit is corrupt! */
        if (stspec)
        {
            start_all_special(u);
            start_affect(u);
        }
    }
    else
    {
        slog(LOG_ALL, 0, "FATAL: UNIT CORRUPT: %s", u->getNames().Name());

        if ((type != UNIT_ST_PC) && (type != UNIT_ST_ROOM) && g_slime_fi)
        {
            return read_unit(g_slime_fi); /* Slime it! */
        }
        else
        {
            return nullptr;
        }
    }

    post_read_unit(u);

    return u;
}

/**
 * Room directions points to file_indexes instead of units
 * after a room has been read, due to initialization considerations
 */
void read_unit_file(file_index_type *org_fi, CByteBuffer *pBuf)
{
    FILE *f = nullptr;
    char buf[256];

    snprintf(buf, sizeof(buf), "%s%s.data", g_cServerConfig.getZoneDir().c_str(), org_fi->getZone()->getFilename());

    if ((f = fopen_cache(buf, "rb")) == nullptr)
    {
        error(HERE, "Couldn't open %s for reading.", buf);
    }

    pBuf->FileRead(f, org_fi->getFilepos(), org_fi->getLength());

    // bread_block(f, org_fi->filepos, org_fi->length, buffer);

    /* was fclose(f) */
}

void bonus_setup(unit_data *u)
{
    if (u->isObj())
    {
        if ((OBJ_TYPE(u) == ITEM_WEAPON) || (OBJ_TYPE(u) == ITEM_SHIELD) || (OBJ_TYPE(u) == ITEM_ARMOR))
        {
            UOBJ(u)->setValueAtIndexTo(1, bonus_map_a(OBJ_VALUE(u, 1)));
            UOBJ(u)->setValueAtIndexTo(2, bonus_map_a(OBJ_VALUE(u, 2)));
        }

        for (unit_affected_type *af = u->getUnitAffected(); af; af = af->getNext())
        {
            if ((af->getID() == ID_TRANSFER_STR) || (af->getID() == ID_TRANSFER_DEX) || (af->getID() == ID_TRANSFER_CON) ||
                (af->getID() == ID_TRANSFER_CHA) || (af->getID() == ID_TRANSFER_BRA) || (af->getID() == ID_TRANSFER_MAG) ||
                (af->getID() == ID_TRANSFER_DIV) || (af->getID() == ID_TRANSFER_HPP))
            {
                af->setDataAtIndex(1, bonus_map_b(af->getDataAtIndex(1)));
            }
            else if ((af->getID() == ID_SKILL_TRANSFER) || (af->getID() == ID_SPELL_TRANSFER) || (af->getID() == ID_WEAPON_TRANSFER))
            {
                af->setDataAtIndex(1, bonus_map_b(af->getDataAtIndex(1)));
            }
        }
    }
}

/**
 * Room directions points to file_indexes instead of units
 * after a room has been read, due to initialization considerations
 */
unit_data *read_unit(file_index_type *org_fi, int ins_list)
{
    unit_data *u = nullptr;

    if (org_fi == nullptr)
    {
        return nullptr;
    }

    if (is_slimed(org_fi))
    {
        org_fi = g_slime_fi;
    }

    read_unit_file(org_fi, &g_FileBuffer);

    unit_error_zone = org_fi->getZone();

    u = read_unit_string(&g_FileBuffer,
                         org_fi->getType(),
                         org_fi->getLength(),
                         str_cc(org_fi->getName(), org_fi->getZone()->getName()),
                         ins_list);
    u->set_fi(org_fi);

    bonus_setup(u);

    if (!u->isRoom())
    {
        assert(u->getUnitIn() == nullptr);
    }

    unit_error_zone = nullptr;

    // if (IS_ROOM(u))
    //   org_fi->unit = u;

    if (ins_list)
    {
        insert_in_unit_list(u); /* Put unit into the g_unit_list      */
        apply_affect(u);        /* Set all affects that modify      */
    }
    else
    {
        if (!u->isRoom())
        {
            slog(LOG_ALL, 0, "Bizarro. This probably shouldn't happen");
        }
    }

    return u;
}

void read_all_rooms()
{
    // MS2020 int room_num = 0;

    for (auto z = g_zone_info.mmp.begin(); z != g_zone_info.mmp.end(); z++)
    {
        g_boot_zone = z->second;

        for (auto fi = z->second->cgetFileIndexMap().begin(); fi != z->second->cgetFileIndexMap().end(); fi++)
        {
            if (fi->second->getType() == UNIT_ST_ROOM)
            {
                read_unit(fi->second);
            }
        }
    }

    g_boot_zone = nullptr;
}

/**
 * After boot time, normalize all room exits
 */
void normalize_world()
{
    file_index_type *fi = nullptr;
    unit_data *u = nullptr;
    unit_data *tmpu = nullptr;
    int i = 0;

    for (u = g_unit_list; u; u = u->getGlobalNext())
    {
        if (u->isRoom())
        {
            /* Place room inside another room? */
            if (u->getUnitIn())
            {
                fi = (file_index_type *)u->getUnitIn();

                assert(!fi->Empty());

                u->setUnitIn(fi->Front());
            }

            /* Change directions into unit_data points from file_index_type */
            for (i = 0; i <= MAX_EXIT; i++)
            {
                if (ROOM_EXIT(u, i))
                {
                    if (((file_index_type *)ROOM_EXIT(u, i)->getToRoom())->Empty())
                    {
                        ROOM_EXIT(u, i)->setToRoom(nullptr);
                    }
                    else
                    {
                        ROOM_EXIT(u, i)->setToRoom(((file_index_type *)ROOM_EXIT(u, i)->getToRoom())->Front());
                    }
                }
            }
        }
    }

    for (u = g_unit_list; u; u = u->getGlobalNext())
    {
        if (u->isRoom() && u->getUnitIn())
        {
            tmpu = u->getUnitIn();
            u->setUnitIn(nullptr);

            if (unit_recursive(u, tmpu))
            {
                slog(LOG_ALL, 0, "Error: %s was recursively in %s!", u->getFileIndexSymName(), tmpu->getFileIndexSymName());
            }
            else
            {
                unit_to_unit(u, tmpu);
            }
        }
    }
}

#define ZON_DIR_CONT 0
#define ZON_DIR_NEST 1
#define ZON_DIR_UNNEST 2

/** For local error purposes */
static zone_type *read_zone_error = nullptr;

zone_reset_cmd *read_zone(FILE *f, zone_reset_cmd *cmd_list)
{
    zone_reset_cmd *cmd = nullptr;
    zone_reset_cmd *tmp_cmd = nullptr;
    file_index_type *fi = nullptr;
    ubit8 cmdno = 0;
    ubit8 direction = 0;
    char zonename[FI_MAX_ZONENAME + 1];
    char name[FI_MAX_UNITNAME + 1];
    CByteBuffer cBuf(100);

    tmp_cmd = cmd_list;

    while (((cmdno = (ubit8)fgetc(f)) != 255) && !feof(f))
    {
        cmd = new zone_reset_cmd();
        cmd->setCommandNum(cmdno);

        fstrcpy(&cBuf, f);

        if (feof(f))
        {
            break;
        }

        strcpy(zonename, (char *)cBuf.GetData());

        fstrcpy(&cBuf, f);

        if (feof(f))
        {
            break;
        }

        strcpy(name, (char *)cBuf.GetData());

        if (*zonename && *name)
        {
            if ((fi = find_file_index(zonename, name)))
            {
                cmd->setFileIndexType(0, fi);
            }
            else
            {
                szonelog(read_zone_error, "Slimed: Illegal ref.: %s@%s", name, zonename);
                cmd->setFileIndexType(0, g_slime_fi);
            }
        }
        else
        {
            cmd->setFileIndexType(0, nullptr);
        }

        fstrcpy(&cBuf, f);

        if (feof(f))
        {
            break;
        }

        strcpy(zonename, (char *)cBuf.GetData());

        fstrcpy(&cBuf, f);

        if (feof(f))
        {
            break;
        }

        strcpy(name, (char *)cBuf.GetData());

        if (*zonename && *name)
        {
            if ((fi = find_file_index(zonename, name)))
            {
                cmd->setFileIndexType(1, fi);
            }
            else
            {
                szonelog(read_zone_error, "Illegal ref.: %s@%s", name, zonename);
                cmd->setFileIndexType(1, g_slime_fi);
            }
        }
        else
        {
            cmd->setFileIndexType(1, nullptr);
        }

        sbit16 temp{};
        if (fread(&temp, sizeof(temp), 1, f) != 1)
        {
            error(HERE, "Failed to fread() cmd->num[0]");
        }
        cmd->setNum(0, temp);

        if (fread(&temp, sizeof(temp), 1, f) != 1)
        {
            error(HERE, "Failed to fread() cmd->num[1]");
        }
        cmd->setNum(1, temp);

        if (fread(&temp, sizeof(temp), 1, f) != 1)
        {
            error(HERE, "Failed to fread() cmd->num[2]");
        }
        cmd->setNum(2, temp);

        ubit8 temp2{};
        if (fread(&temp2, sizeof(temp2), 1, f) != 1)
        {
            error(HERE, "Failed to fread() cmd->cmpl");
        }
        cmd->setCompleteFlag(temp2);

        /* Link into list of next command */
        if (cmd_list == nullptr)
        {
            cmd_list = cmd;
            tmp_cmd = cmd;
        }
        else
        {
            tmp_cmd->setNextPtr(cmd);
            tmp_cmd = cmd;
        }

        direction = (ubit8)fgetc(f);

        switch (direction)
        {
            case ZON_DIR_CONT:
                break;

            case ZON_DIR_NEST:
                cmd->setNestedPtr(read_zone(f, nullptr));
                break;

            case ZON_DIR_UNNEST:
                return cmd_list;
                break;

            default:
                szonelog(read_zone_error, "Serious Error: Unknown zone direction: %d", direction);
                break;
        }
    }

    return cmd_list;
}

void read_all_zones()
{
    char filename[FI_MAX_ZONENAME + 41];
    FILE *f = nullptr;

    for (auto zone = g_zone_info.mmp.begin(); zone != g_zone_info.mmp.end(); zone++)
    {
        read_zone_error = zone->second;

        if (strcmp(zone->second->getName(), "_players") == 0)
        {
            continue;
        }

        snprintf(filename, sizeof(filename), "%s%s.reset", g_cServerConfig.getZoneDir().c_str(), zone->second->getFilename());

        if ((f = fopen(filename, "rb")) == nullptr)
        {
            slog(LOG_OFF, 0, "Could not open zone file: %s", zone->second->getFilename());
            exit(10);
        }

        ubit16 temp{0};
        if (fread(&temp, sizeof(ubit16), 1, f) != 1)
        {
            error(HERE, "Failed to fread() zone->second->zone_time");
        }
        zone->second->setZoneResetTime(temp);

        ubit8 temp2{0};
        if (fread(&temp2, sizeof(ubit8), 1, f) != 1)
        {
            error(HERE, "Failed to fread() zone->second->reset_mode");
        }
        zone->second->setResetMode(temp2);

        zone->second->setZoneResetCommands(read_zone(f, nullptr));

        fclose(f);
    }
}

char *read_info_file(const char *name, char *oldstr)
{
    char tmp[20 * MAX_STRING_LENGTH];
    char buf[20 * MAX_STRING_LENGTH];

    if (oldstr)
        FREE(oldstr);

    file_to_string(name, (char *)tmp, sizeof(tmp) - 1);
    str_escape_format(tmp, buf, sizeof(buf));

    return str_dup(buf);
}

char *read_info_file(const std::string &name, char *oldstr)
{
    return read_info_file(name.c_str(), oldstr);
}

void boot_db()
{
    slog(LOG_OFF, 0, "Boot DB -- BEGIN.");
    slog(LOG_OFF, 0, "Copyright (C) 1994 - 2021 by DikuMUD & Valhalla.");

    slog(LOG_OFF, 0, "Generating indexes from list of zone-filenames.");

    generate_zone_indexes();

    slog(LOG_OFF, 0, "Generating player index.");
    player_file_index();
    /*Removed log because it really didn't explain what this did so no need to  have it in the log.
         slog (LOG_OFF, 0, "Booting reception.");*/
    reception_boot();

    slog(LOG_OFF, 0, "Booting the money-system.");
    boot_money();

    slog(LOG_OFF, 0, "Resetting the game time:");
    boot_time_and_weather();

    slog(LOG_OFF, 0, "Profession, Race, Spell, skill, weapon and ability boot.");
    boot_profession();
    boot_race();
    boot_ability();
    boot_spell();
    boot_weapon();
    boot_skill();
    boot_interpreter();

    slog(LOG_OFF, 0, "Booting the sectors.:");
    boot_sector();

    slog(LOG_OFF, 0, "Reading all rooms into memory.");

    read_all_rooms();

    slog(LOG_OFF, 0, "Normalizing file index ref. and replacing rooms.");
    normalize_world();

    if (!g_player_convert)
    {
        slog(LOG_OFF, 0, "Initilizing shortest path matrixes for world.");
        create_worldgraph();
        slog(LOG_OFF, 0, "Finished shortest path matrix initilization.");
        //			create_dijkstra ();
    }

    slog(LOG_OFF, 0, "Reading Zone Reset information.");
    read_all_zones();

    g_cAccountConfig.Boot();

    slog(LOG_OFF, 0, "Building interpreter trie.");
    assign_command_pointers();

    interpreter_dil_check();

    slog(LOG_OFF, 0, "Loading fight messages.");
    load_messages();

    slog(LOG_OFF, 0, "Calling boot-sequences of all zones.");
    basis_boot();

    slog(LOG_OFF, 0, "Booting ban-file.");
    load_ban();

    slog(LOG_OFF, 0, "Booting slime system.");
    slime_boot();

    if (g_player_convert)
    {
        cleanup_playerfile(g_player_convert);
        exit(0);
    }

    slog(LOG_OFF, 0, "Performing boot time reset.");
    reset_all_zones();

    touch_file(g_cServerConfig.getFileInLogDir(STATISTICS_FILE));
}

void db_shutdown()
{
    return;

    unit_data *tmpu = nullptr;

    slog(LOG_OFF, 0, "Destroying unit list.");

    while (!g_unit_list->isRoom())
    {
        tmpu = g_unit_list;
        extract_unit(tmpu);
        clear_destructed();
    }

    while ((tmpu = g_unit_list))
    {
        //      DeactivateDil(tmpu);
        stop_all_special(tmpu);
        stop_affect(tmpu);
        //      unit_from_unit(tmpu);
        remove_from_unit_list(tmpu);
        tmpu->setNext(nullptr);
        delete tmpu;

        clear_destructed();
    }

    slog(LOG_OFF, 0, "Destroying zone list.");

    auto nextz = g_zone_info.mmp.begin();

    for (auto z = g_zone_info.mmp.begin(); z != g_zone_info.mmp.end(); z = nextz)
    {
        nextz = z++;
        delete z->second;
    }
}
