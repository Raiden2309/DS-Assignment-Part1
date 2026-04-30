#ifndef ARRAYPROCESSOR_HPP
#define ARRAYPROCESSOR_HPP

#include "Resident.hpp"
#include <string>

// Function prototypes
int countLines(std::string filename);
void loadCSV(std::string filename, Dataset& city);
void analyzeAndPrintEmissions(Dataset& city, int targetAgeGroup = -1);

//Sorting functions
void quickSort(Dataset& city, int sortBy, bool ascending); // Uses Quick Sort
void insertionSort(Dataset& city, int sortBy, bool ascending);     // Uses Insertion Sort
void printTop5(Dataset& city);

//Searching functions
void searchByTransportMode(Dataset& city, std::string mode); // Linear Search (Unsorted Data)
void searchByAgeBinary(Dataset& city, int targetAge);        // Binary Search (Sorted Data)

// Advanced Analysis & Insights for Arrays
void ageGroupAnalysis(const Dataset& city);
void printDatasetSummary(const Dataset& city);
void printInsights(const Dataset& city);
void compareWithOtherCity(const Dataset& city1, const Dataset& city2);
void printMemoryUsage(const Dataset& city);

#endif