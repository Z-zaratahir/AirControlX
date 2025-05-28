#include "AVNManager.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <ctime>
using namespace std;


string AVNManager::formatDateTime(time_t t) {
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&t));
    return string(buf);
}

static int avn_id_counter = 1;

string generateAVNId() {
    ostringstream oss;
    oss << "AVN-" << setw(6) << setfill('0') << avn_id_counter++;
    return oss.str();
}

AVNManager::AVNManager() {
    cout << "AVN Manager Initialized\n";
}

void AVNManager::recordViolation(Flight* flight, const string& violationType) {
    lock_guard<mutex> lock(records_mutex);
    ViolationRecord record;
    record.avnId = generateAVNId();
    record.flightId = flight->id;
    record.airlineName = flight->airlinename;
    record.aircraftType = flight->flightype;
    record.speedRecorded = flight->speed;
    record.speedPermissible = (flight->phase == Flight::cruise) ? 800.0f : (flight->phase == Flight::approach ? 350.0f : 200.0f);
    record.violationType = violationType;
    record.phase = flight->phase;
    record.timestamp = time(nullptr);
    record.processed = false;
    record.penaltyAmount = 0.0f;
    record.paymentStatus = "unpaid";
    record.dueDate = record.timestamp + 3 * 24 * 60 * 60; 
    violationRecords.push_back(record);
    airlineStatsTable[findOrAddAirline(record.airlineName)].violationCount++;
    cout << "[AVN Manager] AVN ID: " << record.avnId << " | Flight: " << flight->id 
              << " (" << flight->airlinename << "): " << violationType << "\n";
}

void AVNManager::processViolations() {
    lock_guard<mutex> lock(records_mutex);
    cout << "[AVN Manager] Processing violations...\n";
    for (int i = 0; i < violationRecords.size(); i++) {
        if (!violationRecords[i].processed) {
            violationRecords[i].penaltyAmount = calculatePenalty(
                violationRecords[i].violationType,
                violationRecords[i].phase,
                violationRecords[i].aircraftType
            );
            airlineStatsTable[findOrAddAirline(violationRecords[i].airlineName)].penaltyTotal += violationRecords[i].penaltyAmount;
            violationRecords[i].processed = true;
            cout << "[AVN Manager] Processed violation for " << violationRecords[i].flightId
                      << " - Penalty: $" << fixed << setprecision(2) << violationRecords[i].penaltyAmount << "\n";
        }
    }
    cout << "[AVN Manager] All violations processed.\n";
}

float AVNManager::calculatePenalty(const string& violationType, const string& phase, const string& aircraftType) {
    float basePenalty = 0.0f;
    string type = aircraftType;
    
    
    if (type == "Commercial") {
        basePenalty = 500000.0f;
    } else if (type == "Cargo") {
        basePenalty = 700000.0f;
    } else if (type == "Military") {
        basePenalty = 1000000.0f;
    }
    
    
    float severityMultiplier = 1.0f;
    if (violationType.find("cruise") != string::npos) {
        severityMultiplier = 1.5f;  
    } else if (violationType.find("approach") != string::npos) {
        severityMultiplier = 2.0f;  
    } else if (violationType.find("landing") != string::npos) {
        severityMultiplier = 2.5f;  
    }
    
    
    if (phase == Flight::cruise) {
        basePenalty *= 1.2f;  
    } else if (phase == Flight::approach || phase == Flight::landing) {
        basePenalty *= 1.5f;  
    }
    
    
    float randomVariation = 0.9f + (rand() % 20) / 100.0f;
    
    return basePenalty * severityMultiplier * randomVariation;
}

void AVNManager::generateAirlineReport(const string& airline) {
    lock_guard<mutex> lock(records_mutex);
    cout << "\n=== AVN REPORT FOR " << airline << " ===\n";
    cout << "Total Violations: " << airlineStatsTable[findOrAddAirline(airline)].violationCount << "\n";
    cout << "Total Penalties: $" << fixed << setprecision(2) << airlineStatsTable[findOrAddAirline(airline)].penaltyTotal << "\n";
    cout << "Violation Details:\n";
    int count = 0;
    for (int i = 0; i < violationRecords.size(); i++) {
        if (violationRecords[i].airlineName == airline) {
            count++;
            cout << count << ". AVN ID: " << violationRecords[i].avnId << " | " << violationRecords[i].flightId 
                 << " (" << violationRecords[i].phase << "): " << violationRecords[i].violationType 
                 << " - $" << violationRecords[i].penaltyAmount
                 << " | Status: " << violationRecords[i].paymentStatus << "\n";
        }
    }
    cout << "==============================\n";
}

void AVNManager::generateSummaryReport() {
    lock_guard<mutex> lock(records_mutex);
    cout << "\n=== AVN SUMMARY REPORT ===\n";
    cout << "Total Violations: " << getTotalViolations() << "\n";
    float totalPenalties = 0.0f;
    for (int i = 0; i < airlineStatsTable.size(); i++) {
        totalPenalties += airlineStatsTable[i].penaltyTotal;
    }
    cout << "Total Penalties: $" << fixed << setprecision(2) << totalPenalties << "\n\n";
    cout << "Airline Breakdown:\n";
    cout << left << setw(20) << "Airline" 
              << right << setw(10) << "Violations" 
              << right << setw(15) << "Penalties ($)" << "\n";
    cout << string(45, '-') << "\n";
    for (int i = 0; i < airlineStatsTable.size(); i++) {
        string airline = airlineStatsTable[i].airlineName;
        int violations = airlineStatsTable[i].violationCount;
        float penalties = airlineStatsTable[i].penaltyTotal;
        cout << left << setw(20) << airline 
                  << right << setw(10) << violations 
                  << right << setw(15) << fixed << setprecision(2) << penalties << "\n";
    }
    cout << "\nAll AVNs:\n";
    int count = 0;
    for (int i = 0; i < violationRecords.size(); i++) {
        count++;
        cout << count << ". AVN ID: " << violationRecords[i].avnId << " | " << violationRecords[i].flightId 
                  << " (" << violationRecords[i].phase << "): " << violationRecords[i].violationType 
                  << " - $" << violationRecords[i].penaltyAmount
                  << " | Status: " << violationRecords[i].paymentStatus << "\n";
    }
    cout << "==============================\n";
}

