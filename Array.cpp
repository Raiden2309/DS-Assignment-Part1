#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include "Resident.hpp"
#include "ArrayProcessor.hpp"

using namespace std;

//Constants for categorization
const string TRANSPORT_MODES[] = { "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool" };
const int NUM_MODES = 6;

const string AGE_GROUPS[] = {
	"6 - 17: Children and Teenagers",
	"18 - 25: University Students / Young Adults",
	"26-45: Working Adults (Early Career)",
	"46-60: Working Adults (Late Career)",
	"61-100: Senior Citizens / Retirees"
};

const int NUM_AGE_GROUPS = 5;

//Structures
struct TransportStats {
	int count = 0;
	double totalEmission = 0.0;
};

struct AgeGroupStats {
	TransportStats modes[NUM_MODES];
	double totalGroupEmission = 0.0;
	int totalGroupCount = 0;
};

//Helper Functions
int getModeIndex(string mode) {
	for (int i = 0; i < NUM_MODES; i++) {
		if (mode == TRANSPORT_MODES[i]) return i;
	}
	return 0; //Default fallback
}

int getAgeGroupIndex(int age) {
	if (age >= 6 && age <= 17) return 0;
	if (age >= 18 && age <= 25) return 1;
	if (age >= 26 && age <= 45) return 2;
	if (age >= 46 && age <= 60) return 3;
	if (age >= 61 && age <= 100) return 4;
	return -1; // Out of range
}

//File loading logic
//Function to count lines to determine dynamic array size
int countLines(string filename) {
	ifstream file(filename);
	string line;
	int count = 0;
	getline(file, line); //Skip header
	while (getline(file, line)) count++;
	return count;
}

void loadCSV(string filename, Dataset& city) {
	city.size = countLines(filename);
	city.data = new Resident[city.size]; //Dynamic array allocation

	ifstream file(filename);
	string line, word;
	getline(file, line); //Skip header

	for (int i = 0; i < city.size; i++) {
		getline(file, line);
		stringstream ss(line);

		getline(ss, city.data[i].residentID, ',');
		getline(ss, word, ','); city.data[i].age = stoi(word);
		getline(ss, city.data[i].modeOfTransport, ',');
		getline(ss, word, ','); city.data[i].dailyDistance = stod(word);
		getline(ss, word, ','); city.data[i].carbonEmissionFactor = stod(word);
		getline(ss, word, ','); city.data[i].avgDaysPerMonth = stoi(word);

		//Total Emission Calculation
		city.data[i].totalMonthlyEmissions = city.data[i].dailyDistance * city.data[i].carbonEmissionFactor * city.data[i].avgDaysPerMonth;
	}
}

