#include "matrix_multiplication.h"

MatrixMultiplication* createMatrixMultiplication(int* A_elements, int* B_elements, int n, const char* path) {
    MatrixMultiplication* matrixMult = (MatrixMultiplication*)malloc(sizeof(MatrixMultiplication));
    matrixMult->A = makeMatrix(A_elements, n);
    matrixMult->B = makeMatrix(B_elements, n);
    matrixMult->C = makeMatrix(NULL, n);
    matrixMult->n = n;
    matrixMult->path = strdup(path);
    return matrixMult;
}

void deleteMatrixMultiplication(MatrixMultiplication* matrixMult) {
    deleteMatrix(matrixMult->A, matrixMult->n);
    deleteMatrix(matrixMult->B, matrixMult->n);
    deleteMatrix(matrixMult->C, matrixMult->n);
    free(matrixMult->path);
    free(matrixMult);
}

int** makeMatrix(int* elements, int n) {
    int** matrix = (int**)malloc(n * sizeof(int*));
    for (int i = 0; i < n; ++i) {
        matrix[i] = (int*)malloc(n * sizeof(int));
        for (int j = 0; j < n; ++j) {
            matrix[i][j] = (elements) ? elements[i * n + j] : 0;
        }
    }
    return matrix;
}

void deleteMatrix(int** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        free(matrix[i]);
    }
    free(matrix);
}

void generate(const MatrixMultiplication* matrixMult) {
    if (!matrixMult->A || !matrixMult->B) {
        fprintf(stderr, "Input matrices cannot be null\n");
        return;
    }

    FILE* file = fopen(matrixMult->path, "w");
    if (!file) {
        fprintf(stderr, "Unable to open file\n");
        return;
    }

    fprintf(file, "Type,Address,Value\n");

    int addressBase = 0x1000;
    int addressCounter = 0;

    for (int i = 0; i < matrixMult->n; ++i) {
        for (int j = 0; j < matrixMult->n; ++j) {
            fprintf(file, "W,0x%x,0\n", addressBase + addressCounter * 4);
            matrixMult->C[i][j] = 0;
            for (int k = 0; k < matrixMult->n; ++k) {
                fprintf(file, "R,0x%x,%d\n", addressBase + (i * matrixMult->n + k) * 4, matrixMult->A[i][k]);
                fprintf(file, "R,0x%x,%d\n", addressBase + (k * matrixMult->n + j) * 4, matrixMult->B[k][j]);
                matrixMult->C[i][j] += matrixMult->A[i][k] * matrixMult->B[k][j];
                fprintf(file, "W,0x%x,%d\n", addressBase + addressCounter * 4, matrixMult->C[i][j]);
            }
            addressCounter++;
        }
    }

    fclose(file);
}

void printMatrix(int** matrix, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int** getA(const MatrixMultiplication* matrixMult) {
    return matrixMult->A;
}

int** getB(const MatrixMultiplication* matrixMult) {
    return matrixMult->B;
}

int** getC(const MatrixMultiplication* matrixMult) {
    return matrixMult->C;
}