/*
 $Author: All $
 $RCSfile: db.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.16 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "dbfind.h"
#include "structs.h"
#include "utils.h"
#include "db.h"
#include "db_file.h"
#include "dil.h"
#include "comm.h"
#include "handler.h"
#include "vmelimits.h"
#include "textutil.h"
#include "skills.h"
#include "affect.h"
#include "utility.h"
#include "money.h"
#include "files.h"
#include "common.h"
#include "account.h"
#include "weather.h"
#include "dilrun.h"
#include "sector.h"

const char *player_zone = "_players";

int room_number;                   /* For counting numbers in rooms */
class unit_data *unit_list = NULL; /* The global unit_list          */
class unit_data *npc_head = NULL;
class unit_data *obj_head = NULL;
class unit_data *room_head = NULL;

cSector sector_dat;
extern int mudboot;
/* Global permanent element of zone info */
struct zone_info_type zone_info = { 0, NULL };

extern int memory_total_alloc;

class room_direction_data *create_direction_data(void);

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

/*  Generate array of bin_search_type for the zone_list, and for each
 *  zone's file_index's.
 */
void generate_bin_arrays(void)
{
   //class file_index_type *fi;
   //class zone_type *z;
   //int i;

   /* Generate array for zones 
   CREATE(zone_info.ba, struct bin_search_type, zone_info.no_of_zones);
   for (i = 0, z = zone_info.zone_list; z; z = z->next, i++)
   {
      zone_info.ba[i].block = z;
      zone_info.ba[i].compare = z->name;
   }*/

   /* Generate array for zones 
   for (i = 0, z = zone_info.zone_list; z; z = z->next, i++)
      zone_info.mmp.insert(make_pair("Barry Bonds", z));
   {
      zone_info.ba[i].block = z;
      zone_info.ba[i].compare = z->name;
   }*/

   /* Generate array for file indexes for each zone 
   for (z = zone_info.zone_list; z; z = z->next)
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

/* Resolves DIL templates loaded boottime */
void resolve_templates(void)
{
   int i, j, valid;

   /* all zones */
   for (auto z = zone_info.mmp.begin(); z != zone_info.mmp.end(); z++)
   {
      /* all templates in zone */
      for (auto tmpl = z->second->mmp_tmpl.begin(); tmpl != z->second->mmp_tmpl.end(); tmpl++)
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
                  valid = 0;
               for (j = 0; j < tmpl->second->xrefs[i].argc; j++)
                  if (tmpl->second->xrefs[i].argt[j] != tmpl->second->extprg[i]->argt[j])
                     valid = 0;
            }
            else
            {
               /* ERROR MESSAGE HERE */
               szonelog(z->second, "Cannot resolve external reference '%s'",
                        tmpl->second->xrefs[i].name);
            }
            /* Typecheck error ! */
            if (!valid)
            {
               tmpl->second->extprg[i] = NULL;
               /* ERROR MESSAGE HERE */
               szonelog(z->second, "Error typechecking reference to '%s'",
                        tmpl->second->xrefs[i].name);
            }
         }
      }
   }
}

/* Generate and read DIL templates */
struct diltemplate *generate_templates(FILE *f, class zone_type *zone)
{
   struct diltemplate *tmpllist, *tmpl;
   CByteBuffer Buf;
   ubit32 tmplsize = 0;
   char nBuf[256], zBuf[256];

   tmpllist = NULL;

   /*
     * The global templates are preceded with their length
     * written by write_template() in db_file.c
     */

   if (fread(&(tmplsize), sizeof(ubit32), 1, f) != 1)
      error(HERE, "Failed to fread() tmplsize");

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
         zone->mmp_tmpl.insert(make_pair(tmpl->prgname, tmpl));

         /* Link into list of indexes */

         zone->no_tmpl++;
      }
      /* next size */
      if (fread(&(tmplsize), sizeof(ubit32), 1, f) != 1)
         error(HERE, "Failed to fread() tmplsize");
   }

   return tmpllist;
}

/* Generate index's for each unit in the file 'f', zone 'zone' */
void generate_file_indexes(FILE *f, class zone_type *zone)
{
   class file_index_type *fi;
   class file_index_type *temp_index;
   static int object_num = 0;
   static int npc_num = 0;

   static int room_num = 0;

   CByteBuffer cBuf(100);

   room_number = 0;

   for (;;)
   {
      fstrcpy(&cBuf, f);

      if (feof(f))
         break;

      temp_index = new file_index_type();
      temp_index->name = str_dup((char *)cBuf.GetData());
      str_lower(temp_index->name);

      temp_index->zone = zone;
      //temp_index->unit = NULL;
      temp_index->crc = 0;

      if (fread(&(temp_index->type), sizeof(ubit8), 1, f) != 1)
         error(HERE, "Failed to fread() temp_index->type");

      if (fread(&(temp_index->length), sizeof(ubit32), 1, f) != 1)
         error(HERE, "Failed to fread() temp_index->length");

      if (fread(&(temp_index->crc), sizeof(ubit32), 1, f) != 1)
         error(HERE, "Failed to fread() temp_index->crc");

      zone->mmp_fi.insert(make_pair(temp_index->name, temp_index));
      fi = temp_index;
      zone->no_of_fi++;
      fi->filepos = ftell(f);
      if (fi->type == UNIT_ST_OBJ)
      {
         zone->no_objs++;
         object_num++;
      }

      if (fi->type == UNIT_ST_NPC)
      {
         zone->no_npcs++;
         npc_num++;
      }

      if (fi->type == UNIT_ST_ROOM)
         zone->no_rooms++;

      if (fi->type == UNIT_ST_ROOM)
      {
         fi->room_no = room_number++;
         room_num++;
      }
      else
         fi->room_no = 0;

      /* We are now positioned at first data byte */
      fi->filepos = ftell(f);
      fi->no_in_mem = 0;

      /* Seek forward to next index, so we are ready */
      fseek(f, fi->filepos + fi->length, SEEK_SET);
   }
}

