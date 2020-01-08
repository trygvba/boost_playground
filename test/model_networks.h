#pragma once
#include "entities.h"

namespace graph = playground::graph;

namespace test {
    inline graph::network simple_network() {
        graph::network net;
        // Number of nodes:
        auto n = 10;
        for (auto i = 0; i < n; i++) {
            net.create_node(i, i + 0.5);
        }
        // Do some connections:
        net.connect(0, 1, 1.0)
            .connect(1, 2, 1.0)
            .connect(1, 3, 1.0)
            .connect(1, 4, 1.0)
            .connect(2, 5, 1.0)
            .connect(2, 6, 1.0)
            .connect(3, 7, 1.0)
            .connect(4, 8, 1.0)
            .connect(4, 9, 1.0);
        return std::move(net);
    }

    // Graph for shortest path algorithms:
    inline graph::network shortest_path_graph() {
        graph::network net;
        auto n = 6;
        for (auto i = 0; i < n; i++) {
            net.create_node(i, i + 0.5);
        }

        net.connect(0, 1, 7.0)
            .connect(1, 2, 10.)
            .connect(0, 2, 9.)
            .connect(0, 5, 14.)
            .connect(1, 3, 15.)
            .connect(2, 3, 11.)
            .connect(3, 4, 6.)
            .connect(2, 5, 2.)
            .connect(4, 5, 9.);

        return std::move(net);
    }

    // Incidence network for shortest path:
    inline graph::incidence_network inc_shortest_path_graph() {
        graph::incidence_network net;
        auto a0 = net.create_nodeA(0, 0.5);
        auto b0 = net.create_nodeB(0, 1.5);
        auto a1 = net.create_nodeA(1, 2.5);
        auto b1 = net.create_nodeB(1, 3.5);
        auto a2 = net.create_nodeA(2, 4.5);
        auto b2 = net.create_nodeB(2, 5.5);

        net.connect(a0, b0, 7.0)
            .connect(b0, a1, 10.)
            .connect(a0, a1, 9.)
            .connect(a0, b2, 14.)
            .connect(b0, b1, 15.)
            .connect(a1, b1, 11.)
            .connect(b1, a2, 6.)
            .connect(a1, b2, 2.)
            .connect(a2, b2, 9.);

        return std::move(net);
    }

}