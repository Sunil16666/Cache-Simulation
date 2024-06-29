#ifndef FULLYASSOCIATIVECACHE_H
#define FULLYASSOCIATIVECACHE_H

#include <systemc.h>
#include <cache.h>

#include "cache.h"

class FullyAssociativeCache : public Cache {
protected:
    unsigned offset_bits;
    unsigned tag_bits;
    unsigned lru_index;

public:
    SC_HAS_PROCESS(FullyAssociativeCache);

    FullyAssociativeCache(int cycles, unsigned cacheLines, unsigned cacheLineSize, unsigned cacheLatency,
                      unsigned memoryLatency,
                      size_t num_Requests, Request requests[num_Requests])
        : Cache(cycles, cacheLines, cacheLineSize, cacheLatency, memoryLatency, num_Requests, requests) {
        offset_bits = log2(cache_line_word_num);
        tag_bits = 32 - offset_bits;
        SC_THREAD(process);
    }

    void process() override;
    Result getResult() override;
};

#endif //FULLYASSOCIATIVECACHE_H
