#include "memoryManager.h"
//## Names
//- Nate Webb cssc1467
//- Noah Bakayou cssc1409

//## Class Information
//- CS480-02-Sum202

//## Assignment Information
//- Assignment #3 - Memory Allocation Simulation

memoryManager::memoryManager(int numUnits, Policy policyType) {
    if (numUnits <= 0) {
        throw std::invalid_argument("Memory size must be positive.");
    }

    totalSize = numUnits;
    policy = policyType;

    memoryBlocks.clear();
    // Start with one big free block covering the whole memory
    memoryBlock initialBlock;
    initialBlock.start = 1;
    initialBlock.size = numUnits;
    initialBlock.process_id = -1;
    initialBlock.free = true;

    memoryBlocks.push_back(initialBlock);
}

void memoryManager::coalesce() {
    iterator it = memoryBlocks.begin();
    while (it != memoryBlocks.end()) {
        // If the current block is free and the next block is also free, merge them
        if (it->free && std::next(it) != memoryBlocks.end() && std::next(it)->free) {
            it->size += std::next(it)->size; // Increase size of current block
            memoryBlocks.erase(std::next(it)); // Remove next block
        } else {
            ++it; // Move to the next block
        }
    }
}
int memoryManager::allocate_mem(int process_id, int num_units) {
    // Dispatcher based on policy
    if (policy == Policy::FirstFit) {
        return allocateFirstFit(process_id, num_units);
    } 
    else {
        return allocateBestFit(process_id, num_units);
    }
}

int memoryManager::deallocate_mem(int process_id) {
    try{
        bool found = false;
        iterator it = memoryBlocks.begin();
        while (it != memoryBlocks.end()) {
            if (it->process_id == process_id) {
                it->process_id = -1; // Mark as free
                it->free = true; // Set block as free
                found = true;
            }
            ++it;
        }
        if (!found) {
            return -1;
        }
        coalesce(); // Merge adjacent free blocks after deallocation
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Error deallocating memory: " << e.what() << std::endl;
        return -1;
    }
} 

int memoryManager::fragment_count() {
    iterator it = memoryBlocks.begin();
    coalesce(); // Ensure blocks are coalesced before counting fragments
    int count = 0;
    while(it != memoryBlocks.end()) {
        if(it->free && (it-> size == 1 || it->size == 2)) {
            count++;
        }
        ++it;
    }
    return count;
}

// Private helpers

int memoryManager::allocateFirstFit(int process_id, int num_units) {
    int numNodesTraversed = 0;
    iterator it = memoryBlocks.begin();
    while (it != memoryBlocks.end()) {
        numNodesTraversed++;
        if (it->free && it->size >= num_units) {
            // Found a suitable block
            if (it->size == num_units) {
                // Perfect fit, just mark it as allocated 
                it->process_id = process_id;
                it->free = false;
            } 
            else {
                // Split the block
                memoryBlock newBlock;
                newBlock.start = it->start + num_units;
                newBlock.size = it->size - num_units;
                newBlock.process_id = -1; // New block is free
                newBlock.free = true;

                // Update the current block
                it->size = num_units;
                it->process_id = process_id;
                it->free = false;

                // Insert the new free block after the current one
                memoryBlocks.insert(std::next(it), newBlock);
            }
            coalesce(); // Merge adjacent free blocks after allocation
            return numNodesTraversed; // Return the number of nodes traversed
        }
        
        ++it;
    }
    coalesce(); // Merge adjacent free blocks before returning
    return -1; // No suitable block found
}

int memoryManager::allocateBestFit(int process_id, int num_units) {
    int traversed = 0;
    iterator best = memoryBlocks.end();
    int bestSize = totalSize + 1;

    for (auto it = memoryBlocks.begin(); it != memoryBlocks.end(); ++it) {
        ++traversed; // count nodes visited
        if (it->free && it->size >= num_units && it->size < bestSize) {
            best = it;
            bestSize = it->size;
        }
    }
    if (best == memoryBlocks.end()) {
        return -1;
    }

    if (best->size == num_units) {
        best->process_id = process_id;
        best->free = false;
    } else {
        memoryBlock newBlock;
        newBlock.start = best->start + num_units;
        newBlock.size = best->size - num_units;
        newBlock.process_id = -1;
        newBlock.free = true;

        best->size = num_units;
        best->process_id = process_id;
        best->free = false;

        memoryBlocks.insert(std::next(best), newBlock);
    }
    return traversed;
}

void memoryManager::printMemoryBlocks() {
    iterator it = memoryBlocks.begin();
    std::cout << "Memory Blocks:\n";
    std::cout << "Start\tSize\tPID\tFree\n";
    while (it != memoryBlocks.end()) {
        std::cout << it->start << "\t"
                  << it->size << "\t"
                  << it->process_id << "\t"
                  << (it->free ? "yes" : "no") << "\n";
        ++it;
    }
}