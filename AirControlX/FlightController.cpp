#include "FlightController.h"
#include <iostream>
#include <cstdlib>
using namespace std;

void FlightController::updateSpeed(Flight* flight) {
    
    if (flight->phase == Flight::takeoff) {
        flight->speed += 50.0;
    } else if (flight->phase == Flight::cruise) {
        flight->speed = 800.0; 
        
        
        if (rand() % 100 < 10) {
            flight->speed = 400.0 + (rand() % 100);  
            cout << "[Speed Adjustment] " << flight->id << " speed adjusted to " << flight->speed << " for simulation\n";
        }
    } else if (flight->phase == Flight::landing) {
        flight->speed -= 100.0;
        if (flight->speed < 0) flight->speed = 0;
    }
}

void FlightController::checkViolations(Flight* flight) {
    bool violation = false;
    string violationType = "";
    float speed = flight->speed;
    
    if (flight->phase == Flight::holding) {
        
        if (speed < 400.0f || speed > 600.0f) {
            violation = true;
            violationType = "holding speed violation";
        }
    } else if (flight->phase == Flight::approach) {
        
        if (speed < 240.0f || speed > 290.0f) {
            violation = true;
            violationType = "approach speed violation";
        }
    } else if (flight->phase == Flight::landing) {
        
        if (speed > 240.0f) {
            violation = true;
            violationType = "landing speed too high";
        } else if (speed > 30.0f && flight->status == "Completed") {
            
            violation = true;
            violationType = "landing not decelerated below 30";
        }
    } else if (flight->phase == Flight::taxi) {
        
        if (speed < 15.0f || speed > 30.0f) {
            violation = true;
            violationType = "taxi speed violation";
        }
    } else if (flight->phase == Flight::gate) {
        
        if (speed > 10.0f) {
            violation = true;
            violationType = "gate speed violation";
        }
    } else if (flight->phase == Flight::takeoff) {
        
        if (speed > 290.0f) {
            violation = true;
            violationType = "takeoff roll speed violation";
        }
    } else if (flight->phase == "climb") {
        
        if (speed < 250.0f || speed > 463.0f) {
            violation = true;
            violationType = "climb speed violation";
        }
    } else if (flight->phase == Flight::cruise) {
        
        if (speed < 800.0f || speed > 900.0f) {
            violation = true;
            violationType = "cruise speed violation";
        }
    }
    
    float minAlt = 30000.0, maxAlt = 40000.0;
    if (flight->altitude < minAlt || flight->altitude > maxAlt) {
        violation = true;
        violationType = "altitude violation";
    }
    
    if (rand() % 100 < 5) {
        violation = true;
        violationType = "airspace boundary violation";
    }
    
    if (violation) {
        flight->AVN_Active = true;
        flight->violationStatus = "active";
        cout << "[VIOLATION] " << flight->id << ": " << violationType << " (" << speed << ")\n";
    } else if (flight->violationStatus == "active") {
        flight->violationStatus = "cleared";
    }
}

void FlightController::transitionToNextPhase(Flight* flight) {
    
    if (flight->phase == Flight::gate) {
        flight->phase = Flight::taxi;
        cout << "[Phase Change] " << flight->id << " Gate -> Taxi\n";
    } else if (flight->phase == Flight::taxi) {
        flight->phase = Flight::takeoff;
        cout << "[Phase Change] " << flight->id << " Taxi -> Takeoff\n";
    } else if (flight->phase == Flight::takeoff) {
        flight->phase = Flight::cruise;
        cout << "[Phase Change] " << flight->id << " Takeoff -> Cruise\n";
    } else if (flight->phase == Flight::cruise) {
        flight->phase = Flight::approach;
        cout << "[Phase Change] " << flight->id << " Cruise -> Approach\n";
    } else if (flight->phase == Flight::approach) {
        flight->phase = Flight::landing;
        cout << "[Phase Change] " << flight->id << " Approach -> Landing\n";
    } else if (flight->phase == Flight::landing) {
        flight->phase = Flight::taxi;
        cout << "[Phase Change] " << flight->id << " Landing -> Taxi\n";
    } else if (flight->phase == Flight::holding) {
        flight->phase = Flight::approach;
        cout << "[Phase Change] " << flight->id << " Holding -> Approach\n";
    } else {
        flight->status = "Completed";
        cout << "[Phase Change] " << flight->id << " Taxi -> Completed\n";
    }
}

void FlightController::simulateFlightMovement(Flight* flight) {
    
    updateSpeed(flight);
    
    
    checkViolations(flight);
    
    
    cout << "[Flight Status] " << flight->id 
              << " | Phase: " << flight->phase 
              << " | Speed: " << flight->speed 
              << " | Direction: " << flight->direction << "\n";
}

bool FlightController::isPhaseComplete(Flight* flight) {
    
    
    return (rand() % 100) < 30; 
}

void FlightController::handlePriorityFlight(Flight* flight) {
    
    if (flight->priority > 1) {
        cout << "[Priority Handling] " << flight->id << " given priority treatment\n";
        
    }
} 