/* Call this routine at boot time, to index all zones */
void generate_zone_indexes(void)
{
   class zone_type *z;
   extern int mud_bootzone;
   char zone[82], tmpbuf[82], filename[82 + 41];
   char buf[MAX_STRING_LENGTH];
   char dilfilepath[255];
   CByteBuffer cBuf(MAX_STRING_LENGTH);
   FILE *f, *zone_file;
   char *c;
   ubit8 access, loadlevel, payonly;

   zone_info.no_of_zones = 0;

   if ((zone_file =
            fopen(str_cc(g_cServerConfig.m_etcdir, ZONE_FILE_LIST),
                  "r")) == NULL)
   {
      slog(LOG_OFF, 0,
           "Could not open file containing filenames of zones: %s",
           ZONE_FILE_LIST);
      exit(0);
   }

   // Insert a virtual zone _players
   z = new (class zone_type);
   zone_info.no_of_zones++;
   z->zone_no = zone_info.no_of_zones - 1;
   z->name = str_dup("_players");
   z->filename = str_dup("ply");
   z->title = str_dup("Reserved zone for player file_indexes");
   z->help = str_dup("");
   z->notes = str_dup(   "This zone is only here to allow us to use playername@_plaeyrs as with all "
   "other indexes such as mayor@midgaard. It's not actually a zone, and it's not a represenation "
   "of player files on disk\n");
   zone_info.mmp.insert(make_pair(z->name, z));
   z = NULL;

   for (;;)
   {
      /* Get name of next zone-file */
      if (fgets(buf, 200, zone_file) == NULL)
         break;

      if (*skip_blanks(buf) == '#')
         continue;

      c = str_next_word_copy(buf, zone);

      if (str_is_empty(zone))
         break;

      sprintf(filename, "%s%s.data", g_cServerConfig.m_zondir, zone);

      /* Skip password */
      c = str_next_word_copy(c, tmpbuf);

      /* Skip priority level for DIL compiler */
      c = str_next_word_copy(c, tmpbuf);

      /* Read priority level */
      c = str_next_word_copy(c, tmpbuf);

      if (str_is_number(tmpbuf))
         access = atoi(tmpbuf);
      else
         access = 255;

      /* Read load level */
      c = str_next_word_copy(c, tmpbuf);

      if (str_is_number(tmpbuf))
         loadlevel = MAX(200, atoi(tmpbuf));
      else
         loadlevel = 200;

      /* Read pay only */
      c = str_next_word_copy(c, tmpbuf);

      if (str_is_number(tmpbuf))
         payonly = atoi(tmpbuf);
      else
         payonly = FALSE;

      c = str_next_word_copy(c, dilfilepath);

      if ((f = fopen_cache(filename, "rb")) == NULL)
      {
         slog(LOG_OFF, 0, "Could not open data file: %s", filename);
         continue; /* Next file, please */
      }

      if (fsize(f) <= 3)
      {
         slog(LOG_OFF, 0, "Data file empty: %s", filename);
         continue; /* Next file, please */
      }

      slog(LOG_ALL, 0, "Indexing %s AC[%3d] LL[%d] PO[%d]",
           filename, access, loadlevel, payonly);

      z = new (class zone_type);
      zone_info.no_of_zones++;

      z->zone_no = zone_info.no_of_zones - 1;
      z->filename = str_dup(zone);

      if (*dilfilepath)
      {
         z->dilfilepath = str_dup(dilfilepath);
      }
      else
      {
         z->dilfilepath = NULL;
      }

      fstrcpy(&cBuf, f);
      z->name = str_dup((char *)cBuf.GetData());
      str_lower(z->name);

      if (find_zone(z->name))
      {
         slog(LOG_ALL, 0, "ZONE BOOT: Duplicate zone name [%s] not allowed", z->name);
         exit(42);
      }

      // Insert zone into sorted list
      zone_info.mmp.insert(make_pair(z->name, z));

      int mstmp = fread(&z->weather.base, sizeof(int), 1, f);
      if (mstmp < 1)
      {
         slog(LOG_ALL, 0, "ERROR: Unexpected end of stream %d in db.cpp", mstmp);
         assert(FALSE);
      }

      z->access = access;
      z->loadlevel = loadlevel;
      z->payonly = payonly;

      /* More data read here */
      fstrcpy(&cBuf, f);
      z->notes = str_dup((char *)cBuf.GetData());

      fstrcpy(&cBuf, f);
      z->help = str_dup((char *)cBuf.GetData());

      for (;;)
      {
         fstrcpy(&cBuf, f);

         if (cBuf.GetData()[0] == 0)
            break;

         z->creators.AppendName((char *)cBuf.GetData());
      }

      fstrcpy(&cBuf, f);

      if (cBuf.GetData()[0] != 0)
         z->title = str_dup((char *)cBuf.GetData());
      else
         z->title = str_dup("");

      /* read templates */
      z->no_tmpl = 0;
      generate_templates(f, z);

      z->no_of_fi = 0;
      z->zri = 0;
      generate_file_indexes(f, z);
      z->no_rooms = room_number; /* Number of rooms in the zone */

      fflush(f); /* Don't fclose(f); since we are using _cache */
   }
   fclose(zone_file);

   // _players always there, so that plus basis is the minimum
   if (zone_info.mmp.empty() || zone_info.no_of_zones <= 1)
   {
      slog(LOG_ALL, 0, "Basis zone is minimum reqired environment!");
      exit(0);
   }

   generate_bin_arrays();

   resolve_templates();
   mud_bootzone = 0;

   /* Allocate memory for the largest possible file-buffer */
   /* filbuffer_length = MAX(filbuffer_length + 1, 16384); */
   // CREATE(filbuffer, ubit8, filbuffer_length + 1);
   // slog(LOG_OFF, 0, "Max length for filebuffer is %d bytes.", filbuffer_length);
}

/* Reason why here, and not in db_file.c: Dmc.c would then need affect.c
 *
 * Stored As:
 *   <no of affects (0..255)>
 *   <duration>
 *   <id>
 *   <beat>
 *   <data[3]>
 *   <firstf_i>
 *   <tickf_i>
 *   <lastf_i>
 *   <applyf_i>
 *
 *  Will only link the affect since it is used by both players and by
 *  other units. If the affect should also have an actual effect, then it
 *  must be followed by the function call 'apply_affects'.
 */
