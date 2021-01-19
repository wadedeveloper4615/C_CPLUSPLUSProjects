//============================================================================
// Name        : bammatrix.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef BAMMATRIX_H_
#define BAMMATRIX_H_

#include "array.h"
#include "matrix.h"

class BAMMatrix: public Matrix {
    private:
        int K;
        Array *C;
        int feedthru(Array &A, Array &B);
        int sigmoid(int n);
    public:
        BAMMatrix();
        BAMMatrix(int rows, int cols);
        ~BAMMatrix();
        void encode(ArrayPair &AB);
        void uncode(ArrayPair &AB);
        ArrayPair recall(Array &A);
        int check();
        int check(const ArrayPair &AB);
        int energy(const Matrix &m1);
};
// BAM matrix

#endif /* BAMMATRIX_H_ */
