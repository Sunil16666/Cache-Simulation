#include "../data_generation/matrix_multiplication.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    int A_elements[] = {1, 2, 3, 4};
    int B_elements[] = {4, 5, 6, 7};
    int n = 2;

    MatrixMultiplication* test = createMatrixMultiplication(A_elements, B_elements, n, "../csv/matrix_multiplication_trace.csv");
    generate(test);

    printf("Matrix C:\n");
    printMatrix(getC(test), n);

    deleteMatrixMultiplication(test);

    return 0;
}
