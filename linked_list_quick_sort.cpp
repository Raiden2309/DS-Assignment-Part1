#include "linked_list_quick_sort.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std;

LinkedList::LinkedList() : head(nullptr), linkedListCount(0), currentSortKey(BY_AGE) {}

LinkedList::~LinkedList() {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
    head = nullptr;
    linkedListCount = 0;
}

const std::string LinkedList::TRANSPORT_MODES[6] = {
    "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"
};

// Linkedlist with 3-way Partitioning Quick Sort implementation
// Dutch National Flag but with LinkedList

void LinkedList::appendWithLinkedList(const Resident& resident) {
    Node* newNode = new Node(resident);
    if (head == nullptr) {
        head = newNode;
    }
    else {
        Node* current = head;
        while (current->next)
            current = current->next;
        current->next = newNode;
    }
    linkedListCount++;
}

bool LinkedList::loadFromCSVWithLinkedList(const string& filename, const string& label) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open '" << filename << "'\n";
        return false;
    }
    cityLabel = label;

    string skipHeader;
    getline(file, skipHeader);

    string residentID, age, modeOfTransport, dailyDistance, carbonEmissionFactor, avgDaysPerMonth;
    while (file.good()) {
        getline(file, residentID, ',');
        if (residentID.empty()) break;
        getline(file, age, ',');
        getline(file, modeOfTransport, ',');
        getline(file, dailyDistance, ',');
        getline(file, carbonEmissionFactor, ',');
        getline(file, avgDaysPerMonth);

        Resident resident;
        resident.residentID = residentID;
        resident.age = stoi(age);
        resident.modeOfTransport = modeOfTransport;
        resident.dailyDistance = stod(dailyDistance);
        resident.carbonEmissionFactor = stod(carbonEmissionFactor);
        resident.avgDaysPerMonth = stoi(avgDaysPerMonth);
        resident.totalMonthlyEmissions = resident.dailyDistance * resident.carbonEmissionFactor * resident.avgDaysPerMonth;

        appendWithLinkedList(resident);
    }
    cout << "Loaded " << linkedListCount << " residents from '" << filename << "'";
    if (!label.empty()) cout << " [" << label << "]\n";
    return true;
}

int LinkedList::getlinkedListCountWithLinkedList() const { 
    return linkedListCount; 
}

double LinkedList::getSortKeyWithLinkedList(const Resident& resident, SortKey sortKey) {
    switch (sortKey) {
    case BY_AGE:             return resident.age;
    case BY_DAILY_DISTANCE:  return resident.dailyDistance;
    case BY_CARBON_EMISSION: return resident.totalMonthlyEmissions;
    }
    return 0;
}


void LinkedList::swapNodeData(Node* a, Node* b) {
    Resident* temp = a->data;
    a->data = b->data;
    b->data = temp;
}

void LinkedList::medianOfThreeWithLinkedList(Node* start, Node* mid, Node* end) {
    double startVal = getSortKeyWithLinkedList(*start->data, currentSortKey);
    double midVal = getSortKeyWithLinkedList(*mid->data, currentSortKey);
    double endVal = getSortKeyWithLinkedList(*end->data, currentSortKey);

    if (midVal < startVal) swapNodeData(start, mid);
    if (endVal < startVal) swapNodeData(start, end);
    if (endVal < midVal)   swapNodeData(mid, end);

    swapNodeData(mid, start);
}

void LinkedList::threeWayPartitionWithLinkedList(Node* start, Node* end,
    Node*& lessThanBoundary, Node*& greaterThanBoundary) {
    double pivotValue = getSortKeyWithLinkedList(*start->data, currentSortKey);
    lessThanBoundary = start;
    greaterThanBoundary = end;
    Node* scanPosition = start->next;

    while (scanPosition && scanPosition != greaterThanBoundary->next) {
        double currentValue = getSortKeyWithLinkedList(*scanPosition->data, currentSortKey);
        if (currentValue < pivotValue) {
            lessThanBoundary = lessThanBoundary->next;
            swapNodeData(scanPosition, lessThanBoundary);
            scanPosition = scanPosition->next;
        }
        else if (currentValue > pivotValue) {
            swapNodeData(scanPosition, greaterThanBoundary);
            Node* cur = start;
            while (cur->next != greaterThanBoundary) cur = cur->next;
            greaterThanBoundary = cur;
        }
        else {
            scanPosition = scanPosition->next;
        }
    }
    swapNodeData(start, lessThanBoundary);
}

