#ifndef PRIMITIVEGATECOUNTCALC_H
#define PRIMITIVEGATECOUNTCALC_H
#include <unistd.h>


size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits,
                          bool directMapped);

unsigned muxGateCount(unsigned cacheLines, unsigned indexBits, bool directMapped);

unsigned comparatorGateCount(unsigned tagBits);

unsigned storageGateCount(unsigned CacheLineSize, unsigned cacheLines, unsigned tagBits, bool directMapped);


#endif //PRIMITIVEGATECOUNTCALC_H
