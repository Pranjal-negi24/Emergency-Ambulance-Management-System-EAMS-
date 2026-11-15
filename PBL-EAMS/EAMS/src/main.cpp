#include <iostream>
#include <fstream>
#include <string>
#include "Models.h"
#include "DynamicArray.h"
#include "HashTable.h"
#include "StackQueue.h"
#include "PriorityQueueLinkedList.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Traversals.h"
#include "FileHandler.h"
#include "Input.h"
#include "Demo.h"
using namespace std;

// One entry of the admitted-patients history stack
struct AdmissionEntry
{
    int patientId;
    int hospitalId;
};

// Everything the system keeps in memory
struct Eams
{
    Graph city;
    Dijkstra dijkstra;
    BFS bfs;
    DFS dfs;

    DynamicArray<Patient> patients;       // dynamic array
    DynamicArray<Ambulance> ambulances;   // dynamic array
    DynamicArray<Hospital> hospitals;     // dynamic array
    HashTable patientIndex;               // hash table: patient ID -> position in the patient array
    EmergencyQueue emergencyQueue;        // priority queue (max-heap): pending emergencies
    SimpleQueue<int> admissionQueue;      // queue: dispatched patients waiting for admission
    SimpleStack<AdmissionEntry> admittedHistory; // stack: admitted patients, latest on top
    SimpleStack<int> departedHistory;     // stack: ambulances departed, latest on top
    SimpleLinkedList activeTrips;         // linked list: ambulances that are on a trip
    RecordList records;                   // linked list: records of everything that happens

    int nextPatientId;
    bool saveWarningShown;
};

// ---------------------------------------------------------------- lookups

static int findPatient(Eams &e, int id)
{
    int index;
    if (e.patientIndex.find(id, index))
    {
        return index;
    }
    return -1;
}

static int findAmbulance(Eams &e, int id)
{
    for (int i = 0; i < e.ambulances.size(); i++)
    {
        if (e.ambulances[i].getId() == id)
        {
            return i;
        }
    }
    return -1;
}

static int findHospital(Eams &e, int id)
{
    for (int i = 0; i < e.hospitals.size(); i++)
    {
        if (e.hospitals[i].getId() == id)
        {
            return i;
        }
    }
    return -1;
}

