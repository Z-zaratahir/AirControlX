#pragma once
#include <mutex>
#include "Runway.h"
#include "Flight.h"
using namespace std;

class RunwayManager {
private:
    Runway* runwayA;
    Runway* runwayB;
    Runway* runwayC;
    mutex runwayA_mutex;
    mutex runwayB_mutex;
    mutex runwayC_mutex;
    
public:
    RunwayManager(Runway* a, Runway* b, Runway* c);
    bool assignRunway(Flight* flight);
    void releaseRunway(Runway* runway);
    bool isRunwayAvailable(Runway* runway);
    Runway* getBackupRunway(); 
};
