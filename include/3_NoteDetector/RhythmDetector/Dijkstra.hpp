#ifndef DIJKSTRA
#define DIJKSTRA

#include <vector>
#include <string>
#include <unordered_map>


typedef std::unordered_map<std::string, double> DijkstraEdges;
typedef std::unordered_map<std::string, DijkstraEdges> DijkstraVertices;


class Dijkstra {
public:
    // Constructors & Destructor
    Dijkstra();
    virtual ~Dijkstra();
    // Methods
    std::vector<std::string> shortest_path(const std::string start, const std::string finish);
    void add_vertex(const std::string name, const DijkstraEdges & edges={});
    void add_edge(const std::string from, const std::string to, const double edge);
    void clear();
private:
    DijkstraVertices vertices;
};


#endif /* DIJKSTRA */
