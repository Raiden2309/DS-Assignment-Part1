#include "linked_list_quick_sort.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
using namespace std;

LinkedList::LinkedList() : head(nullptr), tail(nullptr), length(0) {}

LinkedList::~LinkedList() { clear(); }

void LinkedList::clear() {
    Node* cur = head;
    while (cur) {
        Node* nxt = cur->next;
        delete cur;
        cur = nxt;
    }
    head = tail = nullptr;
    length = 0;
}

void LinkedList::append(const Resident& r) {
    Node* n = new Node(r);
    if (!head) head = tail = n;
    else { tail->next = n; tail = n; }
    ++length;
}

size_t LinkedList::getMemoryUsage() const {
    return sizeof(*this) + (size_t)length * sizeof(Node);
}

static string nextField(const string& s, int& i) {
    int start = i;
    while (i < (int)s.size() && s[i] != ',') ++i;
    string field = s.substr(start, i - start);
    if (i < (int)s.size()) ++i;
    return field;
}

static int    toInt(const string& s) { return atoi(s.c_str()); }
static double toDouble(const string& s) { return atof(s.c_str()); }

void LinkedList::parseLine(const string& line, Resident& r) {
    int i = 0;
    r.residentID = nextField(line, i);
    r.age = toInt(nextField(line, i));
    r.modeOfTransport = nextField(line, i);
    r.dailyDistance = toDouble(nextField(line, i));
    r.carbonEmissionFactor = toDouble(nextField(line, i));
    r.avgDaysPerMonth = toInt(nextField(line, i));
    r.totalMonthlyEmissions = r.dailyDistance * r.carbonEmissionFactor * r.avgDaysPerMonth;
}

bool LinkedList::loadFromCSV(const string& filename, const string& label) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Could not open '" << filename << "'\n";
        return false;
    }
    cityLabel = label;
    string line;
    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        Resident r;
        parseLine(line, r);
        append(r);
    }
    cout << "Loaded " << length << " residents from '" << filename << "'";
    if (!label.empty()) cout << " [" << label << "]";
    cout << "  |  Memory: " << getMemoryUsage() << " bytes\n";
    return true;
}

int LinkedList::getLength() const { return length; }

double LinkedList::getSortKey(const Resident& r, SortKey key) {
    switch (key) {
    case BY_AGE:             return r.age;
    case BY_DAILY_DISTANCE:  return r.dailyDistance;
    case BY_CARBON_EMISSION: return r.totalMonthlyEmissions;
    }
    return 0;
}

static const int INSERTION_THRESHOLD = 16;

void LinkedList::insertionSort(int* idx, const double* keys, int low, int high) {
    for (int i = low + 1; i <= high; ++i) {
        int    pi = idx[i];
        double pk = keys[pi];
        int j = i - 1;
        while (j >= low && keys[idx[j]] > pk) { idx[j + 1] = idx[j]; --j; }
        idx[j + 1] = pi;
    }
}

void LinkedList::medianOfThree(int* idx, const double* keys, int low, int high) {
    int mid = low + (high - low) / 2;
    if (keys[idx[mid]] < keys[idx[low]])  swap(idx[low], idx[mid]);
    if (keys[idx[high]] < keys[idx[low]]) swap(idx[low], idx[high]);
    if (keys[idx[high]] < keys[idx[mid]]) swap(idx[mid], idx[high]);
    swap(idx[mid], idx[low]);
}

void LinkedList::threeWayPartition(int* idx, const double* keys,
    int low, int high, int& lt, int& gt) {
    double pv = keys[idx[low]];
    lt = low; gt = high;
    int i = low + 1;
    while (i <= gt) {
        double ck = keys[idx[i]];
        if (ck < pv) swap(idx[i++], idx[lt++]);
        else if (ck > pv) swap(idx[i], idx[gt--]);
        else              ++i;
    }
}

