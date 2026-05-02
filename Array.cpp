#include <iostream>
#include <fstream>
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
	city.data = new Resident * [city.size]; // Array of POINTERS

	ifstream file(filename);
	string line;
	getline(file, line); // Skip header

	for (int i = 0; i < city.size; i++) {
		city.data[i] = new Resident(); // Allocate memory for the object

		getline(file, line);

		// We use two markers to track our position in the string
		int startPos = 0;
		int commaPos = 0;

		// 1. Resident ID
		commaPos = line.find(',', startPos);
		city.data[i]->residentID = line.substr(startPos, commaPos - startPos);
		startPos = commaPos + 1; // Move the starting marker past the comma

		// 2. Age
		commaPos = line.find(',', startPos);
		string ageStr = line.substr(startPos, commaPos - startPos);
		city.data[i]->age = stoi(ageStr);
		startPos = commaPos + 1;

		// 3. Mode of Transport
		commaPos = line.find(',', startPos);
		city.data[i]->modeOfTransport = line.substr(startPos, commaPos - startPos);
		startPos = commaPos + 1;

		// 4. Daily Distance
		commaPos = line.find(',', startPos);
		string distStr = line.substr(startPos, commaPos - startPos);
		city.data[i]->dailyDistance = stod(distStr);
		startPos = commaPos + 1;

		// 5. Carbon Emission Factor
		commaPos = line.find(',', startPos);
		string emissionStr = line.substr(startPos, commaPos - startPos);
		city.data[i]->carbonEmissionFactor = stod(emissionStr);
		startPos = commaPos + 1;

		// 6. Average Days Per Month (This is the last item, so there is no comma after it)
		string daysStr = line.substr(startPos);
		city.data[i]->avgDaysPerMonth = stoi(daysStr);

		// Total Emission Calculation
		city.data[i]->totalMonthlyEmissions = city.data[i]->dailyDistance * city.data[i]->carbonEmissionFactor * city.data[i]->avgDaysPerMonth;
	}
}

//Analysis and Display
void analyzeAndPrintEmissions(Dataset& city, int targetAgeGroup) {
	cout << "\n====================================================================================" << endl;
	cout << "Carbon Emission Analysis: " << city.cityName << endl;
	cout << "====================================================================================\n" << endl;

	AgeGroupStats stats[NUM_AGE_GROUPS];
	double cityTotalEmission = 0.0;

	for (int i = 0; i < city.size; i++) {
		int ageIdx = getAgeGroupIndex(city.data[i]->age);
		int modeIdx = getModeIndex(city.data[i]->modeOfTransport);

		if (ageIdx != -1) {
			stats[ageIdx].modes[modeIdx].count++;
			stats[ageIdx].modes[modeIdx].totalEmission += city.data[i]->totalMonthlyEmissions;
			stats[ageIdx].totalGroupEmission += city.data[i]->totalMonthlyEmissions;
			stats[ageIdx].totalGroupCount++;
			cityTotalEmission += city.data[i]->totalMonthlyEmissions;
		}
	}

	for (int i = 0; i < NUM_AGE_GROUPS; i++) {
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

				if (stats[i].modes[m].totalEmission == 0) cout << setw(30) << "0";
				else cout << setw(30) << fixed << setprecision(2) << stats[i].modes[m].totalEmission;

				if (avgEmission == 0) cout << "0\n";
				else cout << fixed << setprecision(2) << avgEmission << "\n";
			}
		}
		cout << string(85, '-') << endl;
		cout << "Total Emission for Age Group: " << fixed << setprecision(2) << stats[i].totalGroupEmission << " kg CO2\n\n";
	}

	if (targetAgeGroup == -1) {
		cout << ">>> TOTAL CARBON EMISSION FOR " << city.cityName << ": " << fixed << setprecision(2) << cityTotalEmission << " kg CO2 <<<\n";
		cout << "====================================================================================\n" << endl;
	}
}

