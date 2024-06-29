#include "fullyAssociativeCache.h"
#include <systemc.h>

void FullyAssociativeCache::process() {
    hits = 0;
    misses = 0;
    cycles_taken = 0;
    lru_index = 0;
    while (true) {
        for (size_t i = 0; i < num_requests; i++) {
            wait(CACHE_LATENCY);
            cycles_taken += CACHE_LATENCY;

            Request request = requests[i];
            uint32_t offset = request.addr & (1 << offset_bits) - 1;
            uint32_t tag = request.addr >> offset_bits;

        }
    }
}

Result FullyAssociativeCache::getResult() {
    Result result{};
    result.cycles = cycles_taken;
    result.hits = hits;
    result.misses = misses;
    result.primitiveGateCount = primitiveGateCount;
    return result;
}