void LinkedList::quickSortHelper(int* idx, const double* keys, int low, int high) {
    while (low < high) {
        if (high - low < INSERTION_THRESHOLD) {
            insertionSort(idx, keys, low, high);
            break;
        }
        medianOfThree(idx, keys, low, high);
        int lt, gt;
        threeWayPartition(idx, keys, low, high, lt, gt);
        if ((lt - 1 - low) <= (high - gt - 1)) {
            quickSortHelper(idx, keys, low, lt - 1);
            low = gt + 1;
        }
        else {
            quickSortHelper(idx, keys, gt + 1, high);
            high = lt - 1;
        }
    }
}

void LinkedList::sort(SortKey key) {
    if (length <= 1) return;

    Node** nodes = new Node * [length];
    double* keys = new double[length];
    int* indices = new int[length];

    int i = 0;
    for (Node* cur = head; cur; cur = cur->next, ++i) {
        nodes[i] = cur;
        keys[i] = getSortKey(cur->data, key);
        indices[i] = i;
    }

    auto t0 = chrono::high_resolution_clock::now();
    quickSortHelper(indices, keys, 0, length - 1);
    auto t1 = chrono::high_resolution_clock::now();

    Resident* sorted = new Resident[length];
    for (int j = 0; j < length; ++j) sorted[j] = nodes[indices[j]]->data;

    Node* cur = head;
    for (int j = 0; j < length; ++j, cur = cur->next) cur->data = sorted[j];

    static const char* names[] = { "Age", "Daily Distance", "Carbon Emission" };
    size_t mem = sizeof(Node*) * length + sizeof(double) * length
        + sizeof(int) * length + sizeof(Resident) * length;

    cout << "Sort by " << names[key] << " completed in "
        << fixed << setprecision(4)
        << chrono::duration<double, milli>(t1 - t0).count() << " ms"
        << "  |  Aux memory: " << mem << " bytes\n";

    delete[] sorted;
    delete[] indices;
    delete[] keys;
    delete[] nodes;
}

