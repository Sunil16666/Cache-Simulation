#include "directMappedCache.h"
#include <systemc.h>

void DirectMappedCache::process() {
    hits = 0;
    misses = 0;
    cycles_taken = 0;
    while (true) {
        wait(CACHE_LATENCY);
        cycles_taken += CACHE_LATENCY;
        for (size_t i = 0; i < num_requests; i++) {
            Request request = requests[i];
            uint32_t offset = request.addr & (1 << offset_bits) - 1;
            uint32_t index = request.addr >> offset_bits & (1 << index_bits) - 1;
            uint32_t tag = request.addr >> offset_bits + index_bits;

            CacheLine* line = cache[index];
            if (request.we) {
                if (line->tag == tag && line->valid[offset]) {
                    hits++;
                } else {
                    misses++;
                    wait(MEMORY_LATENCY);
                    cycles_taken += MEMORY_LATENCY;
                    line->tag = tag;
                    line->valid[offset] = true;
                    line->data[offset] = request.data;
                }
            } else {
                if (line->tag == tag && line->valid[offset]) {
                    hits++;
                    request.data = line->data[offset];
                } else {
                    misses++;
                    wait(MEMORY_LATENCY);
                    cycles_taken += MEMORY_LATENCY;
                    request.data = line->data[offset];
                }
            }
        }
        cycles_taken = cycles_taken <= CYCLES ? cycles_taken : SIZE_MAX;
    }
}

Result DirectMappedCache::getResult() {
    Result result{};
    result.cycles = cycles_taken;
    result.hits = hits;
    result.misses = misses;
    result.primitiveGateCount = primitiveGateCount;
    return result;
}