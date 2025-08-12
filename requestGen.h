#include <random>
#include <set>
#include <stdexcept>
#include <vector>
using iterator = std::set<int>::iterator;
class requestGen {
public:
    requestGen(int maxPid, int minRequestSize, int maxRequestSize);
    std::vector<int> generateRequest();

private:
    std::set<int> allocatedProcesses; // To track allocated process IDs
    int maxPid;
    int maxRequestSize;
    int minRequestSize;
    iterator it; // Iterator for allocatedProcesses

    std::mt19937 gen;
    std::uniform_int_distribution<> sizeDist;
    std::uniform_int_distribution<> processDist;
    std::uniform_int_distribution<> allocOrDeallocDist;
};