static void printHeader() {
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

static void printRow(const Resident& r) {
    cout << left << setw(14) << r.residentID
        << setw(6) << r.age
        << setw(12) << r.modeOfTransport
        << right << setw(10) << fixed << setprecision(2) << r.dailyDistance
        << setw(16) << r.carbonEmissionFactor
        << setw(6) << r.avgDaysPerMonth
        << setw(22) << r.totalMonthlyEmissions << "\n";
}

void LinkedList::print() const {
    printHeader();
    for (Node* cur = head; cur; cur = cur->next) printRow(cur->data);
}

void LinkedList::printTop5() const {
    cout << "\nTop 5 Highest Emitters:\n"
        << left << setw(15) << "Resident ID"
        << setw(10) << "Age"
        << setw(20) << "Mode of Transport"
        << "Total Emission\n"
        << string(65, '-') << "\n";
    int n = 0;
    for (Node* cur = head; cur && n < 5; cur = cur->next, ++n)
        cout << left << setw(15) << cur->data.residentID
        << setw(10) << cur->data.age
        << setw(20) << cur->data.modeOfTransport
        << fixed << setprecision(2) << cur->data.totalMonthlyEmissions << " kg CO2\n";
}

void LinkedList::printDatasetSummary() const {
    static const char* modes[6] = { "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool" };
    double modeTotals[6] = {};
    int    modeCounts[6] = {};
    double grandTotal = 0;

    for (Node* cur = head; cur; cur = cur->next) {
        grandTotal += cur->data.totalMonthlyEmissions;
        for (int m = 0; m < 6; ++m)
            if (cur->data.modeOfTransport == modes[m]) {
                modeTotals[m] += cur->data.totalMonthlyEmissions;
                ++modeCounts[m];
                break;
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

    for (int m = 0; m < 6; ++m) {
        if (modeCounts[m] == 0) continue;
        cout << left << setw(18) << modes[m]
            << right << setw(8) << modeCounts[m]
            << fixed << setprecision(2)
            << setw(20) << modeTotals[m]
            << setw(18) << modeTotals[m] / modeCounts[m] << "\n";
    }

    cout << string(60, '-') << "\n"
        << "Total Residents  : " << length << "\n"
        << "Total CO2 (month): " << fixed << setprecision(2) << grandTotal << " kg\n"
        << "Avg CO2/Resident : " << (length > 0 ? grandTotal / length : 0.0) << " kg\n"
        << string(60, '-') << "\n";
}

static void printSearchHeader(const string& title) {
    cout << "\nSearch Results - " << title << "\n"
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

static void printSearchFooter(int matches, double ms) {
    cout << string(82, '-') << "\n"
        << "Total matches: " << matches << "\n"
        << "Search time  : " << fixed << setprecision(4) << ms << " ms"
        << "  |  Time complexity: O(n)\n";
}

void LinkedList::searchByAgeGroup(int minAge, int maxAge) const {
    string title = "Age " + to_string(minAge) + " to " + to_string(maxAge);
    int cnt = 0;
    auto t0 = chrono::high_resolution_clock::now();
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data.age >= minAge && cur->data.age <= maxAge) ++cnt;
    auto t1 = chrono::high_resolution_clock::now();

    printSearchHeader(title);
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data.age >= minAge && cur->data.age <= maxAge) printRow(cur->data);
    printSearchFooter(cnt, chrono::duration<double, milli>(t1 - t0).count());
}

void LinkedList::searchByTransport(const string& mode) const {
    int cnt = 0, shown = 0;
    auto t0 = chrono::high_resolution_clock::now();
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data.modeOfTransport == mode) ++cnt;
    auto t1 = chrono::high_resolution_clock::now();

    cout << "\n--- LINEAR SEARCH: Transport = '" << mode << "' ---\n";
    printSearchHeader("Transport: " + mode);
    for (Node* cur = head; cur && shown < 5; cur = cur->next)
        if (cur->data.modeOfTransport == mode) { printRow(cur->data); ++shown; }
    if (cnt > 5) cout << "... and " << (cnt - 5) << " more not shown.\n";
    printSearchFooter(cnt, chrono::duration<double, milli>(t1 - t0).count());
}

void LinkedList::searchByDistance(double threshold) const {
    string title = "Daily Distance > " + to_string(threshold) + " km";
    int cnt = 0;
    auto t0 = chrono::high_resolution_clock::now();
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data.dailyDistance > threshold) ++cnt;
    auto t1 = chrono::high_resolution_clock::now();

    printSearchHeader(title);
    for (Node* cur = head; cur; cur = cur->next)
        if (cur->data.dailyDistance > threshold) printRow(cur->data);
    printSearchFooter(cnt, chrono::duration<double, milli>(t1 - t0).count());
}

void LinkedList::searchByAgeBinary(int targetAge) const {
    cout << "\n--- BINARY SEARCH: Age = " << targetAge << " ---\n";

    Node** arr = new Node * [length];
    int i = 0;
    for (Node* cur = head; cur; cur = cur->next) arr[i++] = cur;

    auto t0 = chrono::high_resolution_clock::now();
    int lo = 0, hi = length - 1, found = -1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid]->data.age == targetAge) { found = mid; break; }
        else if (arr[mid]->data.age < targetAge)  lo = mid + 1;
        else                                        hi = mid - 1;
    }
    auto t1 = chrono::high_resolution_clock::now();

    if (found != -1) {
        cout << "Match found!\n";
        printHeader();
        printRow(arr[found]->data);
    }
    else {
        cout << "No resident found with age " << targetAge << ".\n";
    }
    cout << "Binary Search time: "
        << fixed << setprecision(4)
        << chrono::duration<double, milli>(t1 - t0).count() << " ms"
        << "  |  Time complexity: O(log n)\n";
    cout << "Note: requires list sorted by age first.\n";
    delete[] arr;
}

string getAgeGroup(int age) {
    if (age >= 6 && age <= 17)  return "Children & Teenagers (6-17)";
    if (age >= 18 && age <= 25)  return "University Students / Young Adults (18-25)";
    if (age >= 26 && age <= 45)  return "Working Adults Early Career (26-45)";
    if (age >= 46 && age <= 60)  return "Working Adults Late Career (46-60)";
    if (age >= 61 && age <= 100) return "Senior Citizens / Retirees (61-100)";
    return "Unknown";
}

