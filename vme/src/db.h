/*
 $Author: All $
 $RCSfile: db.h,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.10 $
 */

#ifndef _MUD_DB_H
#define _MUD_DB_H

#include "structs.h"
#include "bytestring.h"
#include "dbfind.h"
#include "config.h"
#include <map>
#include <string>


#define UNIT_VERSION 73
#define MUD_NAME "Valhalla"	/* For ease of localization */

#define UNIT_DIR       "units/"	// Directory for DIL store / restore units
#define PERSIST_DIR    "persist/"	// Directory for persistant units
#define BBS_DIR        "bbs/"	// Default BBS shared directory

#define BASIS_ZONE     "basis"

/* Data files (LIB) used by the game system */

#define PROFESSION_DEFS   "professions.dat"
#define RACE_DEFS         "races.dat"
#define ABILITY_DEFS      "abilities.dat"
#define SPELL_DEFS        "spells.dat"
#define WEAPON_DEFS       "weapons.dat"
#define SKILL_DEFS        "skills.dat"
#define COMMAND_DEFS      "commands.dat"
#define SECTOR_DEFS       "sector.dat"
#define SERVER_CONFIG     "server.cfg"

#define EXECUTE_FILE      "execute.dat"
#define MESSAGE_FILE      "message.dat"
#define STATISTIC_FILE    "connectionsnew.dat"
#define COMMAND_FILE      "commands"
#define ACCOUNT_FILE      "account"
#define ACCOUNT_LOG       "account.log"
#define CREDITFILE_LOG    "ccinfo.log"
#define STATISTICS_FILE   "statis.dat"
#define SLIME_FILE        "slime"
#define CRIME_NUM_FILE    "crime_nr"
#define CRIME_ACCUSE_FILE "crime"
#define PLAYER_ID_NAME    "players.id"
#define MAIL_FILE_NAME    "mailbox.idx"
#define MAIL_BLOCK_NAME   "mailbox.blk"
#define CREDITS_FILE      "credits"	/* for the 'credits' command  */
#define NEWS_FILE         "news"	/* for the 'news' command     */
#define MOTD_FILE         "motd"	/* messages of today          */
#define LOGO_FILE         "logo"
#define WELCOME_FILE      "welcome"
#define GOODBYE_FILE      "goodbye"
#define NEWBIE_FILE       "newbie"
#define CONTRIB_FILE      "contrib"
#define HOMEOWNER_FILE    "homes"
#define IDEA_FILE         "ideas"	/* for the 'idea'-command     */
#define TYPO_FILE         "typos"	/*         'typo'             */
#define BUG_FILE          "bugs"	/*         'bug'              */
#define MESS_FILE         "messages"	/* damage message             */
#define SOCMESS_FILE      "actions"	/* messgs for social acts     */

#define WIZLIST_FILE      "wizlist"	/* for WIZLIST                */
#define COLOR_FILE        "color.dat"	/* for WIZLIST                */

#define BAD_NAMES_FILE     "badnames"	/* bad name list */
#define BAD_STRINGS_FILE     "badstrings"	/* bad name list */
#define MONEYDEF_FILE     "money"	/* For definition of money    */

#define ZONE_FILE_LIST    "zonelist"

struct zone_info_type
{
    int no_of_zones;		/* Total number of zones       */
    //class zone_type *zone_list;	Replaced by ::map below
    void **spmatrix;		/* Inter zone shortest paths   */
    std::map< const char * , zone_type *, cmp_str > mmp;
};

class unit_data *read_unit_string (CByteBuffer * pBuf, int type, int len,
                                       const char *whom, int stspec = TRUE);
void read_unit_file (class file_index_type *org_fi, CByteBuffer * pBuf);
class unit_data *read_unit (class file_index_type *fi, int ins_list = TRUE);
void free_unit (class unit_data * ch);
void free_extra_descr (class extra_descr_data *ex);
void free_extra_descr_list (class extra_descr_data *ex);

class extra_descr_data *create_extra_descr (void);
class unit_data *create_unit (ubit8 type);


/* --- The globals of db.c --- */

extern int room_number;
extern class unit_data *unit_list;
extern class unit_data *room_head;
extern class unit_data *npc_head;
extern class unit_data *obj_head;
extern struct zone_info_type zone_info;
extern const char *player_zone;

#endif /* _MUD_DB_H */
