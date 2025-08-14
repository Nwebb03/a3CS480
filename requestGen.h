#include <random>
#include <set>
#include <stdexcept>
#include <vector>
//## Names
//- Nate Webb cssc1467
//- Noah Bakayou cssc1409

//## Class Information
//- CS480-02-Sum202

//## Assignment Information
//- Assignment #3 - Memory Allocation Simulation
class requestGen {
public:
    requestGen(int maxPid, int minRequestSize, int maxRequestSize);
    std::vector<int> generateRequest();

private:
    std::set<int> allocatedProcesses; // To track allocated process IDs
    std::set<int>availableProcesses; // To track available process IDs
    int maxPid;
    int maxRequestSize;
    int minRequestSize;

    std::mt19937 gen;
    std::uniform_int_distribution<> processDist;
    std::uniform_int_distribution<> sizeDist;
    std::uniform_int_distribution<> allocOrDeallocDist;
};

