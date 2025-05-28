#pragma once
#include "Flight.h"
using namespace std;
class FlightController {
private:
    void updateSpeed(Flight* flight);
    void checkViolations(Flight* flight);
    
public:
    void transitionToNextPhase(Flight* flight);
    void simulateFlightMovement(Flight* flight);
    bool isPhaseComplete(Flight* flight);
    void handlePriorityFlight(Flight* flight); 
};
