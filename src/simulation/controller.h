#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <systemc>

#include "cache.h"
#include "memory.h"
#include "primitiveGateCountCalc.h"

using namespace sc_core;


/**
 * Controller Module of the Cache Simulation
 */
class Controller final : public sc_module
{
public:
    sc_in<bool> clk; ///< Clock Signal
    sc_in<size_t> cycles_max; ///< Maximum Cycles Signal

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
    Controller(sc_module_name name, const bool directMapped, struct Request* requests,
               const size_t num_requests, const unsigned cacheLines, const unsigned cacheLineSize,
               const unsigned cacheLatency,
               const unsigned memoryLatency) :
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
    ~Controller() override
    {
        delete cache;
        delete memory;
    }

    void trace_signals(sc_trace_file* trace_file) const
    {
        sc_trace(trace_file, clk, "clk");
        sc_trace(trace_file, we, "we");
        sc_trace(trace_file, addr, "addr");
        sc_trace(trace_file, data, "data");
        sc_trace(trace_file, hit, "hit");
        sc_trace(trace_file, cycles_per_request, "cycles_per_request");
        sc_trace(trace_file, rdata, "rdata");
        sc_trace(trace_file, memory_rdata, "memory_rdata");
        sc_trace(trace_file, memory_wdata, "memory_wdata");
        sc_trace(trace_file, memory_addr, "memory_addr");
        sc_trace(trace_file, memory_we, "memory_we");
        sc_trace(trace_file, total_hits, "total_hits");
        sc_trace(trace_file, total_misses, "total_misses");
        sc_trace(trace_file, cycles_, "cycles");
        sc_trace(trace_file, primitiveGateCount, "primitiveGateCount");
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

            // Wait for the Cache to process the request and get the result
            wait(cache->finishedProcessingEvent);
            if (!request.we)
            {
                requests[request_counter].data = rdata.read();
                std::printf("Read Data: %u\n", rdata.read());
            }

            cycles += cycles_per_request.read(); ///< Increment the number of cycles per request
            if (hit.read()) ///< Check for hit or miss
            {
                hit_count++;
            }
            else
            {
                miss_count++;
            }

            request_counter++; ///< Increment the request counter
            is_process_finished(); ///< Check if the process is finished
        }

        // Output the final values to the signals (cylces equal excatly to the number of max cycles)
        std::cout << "Simulation finished" << std::endl;
        std::cout << "Total Hits: " << hit_count << std::endl;
        std::cout << "Total Misses: " << miss_count << std::endl;
        std::cout << "Total Cycles: " << cycles << std::endl;
        std::cout << "Primitive Gate Count: " << ::primitiveGateCount(cache->CACHE_LINES, cache->CACHE_LINE_SIZE,
                                                                    cache->TAG_BITS, cache->INDEX_BITS,
                                                                    DIRECT_MAPPED) << std::endl;
        total_hits.write(hit_count); ///< Write the total hits to the output signal
        total_misses.write(miss_count); ///< Write the total misses to the output signal
        cycles_.write(cycles); ///< Write the total cycles to the output signal
        primitiveGateCount.write(::primitiveGateCount(cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->TAG_BITS,
                                                      cache->INDEX_BITS,
                                                      DIRECT_MAPPED)); ///< Calculate and write the primitive gate count
        requests_out.write(requests);
    }

    void is_process_finished()
    {
        std::cout << "Request Counter: " << request_counter << std::endl;
        std::cout << "Num Requests: " << num_requests << std::endl;
        std::cout << "Cycles: " << cycles << std::endl;
        if (request_counter >= num_requests)
        {
            std::cout << "Simulation finished, all requests processed" << std::endl;
            total_hits.write(hit_count);
            total_misses.write(miss_count);
            cycles_.write(cycles);
            primitiveGateCount.write(::primitiveGateCount(cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->TAG_BITS,
                                                          cache->INDEX_BITS,
                                                          DIRECT_MAPPED));
            requests_out.write(requests);
            sc_stop();
        }
        else if (cycles >= cycles_max.read() && request_counter < num_requests)
        {
            std::printf("Cyles: %lu\n", cycles);
            std::printf("MAX Cycles: %lu\n", cycles_max.read());
            std::printf("Request Counter: %lu\n", request_counter);
            std::printf("numRequests: %lu\n", num_requests);
            std::printf("Simulation did not run for the specified number of cycles\n");
            total_hits.write(hit_count);
            total_misses.write(miss_count);
            cycles_.write(SIZE_MAX);
            primitiveGateCount.write(::primitiveGateCount(cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->TAG_BITS,
                                                          cache->INDEX_BITS,
                                                          DIRECT_MAPPED));
            requests_out.write(requests);
            sc_stop();
        }
    }
};

#endif //CONTROLLER_H
