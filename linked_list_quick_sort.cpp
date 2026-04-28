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

// Linkedlist with 3-way Partitioning Quick Sort implementation
// Dutch National Flag but with LinkedList

void LinkedList::appendWithLinkedList(const Resident& resident) {
    Node* newNode = new Node(resident);
    if (head == nullptr) {
        head = newNode;
        linkedListCount++;
        return;
    }
    Node* current = head;
    while (current->next)
        current = current->next;
    current->next = newNode;
    linkedListCount++;
}

void LinkedList::buildUniqueTransportsWithLinkedList() {
    uniqueTransportCount = 0;
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        const std::string& mode = currentNode->data->modeOfTransport;
        bool found = false;
        for (int i = 0; i < uniqueTransportCount; ++i) {
            if (uniqueTransports[i] == mode) { found = true; break; }
        }
        if (!found && uniqueTransportCount < 20)
            uniqueTransports[uniqueTransportCount++] = mode;
    }
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
    buildUniqueTransportsWithLinkedList();
    return true;
}

int LinkedList::getlinkedListCountWithLinkedList() const { 
    return linkedListCount; 
}

double LinkedList::getAbsolute(double val) const {
    return (val < 0) ? -val : val;
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

    Node* slow = start;
    Node* fast = start;
    while (fast != end && fast->next != end) {
        slow = slow->next;
        fast = fast->next->next;
    }
    Node* mid = slow;

    medianOfThreeWithLinkedList(start, mid, end);

    Node* lessThanBoundary;
    Node* greaterThanBoundary;
    threeWayPartitionWithLinkedList(start, end, lessThanBoundary, greaterThanBoundary);

    Node* leftEnd = lessThanBoundary;
    Node* rightStart = greaterThanBoundary;

    if (leftEnd != start)
        quickSortHelperWithLinkedList(start, leftEnd);
    if (rightStart != end)
        quickSortHelperWithLinkedList(rightStart, end);
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

static void printTableHeader() {
    cout << "\n"
        << left << setw(14) << "ResidentID"
        << setw(6) << "Age"
        << setw(12) << "Transport"
        << right << setw(10) << "Dist(km)"
        << setw(16) << "EmissionFactor"
        << setw(6) << "Days"
        << setw(22) << "TotalMonthlyEmissions\n"
        << string(82, '-') << "\n";
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
        << setw(6) << resident.avgDaysPerMonth
        << setw(22) << resident.totalMonthlyEmissions << "\n";
}

void LinkedList::printWithLinkedList() const {
    printTableHeader();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next)
        printResidentRow(*currentNode->data);
}

void LinkedList::printTop5WithLinkedList() const {
    cout << "\nTop 5 Highest Emitters:\n"
        << left << setw(15) << "Resident ID"
        << setw(10) << "Age"
        << setw(20) << "Mode of Transport"
        << "Total Emission\n"
        << string(65, '-') << "\n";

    int skipCount = (linkedListCount > 5) ? (linkedListCount - 5) : 0;
    Node* currentNode = head;
    for (int i = 0; i < skipCount; ++i) currentNode = currentNode->next;

    while (currentNode) {
        cout << left << setw(15) << currentNode->data->residentID
            << setw(10) << currentNode->data->age
            << setw(20) << currentNode->data->modeOfTransport
            << fixed << setprecision(2) << currentNode->data->totalMonthlyEmissions << " kg CO2\n";
        currentNode = currentNode->next;
    }
}

