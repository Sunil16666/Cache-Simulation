#include "simulation.h"

#include <systemc.h>

#include "Controller.h"


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
    struct Request* requests,
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

    sc_trace_file *trace = nullptr;
    if (tracefile)
    {
        trace = sc_create_vcd_trace_file(tracefile);
        controller.trace_signals(trace);
    }

    // Start the simulation and run for the specified number of cycles or until all requests are processed
    sc_start(cycles, SC_NS);

    if (cycles_.read() < cycles && controller.request_counter < num_Requests)
    {
        std::printf("Simulation did not run for the specified number of cycles\n");
        result = {
            SIZE_MAX,
            total_hits.read(),
            total_misses.read(),
            primitiveGateCount.read()
        };
    }
    else
    {
        result = {
            cycles_.read(),
            total_hits.read(),
            total_misses.read(),
            primitiveGateCount.read()
        };
    }

    if (trace)
    {
        sc_close_vcd_trace_file(trace);
    }

    // Return the results of the simulation
    return result;
}

int sc_main(int argc, char* argv[])
{
    std::cout << "ERROR" << std::endl;
    return 1;
}