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
    sc_in<bool> clk;             ///< Clock Signal
    sc_signal<bool> we;          ///< Write Enable Signal
    sc_signal<uint32_t> addr;    ///< Address Signal
    sc_signal<uint32_t> wdata;   ///< Write Data Signal
    sc_out<uint32_t> rdata;      ///< Read Data Signal
    sc_out<size_t> total_hits;   ///< Total Hits Signal
    sc_out<size_t> total_misses; ///< Total Misses Signal
    const bool DIRECT_MAPPED;    ///< boolean flag for cache mapping

    SC_HAS_PROCESS(Controller);  ///< Macro for multiple-argument constructor of the Module

    /**
     * Constructor of the Module
     * @param name
     * @param cache
     * @param memory
     * @param directMapped
     * @param requests
     * @param num_requests
     */
    Controller(sc_module_name name, Cache* cache, Memory* memory, bool directMapped, struct Request* requests,
               size_t num_requests) :
        sc_module(name),
        DIRECT_MAPPED(directMapped),
        cache(cache),
        memory(memory),
        requests(requests),
        num_requests(num_requests),
        request_counter(0),
        hit_count(0),
        miss_count(0),
        cycles(0)
    {
        // Defining the process of the Module
        SC_THREAD(controller_process);
        sensitive << clk.pos();
    }

    /**
     * Destructor of the Controller Module
     * @return Result
     */
    struct Result get_results() const
    {
        // Calculate the number of primitive gates needed to implement the cache in hardware
        const size_t primitiveGateCount = ::primitiveGateCount(
            cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->tag_bits, cache->index_bits, DIRECT_MAPPED);
        return {cycles, miss_count, hit_count, primitiveGateCount};
    }

private:
    Cache* cache;             ///< Cache Module
    Memory* memory;           ///< Memory Module
    struct Request* requests; ///< Array of Requests
    size_t num_requests;      ///< Number of Requests
    size_t request_counter;   ///< Request Counter
    size_t hit_count;         ///< Hit Counter
    size_t miss_count;        ///< Miss Counter
    size_t cycles;            ///< Number of Cycles

    /**
     * Process of the Controller Module that orchestrates the Cache and Memory Modules
     */
    void controller_process()
    {
        // Iterate over all requests and process them accordingly
        while (request_counter < num_requests)
        {
            const struct Request& request = requests[request_counter]; ///< Get the current request
            addr.write(request.addr);                                  ///< Write the address to the Cache
            wdata.write(request.data);                                 ///< Write the data to the Cache
            we.write(request.we);                                      ///< Write the we-signal to the Cache

            // Wait for the Cache to process the request (one cycle)
            wait(clk.posedge_event());
            cycles += cache->cycles_.read();                           ///< Increment the number of the total cycles
            if (cache->hit.read())                                     ///< Check for hit or miss
            {
                hit_count++;
            }
            else
            {
                miss_count++;
            }
            rdata.write(cache->rdata.read());                    ///< Read the data from the Cache
            request_counter++;                                         ///< Increment the request counter
        }
        total_hits.write(hit_count);                                   ///< Write the total hits to the output signal
        total_misses.write(miss_count);                                ///< Write the total misses to the output signal
    }
};

#endif //CONTROLLER_H