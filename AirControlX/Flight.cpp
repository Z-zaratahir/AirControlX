#include "Flight.h"
using namespace std;


const string Flight::north = "North";
const string Flight::south = "South";
const string Flight::east = "East";
const string Flight::west = "West";
const string Flight::gate = "Gate";
const string Flight::taxi = "Taxi";
const string Flight::takeoff = "Takeoff";
const string Flight::cruise = "Cruise";
const string Flight::holding = "Holding";
const string Flight::approach = "Approach";
const string Flight::landing = "Landing";


const int maximumflights = 100;
Flight flights[maximumflights];
int flightcounter = 0;
bool cargoflightflag = false;

string AirlineArray[] = {"PIA", "AirBlue", "FedEx", "Blue Dart", "Pakistan Airforce", "AghaKhan Air Ambulance"};


Flight Flight::getflight(const string &direction)
{
    string airline = fetchingrandairline();
    bool emergency = (rand() % 100 < getEmergencyProbability(direction));
    string type = Aircrafttype(airline, emergency);//e.g. "Commercial"

    if (type == "Cargo")
    {
        cargoflightflag = true; 
    }

    string id = airline + "-" + to_string(rand() % 1000);
    float speed = 0.0;
    int priority = settingprority(type, emergency);

    Flight fl(id, type, airline, speed, direction, Flight::gate, priority, "InQueue", emergency);

    //logging
    cout << "[New Flight Created] " << fl.id
         << " | Type: " << fl.flightype
         << " | Direction: " << fl.direction
         << " | Emergency: " << (fl.emergency ? "Yes" : "No")
         << " | Priority: " << fl.priority
         << " | Airline: " << fl.airlinename << endl;

    return fl;
}

string fetchingrandairline()
{
    int i = 0;
    i = rand() % 6;
    return AirlineArray[i];
}

string Aircrafttype(const string &airline, bool emergency)
{
    if (emergency)
        return "Military";
    if (airline == "FedEx" || airline == "Blue Dart")
        return "Cargo";
    if (airline == "Pakistan Airforce")
        return "Military";

    return "Commercial";
}

int settingprority(const string &type, bool emergency)
{
    if (emergency)
        return 3;
    if (type == "Cargo")
        return 2;
    return 1;
}

int getEmergencyProbability(const string& dir) {
    if (dir == Flight::north) return 10;
    if (dir == Flight::south) return 5;
    if (dir == Flight::east) return 15;
    if (dir == Flight::west) return 20;
    return 10;
}

void flightprocess()
{
    srand(time(0));
    time_t start = time(0);
    time_t current;
    int sleepsecs = 5;

    while (true)
    {
        current = time(0);
        if (difftime(current, start) >= 300)
            break; 
        if (flightcounter >= maximumflights)
            break;

        
        string directions[4] = {Flight::north, Flight::south, Flight::east, Flight::west};
        string randdirection = directions[rand() % 4];

        Flight newflight = Flight::getflight(randdirection);
        flights[flightcounter++].copy(newflight.id, newflight.flightype,
                                        newflight.airlinename, newflight.speed,
                                        newflight.direction, newflight.phase,
                                        newflight.priority, newflight.status,
                                        newflight.emergency, newflight.spawningtime,
                                        newflight.AVN_Active, newflight.estimatedWaitTime,
                                        newflight.scheduledTime, newflight.queuePosition,
                                        newflight.isPreempted);

        sleep(sleepsecs); 
    }

    
    if (!cargoflightflag && flightcounter < maximumflights)
    {
        cout << "\n[Force Generating Cargo Flight]\n";
        Flight cargo("FedEx-999", "Cargo", "FedEx", 0.0, Flight::east, Flight::gate, 2, "InQueue", false);
        flights[flightcounter++].copy(cargo.id, cargo.flightype,
                                        cargo.airlinename, cargo.speed,
                                        cargo.direction, cargo.phase,
                                        cargo.priority, cargo.status,
                                        cargo.emergency, cargo.spawningtime,
                                        cargo.AVN_Active, cargo.estimatedWaitTime,
                                        cargo.scheduledTime, cargo.queuePosition,
                                        cargo.isPreempted);;
        cout << "[Flight Created] " << cargo.id << " | Type: " << cargo.flightype
             << " | Direction: " << cargo.direction << " | Airline: " << cargo.airlinename << endl;
    }

    cout << "\nSimulation complete. Total flights: " << flightcounter << endl;
} 