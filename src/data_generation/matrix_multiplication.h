#ifndef MATRIX_MULTIPLICATION_H
#define MATRIX_MULTIPLICATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int** A;
    int** B;
    int** C;
    int n;
    char* path;
} MatrixMultiplication;

MatrixMultiplication* createMatrixMultiplication(int* A_elements, int* B_elements, int n, const char* path);
void deleteMatrixMultiplication(MatrixMultiplication* matrixMult);
int** makeMatrix(int* elements, int n);
void deleteMatrix(int** matrix, int n);
void generate(const MatrixMultiplication* matrixMult);
void printMatrix(int** matrix, int n);
int** getA(const MatrixMultiplication* matrixMult);
int** getB(const MatrixMultiplication* matrixMult);
int** getC(const MatrixMultiplication* matrixMult);

#endif // MATRIX_MULTIPLICATION_H
