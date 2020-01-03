#pragma once
#include "entities.h"

namespace graph = playground::graph;

namespace test {
    graph::network simple_network() {
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
    graph::network shortest_path_graph() {
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

}