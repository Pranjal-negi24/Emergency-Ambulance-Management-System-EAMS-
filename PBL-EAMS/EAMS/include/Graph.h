#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <string>
#include "DynamicArray.h"
using namespace std;

// One place on the map (one row of data/locations.csv)
struct Location
{
    string id;      // e.g. LOC001
    string name;    // e.g. Central Market
    string zone;
    string type;
    double latitude;
    double longitude;
    string pincode;
};

// Road map: places come from data/locations.csv and roads from data/roads.csv.
// The algorithms live in Dijkstra.h and Traversals.h.
class Graph
{
    DynamicArray<Location> places;
    double **roads; // road length in km between two places, 0 when there is no road
    int count;
    int roadCount;

    void clearRoads();

public:
    Graph();
    ~Graph();
    Graph(const Graph &) = delete;
    Graph &operator=(const Graph &) = delete;

    // Reads the places and then the roads. Returns false if a file is missing or has no places.
    bool load(string locationsFile, string roadsFile);

    void showPlaces();
    void showRoads(); // prints every place with the roads that leave it (adjacency list)
    string getPlace(int n);
    string getPlaceId(int n);
    string getZone(int n);
    int findPlace(string text); // accepts a location ID (LOC001), a place name or a list number
    int getPlaceCount();
    int getRoadCount();            // number of two-way roads
    double getRoad(int from, int to); // road length in km, 0 when there is no road
};

#endif
