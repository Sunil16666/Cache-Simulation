#ifndef PROCESSING_UNIT_H
#define PROCESSING_UNIT_H

#include <string>
#include "../simulation.h"

class ProcessingUnit {
public:
    explicit ProcessingUnit(const std::string& csvFilePath);
    void getRequests(size_t& numRequests, Request*& requests) const;

private:
    std::string csvFilePath;
};

#endif // PROCESSING_UNIT_H
