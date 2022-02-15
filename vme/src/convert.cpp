/*
 $Author: All $
 $RCSfile: convert.cpp,v $
 $Date: 2005/06/28 20:17:48 $
 $Revision: 2.7 $
 */
#include "affect.h"
#include "db.h"
#include "db_file.h"
#include "formatter.h"
#include "handler.h"
#include "nanny.h"
#include "pcsave.h"
#include "reception.h"
#include "structs.h"
#include "textutil.h"
#include "utils.h"
#include "zon_basis.h"

#include <dirent.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;

char player_directory[1024] = "\0";
int max_id = -1;
int top_id = -1;
ubit8 *ids = nullptr; /* For checking duplicate players... */

#define OUTPUT_DIR "lib/"

int sel_name(const dirent *dptr)
{
    if (strchr(dptr->d_name, '.') == nullptr)
    {
        return 1;
    }
    return 0;
}

void convert_free_unit(unit_data *u)
{
    while (UNIT_CONTAINS(u))
    {
        convert_free_unit(UNIT_CONTAINS(u));
    }

    UNIT_AFFECTED(u) = nullptr;
    UNIT_FUNC(u) = nullptr;

    unit_from_unit(u);
    remove_from_unit_list(u);

    delete u;
}

void free_inventory(unit_data *u)
{
    unit_data *tmp = nullptr;
    unit_data *nxt = nullptr;

    for (tmp = u; tmp; tmp = nxt)
    {
        nxt = tmp->next;
        convert_free_unit(tmp);
    }
}

int days_old(time_t last_logon)
{
    return (int)(difftime(time(nullptr), last_logon) / SECS_PER_REAL_DAY);
}

unit_data *convert_item(unit_data *u, unit_data *pc, int bList)
{
    if (bList)
    {
        if (strcmp(UNIT_FI_ZONE(u)->getName(), "treasure") == 0)
        {
            extra_descr_data *ed = UNIT_EXTRA(u).m_pList;
            std::cout << std::endl << UNIT_FI_NAME(u) << "@treasure" << std::endl;
            if (ed)
            {
                bool found = false;
                for (; ed; ed = ed->next)
                {
                    if (ed->names.StrStr("$piname"))
                    {
                        if (found == false)
                        {
                            std::cout << "Owned by:  ";
                            found = true;
                        }

                        std::cout << ed->descr.c_str() << " ";
                    }
                }
                std::cout << std::endl;
            }
        }

        return u;
    }

    if (IS_OBJ(u))
    {
        UNIT_SIZE(u) = UNIT_SIZE(pc);
    }

    return u;
}

void convert_inventory(unit_data *u, unit_data *pc, int bList = FALSE)
{
    unit_data *bla = nullptr;

    if (u == nullptr)
    {
        return;
    }

    convert_inventory(UNIT_CONTAINS(u), pc, bList);

    convert_inventory(u->next, pc, bList);

    bla = convert_item(u, pc, bList);

    assert(!u->is_destructed());

    if (bla != u)
    {
        unit_data *tmpu = nullptr;

        assert(UNIT_IN(u));

        unit_to_unit(bla, UNIT_IN(u));

        while (UNIT_CONTAINS(u))
        {
            tmpu = UNIT_CONTAINS(u);
            unit_from_unit(tmpu);
            unit_to_unit(tmpu, bla);
        }

        unit_from_unit(u);
        convert_free_unit(u);
    }
}

/* This procedure makes any conversion you might want on every player in   *
 * the playerfile, including affects and inventory                         */
void convert_player(unit_data *pc)
{
    ;
}

/* Return TRUE if Ok. */
int sanity_check(unit_data *u)
{
    if (g_nCorrupt == TRUE)
    {
        printf("Corrupted unit in READ.");
        return FALSE;
    }

    if (PC_ID(u) > top_id)
    {
        printf("Maximum ID exceeded: %d > top id %d", PC_ID(u), top_id);
        return FALSE;
    }

    if (PC_ID(u) < 0)
    {
        printf("Illegal ID < 0!");
        return FALSE;
    }

    if ((UNIT_HIT(u) > 10000) || (UNIT_MAX_HIT(u) > 10000))
    {
        printf("Corrupted UNIT HITPOINTS");
        return FALSE;
    }

    if (!IS_PC(u))
    {
        printf("Not a player!");
        return FALSE;
    }

    if (PC_TIME(u).creation > time(nullptr))
    {
        printf("Corrupted creation time.");
        return FALSE;
    }

    if (PC_TIME(u).connect > time(nullptr))
    {
        printf("Corrupted connect time.");
        return FALSE;
    }

    if (CHAR_RACE(u) >= PC_RACE_MAX)
    {
        printf("Corrupted RACE");
        return FALSE;
    }

    if (!UNIT_CONTAINS(u) && (UNIT_WEIGHT(u) != UNIT_BASE_WEIGHT(u)))
    {
        printf("Fixed illegal weight.");
        UNIT_WEIGHT(u) = UNIT_BASE_WEIGHT(u);
    }

    return TRUE;
}

