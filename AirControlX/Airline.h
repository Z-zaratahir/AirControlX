#include <iostream>
using namespace std;


struct Airline{
    string id;
    static const string commercial;
    static const string cargo;
    static const string military;
    string type;
    string airlineName;


    Airline(string id, string type, string airlineName)
    {
        this->id = id;
        this->type = type;
        this->airlineName = airlineName;
    }   

    
};

const string Airline::commercial = "Commercial";
const string Airline::cargo = "Cargo";
const string Airline::military = "Military";

