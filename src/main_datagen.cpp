//
// Created by Yannick Schuermann on 08.07.24.
//
#include "data_gen/matrix_multiplication.h"
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