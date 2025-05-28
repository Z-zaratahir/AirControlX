#pragma once
#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
using namespace std;


string fetchingrandairline();
string Aircrafttype(const string &airline, bool emergency);
int settingprority(const string &type, bool emergency);
int getEmergencyProbability(const string& dir);
extern bool cargoflightflag; 

struct Flight
{
    static const string north;
    static const string south;
    static const string east;
    static const string west;
    
    time_t estimatedWaitTime; 
    time_t scheduledTime;     
    int queuePosition;        
    bool isPreempted;         
    string id;
    string flightype;
    string airlinename;
    float speed;
    string direction;
    string phase;
    int priority;
    string status; 
    time_t spawningtime;
    bool emergency;
    static const string gate;
    static const string taxi;
    static const string takeoff;
    static const string cruise;
    static const string holding;
    static const string approach;
    static const string landing;
    bool AVN_Active = false;
    time_t start_time;
    time_t end_time;
    float altitude = 0.0; 
    string position = ""; 
    string violationStatus = "none"; 

    Flight() {}  
    
    Flight(const Flight& other) {
        this->id = other.id;
        this->flightype = other.flightype;
        this->airlinename = other.airlinename;
        this->speed = other.speed;
        this->direction = other.direction;
        this->phase = other.phase;
        this->priority = other.priority;
        this->status = other.status;
        this->spawningtime = other.spawningtime;
        this->emergency = other.emergency;
        this->AVN_Active = other.AVN_Active;
        
        
        this->estimatedWaitTime = 0;
        this->scheduledTime = time(0);
        this->queuePosition = 0;
        this->isPreempted = false;
    }

    Flight(string id, string flightype, string airlinename, float speed,
       string direction, string phase, int priority,
       string status = "InQueue", bool emergency = false)
    {
        this->id = id;
        this->flightype = flightype;
        this->airlinename = airlinename;
        this->speed = speed;
        this->direction = direction;
        this->phase = phase;
        this->priority = priority;
        this->status = status;
        this->spawningtime = time(0);
        this->emergency = emergency;
        this->estimatedWaitTime = 0;
        this->scheduledTime = time(0);
        this->queuePosition = 0;
        this->isPreempted = false;
    }

    
    Flight& operator=(const Flight& other) {
        if (this != &other) { 
            
            this->id = other.id;
            this->flightype = other.flightype;
            this->airlinename = other.airlinename;
            this->speed = other.speed;
            this->direction = other.direction;
            this->phase = other.phase;
            this->priority = other.priority;
            this->status = other.status;
            this->spawningtime = other.spawningtime;
            this->emergency = other.emergency;
            this->AVN_Active = other.AVN_Active;
            this->estimatedWaitTime = other.estimatedWaitTime;
            this->scheduledTime = other.scheduledTime;
            this->queuePosition = other.queuePosition;
            this->isPreempted = other.isPreempted;
        }
        return *this;
    }

    void copy(const string id, const string flightype, const string airlinename,
                float speed, const string direction, const string phase,
                int priority, const string status, bool emergency,
                time_t spawningtime, bool AVN_Active,
                time_t estimatedWaitTime, time_t scheduledTime,
                int queuePosition, bool isPreempted)
    {
        this->id = id;
        this->flightype = flightype;
        this->airlinename = airlinename;
        this->speed = speed;
        this->direction = direction;
        this->phase = phase;
        this->priority = priority;
        this->status = status;
        this->spawningtime = spawningtime;
        this->emergency = emergency;
        this->AVN_Active = AVN_Active;
        this->estimatedWaitTime = estimatedWaitTime;
        this->scheduledTime = scheduledTime;
        this->queuePosition = queuePosition;
        this->isPreempted = isPreempted;
    }

    static Flight getflight(const string &direction);
};


extern const int maximumflights;
extern Flight flights[];
extern int flightcounter;
extern string AirlineArray[];


void flightprocess();
