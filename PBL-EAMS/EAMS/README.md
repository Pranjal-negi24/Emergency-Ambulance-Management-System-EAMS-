# Emergency Ambulance Management System (EAMS) - C++17 Version

This is a small, beginner-friendly console project for demonstrating the first working part of EAMS. It uses basic classes, arrays, loops, a simple emergency-priority queue, graph traversal and Dijkstra's shortest-path algorithm.

## Compile
Open a terminal in this folder and run:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Graph.cpp src/Dijkstra.cpp src/Traversals.cpp src/FileHandler.cpp -o EAMS
```

Run it from this project folder (the program reads the `data/` folder using a relative path). On Windows, run `EAMS.exe` after compilation. On Linux/macOS, run `./EAMS`.

## Suggested demo
1. Show patients.
2. Show ambulances.
3. Add emergency to queue (try patient 3 and patient 1).
4. Show pending emergencies.
5. Dispatch ambulance; the higher-severity emergency should be selected first.
6. Find a shortest route, then run BFS and DFS.

## Included now
- `Person` base class and `Patient` inheritance
- `Ambulance` and `Hospital` classes
- Patient entry and listing
- Simple emergency queue ordered by severity when dispatching
- Basic ambulance assignment
- Sample location graph, BFS, DFS and Dijkstra route display
- Console menu
- Linked list records, stack history and admission queue (menu options 12-18, see below)
- CSV data files in `data/` loaded at start-up (see below)

## Header files
| Header | Contents |
|--------|----------|
| `include/StackQueue.h` | `SimpleStack` and `SimpleQueue` |
| `include/PriorityQueueLinkedList.h` | `EmergencyQueue` (priority queue), `SimpleLinkedList` and `RecordList` (linked list of records) |
| `include/Dijkstra.h` (+ `src/Dijkstra.cpp`) | `Dijkstra`: shortest distance and route |
| `include/Traversals.h` (+ `src/Traversals.cpp`) | `BFS` (uses the queue) and `DFS` (uses the stack) |
| `include/Graph.h` (+ `src/Graph.cpp`) | the road map only: places and road lengths |
| `include/Models.h`, `include/FileHandler.h` | patient/ambulance/hospital classes and CSV loading |

## Data structures in the menu
| Data structure | Used for | Menu option |
|----------------|----------|-------------|
| Linked list (`RecordList`) | A record is created for every new patient, dispatch and admission; option 18 adds a record by hand; option 17 shows them | 17, 18 |
| Queue (`SimpleQueue`) | Patients sent to a hospital by dispatch wait here and are admitted first in, first out | 13, 14 |
| Stack (`SimpleStack`) | History of admitted patients, latest first | 15 |
| Stack (`SimpleStack`) | History of departed ambulances, latest first | 16 |

- Option 12 registers a new patient and admits them at the nearest hospital with a free bed (uses one bed, pushes the patient on the admitted stack, creates a record, saves `patients.csv`).
- Option 6 (dispatch) puts the patient in the admission queue and pushes the ambulance on the departed stack. Option 13 then admits the patient at the front of the queue.
- History, queue and records are kept while the program runs; they are not saved to a file.

## Not completed yet
- Saving ambulance status and hospital beds back to CSV (only `patients.csv` is saved, when a patient is added)
- A proper min-heap and hash table
- Ambulance release / trip completion workflow
- Comparing ambulances by actual Dijkstra distance (the current initial selection uses a simple location-number comparison)
- Choosing the closest hospital by route distance and checking beds robustly
- Full validation and automated testing

## Data folder
| File | Columns |
|------|---------|
| `data/Ambulance.csv` | id, driver_name, location, availability (`Available` / `Busy`) |
| `data/hospitals.csv` | hospital_id, name, location, available_beds, total_beds, phone |
| `data/patients.csv` | id, name, age, condition, severity (1-5), location |

The `location` column uses a place name from the map (City Centre, Clock Tower, ISBT, Prem Nagar, Hospital Road, University). Do not use commas inside any value. New patients added from the menu are written back to `patients.csv`.

Road distances and locations are sample data, not live road/GPS information. Treat this as an academic prototype, not a real emergency dispatch tool.

Please read each class and method, change the sample names/data, and make sure the team can explain and test the code before presenting it. The code is intentionally kept basic for learning, not to conceal its origin.

Ambulance selection and hospital suggestion compare the sample road distances calculated by the graph. They are still based on the small fixed demo map.
