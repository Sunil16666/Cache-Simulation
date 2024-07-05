#ifndef PRIMITIVEGATECOUNTCALC_H
#define PRIMITIVEGATECOUNTCALC_H



class PrimitiveGateCountCalc {
public:
    static size_t primitiveGateCount(unsigned cacheLines, unsigned CacheLineSize, unsigned tagBits, unsigned indexBits) {
        int muxes = tagBits * muxGateCount(cacheLines, indexBits);
    }

    static int muxGateCount(unsigned cacheLines, unsigned indexBits) {
        int not_num = indexBits;
        int and_num = cacheLines; // (I + 1) Inputs
        int or_num = 1; // N Inputs
        return not_num + and_num + or_num;
    }

    static int comparatorGateCount(unsigned tagBits) {
        int xor_num = 1 * tagBits;
        int and_num = 1; // xor_num Inputs
        return xor_num + and_num;
    }

    static int storageGateCount(unsigned CacheLineSize) {

    }

};



#endif //PRIMITIVEGATECOUNTCALC_H
