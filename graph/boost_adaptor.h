#pragma once

#include "entities.h"
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

namespace boost {
    using playground::graph::network;
    struct network_traversal_category :
        public virtual boost::adjacency_graph_tag,
        public virtual boost::incidence_graph_tag,
        public virtual boost::vertex_list_graph_tag {
    };

    template<>
    struct graph_traits<playground::graph::network> {
        using vertex_descriptor = playground::graph::node_base_;
        using edge_descriptor = playground::graph::edge;

        // Iterator typedefs...
        using vertex_iterator = std::vector<vertex_descriptor>::const_iterator;
        using adjacency_iterator = vertex_iterator;
        using edge_iterator = std::vector<edge_descriptor>::iterator;
        using out_edge_iterator = std::vector<edge_descriptor>::const_iterator;
        using in_edge_iterator = std::vector<edge_descriptor>::const_iterator;

        using traversal_category = network_traversal_category; // cf. https://www.boost.org/doc/libs/1_71_0/libs/graph/doc/graph_traits.html
        using directed_category = boost::undirected_tag; // Directed graph or no...
        using edge_parallel_category = boost::allow_parallel_edge_tag;
        using vertices_size_type = unsigned long;
        using edges_size_type = unsigned long;
        using degree_size_type = unsigned long;
    };

    // PropertyMaps:
    // We inherit from boost::put_get_helper here so that the correct overloads of
    // boost::get(...) and boost::put(...) are called.
    class network_id_map: public boost::put_get_helper<int, network_id_map>
    {
    public:
        using category = boost::readable_property_map_tag;
        using value_type = int;
        using reference = int;
        using key_type = graph_traits<network>::vertex_descriptor;

        network_id_map() = default;

        template<class T>
        value_type operator[](T x) const {return x->id;}
    };

    inline network_id_map get(vertex_index_t, const network& g) {
        return network_id_map();
    }

    template<>
    struct property_map<network, vertex_index_t> {
        using type = network_id_map;
        using const_type = network_id_map;
    };

    class network_edge_weight_map: public boost::put_get_helper<double, network_edge_weight_map>
    {
    public:
        using category = boost::lvalue_property_map_tag;
        using value_type = double;
        using reference = double;
        using key_type = graph_traits<network>::edge_descriptor;

        network_edge_weight_map() = default;
        template<class T>
        value_type operator[](T x) const { return x.weight; }
    };

    inline network_edge_weight_map get(edge_weight_t, const network& g) {
        return network_edge_weight_map();
    }
    template<>
    class property_map<network, edge_index_t>{
        using type = network_edge_weight_map;
        using const_type = network_edge_weight_map;
    };

    template<>
    struct property_map<network, edge_weight_t>{
        using type = network_edge_weight_map;
        using const_type = network_edge_weight_map;
    };
} // BOOST

namespace playground::graph {
    using namespace boost;
    // VertexListGraph Concept:
    std::pair<graph_traits<network>::vertex_iterator, graph_traits<network>::vertex_iterator> vertices(const network& net){
        return {net.nodes.begin(), net.nodes.end()};
    }

    graph_traits<network>::vertices_size_type num_vertices(const network& net) {
        return net.nodes.size();
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

    graph_traits<network>::vertex_descriptor source(const graph_traits<network>::edge_descriptor& e, const network& g){
        return e.source;
    }

    graph_traits<network>::vertex_descriptor target(const graph_traits<network>::edge_descriptor& e, const network& g){
        return e.target;
    }

    // Incidence network concept:
    std::pair<graph_traits<network>::out_edge_iterator, graph_traits<network>::out_edge_iterator> out_edges(const graph_traits<network>::vertex_descriptor v,
        const network& g){
        // Get all edges that are connected to vertex:
        std::vector<graph_traits<network>::edge_descriptor> res;
        std::copy_if(g.edges.begin(), g.edges.end(), std::back_inserter(res),
            [&v, &g](auto el) {return v->id == source(el, g)->id; });
        return {res.begin(), res.end()};
    }

    graph_traits<network>::degree_size_type out_degree(const graph_traits<network>::vertex_descriptor v, const network& g){
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
            for (boost::tie(ai, ai_end) = adjacent_vertices(v,g);
                ai != ai_end; ++ai) {
                std::cout << (*ai)->id << " ";
            }
            std::cout << "\n";
        }
        network& g;
    };
}