int bread_affect(CByteBuffer *pBuf, class unit_data *u, ubit8 nVersion)
{
   class unit_affected_type af;
   int i;
   ubit8 t8;
   ubit16 t16;

   class unit_affected_type *link_alloc_affect(class unit_data * unit, class unit_affected_type * orgaf);

   if (nVersion <= 56)
   {
      if (pBuf->Read8(&t8))
         return 1;
      i = t8;
   }
   else
   {
      if (pBuf->Read16(&t16))
         return 1;
      i = t16;
   }

   for (; 0 < i; i--)
   {
      if (pBuf->Read16(&af.duration))
         return 1;

      if (pBuf->Read16(&af.id))
         return 1;

      if (pBuf->Read16(&af.beat))
         return 1;

      /*if (af.id > ID_TOP_IDX)
           {
           slog(LOG_ALL, 0, "Illegal affect index in bread_affect - corrupt.");
           corrupt = TRUE;
           return;
           } */

      if (pBuf->Read32(&af.data[0]))
         return 1;

      if (pBuf->Read32(&af.data[1]))
         return 1;

      if (pBuf->Read32(&af.data[2]))
         return 1;

      if (pBuf->Read16(&af.firstf_i))
         return 1;

      if (pBuf->Read16(&af.tickf_i))
         return 1;

      if (pBuf->Read16(&af.lastf_i))
         return 1;

      if (pBuf->Read16(&af.applyf_i))
         return 1;

      /* Don't call, don't apply and don't set up tick for this affect (yet) */
      af.event = NULL;
      link_alloc_affect(u, &af);
   }

   return 0;
}

class zone_type *unit_error_zone = NULL;

/* After a unit has been read, this is an opportunity to do stuff on it
 *
 */
