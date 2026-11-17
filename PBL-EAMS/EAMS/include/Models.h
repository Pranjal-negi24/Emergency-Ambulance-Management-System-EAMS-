#ifndef MODELS_H
#define MODELS_H
#include <iostream>
#include <string>
using namespace std;

// IDs in the CSV files look like AMB005 or HOS012. Inside the program they are plain numbers (5, 12).
inline int numberFromCode(string code)
{
    int n = 0;
    bool found = false;
    for (int i = 0; i < (int)code.length(); i++)
    {
        if (code[i] >= '0' && code[i] <= '9')
        {
            n = n * 10 + (code[i] - '0');
            found = true;
        }
    }
    return found ? n : 0;
}

inline string makeCode(string prefix, int n)
{
    string digits = to_string(n);
    while (digits.length() < 3)
    {
        digits = "0" + digits;
    }
    return prefix + digits;
}

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

// Where the patient is in the emergency process
enum PatientStatus
{
    REGISTERED = 0, // in the system, no emergency yet
    PENDING = 1,    // waiting in the emergency priority queue
    DISPATCHED = 2, // ambulance sent, waiting in the admission queue
    ADMITTED = 3,   // admitted in a hospital
    DISCHARGED = 4  // left the hospital
};

class Patient : public Person
{
    string problem;
    int severity;
    int location;
    int status;
    int hospitalId; // hospital where the patient is admitted (0 = none)

public:
    Patient()
    {
        problem = "";
        severity = 1;
        location = 0;
        status = REGISTERED;
        hospitalId = 0;
    }

    Patient(int i, string n, int a, string p, int s, int l) : Person(i, n, a)
    {
        problem = p;
        severity = s;
        location = l;
        status = REGISTERED;
        hospitalId = 0;
    }

    int getStatus()
    {
        return status;
    }

    void setStatus(int s)
    {
        status = s;
    }

    int getHospitalId()
    {
        return hospitalId;
    }

    void setHospitalId(int h)
    {
        hospitalId = h;
    }

    string getStatusText()
    {
        if (status == PENDING)
        {
            return "Waiting for ambulance";
        }
        if (status == DISPATCHED)
        {
            return "Ambulance dispatched";
        }
        if (status == ADMITTED)
        {
            return "Admitted";
        }
        if (status == DISCHARGED)
        {
            return "Discharged";
        }
        return "Registered";
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

    void display(string placeName)
    {
        cout << "ID: " << id << " | Name: " << name << " | Age: " << age
             << " | Problem: " << problem << " | Severity: " << severity
             << " | Location: " << placeName << " | Status: " << getStatusText() << endl;
    }
};

class Ambulance
{
    int id;
    string driver;
    int location;
    bool available;
    int destination; // place where the current trip ends (-1 = not known)
    bool maintenance;       // under maintenance: not available and not on a trip
    string number;          // vehicle registration number
    string type;            // BLS, ALS, Neonatal ...
    string driverCode;      // e.g. DRV005
    string driverPhone;
    string licenseNo;
    string paramedic;
    int baseHospitalId;     // hospital the ambulance belongs to (0 = none)

public:
    Ambulance()
    {
        id = 0;
        driver = "";
        location = 0;
        available = true;
        destination = -1;
        maintenance = false;
        baseHospitalId = 0;
    }

    Ambulance(int i, string d, int l, bool a)
    {
        id = i;
        driver = d;
        location = l;
        available = a;
        destination = -1;
        maintenance = false;
        baseHospitalId = 0;
    }

    void setDetails(string num, string t, string dCode, string phone, string license, string medic, int baseHospital)
    {
        number = num;
        type = t;
        driverCode = dCode;
        driverPhone = phone;
        licenseNo = license;
        paramedic = medic;
        baseHospitalId = baseHospital;
    }

    string getNumber() { return number; }
    string getType() { return type; }
    string getDriverCode() { return driverCode; }
    string getDriverPhone() { return driverPhone; }
    string getLicenseNo() { return licenseNo; }
    string getParamedic() { return paramedic; }
    int getBaseHospitalId() { return baseHospitalId; }
    string getCode() { return makeCode("AMB", id); }

    bool isUnderMaintenance()
    {
        return maintenance;
    }

    void setMaintenance(bool m)
    {
        maintenance = m;
        if (m)
        {
            available = false;
        }
    }

    string getStatusText()
    {
        if (maintenance)
        {
            return "Under Maintenance";
        }
        return available ? "Available" : "On Duty";
    }

    int getDestination()
    {
        return destination;
    }

    void setDestination(int d)
    {
        destination = d;
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

    void display(string placeName)
    {
        cout << "ID: " << getCode() << " | Vehicle: " << number << " | Type: " << type
             << " | Driver: " << driver << " (" << driverCode << ", " << driverPhone << ")"
             << " | Location: " << placeName << " | Status: " << getStatusText() << endl;
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
    string type;       // Government / Private / Trust
    string speciality; // General, Cardiac, Trauma Centre ...
    int icuBeds;
    string traumaUnit; // Yes / No

public:
    Hospital()
    {
        id = 0;
        name = "";
        location = 0;
        beds = 0;
        totalBeds = 0;
        phone = "";
        icuBeds = 0;
    }

    Hospital(int i, string n, int l, int b, int t, string p)
    {
        id = i;
        name = n;
        location = l;
        beds = b;
        totalBeds = t;
        phone = p;
        icuBeds = 0;
    }

    void setDetails(string t, string spec, int icu, string trauma)
    {
        type = t;
        speciality = spec;
        icuBeds = icu;
        traumaUnit = trauma;
    }

    string getType() { return type; }
    string getSpeciality() { return speciality; }
    int getIcuBeds() { return icuBeds; }
    string getTraumaUnit() { return traumaUnit; }
    string getCode() { return makeCode("HOS", id); }

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

    // Frees one bed when a patient leaves. Returns false if every bed is already free.
    bool dischargePatient()
    {
        if (beds >= totalBeds)
        {
            return false;
        }
        beds++;
        return true;
    }

    void display(string placeName)
    {
        cout << "ID: " << getCode() << " | Hospital: " << name << " (" << type << ", " << speciality << ")"
             << " | Location: " << placeName << " | Beds: " << beds << "/" << totalBeds
             << " | ICU: " << icuBeds << " | Trauma unit: " << traumaUnit
             << " | Phone: " << phone << endl;
    }
};

#endif
