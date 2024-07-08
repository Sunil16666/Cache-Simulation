#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <systemc.h>

#include "cache.h"
#include "memory.h"
#include "primitiveGateCountCalc.h"


class Controller : public sc_module
{
public:
    sc_in<bool> clk;
    sc_signal<bool> we;
    sc_signal<uint32_t> addr;
    sc_signal<uint32_t> wdata;
    sc_out<uint32_t> rdata;
    sc_out<size_t> total_hits;
    sc_out<size_t> total_misses;
    const bool DIRECT_MAPPED;

    SC_HAS_PROCESS(Controller);

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
        SC_THREAD(controller_process);
        sensitive << clk.pos();
    }

    struct Result get_results() const
    {
        const size_t primitiveGateCount = ::primitiveGateCount(
            cache->CACHE_LINES, cache->CACHE_LINE_SIZE, cache->tag_bits, cache->index_bits, DIRECT_MAPPED);
        return {cycles, miss_count, hit_count, primitiveGateCount};
    }

private:
    Cache* cache;
    Memory* memory;
    struct Request* requests;
    size_t num_requests;
    size_t request_counter;
    size_t hit_count;
    size_t miss_count;
    size_t cycles;

    void controller_process()
    {
        while (request_counter < num_requests)
        {
            const struct Request& request = requests[request_counter];
            addr.write(request.addr);
            wdata.write(request.data);
            we.write(request.we);

            wait(clk.posedge_event());
            cycles += cache->cycles_.read();
            if (cache->hit.read())
            {
                hit_count++;
            }
            else
            {
                miss_count++;
            }
            rdata.write(cache->rdata.read());
            request_counter++;
        }
        total_hits.write(hit_count);
        total_misses.write(miss_count);
    }
};


#endif //CONTROLLER_H
