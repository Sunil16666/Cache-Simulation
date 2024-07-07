//
// Created by Yannick Schuermann on 07.07.24. (für testen)
//
#include "file_processing/processing_unit.h"
#include <iostream>

int main() {
    ProcessingUnit test("csv/matrix_multiplication_trace.csv");

    size_t numRequests;
    Request* requests;

    test.getRequests(numRequests, requests);

    // Use the requests array
    for (size_t i = 0; i < numRequests; ++i) {
        std::cout << "Request " << i << ": addr=0x" << std::hex << requests[i].addr
                  << ", data=" << std::dec << requests[i].data
                  << ", we=" << requests[i].we << std::endl;
    }

    //delete the allocated memory
    delete[] requests;

    return 0;
}