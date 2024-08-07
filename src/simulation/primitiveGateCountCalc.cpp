#include "primitiveGateCountCalc.h"

#include <cstddef>

/**
 * Function to calculate the number of primitive gates required to implement the cache
 * @param cacheLines
 * @param CacheLineSize
 * @param tagBits
 * @param indexBits
 * @param directMapped
 * @return Number of primitive gates
 * @remark The calculation for the number of primitive gates is based on the following assumptions:
 *  - The cache is implemented using a 6T SRAM cell
 *  - Transistors are considered to be primitive gates
 *  - "More than Two-Inputs" gates are assumed to be still primitive gates
 */
size_t primitiveGateCount(unsigned const cacheLines, unsigned const CacheLineSize, unsigned const tagBits,
                          unsigned const indexBits,
                          bool const directMapped)
{
    // Calculate the number of gates required to realize a multiplexer, comparator, and storage cells
    unsigned const muxGateCount = tagBits * ::muxGateCount(cacheLines, indexBits);
    unsigned const comparatorGateCount = directMapped
                                             ? ::comparatorGateCount(tagBits)
                                             : ::comparatorGateCount(tagBits) * cacheLines;
    unsigned const storageGateCount = ::storageGateCount(CacheLineSize, cacheLines, tagBits);
    unsigned const lruGateCount = ::lruGateCount(cacheLines, tagBits);

    // If direct mapped, we need the mux, comparator, and storage gates
    if (directMapped)
    {
        return muxGateCount + comparatorGateCount + storageGateCount;
    }

    // Total number of primitive gates
    return comparatorGateCount + storageGateCount + 1 + lruGateCount;
    ///< One OR gate (N-Inputs) using the comparator outputs
}

/**
 * A helper function to calculate the number of primitive gates required to implement a multiplexer
 * @param cacheLines
 * @param indexBits
 * @return Number of primitive gates
 */
unsigned muxGateCount(unsigned const cacheLines, unsigned const indexBits)
{
    // N = cacheLines | I = indexBits
    unsigned const not_num = indexBits; ///< Number of NOT gates
    unsigned const and_num = cacheLines; ///< (I + 1) Inputs | Number of AND gates
    unsigned constexpr or_num = 1; ///< N Inputs | Number of OR gates
    return not_num + and_num + or_num; ///< Total number of gates
}

/**
 * A helper function to calculate the number of primitive gates required to implement a comparator
 * @param tagBits
 * @return Number of primitive gates
 */
unsigned comparatorGateCount(unsigned const tagBits)
{
    unsigned const xor_num = 1 * tagBits; ///< Number of XOR gates
    unsigned constexpr and_num = 1; ///< xor_num Inputs | Number of AND gates
    return xor_num + and_num; //< Total number of gates
}

/**
 * A helper function to calculate the number of primitive gates required to implement the cache cells
 * @param CacheLineSize
 * @param cacheLines
 * @param tagBits
 * @return Number of primitive gates
 */
unsigned storageGateCount(unsigned const CacheLineSize, unsigned const cacheLines, unsigned const tagBits)
{
    // assuming 6T SRAM Cell, and assuming transistors are primitive gates
    unsigned constexpr transistors_per_bit = 6;

    // 8 bits per byte, +1 for valid bit, +tagBits for tag
    unsigned const transistors_per_line = (CacheLineSize * 8 + tagBits + 1) * transistors_per_bit;

    // Total number of transistors
    return cacheLines * transistors_per_line;
}

/**
 * A helper function to calculate the number of primitive gates required to implement the LRU logic
 * @param cacheLines
 * @param tagBits
 * @return Number of primitive gates
 */
unsigned lruGateCount(unsigned const cacheLines, unsigned const tagBits)
{
    // LRU update logic estimation
    unsigned const comparatorGateCount = ::comparatorGateCount(tagBits) * cacheLines;

    // Estimate for control logic gates for LRU (this is a simplification)
    unsigned const lruLogicGateCount = cacheLines * 15;

    return comparatorGateCount + lruLogicGateCount;
}
