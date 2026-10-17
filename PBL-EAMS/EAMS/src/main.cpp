#include <iostream>
#include <string>
#include "Models.h"
#include "StackQueue.h"
#include "PriorityQueueLinkedList.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Traversals.h"
#include "FileHandler.h"
using namespace std;

// Returns the index of the nearest hospital (by road distance) that has a free bed, or -1
static int findHospitalWithBed(Graph &city, Dijkstra &dijkstra, Hospital hospitals[], int hospitalCount, int location)
{
    int best = -1;
    int bestDistance = 1000000;
    for (int i = 0; i < hospitalCount; i++)
    {
        int distance = dijkstra.getDistance(city, location, hospitals[i].getLocation());
        if (hospitals[i].getBeds() > 0 && distance >= 0 && distance < bestDistance)
        {
            bestDistance = distance;
            best = i;
        }
    }
    return best;
}

int main()
{
    Graph city;
    Dijkstra dijkstra;
    BFS bfs;
    DFS dfs;
    Patient patients[50];
    Ambulance ambulances[20];
    Hospital hospitals[10];

    int patientCount = loadPatients("data/patients.csv", patients, 50, city);
    int ambulanceCount = loadAmbulances("data/Ambulance.csv", ambulances, 20, city);
    int hospitalCount = loadHospitals("data/hospitals.csv", hospitals, 10, city);

    int nextPatientId = 1;
    for (int i = 0; i < patientCount; i++)
    {
        if (patients[i].getId() >= nextPatientId)
        {
            nextPatientId = patients[i].getId() + 1;
        }
    }

    EmergencyQueue emergencyQueue;
    SimpleQueue admissionQueue;      // patients on the way to a hospital, waiting to be admitted (first in, first out)
    SimpleStack admittedHistory;     // patient IDs admitted, latest on top
    SimpleStack departedHistory;     // ambulance IDs departed, latest on top
    RecordList records;              // linked list of dispatch and admission records
    int assignedHospital[50];
    for (int i = 0; i < 50; i++)
    {
        assignedHospital[i] = -1;
    }
    int choice = -1;

    while (choice != 0)
    {
        cout << "\n===== EMERGENCY AMBULANCE MANAGEMENT SYSTEM =====\n";
        cout << "1. Show patients\n2. Add patient\n3. Show ambulances\n4. Show hospitals\n";
        cout << "5. Add emergency to queue\n6. Dispatch ambulance\n7. Show pending emergencies\n";
        cout << "8. Show locations\n9. Find shortest route\n10. Run BFS\n11. Run DFS\n";
        cout << "12. Admit new patient\n13. Admit next waiting patient\n14. Show patients waiting for admission\n";
        cout << "15. Show admitted patients history\n16. Show ambulances departed history\n";
        cout << "17. Show records\n18. Add new record\n0. Exit\n";
        cout << "Enter choice: ";
        if (!(cin >> choice))
        {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a number.\n";
            continue;
        }

        if (choice == 1)
        {
            if (patientCount == 0)
            {
                cout << "No patients.\n";
            }
            for (int i = 0; i < patientCount; i++)
            {
                patients[i].display();
            }
        }
        else if (choice == 2)
        {
            if (patientCount == 50)
            {
                cout << "Patient list is full.\n";
                continue;
            }
            string name, problem;
            int age, severity, location;
            cout << "Enter patient name (one word): ";
            cin >> name;
            cout << "Enter age: ";
            cin >> age;
            cout << "Enter problem (one word): ";
            cin >> problem;
            city.showPlaces();
            cout << "Enter location number: ";
            cin >> location;
            cout << "Enter severity (1 low to 5 critical): ";
            cin >> severity;
            if (age < 0 || location < 0 || location > 5 || severity < 1 || severity > 5)
            {
                cout << "Invalid details. Patient not added.\n";
                continue;
            }
            patients[patientCount] = Patient(nextPatientId, name, age, problem, severity, location);
            cout << "Patient added. Patient ID: " << nextPatientId << endl;
            nextPatientId++;
            patientCount++;
            records.addRecord("New patient - " + name + " (ID " + to_string(nextPatientId - 1) + ") added, problem: " + problem);
            if (!savePatients("data/patients.csv", patients, patientCount, city))
            {
                cout << "Warning: could not save to data/patients.csv\n";
            }
        }
        else if (choice == 3)
        {
            for (int i = 0; i < ambulanceCount; i++)
            {
                ambulances[i].display();
            }
        }
        else if (choice == 4)
        {
            for (int i = 0; i < hospitalCount; i++)
            {
                hospitals[i].display();
            }
        }
        else if (choice == 5)
        {
            int id;
            int found = 0;
            cout << "Enter patient ID: ";
            cin >> id;
            for (int i = 0; i < patientCount; i++)
            {
                if (patients[i].getId() == id)
                {
                    emergencyQueue.add(id, patients[i].getSeverity());
                    cout << "Emergency added to queue.\n";
                    found = 1;
                    break;
                }
            }
            if (!found)
            {
                cout << "Patient ID not found.\n";
            }
        }
        else if (choice == 6)
        {
            int patientId = emergencyQueue.takeHighestPriority();
            if (patientId == -1)
            {
                cout << "No pending emergencies.\n";
                continue;
            }
            int pIndex = -1;
            for (int i = 0; i < patientCount; i++)
            {
                if (patients[i].getId() == patientId)
                {
                    pIndex = i;
                }
            }
            if (pIndex == -1)
            {
                cout << "Patient record not found.\n";
                continue;
            }
            int ambulanceIndex = -1;
            int bestDistance = 1000000;
            for (int i = 0; i < ambulanceCount; i++)
            {
                if (ambulances[i].isAvailable())
                {
                    int distance = dijkstra.getDistance(city, ambulances[i].getLocation(), patients[pIndex].getLocation());
                    if (distance >= 0 && distance < bestDistance)
                    {
                        bestDistance = distance;
                        ambulanceIndex = i;
                    }
                }
            }
            if (ambulanceIndex == -1)
            {
                cout << "No ambulance is available.\n";
                emergencyQueue.add(patientId, patients[pIndex].getSeverity());
                continue;
            }
            ambulances[ambulanceIndex].setAvailable(false);
            departedHistory.push(ambulances[ambulanceIndex].getId());
            cout << "Emergency assigned to patient: " << patients[pIndex].getName() << endl;
            cout << "Assigned ambulance: " << ambulances[ambulanceIndex].getId()
                 << " (Driver: " << ambulances[ambulanceIndex].getDriver() << ")\n";
            cout << "Route from ambulance to patient:\n";
            dijkstra.shortestPath(city, ambulances[ambulanceIndex].getLocation(), patients[pIndex].getLocation());
            int bestHospital = -1;
            int hospitalDistance = 1000000;
            for (int i = 0; i < hospitalCount; i++)
            {
                int distance = dijkstra.getDistance(city, patients[pIndex].getLocation(), hospitals[i].getLocation());
                if (hospitals[i].getBeds() > 0 && distance >= 0 && distance < hospitalDistance)
                {
                    hospitalDistance = distance;
                    bestHospital = i;
                }
            }
            if (bestHospital != -1)
            {
                cout << "Suggested hospital: " << hospitals[bestHospital].getName() << endl;
                cout << "Route from patient to hospital:\n";
                dijkstra.shortestPath(city, patients[pIndex].getLocation(), hospitals[bestHospital].getLocation());
                assignedHospital[pIndex] = bestHospital;
                admissionQueue.enqueue(patientId);
                records.addRecord("Dispatch - Patient " + patients[pIndex].getName() + " (ID " + to_string(patientId) +
                                  ") picked up by Ambulance " + to_string(ambulances[ambulanceIndex].getId()) +
                                  ", going to " + hospitals[bestHospital].getName());
            }
            else
            {
                cout << "No hospital with beds is listed.\n";
                records.addRecord("Dispatch - Patient " + patients[pIndex].getName() + " (ID " + to_string(patientId) +
                                  ") picked up by Ambulance " + to_string(ambulances[ambulanceIndex].getId()) +
                                  ", no hospital with beds");
            }
            cout << "Note: this demo marks the ambulance busy; option to release it is not added yet.\n";
        }
        else if (choice == 7)
        {
            emergencyQueue.display();
        }
        else if (choice == 8)
        {
            city.showPlaces();
        }
        else if (choice == 9)
        {
            int start, finish;
            city.showPlaces();
            cout << "Enter starting location: ";
            cin >> start;
            cout << "Enter destination location: ";
            cin >> finish;
            dijkstra.shortestPath(city, start, finish);
        }
        else if (choice == 10)
        {
            int start;
            city.showPlaces();
            cout << "Enter start location: ";
            cin >> start;
            bfs.run(city, start);
        }
        else if (choice == 11)
        {
            int start;
            city.showPlaces();
            cout << "Enter start location: ";
            cin >> start;
            dfs.run(city, start);
        }
        else if (choice == 12)
        {
            // Admit a NEW patient straight into the nearest hospital that has a free bed
            if (patientCount == 50)
            {
                cout << "Patient list is full.\n";
                continue;
            }
            string name, problem;
            int age, severity, location;
            cout << "Enter patient name (one word): ";
            cin >> name;
            cout << "Enter age: ";
            cin >> age;
            cout << "Enter problem (one word): ";
            cin >> problem;
            city.showPlaces();
            cout << "Enter location number: ";
            cin >> location;
            cout << "Enter severity (1 low to 5 critical): ";
            cin >> severity;
            if (age < 0 || location < 0 || location > 5 || severity < 1 || severity > 5)
            {
                cout << "Invalid details. Patient not admitted.\n";
                continue;
            }
            int h = findHospitalWithBed(city, dijkstra, hospitals, hospitalCount, location);
            if (h == -1)
            {
                cout << "No hospital with a free bed. Patient not admitted.\n";
                continue;
            }
            patients[patientCount] = Patient(nextPatientId, name, age, problem, severity, location);
            assignedHospital[patientCount] = h;
            hospitals[h].admitPatient();
            admittedHistory.push(nextPatientId);
            cout << "Patient " << name << " (ID " << nextPatientId << ") admitted at " << hospitals[h].getName()
                 << ". Free beds now: " << hospitals[h].getBeds() << endl;
            records.addRecord("Admission - Patient " + name + " (ID " + to_string(nextPatientId) +
                              ") admitted at " + hospitals[h].getName());
            nextPatientId++;
            patientCount++;
            if (!savePatients("data/patients.csv", patients, patientCount, city))
            {
                cout << "Warning: could not save to data/patients.csv\n";
            }
        }
        else if (choice == 13)
        {
            // Admit the patient who has waited longest in the admission queue (first in, first out)
            int patientId;
            if (!admissionQueue.peekAt(0, patientId))
            {
                cout << "No patients waiting for admission. (Dispatch an ambulance with option 6 to fill the queue.)\n";
                continue;
            }
            int pIndex = -1;
            for (int i = 0; i < patientCount; i++)
            {
                if (patients[i].getId() == patientId)
                {
                    pIndex = i;
                }
            }
            if (pIndex == -1)
            {
                admissionQueue.dequeue(patientId);
                cout << "Patient record not found. Removed from queue.\n";
                continue;
            }
            int h = findHospitalWithBed(city, dijkstra, hospitals, hospitalCount, patients[pIndex].getLocation());
            if (h == -1)
            {
                cout << "No hospital has a free bed right now. Patient stays in the queue.\n";
                continue;
            }
            admissionQueue.dequeue(patientId);
            assignedHospital[pIndex] = h;
            hospitals[h].admitPatient();
            admittedHistory.push(patientId);
            cout << "Patient " << patients[pIndex].getName() << " (ID " << patientId << ") admitted at "
                 << hospitals[h].getName() << ". Free beds now: " << hospitals[h].getBeds() << endl;
            records.addRecord("Admission - Patient " + patients[pIndex].getName() + " (ID " + to_string(patientId) +
                              ") admitted at " + hospitals[h].getName());
        }
        else if (choice == 14)
        {
            if (admissionQueue.empty())
            {
                cout << "No patients waiting for admission.\n";
            }
            else
            {
                cout << "Patients waiting for admission (first one is admitted first):\n";
                for (int k = 0; k < admissionQueue.size(); k++)
                {
                    int id;
                    admissionQueue.peekAt(k, id);
                    for (int i = 0; i < patientCount; i++)
                    {
                        if (patients[i].getId() == id)
                        {
                            cout << (k + 1) << ". Patient ID: " << id << " | Name: " << patients[i].getName()
                                 << " | Suggested hospital: " << hospitals[assignedHospital[i]].getName() << endl;
                        }
                    }
                }
            }
        }
        else if (choice == 15)
        {
            if (admittedHistory.empty())
            {
                cout << "No patients admitted yet. (Use option 12 or 13 to admit a patient.)\n";
            }
            else
            {
                cout << "Admitted patients history (latest first):\n";
                for (int k = 0; k < admittedHistory.size(); k++)
                {
                    int id;
                    admittedHistory.peekFromTop(k, id);
                    for (int i = 0; i < patientCount; i++)
                    {
                        if (patients[i].getId() == id)
                        {
                            cout << (k + 1) << ". Patient ID: " << id << " | Name: " << patients[i].getName()
                                 << " | Hospital: " << hospitals[assignedHospital[i]].getName() << endl;
                        }
                    }
                }
            }
        }
        else if (choice == 16)
        {
            if (departedHistory.empty())
            {
                cout << "No ambulances have departed yet. (Use option 5 and then option 6 to dispatch one.)\n";
            }
            else
            {
                cout << "Ambulances departed history (latest first):\n";
                for (int k = 0; k < departedHistory.size(); k++)
                {
                    int id;
                    departedHistory.peekFromTop(k, id);
                    for (int i = 0; i < ambulanceCount; i++)
                    {
                        if (ambulances[i].getId() == id)
                        {
                            cout << (k + 1) << ". Ambulance ID: " << id << " | Driver: " << ambulances[i].getDriver() << endl;
                        }
                    }
                }
            }
        }
        else if (choice == 17)
        {
            records.display();
        }
        else if (choice == 18)
        {
            string note;
            cin.ignore(10000, '\n');
            cout << "Type the record text: ";
            getline(cin, note);
            if (note.length() == 0)
            {
                cout << "Empty record. Nothing added.\n";
            }
            else
            {
                records.addRecord("Note - " + note);
                cout << "Record added. Total records: " << records.size() << endl;
            }
        }
        else if (choice == 0)
        {
            cout << "Exiting system.\n";
        }
        else
        {
            cout << "Invalid choice. Try again.\n";
        }
    }
    return 0;
}
