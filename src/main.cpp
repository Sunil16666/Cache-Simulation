//
// Created by Yannick Schuermann on 07.07.24. (für testen)
//

////test Processing unit
//#include "file_processing/processing_unit.h"
//#include <iostream>

//int main() {
//    ProcessingUnit test("csv/matrix_multiplication_trace.csv");
//
//    size_t numRequests;
//    Request* requests;
//
//    test.getRequests(numRequests, requests);
//
//    // Use the requests array
//    for (size_t i = 0; i < numRequests; ++i) {
//        std::cout << "Request " << i << ": addr=0x" << std::hex << requests[i].addr
//                  << ", data=" << std::dec << requests[i].data
//                  << ", we=" << requests[i].we << std::endl;
//    }
//
//    //delete the allocated memory
//    delete[] requests;

//    return 0;
//}

#include "data_generation/matrix_multiplication.h"
#include <iostream>

int main() {
    int A_elements[] = {1, 2, 3, 4};
    int B_elements[] = {4, 5, 6, 7};
    int n = 2;

    MatrixMultiplication test(A_elements, B_elements, n);
    test.generate();

    // Print the result matrix C
    std::cout << "Matrix C:" << std::endl;
    test.printMatrix(test.getC(), n);

    return 0;
}
