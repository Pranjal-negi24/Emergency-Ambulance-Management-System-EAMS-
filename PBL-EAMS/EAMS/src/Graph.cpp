#include "Graph.h"
#include "FileHandler.h"
#include <fstream>
#include <iomanip>
#include <cstdlib>
using namespace std;

Graph::Graph()
{
    roads = nullptr;
    count = 0;
    roadCount = 0;
}

Graph::~Graph()
{
    clearRoads();
}

void Graph::clearRoads()
{
    if (roads != nullptr)
    {
        for (int i = 0; i < count; i++)
        {
            delete[] roads[i];
        }
        delete[] roads;
        roads = nullptr;
    }
}

bool Graph::load(string locationsFile, string roadsFile)
{
    ifstream file(locationsFile.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << locationsFile << endl;
        return false;
    }
    string line;
    string f[7];
    getline(file, line); // header
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitCsvLine(line, f, 7) != 7)
        {
            cout << "Skipping bad row in " << locationsFile << ": " << line << endl;
            continue;
        }
        Location p;
        p.id = f[0];
        p.name = f[1];
        p.zone = f[2];
        p.type = f[3];
        p.latitude = atof(f[4].c_str());
        p.longitude = atof(f[5].c_str());
        p.pincode = f[6];
        places.add(p);
    }
    file.close();
    count = places.size();
    if (count == 0)
    {
        return false;
    }

    // empty road table
    roads = new double *[count];
    for (int i = 0; i < count; i++)
    {
        roads[i] = new double[count];
        for (int j = 0; j < count; j++)
        {
            roads[i][j] = 0;
        }
    }

    ifstream roadFile(roadsFile.c_str());
    if (!roadFile.is_open())
    {
        cout << "Could not open " << roadsFile << endl;
        return false;
    }
    string r[5];
    getline(roadFile, line); // header
    while (getline(roadFile, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitCsvLine(line, r, 5) != 5)
        {
            cout << "Skipping bad row in " << roadsFile << ": " << line << endl;
            continue;
        }
        int a = findPlace(r[1]);
        int b = findPlace(r[2]);
        double km = atof(r[3].c_str());
        if (a == -1 || b == -1 || a == b || km <= 0)
        {
            cout << "Skipping row in " << roadsFile << ": " << line << endl;
            continue;
        }
        if (roads[a][b] == 0)
        {
            roadCount++;
        }
        roads[a][b] = roads[b][a] = km; // roads can be used in both directions
    }
    roadFile.close();
    return true;
}

string Graph::getPlace(int n)
{
    if (n < 0 || n >= count)
    {
        return "Unknown";
    }
    return places[n].name;
}

string Graph::getPlaceId(int n)
{
    if (n < 0 || n >= count)
    {
        return "Unknown";
    }
    return places[n].id;
}

string Graph::getZone(int n)
{
    if (n < 0 || n >= count)
    {
        return "Unknown";
    }
    return places[n].zone;
}

int Graph::findPlace(string text)
{
    for (int i = 0; i < count; i++)
    {
        if (places[i].id == text || places[i].name == text)
        {
            return i;
        }
    }
    bool digits = text.length() > 0 && text.length() <= 6;
    for (int i = 0; digits && i < (int)text.length(); i++)
    {
        if (text[i] < '0' || text[i] > '9')
        {
            digits = false;
        }
    }
    if (digits)
    {
        int n = atoi(text.c_str());
        if (n >= 0 && n < count)
        {
            return n;
        }
    }
    return -1;
}

void Graph::showPlaces()
{
    cout << "Location list (" << count << " places):\n";
    for (int i = 0; i < count; i++)
    {
        cout << setw(2) << i << " - " << places[i].id << " - " << places[i].name
             << " (" << places[i].zone << ", " << places[i].type << ")" << endl;
    }
}

int Graph::getPlaceCount()
{
    return count;
}

int Graph::getRoadCount()
{
    return roadCount;
}

double Graph::getRoad(int from, int to)
{
    if (from < 0 || from >= count || to < 0 || to >= count)
    {
        return 0;
    }
    return roads[from][to];
}

void Graph::showRoads()
{
    cout << "Road network: " << count << " places, " << roadCount << " two-way roads (km):\n";
    cout << fixed << setprecision(1);
    for (int i = 0; i < count; i++)
    {
        cout << places[i].name << " ->";
        for (int j = 0; j < count; j++)
        {
            if (roads[i][j] > 0)
            {
                cout << " " << places[j].name << " (" << roads[i][j] << ")";
            }
        }
        cout << endl;
    }
    cout.unsetf(ios::floatfield);
    cout << setprecision(6);
}
