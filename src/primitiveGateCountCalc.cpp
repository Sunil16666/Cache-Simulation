#include "primitiveGateCountCalc.h"


size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits,
                          bool directMapped)
{
    unsigned const muxGateCount = tagBits * ::muxGateCount(cacheLines, indexBits, directMapped);
    unsigned const comparatorGateCount = directMapped
                                  ? ::comparatorGateCount(tagBits)
                                  : ::comparatorGateCount(tagBits) * cacheLines;
    unsigned const storageGateCount = ::storageGateCount(CacheLineSize, cacheLines, tagBits, directMapped);

    if (directMapped)
    {
        return muxGateCount + comparatorGateCount + storageGateCount;
    }
    return comparatorGateCount + storageGateCount + 1;
    // 1 for the OR gate (N-Inputs) using the comparator outputs
}

unsigned muxGateCount(unsigned cacheLines, unsigned indexBits, bool directMapped)
{
    unsigned const not_num = indexBits;
    unsigned const and_num = cacheLines; // (I + 1) Inputs
    unsigned constexpr or_num = 1; // N Inputs
    return not_num + and_num + or_num;
}

unsigned comparatorGateCount(unsigned tagBits)
{
    unsigned const xor_num = 1 * tagBits;
    unsigned constexpr and_num = 1; // xor_num Inputs
    return xor_num + and_num;
}

unsigned storageGateCount(unsigned CacheLineSize, unsigned cacheLines, unsigned tagBits, bool directMapped)
{
    unsigned constexpr transistors_per_bit = 6; // assuming 6T SRAM Cell, and assuming transistors are primitive gates
    unsigned const transistors_per_line = (CacheLineSize * 8 + tagBits + 1) * transistors_per_bit;
    // 8 bits per byte, +1 for valid bit, +tagBits for tag

    return cacheLines * transistors_per_line;
}