void post_read_unit(class unit_data *u)
{
   // Add regenerate to NPCs
   if (UNIT_TYPE(u) == UNIT_ST_NPC)
   {
      static struct diltemplate *regen = NULL;

      if (regen == NULL)
         regen = find_dil_template("regenerate@update");

      if (regen)
      {
         class dilprg *prg = dil_copy_template(regen, u, NULL);
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

extern int memory_pc_alloc;
extern int memory_npc_alloc;
extern int memory_obj_alloc;
extern int memory_room_alloc;

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 *  Unit is NOT inserted in unit_list
 *
 * whom is an error message to be printed when something goes wrong.
 */
class unit_data *read_unit_string(CByteBuffer *pBuf, int type, int len,
                                  const char *whom, int stspec)
{
   class unit_data *u;
   class file_index_type *fi;
   char zone[FI_MAX_ZONENAME + 1], name[FI_MAX_UNITNAME + 1];
   int i, j;
   ubit8 unit_version;
   ubit8 t8;
   ubit16 t16;
   sbit16 ts16;
   ubit32 t32;
   ubit32 nStart;
   char tmpbuf[2 * MAX_STRING_LENGTH];

   char *fix_old_codes_to_html(const char *c);

   void start_all_special(class unit_data * u);

   g_nCorrupt = 0;

   if (type != UNIT_ST_NPC &&
       type != UNIT_ST_PC && type != UNIT_ST_ROOM && type != UNIT_ST_OBJ)
   {
      g_nCorrupt = TRUE;
      return NULL;
   }

   // u = new EMPLACE(unit_data) unit_data(type);
   u = new_unit_data(type);

   nStart = pBuf->GetReadPosition();
   g_nCorrupt += pBuf->Read8(&unit_version);

   assert(unit_version >= 37);

   if (unit_version > UNIT_VERSION)
   {
      slog(LOG_EXTENSIVE, 0, "FATAL: Attempted to read '%s' but found "
                             "version number %d which was NEWER than this implementation "
                             "can handle! Aborting server.",
           whom, unit_version);
      exit(0);
   }

   g_nCorrupt += UNIT_NAMES(u).ReadBuffer(pBuf, unit_version);

   if (unit_version < 47 && type == UNIT_ST_PC)
   {
      char buf[256];

      strcpy(buf, UNIT_NAME(u));
      CAP(buf);
      UNIT_NAMES(u).Substitute(0, buf);
   }
   char *c;
   if (pBuf->SkipString(&c))
      g_nCorrupt++;
   UNIT_TITLE(u) = c;
   if (unit_version < 70)
      UNIT_TITLE(u) = fix_old_codes_to_html(UNIT_TITLE(u).c_str());

   if (pBuf->SkipString(&c))
      g_nCorrupt++;
   UNIT_OUT_DESCR(u) = c;
   if (unit_version < 70)
      UNIT_OUT_DESCR(u) = fix_old_codes_to_html(UNIT_OUT_DESCR(u).c_str());

   if (pBuf->SkipString(&c))
      g_nCorrupt++;
   UNIT_IN_DESCR(u) = c;
   if (unit_version < 70)
      UNIT_IN_DESCR(u) = fix_old_codes_to_html(UNIT_IN_DESCR(u).c_str());

   g_nCorrupt += bread_extra(pBuf, UNIT_EXTRA(u), unit_version);

   /* Read Key Zone, Name */
   g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
   g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

   if (!str_is_empty(name))
   {
      sprintf(tmpbuf, "%s@%s", name, zone);
      UNIT_KEY(u) = str_dup(tmpbuf);
   }
   else
      UNIT_KEY(u) = NULL;

   if (unit_version < 46)
   {
      g_nCorrupt += pBuf->Read16(&t16);
      UNIT_MANIPULATE(u) = t16;
   }
   else
      g_nCorrupt += pBuf->Read32(&UNIT_MANIPULATE(u));

   g_nCorrupt += pBuf->Read16(&UNIT_FLAGS(u));
   g_nCorrupt += pBuf->Read16(&ts16);
   UNIT_BASE_WEIGHT(u) = ts16;
   g_nCorrupt += pBuf->Read16(&ts16);
   UNIT_WEIGHT(u) = ts16;
   g_nCorrupt += pBuf->Read16(&UNIT_CAPACITY(u));

   g_nCorrupt += pBuf->Read32(&UNIT_MAX_HIT(u));
   g_nCorrupt += pBuf->Read32(&UNIT_HIT(u));

   if (unit_version <= 54)
   {
      if (UNIT_MAX_HIT(u) <= 0)
         UNIT_HIT(u) = UNIT_MAX_HIT(u) = 1000;
   }

   g_nCorrupt += pBuf->Read16(&UNIT_ALIGNMENT(u));

   g_nCorrupt += pBuf->Read8(&UNIT_OPEN_FLAGS(u));
   if (unit_version >= 71)
      g_nCorrupt += pBuf->Read8(&UNIT_OPEN_DIFF(u));

   g_nCorrupt += pBuf->Read8(&t8);
   UNIT_LIGHTS(u) = t8;
   g_nCorrupt += pBuf->Read8(&t8);
   UNIT_BRIGHT(u) = t8;
   g_nCorrupt += pBuf->Read8(&t8);
   UNIT_ILLUM(u) = t8;
   g_nCorrupt += pBuf->Read8(&UNIT_CHARS(u));
   g_nCorrupt += pBuf->Read8(&UNIT_MINV(u));

   if (unit_version >= 53)
      g_nCorrupt += pBuf->Read16(&UNIT_SIZE(u));
   else
      UNIT_SIZE(u) = 180;

   if (unit_version >= 51) // Get the unit the unit is in
   {
      g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
      g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

      class file_index_type *tmpfi = find_file_index(zone, name);

      if (tmpfi)
      {
         if (UNIT_TYPE(u) == UNIT_ST_ROOM)
            UNIT_IN(u) = (class unit_data *)tmpfi; /* To be normalized! */
         else
         {
            if (IS_PC(u))
               CHAR_LAST_ROOM(u) = tmpfi->fi_unit_list.front();
            else
               UNIT_IN(u) = tmpfi->fi_unit_list.front();
         }
      }
   }

   switch (UNIT_TYPE(u))
   {
   case UNIT_ST_NPC:
      g_nCorrupt += pBuf->ReadStringAlloc(&CHAR_MONEY(u));
      /* fallthru */

   case UNIT_ST_PC:
      g_nCorrupt += pBuf->Read32(&CHAR_EXP(u));
      g_nCorrupt += pBuf->Read32(&CHAR_FLAGS(u));

      g_nCorrupt += pBuf->Read16(&CHAR_MANA(u));
      g_nCorrupt += pBuf->Read16(&CHAR_ENDURANCE(u));

      g_nCorrupt += pBuf->Read8(&CHAR_NATURAL_ARMOUR(u));

      if (unit_version >= 39)
      {
         g_nCorrupt += pBuf->Read8(&CHAR_SPEED(u));
         if (IS_PC(u))
            if (CHAR_SPEED(u) < SPEED_MIN)
               CHAR_SPEED(u) = SPEED_DEFAULT;
      }
      else
         CHAR_SPEED(u) = SPEED_DEFAULT;

      g_nCorrupt += pBuf->Read16(&t16);
      CHAR_ATTACK_TYPE(u) = t16;

      if (unit_version <= 52)
      {
         g_nCorrupt += pBuf->Read16(&UNIT_SIZE(u));
      }
      g_nCorrupt += pBuf->Read16(&CHAR_RACE(u));

      g_nCorrupt += pBuf->Read16(&CHAR_OFFENSIVE(u));
      g_nCorrupt += pBuf->Read16(&CHAR_DEFENSIVE(u));

      g_nCorrupt += pBuf->Read8(&CHAR_SEX(u));
      g_nCorrupt += pBuf->Read8(&CHAR_LEVEL(u));
      g_nCorrupt += pBuf->Read8(&CHAR_POS(u));

      g_nCorrupt += pBuf->Read8(&t8);
      j = t8;

      for (i = 0; i < j; i++)
      {
         if (unit_version < 69)
         {
            ubit8 temp;
            g_nCorrupt += pBuf->Read8(&temp);
            CHAR_ABILITY(u, i) = temp;
         }
         else
            g_nCorrupt += pBuf->Read16(&CHAR_ABILITY(u, i));

         if (IS_PC(u))
         {
            g_nCorrupt += pBuf->Read8(&PC_ABI_LVL(u, i));
            if (unit_version < 72)
               g_nCorrupt += pBuf->Read8(&t8);
         }
      }

      if (IS_PC(u))
      {
         if (unit_version >= 72)
            g_nCorrupt += pBuf->Read8(&PC_PROFESSION(u));

         g_nCorrupt += pBuf->ReadFloat(&PC_ACCOUNT(u).credit);
         g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).credit_limit);
         g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).total_credit);

         if (unit_version >= 44)
            g_nCorrupt += pBuf->Read16(&PC_ACCOUNT(u).last4);
         else
         {
            if (unit_version >= 41)
               g_nCorrupt += pBuf->Skip32(); /* cc_time */
            PC_ACCOUNT(u).last4 = -1;
         }

         if (unit_version >= 45)
         {
            g_nCorrupt += pBuf->Read8(&PC_ACCOUNT(u).discount);

            if (unit_version >= 52)
               g_nCorrupt += pBuf->Read32(&PC_ACCOUNT(u).flatrate);
            else
               PC_ACCOUNT(u).flatrate = 0;

            g_nCorrupt += pBuf->Read8(&PC_ACCOUNT(u).cracks);
         }
         else
         {
            PC_ACCOUNT(u).flatrate = 0;
            PC_ACCOUNT(u).discount = 0;
            PC_ACCOUNT(u).cracks = 0;
         }

         if (unit_version >= 48)
            g_nCorrupt += pBuf->Read16(&PC_LIFESPAN(u));
         else
         {
            CHAR_RACE(u)
            --; /* spooky */

            struct base_race_info_type *sex_race;

            if (CHAR_SEX(u) == SEX_MALE)
               sex_race = &race_info[CHAR_RACE(u)].male;
            else
               sex_race = &race_info[CHAR_RACE(u)].female;

            PC_LIFESPAN(u) =
                sex_race->lifespan + dice(sex_race->lifespan_dice.reps,
                                          sex_race->lifespan_dice.size);
         }

         if (unit_version < 50)
            g_nCorrupt += pBuf->SkipString();

         g_nCorrupt += pBuf->Read8(&PC_SETUP_ECHO(u));
         g_nCorrupt += pBuf->Read8(&PC_SETUP_REDRAW(u));
         g_nCorrupt += pBuf->Read8(&PC_SETUP_WIDTH(u));
         g_nCorrupt += pBuf->Read8(&PC_SETUP_HEIGHT(u));
         g_nCorrupt += pBuf->Read8(&PC_SETUP_EMULATION(u));
         g_nCorrupt += pBuf->Read8(&PC_SETUP_TELNET(u));
         PC_SETUP_TELNET(u) = TRUE; // 2020 we shouldn't allow this to change (BBS support)
         g_nCorrupt += pBuf->Read8(&PC_SETUP_COLOUR(u));

         if (unit_version > 59)
         {
            g_nCorrupt +=
                pBuf->ReadStringCopy(tmpbuf, MAX_STRING_LENGTH * 2);
            UPC(u)->color.create(tmpbuf);
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
               UPC(u)->promptstr = NULL;
            else
               UPC(u)->promptstr = str_dup(tmpbuf);
         }

         if (unit_version < 51)
         {
            g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
            g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));

            class file_index_type *fi;

            if ((fi = find_file_index(zone, name)))
               CHAR_LAST_ROOM(u) = fi->fi_unit_list.front();
         }

         if (unit_version >= 42)
         {
            g_nCorrupt +=
                pBuf->ReadStringCopy(PC_FILENAME(u), PC_MAX_NAME);
            PC_FILENAME(u)
            [PC_MAX_NAME - 1] = 0;
         }
         else
         {
            memcpy(PC_FILENAME(u), UNIT_NAME(u), PC_MAX_NAME);
            PC_FILENAME(u)
            [PC_MAX_NAME - 1] = 0;
            str_lower(PC_FILENAME(u));
         }

         g_nCorrupt += pBuf->ReadStringCopy(PC_PWD(u), PC_MAX_PASSWORD);
         PC_PWD(u)
         [PC_MAX_PASSWORD - 1] = '\0';
         if (unit_version <= 72)
            PC_PWD(u)
            [10] = '\0'; // This will allow me to later extend the password length

         if (unit_version >= 54)
         {
            for (i = 0; i < 5; i++)
               g_nCorrupt += pBuf->Read32(&PC_LASTHOST(u)[i]);
         }
         else
            for (i = 0; i < 5; i++)
               PC_LASTHOST(u)
               [i] = 0;

         g_nCorrupt += pBuf->Read32(&PC_ID(u));

         if (unit_version >= 40)
            g_nCorrupt += pBuf->Read16(&PC_CRACK_ATTEMPTS(u));

         g_nCorrupt += pBuf->ReadStringAlloc(&PC_HOME(u));
         g_nCorrupt += pBuf->ReadStringAlloc(&PC_GUILD(u));

         g_nCorrupt += pBuf->Read32(&PC_GUILD_TIME(u));

         if (unit_version >= 38)
            g_nCorrupt += pBuf->Read16(&PC_VIRTUAL_LEVEL(u));
         else
            PC_VIRTUAL_LEVEL(u) = CHAR_LEVEL(u);

         if (unit_version <= 72)
         {
            // Catch up XP to new hockey stick
            int xpl = required_xp(PC_VIRTUAL_LEVEL(u));
            int xpfloor = xpl + (required_xp(PC_VIRTUAL_LEVEL(u) + 1) - xpl) / 2;
            if (CHAR_EXP(u) < xpfloor)
            {
               slog(LOG_ALL, 0, "ADJUST: Player %s XP increased from %d to %d",
                    UNIT_NAME(u), CHAR_EXP(u), xpfloor);
               CHAR_EXP(u) = xpfloor;
               // xxx
            }
         }

         g_nCorrupt += pBuf->Read32(&t32);
         PC_TIME(u).creation = t32;

         g_nCorrupt += pBuf->Read32(&t32);
         PC_TIME(u).connect = t32;
         g_nCorrupt += pBuf->Read32(&t32);
         PC_TIME(u).birth = t32;
         g_nCorrupt += pBuf->Read32(&t32);
         PC_TIME(u).played = t32;

         if (unit_version < 44)
         {
            void race_adjust(class unit_data *);
            race_adjust(u);
         }

         g_nCorrupt += pBuf->ReadStringAlloc(&PC_BANK(u));

         g_nCorrupt += pBuf->Read32(&PC_SKILL_POINTS(u));
         g_nCorrupt += pBuf->Read32(&PC_ABILITY_POINTS(u));

         g_nCorrupt += pBuf->Read16(&t16);
         PC_FLAGS(u) = t16;

         if (unit_version < 61)
         {
            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;
         }
         else
         {
            g_nCorrupt += pBuf->Read16(&t16);
            j = t16;
         }

         for (i = 0; i < j; i++)
         {
            if (unit_version < 69)
            {
               ubit8 temp;
               g_nCorrupt += pBuf->Read8(&temp);
               PC_SPL_SKILL(u, i) = temp;
            }
            else
               g_nCorrupt += pBuf->Read16(&PC_SPL_SKILL(u, i));
            g_nCorrupt += pBuf->Read8(&PC_SPL_LVL(u, i));

            if (unit_version < 72)
               g_nCorrupt += pBuf->Read8(&t8);

            if (unit_version < 46)
               if ((i < SPL_GROUP_MAX) && (PC_SPL_SKILL(u, i) == 0))
                  PC_SPL_SKILL(u, i) = 1;
         }

         if (unit_version < 61)
         {
            g_nCorrupt += pBuf->Read8(&t8);
            j = t8;
         }
         else
         {
            g_nCorrupt += pBuf->Read16(&t16);
            j = t16;
         }

         for (i = 0; i < j; i++)
         {
            if (unit_version < 69)
            {
               ubit8 temp;
               g_nCorrupt += pBuf->Read8(&temp);
               PC_SKI_SKILL(u, i) = temp;
            }
            else
               g_nCorrupt += pBuf->Read16(&PC_SKI_SKILL(u, i));

            g_nCorrupt += pBuf->Read8(&PC_SKI_LVL(u, i));
            if (unit_version < 72)
               g_nCorrupt += pBuf->Read8(&t8);
         }

         g_nCorrupt += pBuf->Read8(&t8);
         j = t8;

         for (i = 0; i < j; i++)
         {
            if (unit_version < 69)
            {
               ubit8 temp;
               g_nCorrupt += pBuf->Read8(&temp);
               PC_WPN_SKILL(u, i) = temp;
            }
            else
               g_nCorrupt += pBuf->Read16(&PC_WPN_SKILL(u, i));

            g_nCorrupt += pBuf->Read8(&PC_WPN_LVL(u, i));
            if (unit_version < 72)
               g_nCorrupt += pBuf->Read8(&t8);
            if (unit_version < 46)
               if ((i < WPN_GROUP_MAX) && (PC_WPN_SKILL(u, i) == 0))
                  PC_WPN_SKILL(u, i) = 1;
         }

         if (unit_version < 47)
         {
            PC_WPN_SKILL(u, WPN_KICK) = PC_SKI_SKILL(u, SKI_KICK);
            PC_SKI_SKILL(u, SKI_KICK) = 0;
         }

         g_nCorrupt += pBuf->Read16(&PC_CRIMES(u));

         g_nCorrupt += pBuf->Read8(&t8);
         j = t8;

         for (i = 0; i < j; i++)
            g_nCorrupt += pBuf->Read8(&PC_COND(u, i));

         if (unit_version >= 56)
            g_nCorrupt += pBuf->Read8(&PC_ACCESS_LEVEL(u));
         else
            PC_ACCESS_LEVEL(u) = 0;

         g_nCorrupt += bread_extra(pBuf, PC_QUEST(u), unit_version);

         if (unit_version >= 50)
            g_nCorrupt += bread_extra(pBuf, PC_INFO(u), unit_version);
      }
      else
      {
         for (i = 0; i < WPN_GROUP_MAX; i++)
         {
            if (unit_version < 69)
            {
               ubit8 temp;
               g_nCorrupt += pBuf->Read8(&temp);
               NPC_WPN_SKILL(u, i) = temp;
            }
            else
               g_nCorrupt += pBuf->Read16(&NPC_WPN_SKILL(u, i));
         }

         for (i = 0; i < SPL_GROUP_MAX; i++)
         {
            if (unit_version < 69)
            {
               ubit8 temp;
               g_nCorrupt += pBuf->Read8(&temp);
               NPC_SPL_SKILL(u, i) = temp;
            }
            else
               g_nCorrupt += pBuf->Read16(&NPC_SPL_SKILL(u, i));
         }

         g_nCorrupt += pBuf->Read8(&NPC_DEFAULT(u));
         g_nCorrupt += pBuf->Read8(&NPC_FLAGS(u));
      }
      break;

   case UNIT_ST_OBJ:
      g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 0));
      g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 1));
      g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 2));
      g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 3));
      g_nCorrupt += pBuf->Read32(&OBJ_VALUE(u, 4));

      g_nCorrupt += pBuf->Read32(&t32);
      OBJ_FLAGS(u) = t32;
      g_nCorrupt += pBuf->Read32(&OBJ_PRICE(u));
      g_nCorrupt += pBuf->Read32(&OBJ_PRICE_DAY(u));

      g_nCorrupt += pBuf->Read8(&OBJ_TYPE(u));
      OBJ_EQP_POS(u) = 0;

      g_nCorrupt += pBuf->Read8(&OBJ_RESISTANCE(u));
      if (unit_version < 49)
      {
         if (OBJ_TYPE(u) == ITEM_WEAPON && (OBJ_VALUE(u, 3) == 0))
            OBJ_VALUE(u, 3) = RACE_DO_NOT_USE;
      }
      break;

   case UNIT_ST_ROOM:
      if (unit_version < 51)
      {
         /* See if room is to be placed inside another room! */
         g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
         g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
         if ((fi = find_file_index(zone, name)))
            UNIT_IN(u) = (class unit_data *)fi; /* A file index */
         else
            UNIT_IN(u) = NULL;
      }

      /* Read N, S, E, W, U and D directions */
      for (i = 0; i <= MAX_EXIT; i++)
      {
         ROOM_EXIT(u, i) = NULL;
         g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
         g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
         str_lower(zone);
         str_lower(name);         

         if (*zone && *name)
         {
            if ((fi = find_file_index(zone, name)))
            {
               ROOM_EXIT(u, i) = new (class room_direction_data);
               g_nCorrupt += ROOM_EXIT(u, i)->open_name.ReadBuffer(pBuf, unit_version);

               g_nCorrupt += pBuf->Read16(&t16);
               ROOM_EXIT(u, i)->exit_info = t16;
               if (unit_version >= 71)
               {
                  g_nCorrupt += pBuf->Read8(&t8);
                  ROOM_EXIT(u, i)->difficulty = t8; // V71
               }

               g_nCorrupt += pBuf->ReadStringCopy(zone, sizeof(zone));
               g_nCorrupt += pBuf->ReadStringCopy(name, sizeof(name));
               str_lower(zone);
               str_lower(name);         

               if (!str_is_empty(name))
               {
                  sprintf(tmpbuf, "%s@%s", name, zone);
                  ROOM_EXIT(u, i)->key = str_dup(tmpbuf);
               }
               else
                  ROOM_EXIT(u, i)->key = NULL;

               /* NOT fi->unit! Done later */
               ROOM_EXIT(u, i)->to_room = (class unit_data *)fi;
            }
            else
            { /* Exit not existing, skip the junk info! */

               szonelog(unit_error_zone,
                        "Unknown room direction '%s@%s'", name, zone);
               g_nCorrupt += pBuf->SkipNames();
               g_nCorrupt += pBuf->Skip16();
               if (unit_version >= 71)
                  g_nCorrupt += pBuf->Skip8();
               g_nCorrupt += pBuf->SkipString();
               g_nCorrupt += pBuf->SkipString();
            }
         }
      }

      g_nCorrupt += pBuf->Read16(&t16);
      ROOM_FLAGS(u) = t16;
      g_nCorrupt += pBuf->Read8(&ROOM_LANDSCAPE(u));
      g_nCorrupt += pBuf->Read8(&ROOM_RESISTANCE(u));
      if (unit_version >= 70)
      {
         g_nCorrupt += pBuf->Read16(&UROOM(u)->mapx);
         g_nCorrupt += pBuf->Read16(&UROOM(u)->mapy);
      }
      break;

   default:
      assert(FALSE);
      break;
   }

   g_nCorrupt += bread_affect(pBuf, u, unit_version);

   UNIT_FUNC(u) = bread_func(pBuf, unit_version, u, stspec);

   if (len != (int)(pBuf->GetReadPosition() - nStart))
   {
      slog(LOG_ALL, 0, "FATAL: Length read mismatch (%d / %d)",
           len, pBuf->GetReadPosition());
      g_nCorrupt++;
   }

   if (g_nCorrupt == 0)
   {
      if (IS_CHAR(u) && CHAR_MONEY(u) && stspec)
      {
         long int val1, val2;
         char *c, *prev = CHAR_MONEY(u);

         while ((c = strchr(prev, '~')))
         {
            sscanf(prev, "%ld %ld", &val1, &val2);
            if (val1 > 0 && is_in(val2, DEF_CURRENCY, MAX_MONEY))
               coins_to_unit(u, val1, val2);

            prev = c + 1;
         }

         FREE(CHAR_MONEY(u));
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
      extern class file_index_type *slime_fi;

      slog(LOG_ALL, 0, "FATAL: UNIT CORRUPT: %s", u->names.Name());

      if ((type != UNIT_ST_PC) && (type != UNIT_ST_ROOM) && slime_fi)
         return read_unit(slime_fi); /* Slime it! */
      else
         return NULL;
   }

   post_read_unit(u);

   return u;
}

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 */
void read_unit_file(class file_index_type *org_fi, CByteBuffer *pBuf)
{
   FILE *f;
   char buf[256];

   sprintf(buf, "%s%s.data", g_cServerConfig.m_zondir,
           org_fi->zone->filename);

   if ((f = fopen_cache(buf, "rb")) == NULL)
      error(HERE, "Couldn't open %s for reading.", buf);

   pBuf->FileRead(f, org_fi->filepos, org_fi->length);

   // bread_block(f, org_fi->filepos, org_fi->length, buffer);

   /* was fclose(f) */
}

