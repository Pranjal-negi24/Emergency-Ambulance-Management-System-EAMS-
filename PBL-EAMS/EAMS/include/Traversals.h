#ifndef TRAVERSALS_H
#define TRAVERSALS_H
#include "Graph.h"

// Breadth first search (uses SimpleQueue)
class BFS
{
public:
    void run(Graph &city, int start);
};

// Depth first search (uses SimpleStack)
class DFS
{
public:
    void run(Graph &city, int start);
};

#endif
