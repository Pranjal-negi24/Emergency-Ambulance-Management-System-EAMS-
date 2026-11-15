  # Emergency Ambulance Management System (EAMS) - C++17 Version

EAMS is a console project that manages emergency patients, ambulances and hospitals. The most critical patient is dispatched first, the nearest free ambulance and the nearest hospital with a free bed are chosen using the road map, and every step is recorded. Each data structure is used in a real feature and can also be seen working in a guided demonstration (menu option 24).

## How to run
**Windows:** double-click `build_and_run.bat` (needs g++, for example from MinGW or Code::Blocks).
**Linux / macOS:** run `./build_and_run.sh`.

Or compile by hand from inside this folder:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Graph.cpp src/Dijkstra.cpp src/Traversals.cpp src/FileHandler.cpp -o EAMS
```

Then run `EAMS.exe` (Windows) or `./EAMS` (Linux/macOS). Always run it from this project folder, because the program reads the `data/` folder using a relative path.

## Menu
| No. | Option | What it does |
|-----|--------|--------------|
| 1 | Show patients | Lists all patients with their status |
| 2 | Add patient | Registers a new patient |
| 3 | Show ambulances | Lists ambulances and whether they are available |
| 4 | Show hospitals | Lists hospitals and free beds |
| 5 | Add emergency to queue | Puts a patient in the emergency priority queue |
| 6 | Dispatch ambulance | Takes the most critical patient, picks the nearest free ambulance and suggests the nearest hospital with a bed |
| 7 | Show pending emergencies | Shows the priority queue, most critical first |
| 8 | Show locations | Lists the places on the map |
| 9 | Find shortest route | Dijkstra shortest route between two places |
| 10 | Run BFS | Breadth first search of the map |
| 11 | Run DFS | Depth first search of the map |
| 12 | Admit patient | Admits a registered patient, or registers and admits a new one, at the nearest hospital with a free bed |
| 13 | Admit next waiting patient | Admits the patient at the front of the admission queue |
| 14 | Show patients waiting for admission | Shows the admission queue |
| 15 | Show admitted patients history | Shows the stack of admitted patients, latest first |
| 16 | Show ambulances departed history | Shows the stack of departed ambulances, latest first |
| 17 | Show records | Shows all records in the order they were created |
| 18 | Add new record | Adds a note of your own as a record |
| 19 | Release ambulance | Ends a trip: the ambulance is available again at the hospital it travelled to |
| 20 | Discharge patient | Discharges an admitted patient and frees the hospital bed |
| 21 | Find patient by ID | Looks the patient up through the hash table |
| 22 | Search records | Searches all records for a word |
| 23 | Show ambulances on a trip | Shows the list of ambulances that are out on a trip |
| 24 | Data structure demonstration | Step-by-step demonstration of every data structure |
| 0 | Exit | Saves everything and exits |

## Data structures
| Data structure | Class / file | Used for | Menu |
|----------------|--------------|----------|------|
| Priority queue (max-heap) | `EmergencyQueue` in `PriorityQueueLinkedList.h` | The most critical patient is dispatched first; equal severity is served in the order reported | 5, 6, 7 |
| Queue | `SimpleQueue` in `StackQueue.h` | Dispatched patients wait for admission, first in first out. Also used by BFS | 13, 14, 10 |
| Stack | `SimpleStack` in `StackQueue.h` | History of admitted patients and history of departed ambulances, latest first. Also used by DFS | 15, 16, 11 |
| Linked list (integers) | `SimpleLinkedList` in `PriorityQueueLinkedList.h` | Ambulances on a trip: inserted at dispatch, deleted at release | 6, 19, 23 |
| Linked list (records) | `RecordList` in `PriorityQueueLinkedList.h` | Records of every new patient, emergency, dispatch, admission, release, discharge and note. Can be searched | 17, 18, 22 |
| Hash table | `HashTable` in `HashTable.h` | Finds a patient by ID without searching the whole patient list | 21 |
| Dynamic array | `DynamicArray` in `DynamicArray.h` | Patients, ambulances and hospitals; it grows automatically, so there is no fixed limit | all |
| Graph | `Graph` in `Graph.h` | Road map of 40 places and 71 two-way roads, loaded from `data/locations.csv` and `data/roads.csv` | 8, 9, 10, 11 |
| Dijkstra | `Dijkstra` in `Dijkstra.h` | Shortest routes, nearest ambulance and nearest hospital | 6, 9 |

Menu option 24 demonstrates all of these one by one using small sample data, so your real records are not changed. Messages such as `[Stack]`, `[Queue]`, `[Priority queue]` and `[Linked list]` in the normal menu options show where each structure is working.

## Typical workflow
1. Add an emergency (5) for one or more patients.
2. Dispatch an ambulance (6). The most critical patient goes first.
3. Admit the next waiting patient (13) when the ambulance arrives.
4. Release the ambulance (19) and later discharge the patient (20).
5. Look at the histories (15, 16), the records (17) and the trips (23).

## Saved data
Everything is saved automatically after each change and loaded again at the next start: patients, ambulance status and location, hospital beds, all records, the emergency queue, the admission queue, both histories and the ambulances on a trip.

| File | Contents |
|------|----------|
| `data/locations.csv` | the places of the map (graph nodes): location_id, area_name, zone, location_type, latitude, longitude, pincode |
| `data/roads.csv` | the road network (graph edges): road_id, from_location_id, to_location_id, distance_km, road_type |
| `data/patients.csv` | id, name, age, condition, severity (1-5), location_id |
| `data/ambulances.csv` | ambulance_id, ambulance_number, ambulance_type, driver_id, driver_name, driver_phone, driver_license_no, paramedic_name, base_hospital_id, current_location_id, status (`Available` / `On Duty` / `Under Maintenance`) |
| `data/hospitals.csv` | hospital_id, hospital_name, hospital_type, speciality, location_id, available_beds, total_beds, icu_beds_available, trauma_unit, emergency_phone |
| `data/records.txt` | one record per line (created by the program) |
| `data/state.csv` | queues, histories and trips (created by the program) |

**How the files connect.** Every `location_id` / `current_location_id` in the patient, ambulance and hospital files must exist in `locations.csv`, and both ends of every road in `roads.csv` must be a location. The program loads the map first and then everything else. In the menu, an ambulance or hospital is typed as a number: `5` means `AMB005` or `HOS005`.

**Distances.** `distance_km` in `roads.csv` is the road length between two places. It was calculated from the latitude/longitude in `locations.csv` (straight-line distance x 1.3 for road curves), and every place is connected to its 3 nearest places so the whole map is one connected network. Roads work in both directions. To change a distance, edit that row in `roads.csv`; Dijkstra, BFS, DFS, the nearest ambulance and the nearest hospital all use the new value. Do not use commas inside any value.

## Project structure
| Path | Contents |
|------|----------|
| `src/main.cpp` | menu and every menu option |
| `src/Graph.cpp`, `src/Dijkstra.cpp`, `src/Traversals.cpp` | road map (loaded from the CSV files), shortest route, BFS and DFS |
| `src/FileHandler.cpp` | reading and saving the data files |
| `include/Models.h` | `Person`, `Patient` (inherits Person), `Ambulance`, `Hospital` |
| `include/StackQueue.h`, `PriorityQueueLinkedList.h`, `HashTable.h`, `DynamicArray.h` | the data structures |
| `include/Input.h` | safe input: letters or wrong numbers never break the program |
| `include/Demo.h` | the data structure demonstration (option 24) |

## Notes
- Locations, road distances, drivers and hospitals are sample data (the road distances are estimated from the coordinates in `locations.csv`), not live road or GPS information. This is an academic prototype, not a real emergency dispatch tool.
- Names and problems can have spaces but no commas, and are limited to 40 characters.
