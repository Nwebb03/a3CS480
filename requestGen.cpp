#include "requestGen.h"
//## Names
//- Nate Webb cssc1467
//- Noah Bakayou cssc1409

//## Class Information
//- CS480-02-Sum202

//## Assignment Information
//- Assignment #3 - Memory Allocation Simulation

requestGen::requestGen(int maxPid, int minRequestSize, int maxRequestSize)
{
    this->maxPid = maxPid;
    this->minRequestSize = minRequestSize;
    this->maxRequestSize = maxRequestSize;
    gen = std::mt19937(1); // Initialize random number generator with seed 1 for reproducibility
    sizeDist = std::uniform_int_distribution<>(minRequestSize, maxRequestSize);
    processDist = std::uniform_int_distribution<>(1, maxPid); // Process IDs from 1 to maxPid
    allocOrDeallocDist = std::uniform_int_distribution<>(0, 1); // 0 for allocation, 1 for deallocation
    availableProcesses = std::set<int>(); // Initialize available processes set
    // Validate parameters
    if (maxPid <= 0 || maxRequestSize < minRequestSize || minRequestSize < 1) {
        throw std::invalid_argument("All parameters must be positive integers.");
    }
    // Initialize available processes
    for (int i = 1; i <= maxPid; ++i) {
        availableProcesses.insert(i);
    }
}

std::vector<int> requestGen::generateRequest() {
    int action = allocOrDeallocDist(gen);
    int processID;

    // Guard against invalid states to avoid throwing
    if (allocatedProcesses.empty() && availableProcesses.empty()) {
        // No valid operation remains. Emit a deallocation for a non-existent PID 0
        // so the simulator can continue and count a denied operation if desired.
        return {1, 0, sizeDist(gen)};
    }

    // If nothing allocated, must allocate
    if (action == 1 && allocatedProcesses.empty()) {
        action = 0;
    }
    // If no new processes are available, must deallocate
    if (action == 0 && availableProcesses.empty()) {
        action = 1;
    }

    if (action == 0) {
        // Ensure unique process ID for allocation (use each PID at most once overall)
        if (availableProcesses.empty()) {
            // Fallback: force a deallocation of some existing process below
            action = 1;
        } else {
            std::set<int>::iterator it = availableProcesses.begin();
            std::advance(it, processDist(gen) % availableProcesses.size()); // Move iterator to a random index
            processID = *it;
            // Consume this PID permanently so it will never be reused after deallocation
            availableProcesses.erase(it);
            allocatedProcesses.insert(processID);
            int numUnits = sizeDist(gen);
            return {0, processID, numUnits};
        }
    }

    // Deallocation path
    // Choose a process ID from the allocated processes
    if (allocatedProcesses.empty()) {
        // Nothing to deallocate; return a benign deallocation request for PID 0
        return {1, 0, sizeDist(gen)};
    }
    // Randomly select a process ID from the allocated set
    {
        std::set<int>::iterator it = std::next(allocatedProcesses.begin(), processDist(gen) % allocatedProcesses.size());
        processID = *it;
        allocatedProcesses.erase(it); // Remove the process ID from the set (do not re-add to availableProcesses)
        int numUnits = sizeDist(gen);
        return {1, processID, numUnits};
    }
}
