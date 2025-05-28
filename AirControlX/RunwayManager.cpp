#include "RunwayManager.h"
#include <iostream>
using namespace std;

RunwayManager::RunwayManager(Runway* a, Runway* b, Runway* c) {
    this->runwayA = a;
    this->runwayB = b;
    this->runwayC = c;
    
    cout << "[RunwayManager] Initialized with runways:\n";
    cout << "  Runway A: " << a->id << " (" << a->type << ")\n";
    cout << "  Runway B: " << b->id << " (" << b->type << ")\n";
    cout << "  Runway C: " << c->id << " (" << c->type << ")\n";
}

bool RunwayManager::assignRunway(Flight* flight) {
    cout << "[RunwayManager] Attempting to assign runway for " << flight->id << "\n";
    Runway* targetRunway = nullptr;
    mutex* targetMutex = nullptr;
    
    bool isCargo = (flight->flightype == "Cargo");
    bool isEmergency = flight->emergency;
    
    if (isEmergency || isCargo) {
        
        if (runwayC_mutex.try_lock()) {
            if (runwayC->status == "Available") {
                targetRunway = runwayC;
                targetMutex = &runwayC_mutex;
            } else {
                runwayC_mutex.unlock();
            }
        }
        
        if (!targetRunway && runwayA_mutex.try_lock()) {
            if (runwayA->status == "Available") {
                targetRunway = runwayA;
                targetMutex = &runwayA_mutex;
            } else {
                runwayA_mutex.unlock();
            }
        }
        if (!targetRunway && runwayB_mutex.try_lock()) {
            if (runwayB->status == "Available") {
                targetRunway = runwayB;
                targetMutex = &runwayB_mutex;
            } else {
                runwayB_mutex.unlock();
            }
        }
    } else {
        
        if (flight->phase == Flight::gate) {
            
            if (runwayB_mutex.try_lock()) {
                if (runwayB->status == "Available") {
                    targetRunway = runwayB;
                    targetMutex = &runwayB_mutex;
                } else {
                    runwayB_mutex.unlock();
                }
            }
        } else {
            
            if (runwayA_mutex.try_lock()) {
                if (runwayA->status == "Available") {
                    targetRunway = runwayA;
                    targetMutex = &runwayA_mutex;
                } else {
                    runwayA_mutex.unlock();
                }
            }
        }
        
        if (!targetRunway && runwayC_mutex.try_lock()) {
            if (runwayC->status == "Available") {
                targetRunway = runwayC;
                targetMutex = &runwayC_mutex;
            } else {
                runwayC_mutex.unlock();
            }
        }
    }
    if (targetRunway) {
        targetRunway->Assigntheflight(flight->id);
        cout << "[RunwayManager] Assigned " << targetRunway->id << " to " << flight->id << "\n";
        targetMutex->unlock();
        return true;
    } else {
        cout << "[RunwayManager] No suitable runway available for " << flight->id << "\n";
        return false;
    }
}

void RunwayManager::releaseRunway(Runway* runway) {
    cout << "[RunwayManager] Releasing runway " << runway->id << "\n";
    
    mutex* targetMutex = nullptr;
    if (runway == runwayA) {
        targetMutex = &runwayA_mutex;
    } else if (runway == runwayB) {
        targetMutex = &runwayB_mutex;
    } else if (runway == runwayC) {
        targetMutex = &runwayC_mutex;
    }
    
    if (targetMutex) {
        targetMutex->lock();
        runway->status = "Available";
        runway->currflightID = "";
        targetMutex->unlock();
        cout << "[RunwayManager] Runway " << runway->id << " is now available\n";
    }
}

bool RunwayManager::isRunwayAvailable(Runway* runway) {
    mutex* targetMutex = nullptr;
    if (runway == runwayA) {
        targetMutex = &runwayA_mutex;
    } else if (runway == runwayB) {
        targetMutex = &runwayB_mutex;
    } else if (runway == runwayC) {
        targetMutex = &runwayC_mutex;
    }
    
    bool available = false;
    if (targetMutex) {
        targetMutex->lock();
        available = (runway->status == "Available");
        targetMutex->unlock();
    }
    
    return available;
}

Runway* RunwayManager::getBackupRunway() {
    
    cout << "[RunwayManager] Searching for backup runway\n";
    
    
    if (isRunwayAvailable(runwayC)) {
        return runwayC;
    }
    
    
    if (isRunwayAvailable(runwayA)) {
        return runwayA;
    }
    
    if (isRunwayAvailable(runwayB)) {
        return runwayB;
    }
    
    return nullptr; 
} 