#include "Dijkstra.h"

const int NO_ROUTE = 1000000;

// Fills distance[] and previous[] for every place, starting from "start"
void Dijkstra::calculate(Graph &city, int start, int distance[], int previous[])
{
    int count = city.getPlaceCount();
    int visited[6] = {0};
    for (int i = 0; i < count; i++)
    {
        distance[i] = NO_ROUTE;
        previous[i] = -1;
    }
    distance[start] = 0;
    for (int step = 0; step < count; step++)
    {
        int current = -1;
        for (int i = 0; i < count; i++)
        {
            if (!visited[i] && (current == -1 || distance[i] < distance[current]))
            {
                current = i;
            }
        }
        if (current == -1 || distance[current] == NO_ROUTE)
        {
            break;
        }
        visited[current] = 1;
        for (int i = 0; i < count; i++)
        {
            int road = city.getRoad(current, i);
            if (road > 0 && !visited[i] && distance[current] + road < distance[i])
            {
                distance[i] = distance[current] + road;
                previous[i] = current;
            }
        }
    }
}

int Dijkstra::getDistance(Graph &city, int start, int finish)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count || finish < 0 || finish >= count)
    {
        return -1;
    }
    int distance[6];
    int previous[6];
    calculate(city, start, distance, previous);
    if (distance[finish] == NO_ROUTE)
    {
        return -1;
    }
    return distance[finish];
}

void Dijkstra::shortestPath(Graph &city, int start, int finish)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count || finish < 0 || finish >= count)
    {
        cout << "Invalid location.\n";
        return;
    }
    int distance[6];
    int previous[6];
    calculate(city, start, distance, previous);
    if (distance[finish] == NO_ROUTE)
    {
        cout << "No route found.\n";
        return;
    }
    int path[6];
    int steps = 0;
    int current = finish;
    while (current != -1)
    {
        path[steps++] = current;
        current = previous[current];
    }
    cout << "Shortest route: ";
    for (int i = steps - 1; i >= 0; i--)
    {
        cout << city.getPlace(path[i]);
        if (i > 0)
        {
            cout << " -> ";
        }
    }
    cout << "\nTotal road distance: " << distance[finish] << " km (sample data)\n";
}
