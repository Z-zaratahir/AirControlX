#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <signal.h>
#include <algorithm> 
#include "Flight.h"
#include "Airline.h"
#include "Runway.h"
#include "FlightScheduler.h"
#include "RunwayManager.h"
#include "FlightController.h"
#include "AVNManager.h"
#include <cmath>
#include <SFML/Graphics.hpp>
#include <mutex>
using namespace std;
using namespace sf;
int plane = 1;


Runway runwayA("RWY-A", Runway::arrival);
Runway runwayB("RWY-B", Runway::departure);
Runway runwayC("RWY-C", Runway::flex);
FlightScheduler scheduler;
RunwayManager runwayManager(&runwayA, &runwayB, &runwayC);
FlightController controller;
AVNManager avnManager;


bool simulation_running = true;
bool avn_tracking_active = true;
const int numFlights = 6;

string airlineFlightLimits[numFlights] = {"PIA","AirBlue","FedEx","Pakistan Airforce","Blue Dart","AghaKhan Air Ambulance"};
int airlineFlightLimitsNum[numFlights] = {4,4,2,1,2,1};
int airlineFlightCountsNum[numFlights] = {0,0,0,0,0,0};

struct AirlineFlight {
    string name;
    float altitude;
    float speed; 
    Color color;
    Vector2f position;
    Vector2f direction; 
    string phase;      
    string runway;     
};

int findFlightIndex(string airlineName) {
    for (int i = 0; i < numFlights; i++) {
        if (airlineFlightLimits[i] == airlineName) {
            return i;
        }
    }
    return -1;
}

string getAssignedRunwayForFlight(const string& flightId) {
    if (runwayA.currflightID == flightId) return runwayA.id;
    if (runwayB.currflightID == flightId) return runwayB.id;
    if (runwayC.currflightID == flightId) return runwayC.id;
    return "";
}


vector<AirlineFlight> g_flights;
mutex g_flights_mutex;

void drawPlaneSprite(RenderWindow& window, const AirlineFlight& flight) {
    
    ConvexShape body;
    body.setPointCount(3);
    body.setPoint(0, Vector2f(0, 0));
    body.setPoint(1, Vector2f(40, 10));
    body.setPoint(2, Vector2f(0, 20));
    body.setFillColor(flight.color);
    body.setPosition(flight.position);
    window.draw(body);
    
    RectangleShape tail(Vector2f(10, 4));
    tail.setFillColor(Color(100, 100, 100));
    tail.setPosition(flight.position.x - 8, flight.position.y + 8);
    window.draw(tail);
}

void showGraphicsSFMLLive() {
    RenderWindow window(VideoMode(1200, 800), "Air Traffic Control Simulation");
    Font font;
    font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
    Clock clock;
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
        }
        float dt = clock.restart().asSeconds();
        window.clear(Color(20, 40, 60));
        
        RectangleShape rwyA(Vector2f(900, 40));
        rwyA.setPosition(150, 200);
        rwyA.setFillColor(Color(80, 90, 100));
        window.draw(rwyA);
        RectangleShape rwyB(Vector2f(900, 40));
        rwyB.setPosition(150, 400);
        rwyB.setFillColor(Color(80, 90, 100));
        window.draw(rwyB);
        RectangleShape rwyC(Vector2f(900, 40));
        rwyC.setPosition(150, 600);
        rwyC.setFillColor(Color(80, 90, 100));
        window.draw(rwyC);
        
        Text rwyALabel("RWY-A", font, 24); rwyALabel.setPosition(60, 200); window.draw(rwyALabel);
        Text rwyBLabel("RWY-B", font, 24); rwyBLabel.setPosition(60, 400); window.draw(rwyBLabel);
        Text rwyCLabel("RWY-C", font, 24); rwyCLabel.setPosition(60, 600); window.draw(rwyCLabel);
        
        g_flights_mutex.lock();
        for (const auto& flight : g_flights) {
            drawPlaneSprite(window, flight);
            
            string labelStr = flight.name + 
                "\nAlt: " + to_string((int)flight.altitude) +
                "\nSpd: " + to_string((int)flight.speed) +
                "\n" + flight.phase +
                (flight.runway.empty() ? "" : ("\n" + flight.runway));
            Text label(labelStr, font, 16);
            label.setFillColor(Color::White);
            label.setPosition(flight.position.x + 45, flight.position.y - 10);
            window.draw(label);
        }
        g_flights_mutex.unlock();

        window.display();
    }
}