// Nearest hospital (by road distance) that has a free bed, or -1
static int nearestHospitalWithBed(Eams &e, int location)
{
    int best = -1;
    double bestDistance = 1000000.0;
    for (int i = 0; i < e.hospitals.size(); i++)
    {
        double distance = e.dijkstra.getDistance(e.city, location, e.hospitals[i].getLocation());
        if (e.hospitals[i].getBeds() > 0 && distance >= 0 && distance < bestDistance)
        {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

// Nearest available ambulance (by road distance), or -1
static int nearestAvailableAmbulance(Eams &e, int location)
{
    int best = -1;
    double bestDistance = 1000000.0;
    for (int i = 0; i < e.ambulances.size(); i++)
    {
        if (e.ambulances[i].isAvailable())
        {
            double distance = e.dijkstra.getDistance(e.city, e.ambulances[i].getLocation(), location);
            if (distance >= 0 && distance < bestDistance)
            {
                bestDistance = distance;
                best = i;
            }
        }
    }
    return best;
}

// ---------------------------------------------------------------- saving and loading

static bool saveState(Eams &e)
{
    ofstream file("data/state.csv");
    if (!file.is_open())
    {
        return false;
    }
    file << "type,value1,value2\n";
    for (int k = 0; k < e.admittedHistory.size(); k++)
    {
        AdmissionEntry entry;
        e.admittedHistory.peekFromBottom(k, entry);
        file << "admitted," << entry.patientId << "," << entry.hospitalId << "\n";
    }
    for (int i = 0; i < e.patients.size(); i++)
    {
        if (e.patients[i].getStatus() == DISCHARGED)
        {
            file << "discharged," << e.patients[i].getId() << ",0\n";
        }
    }
    int pendingCount = e.emergencyQueue.size();
    int *ids = new int[pendingCount + 1];
    int *levels = new int[pendingCount + 1];
    e.emergencyQueue.snapshot(ids, levels, pendingCount);
    for (int i = 0; i < pendingCount; i++)
    {
        file << "pending," << ids[i] << ",0\n";
    }
    delete[] ids;
    delete[] levels;
    for (int k = 0; k < e.admissionQueue.size(); k++)
    {
        int id;
        e.admissionQueue.peekAt(k, id);
        file << "waiting," << id << ",0\n";
    }
    for (int k = 0; k < e.departedHistory.size(); k++)
    {
        int id;
        e.departedHistory.peekFromBottom(k, id);
        file << "departed," << id << ",0\n";
    }
    for (int k = 0; k < e.activeTrips.size(); k++)
    {
        int id;
        e.activeTrips.getAt(k, id);
        int a = findAmbulance(e, id);
        if (a != -1)
        {
            file << "trip," << id << "," << e.ambulances[a].getDestination() << "\n";
        }
    }
    file.close();
    return true;
}

static void saveAll(Eams &e)
{
    bool ok = savePatients("data/patients.csv", e.patients, e.city);
    ok = saveAmbulances("data/ambulances.csv", e.ambulances, e.city) && ok;
    ok = saveHospitals("data/hospitals.csv", e.hospitals, e.city) && ok;
    ok = saveRecords("data/records.txt", e.records) && ok;
    ok = saveState(e) && ok;
    if (!ok && !e.saveWarningShown)
    {
        cout << "Warning: could not save the data files. Run the program from the EAMS folder.\n";
        e.saveWarningShown = true;
    }
}

static void loadState(Eams &e)
{
    ifstream file("data/state.csv");
    if (!file.is_open())
    {
        return; // first run: nothing saved yet
    }
    string line;
    string f[3];
    getline(file, line); // header
    while (getline(file, line))
    {
        if (splitCsvLine(line, f, 3) != 3)
        {
            continue;
        }
        int a = atoi(f[1].c_str());
        int b = atoi(f[2].c_str());
        if (f[0] == "admitted")
        {
            AdmissionEntry entry;
            entry.patientId = a;
            entry.hospitalId = b;
            e.admittedHistory.push(entry);
            int p = findPatient(e, a);
            if (p != -1)
            {
                e.patients[p].setStatus(ADMITTED);
                e.patients[p].setHospitalId(b);
            }
        }
        else if (f[0] == "discharged")
        {
            int p = findPatient(e, a);
            if (p != -1)
            {
                e.patients[p].setStatus(DISCHARGED);
            }
        }
        else if (f[0] == "pending")
        {
            int p = findPatient(e, a);
            if (p != -1)
            {
                e.patients[p].setStatus(PENDING);
                e.emergencyQueue.add(a, e.patients[p].getSeverity());
            }
        }
        else if (f[0] == "waiting")
        {
            int p = findPatient(e, a);
            if (p != -1)
            {
                e.patients[p].setStatus(DISPATCHED);
                e.admissionQueue.enqueue(a);
            }
        }
        else if (f[0] == "departed")
        {
            e.departedHistory.push(a);
        }
        else if (f[0] == "trip")
        {
            int amb = findAmbulance(e, a);
            if (amb != -1 && !e.ambulances[amb].isAvailable())
            {
                e.ambulances[amb].setDestination(b);
                e.activeTrips.insertEnd(a);
            }
        }
    }
    file.close();
}

static bool loadAll(Eams &e)
{
    // the map comes first: every other file refers to its locations
    if (!e.city.load("data/locations.csv", "data/roads.csv"))
    {
        cout << "Could not load the map (data/locations.csv and data/roads.csv). Run the program from the EAMS folder.\n";
        return false;
    }
    loadPatients("data/patients.csv", e.patients, e.city);
    loadAmbulances("data/ambulances.csv", e.ambulances, e.city);
    loadHospitals("data/hospitals.csv", e.hospitals, e.city);
    e.nextPatientId = 1;
    for (int i = 0; i < e.patients.size(); i++)
    {
        e.patientIndex.insert(e.patients[i].getId(), i);
        if (e.patients[i].getId() >= e.nextPatientId)
        {
            e.nextPatientId = e.patients[i].getId() + 1;
        }
    }
    loadRecords("data/records.txt", e.records);
    loadState(e);
    // a busy ambulance without a saved trip still counts as being on a trip
    for (int i = 0; i < e.ambulances.size(); i++)
    {
        if (!e.ambulances[i].isAvailable() && !e.ambulances[i].isUnderMaintenance() &&
            !e.activeTrips.contains(e.ambulances[i].getId()))
        {
            e.activeTrips.insertEnd(e.ambulances[i].getId());
        }
    }
    return true;
}

// ---------------------------------------------------------------- shared steps

static bool readPatientDetails(Eams &e, string &name, int &age, string &problem, int &severity, int &location)
{
    if (!readText("Enter patient name: ", name))
    {
        return false;
    }
    if (!readInt("Enter age (0 to 120): ", 0, 120, age))
    {
        return false;
    }
    if (!readText("Enter problem: ", problem))
    {
        return false;
    }
    e.city.showPlaces();
    if (!readInt("Enter location number (0 to " + to_string(e.city.getPlaceCount() - 1) + "): ", 0, e.city.getPlaceCount() - 1, location))
    {
        return false;
    }
    return readInt("Enter severity (1 low to 5 critical): ", 1, 5, severity);
}

// Adds a new patient to the dynamic array and the hash table; returns the array position
static int registerPatient(Eams &e, string name, int age, string problem, int severity, int location)
{
    int id = e.nextPatientId;
    e.nextPatientId++;
    e.patients.add(Patient(id, name, age, problem, severity, location));
    int index = e.patients.size() - 1;
    e.patientIndex.insert(id, index);
    return index;
}

// Puts the patient in the hospital: uses a bed, pushes the history stack, adds a record
static void admitToHospital(Eams &e, int p, int h)
{
    e.hospitals[h].admitPatient();
    e.patients[p].setStatus(ADMITTED);
    e.patients[p].setHospitalId(e.hospitals[h].getId());
    AdmissionEntry entry;
    entry.patientId = e.patients[p].getId();
    entry.hospitalId = e.hospitals[h].getId();
    e.admittedHistory.push(entry);
    cout << "Patient " << e.patients[p].getName() << " (ID " << entry.patientId << ") admitted at "
         << e.hospitals[h].getName() << ". Free beds now: " << e.hospitals[h].getBeds() << endl;
    cout << "[Stack] Pushed on the admitted history. [Linked list] Record added.\n";
    e.records.addRecord("Admission - Patient " + e.patients[p].getName() + " (ID " + to_string(entry.patientId) +
                        ") admitted at " + e.hospitals[h].getName());
}

// ---------------------------------------------------------------- menu options

static void showPatients(Eams &e)
{
    if (e.patients.size() == 0)
    {
        cout << "No patients.\n";
    }
    for (int i = 0; i < e.patients.size(); i++)
    {
        e.patients[i].display(e.city.getPlace(e.patients[i].getLocation()));
    }
}

static void addPatient(Eams &e)
{
    string name, problem;
    int age, severity, location;
    if (!readPatientDetails(e, name, age, problem, severity, location))
    {
        return;
    }
    int p = registerPatient(e, name, age, problem, severity, location);
    cout << "Patient added. Patient ID: " << e.patients[p].getId() << endl;
    e.records.addRecord("New patient - " + name + " (ID " + to_string(e.patients[p].getId()) + ") added, problem: " + problem);
    saveAll(e);
}

static void showAmbulances(Eams &e)
{
    for (int i = 0; i < e.ambulances.size(); i++)
    {
        e.ambulances[i].display(e.city.getPlace(e.ambulances[i].getLocation()));
    }
}

static void showHospitals(Eams &e)
{
    for (int i = 0; i < e.hospitals.size(); i++)
    {
        e.hospitals[i].display(e.city.getPlace(e.hospitals[i].getLocation()));
    }
}

static void addEmergency(Eams &e)
{
    int id;
    if (!readInt("Enter patient ID: ", 1, 999999, id))
    {
        return;
    }
    int p = findPatient(e, id);
    if (p == -1)
    {
        cout << "Patient ID not found.\n";
        return;
    }
    int status = e.patients[p].getStatus();
    if (status != REGISTERED && status != DISCHARGED)
    {
        cout << "Patient " << e.patients[p].getName() << " cannot be added: status is \""
             << e.patients[p].getStatusText() << "\".\n";
        return;
    }
    e.emergencyQueue.add(id, e.patients[p].getSeverity());
    e.patients[p].setStatus(PENDING);
    cout << "Emergency added to queue (severity " << e.patients[p].getSeverity() << ").\n";
    cout << "[Priority queue] The heap places the patient according to severity.\n";
    e.records.addRecord("Emergency - Patient " + e.patients[p].getName() + " (ID " + to_string(id) +
                        ") added to the emergency queue, severity " + to_string(e.patients[p].getSeverity()));
    saveAll(e);
}

static void dispatchAmbulance(Eams &e)
{
    int patientId;
    if (!e.emergencyQueue.peekHighest(patientId))
    {
        cout << "No pending emergencies.\n";
        return;
    }
    int p = findPatient(e, patientId);
    if (p == -1)
    {
        e.emergencyQueue.takeHighestPriority();
        cout << "Patient record not found. Removed from the queue.\n";
        return;
    }
    int a = nearestAvailableAmbulance(e, e.patients[p].getLocation());
    if (a == -1)
    {
        cout << "No ambulance is available. The patient stays at the front of the emergency queue.\n";
        return;
    }
    e.emergencyQueue.takeHighestPriority();
    cout << "[Priority queue] Highest priority patient taken out: " << e.patients[p].getName()
         << " (severity " << e.patients[p].getSeverity() << ")\n";
    e.ambulances[a].setAvailable(false);
    e.patients[p].setStatus(DISPATCHED);
    e.departedHistory.push(e.ambulances[a].getId());
    e.activeTrips.insertEnd(e.ambulances[a].getId());
    cout << "Emergency assigned to patient: " << e.patients[p].getName() << endl;
    cout << "Assigned ambulance: " << e.ambulances[a].getCode() << " [" << e.ambulances[a].getNumber() << "] (Driver: " << e.ambulances[a].getDriver() << ")\n";
    cout << "Route from ambulance to patient:\n";
    e.dijkstra.shortestPath(e.city, e.ambulances[a].getLocation(), e.patients[p].getLocation());

    int h = nearestHospitalWithBed(e, e.patients[p].getLocation());
    string hospitalText;
    if (h != -1)
    {
        cout << "Suggested hospital: " << e.hospitals[h].getName() << " (" << e.hospitals[h].getCode() << ")" << endl;
        cout << "Route from patient to hospital:\n";
        e.dijkstra.shortestPath(e.city, e.patients[p].getLocation(), e.hospitals[h].getLocation());
        e.ambulances[a].setDestination(e.hospitals[h].getLocation());
        hospitalText = "going to " + e.hospitals[h].getName();
    }
    else
    {
        cout << "No hospital has a free bed right now. The patient waits in the admission queue.\n";
        e.ambulances[a].setDestination(-1);
        hospitalText = "no hospital with a free bed yet";
    }
    e.admissionQueue.enqueue(patientId);
    cout << "[Queue] Patient added to the admission queue. [Stack] Ambulance pushed on the departed history.\n";
    cout << "[Linked list] Ambulance added to the active trips list and a record was created.\n";
    e.records.addRecord("Dispatch - Patient " + e.patients[p].getName() + " (ID " + to_string(patientId) +
                        ") picked up by Ambulance " + to_string(e.ambulances[a].getId()) + ", " + hospitalText);
    saveAll(e);
}

static void showPending(Eams &e)
{
    int n = e.emergencyQueue.size();
    if (n == 0)
    {
        cout << "No pending emergencies.\n";
        return;
    }
    int *ids = new int[n];
    int *levels = new int[n];
    e.emergencyQueue.snapshot(ids, levels, n);
    cout << "Pending emergencies [Priority queue] - the first one is dispatched first:\n";
    for (int i = 0; i < n; i++)
    {
        int p = findPatient(e, ids[i]);
        cout << (i + 1) << ". Patient ID: " << ids[i] << " | Name: " << (p != -1 ? e.patients[p].getName() : "?")
             << " | Severity: " << levels[i] << endl;
    }
    delete[] ids;
    delete[] levels;
}

static void shortestRoute(Eams &e)
{
    int start, finish;
    e.city.showPlaces();
    if (!readInt("Enter starting location (0 to " + to_string(e.city.getPlaceCount() - 1) + "): ", 0, e.city.getPlaceCount() - 1, start))
    {
        return;
    }
    if (!readInt("Enter destination location (0 to " + to_string(e.city.getPlaceCount() - 1) + "): ", 0, e.city.getPlaceCount() - 1, finish))
    {
        return;
    }
    e.dijkstra.shortestPath(e.city, start, finish);
}

static void runTraversal(Eams &e, bool useBfs)
{
    int start;
    e.city.showPlaces();
    if (!readInt("Enter starting location (0 to " + to_string(e.city.getPlaceCount() - 1) + "): ", 0, e.city.getPlaceCount() - 1, start))
    {
        return;
    }
    if (useBfs)
    {
        e.bfs.run(e.city, start);
    }
    else
    {
        e.dfs.run(e.city, start);
    }
}

static void admitPatient(Eams &e)
{
    int id;
    if (!readInt("Enter registered patient ID to admit (0 to register a new patient): ", 0, 999999, id))
    {
        return;
    }
    if (id == 0)
    {
        string name, problem;
        int age, severity, location;
        if (!readPatientDetails(e, name, age, problem, severity, location))
        {
            return;
        }
        int h = nearestHospitalWithBed(e, location);
        if (h == -1)
        {
            cout << "No hospital has a free bed. Patient not admitted.\n";
            return;
        }
        int p = registerPatient(e, name, age, problem, severity, location);
        e.records.addRecord("New patient - " + name + " (ID " + to_string(e.patients[p].getId()) + ") added, problem: " + problem);
        admitToHospital(e, p, h);
        saveAll(e);
        return;
    }
    int p = findPatient(e, id);
    if (p == -1)
    {
        cout << "Patient ID not found.\n";
        return;
    }
    int status = e.patients[p].getStatus();
    if (status != REGISTERED && status != DISCHARGED)
    {
        cout << "Patient " << e.patients[p].getName() << " cannot be admitted: status is \""
             << e.patients[p].getStatusText() << "\".\n";
        return;
    }
    int h = nearestHospitalWithBed(e, e.patients[p].getLocation());
    if (h == -1)
    {
        cout << "No hospital has a free bed. Patient not admitted.\n";
        return;
    }
    admitToHospital(e, p, h);
    saveAll(e);
}

static void admitNextWaiting(Eams &e)
{
    int patientId;
    if (!e.admissionQueue.peekAt(0, patientId))
    {
        cout << "No patients waiting for admission. (Dispatch an ambulance with option 6 to fill the queue.)\n";
        return;
    }
    int p = findPatient(e, patientId);
    if (p == -1)
    {
        e.admissionQueue.dequeue(patientId);
        cout << "Patient record not found. Removed from the queue.\n";
        return;
    }
    int h = nearestHospitalWithBed(e, e.patients[p].getLocation());
    if (h == -1)
    {
        cout << "No hospital has a free bed right now. The patient stays in the queue.\n";
        return;
    }
    e.admissionQueue.dequeue(patientId);
    cout << "[Queue] First patient in the queue taken out: " << e.patients[p].getName() << endl;
    admitToHospital(e, p, h);
    saveAll(e);
}

static void showWaiting(Eams &e)
{
    if (e.admissionQueue.empty())
    {
        cout << "No patients waiting for admission.\n";
        return;
    }
    cout << "Patients waiting for admission [Queue] - the first one is admitted first:\n";
    for (int k = 0; k < e.admissionQueue.size(); k++)
    {
        int id;
        e.admissionQueue.peekAt(k, id);
        int p = findPatient(e, id);
        cout << (k + 1) << ". Patient ID: " << id;
        if (p != -1)
        {
            int h = nearestHospitalWithBed(e, e.patients[p].getLocation());
            cout << " | Name: " << e.patients[p].getName() << " | Nearest hospital with a bed: "
                 << (h != -1 ? e.hospitals[h].getName() : "none right now");
        }
        cout << endl;
    }
}

static void showAdmittedHistory(Eams &e)
{
    if (e.admittedHistory.empty())
    {
        cout << "No patients admitted yet. (Use option 12 or 13 to admit a patient.)\n";
        return;
    }
    cout << "Admitted patients history [Stack] (latest first):\n";
    for (int k = 0; k < e.admittedHistory.size(); k++)
    {
        AdmissionEntry entry;
        e.admittedHistory.peekFromTop(k, entry);
        int p = findPatient(e, entry.patientId);
        int h = findHospital(e, entry.hospitalId);
        cout << (k + 1) << ". Patient ID: " << entry.patientId << " | Name: " << (p != -1 ? e.patients[p].getName() : "?")
             << " | Hospital: " << (h != -1 ? e.hospitals[h].getName() : "?") << endl;
    }
}

static void showDepartedHistory(Eams &e)
{
    if (e.departedHistory.empty())
    {
        cout << "No ambulances have departed yet. (Use option 5 and then option 6 to dispatch one.)\n";
        return;
    }
    cout << "Ambulances departed history [Stack] (latest first):\n";
    for (int k = 0; k < e.departedHistory.size(); k++)
    {
        int id;
        e.departedHistory.peekFromTop(k, id);
        int a = findAmbulance(e, id);
        cout << (k + 1) << ". Ambulance ID: " << makeCode("AMB", id) << " | Driver: " << (a != -1 ? e.ambulances[a].getDriver() : "?") << endl;
    }
}

static void addRecordNote(Eams &e)
{
    string note;
    cout << "Type the record text: ";
    if (!readLine(note))
    {
        return;
    }
    if (note.length() == 0)
    {
        cout << "Empty record. Nothing added.\n";
        return;
    }
    e.records.addRecord("Note - " + note);
    cout << "Record added. Total records: " << e.records.size() << endl;
    saveAll(e);
}

static void showActiveTrips(Eams &e)
{
    if (e.activeTrips.size() == 0)
    {
        cout << "No ambulances are on a trip.\n";
        return;
    }
    cout << "Ambulances on a trip [Linked list]:\n";
    for (int k = 0; k < e.activeTrips.size(); k++)
    {
        int id;
        e.activeTrips.getAt(k, id);
        int a = findAmbulance(e, id);
        cout << (k + 1) << ". Ambulance ID: " << makeCode("AMB", id);
        if (a != -1)
        {
            cout << " | Driver: " << e.ambulances[a].getDriver();
            if (e.ambulances[a].getDestination() != -1)
            {
                cout << " | Trip ends at: " << e.city.getPlace(e.ambulances[a].getDestination());
            }
        }
        cout << endl;
    }
}

static void releaseAmbulance(Eams &e)
{
    if (e.activeTrips.size() == 0)
    {
        cout << "No ambulances are on a trip.\n";
        return;
    }
    showActiveTrips(e);
    int id;
    if (!readInt("Enter ambulance ID to release (number only, e.g. 5 for AMB005): ", 1, 999999, id))
    {
        return;
    }
    int a = findAmbulance(e, id);
    if (a == -1 || !e.activeTrips.contains(id))
    {
        cout << "That ambulance is not on a trip.\n";
        return;
    }
    e.activeTrips.remove(id);
    e.ambulances[a].setAvailable(true);
    if (e.ambulances[a].getDestination() != -1)
    {
        e.ambulances[a].setLocation(e.ambulances[a].getDestination());
        e.ambulances[a].setDestination(-1);
    }
    cout << "Ambulance " << e.ambulances[a].getCode() << " is available again at " << e.city.getPlace(e.ambulances[a].getLocation()) << ".\n";
    cout << "[Linked list] Ambulance removed from the active trips list and a record was created.\n";
    e.records.addRecord("Release - Ambulance " + to_string(id) + " (Driver " + e.ambulances[a].getDriver() +
                        ") is available again at " + e.city.getPlace(e.ambulances[a].getLocation()));
    saveAll(e);
}

static void dischargePatient(Eams &e)
{
    int id;
    if (!readInt("Enter patient ID to discharge: ", 1, 999999, id))
    {
        return;
    }
    int p = findPatient(e, id);
    if (p == -1)
    {
        cout << "Patient ID not found.\n";
        return;
    }
    if (e.patients[p].getStatus() != ADMITTED)
    {
        cout << "Patient " << e.patients[p].getName() << " is not admitted (status: "
             << e.patients[p].getStatusText() << ").\n";
        return;
    }
    int h = findHospital(e, e.patients[p].getHospitalId());
    string hospitalName = "hospital";
    if (h != -1)
    {
        e.hospitals[h].dischargePatient();
        hospitalName = e.hospitals[h].getName();
        cout << "Patient " << e.patients[p].getName() << " discharged from " << hospitalName
             << ". Free beds now: " << e.hospitals[h].getBeds() << endl;
    }
    e.patients[p].setStatus(DISCHARGED);
    e.patients[p].setHospitalId(0);
    e.records.addRecord("Discharge - Patient " + e.patients[p].getName() + " (ID " + to_string(id) + ") discharged from " + hospitalName);
    saveAll(e);
}

static void findPatientById(Eams &e)
{
    int id;
    if (!readInt("Enter patient ID to find: ", 1, 999999, id))
    {
        return;
    }
    int bucket = e.patientIndex.bucketOf(id);
    int p = findPatient(e, id);
    cout << "[Hash table] ID " << id << " goes to bucket " << bucket << ".\n";
    if (p == -1)
    {
        cout << "Patient ID not found.\n";
        return;
    }
    cout << "Found at array position " << p << ":\n";
    e.patients[p].display(e.city.getPlace(e.patients[p].getLocation()));
}

static void searchRecords(Eams &e)
{
    string word;
    cout << "Enter a word to search in the records: ";
    if (!readLine(word))
    {
        return;
    }
    if (word.length() == 0)
    {
        cout << "Nothing to search for.\n";
        return;
    }
    cout << "[Linked list] Searching every record:\n";
    int found = e.records.search(word);
    if (found == 0)
    {
        cout << "No record contains \"" << word << "\".\n";
    }
    else
    {
        cout << found << " record(s) found.\n";
    }
}

// ---------------------------------------------------------------- main

static void showMenu()
{
    cout << "\n===== EMERGENCY AMBULANCE MANAGEMENT SYSTEM =====\n";
    cout << "1. Show patients\n2. Add patient\n3. Show ambulances\n4. Show hospitals\n";
    cout << "5. Add emergency to queue\n6. Dispatch ambulance\n7. Show pending emergencies\n";
    cout << "8. Show locations\n9. Find shortest route\n10. Run BFS\n11. Run DFS\n";
    cout << "12. Admit patient\n13. Admit next waiting patient\n14. Show patients waiting for admission\n";
    cout << "15. Show admitted patients history\n16. Show ambulances departed history\n";
    cout << "17. Show records\n18. Add new record\n";
    cout << "19. Release ambulance (trip completed)\n20. Discharge patient\n21. Find patient by ID\n";
    cout << "22. Search records\n23. Show ambulances on a trip\n24. Data structure demonstration\n0. Exit\n";
}

int main()
{
    Eams eams;
    eams.saveWarningShown = false;
    if (!loadAll(eams))
    {
        return 1;
    }

    while (true)
    {
        showMenu();
        int choice;
        if (!readInt("Enter choice: ", 0, 24, choice))
        {
            break;
        }
        if (choice == 0)
        {
            saveAll(eams);
            cout << "Exiting system.\n";
            return 0;
        }
        else if (choice == 1) showPatients(eams);
        else if (choice == 2) addPatient(eams);
        else if (choice == 3) showAmbulances(eams);
        else if (choice == 4) showHospitals(eams);
        else if (choice == 5) addEmergency(eams);
        else if (choice == 6) dispatchAmbulance(eams);
        else if (choice == 7) showPending(eams);
        else if (choice == 8) eams.city.showPlaces();
        else if (choice == 9) shortestRoute(eams);
        else if (choice == 10) runTraversal(eams, true);
        else if (choice == 11) runTraversal(eams, false);
        else if (choice == 12) admitPatient(eams);
        else if (choice == 13) admitNextWaiting(eams);
        else if (choice == 14) showWaiting(eams);
        else if (choice == 15) showAdmittedHistory(eams);
        else if (choice == 16) showDepartedHistory(eams);
        else if (choice == 17) eams.records.display();
        else if (choice == 18) addRecordNote(eams);
        else if (choice == 19) releaseAmbulance(eams);
        else if (choice == 20) dischargePatient(eams);
        else if (choice == 21) findPatientById(eams);
        else if (choice == 22) searchRecords(eams);
        else if (choice == 23) showActiveTrips(eams);
        else if (choice == 24) runDataStructureDemo(eams.city);

        if (inputEnded())
        {
            break;
        }
    }
    saveAll(eams);
    cout << "\nInput ended. Exiting system.\n";
    return 0;
}
