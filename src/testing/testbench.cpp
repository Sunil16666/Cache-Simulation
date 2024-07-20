#include <iostream>
#include "simulation.h"

int main()
{
    // Define simulation parameters
    int cycles = 1000;                  // Number of simulation cycles
    int directMapped = 1;               // 1 for direct-mapped, 0 for fully associative
    unsigned cacheLines = 16;           // Number of cache lines
    unsigned cacheLineSize = 4;         // Size of each cache line in words
    unsigned cacheLatency = 1;          // Cache latency in cycles
    unsigned memoryLatency = 10;        // Memory latency in cycles
    size_t numRequests = 5;             // Number of memory requests

    // Create an array of requests
    Request requests[] = {
        {0x00000000, 0xA, 1},  // Write 0xA to address 0x00000000
        {0x00000004, 0xB, 1},  // Write 0xB to address 0x00000004
        {0x00000000, 0x0, 0},  // Read from address 0x00000000
        {0x00000008, 0xC, 1},  // Write 0xC to address 0x00000008
        {0x00000004, 0x0, 0}   // Read from address 0x00000004
    };

    // Optional trace file name for VCD output
    const char* tracefile = "cache_simulation_trace";

    // Run the simulation
    Result result = run_simulation(cycles, directMapped, cacheLines, cacheLineSize, cacheLatency, memoryLatency, numRequests, requests, tracefile);

    // Output the results
    std::cout << "Simulation Results:" << std::endl;
    std::cout << "Total Cycles: " << result.cycles << std::endl;
    std::cout << "Total Hits: " << result.hits << std::endl;
    std::cout << "Total Misses: " << result.misses << std::endl;
    std::cout << "Primitive Gate Count: " << result.primitiveGateCount << std::endl;
    for (size_t i = 0; i < numRequests; i++)
    {
        std::cout << "Request " << i << ": " << requests[i].addr << " -> " << requests[i].data << std::endl;
    }

    return 0;
}
