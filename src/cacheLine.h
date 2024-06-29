#ifndef CACHELINE_H
#define CACHELINE_H

#include <systemc.h>

struct CacheLine {
    uint32_t* data;
    bool* valid;
    uint32_t tag;

    explicit CacheLine(unsigned size) {
        data = new uint32_t[size];
        valid = new bool[size];
        tag = 0;
        for (int i = 0; i < size; i++) {
            valid[i] = false;
        }
    }

    ~CacheLine() {
        delete[] data;
        delete[] valid;
    }
};

#endif //CACHELINE_H
