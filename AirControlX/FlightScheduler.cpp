#include "FlightScheduler.h"
#include <iostream>
using namespace std;

void FlightScheduler::addFlight(Flight* flight) {
    
    if (flight->phase == Flight::gate) {
        
        departureQueue.push(flight);
        cout << "[Scheduler] Added " << flight->id << " to departure queue. Priority: " 
                  << flight->priority << "\n";
    } else {
        
        arrivalQueue.push(flight);
        cout << "[Scheduler] Added " << flight->id << " to arrival queue. Priority: " 
                  << flight->priority << "\n";
    }
}

Flight* FlightScheduler::getNextArrival() {
    if (arrivalQueue.empty()) {
        return nullptr;
    }
    
    Flight* next = arrivalQueue.top();
    arrivalQueue.pop();
    cout << "[Scheduler] Retrieving next arrival: " << next->id << "\n";
    return next;
}

Flight* FlightScheduler::getNextDeparture() {
    if (departureQueue.empty()) {
        return nullptr;
    }
    
    Flight* next = departureQueue.top();
    departureQueue.pop();
    cout << "[Scheduler] Retrieving next departure: " << next->id << "\n";
    return next;
}

void FlightScheduler::reorderQueues() {
    
    cout << "[Scheduler] Reordering queues based on priority and wait time\n";
    
    
    
}

void FlightScheduler::calculateWaitTimes() {
    cout << "[Scheduler] Calculating wait times for all flights\n";
    
    
    cout << "[Queue Status] Arrivals: " << arrivalQueue.size() 
              << " | Departures: " << departureQueue.size() << "\n";
              
    
    
}

void FlightScheduler::handleEmergency(Flight* emergency) {
    cout << "[EMERGENCY] Handling emergency for " << emergency->id << "\n";
    
    
    emergency->priority = 3;
    emergency->emergency = true;
    
    
    
    
    
    
} 