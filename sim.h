#ifndef SIM_H
#define SIM_H
//## Names
//- Nate Webb cssc1467
//- Noah Bakayou cssc1409

//## Class Information
//- CS480-02-Sum202

//## Assignment Information
//- Assignment #3 - Memory Allocation Simulation
#include <vector>
#include <iostream>


class memoryManager;
class requestGen;

// Aggregate statistics for the simulation run
struct Stats {
    int numRequests = 0;
    int numAllocations = 0;
    int numDeallocations = 0;
    int numNodesTraversedFirst = 0;
    int numNodesTraversedBest = 0;
    int totalFragmentsFirst = 0;
    int totalFragmentsBest = 0;
    int firstStatus = 0;
    int bestStatus = 0;
    int firstAllocationFailures = 0;
    int bestAllocationFailures = 0;
};

// Allocation/deallocation handling
void handleAllocation(memoryManager& memFirst,
                      memoryManager& memBest,
                      int processId,
                      int numUnits,
                      Stats& stats);

void handleDeallocation(memoryManager& memFirst,
                        memoryManager& memBest,
                        int processId,
                        Stats& stats);

// Single request dispatcher
void processRequest(memoryManager& memFirst,
                    memoryManager& memBest,
                    const std::vector<int>& request,
                    Stats& stats);

// Simulation driver
std::vector<std::vector<int>> runSimulation(memoryManager& memFirst,
                   memoryManager& memBest,
                   requestGen& reqGen,
                   int numRequests,
                   Stats& stats);

// Stats printer
void printStats(const Stats& stats);

#endif // SIM_H
 