void LinkedList::quickSortHelperWithLinkedList(Node* start, Node* end) {
    if (!start || start == end) return;

    // Find midpoint for median-of-three pivot selection
    Node* slow = start;
    Node* fast = start;
    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }
    medianOfThreeWithLinkedList(start, slow, end);

    Node* lessThanBoundary;
    Node* greaterThanBoundary;
    threeWayPartitionWithLinkedList(start, end, lessThanBoundary, greaterThanBoundary);

    // Left partition: everything BEFORE the equal-to-pivot section
    if (lessThanBoundary != start) {
        Node* leftEnd = start;
        while (leftEnd->next != lessThanBoundary) leftEnd = leftEnd->next;
        quickSortHelperWithLinkedList(start, leftEnd);
    }

    // Right partition: everything AFTER the equal-to-pivot section
    if (greaterThanBoundary != end) {
        quickSortHelperWithLinkedList(greaterThanBoundary->next, end);
    }
}

void LinkedList::sortWithLinkedList(SortKey sortKey) {
    if (linkedListCount <= 1) return;

    currentSortKey = sortKey;

    Node* tailNode = head;
    while (tailNode->next) tailNode = tailNode->next;

    auto startTime = chrono::high_resolution_clock::now();
    quickSortHelperWithLinkedList(head, tailNode);
    auto endTime = chrono::high_resolution_clock::now();

    static const char* keyNames[] = { "Age", "Daily Distance", "Carbon Emission" };
    cout << "Sort by " << keyNames[sortKey] << " completed in "
        << fixed << setprecision(4)
        << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
}

void LinkedList::ageGroupAnalysisWithLinkedList() const {
    static const int   GROUP_COUNT = 5;
    static const char* groupNames[GROUP_COUNT] = {
        "Children & Teenagers (6-17)",
        "University Students / Young Adults (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career (46-60)",
        "Senior Citizens / Retirees (61-100)"
    };
    static const int groupMinAge[GROUP_COUNT] = { 6,  18, 26, 46,  61 };
    static const int groupMaxAge[GROUP_COUNT] = { 17, 25, 45, 60, 100 };

    struct GroupStats {
        double totalEmissions = 0;
        int    residentCount = 0;
        int    modeCount[20] = {};
        double modeEmissions[20] = {};
    } groupStats[GROUP_COUNT];

    auto startTime = chrono::high_resolution_clock::now();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        int residentAge = currentNode->data->age;
        for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
            if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
            groupStats[groupIndex].totalEmissions += currentNode->data->totalMonthlyEmissions;
            groupStats[groupIndex].residentCount++;
            for (int i = 0; i < TRANSPORT_COUNT; i++) {
                if (currentNode->data->modeOfTransport == TRANSPORT_MODES[i]) {
                    groupStats[groupIndex].modeCount[i]++;
                    groupStats[groupIndex].modeEmissions[i] += currentNode->data->totalMonthlyEmissions;
                    break;
                }
            }
            break;
        }
    }
    auto endTime = chrono::high_resolution_clock::now();

    string label = cityLabel.empty() ? "All Datasets" : cityLabel;
    cout << "Age Group Analysis [" << label << "]\n";

    for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
        int dominantModeIndex = 0;
        for (int i = 1; i < TRANSPORT_COUNT; i++)
            if (groupStats[groupIndex].modeCount[i] > groupStats[groupIndex].modeCount[dominantModeIndex])
                dominantModeIndex = i;

        cout << "\nAge Group : " << groupNames[groupIndex] << "\n"
            << string(72, '-') << "\n"
            << left << setw(18) << "Transport"
            << right << setw(7) << "Count"
            << setw(20) << "Total CO2 (kg)"
            << setw(20) << "Avg CO2/Resident\n"
            << string(72, '-') << "\n";

        for (int i = 0; i < TRANSPORT_COUNT; i++) {
            if (groupStats[groupIndex].modeCount[i] == 0) continue;
            cout << left << setw(18) << TRANSPORT_MODES[i]
                << right << setw(7) << groupStats[groupIndex].modeCount[i]
                << fixed << setprecision(2)
                << setw(20) << groupStats[groupIndex].modeEmissions[i]
                << setw(20) << groupStats[groupIndex].modeEmissions[i]
                / groupStats[groupIndex].modeCount[i] << "\n";
        }

        double averageEmission = groupStats[groupIndex].residentCount > 0
            ? groupStats[groupIndex].totalEmissions / groupStats[groupIndex].residentCount
            : 0.0;

        cout << string(72, '-') << "\n"
            << "Most Preferred Transport : " << TRANSPORT_MODES[dominantModeIndex] << "\n"
            << "Total Emission           : " << fixed << setprecision(2)
            << groupStats[groupIndex].totalEmissions << " kg CO2\n"
            << "Average Emission         : " << averageEmission << " kg CO2/resident\n"
            << "Total Residents          : " << groupStats[groupIndex].residentCount << "\n";
    }
    cout << "\nAnalysis time: " << fixed << setprecision(4)
        << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
}

