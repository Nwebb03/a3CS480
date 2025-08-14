#include "sim.h"
#include "memoryManager.h"
#include "requestGen.h"
#include <vector>
#include <iostream>
#include <string>
#include <cstdio>
#include <iomanip>

void handleAllocation(memoryManager& memFirst,
                      memoryManager& memBest,
                      int processId,
                      int numUnits,
                      Stats& stats)
{
    try{
        // First-Fit
        stats.firstStatus = memFirst.allocate_mem(processId, numUnits);
        if (stats.firstStatus < 0) {
            stats.firstAllocationFailures++;
        } else {
            stats.numNodesTraversedFirst += stats.firstStatus;
        }

        // Best-Fit
        stats.bestStatus = memBest.allocate_mem(processId, numUnits);
        if (stats.bestStatus < 0) {
            stats.bestAllocationFailures++;
        } else {
            stats.numNodesTraversedBest += stats.bestStatus;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error allocating memory: " << e.what() << std::endl;
        stats.firstAllocationFailures++;
        stats.bestAllocationFailures++;
    }
    stats.numAllocations++;
}

void handleDeallocation(memoryManager& memFirst,
                        memoryManager& memBest,
                        int processId,
                        Stats& stats){
    try {
        // Deallocate from both managers
        stats.numDeallocations++;
        memFirst.deallocate_mem(processId);
        memBest.deallocate_mem(processId);
    }
    catch (const std::exception& e) {
        std::cerr << "Error deallocating memory: " << e.what() << std::endl;
    }
}

void processRequest(memoryManager& memFirst,
                    memoryManager& memBest,
                    const std::vector<int>& request,
                    Stats& stats){
    // Process a single request
    try{
        stats.numRequests++;
        const int action    = request[0];
        const int processId = request[1];
        const int numUnits  = request[2];

		if (action == 0) {
            handleAllocation(memFirst, memBest, processId, numUnits, stats);
        } else {
            handleDeallocation(memFirst, memBest, processId, stats);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error processing request: " << e.what() << std::endl;
    }
}

std::vector<std::vector<int>> runSimulation(memoryManager& memFirst,
                   memoryManager& memBest,
                   requestGen& reqGen,
                   int numRequests,
                   Stats& stats){
    std::vector<int> fragmentsBest;
    std::vector<int> fragmentsFirst;
    std::vector<int> numNodesTraversedBest;
    std::vector<int> numNodesTraversedFirst;
    std::vector<int> req;
    std::vector<std::vector<int>> results;
    int fragmentCountBest = 0;
    int fragmentCountFirst = 0;


    try{
        for (int i = 0; i < numRequests; ++i) {
            req = reqGen.generateRequest();
            processRequest(memFirst, memBest, req, stats);
            //Collect best fit statistics
            numNodesTraversedBest.push_back(stats.numNodesTraversedBest);
            fragmentsBest.push_back(memBest.fragment_count());
            fragmentCountBest = memBest.fragment_count();
            stats.totalFragmentsBest += fragmentCountBest;

            //Collect first fit statistics
            numNodesTraversedFirst.push_back(stats.numNodesTraversedFirst);
            fragmentCountFirst = memFirst.fragment_count();
            fragmentsFirst.push_back(fragmentCountFirst);
            stats.totalFragmentsFirst += fragmentCountFirst;
        }
        return {fragmentsFirst, fragmentsBest, numNodesTraversedFirst, numNodesTraversedBest};
    }
    catch (const std::exception& e) {
        std::cerr << "Error during simulation: " << e.what() << std::endl;
        return {{}, {}, {}, {}};
    }

}

void printStats(const Stats& stats) {
    std::cout.setf(std::ios::fixed);
    std::cout << std::setprecision(6);

    std::cout << "End of First Fit Allocation\n";
    std::cout << "Average External Fragments Each Request: "
              << (stats.totalFragmentsFirst / static_cast<double>(stats.numRequests)) << "\n";
    std::cout << "Average Nodes Transversed Each Allocation: "
              << (stats.numNodesTraversedFirst / static_cast<double>(stats.numAllocations)) << "\n";
    std::cout << "Percentage Allocation Requests Denied Overall: "
              << (stats.firstAllocationFailures * 100.0 / stats.numAllocations) << "%\n";

    std::cout << "End of Best Fit Allocation\n";
    std::cout << "Average External Fragments Each Request: "
              << (stats.totalFragmentsBest / static_cast<double>(stats.numRequests)) << "\n";
    std::cout << "Average Nodes Transversed Each Allocation: "
              << (stats.numNodesTraversedBest / static_cast<double>(stats.numAllocations)) << "\n";
    std::cout << "Percentage Allocation Requests Denied Overall: "
              << (stats.bestAllocationFailures * 100.0 / stats.numAllocations) << "%\n";

}

void writeResultsToFile(const std::vector<std::vector<int>>& results, const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    std::vector<int> firstFitResults = results[0];
    std::vector<int> bestFitResults = results[1];
    std::vector<int> numNodesTraversedFirst = results[2];
    std::vector<int> numNodesTraversedBest = results[3];
    fprintf(fp, "RequestID, FirstFitFragments, BestFitFragments, FirstFitNodesTraversed, BestFitNodesTraversed\n");
    for (long unsigned int i = 0; i < firstFitResults.size(); ++i) {
        fprintf(fp, "%lu, %d, %d, %d, %d\n", i, firstFitResults[i], bestFitResults[i], numNodesTraversedFirst[i], numNodesTraversedBest[i]);
    }
    fclose(fp);

}

int main() {
    try {
        const int numUnits    = 128;   // 256KB / 2KB = 128 units
        const int numRequests = 10000;
        const int numProcs    = numRequests; // 1 request per process
        const int minUnits    = 3;
        const int maxUnits    = 10;


        memoryManager memManagerFirst(numUnits, Policy::FirstFit);
        memoryManager memManagerBest (numUnits, Policy::BestFit);
        requestGen reqGen(numProcs, minUnits, maxUnits);

        Stats stats{};
        std::vector<std::vector<int>> results = runSimulation(memManagerFirst, memManagerBest, reqGen, numRequests, stats);
        printStats(stats);
        writeResultsToFile(results, "results.csv");
        exit(0);
    }
    catch (const std::exception& e) {
        std::cerr << "Error in main: " << e.what() << std::endl;
        exit(-1);
    
    }
    return 0;
}  
