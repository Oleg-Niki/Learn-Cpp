//Oleg Nikitashin and Jake Sakamoto
//Date: 05/05/2025
//Elevator Project

#include <iostream>
#include <vector> //hold user requests
#include <thread> //thread and chrono are for delay
#include <chrono> //thread and chrono are for delay
#include <algorithm> //for vector sorting (for prioritzing user requests)
#include <sstream>
using namespace std;

class Elevator{
public:
    int id; //to differentiate between multiple elevators
    int currentFloor;
    bool doorOpen;
    bool goingUp;
    bool idle;

    vector<int> upRequests; //simulate button requests for individuals wanting to go up
    vector<int> downRequests; //simulate button requests for individuals wanting to go down

    Elevator(int elevatorId){
      id = elevatorId;
      currentFloor = 1; //default 1st floor
      doorOpen = false;
      goingUp = true;
      idle = true; //if no user requests elevator
    }

    void addRequest(int floor, bool goingUp){ //add user request to vector
        bool reqExists = false; //checking for duplicate requests
        
        if(goingUp){
            for(int i = 0; i < upRequests.size(); i++){
                if(upRequests[i] == floor){
                    reqExists = true;
                    break; //break if another matching request is present, then it will not add to the vector
                }
            }
            if(!reqExists){ //if unique new request, then will add to vector for processing 
                upRequests.push_back(floor);
                sortUpRequests();
            }
        } 
        else{ //handles going down
            for(int i = 0; i < downRequests.size(); i++){
                if(downRequests[i] == floor){
                    reqExists = true;
                    break;
                }
            }
            if(!reqExists){ //similar to upRequests, if it is a unique new request, then will add to vector for processing 
                downRequests.push_back(floor);
                sortDownRequests();
            }
        }
    }

    void sortUpRequests(){ //sorting floors to prioritize when ascending
        sort(upRequests.begin(), upRequests.end()); //sorts vector from least to greatest
    }

    void sortDownRequests(){ //sorting floors to priotize when descending
        sort(downRequests.begin(), downRequests.end());
        reverse(downRequests.begin(), downRequests.end()); //reverses sort from greatest to least
    }

    void move(){ //elevator logic to control up/down movement
        if(upRequests.empty() && downRequests.empty()){
            idle = true;
            return; //if no requests to go up or down (in other words, idle)
        }
        else{
            idle = false;
        }

        //checking direction and switching when necessary
        if(goingUp && upRequests.empty() && !downRequests.empty()){ //if elevator is going up and there are no more up requests, however down requests are present, then will switch directions
            goingUp = false;
        } 
        else if(!goingUp && downRequests.empty() && !upRequests.empty()){ //if elevator is going down and no more down requests, however up requests are present, then will switch direction
            goingUp = true;
        }

        int targetFloor;
        if(goingUp && !upRequests.empty()){ //elevator is going up and there are requests for up
            targetFloor = upRequests[0]; //looking at closest floor to go to since it was sorted in the vectory already
        } 
        else if(!goingUp && !downRequests.empty()){ //elevator is going down and there are requests for down
            targetFloor = downRequests[0];
        } 
        else{
            return; //idle
        }

        if(currentFloor == targetFloor){ //destination reached
            if(goingUp){
                upRequests.erase(upRequests.begin()); //most recent request for up(first in line) gets removed from vector
            } 
            else{
                downRequests.erase(downRequests.begin()); //most recent request for down (first in line) gets removed from vector
            }
        } 
        else if(targetFloor > currentFloor){
            currentFloor++; //increment up to correct level
        } 
        else{
            currentFloor--; //increment down to correct level
        }
    }

    void showStatus()const{ //Provides visual indication for users to understand the current elevator state
        cout << "Elevator " << id << ": Floor " << currentFloor << " | ";
        if(idle){
            cout << "Idle";
        } 
        else if(goingUp){
            cout << "Going Up";
        }
        else{
            cout << "Going Down";
        }
        cout << " | Doors ";
        if (doorOpen){
            cout << "Open";
        }
        else{
            cout << "Closed";
        }
        cout << endl;
    }
};

class Building{
public:
    vector<Elevator> elevators;
    int numFloors;

