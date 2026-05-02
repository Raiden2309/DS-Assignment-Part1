#ifndef RESIDENT_HPP
#define RESIDENT_HPP

#include <string>

struct Resident {
	std::string residentID;
	int age;
	std::string modeOfTransport;
	double dailyDistance;
	double carbonEmissionFactor;
	int avgDaysPerMonth;
	double totalMonthlyEmissions; //calculation: distance * factor * days
};

//Prototypes for array experiments
struct Dataset {
	std::string cityName;
	int size;
	Resident** data;
};

#endif