// 3-WAY PARTITION QUICK SORT
void partition3Way(Dataset& city, int low, int high, int sortBy, bool ascending, int& i, int& j) {
	int mid = low + (high - low) / 2;
	Resident* pivot = city.data[mid]; // Pivot is just a pointer now!

	int lt = low;
	int eq = low;
	int gt = high;

	while (eq <= gt) {
		bool isLess = false;
		bool isGreater = false;

		if (sortBy == 1) { // Sort By Age
			if (ascending) {
				if (city.data[eq]->age < pivot->age) isLess = true;
				else if (city.data[eq]->age > pivot->age) isGreater = true;
			}
			else {
				if (city.data[eq]->age > pivot->age) isLess = true;
				else if (city.data[eq]->age < pivot->age) isGreater = true;
			}
		}
		else if (sortBy == 2) { // Sort By Emission
			if (ascending) {
				if (city.data[eq]->totalMonthlyEmissions < pivot->totalMonthlyEmissions) isLess = true;
				else if (city.data[eq]->totalMonthlyEmissions > pivot->totalMonthlyEmissions) isGreater = true;
			}
			else {
				if (city.data[eq]->totalMonthlyEmissions > pivot->totalMonthlyEmissions) isLess = true;
				else if (city.data[eq]->totalMonthlyEmissions < pivot->totalMonthlyEmissions) isGreater = true;
			}
		}

		// Swapping pure 8-byte pointers! No heavy strings copying!
		if (isLess) {
			Resident* temp = city.data[lt];
			city.data[lt] = city.data[eq];
			city.data[eq] = temp;
			lt++;
			eq++;
		}
		else if (isGreater) {
			Resident* temp = city.data[eq];
			city.data[eq] = city.data[gt];
			city.data[gt] = temp;
			gt--;
		}
		else {
			eq++;
		}
	}
	i = lt - 1;
	j = gt + 1;
}

void quickSortHelper(Dataset& city, int low, int high, int sortBy, bool ascending) {
	if (low < high) {
		int i, j;
		partition3Way(city, low, high, sortBy, ascending, i, j);
		quickSortHelper(city, low, i, sortBy, ascending);
		quickSortHelper(city, j, high, sortBy, ascending);
	}
}

void quickSort(Dataset& city, int sortBy, bool ascending) {
	cout << "\nSorting " << city.cityName << " using Quick Sort..." << endl;
	auto start = chrono::high_resolution_clock::now();
	quickSortHelper(city, 0, city.size - 1, sortBy, ascending);
	auto end = chrono::high_resolution_clock::now();
	chrono::duration<double, std::milli> duration = end - start;
	cout << "Quicksort completed in: " << fixed << setprecision(4) << duration.count() << " ms." << endl;
}

