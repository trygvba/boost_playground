//
// Created by Trygve Bærland on 2019-11-22.
//
#pragma once
#include <memory>
#include <vector>
#include <algorithm>
#include <boost/variant.hpp>
#include <dlib/logger.h>

namespace playground::graph {
    using std::shared_ptr;
    using std::weak_ptr;
    using std::make_shared;
    using std::vector;

    struct network;

    struct node_base {
        int id;
        double weight;
        network* net=nullptr;
        node_base(int aid, double aweight): id{aid}, weight{aweight} {}
        virtual ~node_base(){
            net=nullptr;
        }

        bool operator==(const node_base& other){
            return id == other.id;
        }

    };
    using node_base_ = shared_ptr<node_base>;

    struct nodeA: node_base {
        nodeA(int aid, double aweight): node_base(aid, aweight) {}
    };
    using nodeA_ = shared_ptr<nodeA>;

    struct nodeB: node_base {
        int num_connections = 0;
        nodeB(int aid, double aweight): node_base(aid, aweight) {}
    };
    using nodeB_ = shared_ptr<nodeB>;

    // We make a variant for node types:
    using node_type = boost::variant<nodeA_, nodeB_>;

    struct edge {
        node_base_ target;
        node_base_ source;
        double weight = 1.0;

        edge() = default;
        edge(const edge& o) = default;
        edge(const node_base_& asource) : source{asource}, target{nullptr}, weight{1.0} {}
        edge(node_base_& asource, node_base_& atarget, double aweight=1.0): source{asource}, target{atarget},
            weight{aweight} {}

        bool operator==(const edge& other) const {
            return *source == *(other.source) && *target == *(other.target);
        }

        bool operator!=(const edge& other) const {
            return not operator==(other);
        }
    };

    struct network {
        vector<node_base_> nodes;
        vector<edge> edges;
        static dlib::logger dlog;

        network(){
            dlog << dlib::LINFO << "Setting up an empty network";
        };
        network(network&) = default;
        network(network&&) = default;

        network& add_node(const node_base_& anode){
            anode->net = this;
            // Check that id is unique in network:
            auto it = std::find_if(nodes.begin(), nodes.end(), [&anode](auto el) {return el->id==anode->id;} );
            if (it != nodes.end()) {
                dlog << dlib::LERROR << "Unable to add node " << anode->id;
                throw std::runtime_error("node with ID " + std::to_string(anode->id) + " is already in network");
            }
            nodes.push_back(anode);
            dlog << dlib::LINFO << "Successfully added node " << anode->id;
            return *this;
        }

        node_base_ create_node(int id, double weight) {
            auto node = make_shared<node_base>(id, weight);
            auto it = std::find_if(nodes.begin(), nodes.end(), [id](auto el) {return el->id==id;} );
            if (it != nodes.end()) {
                dlog << dlib::LERROR << "Unable to create node " << id;
                throw std::runtime_error("node with ID " + std::to_string(id) + " is already in network");
            }
            dlog << dlib::LINFO << "Successfully created node " << id;
            nodes.push_back(node);
            return node;
        }

        network& connect(int source, int target, double weight=1.0){
            if (source == target) {
                dlog << dlib::LERROR << "Cannot connect a node to itself";
                throw std::runtime_error("Cannot connect a node to itself.");
            }
            auto its = std::find_if(nodes.begin(), nodes.end(), [source](auto el){return el->id==source;});
            if (its == nodes.end()) {
                dlog << dlib::LERROR << "Unable to connect source with ID " << source;
                throw std::runtime_error("Unable to connect source with ID " + std::to_string(source));
            }
            auto itt = std::find_if(nodes.begin(), nodes.end(), [target](auto el){return el->id==target;});
            if (itt == nodes.end()) {
                dlog << dlib::LERROR << "Unable to connect target with ID " << target;
                throw std::runtime_error("Unable to connect target with ID " + std::to_string(target));
            }
            edges.push_back(edge(*its, *itt, weight));
            dlog << dlib::LINFO << "Successfully connected " << source << " --> " << target;
            return *this;
        }

        network& disconnect(int source, int target){
            if (source == target){
                dlog << dlib::LERROR << "Cannot disconnect node " << source << " from itself";
                throw std::runtime_error("Cannot disconnect a node form itself.");
            }
            // Find edge to remove:
            auto it = std::find_if(edges.begin(), edges.end(),
                [source, target](auto el){
                    return el.source->id == source && el.target->id == target;
            });

            if (it == edges.end()) {
                std::stringstream ss;
                ss << "Unable to find edge to disconnect: ";
                ss << source << " --> " << target;
                dlog << dlib::LERROR << ss.str();
                throw std::runtime_error(ss.str());
            }
            edges.erase(it);
            return *this;
        }
        static void set_log_level(const dlib::log_level& new_level);
    };
}
