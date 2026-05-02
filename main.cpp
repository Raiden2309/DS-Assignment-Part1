#include <iostream>
#include <string>
#include <limits>
#include <iomanip>	
#include "Resident.hpp"
#include "ArrayProcessor.hpp"
#include "linked_list_quick_sort.hpp"

using namespace std;

void clearInput() {
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void displayMenu(bool useLinkedList) {
	const string label = useLinkedList ? "LINKED LIST" : "ARRAY";
	cout << "\n==========================================================" << endl;
	cout << "    CARBON EMISSION TRACKING SYSTEM  [" << label << "]      " << endl;
	cout << "==========================================================" << endl;
	cout << "1. View Carbon Emission Analysis" << endl;
	cout << "2. Sort City Data" << endl;
	cout << "3. Search City Data" << endl;
	cout << "4. Analysis & Insights" << endl; // Now always visible
	cout << "5. Return to Main Menu" << endl;
	cout << "==========================================================" << endl;
	cout << "Enter your choice (1-5): ";
}

int selectCityMenu(bool allowAllOption = false) {
	int choice = 0;
	while (true) {
		cout << "\n --- Select City ---" << endl;
		cout << "1. City A" << endl;
		cout << "2. City B" << endl;
		cout << "3. City C" << endl;
		if (allowAllOption) cout << "4. All Cities" << endl;
		cout << "Enter your choice: ";

		cin >> choice;
		if (cin.fail() || choice < 1 || choice >(allowAllOption ? 4 : 3)) {
			clearInput();
			cout << "Invalid input. Please enter a valid number." << endl;
		}
		else {
			return choice;
		}
	}
}

Dataset* getCityArray(int choice, Dataset& a, Dataset& b, Dataset& c) {
	if (choice == 1) return &a;
	if (choice == 2) return &b;
	if (choice == 3) return &c;
	return nullptr;
}

LinkedList* getCityLL(int choice, LinkedList& a, LinkedList& b, LinkedList& c) {
	if (choice == 1) return &a;
	if (choice == 2) return &b;
	if (choice == 3) return &c;
	return nullptr;
}

static const int AGE_MIN[] = { 6, 18, 26, 46, 61 };
static const int AGE_MAX[] = { 17, 25, 45, 60, 100 };

int main() 
{ 
	bool running = true;

	while (running) {

		int dsChoice = 0;
		cout << "\n==========================================================" << endl;
		cout << "         CARBON EMISSION TRACKING SYSTEM                  " << endl;
		cout << "==========================================================" << endl;
		cout << "Select Data Structure:" << endl;
		cout << "1. Array" << endl;
		cout << "2. Linked List" << endl;
		cout << "3. Exit Program" << endl;
		cout << "==========================================================" << endl;
		cout << "Enter your choice (1-3): ";

		while (true) {
			cin >> dsChoice;
			if (!cin.fail() && dsChoice >= 1 && dsChoice <= 3) break;
			clearInput();
			cout << "Invalid. Enter 1 (Array), 2 (Linked List), or 3 (Exit): ";
		}

		if (dsChoice == 3) {
			cout << "\nExiting program. Goodbye!" << endl;
			break;
		}

		const bool useLinkedList = (dsChoice == 2);

		cout << "\n----------------------------------------------------------" << endl;
		cout << "  Data Structure Selected: "
			<< (useLinkedList ? "Linked List" : "Array") << endl;
		cout << "----------------------------------------------------------" << endl;
		cout << "\nLoading datasets..." << endl;

		Dataset    arrA, arrB, arrC;
		LinkedList llA, llB, llC;

		if (useLinkedList) {
			llA.loadFromCSVWithLinkedList("dataset1-cityA.csv", "City A");
			llB.loadFromCSVWithLinkedList("dataset2-cityB.csv", "City B");
			llC.loadFromCSVWithLinkedList("dataset3-cityC.csv", "City C");
		}
		else {
			arrA.cityName = "City A";
			arrB.cityName = "City B";
			arrC.cityName = "City C";
			loadCSV("dataset1-cityA.csv", arrA);
			loadCSV("dataset2-cityB.csv", arrB);
			loadCSV("dataset3-cityC.csv", arrC);
		}

		cout << "Loading complete.\n" << endl;

		bool sortedByAge[3] = { false, false, false };
		bool sortedByDistance[3] = { false, false, false }; // Binary Search tracking

		int mainChoice = 0;
		do {
			displayMenu(useLinkedList);
			cin >> mainChoice;

			if (cin.fail()) {
				clearInput();
				cout << "Invalid input. Please enter a number between 1 and 4." << endl;
				continue;
			}

			switch (mainChoice) {

			case 1: {
				int cityChoice = selectCityMenu(true);
				const string cityNames[] = { "City A", "City B", "City C", "All Cities" };
				cout << "  >> City Selected: " << cityNames[cityChoice - 1] << endl;

				int ageChoice = 0;
				cout << "\n --- Select Analysis Type ---" << endl;
				cout << "1. Age 6  - 17" << endl;
				cout << "2. Age 18 - 25" << endl;
				cout << "3. Age 26 - 45" << endl;
				cout << "4. Age 46 - 60" << endl;
				cout << "5. Age 61 - 100" << endl;
				cout << "6. All Age Groups" << endl;
				cout << "Choice: ";
				cin >> ageChoice;

				if (useLinkedList) {
					if (ageChoice == 9) {
						if (cityChoice == 4) {
							cout << "Cross-dataset comparison not available for All Cities.\n";
							break;
						}
						LinkedList* currentLL = getCityLL(cityChoice, llA, llB, llC);
						cout << "\nSelect the city to compare " << cityNames[cityChoice - 1] << " against:";
						int otherChoice = selectCityMenu(false);
						LinkedList* otherLL = getCityLL(otherChoice, llA, llB, llC);
						currentLL->compareWithOtherCity(*otherLL);
					}
					else {
						auto runLL = [&](LinkedList& ll) {
							if (ageChoice >= 1 && ageChoice <= 5)
								ll.searchByAgeGroupBinaryWithLinkedList(AGE_MIN[ageChoice - 1], AGE_MAX[ageChoice - 1]);
							else if (ageChoice == 6)
								ll.ageGroupAnalysisWithLinkedList();
							else if (ageChoice == 7)
								ll.printDatasetSummaryWithLinkedList();
							else if (ageChoice == 8)
								ll.printInsightsWithLinkedList();
							};

						// Handle "All Cities" by running each list individually
						if (cityChoice == 4) {
							runLL(llA);
							runLL(llB);
							runLL(llC);
						}
						else {
							runLL(*getCityLL(cityChoice, llA, llB, llC));
						}
					}
				}
				else {
					//Array Logic
					int targetIdx = (ageChoice >= 1 && ageChoice <= 5) ? ageChoice - 1 : -1;
					if (cityChoice == 4) {
						analyzeAndPrintEmissions(arrA, targetIdx);
						analyzeAndPrintEmissions(arrB, targetIdx);
						analyzeAndPrintEmissions(arrC, targetIdx);
					}
					else {
						analyzeAndPrintEmissions(*getCityArray(cityChoice, arrA, arrB, arrC), targetIdx);
					}
				}
				break;
			}

			case 2: {
				int cityChoice = selectCityMenu(false);
				const string cityNames[] = { "City A", "City B", "City C" };
				cout << "  >> City Selected: " << cityNames[cityChoice - 1] << endl;

				if (useLinkedList) {
					LinkedList* llPtr = getCityLL(cityChoice, llA, llB, llC);

					int field;
					cout << "\n --- Select Sort Field ---" << endl;
					cout << "1. Age" << endl;
					cout << "2. Daily Distance" << endl;
					cout << "3. Total Emission" << endl;
					cout << "Enter your choice: ";
					cin >> field;

					SortKey key = BY_AGE;
					if (field == 2) key = BY_DAILY_DISTANCE;
					else if (field == 3) key = BY_CARBON_EMISSION;

					const string keyNames[] = { "Age", "Daily Distance", "Carbon Emission" };
					cout << "  >> Sorting " << cityNames[cityChoice - 1]
						<< " by " << keyNames[key] << " (Ascending)" << endl;

					llPtr->sortWithLinkedList(key);

					// Update search flags based on sort key
					sortedByAge[cityChoice - 1] = (key == BY_AGE);
					sortedByDistance[cityChoice - 1] = (key == BY_DAILY_DISTANCE);

					llPtr->printWithLinkedList();
					llPtr->printMemoryUsageWithLinkedList();
				}
				else {
					Dataset* cityPtr = getCityArray(cityChoice, arrA, arrB, arrC);

					int modeChoice = 0;
					cout << "\n --- Sorting Mode ---" << endl;
					cout << "1. Express Sort (Auto selects optimal method)" << endl;
					cout << "2. Custom Sort  (Manual config)" << endl;
					cout << "Enter your choice: ";
					cin >> modeChoice;

					if (modeChoice == 1) {
						int expressTarget;
						cout << "Sort by: 1. Age (Young to Old) | 2. Emission (High to Low): ";
						cin >> expressTarget;

						// Scan Dataset
						int outOfOrderCount = 0;
						for (int i = 1; i < cityPtr->size; i++) {
							if (expressTarget == 1) {
								// Checking Age (Ascending): Is the current age smaller than the previous one?
								if (cityPtr->data[i].age < cityPtr->data[i - 1].age) outOfOrderCount++;
							}
							else {
								// Checking Emission (Descending): Is current emission higher than previous?
								if (cityPtr->data[i].totalMonthlyEmissions > cityPtr->data[i - 1].totalMonthlyEmissions) outOfOrderCount++;
							}
						}

						// Calculate how messed up the data is (0.0 = perfectly sorted, 1.0 = total chaos)
						double chaosLevel = (cityPtr->size > 1) ? (double)outOfOrderCount / cityPtr->size : 0.0;

						// Use Insertion Sort if it's a tiny dataset OR if it's already mostly sorted (< 15% chaos)
						bool useInsertion = (cityPtr->size < 50) || (chaosLevel < 0.15);

						const string algoName = useInsertion ? "Insertion Sort" : "Quick Sort";
						const string fieldName = (expressTarget == 1) ? "Age (Ascending)" : "Emission (Descending)";

						cout << "  >> Dataset Scanner: Detected " << fixed << setprecision(1) << (chaosLevel * 100) << "% chaos	." << endl;
						cout << "  >> " << cityNames[cityChoice - 1]
							<< " | Algorithm: " << algoName
							<< " | Field: " << fieldName << endl;

						if (useInsertion) {
							if (expressTarget == 1) { insertionSort(*cityPtr, 1, true);  sortedByAge[cityChoice - 1] = true; }
							else { insertionSort(*cityPtr, 2, false); sortedByAge[cityChoice - 1] = false; }
						}
						else {
							if (expressTarget == 1) { quickSort(*cityPtr, 1, true);  sortedByAge[cityChoice - 1] = true; }
							else { quickSort(*cityPtr, 2, false); sortedByAge[cityChoice - 1] = false; }
						}
						printTop5(*cityPtr);

					}
					else {
						int algo, field, dir;
						cout << "Algorithm (1. Quick Sort | 2. Insertion Sort): ";
						cin >> algo;
						cout << "Target Field (1. Age | 2. Total Emission): ";
						cin >> field;
						cout << "Direction (1. Ascending | 2. Descending): ";
						cin >> dir;

						bool isAscending = (dir == 1);
						const string algoName = (algo == 1) ? "Quick Sort" : "Insertion Sort";
						const string fieldName = (field == 1) ? "Age" : "Total Emission";
						const string dirName = isAscending ? "Ascending" : "Descending";
						cout << "  >> " << cityNames[cityChoice - 1]
							<< " | Algorithm: " << algoName
							<< " | Field: " << fieldName
							<< " | Direction: " << dirName << endl;

						if (algo == 1) quickSort(*cityPtr, field, isAscending);
						else           insertionSort(*cityPtr, field, isAscending);

						sortedByAge[cityChoice - 1] = (field == 1 && isAscending);
						printTop5(*cityPtr);
					}
				}
				break;
			}

			case 3: {
				int cityChoice = selectCityMenu(false);
				const string cityNames[] = { "City A", "City B", "City C" };
				cout << "  >> City Selected: " << cityNames[cityChoice - 1] << endl;

				int searchChoice = 0;
				cout << "\n --- Select Search Algorithm ---" << endl;
				if (useLinkedList) {
					cout << "1. Find residents by Transport Mode (Linear)" << endl;
					cout << "2. Find residents by Age (Binary)" << endl;
					cout << "3. Find residents by Daily Distance Threshold (Linear)" << endl;
				}
				else {
					cout << "1. Find residents by Transport Mode (Linear)" << endl;
					cout << "2. Find residents by Age (Binary)" << endl;
				}
				cout << "Enter your choice: ";
				cin >> searchChoice;

				if (useLinkedList) {
					LinkedList* llPtr = getCityLL(cityChoice, llA, llB, llC);

					if (searchChoice == 1) {
						string mode;
						cout << "Enter Transport Mode (Car, Bus, Bicycle, Walking, School Bus, Carpool): ";
						clearInput();
						getline(cin, mode);
						cout << "  >> Linear Search | " << cityNames[cityChoice - 1]
							<< " | Transport: " << mode << endl;
						llPtr->searchByTransportLinearWithLinkedList(mode);
					}
					else if (searchChoice == 2) {
						if (!sortedByAge[cityChoice - 1]) {
							cout << "Warning: Binary Search requires data sorted by age." << endl;
							cout << "Auto-sorting using Linked List Quick Sort..." << endl;
							llPtr->sortWithLinkedList(BY_AGE);
							sortedByAge[cityChoice - 1] = true;
							sortedByDistance[cityChoice - 1] = false;
						}
						int minAge, maxAge;
						cout << "Enter minimum Age: ";
						cin >> minAge;
						cout << "Enter maximum Age: ";
						cin >> maxAge;
						cout << "  >> Binary Search | " << cityNames[cityChoice - 1]
							<< " | Age: " << minAge << " to " << maxAge << endl;
						llPtr->searchByAgeGroupBinaryWithLinkedList(minAge, maxAge);
					}
					else if (searchChoice == 3) {
						if (!sortedByDistance[cityChoice - 1]) {
							cout << "Warning: Binary Search requires data sorted by daily distance." << endl;
							cout << "Auto-sorting using Linked List Quick Sort..." << endl;
							llPtr->sortWithLinkedList(BY_DAILY_DISTANCE);
							sortedByDistance[cityChoice - 1] = true;
							sortedByAge[cityChoice - 1] = false;
						}
						double threshold;
						cout << "Enter Daily Distance threshold in km (e.g. 15): ";
						cin >> threshold;
						cout << "  >> Binary Search | " << cityNames[cityChoice - 1]
							<< " | Distance > " << threshold << " km" << endl;
						llPtr->searchByDistanceBinaryWithLinkedList(threshold);
					}
				}
				else {
					Dataset* cityPtr = getCityArray(cityChoice, arrA, arrB, arrC);

					if (searchChoice == 1) {
						string mode;
						cout << "Enter Transport Mode (Car, Bus, Bicycle, Walking, School Bus, Carpool): ";
						clearInput();
						getline(cin, mode);
						cout << "  >> Linear Search | " << cityNames[cityChoice - 1]
							<< " | Transport: " << mode << endl;
						searchByTransportMode(*cityPtr, mode);

					}
					else if (searchChoice == 2) {
						if (!sortedByAge[cityChoice - 1]) {
							cout << "Warning: Binary Search requires data sorted by age." << endl;
							cout << "Auto-sorting using Insertion Sort..." << endl;
							insertionSort(*cityPtr, 1, true);
							sortedByAge[cityChoice - 1] = true;
						}
						int targetAge;
						cout << "Enter exact Age to search for: ";
						cin >> targetAge;
						cout << "  >> Binary Search | " << cityNames[cityChoice - 1]
							<< " | Age: " << targetAge << endl;
						searchByAgeBinary(*cityPtr, targetAge);
					}
				}
				break;
			}

			case 4: {
				int cityChoice = selectCityMenu(false);
				const string cityNames[] = { "City A", "City B", "City C" };
				cout << "  >> City Selected: " << cityNames[cityChoice - 1] << endl;

				cout << "\n --- Select Analysis Type ---" << endl;
				cout << "1. Comprehensive Age Analysis" << endl;
				cout << "2. Dataset Summary" << endl;
				cout << "3. Insights & Recommendations" << endl;
				cout << "4. Cross-City Comparison" << endl;
				cout << "Choice: ";
				int analysisChoice;
				cin >> analysisChoice;

				if (useLinkedList) {
					LinkedList* llPtr = getCityLL(cityChoice, llA, llB, llC);

					if (analysisChoice == 1) llPtr->ageGroupAnalysisWithLinkedList();
					else if (analysisChoice == 2) llPtr->printDatasetSummaryWithLinkedList();
					else if (analysisChoice == 3) llPtr->printInsightsWithLinkedList();
					else if (analysisChoice == 4) {
						cout << "\nSelect the city to compare " << cityNames[cityChoice - 1] << " against:";
						int otherChoice = selectCityMenu(false);
						LinkedList* otherLL = getCityLL(otherChoice, llA, llB, llC);
						llPtr->compareWithOtherCity(*otherLL);
					}
				}
				else {
					// NEW: Array Routing Logic!
					Dataset* arrPtr = getCityArray(cityChoice, arrA, arrB, arrC);

					if (analysisChoice == 1) analyzeAndPrintEmissions(*arrPtr, -1);
					else if (analysisChoice == 2) printDatasetSummary(*arrPtr);
					else if (analysisChoice == 3) printInsights(*arrPtr);
					else if (analysisChoice == 4) {
						cout << "\nSelect the city to compare " << cityNames[cityChoice - 1] << " against:";
						int otherChoice = selectCityMenu(false);
						Dataset* otherArr = getCityArray(otherChoice, arrA, arrB, arrC);
						compareWithOtherCity(*arrPtr, *otherArr);
					}
				}
				break;
			}

			case 5:
				cout << "\nReturning to Main Menu..." << endl;
				break;
			}

			} while (mainChoice != 5);

			if (!useLinkedList) {
				delete[] arrA.data;
				delete[] arrB.data;
				delete[] arrC.data;
			}
		}
	return 0;
}