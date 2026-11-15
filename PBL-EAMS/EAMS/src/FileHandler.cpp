#include "FileHandler.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Splits one CSV line at every comma and returns how many fields were found.
int splitCsvLine(string line, string fields[], int maxFields)
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

int loadPatients(string fileName, DynamicArray<Patient> &list, Graph &city)
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
        if (splitCsvLine(line, f, 6) != 6)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[5]);
        if (place == -1)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        list.add(Patient(atoi(f[0].c_str()), f[1], atoi(f[2].c_str()), f[3], atoi(f[4].c_str()), place));
        count++;
    }
    file.close();
    return count;
}

int loadAmbulances(string fileName, DynamicArray<Ambulance> &list, Graph &city)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << fileName << endl;
        return 0;
    }
    string line;
    string f[11];
    int count = 0;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitCsvLine(line, f, 11) != 11)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[9]);
        int id = numberFromCode(f[0]);
        if (place == -1 || id == 0)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        // columns: ambulance_id, ambulance_number, ambulance_type, driver_id, driver_name, driver_phone,
        //          driver_license_no, paramedic_name, base_hospital_id, current_location_id, status
        Ambulance amb(id, f[4], place, f[10] == "Available");
        amb.setDetails(f[1], f[2], f[3], f[5], f[6], f[7], numberFromCode(f[8]));
        if (f[10] == "Under Maintenance")
        {
            amb.setMaintenance(true);
        }
        list.add(amb);
        count++;
    }
    file.close();
    return count;
}

int loadHospitals(string fileName, DynamicArray<Hospital> &list, Graph &city)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        cout << "Could not open " << fileName << endl;
        return 0;
    }
    string line;
    string f[10];
    int count = 0;
    getline(file, line);
    while (getline(file, line))
    {
        if (line.length() == 0 || line == "\r")
        {
            continue;
        }
        if (splitCsvLine(line, f, 10) != 10)
        {
            cout << "Skipping bad row in " << fileName << ": " << line << endl;
            continue;
        }
        int place = city.findPlace(f[4]);
        int id = numberFromCode(f[0]);
        if (place == -1 || id == 0)
        {
            cout << "Skipping row in " << fileName << ": " << line << endl;
            continue;
        }
        // columns: hospital_id, hospital_name, hospital_type, speciality, location_id, available_beds,
        //          total_beds, icu_beds_available, trauma_unit, emergency_phone
        Hospital h(id, f[1], place, atoi(f[5].c_str()), atoi(f[6].c_str()), f[9]);
        h.setDetails(f[2], f[3], atoi(f[7].c_str()), f[8]);
        list.add(h);
        count++;
    }
    file.close();
    return count;
}

bool savePatients(string fileName, DynamicArray<Patient> &list, Graph &city)
{
    ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "id,name,age,condition,severity,location_id\n";
    for (int i = 0; i < list.size(); i++)
    {
        file << list[i].getId() << "," << list[i].getName() << "," << list[i].getAge() << ","
             << list[i].getProblem() << "," << list[i].getSeverity() << ","
             << city.getPlaceId(list[i].getLocation()) << "\n";
    }
    file.close();
    return true;
}

bool saveAmbulances(string fileName, DynamicArray<Ambulance> &list, Graph &city)
{
    ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "ambulance_id,ambulance_number,ambulance_type,driver_id,driver_name,driver_phone,"
            "driver_license_no,paramedic_name,base_hospital_id,current_location_id,status\n";
    for (int i = 0; i < list.size(); i++)
    {
        file << list[i].getCode() << "," << list[i].getNumber() << "," << list[i].getType() << ","
             << list[i].getDriverCode() << "," << list[i].getDriver() << "," << list[i].getDriverPhone() << ","
             << list[i].getLicenseNo() << "," << list[i].getParamedic() << ","
             << makeCode("HOS", list[i].getBaseHospitalId()) << ","
             << city.getPlaceId(list[i].getLocation()) << "," << list[i].getStatusText() << "\n";
    }
    file.close();
    return true;
}

bool saveHospitals(string fileName, DynamicArray<Hospital> &list, Graph &city)
{
    ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }
    file << "hospital_id,hospital_name,hospital_type,speciality,location_id,available_beds,total_beds,"
            "icu_beds_available,trauma_unit,emergency_phone\n";
    for (int i = 0; i < list.size(); i++)
    {
        file << list[i].getCode() << "," << list[i].getName() << "," << list[i].getType() << ","
             << list[i].getSpeciality() << "," << city.getPlaceId(list[i].getLocation()) << ","
             << list[i].getBeds() << "," << list[i].getTotalBeds() << "," << list[i].getIcuBeds() << ","
             << list[i].getTraumaUnit() << "," << list[i].getPhone() << "\n";
    }
    file.close();
    return true;
}

void loadRecords(string fileName, RecordList &records)
{
    ifstream file(fileName.c_str());
    if (!file.is_open())
    {
        return; // no records saved yet (first run)
    }
    string line;
    while (getline(file, line))
    {
        if (line.length() > 0 && line[line.length() - 1] == '\r')
        {
            line.erase(line.length() - 1);
        }
        if (line.length() > 0)
        {
            records.addRecord(line);
        }
    }
    file.close();
}

bool saveRecords(string fileName, RecordList &records)
{
    ofstream file(fileName.c_str());
    if (!file.is_open())
    {
        return false;
    }
    for (int i = 0; i < records.size(); i++)
    {
        string text;
        records.getRecord(i, text);
        file << text << "\n";
    }
    file.close();
    return true;
}
