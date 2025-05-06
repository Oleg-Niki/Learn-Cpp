#include <iostream>
#include <vector> //hold user requests
#include <thread>
#include <chrono> //thread and chrono are for delay
#include <algorithm> //for vector sorting (for prioritzing user requests)
using namespace std;

class Elevator{
public:
    int id; //to differentiate between multiple elevators
    int currentFloor;
    bool doorOpen;
    bool goingUp;
    bool idle;

    vector<int> upRequests;   //simulate button requests for individuals wanting to go up
    vector<int> downRequests; //simulate button requests for individuals wanting to go down

    Elevator(int elevatorId){
        id = elevatorId;
        currentFloor = 1;
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
            openDoors();
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

    void openDoors(){
        doorOpen = true;
        cout << "\nElevator " << id << ": Doors opening at floor " << currentFloor << endl;
        this_thread::sleep_for(chrono::seconds(3)); //delay for realism
        closeDoors();
    }

    void closeDoors(){
        if(doorOpen){
            doorOpen = false;
            cout << "Elevator " << id << ": Doors closing." << endl;
            this_thread::sleep_for(chrono::seconds(1));
        }
    }

    void showStatus(){ //Provides visual indication for users to understand the current elevator state
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
    int numElev;

    Building(int totFloors, int totElev){
        numFloors = totFloors;
        numElev = totElev;

        for(int i = 0; i < numElev; i++){
            elevators.push_back(Elevator(i + 1));
        }
    };

    void showStatus(){ //status of elevator is shown
        cout << "\n.............. Building Status ..............\n";
        for(int i = 0; i < elevators.size(); i++){
            elevators[i].showStatus();
        }
    }

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

    void getInput(){ //handles user input
        cout << "\nEnter Floor Number, Direction (1 = Up, 0 = Down), and Elevator # (ex. 5 1 1) or -1 to Continue: ";
        int floor, direction, elevatorId;
        cin >> floor;
        
        if(floor == -1){
            return;
        }

        cin >> direction;
        cin >> elevatorId;
        try{
            if(floor >= 1 && floor <= numFloors && elevatorId >= 1 && elevatorId <= numElev && (direction == 0 || direction == 1)){ //checking for valid input
                elevators[elevatorId - 1].addRequest(floor, direction == 1);
            } 
        }
        catch(...){
            cout << "Invalid input! Floors: 1-" << numFloors << ", Direction: 0 or 1, Elevators: 1-" << numElev << endl;
        }
        
    }
};

int main(){
    int floors;
    int elevCnt;

    //setup for elevator/building
    cout << "ENTER THE NUMBER OF FLOORS IN THE BUILDING:" << endl;
    cin >> floors;
    cout << "ENTER THE NUMBER OF ELEVATORS IN THE BUILDING:" << endl;
    cin >> elevCnt;
    cout << "...................................................\n" << endl;
    Building veryAwesomeBuilding(floors, elevCnt);
    
    while(true){
        veryAwesomeBuilding.moveElevators();
        veryAwesomeBuilding.getInput();
        this_thread::sleep_for(chrono::seconds(1));
    }

    return 0;
}