void LinkedList::ageGroupAnalysis() const {
    static const int    N = 5;
    static const char* groupNames[N] = {
        "Children & Teenagers (6-17)",
        "University Students / Young Adults (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career (46-60)",
        "Senior Citizens / Retirees (61-100)"
    };
    static const int groupMin[N] = { 6, 18, 26, 46,  61 };
    static const int groupMax[N] = { 17, 25, 45, 60, 100 };
    static const char* modes[6] = { "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool" };

    struct Stats {
        double total = 0; int count = 0;
        int    mc[6] = {}; double me[6] = {};
    } gs[N];

    auto t0 = chrono::high_resolution_clock::now();
    for (Node* cur = head; cur; cur = cur->next) {
        int age = cur->data.age;
        for (int g = 0; g < N; ++g) {
            if (age < groupMin[g] || age > groupMax[g]) continue;
            gs[g].total += cur->data.totalMonthlyEmissions;
            ++gs[g].count;
            for (int m = 0; m < 6; ++m)
                if (cur->data.modeOfTransport == modes[m]) {
                    ++gs[g].mc[m]; gs[g].me[m] += cur->data.totalMonthlyEmissions;
                    break;
                }
            break;
        }
    }
    auto t1 = chrono::high_resolution_clock::now();

    string label = cityLabel.empty() ? "All Datasets" : cityLabel;
    cout << "\n===== Age Group Analysis [" << label << "] =====\n";

    for (int g = 0; g < N; ++g) {
        int top = 0;
        for (int m = 1; m < 6; ++m) if (gs[g].mc[m] > gs[g].mc[top]) top = m;

        cout << "\nAge Group : " << groupNames[g] << "\n"
            << string(72, '-') << "\n"
            << left << setw(18) << "Transport"
            << right << setw(7) << "Count"
            << setw(20) << "Total CO2 (kg)"
            << setw(20) << "Avg CO2/Resident\n"
            << string(72, '-') << "\n";

        for (int m = 0; m < 6; ++m) {
            if (gs[g].mc[m] == 0) continue;
            cout << left << setw(18) << modes[m]
                << right << setw(7) << gs[g].mc[m]
                << fixed << setprecision(2)
                << setw(20) << gs[g].me[m]
                << setw(20) << gs[g].me[m] / gs[g].mc[m] << "\n";
        }

        double avg = gs[g].count > 0 ? gs[g].total / gs[g].count : 0.0;
        cout << string(72, '-') << "\n"
            << "Most Preferred Transport : " << modes[top] << "\n"
            << "Total Emission           : " << fixed << setprecision(2)
            << gs[g].total << " kg CO2\n"
            << "Average Emission         : " << avg << " kg CO2/resident\n"
            << "Total Residents          : " << gs[g].count << "\n";
    }
    cout << "\nAnalysis time: " << fixed << setprecision(4)
        << chrono::duration<double, milli>(t1 - t0).count() << " ms\n";
}

