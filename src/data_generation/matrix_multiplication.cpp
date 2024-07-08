#include "matrix_multiplication.h"
#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <iostream>
#include <string>

//constructor
MatrixMultiplication::MatrixMultiplication(int* A_elements, int* B_elements, int n)
    : path("csv/matrix_multiplication_trace.csv"), n(n) {
    //makematrix a b and c
    A = makeMatrix(A_elements, n);
    B = makeMatrix(B_elements, n);
    //c is intialized empty, since it is the output matrix (this may not be necessary in the constructor)
    C = makeMatrix(nullptr, n);
}

//deconstructor to free up space after tunning the function
MatrixMultiplication::~MatrixMultiplication() {
    deleteMatrix(A, n);
    deleteMatrix(B, n);
    deleteMatrix(C, n);
}

//make a nxn matrix, if elements are passed, insert them
int** MatrixMultiplication::makeMatrix(int* elements, int n) const {
    int** matrix = new int*[n];
    for (int i = 0; i < n; ++i) {
        matrix[i] = new int[n];
        for (int j = 0; j < n; ++j) {
            if (elements) {
                matrix[i][j] = elements[i * n + j];
            } else {
                matrix[i][j] = 0;
            }
        }
    }
    return matrix;
}

//delete the matrix
void MatrixMultiplication::deleteMatrix(int** matrix, int n) const {
    for (int i = 0; i < n; ++i) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

//main generate function
void MatrixMultiplication::generate() const {
//null check
    if (!A || !B) {
        throw std::invalid_argument("Input matrices cannot be null");
    }

//ofstream = ouput file stream, similiar to processing unit implementation
    std::ofstream file(path);

//obvious error case
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file");
    }

//push header
    file << "Type,Address,Value\n";

//initialize base address and adress counter
    int addressBase = 0x1000;
    int addressCounter = 0;

//do the matrix multiplication with logging
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            //write result initialization
            file << "W,0x" << std::hex << (addressBase + addressCounter * 4) << ",0\n";
            C[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                //read from first matrix
                file << "R,0x" << std::hex << (addressBase + (i * n + k) * 4) << "," << A[i][k] << "\n";
                //read from second matrix
                file << "R,0x" << std::hex << (addressBase + (k * n + j) * 4) << "," << B[k][j] << "\n";
                //perform the multiplication and accumulate the result
                C[i][j] += A[i][k] * B[k][j];
                //write intermediate result
                file << "W,0x" << std::hex << (addressBase + addressCounter * 4) << "," << std::dec << C[i][j] << "\n";
            }
            addressCounter++;
        }
    }
    file.close();
}

//toString method basically
void MatrixMultiplication::printMatrix(int** matrix, int n) const {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::cout << matrix[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

//getters
int** MatrixMultiplication::getA() const {
    return A;
}

int** MatrixMultiplication::getB() const {
    return B;
}

int** MatrixMultiplication::getC() const {
    return C;
}