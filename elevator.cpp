#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <algorithm>
#include <sstream>    // for serializeStatus()
using namespace std;

// ----------------------------------
// Elevator class
// ----------------------------------
class Elevator {
public:
    int id;
    int currentFloor;
    bool doorOpen;
    bool goingUp;
    bool idle;

    vector<int> upRequests;
    vector<int> downRequests;

    Elevator(int elevatorId)
      : id(elevatorId)
      , currentFloor(1)
      , doorOpen(false)
      , goingUp(true)
      , idle(true)
    {}

    // Enqueue a request, avoid duplicates
    void addRequest(int floor, bool up) {
        auto& queue = up ? upRequests : downRequests;
        if (find(queue.begin(), queue.end(), floor) == queue.end()) {
            queue.push_back(floor);
            if (up) {
                sort(queue.begin(), queue.end());
            } else {
                sort(queue.begin(), queue.end());
                reverse(queue.begin(), queue.end());
            }
        }
    }

    // Move one step: travel toward target or open/close doors
    void move() {
        if (upRequests.empty() && downRequests.empty()) {
            idle = true;
            return;
        }
        idle = false;

        // Switch direction if current queue empty
        if (goingUp && upRequests.empty() && !downRequests.empty())
            goingUp = false;
        else if (!goingUp && downRequests.empty() && !upRequests.empty())
            goingUp = true;

        // Determine next target
        int target = goingUp
                   ? (upRequests.empty()   ? currentFloor : upRequests.front())
                   : (downRequests.empty() ? currentFloor : downRequests.front());

        // Arrive at target?
        if (currentFloor == target) {
            openDoors();
            if (goingUp && !upRequests.empty())
                upRequests.erase(upRequests.begin());
            else if (!goingUp && !downRequests.empty())
                downRequests.erase(downRequests.begin());
        }
        // Move one floor
        else if (target > currentFloor) {
            ++currentFloor;
        }
        else {
            --currentFloor;
        }
    }

    void openDoors() {
        doorOpen = true;
        cout << "\nElevator " << id
             << ": Doors opening at floor " << currentFloor << "\n";
        this_thread::sleep_for(chrono::seconds(3));
        closeDoors();
    }

    void closeDoors() {
        if (!doorOpen) return;
        doorOpen = false;
        cout << "Elevator " << id << ": Doors closing.\n";
        this_thread::sleep_for(chrono::seconds(1));
    }

    void showStatus() const {
        cout << "Elevator " << id
             << ": Floor " << currentFloor
             << " | " << (idle ? "Idle" : (goingUp ? "Going Up" : "Going Down"))
             << " | Doors " << (doorOpen ? "Open" : "Closed")
             << "\n";
    }
};

// ----------------------------------
// Building class
// ----------------------------------
class Building {
public:
    vector<Elevator> elevators;
    int numFloors;

    Building(int totFloors, int totElev)
      : numFloors(totFloors)
    {
        for (int i = 1; i <= totElev; ++i)
            elevators.emplace_back(i);
    }

    // Step each elevator and show status if any moved
    void stepAll() {
        bool moved = false;
        for (auto& el : elevators) {
            int prev = el.currentFloor;
            el.move();
            if (el.currentFloor != prev) moved = true;
        }
        if (moved) showStatus();
    }

    // Display all elevator states
    void showStatus() const {
        cout << "\n===== Building Status =====\n";
        for (auto const& el : elevators)
            el.showStatus();
    }

    // Automatically dispatch the closest idle (or closest overall) elevator
    void dispatch(int floor) {
        int bestIdx = -1, bestDist = numFloors + 1;
        // Prefer idle
        for (int i = 0; i < (int)elevators.size(); ++i) {
            if (elevators[i].idle) {
                int d = abs(elevators[i].currentFloor - floor);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx  = i;
                }
            }
        }
        // Fallback to any if none idle
        if (bestIdx < 0) {
            for (int i = 0; i < (int)elevators.size(); ++i) {
                int d = abs(elevators[i].currentFloor - floor);
                if (d < bestDist) {
                    bestDist = d;
                    bestIdx  = i;
                }
            }
        }
        elevators[bestIdx].addRequest(floor,
            floor > elevators[bestIdx].currentFloor);
        cout << "Dispatched Elevator " << (bestIdx + 1)
             << " to floor " << floor << "\n";
    }

    // Serialize status to a single string
    string serializeStatus() const {
        ostringstream out;
        out << "===== Building Status =====\n";
        for (auto const& el : elevators) {
            out << "Elevator " << el.id
                << ": Floor " << el.currentFloor
                << " | " << (el.idle ? "Idle" : (el.goingUp ? "Going Up" : "Going Down"))
                << " | Doors " << (el.doorOpen ? "Open" : "Closed")
                << "\n";
        }
        return out.str();
    }
};

// ----------------------------------
// CLI entry-point
// ----------------------------------
int main() {
    int floors, elevCnt;
    cout << "ENTER NUMBER OF FLOORS: ";
    cin  >> floors;
    cout << "ENTER NUMBER OF ELEVATORS: ";
    cin  >> elevCnt;
    Building building(floors, elevCnt);
    cout << "\nSimulation starting...\n";

    while (true) {
        building.stepAll();
        cout << "\nEnter floor to call (or -1 to skip): ";
        int floor; cin >> floor;
        if (floor >= 1 && floor <= floors)
            building.dispatch(floor);
        this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}

// ----------------------------------
// WebAssembly Wrappers (C linkage)
// ----------------------------------
static Building gBuilding(10, 2);

extern "C" {
    // Called from JS to enqueue a new request
    void addRequest(int floor) {
        gBuilding.dispatch(floor);
    }
    // Called from JS to advance one simulation step
    void stepSimulation() {
        gBuilding.stepAll();
    }
    // Called from JS to fetch the current status string
    const char* getStatus() {
        static string s;
        s = gBuilding.serializeStatus();
        return s.c_str();
    }
}
