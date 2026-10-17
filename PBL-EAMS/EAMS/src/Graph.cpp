#include "Graph.h"
#include <string>
using namespace std;

Graph::Graph()
{
    places[0] = "City Centre";
    places[1] = "Clock Tower";
    places[2] = "ISBT";
    places[3] = "Prem Nagar";
    places[4] = "Hospital Road";
    places[5] = "University";
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6; j++)
        {
            roads[i][j] = 0;
        }
    }
    roads[0][1] = roads[1][0] = 4;
    roads[0][2] = roads[2][0] = 6;
    roads[1][3] = roads[3][1] = 5;
    roads[1][4] = roads[4][1] = 7;
    roads[2][4] = roads[4][2] = 3;
    roads[3][5] = roads[5][3] = 4;
    roads[4][5] = roads[5][4] = 5;
    roads[2][5] = roads[5][2] = 9;
}

string Graph::getPlace(int n)
{
    if (n < 0 || n >= 6)
    {
        return "Unknown";
    }
    return places[n];
}

int Graph::findPlace(string text)
{
    for (int i = 0; i < 6; i++)
    {
        if (places[i] == text)
        {
            return i;
        }
    }
    if (text.length() == 1 && text[0] >= '0' && text[0] <= '5')
    {
        return text[0] - '0';
    }
    return -1;
}

void Graph::showPlaces()
{
    cout << "Location list:\n";
    for (int i = 0; i < 6; i++)
    {
        cout << i << " - " << places[i] << endl;
    }
}

int Graph::getPlaceCount()
{
    return 6;
}

int Graph::getRoad(int from, int to)
{
    if (from < 0 || from >= 6 || to < 0 || to >= 6)
    {
        return 0;
    }
    return roads[from][to];
}
