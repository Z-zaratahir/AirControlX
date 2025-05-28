#pragma once
#include <queue>
#include <vector>
#include "Flight.h"
using namespace std;

class FlightScheduler {
private:
    
    struct CompareFlightPriority {
        bool operator()(Flight* a, Flight* b) {
            
            if (a->priority != b->priority)
                return a->priority < b->priority;
            
            return a->scheduledTime > b->scheduledTime;
        }
    };
    
    priority_queue<Flight*, vector<Flight*>, CompareFlightPriority> arrivalQueue;
    priority_queue<Flight*, vector<Flight*>, CompareFlightPriority> departureQueue;
    
public:
    void addFlight(Flight* flight);
    Flight* getNextArrival();
    Flight* getNextDeparture();
    void reorderQueues(); 
    void calculateWaitTimes(); 
    void handleEmergency(Flight* emergency); 
};
