#ifndef PRIMITIVEGATECOUNTCALC_H
#define PRIMITIVEGATECOUNTCALC_H


/**
 * Function prototype to calculate the number of primitive gates required to implement the cache
 * @param cacheLines
 * @param CacheLineSize
 * @param tagBits
 * @param indexBits
 * @param directMapped
 * @return Number of primitive gates
 */
size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits,
                          bool directMapped);

/**
 * A helper function prototype to calculate the number of primitive gates required to implement a multiplexer
 * @param cacheLines
 * @param indexBits
 * @return Number of primitive gates
 */
unsigned muxGateCount(unsigned cacheLines, unsigned indexBits);

/**
 * A helper function prototype to calculate the number of primitive gates required to implement a comparator
 * @param tagBits 
 * @return Number of primitive gates
 */
unsigned comparatorGateCount(unsigned tagBits);

/**
 * A helper function prototype to calculate the number of primitive gates required to implement the cache cells
 * @param CacheLineSize
 * @param cacheLines
 * @param tagBits
 * @return Number of primitive gates
 */
unsigned storageGateCount(unsigned CacheLineSize, unsigned cacheLines, unsigned tagBits);


#endif //PRIMITIVEGATECOUNTCALC_H