int shall_delete(unit_data *pc)
{
    int days = 0;

    days = days_old(PC_TIME(pc).connect);

    /* Player which have paid at some point in time remain almost permanent. */
    if (PC_ACCOUNT(pc).total_credit > 0)
    {
        return FALSE;
    }

    if ((days > 60) && (CHAR_LEVEL(pc) <= START_LEVEL + 4))
    {
        std::cout << "2 month inactivity and less than lvl 5 Will delete ";
        return TRUE;
    }

    if ((days > 185) && (CHAR_LEVEL(pc) <= START_LEVEL + 8))
    {
        std::cout << "1/2 year inactivity and less than lvl 8 Will delete ";
        return TRUE;
    }

    return FALSE;
}

int shall_exclude(const char *name)
{
    char buf[256];
    int result = 0;

    result = _parse_name(name, buf);

    if (result == 3)
    {
        std::cout << "Name in Badnames will delete\n";
        return TRUE;
    }

    if (result == 2)
    {
        std::cout << "Name in Badstrings will delete\n";
        return TRUE;
    }

    if (result != 0)
    {
        std::cout << "Name illegal will delete\n";
        return TRUE;
    }

    return FALSE;
}

unit_data *convert_load_player(char *name)
{
    unit_data *ch = nullptr;

    if (!player_exists(name))
    {
        /* printf("No such player.\n"); */
        return nullptr;
    }

    ch = load_player(name);

    if (ch == nullptr)
    {
        printf("NULL.\n");
        return nullptr;
    }

    insert_in_unit_list(ch);
    unit_to_unit(ch, g_destroy_room);

    if (shall_exclude(name))
    {
        printf("EXCLUDED.\n");
        convert_free_unit(ch);
        return nullptr;
    }

    if (!sanity_check(ch))
    {
        printf("SANITY ERROR.\n");
        convert_free_unit(ch);
        return nullptr;
    }

    if (PC_ID(ch) > top_id)
    {
        printf("TOP ID ERROR %d vs %d.\n", PC_ID(ch), top_id);
        convert_free_unit(ch);
        return nullptr;
    }

    if (PC_ID(ch) > max_id)
    {
        max_id = PC_ID(ch);
    }

    return ch;
}

