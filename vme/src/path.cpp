/*
 $Author: All $
 $RCSfile: path.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.8 $
 */
#include "path.h"

#include "common.h"
#include "constants.h"
#include "db.h"
#include "main_functions.h"
#include "sector.h"
#include "slog.h"
#include "utils.h"

#include <pthread.h>

#include <list>
#include <tuple>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/strong_components.hpp>

pthread_mutex_t dijkstra_queue_mutex;
std::list<unit_data *> dijkstra_queue;
pthread_t g_dijkstra_thread;

std::vector<graph_t> g_sc_graphs;
std::vector<std::vector<unit_data *>> g_sc_room_ptr;

void create_worldgraph()
{
    graph_world_t WorldGraph;
    typedef boost::graph_traits<graph_world_t>::vertex_descriptor vertex_descriptor;
    //	typedef graph_traits < graph_world_t >::edge_descriptor edge_descriptor;
    vertex_descriptor vd = 0;
    unit_data *u = nullptr;
    unit_data *uu = nullptr;
    int i = 0;
    for (u = g_room_head; u && u->getUnitType() == UNIT_ST_ROOM; u = u->getGlobalNext())
    {
        vd = add_vertex(WorldGraph);
        UROOM(u)->setRoomNumber(vd);
    }

    for (u = g_room_head; u && u->getUnitType() == UNIT_ST_ROOM; u = u->getGlobalNext())
    {
        for (i = 0; i <= MAX_EXIT; i++)
        {
            if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->getToRoom())
            {
                add_edge(ROOM_NUM(u), ROOM_NUM(ROOM_EXIT(u, i)->getToRoom()), 1, WorldGraph);
            }
        }

        // DIR_ENTER
        for (uu = u->getUnitContains(); uu; uu = uu->getNext())
        {
            if (uu->isRoom() && IS_SET(uu->getManipulate(), MANIPULATE_ENTER))
            {
                add_edge(ROOM_NUM(u), ROOM_NUM(uu), 10, WorldGraph);
            }
        }

        // DIR_EXIT
        if (u->getUnitIn() && u->getUnitIn()->isRoom())
        {
            if (IS_SET(u->getUnitIn()->getManipulate(), MANIPULATE_ENTER))
            {
                add_edge(ROOM_NUM(u), ROOM_NUM(u->getUnitIn()), 10, WorldGraph);
            }
        }
    }

    //	slog (LOG_ALL, 0, "Added %d vertices and %d edges to graph",
    // num_vertices (WorldGraph), num_edges (WorldGraph));

    slog(LOG_ALL, 0, "Creating Strong Connected Components");
    std::vector<vertex_descriptor> sc(num_vertices(WorldGraph));
    i = strong_components(WorldGraph, make_iterator_property_map(sc.begin(), get(boost::vertex_index, WorldGraph)));
    std::vector<vertex_descriptor> sc_num(i);
    slog(LOG_ALL, 0, "Total Number of SC - %d", i);
    slog(LOG_ALL, 0, "Completed Strong Connected Components");
    for (u = g_room_head; u && u->getUnitType() == UNIT_ST_ROOM; u = u->getGlobalNext())
    {
        UROOM(u)->setStrongComponent(sc[ROOM_NUM(u)]);
        //			slog (LOG_ALL, 0, "%d - %d - %s@%s", ROOM_SC (u), ROOM_NUM (u),
        //						UNIT_FI_NAME (u), UNIT_FI_ZONENAME (u));
        sc_num[ROOM_SC(u)]++;
    }

    create_sc_graph(i);
    pthread_mutex_init(&dijkstra_queue_mutex, nullptr);
    slog(LOG_ALL, 0, "Create Shortest Path VME thread");
    pthread_create(&g_dijkstra_thread, nullptr, create_sc_dijkstra, (void *)nullptr);
}

void cleanup_sc_dijkstra(void *t)
{
    slog(LOG_ALL, 0, "Shutting Down Shortest Path VME thread");
}