//Insertion Sort (Pointer Based)
void insertionSort(Dataset& city, int sortBy, bool ascending) {
	cout << "\nSorting " << city.cityName << " using Insertion Sort.." << endl;
	auto start = chrono::high_resolution_clock::now();

	for (int i = 1; i < city.size; i++) {
		Resident* key = city.data[i];
		int j = i - 1;

		bool moveNeeded = true;
		while (j >= 0 && moveNeeded) {
			moveNeeded = false;

			if (sortBy == 1) {
				if (ascending && city.data[j]->age > key->age) moveNeeded = true;
				if (!ascending && city.data[j]->age < key->age) moveNeeded = true;
			}
			else if (sortBy == 2) {
				if (ascending && city.data[j]->totalMonthlyEmissions > key->totalMonthlyEmissions) moveNeeded = true;
				if (!ascending && city.data[j]->totalMonthlyEmissions < key->totalMonthlyEmissions) moveNeeded = true;
			}
			if (moveNeeded) {
				city.data[j + 1] = city.data[j];
				j = j - 1;
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
		cout << left << setw(15) << city.data[i]->residentID
			<< setw(10) << city.data[i]->age
			<< setw(20) << city.data[i]->modeOfTransport
			<< fixed << setprecision(2) << city.data[i]->totalMonthlyEmissions << " kg CO2" << endl;
	}
}

// 1. Linear Search 
void searchByTransportMode(Dataset& city, string mode) {
	int matchCount = 0;
	auto startTime = chrono::high_resolution_clock::now();

	for (int i = 0; i < city.size; i++) {
		if (city.data[i]->modeOfTransport == mode) matchCount++;
	}

	auto endTime = chrono::high_resolution_clock::now();

	cout << "\n--- LINEAR SEARCH: Finding residents who use '" << mode << "' in " << city.cityName << " ---\n"
		<< string(90, '-') << "\n"
		<< left << setw(14) << "ResidentID" << setw(6) << "Age" << setw(12) << "Transport"
		<< right << setw(10) << "Dist(km)" << setw(16) << "EmissionFactor" << setw(8) << "Days"
		<< setw(25) << "TotalMonthlyEmissions\n" << string(90, '-') << "\n";

	int printedCount = 0;
	for (int i = 0; i < city.size; i++) {
		if (city.data[i]->modeOfTransport == mode) {
			if (printedCount < 5) {
				cout << left << setw(14) << city.data[i]->residentID << setw(6) << city.data[i]->age
					<< setw(12) << city.data[i]->modeOfTransport << right << setw(10) << fixed << setprecision(2) << city.data[i]->dailyDistance
					<< setw(16) << city.data[i]->carbonEmissionFactor << setw(8) << city.data[i]->avgDaysPerMonth
					<< setw(25) << city.data[i]->totalMonthlyEmissions << "\n";
				printedCount++;
			}
		}
	}

	if (matchCount > 5) cout << "... and " << (matchCount - 5) << " more matches hidden.\n";
	if (matchCount == 0) cout << "No residents found using " << mode << ".\n";

	double elapsedMs = chrono::duration<double, milli>(endTime - startTime).count();
	size_t stackMem = sizeof(int) * 3 + sizeof(chrono::steady_clock::time_point) * 2;

	cout << string(90, '-') << "\n" << "Total matches: " << matchCount << "\n"
		<< "Search time  : " << fixed << setprecision(4) << elapsedMs << " ms\n"
		<< "Search Memory: " << stackMem << " bytes (local variables)\n";
}

// 2. Binary Search
void searchByAgeBinary(Dataset& city, int targetAge) {
	int low = 0; int high = city.size - 1;
	int foundIndex = -1; int startIndex = -1; int endIndex = -1;

	auto startTime = chrono::high_resolution_clock::now();

	while (low <= high) {
		int mid = low + (high - low) / 2;
		if (city.data[mid]->age == targetAge) { foundIndex = mid; break; }
		if (city.data[mid]->age < targetAge) low = mid + 1;
		else high = mid - 1;
	}

	if (foundIndex != -1) {
		startIndex = foundIndex;
		while (startIndex > 0 && city.data[startIndex - 1]->age == targetAge) startIndex--;
		endIndex = foundIndex;
		while (endIndex < city.size - 1 && city.data[endIndex + 1]->age == targetAge) endIndex++;
	}

	auto endTime = chrono::high_resolution_clock::now();
	int matchCount = 0;

	cout << "\nSearch Results - Exact Age: " << targetAge << " [" << city.cityName << "]\n"
		<< string(90, '-') << "\n" << left << setw(14) << "ResidentID" << setw(6) << "Age" << setw(12) << "Transport"
		<< right << setw(10) << "Dist(km)" << setw(16) << "EmissionFactor" << setw(8) << "Days"
		<< setw(25) << "TotalMonthlyEmissions\n" << string(90, '-') << "\n";

	if (foundIndex != -1) {
		for (int i = startIndex; i <= endIndex; i++) {
			cout << left << setw(14) << city.data[i]->residentID << setw(6) << city.data[i]->age
				<< setw(12) << city.data[i]->modeOfTransport << right << setw(10) << fixed << setprecision(2) << city.data[i]->dailyDistance
				<< setw(16) << city.data[i]->carbonEmissionFactor << setw(8) << city.data[i]->avgDaysPerMonth
				<< setw(25) << city.data[i]->totalMonthlyEmissions << "\n";
			matchCount++;
		}
	}
	else cout << "No residents found with age " << targetAge << ".\n";

	double elapsedMs = chrono::duration<double, milli>(endTime - startTime).count();
	size_t stackMem = sizeof(int) * 6 + sizeof(chrono::steady_clock::time_point) * 2;

	cout << string(90, '-') << "\n" << "Total matches: " << matchCount << "\n"
		<< "Search time  : " << fixed << setprecision(4) << elapsedMs << " ms\n"
		<< "Search Memory: " << stackMem << " bytes (local variables)\n";
}

void printMemoryUsage(const Dataset& city) {
	size_t totalMemory = (sizeof(Resident*) + sizeof(Resident)) * city.size;
	cout << "Structure Memory Usage: " << city.size << " elements x "
		<< "(" << sizeof(Resident*) << " + " << sizeof(Resident) << ") bytes = "
		<< totalMemory << " bytes (" << fixed << setprecision(2) << totalMemory / 1024.0 << " KB)\n";
}

void ageGroupAnalysis(const Dataset& city) {
	static const int GROUP_COUNT = 5;
	static const char* groupNames[GROUP_COUNT] = {
		"Children & Teenagers (6-17)", "University Students / Young Adults (18-25)",
		"Working Adults Early Career (26-45)", "Working Adults Late Career (46-60)", "Senior Citizens / Retirees (61-100)"
	};
	static const int groupMinAge[GROUP_COUNT] = { 6, 18, 26, 46, 61 };
	static const int groupMaxAge[GROUP_COUNT] = { 17, 25, 45, 60, 100 };

	struct GroupStats {
		double totalEmissions = 0; int residentCount = 0;
		int modeCount[20] = {}; double modeEmissions[20] = {};
	} groupStats[GROUP_COUNT];

	auto startTime = chrono::high_resolution_clock::now();
	for (int i = 0; i < city.size; i++) {
		int residentAge = city.data[i]->age;
		for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
			if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
			groupStats[groupIndex].totalEmissions += city.data[i]->totalMonthlyEmissions;
			groupStats[groupIndex].residentCount++;
			for (int m = 0; m < NUM_MODES; m++) {
				if (city.data[i]->modeOfTransport == TRANSPORT_MODES[m]) {
					groupStats[groupIndex].modeCount[m]++;
					groupStats[groupIndex].modeEmissions[m] += city.data[i]->totalMonthlyEmissions;
					break;
				}
			}
			break;
		}
	}
	auto endTime = chrono::high_resolution_clock::now();

	cout << "Age Group Analysis [" << city.cityName << "]\n";
	for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
		int dominantModeIndex = 0;
		for (int m = 1; m < NUM_MODES; m++)
			if (groupStats[groupIndex].modeCount[m] > groupStats[groupIndex].modeCount[dominantModeIndex]) dominantModeIndex = m;

		cout << "\nAge Group : " << groupNames[groupIndex] << "\n" << string(72, '-') << "\n"
			<< left << setw(18) << "Transport" << right << setw(7) << "Count"
			<< setw(20) << "Total CO2 (kg)" << setw(20) << "Avg CO2/Resident\n" << string(72, '-') << "\n";

		for (int m = 0; m < NUM_MODES; m++) {
			if (groupStats[groupIndex].modeCount[m] == 0) continue;
			cout << left << setw(18) << TRANSPORT_MODES[m] << right << setw(7) << groupStats[groupIndex].modeCount[m]
				<< fixed << setprecision(2) << setw(20) << groupStats[groupIndex].modeEmissions[m]
				<< setw(20) << groupStats[groupIndex].modeEmissions[m] / groupStats[groupIndex].modeCount[m] << "\n";
		}
		double averageEmission = groupStats[groupIndex].residentCount > 0 ? groupStats[groupIndex].totalEmissions / groupStats[groupIndex].residentCount : 0.0;
		cout << string(72, '-') << "\n" << "Most Preferred Transport : " << TRANSPORT_MODES[dominantModeIndex] << "\n"
			<< "Total Emission           : " << fixed << setprecision(2) << groupStats[groupIndex].totalEmissions << " kg CO2\n"
			<< "Average Emission         : " << averageEmission << " kg CO2/resident\n"
			<< "Total Residents          : " << groupStats[groupIndex].residentCount << "\n";
	}
	cout << "\nAnalysis time: " << fixed << setprecision(4) << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
}

