#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "Graph.h"

// Dijkstra's shortest path on the road map
class Dijkstra
{
    void calculate(Graph &city, int start, int distance[], int previous[]);

public:
    int getDistance(Graph &city, int start, int finish); // -1 when there is no route
    void shortestPath(Graph &city, int start, int finish);
};

#endif