void *create_sc_dijkstra(void *thread)
{
    //	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    unit_data *u = nullptr;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, nullptr);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
    pthread_cleanup_push(cleanup_sc_dijkstra, nullptr);

    while (!g_mud_shutdown)
    {
        while (dijkstra_queue.size() > 0)
        {
            pthread_mutex_lock(&dijkstra_queue_mutex);
            u = dijkstra_queue.front();
            dijkstra_queue.pop_front();
            pthread_mutex_unlock(&dijkstra_queue_mutex);
            //					slog (LOG_ALL, 0, "Creating Dijkstra for room %s@%s",
            //								UNIT_FI_NAME (u), UNIT_FI_ZONENAME (u));

            ROOM_PATH(u).resize(num_vertices(g_sc_graphs[ROOM_SC(u)]));
            ROOM_DISTANCE(u).resize(num_vertices(g_sc_graphs[ROOM_SC(u)]));

            dijkstra_shortest_paths(g_sc_graphs[ROOM_SC(u)],
                                    ROOM_NUM(u),
                                    boost::predecessor_map(&ROOM_PATH(u)[0]).distance_map(&ROOM_DISTANCE(u)[0]));
            pthread_mutex_lock(&dijkstra_queue_mutex);
            UROOM(u)->setWaitingDijkstra(false);
            pthread_mutex_unlock(&dijkstra_queue_mutex);
        }
        usleep(10000);
    }
    pthread_cleanup_pop(1);
    slog(LOG_ALL, 0, "Exiting Dijkstra Thread");

    // pthread_exit(0);
    return nullptr;
}

void create_sc_graph(int num_of_sc)
{
    int sc = 0;
    typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    int i = 0;
    vertex_descriptor vd = 0;
    unit_data *u = nullptr;
    unit_data *uu = nullptr;
    graph_t base_g;
    bool success = 0;
    edge_descriptor ed;

    std::vector<unit_data *> rmptr;

    //	slog (LOG_ALL, 0, "Number of SC to create %d", num_of_sc);
    for (sc = 0; sc < num_of_sc; sc++)
    {
        g_sc_graphs.push_back(base_g);
        g_sc_room_ptr.push_back(rmptr);

        for (u = g_room_head; u && u->getUnitType() == UNIT_ST_ROOM; u = u->getGlobalNext())
        {
            if (ROOM_SC(u) == sc)
            {
                vd = add_vertex(g_sc_graphs[sc]);
                g_sc_room_ptr.back().push_back(u);
                UROOM(u)->setRoomNumber(vd);
            }
        }

        boost::property_map<graph_t, boost::edge_dir_t>::type dir = get(boost::edge_dir, g_sc_graphs[sc]);

        for (u = g_room_head; u && u->getUnitType() == UNIT_ST_ROOM; u = u->getGlobalNext())
        {
            for (i = 0; i <= MAX_EXIT; i++)
            {
                if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->getToRoom() && (ROOM_SC(u) == sc) && (ROOM_SC(ROOM_EXIT(u, i)->getToRoom()) == sc))
                {
                    tie(ed, success) = add_edge(ROOM_NUM(u),
                                                ROOM_NUM(ROOM_EXIT(u, i)->getToRoom()),
                                                path_weight(u, ROOM_EXIT(u, i)->getToRoom(), i),
                                                g_sc_graphs[sc]);
                    dir[ed] = i;
                }
            }
            // DIR_ENTER
            for (uu = u->getUnitContains(); uu; uu = uu->getNext())
            {
                if (uu->isRoom() && IS_SET(uu->getManipulate(), MANIPULATE_ENTER) && (ROOM_SC(u) == sc) && (ROOM_SC(uu) == sc))
                {
                    tie(ed, success) = add_edge(ROOM_NUM(u), ROOM_NUM(uu), path_weight(u, uu, DIR_ENTER) + 10, g_sc_graphs[sc]);
                    dir[ed] = DIR_ENTER;
                }
            }
            // DIR_EXIT
            if (u->getUnitIn() && u->getUnitIn()->isRoom())
            {
                if (IS_SET(u->getUnitIn()->getManipulate(), MANIPULATE_ENTER) && (ROOM_SC(u) == sc) && (ROOM_SC(u->getUnitIn()) == sc))
                {
                    tie(ed, success) =
                        add_edge(ROOM_NUM(u), ROOM_NUM(u->getUnitIn()), path_weight(u, u->getUnitIn(), DIR_EXIT) + 10, g_sc_graphs[sc]);
                    dir[ed] = DIR_EXIT;
                }
            }
        }
    }
    //	for (sc = 0; sc < num_of_sc; sc++)
    //{
    // slog (LOG_ALL, 0, "Added %d vertices and %d edges to graph %d",
    // num_vertices (g_sc_graphs[sc]), num_edges (g_sc_graphs[sc]), sc);
    //}
}

