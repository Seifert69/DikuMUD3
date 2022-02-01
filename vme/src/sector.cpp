#include "sector.h"

#include "db.h"
#include "essential.h"
#include "slog.h"

#include <fstream>
#include <queue>
#include <string>
#include <vector>

std::string cSector::get_name(int sector)
{
    if ((sector >= 0) && (sector < (int)names.size()))
    {
        return (names[sector]);
    }
    else
    {
        return ("");
    }
};

int cSector::get_path_cost(int from, int to)
{
    if ((from >= 0) && (from < (int)names.size()) && (to >= 0) && (to < (int)names.size()))
    {
        if (path[from][to] > 0)
        {
            return (path[from][to]);
        }
        else
        {
            return (1);
        }
    }
    else
    {
        return (1);
    }
};

int cSector::get_enduance_cost(int from, int to)
{
    if ((from >= 0) && (from < (int)names.size()) && (to >= 0) && (to < (int)names.size()))
    {
        return (endurance[from][to]);
    }
    else
    {
        return (1);
    }
};

void cSector::add_sector(int sector, std::string sector_name)
{
    if (sector < 0)
    {
        return;
    }

    if ((int)names.size() < (sector + 1))
    {
        names.resize(sector + 1);
        path.resize(sector + 1);
        endurance.resize(sector + 1);

        for (int x = 0; x < (int)names.size(); x++)
        {
            path[x].resize(sector + 1);
            endurance[x].resize(sector + 1);
        }
    }
    names[sector] = sector_name;
};

void cSector::add_sector(int sector, char *sector_name)
{
    if (sector < 0)
    {
        return;
    }

    if ((int)names.size() < (sector + 1))
    {
        names.resize(sector + 1);
        path.resize(sector + 1);
        endurance.resize(sector + 1);

        for (int x = 0; x < (int)names.size(); x++)
        {
            path[x].resize(sector + 1);
            endurance[x].resize(sector + 1);
        }
    }
    names[sector] = sector_name;
};

void cSector::set_path_cost(int from, int to, int cost)
{
    if ((from < 0) || (to < 0))
    {
        return;
    }

    if (((int)names.size() > from) && ((int)names.size() > to))
    {
        path[from][to] = cost;
    }
};

void cSector::set_endurance_cost(int from, int to, int cost)
{
    if ((from < 0) || (to < 0))
    {
        return;
    }

    if (((int)names.size() > from) && ((int)names.size() > to))
    {
        endurance[from][to] = cost;
    }
};

void cSector::set_path_endurance_cost(int from, int to, int pcost, int ecost)
{
    if ((from < 0) || (to < 0))
    {
        return;
    }

    if (((int)names.size() > from) && ((int)names.size() > to))
    {
        path[from][to] = pcost;
        endurance[from][to] = ecost;
    }
};

void boot_sector()
{
    std::ifstream in_file;
    std::string temp;
    std::vector<std::string> sector_vect;
    std::queue<int> sector_queue;

    in_file.open(g_cServerConfig.getFileInLibDir(SECTOR_DEFS));
    if (in_file.bad())
    {
        slog(LOG_ALL, 0, "ERROR:  Problem opening the Sector.dat");
    }
    else
    {
        while (in_file >> temp)
        {
            if (temp == "INDEX")
            {
                unsigned int index = 0;

                in_file >> temp;
                in_file >> index;
                if (sector_vect.size() < (index + 1))
                {
                    sector_vect.resize(index + 1);
                }
                sector_vect[index] = "";
                while (in_file >> temp)
                {
                    if ((temp == "NAME") || (temp == "="))
                    {
                        continue;
                    }
                    if (temp == "BEGIN")
                    {
                        break;
                    }
                    if (temp.length() > 0)
                    {
                        sector_vect[index] += " " + temp;
                    }
                    else
                    {
                        sector_vect[index] += temp;
                    }
                }

                sector_queue.push(index);
                while (in_file >> temp)
                {
                    if (temp == "END")
                    {
                        sector_queue.push(-1);
                        break;
                    }
                    else
                    {
                        sector_queue.push(atoi(temp.c_str()));
                    }
                }
            }
        }
        in_file.close();

        for (unsigned int i = 0; i < sector_vect.size(); i++)
        {
            g_sector_dat.add_sector(i, sector_vect[i]);
        }

        int sector_index = 0;
        int sector_end = 0;
        int sector_path = 0;
        int sector_from = 0;
        while (sector_queue.size() > 0)
        {
            sector_from = 0;
            sector_end = 0;
            sector_path = 0;
            sector_index = 0;
            sector_from = sector_queue.front();
            sector_queue.pop();

            while (sector_queue.front() != -1)
            {
                sector_index = sector_queue.front();
                sector_queue.pop();

                sector_end = sector_queue.front();
                sector_queue.pop();

                sector_path = sector_queue.front();
                sector_queue.pop();
                g_sector_dat.set_path_endurance_cost(sector_from, sector_index, sector_path, sector_end);
            }
            sector_queue.pop();
        }
    }
}
