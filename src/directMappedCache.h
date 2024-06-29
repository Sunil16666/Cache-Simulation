#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H

#include <systemc.h>
#include <cache.h>

#include "cache.h"

class DirectMappedCache : public Cache {
protected:
    unsigned offset_bits;
    unsigned index_bits;
    unsigned tag_bits;

public:
    SC_HAS_PROCESS(DirectMappedCache);

    DirectMappedCache(int cycles, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency,
                      unsigned memoryLatency,
                      size_t num_Requests, Request requests[num_Requests])
        : Cache(cycles, cacheLines, cacheLineSize, cacheLatency, memoryLatency, num_Requests, requests) {
        offset_bits = log2(cache_line_word_num);
        index_bits = log2(CACHE_LINES);
        tag_bits = 32 - offset_bits - index_bits;
        SC_THREAD(process);
    }

    void process() override;
    Result getResult() override;
};

#endif //DIRECTMAPPEDCACHE_H