const char *isodate(tm *t)
{
    static std::string isodate;

    isodate = diku::format_to_str("%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return isodate.c_str();
}

void clist()
{
    std::string ipath;
    /*
        std::cout << "\nEnter the full path to the root player directory for example '/home/mud/vme2.0/lib/ply' or \n<enter> for the one
    listed in the server.cfg file:  "; char cpath[1024]; cpath[0]=0; std::cin.ignore(); std::cin.getline(cpath,1024); ipath=cpath;
    */
    if (ipath.empty())
    {
        ipath = g_cServerConfig.getFileInLibDir("ply");
    }

    fs::path full_path(ipath);

    unsigned long file_count = 0;
    unsigned long dir_count = 0;
    unsigned long err_count = 0;
    time_info_data tid2;

    unit_data *pc = nullptr;
    unit_data *void_char = new EMPLACE(npc_data) npc_data;

    if (!fs::exists(full_path))
    {
        std::cout << "\nNot found: " << full_path << std::endl;
        return;
    }

    if (fs::is_directory(full_path))
    {
        std::cout << "\nIn directory: " << full_path << "\n\n";
        fs::directory_iterator end_iter;
        std::string path_end = "";

        std::cout << "name;id;Level;Admin;days since login;created;birth;" << std::endl;

        for (char c = 'a'; c <= 'z'; c++)
        {
            path_end = c;
            full_path = ipath + "/" + path_end;
            if (!fs::exists(full_path))
            {
                std::cout << "\nNot found: " << full_path << std::endl;
                continue;
            }

            if (!fs::is_directory(full_path))
            {
                continue;
            }
            // std::cout << "\nIn directory: " << full_path << "\n\n";

            for (fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
            {
                char *temp = nullptr;
                try
                {
                    if (fs::is_directory(*dir_itr))
                    {
                        ++dir_count;
                    }
                    else if (dir_itr->path().filename().string().find(".") == std::string::npos)
                    {
                        ++file_count;

                        std::cout << dir_itr->path() << ";";
                        temp = new char[PC_MAX_NAME];

                        strcpy(temp, dir_itr->path().filename().c_str());
                        pc = convert_load_player(temp);
                        if (pc == nullptr)
                        {
                            std::cout << "ERROR: Corrupt\n";
                            delete temp;
                            continue;
                        }

                        std::cout << PC_ID(pc) << ";" << (int)CHAR_LEVEL(pc) << ";" << (IS_MORTAL(pc) ? " PLY  " : "ADMIN") << ";"
                                  << days_old(PC_TIME(pc).connect) << ";";

                        if (ids[PC_ID(pc)])
                        {
                            std::cout << "Duplicate ID! (" << (signed long)PC_ID(pc) << ")";
                        }
                        else
                        {
                            ids[PC_ID(pc)] = 1;
                        }

                        shall_exclude(UNIT_NAME(pc));
                        // shall_delete(pc);

                        UNIT_CONTAINS(void_char) = nullptr;
                        /* load_contents(temp, void_char);

                        if (UNIT_CONTAINS(void_char))
                        {
                            std::cout << "  INV";
                            convert_inventory(UNIT_CONTAINS(void_char), pc, TRUE);
                            free_inventory(UNIT_CONTAINS(void_char));
                        }

                        std::cout << endl;*/

                        bool tmp = false;

                        tm *t = gmtime(&PC_TIME(pc).creation);
                        std::cout << isodate(t) << ";";

                        t = gmtime(&PC_TIME(pc).birth);
                        std::cout << isodate(t) << ";";

                        time_t secs = (time_t)PC_TIME(pc).played;

                        convert_free_unit(pc);

                        time_t val = 0;
                        val = secs / SECS_PER_REAL_HOUR;
                        tid2.setHours(val % 24);
                        tid2.setDay(val / 24);
                        tid2.setMonth(-1);
                        tid2.setYear(val / (365 * 24));

                        if (tid2.getYear() > 0)
                        {
                            std::cout << " " << tid2.getYear() << " years";
                            tmp = true;
                        }
                        if (tid2.getDay() > 0)
                        {
                            std::cout << " " << tid2.getDay() << " days";
                            tmp = true;
                        }
                        if (tid2.getHours() > 0)
                        {
                            std::cout << " " << tid2.getHours() << " hours";
                            tmp = true;
                        }
                        if (tmp)
                        {
                            std::cout << ".";
                        }
                        else
                        {
                            std::cout << " Less than an hour";
                        }

                        std::cout << "; " << std::endl;

                        delete temp;
                    }
                }
                catch (const std::exception &ex)
                {
                    ++err_count;
                    std::cout << dir_itr->path() << " " << ex.what() << std::endl;
                }
            }
        }

        std::cout << "Maximum ID was " << max_id << " / Top " << top_id << "\n";
        std::cout << "\n" << file_count << " files\n" << dir_count << " directories\n" << err_count << " errors\n";
    }
    else // must be a file
    {
        std::cout << "\nFound: " << full_path << "\n";
    }

    return;
}

void convert_file()
{
    std::string ipath{g_cServerConfig.getFileInLibDir("ply")};
    fs::path full_path(ipath);

    unsigned long file_count = 0;
    unsigned long dir_count = 0;
    unsigned long err_count = 0;

    unit_data *pc = nullptr;

    if (!fs::exists(full_path))
    {
        std::cout << "\nNot found: " << full_path << std::endl;
        return;
    }

    if (fs::is_directory(full_path))
    {
        std::cout << "\nIn directory: " << full_path << "\n\n";
        fs::directory_iterator end_iter;
        std::string path_end = "";
        for (char c = 'a'; c <= 'z'; c++)
        {
            path_end = c;
            full_path = ipath + "/" + path_end;
            if (!fs::exists(full_path))
            {
                std::cout << "\nNot found: " << full_path << std::endl;
                continue;
            }

            if (!fs::is_directory(full_path))
            {
                continue;
            }
            std::cout << "\nIn directory: " << full_path << "\n\n";

            for (fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
            {
                char *temp = nullptr;
                try
                {
                    if (fs::is_directory(*dir_itr))
                    {
                        ++dir_count;
                    }
                    else if (dir_itr->path().filename().string().find(".") == std::string::npos)
                    {
                        ++file_count;
                        std::cout << dir_itr->path() << "\n";
                        temp = new char[PC_MAX_NAME];
                        strcpy(temp, dir_itr->path().filename().c_str());
                        pc = convert_load_player(temp);

                        if (pc == nullptr)
                        {
                            std::cout << "Corrupt Player ERASED." << std::endl;
                            delete_player(temp);
                            continue;
                        }

                        if (ids[PC_ID(pc)])
                        {
                            std::cout << "Duplicate ID! (" << (signed long)PC_ID(pc) << ")" << std::endl;
                        }
                        else
                        {
                            ids[PC_ID(pc)] = 1;
                        }

                        std::cout << UNIT_NAME(pc) << " Lvl [" << CHAR_LEVEL(pc) << "] " << (IS_MORTAL(pc) ? "   " : "ADMIN") << std::endl;

                        std::cout.flush();
                        load_contents(temp, pc);
                        convert_player(pc);
                        save_player(pc);
                        save_player_contents(pc, TRUE);

                        convert_free_unit(pc);
                        delete temp;
                    }
                }
                catch (const std::exception &ex)
                {
                    ++err_count;
                    std::cout << dir_itr->path() << " " << ex.what() << std::endl;
                }
            }
        }

        std::cout << "\n" << file_count << " files\n" << dir_count << " directories\n" << err_count << " errors\n";
    }
    else // must be a file
    {
        std::cout << "\nFound: " << full_path << "\n";
    }

    return;

} /*end of convert_file*/

void cleanup()
{
    std::string ipath{g_cServerConfig.getFileInLibDir("ply")};

    fs::path full_path(ipath);

    unsigned long file_count = 0;
    unsigned long players_deleted = 0;
    unsigned long players_converted = 0;
    unsigned long dir_count = 0;
    unsigned long err_count = 0;

    unit_data *pc = nullptr;
    unit_data *void_char = new EMPLACE(npc_data) npc_data;

    if (!fs::exists(full_path))
    {
        std::cout << "\nNot found: " << full_path << std::endl;
        return;
    }

    if (fs::is_directory(full_path))
    {
        std::cout << "\nIn directory: " << full_path << "\n\n";
        fs::directory_iterator end_iter;
        std::string path_end = "";
        for (char c = 'a'; c <= 'z'; c++)
        {
            path_end = c;
            full_path = ipath + "/" + path_end;
            if (!fs::exists(full_path))
            {
                std::cout << "\nNot found: " << full_path << std::endl;
                continue;
            }

            if (!fs::is_directory(full_path))
            {
                continue;
            }
            std::cout << "\nIn directory: " << full_path << "\n\n";

            for (fs::directory_iterator dir_itr(full_path); dir_itr != end_iter; ++dir_itr)
            {
                char *temp = nullptr;
                try
                {
                    if (fs::is_directory(*dir_itr))
                    {
                        ++dir_count;
                    }
                    else if (dir_itr->path().filename().string().find(".") == std::string::npos)
                    {
                        ++file_count;
                        std::cout << dir_itr->path() << "\n";
                        temp = new char[PC_MAX_NAME];
                        strcpy(temp, dir_itr->path().filename().c_str());
                        std::cout << temp;
                        pc = convert_load_player(temp);

                        if (pc == nullptr)
                        {
                            std::cout << "Corrupt Player ERASED." << std::endl;
                            delete_player(temp);
                            delete temp;
                            ++players_deleted;
                            continue;
                        }

                        if (str_ccmp(temp, UNIT_NAME(pc)))
                        {
                            std::cout << "Name in file doesn't match filename." << std::endl;
                            convert_free_unit(pc);
                            delete_player(temp);
                            ++players_deleted;
                            delete temp;
                            continue;
                        }

                        std::cout << UNIT_NAME(pc) << "]  Lvl " << CHAR_LEVEL(pc) << "]  " << (IS_MORTAL(pc) ? "   " : "ADMIN");

                        if (shall_delete(pc))
                        {
                            convert_free_unit(pc);
                            delete_player(temp);
                            ++players_deleted;
                            delete temp;
                            continue;
                        }

                        std::cout.flush();

                        UNIT_CONTAINS(void_char) = nullptr;
                        load_contents(temp, void_char);
                        if (UNIT_CONTAINS(void_char))
                        {
                            std::cout << "  INV";
                            convert_inventory(UNIT_CONTAINS(void_char), pc, TRUE);
                            free_inventory(UNIT_CONTAINS(void_char));
                        }

                        convert_free_unit(pc);
                        ++players_converted;
                        delete temp;
                    }
                }
                catch (const std::exception &ex)
                {
                    ++err_count;
                    std::cout << dir_itr->path() << " " << ex.what() << std::endl;
                }
            }
        }

        std::cout << "Players deleted:  " << players_deleted << std::endl << "Players converted:  " << players_converted << std::endl;
        std::cout << "\n" << file_count << " files\n" << dir_count << " directories\n" << err_count << " errors\n";
    }
    else // must be a file
    {
        std::cout << "\nFound: " << full_path << "\n";
    }

    return;
}

void cleanup_playerfile(int c)
{
    top_id = read_player_id();
    CREATE(ids, ubit8, top_id + 1);

    memset(ids, 0, top_id);

    g_entry_room = new EMPLACE(room_data) room_data;
    g_destroy_room = new EMPLACE(room_data) room_data;
    if (c == 1)
    {
        convert_file();
    }
    else if (c == 2)
    {
        cleanup();
    }
    else
    {
        clist();
    }

    printf("\n\nFinished.\n");
}
