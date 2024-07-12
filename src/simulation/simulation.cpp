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

    // Create instance of the Controller and Result
    Controller controller("controller", directMapped, requests, num_Requests, cacheLines, CacheLineSize, cacheLatency, memoryLatency);
    Result result{};

    controller.clk(clk);
    controller.total_hits(total_hits);
    controller.total_misses(total_misses);
    controller.cycles_(cycles_);
    controller.primitiveGateCount(primitiveGateCount);
    controller.requests_out(requests_out);

    // Start the simulation
    sc_start(cycles, SC_NS);

    if (controller.cycles >= cycles && controller.request_counter < num_Requests)
    {
        result.cycles = SIZE_MAX;
        sc_stop();
    }

    // Get the results
    result = {
        cycles_.read(),
        total_hits.read(),
        total_misses.read(),
        primitiveGateCount.read()
    };

    // Handle tracefile if needed
    if (tracefile)
    {
        // TODO: Implement tracefile handling
    }

    // Return the results of the simulation
    return result;
}
