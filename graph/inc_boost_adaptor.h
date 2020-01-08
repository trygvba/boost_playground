#pragma once

#include "entities.h"
#include <utility> // std::pair
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/range/join.hpp>
#include <boost/variant.hpp>

// Graph traits, property maps & misc.
namespace boost {
    using IncNet = playground::graph::incidence_network;
    struct inc_net_traversal_category:
        public virtual vertex_list_graph_tag,
        public virtual incidence_graph_tag,
        public virtual bidirectional_graph_tag,
        public virtual adjacency_graph_tag
    { };

    template<>
    struct graph_traits<IncNet> {
        using traversal_category = inc_net_traversal_category;
        using directed_category = boost::directed_tag;

        using vertex_descriptor = playground::graph::node_base_;
        using edge_descriptor = std::pair<playground::graph::node_base_, playground::graph::connection>;

        using edge_parallel_category = allow_parallel_edge_tag;
        using vertices_size_type = size_t;
        using degree_size_type = size_t;
        using edges_size_type = size_t;
        /* Iterator classes */
        class vertex_iterator:
            public iterator_facade<vertex_iterator,
                playground::graph::node_base_,
                bidirectional_traversal_tag,
                playground::graph::node_base_>{
        public:
            vertex_iterator(playground::graph::node_base_ node=nullptr, const IncNet* g=nullptr): base(node), g(g) {
                if (node && g) {
                    auto anode = std::dynamic_pointer_cast<playground::graph::nodeA>(node);
                    if (anode) {
                        ait = std::find_if(g->a_nodes.begin(), g->a_nodes.end(),
                                           [&node](auto el) { return *el == *node; });
                    } else ait = g->a_nodes.end();
                    auto bnode = std::dynamic_pointer_cast<playground::graph::nodeB>(node);
                    if (bnode) {
                        auto bit = std::find_if(g->b_nodes.begin(), g->b_nodes.end(),
                                                [&node](auto el) { return *el == *node; });
                    } else bit = g->b_nodes.end();
                }
            }

        private:
            const playground::graph::node_base_ dereference() const { return base; }

            bool equal(const vertex_iterator& other) const {
                if (!(other.base) && !(base)) return true;
                if (!(other.base) || !(base)) return false;
                return *base == *(other.base);
            }

            void base_from_iterator() {
                if (ait != g->a_nodes.end()) {
                    base = *ait;
                    return;
                }
                if (bit != g->b_nodes.end()) {
                    base = *bit;
                    return;
                }
                base = nullptr;
            }
            void increment() {
                if (ait != g->a_nodes.end()){
                    ++ait;
                    if (ait == g->a_nodes.end()) bit = g->b_nodes.begin();
                } else if (bit != g->b_nodes.end()) {
                    ++bit;
                }
                base_from_iterator();
            }
            void decrement() {
                if (bit != g->b_nodes.rend().base()) {
                    --bit;
                    if (bit == g->b_nodes.rend().base()) ait = g->a_nodes.rbegin().base();
                } else if (ait != g->a_nodes.rend().base()) {
                    --ait;
                }
                base_from_iterator();
            }
            // Iterator members:
            playground::graph::node_base_ base;
            std::vector<playground::graph::nodeA_>::const_iterator ait;
            std::vector<playground::graph::nodeB_>::const_iterator bit;
            // Maybe keep reference to corresponding iterator, and container??
            const IncNet* g;

            friend class iterator_core_access;
        };

