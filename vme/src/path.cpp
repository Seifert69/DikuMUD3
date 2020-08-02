/*
 $Author: All $
 $RCSfile: path.cpp,v $
 $Date: 2003/11/29 03:31:16 $
 $Revision: 2.8 $
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/graph_utility.hpp>
#include <pthread.h>
#include <vector>
#include <list>

#include "structs.h"
#include "utils.h"
#include "interpreter.h"
#include "comm.h"
#include "db.h"
#include "handler.h"
#include "utility.h"
#include "common.h"
#include "constants.h"
#include "sector.h"
#include "path.h"

void signal_reset(void);
extern cSector sector_dat;
void create_sc_graph(int num_of_sc);
void *create_sc_dijkstra(void *thread);
pthread_mutex_t dijkstra_queue_mutex;
std::list<unit_data *> dijkstra_queue;
pthread_t dijkstra_thread;

extern unit_data *room_head;

using namespace boost;

std::vector<graph_t> sc_graphs;
std::vector<std::vector<unit_data *>> sc_room_ptr;

void create_worldgraph()
{
    graph_world_t WorldGraph;
    typedef graph_traits<graph_world_t>::vertex_descriptor vertex_descriptor;
    //	typedef graph_traits < graph_world_t >::edge_descriptor edge_descriptor;
    vertex_descriptor vd;
    unit_data *u, *uu;
    int i;
    for (u = room_head; u && UNIT_TYPE(u) == UNIT_ST_ROOM; u = u->gnext)
    {
        vd = add_vertex(WorldGraph);
        ROOM_NUM(u) = vd;
    }

    for (u = room_head; u && UNIT_TYPE(u) == UNIT_ST_ROOM; u = u->gnext)
    {
        for (i = 0; i <= MAX_EXIT; i++)
            if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->to_room)
                add_edge(ROOM_NUM(u), ROOM_NUM(ROOM_EXIT(u, i)->to_room),
                         1, WorldGraph);

        //DIR_ENTER
        for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
            if (IS_ROOM(uu) && IS_SET(UNIT_MANIPULATE(uu), MANIPULATE_ENTER))
                add_edge(ROOM_NUM(u), ROOM_NUM(uu), 10, WorldGraph);

        //DIR_EXIT
        if (UNIT_IN(u) && IS_ROOM(UNIT_IN(u)))
            if (IS_SET(UNIT_MANIPULATE(UNIT_IN(u)), MANIPULATE_ENTER))
                add_edge(ROOM_NUM(u), ROOM_NUM(UNIT_IN(u)), 10, WorldGraph);
    }

    //	slog (LOG_ALL, 0, "Added %d vertices and %d edges to graph",
    //num_vertices (WorldGraph), num_edges (WorldGraph));

    slog(LOG_ALL, 0, "Creating Strong Connected Components");
    std::vector<vertex_descriptor> sc(num_vertices(WorldGraph));
    i = strong_components(WorldGraph,
                          make_iterator_property_map(sc.begin(),
                                                     get(vertex_index,
                                                         WorldGraph)));
    std::vector<vertex_descriptor>
        sc_num(i);
    slog(LOG_ALL, 0, "Total Number of SC - %d", i);
    slog(LOG_ALL, 0, "Completed Strong Connected Components");
    for (u = room_head; u && UNIT_TYPE(u) == UNIT_ST_ROOM; u = u->gnext)
    {
        ROOM_SC(u) = sc[ROOM_NUM(u)];
        //			slog (LOG_ALL, 0, "%d - %d - %s@%s", ROOM_SC (u), ROOM_NUM (u),
        //						UNIT_FI_NAME (u), UNIT_FI_ZONENAME (u));
        sc_num[ROOM_SC(u)]++;
    }

    create_sc_graph(i);
    pthread_mutex_init(&dijkstra_queue_mutex, NULL);
    slog(LOG_ALL, 0, "Create Shortest Path VME thread");
    pthread_create(&dijkstra_thread, NULL, create_sc_dijkstra, (void *)NULL);
}

void cleanup_sc_dijkstra(void *t)
{
    slog(LOG_ALL, 0, "Shutting Down Shortest Path VME thread");
}

void *create_sc_dijkstra(void *thread)
{
    //	typedef graph_traits < graph_t >::vertex_descriptor vertex_descriptor;
    unit_data *u;
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(cleanup_sc_dijkstra, NULL);

    extern int mud_shutdown;

    while (!mud_shutdown)
    {
        while (dijkstra_queue.size() > 0)
        {
            pthread_mutex_lock(&dijkstra_queue_mutex);
            u = dijkstra_queue.front();
            dijkstra_queue.pop_front();
            pthread_mutex_unlock(&dijkstra_queue_mutex);
            //					slog (LOG_ALL, 0, "Creating Dijkstra for room %s@%s",
            //								UNIT_FI_NAME (u), UNIT_FI_ZONENAME (u));

            ROOM_PATH(u).resize(num_vertices(sc_graphs[ROOM_SC(u)]));
            ROOM_DISTANCE(u).resize(num_vertices(sc_graphs[ROOM_SC(u)]));

            dijkstra_shortest_paths(sc_graphs[ROOM_SC(u)], ROOM_NUM(u),
                                    predecessor_map(&ROOM_PATH(u)[0]).distance_map(&ROOM_DISTANCE(u)[0]));
            ROOM_WAITD(u) = FALSE;
        }
        usleep(10000);
    }
    pthread_cleanup_pop(1);
    slog(LOG_ALL, 0, "Exiting Dijkstra Thread");

    //pthread_exit(0);
    return NULL;
}

void create_sc_graph(int num_of_sc)
{
    int sc = 0;
    typedef graph_traits<graph_t>::vertex_descriptor vertex_descriptor;
    typedef graph_traits<graph_t>::edge_descriptor edge_descriptor;
    int i;
    vertex_descriptor vd;
    unit_data *u, *uu;
    graph_t base_g;
    bool success;
    edge_descriptor ed;

    std::vector<unit_data *> rmptr;

    //	slog (LOG_ALL, 0, "Number of SC to create %d", num_of_sc);
    for (sc = 0; sc < num_of_sc; sc++)
    {
        sc_graphs.push_back(base_g);
        sc_room_ptr.push_back(rmptr);

        for (u = room_head; u && UNIT_TYPE(u) == UNIT_ST_ROOM; u = u->gnext)
            if (ROOM_SC(u) == sc)
            {
                vd = add_vertex(sc_graphs[sc]);
                sc_room_ptr.back().push_back(u);
                ROOM_NUM(u) = vd;
            }

        property_map<graph_t, edge_dir_t>::type dir = get(edge_dir, sc_graphs[sc]);

        for (u = room_head; u && UNIT_TYPE(u) == UNIT_ST_ROOM; u = u->gnext)
        {
            for (i = 0; i <= MAX_EXIT; i++)
                if (ROOM_EXIT(u, i) && ROOM_EXIT(u, i)->to_room && (ROOM_SC(u) == sc) && (ROOM_SC(ROOM_EXIT(u, i)->to_room) == sc))
                {
                    tie(ed, success) = add_edge(ROOM_NUM(u),
                                                ROOM_NUM(ROOM_EXIT(u, i)->to_room),
                                                path_weight(u, ROOM_EXIT(u, i)->to_room, i), sc_graphs[sc]);
                    dir[ed] = i;
                }
            //DIR_ENTER
            for (uu = UNIT_CONTAINS(u); uu; uu = uu->next)
                if (IS_ROOM(uu) && IS_SET(UNIT_MANIPULATE(uu), MANIPULATE_ENTER) && (ROOM_SC(u) == sc) && (ROOM_SC(uu) == sc))
                {
                    tie(ed, success) = add_edge(ROOM_NUM(u), ROOM_NUM(uu), path_weight(u, uu, DIR_ENTER) + 10, sc_graphs[sc]);
                    dir[ed] = DIR_ENTER;
                }
            //DIR_EXIT
            if (UNIT_IN(u) && IS_ROOM(UNIT_IN(u)))
                if (IS_SET(UNIT_MANIPULATE(UNIT_IN(u)), MANIPULATE_ENTER) && (ROOM_SC(u) == sc) && (ROOM_SC(UNIT_IN(u)) == sc))
                {
                    tie(ed, success) = add_edge(ROOM_NUM(u), ROOM_NUM(UNIT_IN(u)), path_weight(u, UNIT_IN(u), DIR_EXIT) + 10, sc_graphs[sc]);
                    dir[ed] = DIR_EXIT;
                }
        }
    }
    //	for (sc = 0; sc < num_of_sc; sc++)
    //{
    //slog (LOG_ALL, 0, "Added %d vertices and %d edges to graph %d",
    //num_vertices (sc_graphs[sc]), num_edges (sc_graphs[sc]), sc);
    //}
}

int path_weight(unit_data *from, unit_data *to, int dir)
{
    int weight = 0;

    if (dir <= DIR_SOUTHWEST)
    {
        if (IS_SET(ROOM_EXIT(from, dir)->exit_info, EX_OPEN_CLOSE))
            weight += 10;

        if (IS_SET(ROOM_EXIT(from, dir)->exit_info, EX_LOCKED))
            weight += 50;

        if (IS_SET(ROOM_EXIT(from, dir)->exit_info, EX_HIDDEN))
            weight += 200;
    }


    if ((dir == DIR_EXIT) || (dir == DIR_ENTER))
        weight += 10;

    if ((ROOM_LANDSCAPE(from) == SECT_WATER_SWIM) || (ROOM_LANDSCAPE(to) == SECT_WATER_SWIM))
        weight += 500;

    if ((ROOM_LANDSCAPE(from) == SECT_WATER_SAIL) || (ROOM_LANDSCAPE(to) == SECT_WATER_SAIL))
        weight += 5000;

    weight += sector_dat.get_path_cost(ROOM_LANDSCAPE(from), ROOM_LANDSCAPE(to));

    return weight;
}

/* Primitive move generator, returns direction */
int move_to(unit_data *from, unit_data *to)
{
    typedef graph_traits<graph_t>::edge_descriptor edge_descriptor;
    edge_descriptor ed;
    bool success;
    int i, next;

    if (!from)
        return DIR_IMPOSSIBLE;

    /* Assertion is that both from and to are rooms! */
    if (!IS_ROOM(from) || !IS_ROOM(to))
        return DIR_IMPOSSIBLE;

    if (from == to)
        return DIR_HERE;

    if (ROOM_SC(from) != ROOM_SC(to))
        return DIR_IMPOSSIBLE;

    if (ROOM_WAITD(from) == TRUE)
        return DIR_TRYAGAIN;

    if (ROOM_PATH(from).size() != num_vertices(sc_graphs[ROOM_SC(from)]))
    {
        pthread_mutex_lock(&dijkstra_queue_mutex);
        ROOM_WAITD(from) = TRUE;
        dijkstra_queue.push_back(from);
        pthread_mutex_unlock(&dijkstra_queue_mutex);
        return DIR_TRYAGAIN;
    }

    i = ROOM_NUM(to);
    next = i; //MS2020 Next could be uninitialized. I'm not sure if it should be zero or i? Or if more bugs are hiding here.
    while (i != ROOM_NUM(from))
    {
        next = i;
        i = ROOM_PATH(from)[i];
    }

    property_map<graph_t, edge_dir_t>::type dir = get(edge_dir, sc_graphs[ROOM_SC(from)]);
    tie(ed, success) = edge(ROOM_NUM(from), next, sc_graphs[ROOM_SC(from)]);
    if (success)
        return (dir[ed]);
    else
        return (DIR_IMPOSSIBLE);
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
