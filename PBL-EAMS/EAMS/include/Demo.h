#ifndef DEMO_H
#define DEMO_H
#include <iostream>
#include <string>
#include "StackQueue.h"
#include "PriorityQueueLinkedList.h"
#include "HashTable.h"
#include "Graph.h"
#include "Dijkstra.h"
#include "Traversals.h"
#include "Input.h"
using namespace std;
// Guided demonstration of every data structure used in EAMS
inline bool demoPause()
{
    cout << "\n(Press Enter to continue)";
    string line;
    return readLine(line);
}
inline void demoTitle(string title, string usedFor)
{
    cout << "\n==================================================\n";
    cout << title << "\n";
    cout << "Used in EAMS for: " << usedFor << "\n";
    cout << "==================================================\n";
}
inline void demoLinkedList()
{
    demoTitle("1. LINKED LIST", "active ambulance trips (menu 19, 23) and all records (menu 17, 18, 22)");
    SimpleLinkedList trips;
    cout << "Ambulances 101, 102 and 103 leave on trips. Each dispatch inserts a node at the end:\n";
    trips.insertEnd(101);
    trips.insertEnd(102);
    trips.insertEnd(103);
    trips.display();
    cout << "Ambulance 102 finishes its trip and is released. Its node is deleted from the middle:\n";
    trips.remove(102);
    trips.display();
    cout << "Is ambulance 101 still on a trip? " << (trips.contains(101) ? "Yes" : "No") << "\n";
    cout << "Is ambulance 102 still on a trip? " << (trips.contains(102) ? "Yes" : "No") << "\n";
    cout << "\nRecords are also a linked list. Every event adds a new record at the end:\n";
    RecordList records;
    records.addRecord("New patient - Asha (ID 7) added, problem: Fever");
    records.addRecord("Dispatch - Patient Asha (ID 7) picked up by Ambulance 101");
    records.addRecord("Admission - Patient Asha (ID 7) admitted at City Hospital");
    records.display();
    cout << "Searching the list for the word \"admission\":\n";
    records.search("admission");
}
inline void demoStack()
{
    demoTitle("2. STACK (last in, first out)", "history of admitted patients (menu 15), history of departed ambulances (menu 16), DFS (menu 11)");
    SimpleStack<int> history;
    cout << "Ambulances 101, 102 and 103 depart one after another. Each one is pushed on the stack:\n";
    history.push(101);
    history.push(102);
    history.push(103);
    cout << "History, latest first (top of the stack first): ";
    for (int k = 0; k < history.size(); k++)
    {
        int x;
        history.peekFromTop(k, x);
        cout << x << " ";
    }
    cout << "\nPop removes the top item (the latest one comes out first): ";
    int x;
    history.pop(x);
    cout << x << "\nStack now has " << history.size() << " items. The top is now: ";
    history.peekFromTop(0, x);
    cout << x << "\n";
}
inline void demoQueue()
{
    demoTitle("3. QUEUE (first in, first out)", "patients waiting for hospital admission (menu 13, 14), BFS (menu 10)");
    SimpleQueue<int> waiting;
    cout << "Patients 11, 12 and 13 are dispatched in this order. Each one joins the back of the queue:\n";
    waiting.enqueue(11);
    waiting.enqueue(12);
    waiting.enqueue(13);
    cout << "Queue, front first: ";
    for (int k = 0; k < waiting.size(); k++)
    {
        int x;
        waiting.peekAt(k, x);
        cout << x << " ";
    }
    int x;
    waiting.dequeue(x);
    cout << "\nA hospital admits the next patient. Dequeue removes the front item: patient " << x << "\n";
    waiting.dequeue(x);
    cout << "Next admitted: patient " << x << " (the one who waited longest always goes first)\n";
    cout << "Patients still waiting: " << waiting.size() << "\n";
}
inline void demoPriorityQueue()
{
    demoTitle("4. PRIORITY QUEUE (max-heap)", "pending emergencies (menu 5, 6, 7): the most critical patient is dispatched first");
    EmergencyQueue emergencies;
    cout << "Four emergencies are reported, in this order:\n";
    cout << "  Patient 21 (severity 2), Patient 22 (severity 5), Patient 23 (severity 3), Patient 24 (severity 5)\n";
    emergencies.add(21, 2);
    emergencies.add(22, 5);
    emergencies.add(23, 3);
    emergencies.add(24, 5);
    cout << "The heap keeps the most critical patient at the top of the array:\n";
    emergencies.showHeapArray();
    cout << "Dispatching one patient at a time (highest severity first, earlier report first when equal):\n";
    int round = 1;
    int id;
    while (emergencies.peekHighest(id))
    {
        cout << "  Dispatch " << round << ": patient " << emergencies.takeHighestPriority() << "\n";
        round++;
    }
    cout << "Patients 22 and 24 both have severity 5, so the one reported first (22) goes first.\n";
}
inline void demoHashTable()
{
    demoTitle("5. HASH TABLE", "finding a patient by ID without searching the whole list (menu 21)");
    HashTable table;
    cout << "Patient IDs 101, 205, 310, 412 and 518 are stored with their position in the patient array.\n";
    cout << "The bucket is chosen with: bucket = ID % number of buckets.\n";
    table.insert(101, 0);
    table.insert(205, 1);
    table.insert(310, 2);
    table.insert(412, 3);
    table.insert(518, 4);
    table.display();
    int index;
    cout << "Looking up ID 310: go straight to bucket " << table.bucketOf(310) << "... ";
    if (table.find(310, index))
    {
        cout << "found at array index " << index << "\n";
    }
    cout << "Looking up ID 999: ";
    if (!table.find(999, index))
    {
        cout << "not found (only bucket " << table.bucketOf(999) << " was checked)\n";
    }
    cout << "Removing ID 205 from the table:\n";
    table.remove(205);
    table.display();
}
inline void demoGraph(Graph &city)
{
    demoTitle("6. GRAPH (with Dijkstra, BFS and DFS)", "road map: shortest routes, nearest ambulance and hospital (menu 6, 9, 10, 11)");
    city.showPlaces();
    cout << "\n";
    city.showRoads();
    cout << "\nBFS (uses a queue) visits the nearest places first, starting from " << city.getPlace(0) << ":\n";
    BFS bfs;
    bfs.run(city, 0);
    cout << "DFS (uses a stack) goes as deep as possible first, starting from " << city.getPlace(0) << ":\n";
    DFS dfs;
    dfs.run(city, 0);
    cout << "Dijkstra finds the shortest road route from " << city.getPlace(0) << " to " << city.getPlace(city.getPlaceCount() - 1) << ":\n";
    Dijkstra dijkstra;
    dijkstra.shortestPath(city, 0, city.getPlaceCount() - 1);
}
inline void runDataStructureDemo(Graph &city)
{
    cout << "\n##### DATA STRUCTURE DEMONSTRATION #####\n";
    cout << "Each part uses small sample data. Your real records are not changed.\n";
    if (!demoPause()) return;
    demoLinkedList();
    if (!demoPause()) return;
    demoStack();
    if (!demoPause()) return;
    demoQueue();
    if (!demoPause()) return;
    demoPriorityQueue();
    if (!demoPause()) return;
    demoHashTable();
    if (!demoPause()) return;
    demoGraph(city);
    cout << "\n##### END OF DEMONSTRATION #####\n";
}
#endif
