#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <string>

// Enumeration for elevator direction
enum class Direction { IDLE, UP, DOWN };

// Elevator class that simulates basic elevator behavior
class Elevator {
public:
    int id;
    int currentFloor;
    int targetFloor;  // -1 means no active request
    Direction direction;
    bool doorOpen;

    Elevator(int elevatorId, int startFloor = 1)
        : id(elevatorId), currentFloor(startFloor), targetFloor(-1), direction(Direction::IDLE), doorOpen(false) {}

    // Add a floor request and determine the direction
    void addRequest(int floor) {
        targetFloor = floor;
        if (targetFloor > currentFloor)
            direction = Direction::UP;
        else if (targetFloor < currentFloor)
            direction = Direction::DOWN;
        else {
            // Already at the requested floor
            direction = Direction::IDLE;
            openDoors();
            closeDoors();
            targetFloor = -1;
        }
    }

    // Simulate one time step of the elevator's operation
    void step() {
        // If no active request, remain idle
        if (targetFloor == -1) {
            direction = Direction::IDLE;
            return;
        }

        // Move the elevator one floor in the current direction
        if (direction == Direction::UP) {
            currentFloor++;
        } else if (direction == Direction::DOWN) {
            currentFloor--;
        }

        // Check if the target is reached
        if (currentFloor == targetFloor) {
            openDoors();
            // Simulate door open delay for realism
            std::this_thread::sleep_for(std::chrono::seconds(1));
            closeDoors();
            // Clear the request
            targetFloor = -1;
            direction = Direction::IDLE;
        }
    }

    // Simulate door opening
    void openDoors() {
        doorOpen = true;
        std::cout << "Elevator " << id << " reached floor " << currentFloor << " -- Doors Open\n";
    }

    // Simulate door closing
    void closeDoors() {
        doorOpen = false;
        std::cout << "Elevator " << id << " doors closing...\n";
    }
};

// Building class to manage multiple elevators and process incoming requests
class Building {
private:
    int floors;
    std::vector<Elevator> elevators;

public:
    Building(int totalFloors, int numElevators)
        : floors(totalFloors) {
        // Initialize elevators at floor 1 by default
        for (int i = 0; i < numElevators; i++) {
            elevators.emplace_back(i + 1, 1);
        }
    }

    // Assign a request to an elevator (simple scheduling: choose the idle elevator closest to the requested floor)
    void requestElevator(int floor) {
        int chosenIndex = -1;
        int minDistance = floors + 1;

        // First, look for an idle elevator
        for (int i = 0; i < elevators.size(); i++) {
            if (elevators[i].direction == Direction::IDLE) {
                int distance = std::abs(elevators[i].currentFloor - floor);
                if (distance < minDistance) {
                    chosenIndex = i;
                    minDistance = distance;
                }
            }
        }
        // If no idle elevator is available, choose the one with the minimum distance overall.
        if (chosenIndex == -1) {
            for (int i = 0; i < elevators.size(); i++) {
                int distance = std::abs(elevators[i].currentFloor - floor);
                if (distance < minDistance) {
                    chosenIndex = i;
                    minDistance = distance;
                }
            }
        }

        elevators[chosenIndex].addRequest(floor);
        std::cout << "Request for floor " << floor << " assigned to Elevator " 
                  << elevators[chosenIndex].id << "\n";
    }

    // Advance the simulation by one step for all elevators
    void step() {
        for (auto &elevator : elevators) {
            elevator.step();
        }
    }

    // Display the current state of all elevators
    void displayStatus() {
        std::cout << "Current Elevator Status:\n";
        for (const auto &elevator : elevators) {
            std::cout << "Elevator " << elevator.id << ": Floor " << elevator.currentFloor;
            if (elevator.doorOpen)
                std::cout << " (Doors Open)";
            else if (elevator.direction == Direction::UP)
                std::cout << " (Moving Up)";
            else if (elevator.direction == Direction::DOWN)
                std::cout << " (Moving Down)";
            else
                std::cout << " (Idle)";
            std::cout << "\n";
        }
        std::cout << "-----------------------------\n";
    }

    int getTotalFloors() const {
        return floors;
    }
};

int main() {
    // Configuration for the building: 10 floors and 2 elevators (you can adjust these values)
    int totalFloors = 10;
    int numElevators = 2;
    Building building(totalFloors, numElevators);

    std::string input;
    bool running = true;

    std::cout << "Elevator Simulation Started.\n";
    std::cout << "Enter a floor number (1-" << totalFloors << ") to request an elevator, or 'q' to quit.\n\n";

    // Main simulation loop (step-based)
    while (running) {
        building.displayStatus();

        std::cout << "Enter floor request (or 'q' to quit): ";
        std::cin >> input;

        if (input == "q" || input == "Q") {
            running = false;
            continue;
        }

        try {
            int requestedFloor = std::stoi(input);
            if (requestedFloor < 1 || requestedFloor > totalFloors) {
                std::cout << "Invalid floor. Please enter a number between 1 and " << totalFloors << ".\n";
            } else {
                building.requestElevator(requestedFloor);
            }
        } catch (const std::exception &e) {
            std::cout << "Invalid input. Please enter a valid floor number.\n";
        }

        // Simulate a time step for all elevators (with a delay for realism)
        building.step();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "\n";
    }

    std::cout << "Elevator Simulation Ended.\n";
    return 0;
}