//Analysis and Display
void analyzeAndPrintEmissions(Dataset& city, int targetAgeGroup) {
	cout << "\n====================================================================================" << endl;
	cout << "Carbon Emission Analysis: " << city.cityName << endl;
	cout << "====================================================================================\n" << endl;

	AgeGroupStats stats[NUM_AGE_GROUPS];
	double cityTotalEmission = 0.0;

	// 1. Process data and calculate totals
	for (int i = 0; i < city.size; i++) {
		int ageIdx = getAgeGroupIndex(city.data[i].age);
		int modeIdx = getModeIndex(city.data[i].modeOfTransport);

		if (ageIdx != -1) {
			stats[ageIdx].modes[modeIdx].count++;
			stats[ageIdx].modes[modeIdx].totalEmission += city.data[i].totalMonthlyEmissions;
			stats[ageIdx].totalGroupEmission += city.data[i].totalMonthlyEmissions;
			stats[ageIdx].totalGroupCount++;
			cityTotalEmission += city.data[i].totalMonthlyEmissions;
		}
	}

	// 2. Print the formatted table matching the screenshot
	for (int i = 0; i < NUM_AGE_GROUPS; i++) {
		// Skip age groups if the user selected a different specific one
		if (targetAgeGroup != -1 && i != targetAgeGroup) continue;
		if (stats[i].totalGroupCount == 0) continue;

		cout << "Age Group: " << AGE_GROUPS[i] << endl;
		cout << string(85, '-') << endl;
		cout << left << setw(22) << "Mode of Transport"
			<< setw(10) << "Count"
			<< setw(30) << "Total Emission (kg CO2)"
			<< "Average per Resident" << endl;

		for (int m = 0; m < NUM_MODES; m++) {
			if (stats[i].modes[m].count > 0) {
				double avgEmission = stats[i].modes[m].totalEmission / stats[i].modes[m].count;

				cout << left << setw(22) << TRANSPORT_MODES[m]
					<< setw(10) << stats[i].modes[m].count;

				// Format to match screenshot (print "0" instead of "0.00" for empty values)
				if (stats[i].modes[m].totalEmission == 0) {
					cout << setw(30) << "0";
				}
				else {
					cout << setw(30) << fixed << setprecision(2) << stats[i].modes[m].totalEmission;
				}

				if (avgEmission == 0) {
					cout << "0\n";
				}
				else {
					cout << fixed << setprecision(2) << avgEmission << "\n";
				}
			}
		}
		cout << string(85, '-') << endl;
		cout << "Total Emission for Age Group: " << fixed << setprecision(2) << stats[i].totalGroupEmission << " kg CO2\n\n";
	}

	// Only show the grand total if they are viewing ALL age groups
	if (targetAgeGroup == -1) {
		cout << ">>> TOTAL CARBON EMISSION FOR " << city.cityName << ": " << fixed << setprecision(2) << cityTotalEmission << " kg CO2 <<<\n";
		cout << "====================================================================================\n" << endl;
	}
}

//Quick Sort
//1. Quicksort Helper (Partitioning for Descending Order by Emission)
int partition(Dataset& city, int low, int high, int sortBy, bool ascending) {
	Resident pivot = city.data[high];
	int i = (low - 1);

	for (int j = low; j < high; j++) {
		bool swapNeeded = false;

		if (sortBy == 1) { //1 - Sort By Age
			if (ascending) {
				if (city.data[j].age < pivot.age) swapNeeded = true;
			}
			else {
				if (city.data[j].age > pivot.age) swapNeeded = true;
			}
		}
		else if (sortBy == 2) { //2 - Sort By Emission
			if (ascending) {
				if (city.data[j].totalMonthlyEmissions < pivot.totalMonthlyEmissions) swapNeeded = true;
			}
			else {
				if (city.data[j].totalMonthlyEmissions > pivot.totalMonthlyEmissions) swapNeeded = true;
			}
		}
		if (swapNeeded) {
			i++;
			Resident temp = city.data[i];
			city.data[i] = city.data[j];
			city.data[j] = temp;
		}
	}
	Resident temp = city.data[i + 1];
	city.data[i + 1] = city.data[high];
	city.data[high] = temp;

	return (i + 1);
}

//2. Quicksort Recursive Helper
void quickSortHelper(Dataset& city, int low, int high, int sortBy, bool ascending) {
	if (low < high) {
		int pi = partition (city, low, high, sortBy, ascending);
		quickSortHelper(city, low, pi - 1, sortBy, ascending);
		quickSortHelper(city, pi + 1, high, sortBy, ascending);
	}
}

//3. Quicksort Main Function
void quickSort(Dataset& city, int sortBy, bool ascending) {
	cout << "\nSorting " << city.cityName << " using Quick Sort..." << endl;

	auto start = chrono::high_resolution_clock::now();

	quickSortHelper(city, 0, city.size - 1, sortBy, ascending);

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;

	cout << "Quicksort completed in: " << fixed << setprecision(4) << duration.count() << " ms." << endl;
}

