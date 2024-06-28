#ifndef FULLYASSOCIATIVECACHE_H
#define FULLYASSOCIATIVECACHE_H

#include <systemc.h>
#include <cache.h>

#include "cache.h"

class FullyAssociativeCache : public Cache {
public:
    void process() override;
};

#endif //FULLYASSOCIATIVECACHE_H
