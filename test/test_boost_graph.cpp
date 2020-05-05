#include <doctest/doctest.h>
#include <memory>
#include <algorithm>
#include "entities.h"
#include "model_networks.h"
#include "boost_adaptor.h"
#include "inc_boost_adaptor.h"

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
        auto distances = make_iterator_property_map(d.begin(),
                                                    get(boost::vertex_index, net));
        boost::dijkstra_shortest_paths(net, s,
            boost::distance_map(distances)
            .weight_map(weights));

        std::cout << "distances from start node:\n";
        boost::graph_traits<network>::vertex_iterator vi;
        for (vi = vertices(net).first; vi != vertices(net).second; ++vi){
                std::cout << "distance(" << index[*vi] << ") = "
                << distances[*vi] << "\n";
        }

        std::vector<double> exp_dist = {0, 7, 9, 20, 26, 11};
        for (int i=0; i < 6; i++) {
            REQUIRE(d[i] == doctest::Approx(exp_dist[i]));
        }
   }

   TEST_CASE("inc_boost_graph") {
        std::cout << "Starting inc_boost_graph.\n";
        using IncNet = playground::graph::incidence_network;
        using Vertex = boost::graph_traits<IncNet>::vertex_descriptor;
        namespace graph = playground::graph;

        auto g = test::inc_shortest_path_graph();
        CHECK_EQ(g.a_nodes.size() + g.b_nodes.size(), graph::num_vertices(g));
        // Get vertices using graph_traits:
        using IndexMap = boost::inc_network_id_map;
        IndexMap index = get(boost::vertex_index, g);
        using vertex_iter = boost::graph_traits<IncNet>::vertex_iterator;
        std::pair<vertex_iter, vertex_iter> vp;
        std::cout << "vertices(g):\n";
        int exp_id = 0;
        for (vp = graph::vertices(g); vp.first != vp.second; ++vp.first) {
                Vertex v = *vp.first;
                std::cout << "\t";
                v->print();
                std::cout << "\n";
                // Verify index map:
                CHECK_EQ(exp_id++, index[v]);
        }
        std::cout << "\n";

        // Get out edges for each nodes
        std::cout << "out_edges(g):\n";
        using oedge_iter = boost::graph_traits<IncNet>::out_edge_iterator;
        vertex_iter vi, vi_end;
        oedge_iter oei, oei_end;
        for (boost::tie(vi, vi_end) = graph::vertices(g); vi != vi_end; ++vi){
            std::cout << "\t";
            Vertex v = *vi;
            CHECK_EQ(v->out_conns.size(), graph::out_degree(v,g));
            v->print();
            std::cout << ": ";
            for (boost::tie(oei, oei_end) = graph::out_edges(v,g); oei != oei_end; oei++){
                Vertex vt = graph::target(*oei,g);
                vt->print();
                std::cout << " ";
            }
            std::cout << "\n";
        }

        // Get in edges for each nodes
        std::cout << "int_edges(g):\n";
        using iedge_iter = boost::graph_traits<IncNet>::in_edge_iterator;
        iedge_iter iei, iei_end;
        for (boost::tie(vi, vi_end) = graph::vertices(g); vi != vi_end; ++vi){
            std::cout << "\t";
            Vertex v = *vi;
            CHECK_EQ(v->in_conns.size(), graph::in_degree(v,g));
            v->print();
            std::cout << ": ";
            for (boost::tie(iei, iei_end) = graph::in_edges(v,g); iei != iei_end; iei++){
                Vertex vt = graph::target(*iei,g);
                vt->print();
                std::cout << " ";
            }
            std::cout << "\n";
        }

        // Dijkstra's:
        std::vector<double> d(graph::num_vertices(g));
        // Use the first node as source:
        auto s = *(graph::vertices(g).first);
        auto distances = make_iterator_property_map(d.begin(),
                                                index);
        auto weight_map = boost::get(boost::edge_weight, g);
        // Invoke variant 2 of Dijkstra's algorithm
        boost::dijkstra_shortest_paths(g, s,
            boost::distance_map(distances)
            .weight_map(weight_map));

        std::cout << "distances from start node:\n";
        std::vector<double> exp_dist = {0, 9, 26, 7, 20, 11};
        for (vi = vertices(g).first; vi != vertices(g).second; ++vi){
                std::cout << "distance(" << index[*vi] << ") = "
                << distances[*vi] << "\n";
                CHECK_EQ(distances[*vi], doctest::Approx(exp_dist[index[*vi]]));
        }
    }
}
