#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <mutex>
#include "Flight.h"
using namespace std;


class AVNManager {
private:
    struct ViolationRecord {
        string avnId;
        string flightId; 
        string airlineName;
        string aircraftType;
        float speedRecorded;
        float speedPermissible;
        string violationType;
        string phase;
        time_t timestamp;
        bool processed;
        float penaltyAmount;
        string paymentStatus; 
        time_t dueDate;
    };

    struct AirlineStats {
        string airlineName;
        int violationCount;
        float penaltyTotal;
    };

    vector<ViolationRecord> violationRecords;
    vector<AirlineStats> airlineStatsTable;
    mutex records_mutex;
    int findOrAddAirline(const string& airline);

public:
    AVNManager();
    
    
    void recordViolation(Flight* flight, const string& violationType);
    
    
    void processViolations();
    
    
    float calculatePenalty(const string& violationType, const string& phase, const string& aircraftType);
    
    
    void generateAirlineReport(const string& airline);
    void generateSummaryReport();
    
    
    bool exportViolationsToFile(const string& filename);
    
    
    bool importViolationsFromFile(const string& filename);
    
    
    int getTotalViolations() const;
    int getAirlineViolations(const string& airline) const;
    float getAirlinePenalties(const string& airline) const;
    
    
    vector<string> getViolatingFlights() const;

    
    bool updatePaymentStatus(const string& avnId, const string& status);
    
    void checkOverdue();

    static string formatDateTime(time_t t);
    const vector<ViolationRecord>& getViolationRecords() const;
}; 