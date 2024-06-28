#ifndef CACHE_H
#define CACHE_H

#include <systemc.h>

#include "cacheLine.h"
#include "primitiveGateCountCalc.h"
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
    CacheLine** cache;
    int primitiveGateCount;
    sc_fifo<Request>* request_queue;


public:
    Cache(int cycles, unsigned cacheLines, unsigned CacheLineSize, unsigned cacheLatency, unsigned memoryLatency,
          size_t num_Requests, Request requests[num_Requests]): CYCLES(cycles), CACHE_LINES(cacheLines),
                                                                       CACHE_LINE_SIZE(CacheLineSize),
                                                                       CACHE_LATENCY(cacheLatency),
                                                                       MEMORY_LATENCY(memoryLatency),
                                                                       num_requests(num_Requests) {
        this->requests = requests;
        cache = new CacheLine*[cacheLines];
        for (int i = 0; i < cacheLines; i++) {
            cache[i] = new CacheLine(CacheLineSize / sizeof(uint32_t));
        }
        //primitiveGateCount =  PRIMITIVEGATECOUNTCALC_H.calc(CacheType, args);
        request_queue = new sc_fifo<Request>(num_Requests);
    }

    virtual void process();
};

#endif //CACHE_H
