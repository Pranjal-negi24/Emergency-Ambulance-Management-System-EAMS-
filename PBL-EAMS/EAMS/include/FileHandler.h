#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <string>
#include "Models.h"
#include "Graph.h"
#include "DynamicArray.h"
#include "PriorityQueueLinkedList.h"
using namespace std;

// Splits one CSV line at every comma; returns how many fields the line has
int splitCsvLine(string line, string fields[], int maxFields);

int loadPatients(string fileName, DynamicArray<Patient> &list, Graph &city);
int loadAmbulances(string fileName, DynamicArray<Ambulance> &list, Graph &city);
int loadHospitals(string fileName, DynamicArray<Hospital> &list, Graph &city);

bool savePatients(string fileName, DynamicArray<Patient> &list, Graph &city);
bool saveAmbulances(string fileName, DynamicArray<Ambulance> &list, Graph &city);
bool saveHospitals(string fileName, DynamicArray<Hospital> &list, Graph &city);

// Records are stored one per line in a plain text file
void loadRecords(string fileName, RecordList &records);
bool saveRecords(string fileName, RecordList &records);

#endif