void updateAirlineFlightInGraphics(const Flight* flight) {
    lock_guard<mutex> lock(g_flights_mutex);
    for (auto& af : g_flights) {  
        if (af.name == flight->id) {
            af.altitude = flight->altitude;
            af.speed = flight->speed;
            af.phase = flight->phase;
            af.runway = getAssignedRunwayForFlight(flight->id);
            af.position.x += (rand() % 30);
            
            break;
        }
    }
}


void* simulateFlight(void* arg) {
    Flight* flight = (Flight*)arg;
    flight->start_time = time(0);
    
    cout << "\n[Simulating Flight] " << flight->id << " (" << flight->flightype << ")\n";
    
    bool faulty = false;
    
    while (flight->status != "Completed" && simulation_running) {
        
        if ((flight->phase == Flight::taxi || flight->phase == Flight::gate) && !faulty) {
            if (rand() % 100 < 5) { 
                faulty = true;
                cout << "[FAULT] " << flight->id << " encountered a ground fault. Towing and removing from queue.\n";
                flight->status = "Faulty";
                break; 
            }
        }
        
        controller.simulateFlightMovement(flight);
        updateAirlineFlightInGraphics(flight);
        
        
        if (flight->AVN_Active && avn_tracking_active) {
            
            string violationType = "unknown";
            if (flight->phase == Flight::cruise) {
                violationType = "cruise speed violation";
            } else if (flight->phase == Flight::approach) {
                violationType = "approach speed violation";
            } else if (flight->phase == Flight::landing) {
                violationType = "landing speed violation";
            }
            
            
            avnManager.recordViolation(flight, violationType);
            
            
            flight->AVN_Active = false;
        }
        
        
        if (controller.isPhaseComplete(flight)) {
            controller.transitionToNextPhase(flight);
        }
        
        
        sleep(2);
    }
    
    flight->end_time = time(0);
    if (flight->status == "Faulty") {
        cout << "[Flight Removed] " << flight->id << " was towed due to ground fault.\n";
    } else {
        cout << "[Flight Complete] " << flight->id << " | Total Time: "
             << difftime(flight->end_time, flight->start_time) << " sec"
             << (flight->AVN_Active ? " | [AVN Issued]" : "") << "\n";
    }
    
    {
        lock_guard<mutex> lock(g_flights_mutex);
        g_flights.erase(
            remove_if(g_flights.begin(), g_flights.end(),
                [&](const AirlineFlight& af) { return af.name == flight->id; }),
            g_flights.end()
        );
    }
    
    pthread_exit(nullptr);
}


void* generateFlights(void* arg) {
    string direction = *(string*)arg;
    int interval = 0;
    
    
    if (direction == Flight::north) interval = 3 * 5; 
    else if (direction == Flight::south) interval = 2 * 5;
    else if (direction == Flight::east) interval = 2.5 * 5;
    else if (direction == Flight::west) interval = 4 * 5;
    
    time_t start = time(0);
    
    while (difftime(time(0), start) < 120 && simulation_running) { 
        Flight* newflight = new Flight();
        Flight tempFlight(Flight::getflight(direction));
        *newflight = move(tempFlight);
        newflight->scheduledTime = time(0);
        
        
        if (airlineFlightCountsNum[findFlightIndex(newflight->airlinename)] >= airlineFlightLimitsNum[findFlightIndex(newflight->airlinename)]) {
            cout << "Flight limit reached for airline: " << newflight->airlinename << ". Skipping this flight.\n";
            continue;
        }
        
        
        scheduler.addFlight(newflight);
        
        
        if (runwayManager.assignRunway(newflight)) {
            
            pthread_t tid;
            pthread_create(&tid, nullptr, simulateFlight, newflight);
            pthread_detach(tid);
            
            
            airlineFlightCountsNum[findFlightIndex(newflight->airlinename)]++;

            
            lock_guard<mutex> lock(g_flights_mutex);
            AirlineFlight af;
            af.name = newflight->id;
            af.altitude = newflight->altitude;
            af.speed = newflight->speed;
            af.color = Color(rand() % 256, rand() % 256, rand() % 256);
            
            if(plane == 1){
                af.position = Vector2f(60, 200); 
                ++plane;
            }
            else if(plane == 2){
                af.position = Vector2f(60, 400); 
                ++plane;
            }
            else if(plane == 3){
                af.position = Vector2f(60, 600); 
                plane = 1;
            }
            af.direction = Vector2f(0, 1); 
            af.phase = newflight->phase;
            af.runway = getAssignedRunwayForFlight(newflight->id);
            g_flights.push_back(af);
        }
        
        
        sleep(interval);
    }
    
    pthread_exit(nullptr);
}


