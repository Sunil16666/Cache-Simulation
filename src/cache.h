#ifndef CACHE_H
#define CACHE_H

#include <systemc.h>

#include "cacheLine.h"
#include "primitiveGateCountCalc.h"
#include "simulation.h"

class Cache : public sc_module {
protected:
    const int CYCLES;
    const unsigned CACHE_LINES;
    const unsigned CACHE_LINE_SIZE;
    const unsigned CACHE_LATENCY;
    const unsigned MEMORY_LATENCY;
    unsigned cache_line_word_num;
    size_t misses;
    size_t hits;
    size_t cycles_taken;
    Request *requests;
    size_t num_requests;
    CacheLine** cache;
    int primitiveGateCount;

public:
    Cache(int cycles, unsigned cacheLines, unsigned CacheLineSize, unsigned cacheLatency, unsigned memoryLatency,
          size_t num_Requests, Request requests[num_Requests]): CYCLES(cycles), CACHE_LINES(cacheLines),
                                                                       CACHE_LINE_SIZE(CacheLineSize),
                                                                       CACHE_LATENCY(cacheLatency),
                                                                       MEMORY_LATENCY(memoryLatency),
                                                                       num_requests(num_Requests) {
        this->requests = requests;
        cache = new CacheLine*[cacheLines];
        cache_line_word_num = CacheLineSize / sizeof(uint32_t);
        for (int i = 0; i < cacheLines; i++) {
            cache[i] = new CacheLine(cache_line_word_num);
        }
        //primitiveGateCount =  PRIMITIVEGATECOUNTCALC_H.calc(CacheType, args);
    }

    virtual void process();
    virtual Result getResult();
};

#endif //CACHE_H
