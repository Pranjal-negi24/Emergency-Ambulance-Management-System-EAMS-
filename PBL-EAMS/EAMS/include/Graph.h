#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <string>
using namespace std;

// Road map only: places and road lengths. The algorithms live in
// Dijkstra.h and Traversals.h.
class Graph
{
    int roads[6][6];
    string places[6];

public:
    Graph();
    void showPlaces();
    string getPlace(int n);
    int findPlace(string text);
    int getPlaceCount();
    int getRoad(int from, int to); // road length in km, 0 when there is no road
};

#endif
