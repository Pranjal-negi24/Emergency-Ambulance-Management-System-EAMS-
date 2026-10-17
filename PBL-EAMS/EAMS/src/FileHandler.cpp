#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Splits one CSV line at every comma and returns how many fields were found.
static int splitLine(string line, string fields[], int maxFields)
{
    int count = 0;
    string current = "";
    for (int i = 0; i < (int)line.length(); i++)
    {
        if (line[i] == '\r' || line[i] == '\n')
        {
            continue;
        }
        if (line[i] == ',')
        {
            if (count < maxFields)
            {
                fields[count] = current;
            }
            count++;
            current = "";
        }
        else
        {
            current += line[i];
        }
    }
    if (count < maxFields)
    {
        fields[count] = current;
    }
    count++;
    return count;
}

int loadPatients(string fileName, Patient list[], int maxCount, Graph &city)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << fileName << endl;
        return 0;
    }
    string line;
    string f[6];
    int count = 0;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitLine(line, f, 6) != 6)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[5]);
        if (place == -1 || count == maxCount)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        list[count] = Patient(atoi(f[0].c_str()), f[1], atoi(f[2].c_str()), f[3], atoi(f[4].c_str()), place);
        count++;
    }
    file.close();
    return count;
}

int loadAmbulances(string fileName, Ambulance list[], int maxCount, Graph &city)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << fileName << endl;
        return 0;
    }
    string line;
    string f[4];
    int count = 0;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitLine(line, f, 4) != 4)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[2]);
        if (place == -1 || count == maxCount)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        list[count] = Ambulance(atoi(f[0].c_str()), f[1], place, f[3] == "Available");
        count++;
    }
    file.close();
    return count;
}

int loadHospitals(string fileName, Hospital list[], int maxCount, Graph &city)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << fileName << endl;
        return 0;
    }
    string line;
    string f[6];
    int count = 0;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitLine(line, f, 6) != 6)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[2]);
        if (place == -1 || count == maxCount)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        list[count] = Hospital(atoi(f[0].c_str()), f[1], place, atoi(f[3].c_str()), atoi(f[4].c_str()), f[5]);
        count++;
    }
    file.close();
    return count;
}

bool savePatients(string fileName, Patient list[], int count, Graph &city)
{
    ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "id,name,age,condition,severity,location\n";
    for (int i = 0; i < count; i++)
    {
        file << list[i].getId() << "," << list[i].getName() << "," << list[i].getAge() << ","
             << list[i].getProblem() << "," << list[i].getSeverity() << ","
             << city.getPlace(list[i].getLocation()) << "\n";
    }
    file.close();
    return true;
}
