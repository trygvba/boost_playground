#include <doctest/doctest.h>
#include <memory>
#include <algorithm>
#include "entities.h"
#include "model_networks.h"
#include "boost_adaptor.h"
TEST_SUITE("playground_boost_graph") {
    TEST_CASE("simple_shortest_path") {
        using playground::graph::network;
        namespace graph = playground::graph;
        auto net = test::shortest_path_graph();
        CHECK_EQ(net.nodes.size(), 6);
        CHECK_EQ(net.edges.size(), 9);
        CHECK_EQ(net.nodes.size(), graph::num_vertices(net));
        CHECK_EQ(net.edges.size(), graph::num_edges(net));

        using Vertex = boost::graph_traits<network>::vertex_descriptor;
        //typedef boost::property_map::type IndexMap;
        using IndexMap = boost::network_id_map;
        IndexMap index = get(boost::vertex_index, net);

        // Get vertices using graph_traits:
        typedef boost::graph_traits<network>::vertex_iterator vertex_iter;
        std::pair<vertex_iter, vertex_iter> vp;
        std::cout << "vertices(g): ";
        for (vp = graph::vertices(net); vp.first != vp.second; ++vp.first) {
            Vertex v = *vp.first;
            std::cout << index[v] << " ";
        }
        std::cout << "\n";

        // Access edges:
        using EdgeWeights = boost::network_edge_weight_map;
        EdgeWeights weights = get(boost::edge_weight, net);
        std::cout << "edges(g): ";
        boost::graph_traits<network>::edge_iterator ei, ei_end;
        for(boost::tie(ei, ei_end) = graph::edges(net); ei != ei_end; ++ei) {
            std::cout << "(" << index[graph::source(*ei, net)] << ", "
                << index[graph::target(*ei, net)] << "){ "
                << weights[*ei] << "} ";
        }
        std::cout << "\n";

        // Adjacency structure:
        std::for_each(graph::vertices(net).first, graph::vertices(net).second,
            graph::vertex_visitor(net));
        std::vector<int> exp_out = {3,2,2,1,1,0};
        for (int i=0; i<6; i++) {
            CHECK_EQ(exp_out[i], graph::out_degree(net.nodes[i], net));
        }

        // Dijkstra's:
        std::vector<double> d(graph::num_vertices(net));
        // Use the first node as source:
        auto s = *(graph::vertices(net).first);
        // Invoke variant 2 of Dijkstra's algorithm
        boost::dijkstra_shortest_paths(net, s,
            boost::distance_map(make_iterator_property_map(d.begin(),
                                                           get(boost::vertex_index, net))));

        std::cout << "distances from start node:\n";
        boost::graph_traits<network>::vertex_iterator vi;
        for (vi = vertices(net).first; vi != vertices(net).second; ++vi){
                std::cout << "distance(" << index[*vi] << ") = "
                << d[index[*vi]] << "\n";
        }

        std::vector<double> exp_dist = {0, 7, 9, 20, 26, 11};
        for (int i=0; i < 6; i++) {
            REQUIRE(d[i] == doctest::Approx(exp_dist[i]));
        }
   }
}