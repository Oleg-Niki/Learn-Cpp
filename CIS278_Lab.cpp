#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>            // <-- make sure std::abs(int) is declared
#include <thread>
#include <chrono>
#include <string>
#include <stdexcept>          // <-- for the exception below

// Enumeration for elevator direction
enum class Direction { IDLE, UP, DOWN };

// Elevator class that simulates basic elevator behavior
class Elevator {
public:
    int          id;
    int          currentFloor;
    int          targetFloor;  // -1 means no active request
    Direction    direction;
    bool         doorOpen;

    Elevator(int elevatorId, int startFloor = 1)
      : id(elevatorId)
      , currentFloor(startFloor)
      , targetFloor(-1)
      , direction(Direction::IDLE)
      , doorOpen(false)
    {}

    void addRequest(int floor) {
        targetFloor = floor;
        if      (floor > currentFloor) direction = Direction::UP;
        else if (floor < currentFloor) direction = Direction::DOWN;
        else {
            // Already here
            openDoors();
            closeDoors();
            targetFloor = -1;
            direction   = Direction::IDLE;
        }
    }

    void step() {
        if (targetFloor == -1) {
            direction = Direction::IDLE;
            return;
        }

        // Move one floor per step
        if      (direction == Direction::UP)   ++currentFloor;
        else if (direction == Direction::DOWN) --currentFloor;

        // Arrived?
        if (currentFloor == targetFloor) {
            openDoors();
            std::this_thread::sleep_for(std::chrono::seconds(1));
            closeDoors();
            targetFloor = -1;
            direction   = Direction::IDLE;
        }
    }

    void openDoors() {
        doorOpen = true;
        std::cout << "Elevator " << id
                  << " reached floor " << currentFloor
                  << " -- Doors Open\n";
    }

    void closeDoors() {
        doorOpen = false;
        std::cout << "Elevator " << id
                  << " doors closing...\n";
    }
};

// Building class to manage multiple elevators
class Building {
    int                 floors;
    std::vector<Elevator> elevators;

public:
    Building(int totalFloors, int numElevators)
      : floors(totalFloors)
    {
        for (int i = 0; i < numElevators; ++i) {
            elevators.emplace_back(i + 1, 1);
        }
    }

    void requestElevator(int floor) {
        if (elevators.empty()) {
            throw std::runtime_error("No elevators available");
        }

        int chosenIndex = -1;
        int minDistance = floors + 1;

        // 1) Prefer idle elevators
        for (int i = 0; i < (int)elevators.size(); ++i) {
            if (elevators[i].direction == Direction::IDLE) {
                int d = std::abs(elevators[i].currentFloor - floor);
                if (d < minDistance) {
                    minDistance  = d;
                    chosenIndex  = i;
                }
            }
        }

        // 2) Fallback to any elevator
        if (chosenIndex == -1) {
            for (int i = 0; i < (int)elevators.size(); ++i) {
                int d = std::abs(elevators[i].currentFloor - floor);
                if (d < minDistance) {
                    minDistance  = d;
                    chosenIndex  = i;
                }
            }
        }

        // Sanity check (should never happen if numElevators > 0)
        if (chosenIndex < 0) {
            throw std::logic_error("Failed to select an elevator");
        }

        elevators[chosenIndex].addRequest(floor);
        std::cout << "Request for floor " << floor
                  << " assigned to Elevator "
                  << elevators[chosenIndex].id << "\n";
    }

    void step() {
        for (auto &e : elevators) e.step();
    }

    void displayStatus() const {
        std::cout << "Current Elevator Status:\n";
        for (auto const &e : elevators) {
            std::cout << "  Elevator " << e.id
                      << ": Floor " << e.currentFloor;
            if      (e.doorOpen)              std::cout << " (Doors Open)";
            else if (e.direction == Direction::UP)   std::cout << " (Moving Up)";
            else if (e.direction == Direction::DOWN) std::cout << " (Moving Down)";
            else                                 std::cout << " (Idle)";
            std::cout << "\n";
        }
        std::cout << "-----------------------------\n";
    }
};

int main() {
    const int totalFloors  = 10;
    const int numElevators = 2;
    Building building(totalFloors, numElevators);

    std::string input;
    bool        running = true;

    std::cout << "Elevator Simulation Started.\n"
              << "Enter a floor number (1-" << totalFloors
              << ") to request an elevator, or 'q' to quit.\n\n";

    while (running) {
        building.displayStatus();

        std::cout << "Enter floor request (or 'q' to quit): ";
        std::cin >> input;

        if (input == "q" || input == "Q") {
            running = false;
        }
        else {
            try {
                int requestedFloor = std::stoi(input);
                if (requestedFloor < 1 || requestedFloor > totalFloors) {
                    std::cout << "Invalid floor. Please enter a number between 1 and "
                              << totalFloors << ".\n";
                } else {
                    building.requestElevator(requestedFloor);
                }
            }
            catch (...) {
                std::cout << "Invalid input. Please enter a valid floor number.\n";
            }
        }

        building.step();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\n";
    }

    std::cout << "Elevator Simulation Ended.\n";
    return 0;
}
