#pragma once

#include "structs.h"

#include <boost/graph/adjacency_list.hpp>

namespace boost
{
enum edge_dir_t
{
    edge_dir = 101
};
BOOST_INSTALL_PROPERTY(edge, dir);
} // namespace boost

using graph_world_t =
    boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::no_property, boost::property<boost::edge_weight_t, int>>;
using graph_t = boost::adjacency_list<boost::vecS,
                                      boost::vecS,
                                      boost::directedS,
                                      boost::no_property,
                                      boost::property<boost::edge_weight_t, int, boost::property<boost::edge_dir_t, int>>>;

int move_to(unit_data *from, unit_data *to);
int path_weight(unit_data *from, unit_data *to, int dir);
void create_sc_graph(int num_of_sc);
void *create_sc_dijkstra(void *thread);
void create_worldgraph();

extern std::vector<graph_t> g_sc_graphs;
extern std::vector<std::vector<unit_data *>> g_sc_room_ptr;
extern pthread_t g_dijkstra_thread;