int path_weight(unit_data *from, unit_data *to, int dir)
{
    int weight = 0;

    if (dir <= DIR_SOUTHWEST)
    {
        if (ROOM_EXIT(from, dir)->isDoorFlagSet(EX_OPEN_CLOSE))
        {
            weight += 10;
        }

        if (ROOM_EXIT(from, dir)->isDoorFlagSet(EX_LOCKED))
        {
            weight += 50;
        }

        if (ROOM_EXIT(from, dir)->isDoorFlagSet(EX_HIDDEN))
        {
            weight += 200;
        }
    }

    if ((dir == DIR_EXIT) || (dir == DIR_ENTER))
    {
        weight += 10;
    }

    if ((ROOM_LANDSCAPE(from) == SECT_WATER_SWIM) || (ROOM_LANDSCAPE(to) == SECT_WATER_SWIM))
    {
        weight += 500;
    }

    if ((ROOM_LANDSCAPE(from) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(to) == SECT_WATER_SAIL))
    {
        weight += 5000;
    }

    weight += g_sector_dat.get_path_cost(ROOM_LANDSCAPE(from), ROOM_LANDSCAPE(to));

    return weight;
}

/* Primitive move generator, returns direction */
int move_to(unit_data *from, unit_data *to)
{
    typedef boost::graph_traits<graph_t>::edge_descriptor edge_descriptor;
    edge_descriptor ed;
    bool success = 0;
    int i = 0;
    int next = 0;

    if (!from)
    {
        return DIR_IMPOSSIBLE;
    }

    /* Assertion is that both from and to are rooms! */
    if (!from->isRoom() || !to->isRoom())
    {
        return DIR_IMPOSSIBLE;
    }

    if (from == to)
    {
        return DIR_HERE;
    }

    if (ROOM_SC(from) != ROOM_SC(to))
    {
        return DIR_IMPOSSIBLE;
    }
    pthread_mutex_lock(&dijkstra_queue_mutex);
    auto is_waiting = UROOM(from)->getWaitingDijkstra();
    pthread_mutex_unlock(&dijkstra_queue_mutex);
    if (is_waiting == true)
    {
        return DIR_TRYAGAIN;
    }

    if (ROOM_PATH(from).size() != num_vertices(g_sc_graphs[ROOM_SC(from)]))
    {
        pthread_mutex_lock(&dijkstra_queue_mutex);
        UROOM(from)->setWaitingDijkstra(true);
        dijkstra_queue.push_back(from);
        pthread_mutex_unlock(&dijkstra_queue_mutex);
        return DIR_TRYAGAIN;
    }

    i = ROOM_NUM(to);
    next = i; // MS2020 Next could be uninitialized. I'm not sure if it should be zero or i? Or if more bugs are hiding here.
    while (i != ROOM_NUM(from))
    {
        next = i;
        i = ROOM_PATH(from)[i];
    }

    boost::property_map<graph_t, boost::edge_dir_t>::type dir = get(boost::edge_dir, g_sc_graphs[ROOM_SC(from)]);
    tie(ed, success) = edge(ROOM_NUM(from), next, g_sc_graphs[ROOM_SC(from)]);
    if (success)
    {
        return (dir[ed]);
    }
    else
    {
        return (DIR_IMPOSSIBLE);
    }
    /*	for (i = 0; i <= MAX_EXIT; i++)
                if (ROOM_EXIT(from,i) && ROOM_EXIT(from,i)->to_room && ROOM_NUM (ROOM_EXIT (from, i)->to_room) == next)
                        return i;


        for (uu = UNIT_CONTAINS (from); uu; uu = uu->next)
                if (IS_ROOM (uu)
                                && IS_SET (UNIT_MANIPULATE (uu), MANIPULATE_ENTER)
                                && ROOM_NUM (uu) == next)
                        return DIR_ENTER;

        if (UNIT_IN (from) && IS_ROOM (UNIT_IN (from)))
                if (IS_SET (UNIT_MANIPULATE (UNIT_IN (from)), MANIPULATE_ENTER)
                                && ROOM_NUM (UNIT_IN (from)) == next)
                        return DIR_EXIT;

        return DIR_IMPOSSIBLE;
        */
}
