#include "sim.h"
#include "memoryManager.h"
#include "requestGen.h"
#include <iostream>
#include <vector>


void handleAllocation(memoryManager& memFirst,
                      memoryManager& memBest,
                      int processId,
                      int numUnits,
                      Stats& stats)
{
    stats.numAllocations++;
    std::cout << "Allocating: PID=" << processId << ", Units=" << numUnits << "\n";

    // First-Fit
    stats.firstStatus = memFirst.allocate_mem(processId, numUnits);
    if (stats.firstStatus < 0) {
        stats.firstAllocationFailures++;
        std::cout << "First-Fit allocation failed for PID=" << processId << "\n";
        memFirst.printMemoryBlocks();
        //exit(-1);
    } else {
        stats.numNodesTraversedFirst += stats.firstStatus;
    }

    // Best-Fit
    stats.bestStatus = memBest.allocate_mem(processId, numUnits);
    if (stats.bestStatus < 0) {
        stats.bestAllocationFailures++;
        std::cout << "Best-Fit allocation failed for PID=" << processId << "\n";
        memBest.printMemoryBlocks();
        //exit(-1);
    } else {
        stats.numNodesTraversedBest += stats.bestStatus;
    }
}

void handleDeallocation(memoryManager& memFirst,
                        memoryManager& memBest,
                        int processId,
                        Stats& stats)
{
    stats.numDeallocations++;
    std::cout << "Deallocating: PID=" << processId << "\n";
    memFirst.deallocate_mem(processId);
    memBest.deallocate_mem(processId);
}

void processRequest(memoryManager& memFirst,
                    memoryManager& memBest,
                    const std::vector<int>& request,
                    Stats& stats)
{
    const int action    = request[0];
    const int processId = request[1];
    const int numUnits  = request[2];

    if (action == 0) {
        handleAllocation(memFirst, memBest, processId, numUnits, stats);
    } else {
        handleDeallocation(memFirst, memBest, processId, stats);
    }
}

void runSimulation(memoryManager& memFirst,
                   memoryManager& memBest,
                   requestGen& reqGen,
                   int numRequests,
                   Stats& stats)
{
    stats.numRequests = numRequests;
    for (int i = 0; i < numRequests; ++i) {
        std::vector<int> req = reqGen.generateRequest();
        processRequest(memFirst, memBest, req, stats);
        stats.totalFragmentsBest += memBest.fragment_count();
        stats.totalFragmentsFirst += memFirst.fragment_count();
    }
}

void printStats(const Stats& stats) {
    std::cout << "Simulation Statistics:\n";
    std::cout << "Total Requests: " << stats.numRequests << "\n";
    std::cout << "Total Allocations: " << stats.numAllocations << "\n";
    std::cout << "Total Deallocations: " << stats.numDeallocations << "\n";
    std::cout << "First-Fit Nodes Traversed: " << stats.numNodesTraversedFirst << "\n";
    std::cout << "Best-Fit Nodes Traversed: " << stats.numNodesTraversedBest << "\n";
    std::cout << "First-Fit Allocation Failures: " << stats.firstAllocationFailures << "\n";
    std::cout << "Best-Fit Allocation Failures: " << stats.bestAllocationFailures << "\n";
    std::cout << "First-Fit Fragment Count: " << stats.totalFragmentsFirst << "\n";
    std::cout << "Best-Fit Fragment Count: " << stats.totalFragmentsBest << "\n";
}

// ---- Optional: a small main that uses the simulation ----

int main() {
    const int numUnits    = 128;   // 256KB / 2KB = 128 units
    const int numProcs    = 10000; // Max 10,000 processes
    const int minUnits    = 3;
    const int maxUnits    = 10;
    const int numRequests = 10000;

    memoryManager memManagerFirst(numUnits, Policy::FirstFit);
    memoryManager memManagerBest (numUnits, Policy::BestFit);
    requestGen reqGen(numProcs, minUnits, maxUnits);

    Stats stats{};
    runSimulation(memManagerFirst, memManagerBest, reqGen, numRequests, stats);
    printStats(stats);
    return 0;
}
