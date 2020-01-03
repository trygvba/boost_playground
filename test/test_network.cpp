//
// Created by Trygve Bærland on 2019-11-22.
//
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "entities.h"
#include <dlib/logger.h>
#include <memory>

namespace graph=playground::graph;
using std::dynamic_pointer_cast;
using std::make_shared;

// Set logging stuff:
auto graph::network::dlog = dlib::logger("network_logger");

TEST_SUITE("playground_graph") {
    TEST_CASE("network") {
        dlib::set_all_logging_levels(dlib::LALL);
        graph::network net;
        //net.dlog.set_level(dlib::LALL);
        CHECK_EQ(0, net.nodes.size());
        // Let's add a node
        auto a = net.create_node(1, 1.5);
        CHECK_EQ(1, net.nodes.size());
        CHECK_EQ(a->id, 1);
        CHECK_EQ(a->weight, doctest::Approx(1.5));

        // We add another node:
        auto b = net.create_node(2, 3.0);
        CHECK_EQ(2, net.nodes.size());

        // Try to add a non unique node:
        CHECK_THROWS_AS(net.add_node(b), std::runtime_error);
        // And we connect them:
        CHECK_EQ(net.edges.size(), 0);
        net.connect(1, 2, 5.5);
        CHECK_EQ(net.edges.size(), 1);
        CHECK_EQ(net.edges[0].weight, doctest::Approx(5.5));
        // Check invalid connects:
        CHECK_THROWS_AS(net.connect(1,1), std::runtime_error);
        CHECK_THROWS_AS(net.connect(3,2), std::runtime_error);
        CHECK_THROWS_AS(net.connect(1,4), std::runtime_error);

        // CHECK INVALID disconnect
        CHECK_THROWS_AS(net.disconnect(1,1), std::runtime_error);
        CHECK_THROWS_AS(net.disconnect(1,3), std::runtime_error);

        // CHECK disconnect:
        net.disconnect(1,2);
        CHECK_EQ(net.edges.size(), 0);
    }

    TEST_CASE("network_different_node_types") {
        graph::network net;
        // Let's create a node of type A:
        auto a = make_shared<graph::nodeA>(1,1.5);
        auto b = make_shared<graph::nodeB>(2,2.0);
        // Let's add them to the network:
        net.add_node(a).add_node(b).connect(1,2);
        CHECK_EQ(net.nodes.size(), 2);
        CHECK_EQ(net.edges.size(), 1);

        // Check that we can downcast:
        int acount = 0;
        int bcount = 0;
        for (auto& it : net.nodes) {
            auto ap = dynamic_pointer_cast<graph::nodeA>(it);
            if (ap) acount++;
            auto bp = dynamic_pointer_cast<graph::nodeB>(it);
            if (bp) bcount++;
        }
        CHECK_EQ(acount, 1);
        CHECK_EQ(bcount, 1);
    }
}
