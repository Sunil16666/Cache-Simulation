#ifndef CACHE_H
#define CACHE_H

#include <systemc.h>

#include "cacheLine.h"
#include "simulation.h"


class Cache : public sc_module {
private:
    const int CYCLES;
    const unsigned CACHE_LINES;
    const unsigned CACHE_LINE_SIZE;
    const unsigned CACHE_LATENCY;
    const unsigned MEMORY_LATENCY;
    Request *requests;
    size_t num_requests;
    uint32_t *cacheLine;
    CacheLine** cache;

public:
    Cache(int cycles, unsigned cacheLines, unsigned CacheLineSize, unsigned cacheLatency, unsigned memoryLatency,
          size_t num_Requests, struct Request requests[num_Requests]): CYCLES(cycles), CACHE_LINES(cacheLines),
                                                                       CACHE_LINE_SIZE(CacheLineSize),
                                                                       CACHE_LATENCY(cacheLatency),
                                                                       MEMORY_LATENCY(memoryLatency),
                                                                       num_requests(num_Requests) {
        this->requests = new Request[num_Requests];
        cache = new CacheLine*[cacheLines];
        for (int i = 0; i < cacheLines; i++) {
            cache[i] = new CacheLine(CacheLineSize / sizeof(uint32_t));
        }
    }

};

#endif //CACHE_H