void LinkedList::printInsights() const {
    static const int    N = 5;
    static const char* groupNames[N] = {
        "Children & Teenagers (6-17)",
        "University Students / Young Adults (18-25)",
        "Working Adults Early Career (26-45)",
        "Working Adults Late Career (46-60)",
        "Senior Citizens / Retirees (61-100)"
    };
    static const int groupMin[N] = { 6, 18, 26, 46,  61 };
    static const int groupMax[N] = { 17, 25, 45, 60, 100 };
    static const char* modes[6] = { "Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool" };

    struct Stats {
        double total = 0; int count = 0;
        int mc[6] = {};
    } gs[N];

    for (Node* cur = head; cur; cur = cur->next) {
        int age = cur->data.age;
        for (int g = 0; g < N; ++g) {
            if (age < groupMin[g] || age > groupMax[g]) continue;
            gs[g].total += cur->data.totalMonthlyEmissions;
            ++gs[g].count;
            for (int m = 0; m < 6; ++m)
                if (cur->data.modeOfTransport == modes[m]) { ++gs[g].mc[m]; break; }
            break;
        }
    }

    int highestGroup = 0;
    for (int g = 1; g < N; ++g)
        if (gs[g].total > gs[highestGroup].total) highestGroup = g;

    cout << "\n========== INSIGHTS & RECOMMENDATIONS ==========\n"
        << string(72, '-') << "\n"
        << left << setw(40) << "Age Group"
        << right << setw(10) << "Residents"
        << setw(18) << "Total CO2 (kg)"
        << setw(18) << "Avg CO2\n"
        << string(72, '-') << "\n";

    for (int g = 0; g < N; ++g) {
        double avg = gs[g].count > 0 ? gs[g].total / gs[g].count : 0.0;
        cout << left << setw(40) << groupNames[g]
            << right << setw(10) << gs[g].count
            << fixed << setprecision(2)
            << setw(18) << gs[g].total
            << setw(18) << avg;
        if (g == highestGroup) cout << "  << HIGHEST";
        cout << "\n";
    }

    cout << string(72, '-') << "\n";
    cout << "\nKey Findings:\n";
    cout << "  1. Highest emitting group : " << groupNames[highestGroup]
        << " (" << fixed << setprecision(2) << gs[highestGroup].total << " kg CO2)\n";

    int carIdx = 0, bikeIdx = 2;
    cout << "  2. Car users vs Bicycle users by group:\n";
    cout << "     " << left << setw(42) << "Age Group"
        << setw(8) << "Car" << setw(10) << "Bicycle\n";
    for (int g = 0; g < N; ++g)
        cout << "     " << left << setw(42) << groupNames[g]
        << setw(8) << gs[g].mc[carIdx] << setw(10) << gs[g].mc[bikeIdx] << "\n";

    cout << "\nRecommendations for City Planners:\n"
        << string(72, '-') << "\n"
        << "  - " << groupNames[highestGroup]
        << " produce the most emissions.\n"
        << "    Priority: subsidize public transport or carpooling for this group.\n"
        << "  - Expand protected cycling infrastructure to shift car users\n"
        << "    (especially ages 26-60) toward bicycles for short distances.\n"
        << "  - Introduce school bus programs targeting the 6-17 age group\n"
        << "    to reduce car trips by parents.\n"
        << "  - Offer remote-work or flexible-hour incentives to Working Adults\n"
        << "    to reduce total commuting days per month.\n"
        << string(72, '-') << "\n";
}

void LinkedList::searchByAgeBinary(int targetAge) const {
    cout << "\n--- BINARY SEARCH: Age = " << targetAge << " ---\n";

    Node** arr = new Node * [length];
    int i = 0;
    for (Node* cur = head; cur; cur = cur->next) arr[i++] = cur;

    auto t0 = chrono::high_resolution_clock::now();
    int lo = 0, hi = length - 1, found = -1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (arr[mid]->data.age == targetAge) { found = mid; break; }
        else if (arr[mid]->data.age < targetAge)  lo = mid + 1;
        else                                       hi = mid - 1;
    }

    int cnt = 0;
    int left = found, right = found;
    if (found != -1) {
        while (left > 0 && arr[left - 1]->data.age == targetAge) --left;
        while (right < length - 1 && arr[right + 1]->data.age == targetAge) ++right;
        cnt = right - left + 1;
    }
    auto t1 = chrono::high_resolution_clock::now();

    if (found != -1) {
        cout << cnt << " match(es) found.\n";
        printHeader();
        for (int j = left; j <= right; ++j) printRow(arr[j]->data);
    }
    else {
        cout << "No resident found with age " << targetAge << ".\n";
    }

    cout << "Binary Search time: "
        << fixed << setprecision(4)
        << chrono::duration<double, milli>(t1 - t0).count() << " ms"
        << "  |  Time complexity: O(log n + k)\n";
    cout << "Note: requires list sorted by age first.\n";
    delete[] arr;
}