void LinkedList::ageGroupAnalysisRangeWithLinkedList(int minAge, int maxAge, const string& groupLabel) const {
    double modeEmissions[20] = {};
    int    modeCount[20] = {};
    double totalEmissions = 0;
    int    residentCount = 0;

    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        int age = currentNode->data->age;
        if (age < minAge || age > maxAge) continue;

        totalEmissions += currentNode->data->totalMonthlyEmissions;
        residentCount++;

        for (int i = 0; i < TRANSPORT_COUNT; i++) {
            if (currentNode->data->modeOfTransport == TRANSPORT_MODES[i]) {
                modeCount[i]++;
                modeEmissions[i] += currentNode->data->totalMonthlyEmissions;
                break;
            }
        }
    }

    cout << "\n" << string(84, '=') << "\n"
        << "Carbon Emission Analysis: " << cityLabel << "\n"
        << string(84, '=') << "\n"
        << "Age Group: " << minAge << "-" << maxAge << ": " << groupLabel << "\n"
        << string(84, '-') << "\n"
        << left << setw(22) << "Mode of Transport"
        << setw(10) << "Count"
        << setw(32) << "Total Emission (kg CO2)"
        << setw(20) << "Average per Resident\n"
        << string(84, '-') << "\n";

    for (int i = 0; i < TRANSPORT_COUNT; i++) {
        if (modeCount[i] == 0) continue;
        cout << left << setw(22) << TRANSPORT_MODES[i]
            << setw(10) << modeCount[i]
            << fixed << setprecision(2)
            << setw(32) << modeEmissions[i]
            << setw(20) << modeEmissions[i] / modeCount[i] << "\n";
    }

    cout << string(84, '-') << "\n"
        << "Total Emission for Age Group: " << totalEmissions << " kg CO2\n";
}

// Printing Helpers

static void printTableHeader() {
    cout << "\n"
        << left << setw(14) << "ResidentID"
        << setw(6) << "Age"
        << setw(12) << "Transport"
        << right << setw(10) << "Dist(km)"
        << setw(16) << "EmissionFactor"
        << setw(8) << "Days"
        << setw(25) << "TotalMonthlyEmissions\n"
        << string(90, '-') << "\n";
}

void LinkedList::printMemoryUsageWithLinkedList() const {
    size_t totalMemory = (sizeof(Node) + sizeof(Resident)) * linkedListCount;
    cout << "Structure Memory Usage: " << linkedListCount << " nodes x "
        << "(" << sizeof(Node) << " + " << sizeof(Resident) << ") bytes = "
        << totalMemory << " bytes ("
        << fixed << setprecision(2) << totalMemory / 1024.0 << " KB)\n";
}

static void printResidentRow(const Resident& resident) {
    cout << left << setw(14) << resident.residentID
        << setw(6) << resident.age
        << setw(12) << resident.modeOfTransport
        << right << setw(10) << fixed << setprecision(2) << resident.dailyDistance
        << setw(16) << resident.carbonEmissionFactor
        << setw(8) << resident.avgDaysPerMonth
        << setw(25) << resident.totalMonthlyEmissions << "\n";
}

