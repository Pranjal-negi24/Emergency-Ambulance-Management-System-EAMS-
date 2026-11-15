#ifndef DIJKSTRA_H
#define DIJKSTRA_H
#include "Graph.h"

// Dijkstra's shortest path on the road map
class Dijkstra
{
    void calculate(Graph &city, int start, double distance[], int previous[]);

public:
    double getDistance(Graph &city, int start, int finish); // km, -1 when there is no route
    void shortestPath(Graph &city, int start, int finish);
};

#endif
