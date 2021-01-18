//============================================================================
// Name        : array.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef ARRAY_H_
#define ARRAY_H_

#include "util.h"
#include "matrix.h"
#include "bamMatrix.h"

class Array {
        friend class Matrix;
        friend class BAMMatrix;
        NetworkTypePtr elements;
        int length;
        long size;
    public:
        Array();
        Array(int length);
        Array(int length, NetworkType value);
        Array(Array &v1);
        Array(initializer_list<NetworkType> list);
        virtual ~Array();
    public:
        Array& operator=(const Array &v1);
        Array& operator+(const Array &v1);
        Array& operator-(const Array &v1);
        int operator*(const Array &v1);
        Array& operator*(int c);
        Array& operator/(const Array &v1);
        Array& operator+=(const Array &v1);
        Array& operator*=(int i);
        int operator==(const Array &v1);
        NetworkType& operator[](int index) const;
        friend istream& operator>>(istream &s, Array &v);
        friend ostream& operator<<(ostream &s, Array &v);
    public:
        void deallocate();
        void allocate(int size);
        void allocate(int size, NetworkType value);
        long getSize();
        void setSize(long size);
        void setElements(NetworkTypePtr elements);
        void setElement(int index, NetworkType element);
        NetworkType getElement(int index);
        NetworkTypePtr getElement();
        long getLength();
        void setLength(long number);
};
typedef Array *ArrayPtr;
typedef Array **ArrayPtrPtr;

#endif /* ARRAY_H_ */
