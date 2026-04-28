#ifndef LINKED_LIST_QUICK_SORT_HPP
#define LINKED_LIST_QUICK_SORT_HPP

#include <string>
#include <fstream>
#include "Resident.hpp"

struct Node {
    Resident* data;
    Node* next;
    Node(const Resident& resident) : data(new Resident(resident)), next(nullptr) {}
    ~Node() { delete data; }
};

enum SortKey { BY_AGE, BY_DAILY_DISTANCE, BY_CARBON_EMISSION };

class LinkedList {
public:
    LinkedList();
    ~LinkedList();

    LinkedList(const LinkedList&) = delete;
    LinkedList& operator=(const LinkedList&) = delete;

    void appendWithLinkedList(const Resident& resident);
    void sortWithLinkedList(SortKey sortKey = BY_AGE);
    void printWithLinkedList() const;
    void printTop5WithLinkedList() const;
    void printDatasetSummaryWithLinkedList() const;
    void printInsightsWithLinkedList() const;
    Node* getHeadWithLinkedList() const { return head; }
    bool loadFromCSVWithLinkedList(const std::string& filename, const std::string& label = "");
    void searchByAgeGroupWithLinkedList(int minAge, int maxAge) const;
    void searchByTransportWithLinkedList(const std::string& transportMode) const;
    void searchByDistanceWithLinkedList(double distanceThreshold) const;
    void searchByAgeBinaryWithLinkedList(int targetAge) const;
    void searchByDistanceBinaryWithLinkedList(double targetDistance) const;
    void ageGroupAnalysisWithLinkedList() const;
    int  getlinkedListCountWithLinkedList() const;
    void printMemoryUsageWithLinkedList() const;
    void compareWithOtherCity(const LinkedList& other) const;

private:
    Node* head;
    int   linkedListCount;
    std::string cityLabel;
    SortKey currentSortKey;
    std::string uniqueTransports[20];
    int    uniqueTransportCount;

    void buildUniqueTransportsWithLinkedList();
    void swapNodeData(Node* a, Node* b);
    void medianOfThreeWithLinkedList(Node* start, Node* mid, Node* end);
    void threeWayPartitionWithLinkedList(Node* start, Node* end,
        Node*& lessThanBoundary, Node*& greaterThanBoundary);
    void quickSortHelperWithLinkedList(Node* start, Node* end);
    static double getSortKeyWithLinkedList(const Resident& resident, SortKey sortKey);
    Node* getMiddle(Node* start, Node* last) const;
    double getAbsolute(double val) const;
};

#endif