#ifndef ARRAYPROCESSOR_HPP
#define ARRAYPROCESSOR_HPP

#include "Resident.hpp"
#include <string>

// Function prototypes
int countLines(std::string filename);
void loadCSV(std::string filename, Dataset& city);
void analyzeAndPrintEmissions(Dataset& city);

//Sorting functions
void quickSortByEmission(Dataset& city); // Uses Quick Sort
void sortByAge(Dataset& city);      // Uses Insertion Sort
void printTop5(Dataset& city);

//Searching functions
void searchByTransportMode(Dataset& city, std::string mode); // Linear Search (Unsorted Data)
void searchByAgeBinary(Dataset& city, int targetAge);        // Binary Search (Sorted Data)

#endif