/*  Room directions points to file_indexes instead of units
 *  after a room has been read, due to initialization considerations
 */
class unit_data *read_unit(class file_index_type *org_fi, int ins_list)
{
   int is_slimed(class file_index_type * sp);

   extern class file_index_type *slime_fi;

   class unit_data *u;

   if (org_fi == NULL)
      return NULL;

   if (org_fi == NULL)
      return NULL;

   if (is_slimed(org_fi))
      org_fi = slime_fi;

   read_unit_file(org_fi, &g_FileBuffer);

   unit_error_zone = org_fi->zone;

   u = read_unit_string(&g_FileBuffer, org_fi->type, org_fi->length,
                        str_cc(org_fi->name, org_fi->zone->name),
                        ins_list);
   u->set_fi(org_fi);

   if (!IS_ROOM(u))
      assert(UNIT_IN(u) == NULL);

   unit_error_zone = NULL;

   //if (IS_ROOM(u))
   //   org_fi->unit = u;

   if (ins_list)
   {
      insert_in_unit_list(u); /* Put unit into the unit_list      */
      apply_affect(u);        /* Set all affects that modify      */
   }
   else
   {
      if (UNIT_TYPE(u) != UNIT_ST_ROOM)
         slog(LOG_ALL, 0, "Bizarro. This probably shouldn't happen");
   }
   
