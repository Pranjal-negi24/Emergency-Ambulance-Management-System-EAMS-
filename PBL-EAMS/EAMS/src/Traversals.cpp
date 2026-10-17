#include "Traversals.h"
#include "StackQueue.h"

void BFS::run(Graph &city, int start)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count)
    {
        cout << "Invalid location.\n";
        return;
    }
    SimpleQueue waiting;
    int visited[6] = {0};
    waiting.enqueue(start);
    visited[start] = 1;
    cout << "BFS: ";
    int current;
    while (waiting.dequeue(current))
    {
        cout << city.getPlace(current) << " ";
        for (int i = 0; i < count; i++)
        {
            if (city.getRoad(current, i) > 0 && visited[i] == 0)
            {
                waiting.enqueue(i);
                visited[i] = 1;
            }
        }
    }
    cout << endl;
}

void DFS::run(Graph &city, int start)
{
    int count = city.getPlaceCount();
    if (start < 0 || start >= count)
    {
        cout << "Invalid location.\n";
        return;
    }
    SimpleStack pending;
    int visited[6] = {0};
    pending.push(start);
    cout << "DFS: ";
    int current;
    while (pending.pop(current))
    {
        if (visited[current])
        {
            continue;
        }
        visited[current] = 1;
        cout << city.getPlace(current) << " ";
        for (int i = count - 1; i >= 0; i--)
        {
            if (city.getRoad(current, i) > 0 && visited[i] == 0)
            {
                pending.push(i);
            }
        }
    }
    cout << endl;
}
