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
    sc_clock clk("clk", 1, SC_NS);
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> wdata;
    sc_signal<uint32_t> rdata;
    sc_signal<bool> we;
    sc_signal<bool> hit;
    sc_signal<size_t> cycles_;
    sc_signal<size_t> total_hits;
    sc_signal<size_t> total_misses;

    sc_signal<uint32_t> memory_addr;
    sc_signal<uint32_t> memory_wdata;
    sc_signal<uint32_t> memory_rdata;
    sc_signal<bool> memory_we;

    Cache cache("cache", cacheLines, CacheLineSize, cacheLatency, memoryLatency, directMapped);
    Memory memory("memory");
    Controller controller("controller", &cache, &memory, directMapped, requests, num_Requests);

    // Connect signals
    cache.clk(clk);
    cache.addr(addr);
    cache.wdata(wdata);
    cache.rdata(rdata);
    cache.we(we);
    cache.hit(hit);
    cache.cycles_(cycles_);

    cache.memory_addr(memory_addr);
    cache.memory_wdata(memory_wdata);
    cache.memory_rdata(memory_rdata);
    cache.memory_we(memory_we);

    memory.clk(clk);
    memory.addr(memory_addr);
    memory.wdata(memory_wdata);
    memory.we(memory_we);
    memory.rdata(memory_rdata);

    controller.clk(clk);
    controller.addr.write(addr);
    controller.wdata.write(wdata);
    controller.we.write(we);
    controller.total_hits(total_hits);
    controller.total_misses(total_misses);
    controller.rdata(rdata);

    memory.initialize();
    cache.initialize();


    sc_start(cycles, SC_NS);

    Result result = controller.get_results();

    if (tracefile)
    {
        // TODO: Implement tracefile
    }

    return result;
}
