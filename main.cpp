#include <iostream>
#include <string>
#include <limits>
#include "Resident.hpp"
#include "ArrayProcessor.hpp"

using namespace std;

void clearInput() {
    cin.clear(); // Clear error flags
	cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
}

//Helper function to display menu
void displayMenu() {
    cout << "\n==========================================================" << endl;
    cout << "           CARBON EMISSION TRACKING SYSTEM                " << endl;
    cout << "==========================================================" << endl;
    cout << "1. View Carbon Emission Analysis" << endl;
    cout << "2. Sort City Data" << endl;
    cout << "3. Search City Data" << endl;
    cout << "4. Exit Program" << endl;
    cout << "==========================================================" << endl;
    cout << "Enter your choice (1-4): ";
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
		if (cin.fail() || choice < 1 || choice > (allowAllOption ? 4 : 3)){
            clearInput();
            cout << "Invalid input. Please enter a number between 1 and 3." << endl;
        } else {
            return choice;
        }
    }
}

//Returns a pointer to target the correct array
Dataset* getCityPointer(int choice, Dataset& cityA, Dataset& cityB, Dataset& cityC) {
	if (choice == 1) return &cityA;
	if (choice == 2) return &cityB;
	if (choice == 3) return &cityC;
    return nullptr;
}

int main() {
    //Initialize datasets
    Dataset cityA, cityB, cityC;
    cityA.cityName = "City A";
    cityB.cityName = "City B";
    cityC.cityName = "City C";

    cout << "Loading datasets into dynamic arrays..." << endl;

    // Call functions defined in Array.cpp
    loadCSV("dataset1-cityA.csv", cityA);
    loadCSV("dataset2-cityB.csv", cityB);
    loadCSV("dataset3-cityC.csv", cityC);

    cout << "Loading complete.\n" << endl;

    bool isCitySortedByAge[3] = { false, false, false }; //Flag to track if a city is sorted for binary search

    //Interactive Menu Loop
    int mainChoice = 0;
    do {
		displayMenu();
        cin >> mainChoice;

		//Input validation
        if (cin.fail()) {
            cin.clear(); //Clear error flag
			cin.ignore(numeric_limits<streamsize>::max(), '\n'); //Discard invalid input
            cout<<"Invalid input. Please enter a number between 1 and 6." << endl;
			continue; // Restart loop for valid input
        } 

        switch (mainChoice) {
            case 1: {
                //View Analysis
                int cityChoice = selectCityMenu(true);

                int ageChoice = 0;
                cout << "\n --- Select Age Group ---" << endl;
                cout << "1. 6 - 17" << endl;
                cout << "2. 18 -25" << endl;
                cout << "3. 26 - 45" << endl;
                cout << "4. 46 - 60" << endl;
                cout << "5. 61 - 100" << endl;
                cout << "6. All Age Groups" << endl;
                cout << "Choice: ";
                cin >> ageChoice;

                int targetIdx = (ageChoice >= 1 && ageChoice <= 5) ? ageChoice - 1 : -1;

                if (cityChoice == 4) {
                    analyzeAndPrintEmissions(cityA, targetIdx);
                    analyzeAndPrintEmissions(cityB, targetIdx);
                    analyzeAndPrintEmissions(cityC, targetIdx);
                }
                else {
                    analyzeAndPrintEmissions(*getCityPointer(cityChoice, cityA, cityB, cityC), targetIdx);
                }
                break;
            }
            case 2: {
                //Sorting
                int cityChoice = selectCityMenu(false);
                Dataset* cityPtr = getCityPointer(cityChoice, cityA, cityB, cityC);

                int modeChoice = 0;
                cout << "\n --- Sorting Mode ---" << endl;
                cout << "1. Express Sort (Auto selects optimal method)" << endl;
                cout << "2. Custom Sort (Manual config)" << endl;
                cout << "Enter your choice: ";
                cin >> modeChoice;

                if (modeChoice == 1) {
                    int expressTarget;
                    cout << " Sort by: 1 Age (Young to Old) | 2. Emission (High to Low): ";
                    cin >> expressTarget;

                    //Smart Selection Logic: Check Dataset Size
                    bool useInsertion = (cityPtr->size < 50);

                    if (useInsertion) {
                        cout << "Small dataset detected (" << cityPtr->size << " items). Auto-selecting Insertion Sort." << endl;
                        if (expressTarget == 1) {
                            insertionSort(*cityPtr, 1, true); //Age, Ascending
                            isCitySortedByAge[cityChoice - 1] = true;
                        }
                        else {
                            insertionSort(*cityPtr, 2, false); //Emission, Descending
                            isCitySortedByAge[cityChoice - 1] = false;
                        }
                    }
                    else {
                        cout << "Large dataset detected (" << cityPtr->size << " items). Auto-selecting Quick Sort." << endl;
                        if (expressTarget == 1) {
                            quickSort(*cityPtr, 1, true); //Age, Ascending
                            isCitySortedByAge[cityChoice - 1] = true;
                        }
                        else {
                            quickSort(*cityPtr, 2, false); //Emission, Descending
                            isCitySortedByAge[cityChoice - 1] = false;
                        }
                    }
                    printTop5(*cityPtr);
                }
                else if (modeChoice == 2) {
                    int algo, field, dir;

                    cout << "Algorithm (1. Quick Sort | 2. Insertion Sort): ";
                    cin >> algo;
                    cout << "Target Field (1. Age | 2. Total Emission): ";
                    cin >> field;
                    cout << "Direction (1. Ascending | 2. Descending):";
                    cin >> dir;

                    bool isAscending = (dir == 1);

                    if (algo == 1) {
                        quickSort(*cityPtr, field, isAscending);
                    }
                    else {
                        insertionSort(*cityPtr, field, isAscending);
                    }

                    if (field == 1 && isAscending) {
                        isCitySortedByAge[cityChoice - 1] = true;
                    }
                    else {
                        isCitySortedByAge[cityChoice - 1] = false;
                    }
                    printTop5(*cityPtr);
                }
                break;
            }

            case 3: {
                //Searching
                int cityChoice = selectCityMenu(false);
                Dataset* cityPtr = getCityPointer(cityChoice, cityA, cityB, cityC);

                int searchChoice = 0;
                cout << "\n --- Select Search Algorithm ---" << endl;
                cout << "1. Find residents by Transport Mode" << endl;
                cout << "2. Find residents by Age" << endl;
                cout << "Enter your Choice: ";
                cin >> searchChoice;

                if (searchChoice == 1) {
                    string mode;
                    cout << "Enter Transport Mode (Car, Bus, Bicycle, Walking, School Bus, Carpool): ";
                    clearInput();
                    getline(cin, mode);
                    searchByTransportMode(*cityPtr, mode);
                }
                else if (searchChoice == 2) {
                    //Safety: Binary Search needs sorted data;
                    if (!isCitySortedByAge[cityChoice - 1]) {
                        cout << "Warning: Binary Search requires data sorted by age." << endl;
                        cout << "Auto-sorting " << cityPtr->cityName << "using Insertion Sort..." << endl;
                        insertionSort(*cityPtr, 1, true); //Sort by Age, Ascending
                        isCitySortedByAge[cityChoice - 1] = true;
                    }

                    int targetAge;
                    cout << "Enter exact Age to search for: ";
                    cin >> targetAge;
                    searchByAgeBinary(*cityPtr, targetAge);
                }
                break;

            }
            case 4: {
                cout << "\n Exiting Program..." << endl;
                break;
            }
            default: {
                //Default case for bad inputs
                cout << "\nInvalid Choice. Please select an option from the menu." << endl;
                break;
            }
        }

    } while (mainChoice != 4);

    // Memory cleanup
    delete[] cityA.data;
    delete[] cityB.data;
    delete[] cityC.data;

    return 0;
}