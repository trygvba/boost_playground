#include <doctest/doctest.h>
#include <memory>
#include <algorithm>
#include "entities.h"
#include "model_networks.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace boost {
    using playground::graph::network;

    struct network_traversal_category:
        public virtual boost::incidence_graph_tag,
        public virtual boost::vertex_list_graph_tag { };
    template<>
    struct graph_traits<playground::graph::network> {
        using vertex_descriptor = playground::graph::node_base_;
        using edge_descriptor = playground::graph::edge;

        // Iterator typedefs...
        using vertex_iterator = std::vector<vertex_descriptor>::iterator;
        using adjacency_iterator = vertex_iterator;
        using edge_iterator = std::vector<edge_descriptor>::iterator;
        using out_edge_iterator = edge_iterator;
        using in_edge_iterator = edge_iterator;

        using traversal_category = network_traversal_category; // cf. https://www.boost.org/doc/libs/1_71_0/libs/graph/doc/graph_traits.html
        using directed_category = boost::undirected_tag; // Directed graph or no...
        using edge_parallel_category = boost::allow_parallel_edge_tag;
        using vertices_size_type = unsigned long;
        using edges_size_type = unsigned long;
        using degree_size_type = unsigned long;
    };

    // VertexListGraph Concept:
    std::pair<graph_traits<network>::vertex_iterator, graph_traits<network>::vertex_iterator> vertices(network& net){
        return {net.nodes.begin(), net.nodes.end()};
    }

    graph_traits<network>::vertices_size_type num_vertices(network& net) {
        return net.nodes.size();
    }

    // EdgeListGraph concept:
    std::pair<graph_traits<network>::edge_iterator, graph_traits<network>::edge_iterator> edges(network& net){
        return {net.edges.begin(), net.edges.end()};
    }

    graph_traits<network>::edges_size_type num_edges(network& net){
        return net.edges.size();
    }

    graph_traits<network>::vertex_descriptor source(graph_traits<network>::edge_descriptor& e, network& g){
        return e.source;
    }

    graph_traits<network>::vertex_descriptor target(graph_traits<network>::edge_descriptor& e, network& g){
        return e.target;
    }

    // Incidence network concept:
    std::pair<graph_traits<network>::out_edge_iterator, graph_traits<network>::out_edge_iterator> out_edges(graph_traits<network>::vertex_descriptor v,
        network& g){
        // Get all edges that are connected to vertex:
        std::vector<graph_traits<network>::edge_descriptor> res;
        std::copy_if(g.edges.begin(), g.edges.end(), std::back_inserter(res),
            [&v, &g](auto el) {return v->id == source(el, g)->id; });
        return {res.begin(), res.end()};
    }

    graph_traits<network>::degree_size_type out_degree(graph_traits<network>::vertex_descriptor v, network& g){
        // Get all edges that are connected to vertex:
        std::vector<graph_traits<network>::edge_descriptor> res;
        std::copy_if(g.edges.begin(), g.edges.end(), std::back_inserter(res),
                     [&v, &g](auto el) {return v->id == source(el, g)->id; });
        return res.size();
    }

    // AdjacencyGraph Concept:
    std::pair<graph_traits<network>::adjacency_iterator, graph_traits<network>::adjacency_iterator> adjacent_vertices(
        graph_traits<network>::vertex_descriptor v,
        network& g
        ) {
        std::vector<graph_traits<network>::vertex_descriptor> res;
        graph_traits<network>::edge_iterator ei, ei_end;
        graph_traits<network>::edge_descriptor e;
        for (boost::tie(ei, ei_end) = edges(g);
            ei != ei_end ; ++ei){
            e = *ei;
            if (source(e,g)->id == v->id) {
                res.push_back(target(e,g));
            }
            else if (target(e,g)->id == v->id)
                res.push_back(source(e,g));
        }
        return {res.begin(), res.end()};
    }
    // Adjacency Structure:
    struct vertex_visitor {
        explicit vertex_visitor(network& net_): g(net_) {
        }

        using Vertex = graph_traits<network>::vertex_descriptor;
        void operator()(const Vertex& v) const {
            std::cout << "vertex[" << v->id << "]: ";
            graph_traits<network>::out_edge_iterator out_i, out_end;
            graph_traits<network>::edge_descriptor e;
            for (boost::tie(out_i, out_end) = out_edges(v, g);
                out_i != out_end; out_i++){
                e = *out_i;
                Vertex src = source(e, g), targ = target(e,g);
                std::cout << "(" << src->id << ", " << targ->id << ") ";
            }
            std::cout << "\n";
            std::cout << "adjacent vertices: ";
            graph_traits<network>::adjacency_iterator ai, ai_end;
            for (boost::tie(ai, ai_end) = boost::adjacent_vertices(v,g);
                ai != ai_end; ++ai) {
                std::cout << (*ai)->id << " ";
            }
            std::cout << "\n";
        }
        network& g;
    };
} // BOOST


TEST_SUITE("playground_boost_graph") {
    TEST_CASE("simple_shortest_path") {
        using playground::graph::network;
        auto net = test::shortest_path_graph();
        CHECK_EQ(net.nodes.size(), 6);
        CHECK_EQ(net.edges.size(), 9);
        CHECK_EQ(net.nodes.size(), boost::num_vertices(net));
        CHECK_EQ(net.edges.size(), boost::num_edges(net));

        typedef boost::graph_traits<network>::vertex_descriptor Vertex;

        //typedef boost::property_map::type IndexMap;

        // Get vertices using graph_traits:
        typedef boost::graph_traits<network>::vertex_iterator vertex_iter;
        std::pair<vertex_iter, vertex_iter> vp;
        std::cout << "vertices(g): ";
        for (vp = boost::vertices(net); vp.first != vp.second; ++vp.first) {
            Vertex v = *vp.first;
            std::cout << v->id << " ";
        }
        std::cout << "\n";

        // Access edges:
        std::cout << "edges(g): ";
        boost::graph_traits<network>::edge_iterator ei, ei_end;
        for(boost::tie(ei, ei_end) = boost::edges(net); ei != ei_end; ++ei) {
            std::cout << "(" << boost::source(*ei, net)->id << ", "
                << boost::target(*ei, net)->id << ") ";
        }
        std::cout << "\n";

        // Adjacency structure:
        std::for_each(boost::vertices(net).first, boost::vertices(net).second,
            boost::vertex_visitor(net));
        std::vector<int> exp_out = {3,2,2,1,1,0};
        for (int i=0; i<6; i++) {
            CHECK_EQ(exp_out[i], boost::out_degree(net.nodes[i], net));
        }

        // Dijkstra's:
        std::vector<float> d(boost::num_vertices(net));
        // Use the first node as source:
        auto s = *(boost::vertices(net).first);
        // Invoke variant 2 of Dijkstra's algorithm
        boost::dijkstra_shortest_paths(net, s, boost::distance_map(&d[0]));

   }
}