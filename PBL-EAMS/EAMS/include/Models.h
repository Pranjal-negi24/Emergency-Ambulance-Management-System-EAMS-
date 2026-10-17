#ifndef MODELS_H
#define MODELS_H
#include <iostream>
#include <string>
using namespace std;

class Person
{
protected:
    int id;
    string name;
    int age;

public:
    Person()
    {
        id = 0;
        name = "";
        age = 0;
    }

    Person(int i, string n, int a)
    {
        id = i;
        name = n;
        age = a;
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }

    int getAge()
    {
        return age;
    }
};

class Patient : public Person
{
    string problem;
    int severity;
    int location;

public:
    Patient()
    {
        problem = "";
        severity = 1;
        location = 0;
    }

    Patient(int i, string n, int a, string p, int s, int l) : Person(i, n, a)
    {
        problem = p;
        severity = s;
        location = l;
    }

    string getProblem()
    {
        return problem;
    }

    int getSeverity()
    {
        return severity;
    }

    int getLocation()
    {
        return location;
    }

    void display()
    {
        cout << "ID: " << id << " | Name: " << name << " | Age: " << age
             << " | Problem: " << problem << " | Severity: " << severity
             << " | Location: " << location << endl;
    }
};

class Ambulance
{
    int id;
    string driver;
    int location;
    bool available;

public:
    Ambulance()
    {
        id = 0;
        driver = "";
        location = 0;
        available = true;
    }

    Ambulance(int i, string d, int l, bool a)
    {
        id = i;
        driver = d;
        location = l;
        available = a;
    }

    int getId()
    {
        return id;
    }

    string getDriver()
    {
        return driver;
    }

    int getLocation()
    {
        return location;
    }

    bool isAvailable()
    {
        return available;
    }

    void setAvailable(bool a)
    {
        available = a;
    }

    void setLocation(int l)
    {
        location = l;
    }

    void display()
    {
        cout << "ID: " << id << " | Driver: " << driver << " | Location: " << location
             << " | Status: " << (available ? "Available" : "Busy") << endl;
    }
};

class Hospital
{
    int id;
    string name;
    int location;
    int beds;
    int totalBeds;
    string phone;

public:
    Hospital()
    {
        id = 0;
        name = "";
        location = 0;
        beds = 0;
        totalBeds = 0;
        phone = "";
    }

    Hospital(int i, string n, int l, int b, int t, string p)
    {
        id = i;
        name = n;
        location = l;
        beds = b;
        totalBeds = t;
        phone = p;
    }

    int getId()
    {
        return id;
    }

    string getName()
    {
        return name;
    }

    int getLocation()
    {
        return location;
    }

    int getBeds()
    {
        return beds;
    }

    int getTotalBeds()
    {
        return totalBeds;
    }

    string getPhone()
    {
        return phone;
    }

    // Uses one bed when a patient is admitted. Returns false if no bed is free.
    bool admitPatient()
    {
        if (beds <= 0)
        {
            return false;
        }
        beds--;
        return true;
    }

    void display()
    {
        cout << "ID: " << id << " | Hospital: " << name << " | Location: " << location
             << " | Beds: " << beds << "/" << totalBeds
             << " | Phone: " << phone << endl;
    }
};

#endif