void printDatasetSummary(const Dataset& city) {
	double modeEmissionTotals[20] = {}; int modeResidentCounts[20] = {}; double grandTotalEmissions = 0;

	for (int i = 0; i < city.size; i++) {
		grandTotalEmissions += city.data[i]->totalMonthlyEmissions;
		for (int m = 0; m < NUM_MODES; m++) {
			if (city.data[i]->modeOfTransport == TRANSPORT_MODES[m]) {
				modeEmissionTotals[m] += city.data[i]->totalMonthlyEmissions;
				modeResidentCounts[m]++;
				break;
			}
		}
	}

	cout << "\n===== Carbon Emission Summary [" << city.cityName << "] =====\n" << string(60, '-') << "\n"
		<< left << setw(18) << "Mode of Transport" << right << setw(8) << "Count"
		<< setw(20) << "Total CO2 (kg)" << setw(18) << "Avg CO2/User\n" << string(60, '-') << "\n";

	for (int m = 0; m < NUM_MODES; m++) {
		if (modeResidentCounts[m] == 0) continue;
		cout << left << setw(18) << TRANSPORT_MODES[m] << right << setw(8) << modeResidentCounts[m]
			<< fixed << setprecision(2) << setw(20) << modeEmissionTotals[m]
			<< setw(18) << modeEmissionTotals[m] / modeResidentCounts[m] << "\n";
	}
	cout << string(60, '-') << "\n" << "Total Residents  : " << city.size << "\n"
		<< "Total CO2 (month): " << fixed << setprecision(2) << grandTotalEmissions << " kg\n"
		<< "Avg CO2/Resident : " << (city.size > 0 ? grandTotalEmissions / city.size : 0.0) << " kg\n" << string(60, '-') << "\n";
}

