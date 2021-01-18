//============================================================================
// Name        : bammatrix.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "bamMatrix.h"
#include "array.h"

BAMMatrix::BAMMatrix() : Matrix(0, 0) {
    K = 0;
    C = NULL;
}

BAMMatrix::BAMMatrix(int rows, int cols) : Matrix(rows, cols) {
    C = new Array[min(rows,cols) * 2];
    K = 0;
}

BAMMatrix::~BAMMatrix() {
}

int BAMMatrix::feedthru(Array &A, Array &B) {
    Array temp = B;
    for (auto i = 0; i < B.length(); i++) {
        int value;
        if (A.length() == width())
            value = A * rowslice(i);
        else
            value = A * colslice(i);
        int n = sigmoid(value);
        if (n)
            B.elements[i] = n;
    }
    return B == temp;
}

int BAMMatrix::sigmoid(int n) {
    if (n < 0)
        return -1;
    if (n > 0)
        return 1;
    return 0;
}

void BAMMatrix::encode(ArrayPair &AB) {
    Matrix T(AB);
    (*this) += T; // add the matrix transpose to the current matrix
    C[K] = AB;
    K++;
}

void BAMMatrix::uncode(ArrayPair &AB) {
    Array a = AB.getA(); // @suppress("Invalid arguments")
    Array b = AB.getB(); // @suppress("Invalid arguments")
    Array v = b * -1;
    Matrix T(a, v);
    *this += T;
    K--;
}

ArrayPair BAMMatrix::recall(Array &A) {
    int givenrow = (A.length() == width());
    Array B(givenrow ? depth() : width(), 1);
    for (;;) {
        feedthru(A, B);
        if (feedthru(B, A))
            break;
    }
    if (givenrow)
        return ArrayPair(B, A);
    else
        return ArrayPair(A, B);
}

int BAMMatrix::check() {
    ArrayPair AB;
    for (int i = 0; i < K; i++) {
        AB = recall(C[i].a); // @suppress("Field cannot be resolved") // @suppress("Invalid arguments")
        if (!(AB == C[i])) {
            return 0;
        }
    }
    return 1;
}

int BAMMatrix::check(const ArrayPair &AB) {
    Matrix T(AB);
    return energy(T) == -depth() * width();
}

int BAMMatrix::energy(const Matrix &m1) {
    int sum = 0;
    for (int i = 0; i < depth(); i++)
        for (int j = 0; j < width(); j++)
            sum += (m1.elements[i][j] * this->elements[i][j]);
    return -sum;
}
