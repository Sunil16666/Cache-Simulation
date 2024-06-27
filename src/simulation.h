#ifndef SIMULATION_H
#define SIMULATION_H

#include <cstddef>
#include <cstdint>

/**
 * Function prototype (Decleration) of running the SystemC Cache Simulation
 * @param cycles
 * @param directMapped
 * @param cacheLines
 * @param CacheLineSize
 * @param cacheLatency
 * @param memoryLatency
 * @param num_Requests
 * @param requests
 * @param tracefile
 * @return Result
 */
struct Result run_simulation(
    int cycles,
    int directMapped,
    unsigned cacheLines,
    unsigned CacheLineSize,
    unsigned cacheLatency,
    unsigned memoryLatency,
    size_t num_Requests,
    struct Request requests[num_Requests],
    const char *tracefile);

/**
 * Structure representing a request for the cache (memory request)
 */
struct Request {
    uint32_t addr; ///<
    uint32_t data;
    int we;
};

/**
 * Structure representing the result of a SystemC Cache Simulation
 */
struct Result {
    size_t cycles;             ///< Number of cycles needed to complete the simulation
    size_t misses;             ///< Number of total misses occured during the simulation
    size_t hits;               ///< Number of total hits occured during the simulation
    size_t primitiveGateCount; ///< Number of primitive Gates needed to realize such Cache
};

#endif //SIMULATION_H