        class out_edge_iterator:
            public iterator_facade<out_edge_iterator,
                std::pair<playground::graph::node_base_, playground::graph::connection>,
                bidirectional_traversal_tag,
                std::pair<playground::graph::node_base_, playground::graph::connection>,
                std::ptrdiff_t >{
        public:
            out_edge_iterator(std::vector<playground::graph::connection>::const_iterator an_iter = std::vector<playground::graph::connection>::const_iterator{} ,
                playground::graph::node_base_ asource=nullptr): source(asource), iter(an_iter) {
            }

            virtual ~out_edge_iterator() {}


        private:
            const std::pair<playground::graph::node_base_, playground::graph::connection> dereference() const {
                if (source) {
                    if (iter != source->out_conns.end())
                        return std::make_pair(source, *iter);
                }
                return std::pair<playground::graph::node_base_, playground::graph::connection>{};
            }

            bool equal(const out_edge_iterator& other) const {
                if (!source || !other.source) return false; // One of the source isn't set.
                // One of the iterators doesn't point to anything
                if (iter == source->out_conns.end() &&
                    other.iter == other.source->out_conns.end()) return true;
                if (iter == source->out_conns.end() ||
                    other.iter == other.source->out_conns.end()) return false;
                return *source == *(other.source) &&
                    *iter == *(other.iter);
            }

            void increment() { ++iter; }
            void decrement() { --iter; }

            playground::graph::node_base_ source;
            std::vector<playground::graph::connection>::const_iterator iter;

            friend class iterator_core_access;
        };

        class in_edge_iterator:
            public iterator_facade<in_edge_iterator,
                std::pair<playground::graph::node_base_, playground::graph::connection>,
                bidirectional_traversal_tag,
                std::pair<playground::graph::node_base_, playground::graph::connection>>{
        public:
            in_edge_iterator(std::vector<playground::graph::connection>::const_iterator an_iter = std::vector<playground::graph::connection>::const_iterator{} ,
                playground::graph::node_base_ asource=nullptr): source(asource), iter(an_iter) {
            }


        private:
            const std::pair<playground::graph::node_base_, playground::graph::connection> dereference() const {
                if (source) {
                    if (iter != source->in_conns.end())
                        return std::make_pair(source, *iter);
                }
                return std::pair<playground::graph::node_base_, playground::graph::connection>{};
            }

            bool equal(const in_edge_iterator& other) const {
                if (!source || !other.source) return false; // One of the source isn't set.
                // One of the iterators doesn't point to anything
                if (iter == source->in_conns.end() &&
                    other.iter == other.source->in_conns.end()) return true;
                if (iter == source->in_conns.end() ||
                    other.iter == other.source->in_conns.end()) return false;
                return *source == *(other.source) &&
                    *iter == *(other.iter);
            }

            void increment() { ++iter; }
            void decrement() { --iter; }

            playground::graph::node_base_ source;
            std::vector<playground::graph::connection>::const_iterator iter;

            friend class iterator_core_access;
        };

        using adjacency_iterator = boost::adjacency_iterator_generator<IncNet, vertex_descriptor, out_edge_iterator>::type;
    };

    /**
     *  Property maps:
     */
    class inc_network_id_map: public put_get_helper<int, inc_network_id_map>
    {
    public:
        using category = boost::readable_property_map_tag;
        using value_type = int;
        using reference = int;
        using key_type = graph_traits<network>::vertex_descriptor;

        inc_network_id_map(const IncNet* ag): g(ag) {
            offset = g->a_nodes.size();
        };

        value_type operator[](const graph_traits<IncNet>::vertex_descriptor& x) const {
            auto anode = std::dynamic_pointer_cast<playground::graph::nodeA>(x);
            if (anode) {
                auto it = std::find_if(g->a_nodes.begin(), g->a_nodes.end(),
                    [&x](auto el) { return *x == *el; });
                if (it != g->a_nodes.end()) return it - g->a_nodes.begin();
            }
            auto bnode = std::dynamic_pointer_cast<playground::graph::nodeB>(x);
            if (bnode) {
                auto it = std::find_if(g->b_nodes.begin(), g->b_nodes.end(),
                    [&x](auto el) { return *x == *el; });
                if (it != g->b_nodes.end()) return it - g->b_nodes.begin() + offset;
            }
            throw std::runtime_error("Unable to calculate index for vertex.");
        }
    private:
        const IncNet* g;
        int offset;
    };

    inline inc_network_id_map get(vertex_index_t, const IncNet& g) {
        return inc_network_id_map(&g);
    }

