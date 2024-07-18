#ifndef CACHE_H
#define CACHE_H

#include <list>
#include <queue>
#include <systemc.h>

#include "cacheLine.h"
#include "simulation.h"

/**
 * Cache Module of the Simulation
 */
class Cache final : public sc_module
{
public:
    const unsigned CACHE_LINES; ///< Number of Cache Lines
    const unsigned CACHE_LINE_SIZE; ///< Size of a Cache Line
    const unsigned CACHE_LATENCY; ///< Latency of the Cache in Cycles
    const unsigned MEMORY_LATENCY; ///< Latency of the Memory in Cycles
    const bool DIRECT_MAPPED; ///< boolean flag for cache mapping
    const unsigned OFFSET_BITS = log2(CACHE_LINE_SIZE); ///< Number of bits for the offset
    const unsigned INDEX_BITS = log2(CACHE_LINES); ///< Number of bits for the index
    const unsigned TAG_BITS = 32 - OFFSET_BITS - INDEX_BITS; ///< Number of bits for the tag
    // Cache Input Signals
    sc_in<bool> clk; ///< Clock Signal
    sc_in<bool> we; ///< Write Enable Signal
    sc_in<uint32_t> addr; ///< Address Signal
    sc_in<uint32_t> wdata; ///< Write Data Signal
    // Cache Output Signals
    sc_out<uint32_t> rdata; ///< Read Data Signaln
    sc_out<bool> hit; ///< Hit Signal
    sc_out<size_t> cycles_total; ///< Number of Cycles needed to complete the operation
    // Memory Input Signals
    sc_in<uint32_t> memory_rdata; ///< Read Data Signal from Memory
    // Memory Output Signals
    sc_out<uint32_t> memory_addr; ///< Address Signal to Memory
    sc_out<uint32_t> memory_wdata; ///< Write Data Signal to Memory
    sc_out<bool> memory_we; ///< Write Enable Signal to Memory

    SC_HAS_PROCESS(Cache); ///< Macro for multiple-argument constructor of the Module

    /**
     * Constructor of the Module
     * @param name
     * @param cacheLines
     * @param cacheLineSize
     * @param cacheLatency
     * @param memoryLatency
     * @param directMapped
     */
    Cache(sc_module_name name, const unsigned cacheLines, const unsigned cacheLineSize, const unsigned cacheLatency,
          const unsigned memoryLatency, const bool directMapped) :
        sc_module(name),
        CACHE_LINES(cacheLines),
        CACHE_LINE_SIZE(cacheLineSize),
        CACHE_LATENCY(cacheLatency),
        MEMORY_LATENCY(memoryLatency),
        DIRECT_MAPPED(directMapped)
    {
        if (DIRECT_MAPPED) ///< If Direct Mapped
        {
            SC_THREAD(process_direct_mapped); ///< Process Direct Mapped Cache
        }
        else
        {
            SC_THREAD(process_fully_associative); ///< Else process Fully Associative Cache
        }
        sensitive << clk.pos() << we << addr << wdata; ///< Sensitivity List

        // Initialize the Cache
        initialize();
    }

    // Intialize and clean up the Cache
    void initialize()
    {
        cache.resize(CACHE_LINES);
        for (unsigned i = 0; i < CACHE_LINES; ++i)
        {
            cache[i] = std::make_unique<CacheLine>(CACHE_LINE_SIZE);
        }
        if (!DIRECT_MAPPED)
        {
            initialize_lru_list();
        }
    }

    ~Cache() ///< Destructor of the Module
    {
        cache.clear();
    }

private:
    std::vector<std::unique_ptr<CacheLine>> cache; ///< Cache Vector of Cache Lines
    std::list<unsigned> lru_list; ///< List for LRU

    /**
     * Initialize the LRU List
     */
    void initialize_lru_list()
    {
        lru_list.clear();
        for (unsigned i = 0; i < CACHE_LINES; ++i)
        {
            lru_list.push_back(i);
        }
    }

    /**
     * Update the LRU List
     * @param index
     */
    void update_lru(unsigned index)
    {
        lru_list.remove(index);
        lru_list.push_back(index);
    }

    /**
     * Get the Least Recently Used Index
     * @return lru index
     */
    unsigned get_lru_index() const
    {
        return lru_list.front();
    }

