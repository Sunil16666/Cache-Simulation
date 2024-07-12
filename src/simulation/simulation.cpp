#include "simulation.h"

#include <systemc.h>

#include "cache.h"
#include "Controller.h"
#include "memory.h"


/**
 * Runs the SystemC Cache Simulation
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
    const char* tracefile)
{
    sc_clock clk("clk", 1, SC_NS); ///< Clock signal
    sc_signal<size_t> cycles_;                            ///< Cycles signal
    sc_signal<size_t> total_hits;                         ///< Total Hits signal
    sc_signal<size_t> total_misses;                       ///< Total Misses signal
    sc_signal<size_t> primitiveGateCount;                 ///< Primitive Gate Count signal
    sc_signal<Request*> requests_out;                     ///< Requests Feedback signal
    sc_signal<uint32_t> memory_data;                      ///< Memory Data signal

    // Create instances of Cache, Memory and Controller
    Cache cache("cache", cacheLines, CacheLineSize, cacheLatency, memoryLatency, directMapped);
    Memory memory("memory");
    Controller controller("controller", &cache, &memory, directMapped, requests, num_Requests);

    // TODO: Connect signals properly
    cache.clk(clk);
    cache.memory_rdata(memory_data);

    memory.clk(clk);
    memory.rdata(memory_data);

    controller.clk(clk);
    controller.total_hits(total_hits);
    controller.total_misses(total_misses);
    controller.cycles_(cycles_);
    controller.primitiveGateCount(primitiveGateCount);
    controller.requests_out(requests_out);

    // Initialize the Cache and Memory
    memory.initialize();
    cache.initialize();

    // Start the simulation
    sc_start(cycles, SC_NS);


    // Get the results
    Result result = {
        cycles_.read(),
        total_hits.read(),
        total_misses.read(),
        primitiveGateCount.read()
    };

    sc_stop();


    // Handle tracefile if needed
    if (tracefile)
    {
        // TODO: Implement tracefile handling
    }

    // Return the results of the simulation
    return result;
}