    // Though not strictly needed it provides a decent default:
    template<>
    struct property_map<IncNet, vertex_index_t> {
        using type = inc_network_id_map;
        using const_type = inc_network_id_map;
    };

    // Edge weight map:
    class inc_edge_weight_map: public put_get_helper<double, inc_edge_weight_map> {
    public:
        using category = readable_property_map_tag;
        using value_type = double;
        using reference = double;
        using key_type = graph_traits<IncNet>::edge_descriptor;

        inc_edge_weight_map() = default;

        template<class T>
        value_type operator[](T edge) const {
            return edge.second.weight;
        }
    };

    inline inc_edge_weight_map get(edge_weight_t, const IncNet& g) {
        return inc_edge_weight_map();
    }
}

// Function overloads used by boost.Graph
namespace playground::graph {
    using IncNet = incidence_network;

    // Functions needed for boost::VertexListGraphConcept
    boost::graph_traits<IncNet>::vertices_size_type num_vertices(const IncNet& g){
        return g.a_nodes.size() + g.b_nodes.size();
    }

    std::pair<boost::graph_traits<IncNet>::vertex_iterator, boost::graph_traits<IncNet>::vertex_iterator>
        vertices(const IncNet& g){
        using Iter = boost::graph_traits<IncNet>::vertex_iterator;
        return std::make_pair( Iter(g.a_nodes[0], &g), Iter(nullptr, &g) );
    }

    // Function needed for boost::IncidenceGraphConcept
    boost::graph_traits<IncNet>::vertex_descriptor source(const boost::graph_traits<IncNet>::edge_descriptor& edge,
                                                          const IncNet& g){
        return edge.first;
    }

    boost::graph_traits<IncNet>::vertex_descriptor target(const boost::graph_traits<IncNet>::edge_descriptor& edge,
                                                          const IncNet& g){
        return edge.second.target;
    }

    boost::graph_traits<IncNet>::degree_size_type out_degree(const boost::graph_traits<IncNet>::vertex_descriptor& u,
                                                             const IncNet& g){
        return u->out_conns.size();
    }

    std::pair<boost::graph_traits<IncNet>::out_edge_iterator, boost::graph_traits<IncNet>::out_edge_iterator>
        out_edges(const boost::graph_traits<IncNet>::vertex_descriptor& u, const IncNet& g){
        using Iter = boost::graph_traits<IncNet>::out_edge_iterator;
        return std::make_pair(Iter(u->out_conns.begin(), u), Iter(u->out_conns.end(), u));
    }

    //Functions for BidirectionalGraphConcept:
    std::pair<boost::graph_traits<IncNet>::in_edge_iterator, boost::graph_traits<IncNet>::in_edge_iterator>
        in_edges(const boost::graph_traits<IncNet>::vertex_descriptor& u, const IncNet& g){
        using Iter = boost::graph_traits<IncNet>::in_edge_iterator;
        return std::make_pair( Iter(u->in_conns.begin(), u), Iter(u->in_conns.end(), u));
    }

    boost::graph_traits<IncNet>::degree_size_type in_degree(const boost::graph_traits<IncNet>::vertex_descriptor& u,
                                                            const IncNet& g) {
        return u->in_conns.size();
    }

    boost::graph_traits<IncNet>::degree_size_type degree(const boost::graph_traits<IncNet>::vertex_descriptor& u,
                                                         const IncNet& g) {
        return out_degree(u,g) + in_degree(u,g);
    }

    // Functions for adjacencyGraphConcept:
    std::pair<boost::graph_traits<IncNet>::adjacency_iterator, boost::graph_traits<IncNet>::adjacency_iterator>
        adjacent_vertices(const boost::graph_traits<IncNet>::vertex_descriptor& u,
                          const IncNet& g){
        using EdgeIter = boost::graph_traits<IncNet>::out_edge_iterator;
        using AdjIter = boost::graph_traits<IncNet>::adjacency_iterator;
        auto edge_iters = out_edges(u,g);
        return std::make_pair(AdjIter(edge_iters.first, &g), AdjIter(edge_iters.second, &g));
    }
}
