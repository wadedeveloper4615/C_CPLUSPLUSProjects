//============================================================================
// Name        : arraypair.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef ARRAYPAIR_H_
#define ARRAYPAIR_H_

#include "array.h"
#include "matrix.h"
#include "bamMatrix.h"

class ArrayPair {
        friend class Matrix;
        friend class BAMMatrix;
        int flag;
        Array a;
        Array b;
    public:
        ArrayPair();
        ArrayPair(int rows, int cols); // constructor
        ArrayPair(const Array &A, const Array &B);
        ArrayPair(const ArrayPair &AB); // copy initializer
        ~ArrayPair();
        ArrayPair& operator=(const ArrayPair &v1);
        int operator==(const ArrayPair &v1);
        friend istream& operator>>(istream &s, ArrayPair &v);
        friend ostream& operator<<(ostream &s, ArrayPair &v);
        friend Matrix::Matrix(const ArrayPair &vp);
    public:
        Array getA();
        Array getB();
};

#endif /* ARRAYPAIR_H_ */