void* processAVNs(void* arg) {
    time_t start = time(0);
    int processInterval = 30; 
    
    while (simulation_running) {
        sleep(processInterval);
        
        
        if (avn_tracking_active) {
            avnManager.processViolations();
        }
    }
    
    pthread_exit(nullptr);
}


void handle_signal(int sig) {
    if (sig == SIGINT) {
        cout << "\n[Signal] Received termination signal. Wrapping up simulation...\n";
        simulation_running = false;
    }
}


void runSimulation() {
    thread sfmlThread(showGraphicsSFMLLive);
    cout << "Starting AirControlX Simulation...\n\n";
    
    
    signal(SIGINT, handle_signal);
    
    
    pthread_t northThread, southThread, eastThread, westThread, avnThread;
    string north = Flight::north;
    string south = Flight::south;
    string east = Flight::east;
    string west = Flight::west;
    
    pthread_create(&northThread, nullptr, generateFlights, &north);
    pthread_create(&southThread, nullptr, generateFlights, &south);
    pthread_create(&eastThread, nullptr, generateFlights, &east);
    pthread_create(&westThread, nullptr, generateFlights, &west);
    
    
    pthread_create(&avnThread, nullptr, processAVNs, nullptr);
    
    
    if (!cargoflightflag) {
        cout << "\n[Force Generating Cargo Flight]\n";
        Flight* cargo = new Flight("FedEx-999", "Cargo", "FedEx", 0.0, 
                                  Flight::east, Flight::gate, 2, "InQueue", false);
        cargo->scheduledTime = time(0);
        scheduler.addFlight(cargo);
        
        if (runwayManager.assignRunway(cargo)) {
            pthread_t tid;
            pthread_create(&tid, nullptr, simulateFlight, cargo);
            pthread_detach(tid);
        }
    }
    
    
    time_t start = time(0);
    while (difftime(time(0), start) < 120 && simulation_running) { 
        
        if ((int)difftime(time(0), start) % 30 == 0) {
            scheduler.calculateWaitTimes();
        }
        sleep(1);
    }
    
    cout << "\nSimulation ending, please wait for threads to finish...\n";
    simulation_running = false;
    
    
    pthread_join(northThread, nullptr);
    pthread_join(southThread, nullptr);
    pthread_join(eastThread, nullptr);
    pthread_join(westThread, nullptr);
    pthread_join(avnThread, nullptr);
    
    
    cout << "\n============= AVN REPORTS =============\n";
    avnManager.generateSummaryReport();
    
    
    vector<string> airlines = {"PIA", "AirBlue", "FedEx", "Blue Dart", "Pakistan Airforce"};
    for (const auto& airline : airlines) {
        if (avnManager.getAirlineViolations(airline) > 0) {
            avnManager.generateAirlineReport(airline);
        }
    }
    
    
    avnManager.exportViolationsToFile("avn_violations.csv");
    
    cout << "\nSimulation complete.\n";
    
    
    sfmlThread.join();
}