   return u;
}

void read_all_rooms(void)
{
   // MS2020 int room_num = 0;

   extern class zone_type *boot_zone;

   for (auto z = zone_info.mmp.begin(); z != zone_info.mmp.end(); z++)
   {
      boot_zone = z->second;

      for (auto fi = z->second->mmp_fi.begin(); fi != z->second->mmp_fi.end(); fi++)
      {
         if (fi->second->type == UNIT_ST_ROOM)
         {
            read_unit(fi->second);
         }
      }
   }

   boot_zone = NULL;
}

/* After boot time, normalize all room exits */
void normalize_world(void)
{
   class file_index_type *fi;
   class unit_data *u, *tmpu;
   int i;

   for (u = unit_list; u; u = u->gnext)
      if (IS_ROOM(u))
      {
         /* Place room inside another room? */
         if (UNIT_IN(u))
         {
            fi = (class file_index_type *)UNIT_IN(u);

            assert(!fi->fi_unit_list.empty());

            UNIT_IN(u) = fi->fi_unit_list.front();
         }

         /* Change directions into unit_data points from file_index_type */
         for (i = 0; i <= MAX_EXIT; i++)
            if (ROOM_EXIT(u, i))
            {
               if (((class file_index_type *) ROOM_EXIT(u, i)->to_room)->fi_unit_list.empty())
                  ROOM_EXIT(u, i)->to_room = NULL;
               else
                  ROOM_EXIT(u, i)->to_room =
                     ((class file_index_type *)ROOM_EXIT(u, i)->to_room)->fi_unit_list.front();

            }
      }

   for (u = unit_list; u; u = u->gnext)
      if (IS_ROOM(u) && UNIT_IN(u))
      {
         tmpu = UNIT_IN(u);
         UNIT_IN(u) = NULL;

         if (unit_recursive(u, tmpu))
         {
            slog(LOG_ALL, 0, "Error: %s@%s was recursively in %s@%s!",
                 UNIT_FI_NAME(u), UNIT_FI_ZONENAME(u),
                 UNIT_FI_NAME(tmpu), UNIT_FI_ZONENAME(tmpu));
         }
         else
         {
            unit_to_unit(u, tmpu);
         }
      }
}

