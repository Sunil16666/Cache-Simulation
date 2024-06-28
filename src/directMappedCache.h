#ifndef DIRECTMAPPEDCACHE_H
#define DIRECTMAPPEDCACHE_H

#include <systemc.h>
#include <cache.h>

#include "cache.h"

class DirectMappedCache : public Cache {
public:
    void process() override;
};

#endif //DIRECTMAPPEDCACHE_H
