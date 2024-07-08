//
// Created by Yannick Schuermann on 07.07.24.
//

#ifndef MATRIX_MULTIPLICATION_H
#define MATRIX_MULTIPLICATION_H

#include <string>

class MatrixMultiplication {
public:
    MatrixMultiplication(int* A_elements, int* B_elements, int n);
    ~MatrixMultiplication();
    void generate() const;
    int** makeMatrix(int* elements, int n) const;
    void printMatrix(int** matrix, int n) const;
    int** getA() const;
    int** getB() const;
    int** getC() const;

private:
    std::string path;
    int** A;
    int** B;
    mutable int** C;
    int n;
    void deleteMatrix(int** matrix, int n) const;
};

#endif // MATRIX_MULTIPLICATION_H