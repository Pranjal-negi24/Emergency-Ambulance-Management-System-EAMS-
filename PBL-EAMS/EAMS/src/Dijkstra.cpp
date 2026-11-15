#include "Dijkstra.h"
#include <iomanip>

const double NO_ROUTE = 1000000.0;

// Fills distance[] and previous[] for every place, starting from "start"
void Dijkstra::calculate(Graph &city, int start, double distance[], int previous[])
{
    int count = city.getPlaceCount();
    int *visited = new int[count];
    for (int i = 0; i < count; i++)
    {
        distance[i] = NO_ROUTE;
        previous[i] = -1;
        visited[i] = 0;
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
            double road = city.getRoad(current, i);
            if (road > 0 && !visited[i] && distance[current] + road < distance[i])
            {
                distance[i] = distance[current] + road;
                previous[i] = current;
            }
        }
    }
    delete[] visited;
}

double Dijkstra::getDistance(Graph &city, int start, int finish)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count || finish < 0 || finish >= count)
    {
        return -1;
    }
    double *distance = new double[count];
    int *previous = new int[count];
    calculate(city, start, distance, previous);
    double answer = distance[finish];
    delete[] distance;
    delete[] previous;
    if (answer == NO_ROUTE)
    {
        return -1;
    }
    return answer;
}

void Dijkstra::shortestPath(Graph &city, int start, int finish)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count || finish < 0 || finish >= count)
    {
        cout << "Invalid location.\n";
        return;
    }
    double *distance = new double[count];
    int *previous = new int[count];
    calculate(city, start, distance, previous);
    if (distance[finish] == NO_ROUTE)
    {
        cout << "No route found.\n";
        delete[] distance;
        delete[] previous;
        return;
    }
    int *path = new int[count];
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
    cout << fixed << setprecision(1);
    cout << "\nTotal road distance: " << distance[finish] << " km (from data/roads.csv)\n";
    cout.unsetf(ios::floatfield);
    cout << setprecision(6);
    delete[] distance;
    delete[] previous;
    delete[] path;
}