void LinkedList::printWithLinkedList() const {
    printTableHeader();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next)
        printResidentRow(*currentNode->data);
}

void LinkedList::printDatasetSummaryWithLinkedList() const {
    double modeEmissionTotals[20] = {};
    int    modeResidentCounts[20] = {};
    double grandTotalEmissions = 0;

    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        grandTotalEmissions += currentNode->data->totalMonthlyEmissions;
        for (int i = 0; i < TRANSPORT_COUNT; i++) {
            if (currentNode->data->modeOfTransport == TRANSPORT_MODES[i]) {
                modeEmissionTotals[i] += currentNode->data->totalMonthlyEmissions;
                modeResidentCounts[i]++;
                break;
            }
        }
    }

    string label = cityLabel.empty() ? "All Datasets" : cityLabel;
    cout << "\n===== Carbon Emission Summary [" << label << "] =====\n"
        << string(60, '-') << "\n"
        << left << setw(18) << "Mode of Transport"
        << right << setw(8) << "Count"
        << setw(20) << "Total CO2 (kg)"
        << setw(18) << "Avg CO2/User\n"
        << string(60, '-') << "\n";

    for (int i = 0; i < TRANSPORT_COUNT; i++) {
        if (modeResidentCounts[i] == 0) continue;
        cout << left << setw(18) << TRANSPORT_MODES[i]
            << right << setw(8) << modeResidentCounts[i]
            << fixed << setprecision(2)
            << setw(20) << modeEmissionTotals[i]
            << setw(18) << modeEmissionTotals[i] / modeResidentCounts[i] << "\n";
    }

    cout << string(60, '-') << "\n"
        << "Total Residents  : " << linkedListCount << "\n"
        << "Total CO2 (month): " << fixed << setprecision(2) << grandTotalEmissions << " kg\n"
        << "Avg CO2/Resident : " << (linkedListCount > 0 ? grandTotalEmissions / linkedListCount : 0.0) << " kg\n"
        << string(60, '-') << "\n";
}

static void printSearchHeader(const string& searchTitle) {
    cout << "\nSearch Results - " << searchTitle << "\n"
        << string(90, '-') << "\n"
        << left << setw(14) << "ResidentID"
        << setw(6) << "Age"
        << setw(12) << "Transport"
        << right << setw(10) << "Dist(km)"
        << setw(16) << "EmissionFactor"
        << setw(8) << "Days"
        << setw(25) << "TotalMonthlyEmissions\n"
        << string(90, '-') << "\n";
}

static void printSearchFooter(int matchCount, double elapsedMs, size_t stackMem) {
    cout << string(90, '-') << "\n"
        << "Total matches: " << matchCount << "\n"
        << "Search time  : " << fixed << setprecision(4) << elapsedMs << " ms\n"
        << "Search Memory: " << stackMem << " bytes (local variables, compile-time estimate)\n";
}

