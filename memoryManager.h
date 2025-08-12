#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <list>
#include <cstddef> // for size_t
#include <stdexcept> // for std::invalid_argument
#include <iostream>
#include <iterator>
// Allocation policy type
enum class Policy {
    FirstFit,
    BestFit
};

// Represents a contiguous block of memory
struct memoryBlock {
    int start;         // Starting unit index
    int size;          // Number of units in this block
    int process_id;    // Owning process (-1 if free)
    bool free;         // Whether block is available
};

class memoryManager {
public:
    // Constructors
    explicit memoryManager(int numUnits, Policy policyType = Policy::FirstFit);
    memoryManager(); // default

    int allocate_mem(int process_id, int num_units);
    int deallocate_mem(int process_id);
    int fragment_count();
    void coalesce(); // Merge adjacent free blocks
    void printMemoryBlocks();

private:
    using iterator = std::list<memoryBlock>::iterator;
    std::list<memoryBlock> memoryBlocks; // linked list of blocks
    int totalSize;                     // total memory size in units
    Policy policy{Policy::FirstFit};     // allocation policy

    // Internal helpers
    int allocateFirstFit(int process_id, int num_units);
    int allocateBestFit(int process_id, int num_units);
};

#endif 