void airlinePortal(AVNManager& avnManager) {
    string aircraftId;
    string avnDate;
    vector<string> allFlightIds;
    for (int i = 0; i < avnManager.getViolatingFlights().size(); i++) {
        allFlightIds.push_back(avnManager.getViolatingFlights()[i]);
    }
    string aircraftIdLower; 
    while (true) {
        cout << "\n[Airline Portal] Enter Aircraft ID (Flight Number, e.g., AirBlue-777): ";
        cin >> aircraftId;
        if (aircraftId.empty()) {
            cout << "Aircraft ID cannot be empty. Please enter a valid Aircraft ID from the CSV.\n";
            continue;
        }
        aircraftIdLower = aircraftId;
        transform(aircraftIdLower.begin(), aircraftIdLower.end(), aircraftIdLower.begin(), ::tolower);
        bool found = false;
        for (int i = 0; i < allFlightIds.size(); i++) {
            string fidLower = allFlightIds[i];
            transform(fidLower.begin(), fidLower.end(), fidLower.begin(), ::tolower);
            if (fidLower == aircraftIdLower) {
                found = true;
                break;
            }
        }
        if (!found) {
            cout << "Invalid Aircraft ID. Please enter a valid Aircraft ID from the CSV.\n";
            continue;
        }
        break;
    }
    cout << "Enter AVN Issue Date (YYYY-MM-DD) or 'all' to show all: ";
    cin >> avnDate;
    cout << "\nAVNs for " << aircraftId << ":\n";
    cout << "(Note: Only 'unpaid' or 'overdue' AVNs can be paid. 'paid' AVNs are for record only.)\n";
    int avnCount = 0;
    vector<string> validAvnIds;
    for (int i = 0; i < avnManager.getViolationRecords().size(); i++) {
        string recIdLower = avnManager.getViolationRecords()[i].flightId;
        transform(recIdLower.begin(), recIdLower.end(), recIdLower.begin(), ::tolower);
        if (recIdLower == aircraftIdLower) {
            string dateStr = AVNManager::formatDateTime(avnManager.getViolationRecords()[i].timestamp).substr(0, 10);
            if (avnDate == "all" || avnDate == dateStr) {
                avnCount++;
                validAvnIds.push_back(avnManager.getViolationRecords()[i].avnId);
                cout << avnCount << ". AVN ID: " << avnManager.getViolationRecords()[i].avnId
                     << " | Date: " << dateStr
                     << " | Type: " << avnManager.getViolationRecords()[i].aircraftType
                     << " | Status: " << avnManager.getViolationRecords()[i].paymentStatus
                     << " | Fine: " << avnManager.getViolationRecords()[i].penaltyAmount << endl;
            }
        }
    }
    if (avnCount == 0) {
        cout << "No AVNs found for this Aircraft ID and date.\n";
        return;
    }
    string avnId;
    while (true) {
        cout << "Enter AVN ID to pay (or 'none' to skip): ";
        cin >> avnId;
        if (avnId == "none") break;
        bool valid = false;
        for (int i = 0; i < validAvnIds.size(); i++) {
            if (avnId == validAvnIds[i]) {
                valid = true;
                break;
            }
        }
        if (!valid) {
            cout << "Invalid AVN ID. Please enter a valid AVN ID from the list above.\n";
            continue;
        }
        cout << "Simulating payment for " << avnId << "...\n";
        avnManager.updatePaymentStatus(avnId, "paid");
        avnManager.exportViolationsToFile("avn_violations.csv");
        break;
    }
}


void stripePay(AVNManager& avnManager) {
    string avnId;
    while (true) {
        cout << "\n[StripePay] Enter AVN ID to pay (e.g., AVN-000123): ";
        cin >> avnId;
        
        bool found = false;
        for (int i = 0; i < avnManager.getViolationRecords().size(); i++) {
            if (avnManager.getViolationRecords()[i].avnId == avnId) {
                found = true;
                cout << "AVN ID: " << avnManager.getViolationRecords()[i].avnId << endl;
                cout << "Aircraft ID: " << avnManager.getViolationRecords()[i].flightId << endl;
                cout << "Aircraft Type: " << avnManager.getViolationRecords()[i].aircraftType << endl;
                cout << "Fine Amount: " << avnManager.getViolationRecords()[i].penaltyAmount << endl;
                break;
            }
        }
        if (!found) {
            cout << "AVN not found. Please check the CSV for valid AVN IDs.\n";
            continue;
        }
        avnManager.updatePaymentStatus(avnId, "paid");
        avnManager.exportViolationsToFile("avn_violations.csv");
        cout << "Payment processed for " << avnId << ".\n";
        break;
    }
}



int main() {
    cout << "========================\n";
    cout << " AirControlX Simulation \n";
    cout << "========================\n\n";
    
    
    runSimulation();
    
    
    char more = 'y';
    while (true) {
        cout << "\nDo you want to access the Airline Portal or StripePay? (a/s/n): ";
        char opt; cin >> opt;
        if (opt == 'a') airlinePortal(avnManager);
        else if (opt == 's') stripePay(avnManager);
        else if (opt == 'n' || opt == 'N') break;
        else {
            cout << "Invalid option. Please enter 'a', 's', or 'n'.\n";
            continue;
        }
        while (true) {
            cout << "Do you want to perform another action? (Y/N): ";
            cin >> more;
            if (more == 'y' || more == 'Y') break;
            else if (more == 'n' || more == 'N') return 0;
            else {
                cout << "Wrong input, input again." << endl;
            }
        }
    }
    
    return 0;
}
