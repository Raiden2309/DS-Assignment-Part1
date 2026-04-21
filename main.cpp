#include <iostream>
#include "Resident.hpp"
#include "ArrayProcessor.hpp"

using namespace std;

int main() {
    Dataset cityA, cityB, cityC;
    cityA.cityName = "City A";
    cityB.cityName = "City B";
    cityC.cityName = "City C";

    cout << "Loading datasets into dynamic arrays..." << endl;

    // Call functions defined in ArrayProcessor.cpp
    loadCSV("dataset1-cityA.csv", cityA);
    loadCSV("dataset2-cityB.csv", cityB);
    loadCSV("dataset3-cityC.csv", cityC);

    cout << "Loading complete.\n" << endl;

    // Run the analysis
    analyzeAndPrintEmissions(cityA);
    analyzeAndPrintEmissions(cityB);
    analyzeAndPrintEmissions(cityC);

	// Sorting and searching examples
    cout << "\n\n**************************************************************" << endl;
    cout << "                 STARTING SEARCH & SORT EXPERIMENTS           " << endl;
    cout << "**************************************************************" << endl;
    searchByTransportMode(cityA, "Bicycle"); //Linear Search
    quickSortByEmission(cityA); //Sorting Data
    sortByAge(cityB); //Insertion Sort
	searchByAgeBinary(cityB, 22); //Binary Search (Requires sorted data by age)


    // Memory cleanup
    delete[] cityA.data;
    delete[] cityB.data;
    delete[] cityC.data;

    return 0;
}