void printInsights(const Dataset& city) {
	static const int GROUP_COUNT = 5;
	static const char* groupNames[GROUP_COUNT] = {
		"Children & Teenagers (6-17)", "University Students / Young Adults (18-25)",
		"Working Adults Early Career (26-45)", "Working Adults Late Career (46-60)", "Senior Citizens / Retirees (61-100)"
	};
	static const int groupMinAge[GROUP_COUNT] = { 6, 18, 26, 46, 61 };
	static const int groupMaxAge[GROUP_COUNT] = { 17, 25, 45, 60, 100 };

	struct GroupStats {
		double totalEmissions = 0; int residentCount = 0; int modeCount[20] = {};
	} groupStats[GROUP_COUNT];

	for (int i = 0; i < city.size; i++) {
		int residentAge = city.data[i]->age;
		for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
			if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
			groupStats[groupIndex].totalEmissions += city.data[i]->totalMonthlyEmissions;
			groupStats[groupIndex].residentCount++;
			for (int m = 0; m < NUM_MODES; m++) {
				if (city.data[i]->modeOfTransport == TRANSPORT_MODES[m]) {
					groupStats[groupIndex].modeCount[m]++; break;
				}
			}
			break;
		}
	}

	int carIndex = -1, bicycleIndex = -1;
	for (int i = 0; i < NUM_MODES; i++) {
		if (TRANSPORT_MODES[i] == "Car") carIndex = i;
		if (TRANSPORT_MODES[i] == "Bicycle") bicycleIndex = i;
	}

	int highestEmittingGroup = 0;
	for (int groupIndex = 1; groupIndex < GROUP_COUNT; groupIndex++)
		if (groupStats[groupIndex].totalEmissions > groupStats[highestEmittingGroup].totalEmissions) highestEmittingGroup = groupIndex;

	cout << "\n INSIGHTS & RECOMMENDATIONS [" << city.cityName << "]\n" << string(90, '-') << "\n"
		<< left << setw(46) << "Age Group" << right << setw(10) << "Residents"
		<< setw(18) << "Total CO2 (kg)" << setw(18) << "Avg CO2\n" << string(90, '-') << "\n";

	for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
		double averageEmission = groupStats[groupIndex].residentCount > 0 ? groupStats[groupIndex].totalEmissions / groupStats[groupIndex].residentCount : 0.0;
		cout << left << setw(46) << groupNames[groupIndex] << right << setw(10) << groupStats[groupIndex].residentCount
			<< fixed << setprecision(2) << setw(18) << groupStats[groupIndex].totalEmissions << setw(18) << averageEmission;
		if (groupIndex == highestEmittingGroup) cout << "  << HIGHEST"; cout << "\n";
	}

	cout << string(90, '-') << "\n\nKey Findings:\n"
		<< "  1. Highest emitting group : " << groupNames[highestEmittingGroup] << " (" << fixed << setprecision(2) << groupStats[highestEmittingGroup].totalEmissions << " kg CO2)\n"
		<< "  2. Car vs Bicycle usage by age group:\n     " << left << setw(44) << "Age Group" << right << setw(8) << "Car" << setw(10) << "Bicycle\n";

	for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++)
		cout << "     " << left << setw(44) << groupNames[groupIndex] << right << setw(8) << (carIndex != -1 ? groupStats[groupIndex].modeCount[carIndex] : 0)
		<< setw(10) << (bicycleIndex != -1 ? groupStats[groupIndex].modeCount[bicycleIndex] : 0) << "\n";

	cout << "\nRecommendations:\n" << string(72, '-') << "\n"
		<< "  - " << groupNames[highestEmittingGroup] << " has the highest emissions.\n    Encourage carpooling or public transport for this group.\n"
		<< "  - Groups with high car usage should be targeted with\n    cycling infrastructure and incentives.\n" << string(72, '-') << "\n";
}

