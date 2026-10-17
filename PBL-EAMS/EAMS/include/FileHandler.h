#ifndef FILEHANDLER_H
#define FILEHANDLER_H
#include <string>
#include "Models.h"
#include "Graph.h"
using namespace std;

int loadPatients(string fileName, Patient list[], int maxCount, Graph &city);
int loadAmbulances(string fileName, Ambulance list[], int maxCount, Graph &city);
int loadHospitals(string fileName, Hospital list[], int maxCount, Graph &city);
bool savePatients(string fileName, Patient list[], int count, Graph &city);

#endif