#define ZON_DIR_CONT 0
#define ZON_DIR_NEST 1
#define ZON_DIR_UNNEST 2

/* For local error purposes */
static class zone_type *read_zone_error = NULL;

struct zone_reset_cmd *read_zone(FILE *f, struct zone_reset_cmd *cmd_list)
{
   struct zone_reset_cmd *cmd, *tmp_cmd;
   class file_index_type *fi;
   ubit8 cmdno, direction;
   char zonename[FI_MAX_ZONENAME + 1], name[FI_MAX_UNITNAME + 1];
   CByteBuffer cBuf(100);

   extern class file_index_type *slime_fi;

   tmp_cmd = cmd_list;

   while (((cmdno = (ubit8)fgetc(f)) != 255) && !feof(f))
   {
      CREATE(cmd, struct zone_reset_cmd, 1);
      cmd->cmd_no = cmdno;

      fstrcpy(&cBuf, f);

      if (feof(f))
         break;

      strcpy(zonename, (char *)cBuf.GetData());

      fstrcpy(&cBuf, f);

      if (feof(f))
         break;

      strcpy(name, (char *)cBuf.GetData());

      if (*zonename && *name)
         if ((fi = find_file_index(zonename, name)))
            cmd->fi[0] = fi;
         else
         {
            szonelog(read_zone_error, "Slimed: Illegal ref.: %s@%s",
                     name, zonename);
            cmd->fi[0] = slime_fi;
         }
      else
         cmd->fi[0] = 0;

      fstrcpy(&cBuf, f);

      if (feof(f))
         break;

      strcpy(zonename, (char *)cBuf.GetData());

      fstrcpy(&cBuf, f);

      if (feof(f))
         break;

      strcpy(name, (char *)cBuf.GetData());

      if (*zonename && *name)
         if ((fi = find_file_index(zonename, name)))
            cmd->fi[1] = fi;
         else
         {
            szonelog(read_zone_error, "Illegal ref.: %s@%s", name, zonename);
            cmd->fi[1] = slime_fi;
         }
      else
         cmd->fi[1] = 0;

      if (fread(&(cmd->num[0]), sizeof(cmd->num[0]), 1, f) != 1)
         error(HERE, "Failed to fread() cmd->num[0]");
      if (fread(&(cmd->num[1]), sizeof(cmd->num[1]), 1, f) != 1)
         error(HERE, "Failed to fread() cmd->num[1]");
      if (fread(&(cmd->num[2]), sizeof(cmd->num[2]), 1, f) != 1)
         error(HERE, "Failed to fread() cmd->num[2]");
      if (fread(&(cmd->cmpl), sizeof(ubit8), 1, f) != 1)
         error(HERE, "Failed to fread() cmd->cmpl");

      /* Link into list of next command */
      if (cmd_list == NULL)
      {
         cmd_list = cmd;
         tmp_cmd = cmd;
      }
      else
      {
         tmp_cmd->next = cmd;
         tmp_cmd = cmd;
      }

      direction = (ubit8)fgetc(f);

      switch (direction)
      {
      case ZON_DIR_CONT:
         break;

      case ZON_DIR_NEST:
         cmd->nested = read_zone(f, 0);
         break;

      case ZON_DIR_UNNEST:
         return cmd_list;
         break;

      default:
         szonelog(read_zone_error,
                  "Serious Error: Unknown zone direction: %d", direction);
         break;
      }
   }

