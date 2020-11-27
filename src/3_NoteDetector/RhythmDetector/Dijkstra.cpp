#include "Dijkstra.hpp"
#include <unordered_map>
#include <vector>
#include <limits>
#include <string>
#include <algorithm>


// https://github.com/mburst/dijkstras-algorithm/blob/master/dijkstras.cpp


Dijkstra::Dijkstra() {
    // Constructor
}

Dijkstra::~Dijkstra() {
    // Destructor
}

void Dijkstra::add_vertex(const std::string name, const DijkstraEdges & edges/*={}*/) {
    this->vertices.insert(DijkstraVertices::value_type(name, edges));
}


void Dijkstra::add_edge(const std::string from, const std::string to, const double edge) {
    this->vertices[from].insert(DijkstraEdges::value_type(to, edge));
}


void Dijkstra::clear() {
    this->vertices.clear();
}


std::vector<std::string> Dijkstra::shortest_path(const std::string start, const std::string finish) {
    std::unordered_map<std::string, double> distances;
    std::unordered_map<std::string, std::string> previous;
    std::vector<std::string> nodes;
    std::vector<std::string> path;
    auto comparator = [&] (std::string left, std::string right) { return distances[left] > distances[right]; };
    for (auto& vertex : this->vertices) {
        if (vertex.first == start) {
            distances[vertex.first] = 0;
        }
        else {
            distances[vertex.first] = std::numeric_limits<double>::max();
        }
        nodes.push_back(vertex.first);
        push_heap(nodes.begin(), nodes.end(), comparator);
    }
    while (!nodes.empty()) {
        pop_heap(nodes.begin(), nodes.end(), comparator);
        std::string smallest = nodes.back();
        nodes.pop_back();
        if (smallest == finish) {
            while (previous.find(smallest) != previous.end()) {
                path.push_back(smallest);
                smallest = previous[smallest];
            }
            break;
        }
        if (distances[smallest] == std::numeric_limits<double>::max()) {
            break;
        }
        for (auto& neighbor : this->vertices[smallest]) {
            double alt = distances[smallest] + neighbor.second;
            if (alt < distances[neighbor.first]) {
                distances[neighbor.first] = alt;
                previous[neighbor.first] = smallest;
                make_heap(nodes.begin(), nodes.end(), comparator);
            }
        }
    }
    return path;
}