    /**
     * Process the request for a Direct Mapped Cache
     */
    void process_direct_mapped()
    {
        while (true)
        {
            std::printf("Process Direct Mapped\n");
            wait(clk.posedge_event());

            uint32_t const offset = addr.read() & ((1 << OFFSET_BITS) - 1); ///< Offset for current request
            uint32_t const index = (addr.read() >> OFFSET_BITS) & ((1 << INDEX_BITS) - 1);
            ///< Index for current request
            uint32_t const tag = addr.read() >> (OFFSET_BITS + INDEX_BITS); ///< Tag for current request

            if (index >= CACHE_LINES)
            {
                std::fprintf(stderr, "Index out of bounds\n");
                return;
            }
            if (tag >= (1 << TAG_BITS))
            {
                std::fprintf(stderr, "Tag out of bounds\n");
                return;
            }
            if (offset >= CACHE_LINE_SIZE)
            {
                std::fprintf(stderr, "Offset out of bounds\n");
                return;
            }

            CacheLine* line = cache[index].get(); ///< Cache Line for the current request
            if (line == nullptr)
            {
                std::fprintf(stderr, "Cache Line is null\n");
                return;
            }

            size_t cycles = CACHE_LATENCY; ///< Add Cache Latency to the total cycles

            if (we.read()) ///< Write to cache
            {
                cycles += MEMORY_LATENCY; ///< Add Memory Latency to the total cycles
                if (line->tag == tag && line->valid[offset]) ///< Cache hit
                {
                    hit.write(true); ///< Hit Signal (true)
                }
                else ///< Cache miss
                {
                    hit.write(false); ///< Hit Signal (false)
                    line->tag = tag; ///< Update the tag
                    line->valid[offset] = true; ///< Set the valid bit
                }
                line->data[offset] = wdata.read(); ///< Write the data to the cache
                memory_addr.write(addr.read()); ///< Address to memory
                memory_wdata.write(wdata.read()); ///< Write data to memory
                memory_we.write(true); ///< Enable write to memory
            }
            else ///< Read from cache
            {
                if (line->tag == tag && line->valid[offset]) ///< Cache hit
                {
                    hit.write(true); ///< Hit Signal (true)
                    rdata.write(line->data[offset]); ///< Read the data from the cache
                }
                else ///< Cache miss
                {
                    cycles += MEMORY_LATENCY; ///< Add Memory Latency to the total cycles
                    hit.write(false); ///< Hit Signal (false)
                    memory_addr.write(addr.read()); ///< Address to memory
                    memory_we.write(false); ///< Disable write to memory (read from memory)
                    wait(clk.posedge_event()); ///< Wait for memory to provide data
                    uint32_t memory_data = memory_rdata.read(); ///< Read the data from memory
                    rdata.write(memory_data); ///< Write the data to the read data signal
                    line->valid[offset] = true; ///< Set the valid bit
                    line->tag = tag; ///< Update the tag
                    line->data[offset] = memory_data; ///< Write the data to the cache
                }
            }
            cycles_total.write(cycles); ///< Write the total cycles to the cycles signal
        }
    }

    /**
     * Process the request for a Fully Associative Cache
     */
    void process_fully_associative()
    {
        while (true)
        {
            std::printf("Process Fully Associative\n");
            wait(clk.posedge_event());

            uint32_t offset = addr.read() & ((1 << OFFSET_BITS) - 1); ///< Offset for current request
            uint32_t tag = addr.read() >> OFFSET_BITS; ///< Tag for current request

            if (tag >= (1 << TAG_BITS))
            {
                std::fprintf(stderr, "Tag out of bounds\n");
                return;
            }
            if (offset >= CACHE_LINE_SIZE)
            {
                std::fprintf(stderr, "Offset out of bounds\n");
                return;
            }

            // Find the line in the cache that contains the tag and the offset
            auto linePointer = std::find_if(
                cache.begin(), cache.end(), [tag, offset](const std::unique_ptr<CacheLine>& line)
                {
                    return line->tag == tag && line->valid[offset];
                });

            // Get the index of the line in the cache and add the cache latency to the total cycles
            int lineIndex = linePointer != cache.end() ? std::distance(cache.begin(), linePointer) : -1;
            size_t cycles = CACHE_LATENCY;

            if (lineIndex != -1) ///< Cache hit
            {
                hit.write(true); ///< Hit Signal (true)
                if (we.read()) ///< Write to cache
                {
                    cycles += MEMORY_LATENCY;
                    wait(MEMORY_LATENCY);
                    cache[lineIndex]->data[offset] = wdata.read(); ///< Write the data to the cache
                    cache[lineIndex]->valid[offset] = true; ///< Set the valid bit
                    memory_addr.write(addr.read()); ///< Address to memory
                    memory_wdata.write(wdata.read()); ///< Write data to memory
                    memory_we.write(true); ///< Enable write to memory
                }
                else
                {
                    rdata.write(cache[lineIndex]->data[offset]); ///< Read the data from the cache
                }
            }
            else
            {
                hit.write(false); ///< Cache miss
                const uint32_t lru_pointer = get_lru_index(); ///< Get the LRU index
                std::printf("LRU Pointer: %d\n", lru_pointer);
                if (we.read()) ///< Write to cache
                {
                    cycles += MEMORY_LATENCY; ///< Add Memory Latency to the total cycles
                    cache[lru_pointer]->tag = tag; ///< Update the tag
                    cache[lru_pointer]->data[offset] = wdata.read(); ///< Write the data to the cache
                    cache[lru_pointer]->valid[offset] = true; ///< Set the valid bit
                    memory_addr.write(addr.read()); ///< Address to memory
                    memory_wdata.write(wdata.read()); ///< Write data to memory
                    memory_we.write(true); ///< Enable write to memory
                    update_lru(lru_pointer); ///< Update the LRU list
                }
                else
                {
                    cycles += MEMORY_LATENCY; ///< Add Memory Latency to the total cycles
                    memory_addr.write(addr.read()); ///< Address to memory
                    memory_we.write(false); ///< Disable write to memory (read from memory)
                    wait(clk.posedge_event()); ///< Wait for memory to provide data
                    uint32_t memory_data = memory_rdata.read(); ///< Read the data from memory
                    rdata.write(memory_data); ///< Write the data to the read data signal
                    cache[lru_pointer]->tag = tag; ///< Update the tag
                    cache[lru_pointer]->data[offset] = memory_data; ///< Write the data to the cache
                    cache[lru_pointer]->valid[offset] = true; ///< Set the valid bit
                    update_lru(lru_pointer); ///< Update the LRU list
                }
            }
            cycles_total.write(cycles); ///< Write the total cycles to the cycles signal
        }
    }
};

#endif //CACHE_H
