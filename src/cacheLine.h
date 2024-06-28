#ifndef CACHELINE_H
#define CACHELINE_H

#include <systemc.h>

struct CacheLine {
    uint32_t* data;

    explicit CacheLine(unsigned size) {
        data = new uint32_t[size];
    }

    ~CacheLine() {
        delete[] data;
    }
};

#endif //CACHELINE_H