void LinkedList::printInsightsWithLinkedList() const {
    static const int   GROUP_COUNT = 5;
    static const char* groupNames[GROUP_COUNT] = {
        "Children & Teenagers (6-17)",
        "University Students / Young Adults (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career (46-60)",
        "Senior Citizens / Retirees (61-100)"
    };
    static const int groupMinAge[GROUP_COUNT] = { 6,  18, 26, 46,  61 };
    static const int groupMaxAge[GROUP_COUNT] = { 17, 25, 45, 60, 100 };

    struct GroupStats {
        double totalEmissions = 0;
        int    residentCount = 0;
        int    modeCount[20] = {};
    } groupStats[GROUP_COUNT];

    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        int residentAge = currentNode->data->age;
        for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
            if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
            groupStats[groupIndex].totalEmissions += currentNode->data->totalMonthlyEmissions;
            groupStats[groupIndex].residentCount++;
            for (int i = 0; i < TRANSPORT_COUNT; i++) {
                if (currentNode->data->modeOfTransport == TRANSPORT_MODES[i]) {
                    groupStats[groupIndex].modeCount[i]++;
                    break;
                }
            }
            break;
        }
    }

    int carIndex = -1, bicycleIndex = -1;
    for (int i = 0; i < TRANSPORT_COUNT; i++) {
        if (TRANSPORT_MODES[i] == "Car")     carIndex = i;
        if (TRANSPORT_MODES[i] == "Bicycle") bicycleIndex = i;
    }

    int highestEmittingGroup = 0;
    for (int groupIndex = 1; groupIndex < GROUP_COUNT; groupIndex++)
        if (groupStats[groupIndex].totalEmissions > groupStats[highestEmittingGroup].totalEmissions)
            highestEmittingGroup = groupIndex;

    cout << "\n INSIGHTS & RECOMMENDATIONS \n"
        << string(90, '-') << "\n"
        << left << setw(46) << "Age Group"
        << right << setw(10) << "Residents"
        << setw(18) << "Total CO2 (kg)"
        << setw(18) << "Avg CO2\n"
        << string(90, '-') << "\n";

    for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++) {
        double averageEmission = groupStats[groupIndex].residentCount > 0
            ? groupStats[groupIndex].totalEmissions / groupStats[groupIndex].residentCount
            : 0.0;
        cout << left << setw(46) << groupNames[groupIndex]
            << right << setw(10) << groupStats[groupIndex].residentCount
            << fixed << setprecision(2)
            << setw(18) << groupStats[groupIndex].totalEmissions
            << setw(18) << averageEmission;
        if (groupIndex == highestEmittingGroup) cout << "  << HIGHEST";
        cout << "\n";
    }

    cout << string(90, '-') << "\n";
    cout << "\nKey Findings:\n";
    cout << "  1. Highest emitting group : " << groupNames[highestEmittingGroup]
        << " (" << fixed << setprecision(2)
        << groupStats[highestEmittingGroup].totalEmissions << " kg CO2)\n";

    cout << "  2. Car vs Bicycle usage by age group:\n";
    cout << "     " << left << setw(44) << "Age Group"
        << right << setw(8) << "Car"
        << setw(10) << "Bicycle\n";
    for (int groupIndex = 0; groupIndex < GROUP_COUNT; groupIndex++)
        cout << "     " << left << setw(44) << groupNames[groupIndex]
        << right << setw(8) << (carIndex != -1 ? groupStats[groupIndex].modeCount[carIndex] : 0)
        << setw(10) << (bicycleIndex != -1 ? groupStats[groupIndex].modeCount[bicycleIndex] : 0)
        << "\n";

    cout << "\nRecommendations:\n"
        << string(72, '-') << "\n"
        << "  - " << groupNames[highestEmittingGroup] << " has the highest emissions.\n"
        << "    Encourage carpooling or public transport for this group.\n"
        << "  - Groups with high car usage should be targeted with\n"
        << "    cycling infrastructure and incentives.\n"
        << string(72, '-') << "\n";
}

// Searching Algorithms for Linked List
// Binary Search - Code360
Node* LinkedList::getMiddle(Node* start, Node* last) const {
    if (start == NULL) return NULL;
    Node* slow = start;
    Node* fast = start;
    while (fast != last && fast->next != last) {
        fast = fast->next->next;
        slow = slow->next;
    }
    return slow;
}

