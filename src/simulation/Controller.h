#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <systemc.h>

#include "cache.h"
#include "memory.h"
#include "primitiveGateCountCalc.h"


/**
 * Controller Module of the Cache Simulation
 */
class Controller : public sc_module
{
public:
    sc_in<bool> clk; ///< Clock Signal

    sc_signal<bool> we; ///< Write Enable Signal
    sc_signal<uint32_t> addr; ///< Address Signal
    sc_signal<uint32_t> data; ///< Data Signal
    sc_signal<bool> hit; ///< Hit Signal
    sc_signal<size_t> cycles_per_request; ///< Cycles per Request Signal
    sc_signal<uint32_t> rdata; ///< Read Data Signal
    sc_signal<uint32_t> memory_rdata; ///< Memory Read Data Signal
    sc_signal<uint32_t> memory_wdata; ///< Memory Write Data Signal
    sc_signal<uint32_t> memory_addr; ///< Memory Address Signal
    sc_signal<bool> memory_we; ///< Memory Write Enable Signal

    sc_out<Request*> requests_out; ///< Requests Feedback Signal
    sc_out<size_t> total_hits; ///< Total Hits Signal
    sc_out<size_t> total_misses; ///< Total Misses Signal
    sc_out<size_t> cycles_; ///< Cycles Signal
    sc_out<size_t> primitiveGateCount; ///< Primitive Gate Count Signal

    const bool DIRECT_MAPPED; ///< boolean flag for cache mapping
    size_t cycles; ///< Number of Cycles
    size_t request_counter; ///< Request Counter

    SC_HAS_PROCESS(Controller); ///< Macro for multiple-argument constructor of the Module


    /**
     * Controller Module Constructor
     * @param name
     * @param directMapped
     * @param requests
     * @param num_requests
     * @param cacheLines
     * @param cacheLineSize
     * @param cacheLatency
     * @param memoryLatency
     */
    Controller(sc_module_name name, bool const directMapped, struct Request* requests,
               size_t const num_requests, unsigned const cacheLines, unsigned const cacheLineSize, unsigned const cacheLatency,
               unsigned const memoryLatency) :
        sc_module(name),
        DIRECT_MAPPED(directMapped),
        cycles(0),
        request_counter(0),
        requests(requests),
        num_requests(num_requests),
        hit_count(0),
        miss_count(0)
    {
        // Defining the process of the Module
        SC_THREAD(controller_process);

        // Create instances of Cache and Memory
        cache = new Cache("cache", cacheLines, cacheLineSize, cacheLatency, memoryLatency, DIRECT_MAPPED);
        memory = new Memory("memory");

        // Drive the signals
        cache->clk(clk);
        cache->we(we);
        cache->addr(addr);
        cache->wdata(data);
        cache->rdata(rdata);
        cache->hit(hit);
        cache->cycles_total(cycles_per_request);
        cache->memory_rdata(memory_rdata);
        cache->memory_we(memory_we);
        cache->memory_addr(memory_addr);
        cache->memory_wdata(memory_wdata);

        memory->clk(clk);
        memory->we(memory_we);
        memory->addr(memory_addr);
        memory->wdata(memory_wdata);
        memory->rdata(memory_rdata);

        sensitive << clk.pos() << requests_out;
    }

    // destructor
    ~Controller()
    {
        delete cache;
        delete memory;
    }

private:
    Cache* cache; ///< Cache Module
    Memory* memory; ///< Memory Module
    struct Request* requests; ///< Array of Requests

    size_t num_requests; ///< Number of Requests
    size_t hit_count; ///< Hit Counter
    size_t miss_count; ///< Miss Counter

    /**
     * Process of the Controller Module that orchestrates the Cache and Memory Modules
     */
    void controller_process()
    {
        // Iterate over all requests and process them accordingly
        while (request_counter < num_requests)
        {
            const struct Request& request = requests[request_counter]; ///< Get the current request
            addr.write(request.addr);
            data.write(request.data);
            we.write(request.we);
            cache->addr(addr); ///< Write the address to the Cache
            cache->wdata(data); ///< Write the data to the Cache
            cache->we(we); ///< Write the we-signal to the Cache

            // Wait for the Cache to process the request (one cycle)
            wait(clk.posedge_event());
            if (!request.we)
            {
                requests[request_counter].data = rdata.read();
            }

            cycles++; ///< Increment the number of the total cycles
            if (hit.read()) ///< Check for hit or miss
            {
                hit_count++;
            }
            else
            {
                miss_count++;
            }

            request_counter++; ///< Increment the request counter
        }

        // Output the final values to the signals
        total_hits.write(hit_count); ///< Write the total hits to the output signal
        total_misses.write(miss_count); ///< Write the total misses to the output signal
        cycles_.write(cycles); ///< Write the total cycles to the output signal
        primitiveGateCount.write(::primitiveGateCount(cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->TAG_BITS,
                                                      cache->INDEX_BITS,
                                                      DIRECT_MAPPED)); ///< Calculate and write the primitive gate count
        requests_out.write(requests);
    }
};

#endif //CONTROLLER_H