//Insertion Sort
//Insertion Sort (Ascending Order by Age)
void insertionSort(Dataset& city, int sortBy, bool ascending) {
	cout << "\nSorting " << city.cityName << " using Insertion Sort.." << endl;

	auto start = chrono::high_resolution_clock::now();

	for (int i = 1; i < city.size; i++) {
		Resident key = city.data[i];
		int j = i - 1;

		bool moveNeeded = true;
		while (j >= 0 && moveNeeded) {
			moveNeeded = false;

			if (sortBy == 1) { //1 - Sort By Age
				if (ascending && city.data[j].age > key.age)
					moveNeeded = true;
				if (!ascending && city.data[j].age < key.age)
					moveNeeded = true;
			} else if (sortBy == 2) { //2 - Sort By Emission
				if (ascending && city.data[j].totalMonthlyEmissions > key.totalMonthlyEmissions)
					moveNeeded = true;
				if (!ascending && city.data[j].totalMonthlyEmissions < key.totalMonthlyEmissions)
					moveNeeded = true;
			}
			if (moveNeeded) {
				city.data[j + 1] = city.data[j];
				j= j -1;
			}
		}
		city.data[j + 1] = key;
	}

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;

	cout << "Insertion Sort completed in: " << fixed << setprecision(4) << duration.count() << " ms." << endl;
}

//Helper Function to Display Sorted Results
void printTop5(Dataset& city) {
	cout << "\nTop 5 Results for " << city.cityName << ":" << endl;
	cout << left << setw(15) << "Resident ID" << setw(10) << "Age"
		<< setw(20) << "Mode of Transport" << "Total Emission" << endl;
	cout << string(65, '-') << endl;

	int limit = (city.size < 5) ? city.size : 5;
	for (int i = 0; i < limit; i++) {
		cout << left << setw(15) << city.data[i].residentID
			<< setw(10) << city.data[i].age
			<< setw(20) << city.data[i].modeOfTransport
			<< fixed << setprecision(2) << city.data[i].totalMonthlyEmissions << " kg CO2" << endl;
	}
}

// 1. Linear Search (Best for unsorted data)
void searchByTransportMode(Dataset& city, string mode) {
	cout << "\n--- LINEAR SEARCH: Finding residents who use '" << mode << "' in " << city.cityName << " ---" << endl;

	auto start = chrono::high_resolution_clock::now();

	int matchCount = 0;
	for (int i = 0; i < city.size; i++) {
		if (city.data[i].modeOfTransport == mode) {
			if (matchCount < 5) { // Print only first 5 to avoid console spam
				if (matchCount == 0) {
					cout << left << setw(15) << "Resident ID" << setw(10) << "Age" << setw(20) << "Mode of Transport" << endl;
					cout << string(45, '-') << endl;
				}
				cout << left << setw(15) << city.data[i].residentID
					<< setw(10) << city.data[i].age
					<< setw(20) << city.data[i].modeOfTransport << endl;
			}
			matchCount++;
		}
	}

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;

	if (matchCount > 5) cout << "... and " << (matchCount - 5) << " more matches hidden." << endl;
	if (matchCount == 0) cout << "No residents found using " << mode << "." << endl;

	cout << "Total found: " << matchCount << endl;
	cout << "Linear Search Execution Time: " << fixed << setprecision(4) << duration.count() << " ms.\n" << endl;
}

// 2. Binary Search (Requires data to be sorted by Age first)
void searchByAgeBinary(Dataset& city, int targetAge) {
	cout << "\n--- BINARY SEARCH: Finding a resident aged " << targetAge << " in " << city.cityName << " ---" << endl;

	auto start = chrono::high_resolution_clock::now();

	int left_idx = 0;
	int right_idx = city.size - 1;
	int firstMatch = -1;

	while (left_idx <= right_idx) {
		int mid = left_idx + (right_idx - left_idx) / 2;
		if (city.data[mid].age == targetAge) {
			firstMatch = mid;
			break; // Match found
		}
		else if (city.data[mid].age < targetAge) {
			left_idx = mid + 1;
		}
		else {
			right_idx = mid - 1;
		}
	}

	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;

	if (firstMatch != -1) {
		cout << "Match found! Resident ID: " << city.data[firstMatch].residentID
			<< " | Age: " << city.data[firstMatch].age
			<< " | Mode: " << city.data[firstMatch].modeOfTransport << endl;
	}
	else {
		cout << "No residents found with age " << targetAge << "." << endl;
	}

	cout << "Binary Search Execution Time: " << fixed << setprecision(4) << duration.count() << " ms.\n" << endl;
}