   return cmd_list;
}

void read_all_zones(void)
{
   char filename[FI_MAX_ZONENAME + 41];
   FILE *f;

   for (auto zone = zone_info.mmp.begin(); zone != zone_info.mmp.end(); zone++)
   {
      read_zone_error = zone->second;

      if (strcmp(zone->second->name, "_players") == 0)
         continue;

      sprintf(filename, "%s%s.reset", g_cServerConfig.m_zondir,
              zone->second->filename);

      if ((f = fopen(filename, "rb")) == NULL)
      {
         slog(LOG_OFF, 0, "Could not open zone file: %s", zone->second->filename);
         exit(10);
      }

      if (fread(&(zone->second->zone_time), sizeof(ubit16), 1, f) != 1)
         error(HERE, "Failed to fread() zone->second->zone_time");

      if (fread(&(zone->second->reset_mode), sizeof(ubit8), 1, f) != 1)
         error(HERE, "Failed to fread() zone->second->reset_mode");

      zone->second->zri = read_zone(f, 0);

      fclose(f);
   }
}

char *read_info_file(char *name, char *oldstr)
{
   char tmp[20 * MAX_STRING_LENGTH];
   char buf[20 * MAX_STRING_LENGTH];

   if (oldstr)
      FREE(oldstr);

   file_to_string(name, (char *)tmp, sizeof(tmp) - 1);
   str_escape_format(tmp, buf, sizeof(buf));

   return str_dup(buf);
}

extern int memory_roomread_alloc;
extern int memory_zoneidx_alloc;
extern int memory_zonereset_alloc;

void boot_db(void)
{
   void create_worldgraph(void);
   void create_dijkstra(void);
   void player_file_index(void);
   void reception_boot(void);
   void load_messages(void);
   void assign_command_pointers(void);
   void assign_spell_pointers(void);
   void reset_all_zones(void);
   void load_ban(void);
   void boot_money(void);
   void boot_sector(void);
   void basis_boot(void);
   void slime_boot(void);
   void boot_spell(void);
   void boot_skill(void);
   void boot_weapon(void);
   void boot_ability(void);
   void boot_race(void);
   void boot_profession(void);
   void boot_interpreter(void);
   void interpreter_dil_check(void);

   void cleanup_playerfile(int c);
   extern int player_convert;

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

   if (!player_convert)
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

   if (player_convert)
   {
      cleanup_playerfile(player_convert);
      exit(0);
   }

   slog(LOG_OFF, 0, "Performing boot time reset.");
   reset_all_zones();

   touch_file(str_cc(g_cServerConfig.m_logdir, STATISTICS_FILE));
}

void db_shutdown(void)
{
   return;

   class unit_data *tmpu;

   slog(LOG_OFF, 0, "Destroying unit list.");

   void clear_destructed(void);

   while (!IS_ROOM(unit_list))
   {
      tmpu = unit_list;
      extract_unit(tmpu);
      clear_destructed();
   }

   void stop_all_special(class unit_data * u);

   while ((tmpu = unit_list))
   {
      //      DeactivateDil(tmpu);
      stop_all_special(tmpu);
      stop_affect(tmpu);
      //      unit_from_unit(tmpu);
      remove_from_unit_list(tmpu);
      tmpu->next = NULL;
      delete tmpu;

      clear_destructed();
   }

   slog(LOG_OFF, 0, "Destroying zone list.");


   auto nextz = zone_info.mmp.begin();

   for (auto z = zone_info.mmp.begin(); z != zone_info.mmp.end(); z = nextz)
   {
      nextz = z++;
      delete z->second;
   }
}
