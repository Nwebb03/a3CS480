#include "requestGen.h"

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
    if (action == 1 && allocatedProcesses.empty()) {
        // If no processes are allocated, we can only allocate
        action = 0;
    }
    if (action == 0) {
        //Ensure unique process ID for allocation
        while (true) {
            std::set<int>::iterator it = availableProcesses.begin();
            if (availableProcesses.empty()) {
                throw std::runtime_error("No available processes for allocation.");
            }
            std::advance(it, processDist(gen) % availableProcesses.size()); // Move iterator to the random index
            processID = *it;
            // Check if the process ID is already allocated
            if (allocatedProcesses.find(processID) == allocatedProcesses.end()) {
                allocatedProcesses.insert(processID);
                break;

            }
        }
    }
    else {
        // Choose a process ID from the allocated processes
        if (allocatedProcesses.empty()) {
            throw std::runtime_error("No processes allocated for deallocation.");
        }
        // Randomly select a process ID from the allocated set
        std::set<int>::iterator it = std::next(allocatedProcesses.begin(), processDist(gen) % allocatedProcesses.size());
        processID = *it;
        allocatedProcesses.erase(it); // Remove the process ID from the set
    }
    int numUnits = sizeDist(gen);
    // Return a vector containing the action, process ID, and number of units
    return {action, processID, numUnits};
}