    Building(int totFloors, int numElev){
        numFloors = totFloors;

        for(int i = 0; i < numElev; i++){
            elevators.push_back(Elevator(i + 1)); //Want to start with Elevator 1 instead of Elevator 0
        }
    };

    void moveElevators(){  //checks state of elevator - to see if any of the elevators are moving (to be able to update status)
        bool elevIsMoving = false; 
        for(int i = 0; i < elevators.size(); i++){ //iterate through the multiple elevators present
            int prevFloor = elevators[i].currentFloor;
            elevators[i].move();
            if(prevFloor != elevators[i].currentFloor){
                elevIsMoving = true;
            }
        }
        if(elevIsMoving){ //updates the status display when elevator goes to a new floor
            showStatus();
        }
    }

    void heading(int floor){ //logic to determine the closest available elevator (depending on how many elevators are configured)
        int freeElevator = -1; //default -1 if elevator not found, will hold the index of the closest elevator
        int bestDist = numFloors + 1; //used to calculate shortest dist

        for(int i = 0; i < elevators.size(); i++){ 
            if(elevators[i].idle){ //checking if idle
                int currDistance = abs(elevators[i].currentFloor - floor); //calculating shortest dist
                if(currDistance < bestDist){ //if a elevator has a closer dist than the current set one, then update the bestDist and freeElevator vairable
                    bestDist = currDistance;
                    freeElevator  = i;
                }
            }
        }

        if(freeElevator < 0){ //if all elevators are operatnig, will default to closest operating elevator -- the reason why you want to do this is because if there is an elevator that is not operating, we want to put that to use before requesting one that's already moving
            for(int i = 0; i < elevators.size(); i++){
                int currDistance = abs(elevators[i].currentFloor - floor);
                if(currDistance < bestDist){
                    bestDist = currDistance;
                    freeElevator  = i;
                }
            }
        }
        elevators[freeElevator].addRequest(floor, floor > elevators[freeElevator].currentFloor); //the second paramter will yield a true/false depending on if the elevator is head up or down
        cout << "Elevator " << (freeElevator + 1) << " is now headed to floor " << floor << "\n";
    }

    string showStatus() const{ //shows status of elevator via output string stream
        ostringstream out;
        out << "\n.............. Building Status ..............\n";
        
        for (int i = 0; i < elevators.size(); ++i) {
            out << "Elevator " << elevators[i].id << ": Floor " << elevators[i].currentFloor << " | ";
            if (elevators[i].idle){
                out << "Idle";
            } 
            else if (elevators[i].goingUp){
                out << "Going Up";
            }
            else{
                out << "Going Down";
            }
            
            out << " | Doors ";
            if (elevators[i].doorOpen){
                out << "Open\n";
            }
            else {
                out << "Closed\n";
            }
        }
        return out.str(); //takes the data and puts it into a single string -- need this for the javascript for GUI
    }
};

int main() {
    //this whole part is to validate that the code runs successfully on vscode without any errors
    // int floors = 10;
    // int elevCnt = 1;
    // Building veryAwesomeBuilding(floors, elevCnt);

    // while(true){
    //     veryAwesomeBuilding.moveElevators();
    //     cout << "Enter floor to call (or -1 to skip): ";
    //     int floor; 
    //     cin >> floor;
    //     if(floor >= 1 && floor <= floors){
    //         veryAwesomeBuilding.heading(floor);
    //     }
    //     this_thread::sleep_for(chrono::seconds(1));
    // }
    // return 0;
}

//the part below focuses on WebAssembly Wrappers for C linkage
static Building globalBuilding(10, 2); //global building to make sure javascript knows the class exists while running

extern "C"{ //this is to make sure names stay consistent

    void addRequest(int floor){ //javascript's call for an elevator, we will then use the method we created
        globalBuilding.heading(floor);
    }

    void stepSimulation(){ //takes javascript's call and uses our cpp method to make elevator move
        globalBuilding.moveElevators();
    }

    const char* getStatus(){ //allows javascript to access the current elevator state, i.e. the output string
        static string elevStatus;
        elevStatus = globalBuilding.showStatus();
        return elevStatus.c_str(); //converting to a format javascript can understand
    }
}
