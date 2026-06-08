
# Carbon Emission Tracking System

A performance-focused C++ console application designed to load, process, sort, and analyze citizen transportation carbon emission datasets. This system evaluates algorithmic performance and memory overhead across two distinct data structures: **Dynamic Pointer Arrays** and **Doubly-Linked Lists**.

## Features

* **Multi-Structure Implementation**: Toggle seamlessly between raw pointer arrays and a dynamic node-based Linked List to handle dataset operations.
* **Advanced Sorting Algorithms**:
* **3-Way Partitioning Quick Sort (Dutch National Flag)**: Optimized for handling datasets with high collections of duplicate sorting values (e.g., repeating ages or emission values) while minimizing string data copying overhead by swapping 8-byte object references. Includes stack-frame depth tracking to measure runtime memory footprints.
* **Insertion Sort**: Implemented to provide high efficiency on smaller datasets or pre-sorted collection structures.
* **Express Sort Option**: Scans the dataset dynamically to assess its structural state ("chaos level") and automatically chooses the mathematically optimal sorting algorithm for the condition.


* **High-Speed Searching Suite**:
* **Linear Search**: Evaluates criteria across unsorted properties such as transportation categories.
* **Sub-Linear Binary Search**: Implements fast iterative partitioning to fetch precise records or range groups across sorted targets.


* **Analytical Insights Engine**: Generates comprehensive metrics categorized by 5 different age groups, tracks total monthly carbon footprints (calculated via $\text{Distance} \times \text{Factor} \times \text{Days}$), identifies dominant high-emitting categories, and provides cross-city comparative summaries.

## Project Structure

```text
├── .gitattributes             # Git attributes configuration for LF line-ending normalization
├── .gitignore                 # Exclusion configuration for Visual Studio build files
├── Resident.hpp               # Core domain structures (Resident, Dataset)
├── ArrayProcessor.hpp         # Interface definitions for pointer array functions
├── Array.cpp                  # Implementations for array loading, sorting, and analytical metrics
├── linked_list_quick_sort.hpp # Definition of the LinkedList class and Node structure
├── linked_list_quick_sort.cpp # Implementations for 3-way Quick Sort and searching on Linked Lists
└── main.cpp                   # System driver, interactive console interface, and data orchestration

```

## Dataset Configuration

The system is configured to read three native CSV data tables:

1. **`dataset1-cityA.csv`**: Contains resident demographic records primarily for middle-aged distributions (Ages 26–60).
2. **`dataset2-cityB.csv`**: Focuses on younger resident demographics (Ages 6–25) featuring localized school transit.
3. **`dataset3-cityC.csv`**: Reflects an expansive, mixed demographic footprint.

### Target Attributes

Every dataset entry maps into the underlying domain entity containing:

* `residentID` (String)
* `age` (Integer)
* `modeOfTransport` (String matching Car, Bus, Bicycle, Walking, School Bus, Carpool)
* `dailyDistance` (Double)
* `carbonEmissionFactor` (Double)
* `avgDaysPerMonth` (Integer)
* `totalMonthlyEmissions` (Double calculated value)

## Prerequisites & Compilation

The code utilizes explicit modern standard structures. Ensure your build toolchains meet the requirements below:

* **Compiler**: GCC 11+, Clang 13+, or MSVC 2022+
* **Language Standard**: C++20 (`-std=c++20` flag enabled)
* **IDE Support**: Fully compatible with Visual Studio 2022 using the included solution and project structure configuration files (`.slnx`, `.vcxproj`).

### Standard Compilation via Command Line

```bash
g++ -std=c++20 main.cpp Array.cpp linked_list_quick_sort.cpp -o EmissionTracker

```

## Execution Instructions

1. Ensure the dataset CSV documents (`dataset1-cityA.csv`, `dataset2-cityB.csv`, `dataset3-cityC.csv`) reside within the identical run folder as your compiled executable file.
2. Launch the application binary file. If using Visual Studio, open the `Assignment Part1.slnx` or `.vcxproj` solution descriptor file directly to launch and execute within the integrated workspace environment.
3. Follow the interactive shell prompts to select the targeted memory abstraction strategy (Array vs Linked List) and navigate the sorting, search, or metrics analytical interfaces.
