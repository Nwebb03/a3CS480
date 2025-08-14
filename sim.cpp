#include "sim.h"
#include "memoryManager.h"
#include "requestGen.h"




void handleAllocation(memoryManager& memFirst,
                      memoryManager& memBest,
                      int processId,
                      int numUnits,
                      Stats& stats)
{
    try{
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
        std::cout << "Deallocating: PID=" << processId << "\n";
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

vector<vector<int>> runSimulation(memoryManager& memFirst,
                   memoryManager& memBest,
                   requestGen& reqGen,
                   int numRequests,
                   Stats& stats){
    vector<int> fragmentsBest;
    vector<int> fragmentsFirst;
    vector<int> numNodesTraversedBest;
    vector<int> numNodesTraversedFirst;
    vector<int> req;
    vector<vector<int>> results;
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
    std::cout << "Simulation Statistics:\n";
    std::cout << "Total Requests: " << stats.numRequests << "\n";
    std::cout << "Total Allocations: " << stats.numAllocations << "\n";
    std::cout << "Total Deallocations: " << stats.numDeallocations << "\n";
    std::cout << "First-Fit Nodes Traversed: " << stats.numNodesTraversedFirst << "\n";
    std::cout << "Best-Fit Nodes Traversed: " << stats.numNodesTraversedBest << "\n";
    std::cout << "First-Fit Allocation Failures: " << stats.firstAllocationFailures << "\n";
    std::cout << "Best-Fit Allocation Failures: " << stats.bestAllocationFailures << "\n";
    std::cout << "First-Fit Fragment Count: " << stats.totalFragmentsFirst << "\n";
    std::cout << "Best-Fit Fragment Count: " << stats.totalFragmentsBest << "\n\n\n";
    cout <<"======================================\n";
    cout << "End of First-Fit Statistics\n";
    cout << "Average First-Fit Fragment Count: " << (stats.totalFragmentsFirst / static_cast<double>(stats.numRequests)) << "\n";
    cout << "Average First-Fit Nodes Traversed: " << (stats.numNodesTraversedFirst / static_cast<double>(stats.numRequests)) << "\n";
    cout <<"Percentage of First-Fit Allocation Failures: " << (stats.firstAllocationFailures * 100.0 / stats.numAllocations) << "%\n";
    cout <<"======================================\n";

    cout << "Average Best-Fit Fragment Count: " << (stats.totalFragmentsBest / static_cast<double>(stats.numRequests)) << "\n";
    cout << "Average Best-Fit Nodes Traversed: " << (stats.numNodesTraversedBest / static_cast<double>(stats.numRequests)) << "\n";
    cout <<"Percentage of Best-Fit Allocation Failures: " << (stats.bestAllocationFailures * 100.0 / stats.numAllocations) << "%\n";
    cout <<"======================================\n";

}

void writeResultsToFile(const vector<vector<int>>& results, const string& filename) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (!fp) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }
    vector<int> firstFitResults = results[0];
    vector<int> bestFitResults = results[1];
    vector<int> numNodesTraversedFirst = results[2];
    vector<int> numNodesTraversedBest = results[3];
    fprintf(fp, "RequestID, FirstFitFragments, BestFitFragments, FirstFitNodesTraversed, BestFitNodesTraversed\n");
    for (long unsigned int i = 0; i < firstFitResults.size(); ++i) {
        fprintf(fp, "%lu, %d, %d, %d, %d\n", i, firstFitResults[i], bestFitResults[i], numNodesTraversedFirst[i], numNodesTraversedBest[i]);
    }

}

int main() {
    try {
        const int numUnits    = 128;   // 256KB / 2KB = 128 units
        const int numRequests = 1000000;
        const int numProcs    = numRequests; // 1 request per process
        const int minUnits    = 3;
        const int maxUnits    = 10;


        memoryManager memManagerFirst(numUnits, Policy::FirstFit);
        memoryManager memManagerBest (numUnits, Policy::BestFit);
        requestGen reqGen(numProcs, minUnits, maxUnits);

        Stats stats{};
        vector<vector<int>> results = runSimulation(memManagerFirst, memManagerBest, reqGen, numRequests, stats);
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