void LinkedList::searchByAgeGroupBinaryWithLinkedList(int minAge, int maxAge) const {
    if (currentSortKey != BY_AGE) {
        cout << "WARNING: List is not sorted by Age. Binary search results will be incorrect.\n";
    }

    auto startTime = chrono::high_resolution_clock::now();
    size_t stackUsage = sizeof(Node*) * 2 + sizeof(int); 

    // Binary search for minAge boundary
    Node* start = head;
    Node* last = NULL;
    Node* foundMin = NULL;

    while (start != last) {
        Node* mid = getMiddle(start, last);
        if (mid == NULL) break;
        if (mid->data->age >= minAge) {
            foundMin = mid;
            last = mid;
        }
        else {
            start = mid->next;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();

    int matchCount = 0;
    string searchTitle = "Age " + to_string(minAge) + " to " + to_string(maxAge);
    printSearchHeader(searchTitle);

    // From foundMin, print all nodes within range
    for (Node* currentNode = foundMin; currentNode; currentNode = currentNode->next) {
        if (currentNode->data->age > maxAge) break;
        if (currentNode->data->age >= minAge) {
            printResidentRow(*currentNode->data);
            matchCount++;
        }
    }

    double duration = chrono::duration<double, milli>(endTime - startTime).count();
    printSearchFooter(matchCount, duration, stackUsage);
}

void LinkedList::searchByDistanceBinaryWithLinkedList(double distanceThreshold) const {
    if (currentSortKey != BY_DAILY_DISTANCE) {
        cout << "WARNING: List is not sorted by Daily Distance. Binary search results will be incorrect.\n";
    }

    auto startTime = chrono::high_resolution_clock::now();
    size_t stackUsage = sizeof(Node*) * 5 + sizeof(double) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    // Binary search for first node > distanceThreshold
    Node* start = head;
    Node* last = NULL;
    Node* foundBoundary = NULL;

    while (start != last) {
        Node* mid = getMiddle(start, last);
        if (mid == NULL) break;
        if (mid->data->dailyDistance > distanceThreshold) {
            foundBoundary = mid;
            last = mid;
        }
        else {
            start = mid->next;
        }
    }

    auto endTime = chrono::high_resolution_clock::now();

    int matchCount = 0;
    string searchTitle = "Daily Distance > " + to_string(distanceThreshold) + " km";
    printSearchHeader(searchTitle);

    // Walk forward from boundary printing all nodes > threshold
    for (Node* currentNode = foundBoundary; currentNode; currentNode = currentNode->next) {
        if (currentNode->data->dailyDistance > distanceThreshold) {
            printResidentRow(*currentNode->data);
            matchCount++;
        }
    }

    double duration = chrono::duration<double, milli>(endTime - startTime).count();
    printSearchFooter(matchCount, duration, stackUsage);
}

// Linear Search

void LinkedList::searchByTransportLinearWithLinkedList(const string& transportMode) const {
    int matchCount = 0;
    size_t stackUsage = sizeof(Node*) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    auto startTime = chrono::high_resolution_clock::now();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        if (currentNode->data->modeOfTransport == transportMode) {
            matchCount++;
        }
    }
    auto endTime = chrono::high_resolution_clock::now();

    cout << "LINEAR SEARCH: Transport = '" << transportMode << "'\n";
    printSearchHeader("Transport: " + transportMode);

    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        if (currentNode->data->modeOfTransport == transportMode) {
            printResidentRow(*currentNode->data);
        }
    }

    double duration = chrono::duration<double, milli>(endTime - startTime).count();
    printSearchFooter(matchCount, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
}

// Compare one set with others
void LinkedList::compareWithOtherCity(const LinkedList& other) const {
    string col1 = cityLabel.empty() ? "Source A" : cityLabel;
    string col2 = other.cityLabel.empty() ? "Source B" : other.cityLabel;

    cout << "\nCROSS-DATASET COMPARISON\n"
        << string(70, '-') << "\n"
        << left << setw(20) << "Metric"
        << setw(25) << col1
        << setw(25) << col2 << "\n"
        << string(70, '-') << "\n";

    auto calculateStats = [](const LinkedList& list, double& total, double& avg) {
        total = 0;
        for (Node* n = list.getHeadWithLinkedList(); n; n = n->next) total += n->data->totalMonthlyEmissions;
        avg = list.getlinkedListCountWithLinkedList() > 0 ? total / list.getlinkedListCountWithLinkedList() : 0;
        };

    double t1, a1, t2, a2;
    auto startTime = chrono::high_resolution_clock::now();
    calculateStats(*this, t1, a1);
    calculateStats(other, t2, a2);
    auto endTime = chrono::high_resolution_clock::now();

    cout << left << setw(20) << "Total Residents" << setw(25) << linkedListCount << setw(25) << other.linkedListCount << "\n"
        << left << setw(20) << "Total CO2 (kg)" << setw(25) << fixed << setprecision(2) << t1 << setw(25) << t2 << "\n"
        << left << setw(20) << "Avg CO2/Resident" << setw(25) << a1 << setw(25) << a2 << "\n"
        << string(70, '-') << "\n"
        << "Comparison time  : " << fixed << setprecision(4)
        << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
}