#include <gtest/gtest.h>
#include "Dijkstra.hpp"


TEST(DijkstraTest, Basic) {
    Dijkstra graph;
    graph.add_vertex("START");
    graph.add_vertex("END");
    graph.add_vertex("A");
    graph.add_vertex("B");
    graph.add_edge("START", "A", 1.5);
    graph.add_edge("START", "B", 1.0);
    std::vector<std::string> path;
    std::vector<std::string> path_expected;
    // No path available
    path = graph.shortest_path("START", "END");
    EXPECT_TRUE(path.empty());
    // Single path available
    graph.add_edge("A", "END", 2.0);
    path = graph.shortest_path("START", "END");
    path_expected = {"END", "A"};
    EXPECT_EQ(path, path_expected);
    // Two path available
    graph.add_edge("B", "END", 2.0);
    path = graph.shortest_path("START", "END");
    path_expected = {"END", "B"};
    EXPECT_EQ(path, path_expected);
    // Negative weight
    graph.add_edge("A", "B", -5.0);
    path = graph.shortest_path("START", "END");
    path_expected = {"END", "B", "A"};
    EXPECT_EQ(path, path_expected);
}