void compareWithOtherCity(const Dataset& city1, const Dataset& city2) {
	cout << "\nCROSS-DATASET COMPARISON\n" << string(70, '-') << "\n"
		<< left << setw(20) << "Metric" << setw(25) << city1.cityName << setw(25) << city2.cityName << "\n" << string(70, '-') << "\n";

	auto calculateStats = [](const Dataset& list, double& total, double& avg) {
		total = 0;
		for (int i = 0; i < list.size; i++) total += list.data[i]->totalMonthlyEmissions;
		avg = list.size > 0 ? total / list.size : 0;
		};

	double t1, a1, t2, a2;
	auto startTime = chrono::high_resolution_clock::now();
	calculateStats(city1, t1, a1);
	calculateStats(city2, t2, a2);
	auto endTime = chrono::high_resolution_clock::now();

	cout << left << setw(20) << "Total Residents" << setw(25) << city1.size << setw(25) << city2.size << "\n"
		<< left << setw(20) << "Total CO2 (kg)" << setw(25) << fixed << setprecision(2) << t1 << setw(25) << t2 << "\n"
		<< left << setw(20) << "Avg CO2/Resident" << setw(25) << a1 << setw(25) << a2 << "\n" << string(70, '-') << "\n"
		<< "Comparison time  : " << fixed << setprecision(4) << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
}