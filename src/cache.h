#ifndef CACHE_H
#define CACHE_H

#include <list>
#include <queue>
#include <systemc.h>

#include "cacheLine.h"
#include "simulation.h"

class Cache : public sc_module
{
public:
    const unsigned CACHE_LINES;
    const unsigned CACHE_LINE_SIZE;
    const unsigned CACHE_LATENCY;
    const unsigned MEMORY_LATENCY;
    const bool DIRECT_MAPPED;
    const unsigned offset_bits = log2(CACHE_LINE_SIZE);
    const unsigned index_bits = log2(CACHE_LINES);
    const unsigned tag_bits = 32 - offset_bits - index_bits;

    sc_in<bool> clk;
    sc_in<bool> we;
    sc_in<uint32_t> addr;
    sc_in<uint32_t> wdata;
    sc_out<uint32_t> rdata;
    sc_out<bool> hit;
    sc_out<size_t> cycles_;

    sc_in<uint32_t> memory_rdata;
    sc_out<uint32_t> memory_addr;
    sc_out<uint32_t> memory_wdata;
    sc_out<bool> memory_we;

    SC_HAS_PROCESS(Cache);

    Cache(sc_module_name name, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency,
          unsigned memoryLatency, bool directMapped) :
        sc_module(name),
        CACHE_LINES(cacheLines),
        CACHE_LINE_SIZE(cacheLineSize),
        CACHE_LATENCY(cacheLatency),
        MEMORY_LATENCY(memoryLatency),
        DIRECT_MAPPED(directMapped)
    {
        if (DIRECT_MAPPED)
        {
            SC_METHOD(process_direct_mapped);
        }
        else
        {
            SC_METHOD(process_fully_associative);
        }
        sensitive << clk.pos();
        cache = new CacheLine*[cacheLines];
    }

    void initialize()
    {
        for (int i = 0; i < CACHE_LINES; i++)
        {
            delete cache[i];
            cache[i] = new CacheLine(CACHE_LINE_SIZE);
        }
        if (!DIRECT_MAPPED)
        {
            initialize_lru_list();
        }
    }

private:
    CacheLine** cache;
    std::list<unsigned> lru_list;

    void initialize_lru_list()
    {
        lru_list.clear();
        for (unsigned i = 0; i < CACHE_LINES; ++i)
        {
            lru_list.push_back(i);
        }
    }

    void update_lru(unsigned index)
    {
        lru_list.remove(index);
        lru_list.push_back(index);
    }

    unsigned get_lru_index()
    {
        return lru_list.front();
    }

    void process_direct_mapped()
    {
        while (true)
        {
            wait(clk.posedge_event());
            uint32_t offset = addr.read() & ((1 << offset_bits) - 1);
            uint32_t index = (addr.read() >> offset_bits) & ((1 << index_bits) - 1);
            uint32_t tag = addr.read() >> (offset_bits + index_bits);

            CacheLine* line = cache[index];

            size_t cycles = CACHE_LATENCY;
            // write
            if (we.read())
            {
                cycles += MEMORY_LATENCY;
                // hit
                if (line->tag == tag && line->valid[offset])
                {
                    hit.write(true);
                    memory_addr->write(addr.read());
                    memory_wdata->write(wdata.read());
                    memory_we->write(true); // write to memory
                    // miss
                }
                else
                {
                    hit.write(false);
                    line->tag = tag;
                    line->valid[offset] = true;
                    line->data[offset] = wdata.read();
                    memory_addr->write(addr.read());
                    memory_wdata->write(wdata.read());
                    memory_we->write(true); // write to memory
                }
                // read
            }
            else
            {
                // hit
                if (line->tag == tag && line->valid[offset])
                {
                    hit.write(true);
                    rdata.write(line->data[offset]);
                    // miss
                }
                else
                {
                    cycles += MEMORY_LATENCY;
                    hit.write(false);
                    memory_addr.write(addr.read());
                    memory_we.write(false); // read from memory
                    wait(clk.posedge_event());
                    uint32_t memory_data = memory_rdata.read();
                    rdata.write(memory_data);
                    line->valid[offset] = true;
                    line->tag = tag;
                    line->data[offset] = memory_data;
                }
            }
            cycles_.write(cycles);
        }
    }

    void process_fully_associative()
    {
        while (true)
        {
            wait(clk.posedge_event());
            uint32_t offset = addr.read() & (1 << offset_bits) - 1;
            uint32_t tag = addr.read() >> offset_bits;

            auto linePointer = std::find_if(cache, cache + CACHE_LINES, [tag, offset](CacheLine* line)
            {
                return line->tag == tag && line->valid[offset];
            });
            int lineIndex = linePointer != cache + CACHE_LINES ? linePointer - cache : -1;
            size_t cycles = CACHE_LATENCY;

            if (lineIndex != -1)
            {
                hit.write(true);
                if (we.read())
                {
                    cycles += MEMORY_LATENCY;
                    cache[lineIndex]->data[offset] = wdata.read();
                    cache[lineIndex]->valid[offset] = true;
                    memory_addr.write(addr.read());
                    memory_wdata.write(wdata.read());
                    memory_we.write(true); // write to memory
                }
                else
                {
                    rdata.write(cache[lineIndex]->data[offset]);
                }
            }
            else
            {
                hit.write(false);
                uint32_t lru_pointer = get_lru_index();
                if (we.read())
                {
                    cycles += MEMORY_LATENCY;
                    cache[lru_pointer]->tag = tag;
                    cache[lru_pointer]->data[offset] = wdata.read();
                    cache[lru_pointer]->valid[offset] = true;
                    memory_addr.write(addr.read());
                    memory_wdata.write(wdata.read());
                    memory_we.write(true); // write to memory
                    update_lru(lru_pointer);
                }
                else
                {
                    cycles += MEMORY_LATENCY;
                    memory_addr.write(addr.read());
                    memory_we.write(false); // read from memory
                    wait(clk.posedge_event()); // wait for memory to provide data
                    uint32_t memory_data = memory_rdata.read();
                    rdata.write(memory_data);
                    cache[lru_pointer]->tag = tag;
                    cache[lru_pointer]->data[offset] = memory_data;
                    cache[lru_pointer]->valid[offset] = true;
                    update_lru(lru_pointer);
                }
            }
            cycles_.write(cycles);
        }
    }
};

#endif //CACHE_H
