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

class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void  appendWithLinkedList(const Resident& r);
    void  clearWithLinkedList();
    void  sortWithLinkedList(SortKey key = BY_AGE);
    void  printWithLinkedList()               const;
    void  printTop5WithLinkedList()           const;
    void  printDatasetSummaryWithLinkedList() const;
    void  printInsightsWithLinkedList()       const;
    int   getLengthWithLinkedList()           const;
    Node* getHeadWithLinkedList()             const { return head; }

    bool  loadFromCSVWithLinkedList(const std::string& filename, const std::string& cityLabel = "");

    void  searchByAgeGroupWithLinkedList(int minAge, int maxAge)         const;
    void  searchByTransportWithLinkedList(const std::string& mode)       const;
    void  searchByDistanceWithLinkedList(double threshold)               const;
    void  searchByAgeBinaryWithLinkedList(int targetAge)                 const;
    void  ageGroupAnalysisWithLinkedList()                               const;
private:
    Node* head;
    Node* tail;
    int   length;
    std::string cityLabel;

    void  medianOfThreeWithLinkedList(int* idx, const double* keys, int low, int high);
    void  threeWayPartitionWithLinkedList(int* idx, const double* keys,
        int low, int high, int& lt, int& gt);
    void  quickSortHelperWithLinkedList(int* idx, const double* keys, int low, int high);
        
    static double getSortKeyWithLinkedList(const Resident& r, SortKey key);
    static void   parseLineWithLinkedList(const std::string& line, Resident& r);
};

#endif