bool AVNManager::exportViolationsToFile(const string& filename) {
    lock_guard<mutex> lock(records_mutex);
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << " for writing.\n";
        return false;
    }
    file << "AVNID,FlightID(FlightNumber),Airline,ViolationType,Phase,Timestamp,Processed,PenaltyAmount,PaymentStatus,DueDate\n";
    for (int i = 0; i < violationRecords.size(); i++) {
        file << violationRecords[i].avnId << ","
             << violationRecords[i].flightId << ","
             << violationRecords[i].airlineName << ","
             << violationRecords[i].violationType << ","
             << violationRecords[i].phase << ","
             << violationRecords[i].timestamp << ","
             << (violationRecords[i].processed ? "true" : "false") << ","
             << violationRecords[i].penaltyAmount << ","
             << violationRecords[i].paymentStatus << ","
             << formatDateTime(violationRecords[i].dueDate) << "\n";
    }
    file.close();
    cout << "[AVN Manager] Exported " << violationRecords.size() << " violations to " << filename << "\n";
    return true;
}

bool AVNManager::importViolationsFromFile(const string& filename) {
    lock_guard<mutex> lock(records_mutex);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << " for reading.\n";
        return false;
    }
    violationRecords.clear();
    airlineStatsTable.clear();
    string header;
    getline(file, header);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        ViolationRecord record;
        getline(ss, token, ',');
        record.avnId = token;
        getline(ss, token, ',');
        record.flightId = token;
        getline(ss, token, ',');
        record.airlineName = token;
        getline(ss, token, ',');
        record.violationType = token;
        getline(ss, token, ',');
        record.phase = token;
        getline(ss, token, ',');
        record.timestamp = stol(token);
        getline(ss, token, ',');
        record.processed = (token == "true");
        getline(ss, token, ',');
        record.penaltyAmount = stof(token);
        getline(ss, token, ',');
        record.paymentStatus = token;
        getline(ss, token, ',');
        
        
        violationRecords.push_back(record);
        airlineStatsTable[findOrAddAirline(record.airlineName)].violationCount++;
        if (record.processed) {
            airlineStatsTable[findOrAddAirline(record.airlineName)].penaltyTotal += record.penaltyAmount;
        }
    }
    file.close();
    cout << "[AVN Manager] Imported " << violationRecords.size() << " violations from " << filename << "\n";
    checkOverdue();
    return true;
}

int AVNManager::getTotalViolations() const {
    return violationRecords.size();
}

int AVNManager::getAirlineViolations(const string& airline) const {
    for (int i = 0; i < airlineStatsTable.size(); i++) {
        if (airlineStatsTable[i].airlineName == airline) return airlineStatsTable[i].violationCount;
    }
    return 0;
}

float AVNManager::getAirlinePenalties(const string& airline) const {
    for (int i = 0; i < airlineStatsTable.size(); i++) {
        if (airlineStatsTable[i].airlineName == airline) return airlineStatsTable[i].penaltyTotal;
    }
    return 0.0f;
}

vector<string> AVNManager::getViolatingFlights() const {
    vector<string> flightIds;
    for (int i = 0; i < violationRecords.size(); i++) {
        bool found = false;
        for (int j = 0; j < flightIds.size(); j++) {
            if (flightIds[j] == violationRecords[i].flightId) {
                found = true;
                break;
            }
        }
        if (!found) {
            flightIds.push_back(violationRecords[i].flightId);
        }
    }
    return flightIds;
}

bool AVNManager::updatePaymentStatus(const string& avnId, const string& status) {
    lock_guard<mutex> lock(records_mutex);
    for (int i = 0; i < violationRecords.size(); i++) {
        if (violationRecords[i].avnId == avnId) {
            violationRecords[i].paymentStatus = status;
            cout << "[AVN Manager] Payment status for " << avnId << " updated to " << status << "\n";
            if (status == "paid") {
                cout << "[ATC Controller] Violation for AVN " << avnId << " has been cleared.\n";
            }
            return true;
        }
    }
    return false;
}

void AVNManager::checkOverdue() {
    lock_guard<mutex> lock(records_mutex);
    time_t now = time(nullptr);
    for (int i = 0; i < violationRecords.size(); i++) {
        if (violationRecords[i].paymentStatus == "unpaid" && now > violationRecords[i].dueDate) {
            violationRecords[i].paymentStatus = "overdue";
            cout << "[AVN Manager] AVN " << violationRecords[i].avnId << " is now overdue.\n";
        }
    }
}

int AVNManager::findOrAddAirline(const string& airline) {
    for (int i = 0; i < airlineStatsTable.size(); i++) {
        if (airlineStatsTable[i].airlineName == airline) return i;
    }
    airlineStatsTable.push_back({airline, 0, 0.0f});
    return airlineStatsTable.size() - 1;
}

const vector<AVNManager::ViolationRecord>& AVNManager::getViolationRecords() const {
    return violationRecords;
} 