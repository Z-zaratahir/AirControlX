#pragma once
#include <iostream>
using namespace std;


struct Runway
{
    string id;
    string type; 
    string status; 
    string currflightID;
    static const string arrival;
    static const string departure;
    static const string flex;
    Runway(string id, string type, string status = "Available", string currflightID = "")
    {
        this->id = id;
        this->type = type;
        this->status = status;
        this->currflightID = currflightID;
    }
    void Assigntheflight(string flightID)
    {
        this->currflightID = flightID;
        this->status = "Occupied";
    }
};



