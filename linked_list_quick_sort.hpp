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
    void printDatasetSummaryWithLinkedList() const;
    void printInsightsWithLinkedList() const;
    Node* getHeadWithLinkedList() const { return head; }
    bool loadFromCSVWithLinkedList(const std::string& filename, const std::string& label = "");

    // linear
    void searchByTransportLinearWithLinkedList(const std::string& transportMode) const;

    // Binary
    void searchByAgeGroupBinaryWithLinkedList(int minAge, int maxAge) const;
    void searchByDistanceBinaryWithLinkedList(double distanceThreshold) const;


    void ageGroupAnalysisWithLinkedList() const;
    int  getlinkedListCountWithLinkedList() const;
    void printMemoryUsageWithLinkedList() const;
    void compareWithOtherCity(const LinkedList& other) const;

private:
    Node* head;
    int   linkedListCount;
    std::string cityLabel;
    SortKey currentSortKey;
    static const std::string TRANSPORT_MODES[6];
    static const int TRANSPORT_COUNT = 6;

    void swapNodeData(Node* a, Node* b);
    void medianOfThreeWithLinkedList(Node* start, Node* mid, Node* end);
    void threeWayPartitionWithLinkedList(Node* start, Node* end,
        Node*& lessThanBoundary, Node*& greaterThanBoundary);
    void quickSortHelperWithLinkedList(Node* start, Node* end);
    static double getSortKeyWithLinkedList(const Resident& resident, SortKey sortKey);
    Node* getMiddle(Node* start, Node* last) const;
};

#endif