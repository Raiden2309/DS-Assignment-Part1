#ifndef LINKED_LIST_QUICK_SORT_HPP
#define LINKED_LIST_QUICK_SORT_HPP

#include <string>
#include <fstream>
#include "resident.hpp"

struct Node {
    Resident data;
    Node* next;
    explicit Node(const Resident& r) : data(r), next(nullptr) {}
};

enum SortKey { BY_AGE, BY_DAILY_DISTANCE, BY_CARBON_EMISSION };

std::string getAgeGroup(int age);

class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void  append(const Resident& r);
    void  clear();
    void  sort(SortKey key = BY_AGE);
    void  print()               const;
    void  printTop5()           const;
    void  printDatasetSummary() const;
    void  printInsights()       const;
    int   getLength()           const;
    size_t getMemoryUsage()     const;
    Node* getHead()             const { return head; }

    bool  loadFromCSV(const std::string& filename, const std::string& cityLabel = "");

    void  searchByAgeGroup(int minAge, int maxAge)         const;
    void  searchByTransport(const std::string& mode)       const;
    void  searchByDistance(double threshold)               const;
    void  searchByAgeBinary(int targetAge)                 const;
    void  ageGroupAnalysis()                               const;

private:
    Node* head;
    Node* tail;
    int   length;
    std::string cityLabel;

    void  insertionSort(int* idx, const double* keys, int low, int high);
    void  medianOfThree(int* idx, const double* keys, int low, int high);
    void  threeWayPartition(int* idx, const double* keys,
        int low, int high, int& lt, int& gt);
    void  quickSortHelper(int* idx, const double* keys, int low, int high);

    static double getSortKey(const Resident& r, SortKey key);
    static void   parseLine(const std::string& line, Resident& r);

	void searchByAgeBinary(int targetAge) const;
};

#endif