void LinkedList::printDatasetSummaryWithLinkedList() const {
    double modeEmissionTotals[20] = {};
    int    modeResidentCounts[20] = {};
    double grandTotalEmissions = 0;

    for (Node* currentNode = head; currentNode; currentNode = currentNode->next) {
        grandTotalEmissions += currentNode->data->totalMonthlyEmissions;
        for (int i = 0; i < uniqueTransportCount; ++i) {
            if (currentNode->data->modeOfTransport == uniqueTransports[i]) {
                modeEmissionTotals[i] += currentNode->data->totalMonthlyEmissions;
                ++modeResidentCounts[i];
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

    for (int i = 0; i < uniqueTransportCount; ++i) {
        if (modeResidentCounts[i] == 0) continue;
        cout << left << setw(18) << uniqueTransports[i]
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
        << string(82, '-') << "\n"
        << left << setw(14) << "ResidentID"
        << setw(6) << "Age"
        << setw(12) << "Transport"
        << right << setw(10) << "Dist(km)"
        << setw(16) << "EmissionFactor"
        << setw(6) << "Days"
        << setw(22) << "TotalMonthlyEmissions\n"
        << string(82, '-') << "\n";
}

static void printSearchFooter(int matchCount, double elapsedMs, size_t stackMem) {
    cout << string(82, '-') << "\n"
        << "Total matches: " << matchCount << "\n"
        << "Search time  : " << fixed << setprecision(4) << elapsedMs << " ms\n"
        << "Search Memory: " << stackMem << " bytes (stack/local pointers)\n";
}

void LinkedList::searchByAgeGroupWithLinkedList(int minAge, int maxAge) const {
    string searchTitle = "Age " + to_string(minAge) + " to " + to_string(maxAge);
    int matchCount = 0;
    size_t stackUsage = sizeof(Node*) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    auto startTime = chrono::high_resolution_clock::now();
    printSearchHeader(searchTitle);
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next)
        if (currentNode->data->age >= minAge && currentNode->data->age <= maxAge) {
            printResidentRow(*currentNode->data);
            ++matchCount;
        }
    auto endTime = chrono::high_resolution_clock::now();

    printSearchFooter(matchCount, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
}

void LinkedList::searchByTransportWithLinkedList(const string& transportMode) const {
    int matchCount = 0;
    size_t stackUsage = sizeof(Node*) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    cout << "LINEAR SEARCH: Transport = '" << transportMode << "'\n";
    printSearchHeader("Transport: " + transportMode);

    auto startTime = chrono::high_resolution_clock::now();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next)
        if (currentNode->data->modeOfTransport == transportMode) {
            if (matchCount < 5) printResidentRow(*currentNode->data);
            ++matchCount;
        }
    auto endTime = chrono::high_resolution_clock::now();

    printSearchFooter(matchCount, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
}

void LinkedList::searchByDistanceWithLinkedList(double distanceThreshold) const {
    string searchTitle = "Daily Distance > " + to_string(distanceThreshold) + " km";
    int matchCount = 0;
    size_t stackUsage = sizeof(Node*) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    printSearchHeader(searchTitle);

    auto startTime = chrono::high_resolution_clock::now();
    for (Node* currentNode = head; currentNode; currentNode = currentNode->next)
        if (currentNode->data->dailyDistance > distanceThreshold) {
            printResidentRow(*currentNode->data);
            ++matchCount;
        }
    auto endTime = chrono::high_resolution_clock::now();

    printSearchFooter(matchCount, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
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
        for (int groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex) {
            if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
            groupStats[groupIndex].totalEmissions += currentNode->data->totalMonthlyEmissions;
            ++groupStats[groupIndex].residentCount;
            for (int i = 0; i < uniqueTransportCount; ++i) {
                if (currentNode->data->modeOfTransport == uniqueTransports[i]) {
                    ++groupStats[groupIndex].modeCount[i];
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

    for (int groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex) {
        int dominantModeIndex = 0;
        for (int i = 1; i < uniqueTransportCount; ++i)
            if (groupStats[groupIndex].modeCount[i] > groupStats[groupIndex].modeCount[dominantModeIndex])
                dominantModeIndex = i;

        cout << "\nAge Group : " << groupNames[groupIndex] << "\n"
            << string(72, '-') << "\n"
            << left << setw(18) << "Transport"
            << right << setw(7) << "Count"
            << setw(20) << "Total CO2 (kg)"
            << setw(20) << "Avg CO2/Resident\n"
            << string(72, '-') << "\n";

        for (int i = 0; i < uniqueTransportCount; ++i) {
            if (groupStats[groupIndex].modeCount[i] == 0) continue;
            cout << left << setw(18) << uniqueTransports[i]
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
            << "Most Preferred Transport : " << uniqueTransports[dominantModeIndex] << "\n"
            << "Total Emission           : " << fixed << setprecision(2)
            << groupStats[groupIndex].totalEmissions << " kg CO2\n"
            << "Average Emission         : " << averageEmission << " kg CO2/resident\n"
            << "Total Residents          : " << groupStats[groupIndex].residentCount << "\n";
    }
    cout << "\nAnalysis time: " << fixed << setprecision(4)
        << chrono::duration<double, milli>(endTime - startTime).count() << " ms\n";
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
        for (int groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex) {
            if (residentAge < groupMinAge[groupIndex] || residentAge > groupMaxAge[groupIndex]) continue;
            groupStats[groupIndex].totalEmissions += currentNode->data->totalMonthlyEmissions;
            ++groupStats[groupIndex].residentCount;
            for (int i = 0; i < uniqueTransportCount; ++i) {
                if (currentNode->data->modeOfTransport == uniqueTransports[i]) {
                    ++groupStats[groupIndex].modeCount[i];
                    break;
                }
            }
            break;
        }
    }

    int carIndex = -1, bicycleIndex = -1;
    for (int i = 0; i < uniqueTransportCount; ++i) {
        if (uniqueTransports[i] == "Car")     carIndex = i;
        if (uniqueTransports[i] == "Bicycle") bicycleIndex = i;
    }

    int highestEmittingGroup = 0;
    for (int groupIndex = 1; groupIndex < GROUP_COUNT; ++groupIndex)
        if (groupStats[groupIndex].totalEmissions > groupStats[highestEmittingGroup].totalEmissions)
            highestEmittingGroup = groupIndex;

    cout << "\n========== INSIGHTS & RECOMMENDATIONS ==========\n"
        << string(72, '-') << "\n"
        << left << setw(40) << "Age Group"
        << right << setw(10) << "Residents"
        << setw(18) << "Total CO2 (kg)"
        << setw(18) << "Avg CO2\n"
        << string(72, '-') << "\n";

    for (int groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex) {
        double averageEmission = groupStats[groupIndex].residentCount > 0
            ? groupStats[groupIndex].totalEmissions / groupStats[groupIndex].residentCount
            : 0.0;
        cout << left << setw(40) << groupNames[groupIndex]
            << right << setw(10) << groupStats[groupIndex].residentCount
            << fixed << setprecision(2)
            << setw(18) << groupStats[groupIndex].totalEmissions
            << setw(18) << averageEmission;
        if (groupIndex == highestEmittingGroup) cout << "  << HIGHEST";
        cout << "\n";
    }

    cout << string(72, '-') << "\n";
    cout << "\nKey Findings:\n";
    cout << "  1. Highest emitting group : " << groupNames[highestEmittingGroup]
        << " (" << fixed << setprecision(2)
        << groupStats[highestEmittingGroup].totalEmissions << " kg CO2)\n";

    cout << "  2. Car vs Bicycle usage by age group:\n";
    cout << "     " << left << setw(42) << "Age Group" << setw(8) << "Car" << setw(10) << "Bicycle\n";
    for (int groupIndex = 0; groupIndex < GROUP_COUNT; ++groupIndex)
        cout << "     " << left << setw(42) << groupNames[groupIndex]
        << setw(8) << (carIndex != -1 ? groupStats[groupIndex].modeCount[carIndex] : 0)
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

// Binary Search on Linked List - Code360
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

void LinkedList::searchByAgeBinaryWithLinkedList(int targetAge) const {
    if (currentSortKey != BY_AGE) {
        cout << "WARNING: List is not sorted by Age. Binary search results will be incorrect.\n";
    }

    auto startTime = chrono::high_resolution_clock::now();
    size_t stackUsage = sizeof(Node*) * 5 + sizeof(int) * 2 + sizeof(chrono::steady_clock::time_point) * 2;

    Node* start = head;
    Node* last = NULL;
    Node* found = NULL;

    while (start != last) {
        Node* mid = getMiddle(start, last);
        if (mid == NULL) break;
        if (mid->data->age == targetAge) { found = mid; break; }
        else if (mid->data->age < targetAge)    start = mid->next;
        else                                    last = mid;
    }

    Node* leftMatch = found;
    Node* rightMatch = found;
    int   matchCount = 0;

    if (found != NULL) {
        while (rightMatch->next != NULL && rightMatch->next->data->age == targetAge)
            rightMatch = rightMatch->next;
        while (leftMatch != head) {
            Node* prev = head;
            while (prev->next != leftMatch) prev = prev->next;
            if (prev->data->age == targetAge) leftMatch = prev;
            else break;
        }
        for (Node* currentNode = leftMatch; currentNode != rightMatch->next; currentNode = currentNode->next)
            matchCount++;
    }

    auto endTime = chrono::high_resolution_clock::now();

    if (found != NULL) {
        cout << matchCount << " match(es) found (Age Binary Search).\n";
        printTableHeader();
        for (Node* currentNode = leftMatch; currentNode != rightMatch->next; currentNode = currentNode->next)
            printResidentRow(*currentNode->data);
    }
    else {
        cout << "No resident found with age " << targetAge << ".\n";
    }

    printSearchFooter(matchCount, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
}

//

void LinkedList::searchByDistanceBinaryWithLinkedList(double targetDistance) const {
    if (currentSortKey != BY_DAILY_DISTANCE) {
        cout << "WARNING: List is not sorted by Daily Distance. Binary search results will be incorrect.\n";
    }

    auto startTime = chrono::high_resolution_clock::now();
    size_t stackUsage = sizeof(Node*) * 3 + sizeof(double) + sizeof(int) + sizeof(chrono::steady_clock::time_point) * 2;

    Node* start = head;
    Node* last = NULL;
    Node* found = NULL;

    while (start != last) {
        Node* mid = getMiddle(start, last);
        if (mid == NULL) break;
        if (getAbsolute(mid->data->dailyDistance - targetDistance) < 0.001) { found = mid; break; }
        else if (mid->data->dailyDistance < targetDistance)         start = mid->next;
        else                                                        last = mid;
    }

    auto endTime = chrono::high_resolution_clock::now();

    if (found != NULL) {
        cout << "Match found (Distance Binary Search):\n";
        printTableHeader();
        printResidentRow(*found->data);
    }
    else {
        cout << "No resident found with daily distance " << targetDistance << ".\n";
    }

    printSearchFooter(found ? 1 : 0, chrono::duration<double, milli>(endTime - startTime).count(), stackUsage);
}

void LinkedList::compareWithOtherCity(const LinkedList& other) const {
    cout << "\n===== CROSS-DATASET COMPARISON =====\n"
        << left << setw(20) << "Metric" << setw(25) << (cityLabel.empty() ? "Source A" : cityLabel)
        << setw(25) << (other.cityLabel.empty() ? "Source B" : other.cityLabel) << "\n"
        << string(70, '-') << "\n";

    auto calculateStats = [](const LinkedList& list, double& total, double& avg) {
        total = 0;
        for (Node* n = list.getHeadWithLinkedList(); n; n = n->next) total += n->data->totalMonthlyEmissions;
        avg = list.getlinkedListCountWithLinkedList() > 0 ? total / list.getlinkedListCountWithLinkedList() : 0;
        };

    double t1, a1, t2, a2;
    calculateStats(*this, t1, a1);
    calculateStats(other, t2, a2);

    cout << left << setw(20) << "Total Residents" << setw(25) << linkedListCount << setw(25) << other.linkedListCount << "\n"
        << left << setw(20) << "Total CO2 (kg)" << setw(25) << fixed << setprecision(2) << t1 << setw(25) << t2 << "\n"
        << left << setw(20) << "Avg CO2/Resident" << setw(25) << a1 << setw(25) << a2 << "\n"
        << string(70, '-') << "\n";
}