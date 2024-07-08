#ifndef PRIMITIVEGATECOUNTCALC_H
#define PRIMITIVEGATECOUNTCALC_H
#include <unistd.h>


size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits,
                          bool directMapped);

int muxGateCount(unsigned cacheLines, unsigned indexBits, bool directMapped);

int comparatorGateCount(unsigned tagBits);

int storageGateCount(unsigned CacheLineSize, unsigned cacheLines, unsigned tagBits, bool directMapped);


#endif //PRIMITIVEGATECOUNTCALC_H
