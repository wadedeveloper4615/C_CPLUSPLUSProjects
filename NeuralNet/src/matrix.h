//============================================================================
// Name        : nn.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef MATRIX_H_
#define MATRIX_H_

#include "util.h"

class ArrayPair;
class Array;

class Matrix {
        NetworkTypePtrPtr elements;
        int rows;
        int cols;
        long size;
    public:
        Matrix();
        Matrix(int rows, int cols);
        Matrix(int rows, int cols, NetworkType value);
        Matrix(initializer_list<initializer_list<NetworkType>> list);
        Matrix(const Matrix &v1);
        Matrix(const Array &v1, const Array &v2);
        Matrix(Array &v1, Array &v2);
        Matrix(const ArrayPair &vp);
        virtual ~Matrix();
    public:
        Matrix& operator=(const Matrix &m1);
        Matrix& operator+(const Matrix &m1);
        Matrix& operator-(const Matrix &m1);
        Matrix& operator+=(const Matrix &m1);
        NetworkTypePtr& operator[](int index) const;
        friend istream& operator>>(istream &s, Matrix &v);
        friend ostream& operator<<(ostream &s, Matrix &v);
    public:
        void deallocate();
        void allocate(int rows, int cols, double low, double high);
        void allocate(int rows, int cols, NetworkType value);
        void allocate(int rows, int cols);
        long getSize();
        long getRows();
        void setRows(long rows);
        long getCols();
        void setCols(long cols);
        NetworkTypePtrPtr getElements();
        int depth();
        int width();
        Array colslice(int col);
        Array rowslice(int row);
};
typedef Matrix *MatrixPtr;
typedef Matrix **MatrixPtrPtr;

#endif /* MATRIX_H_ */
