#ifndef PRIMITIVEGATECOUNTCALC_H
#define PRIMITIVEGATECOUNTCALC_H
#include <unistd.h>


class PrimitiveGateCountCalc
{
public:
    static size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits,
                                     bool directMapped)
    {
        int muxGateCount = tagBits * muxGateCount(cacheLines, indexBits, directMapped);
        int comparatorGateCount = directMapped
                                      ? comparatorGateCount(tagBits)
                                      : comparatorGateCount(tagBits) * cacheLines;
        int storageGateCount = storageGateCount(CacheLineSize, cacheLines, tagBits, directMapped);

        if (directMapped)
        {
            return muxGateCount + comparatorGateCount + storageGateCount;
        }
        else
        {
            return comparatorGateCount + storageGateCount + 1;
            // 1 for the OR gate (N-Inputs) using the comparator outputs
        }
    }

    static int muxGateCount(unsigned cacheLines, unsigned indexBits, bool directMapped)
    {
        int not_num = indexBits;
        int and_num = cacheLines; // (I + 1) Inputs
        int or_num = 1; // N Inputs
        return not_num + and_num + or_num;
    }

    static int comparatorGateCount(unsigned tagBits)
    {
        int xor_num = 1 * tagBits;
        int and_num = 1; // xor_num Inputs
        return xor_num + and_num;
    }

    static int storageGateCount(unsigned CacheLineSize, unsigned cacheLines, unsigned tagBits, bool directMapped)
    {
        int transistors_per_bit = 6; // assuming 6T SRAM Cell, and assuming transistors are primitive gates
        int transistors_per_line = (CacheLineSize * 8 + tagBits + 1) * transistors_per_bit;
        // 8 bits per byte, +1 for valid bit, +tagBits for tag
        return cacheLines * transistors_per_line;
    }
};


#endif //PRIMITIVEGATECOUNTCALC_H
