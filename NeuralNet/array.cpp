//============================================================================
// Name        : array.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "array.h"
#include "matrix.h"

Array::Array() {
    elements = NULL;
    length = 0;
    size = 0;
}

Array::Array(int length) {
    allocate(length);
}

Array::Array(int length, NetworkType value) {
    allocate(length, value);
}

Array::Array(Array &v1) {
    size = 0;
    length = v1.getLength();
    elements = new NetworkType[length];
    for (auto i = 0; i < length; i++) {
        elements[i] = v1[i];
    }
}

Array::Array(initializer_list<NetworkType> list) {
    size = 0;
    length = list.size();
    elements = new NetworkType[length];
    for (auto i = 0; i < length; i++) {
        elements[i] = list.begin()[i];
    }
}

Array::~Array() {
    deallocate();
}

Array& Array::operator=(const Array &v1) {
    deallocate();
    length = v1.length;
    elements = new NetworkType[length];
    for (auto i = 0; i < length; i++) {
        elements[i] = v1.elements[i];
    }
    return *this;
}

Array& Array::operator+(const Array &v1) {
    Array *sum = new Array(v1.length, 0);
    for (auto i = 0; i < v1.length; i++) {
        (*sum)[i] = elements[i] + v1[i];
    }
    return *sum;
}

Array& Array::operator-(const Array &v1) {
    Array *sum = new Array(v1.length, 0);
    for (auto i = 0; i < length; i++) {
        (*sum)[i] = elements[i] - v1[i];
    }
    return *sum;
}

Array& Array::operator/(const Array &v1) {
    Array *sum = new Array(v1.length, 0);
    for (auto i = 0; i < length; i++) {
        (*sum)[i] = elements[i] / v1[i];
    }
    return *sum;
}

Array& Array::operator+=(const Array &v1) {
    for (auto i = 0; i < length; i++) {
        (*this)[i] += v1.elements[i];
    }
    return *this;
}

Array& Array::operator*=(int i) {
    for (auto j = 0; j < length; j++) {
        (*this)[j] *= i;
    }
    return *this;
}

int Array::operator*(const Array &v1) {
    int sum = 0;
    for (int i = 0; i < min(length, v1.length); i++)
        sum += (v1[i] * elements[i]);
    return sum;
}

Array& Array::operator*(int c) {
    ArrayPtr sum = new Array(getLength(), 0);
    for (auto i = 0; i < length; i++) {
        (*sum)[i] = getElement(i) * c;
    }
    return *sum;
}

// vector transpose multiply needs access to v array
int Array::operator==(const Array &v1) {
    if (v1.length != length)
        return 0;
    for (int i = 0; i < min(length, v1.length); i++) {
        if (v1[i] != elements[i]) {
            return 0;
        }
    }
    return 1;
}

NetworkType& Array::operator[](int index) const {
    return elements[index];
}

void Array::allocate(int size, NetworkType value) {
    elements = new NetworkType[size];
    long memory = size * sizeof(NetworkType);
    length = size;
    for (auto j = 0; j < size; j++) {
        elements[j] = value;
    }
    size = memory;
}

void Array::allocate(int size) {
    elements = new NetworkType[size];
    long memory = size * sizeof(NetworkType);
    length = size;
    size = memory;
}

long Array::getSize() {
    return size;
}

void Array::setSize(long size) {
    this->size = size;
}

void Array::deallocate() {
    delete[] elements;
    elements = NULL;
}

void Array::setElements(NetworkTypePtr elements) {
    this->elements = elements;
}

NetworkTypePtr Array::getElement() {
    return elements;
}

NetworkType Array::getElement(int index) {
    return elements[index];
}

long Array::getLength() {
    return length;
}

void Array::setLength(long length) {
    this->length = length;
}

void Array::setElement(int index, NetworkType element) {
    this->elements[index] = element;
}

istream& operator>>(istream &s, Array &v) {
    long length;
    s >> length;
    v.deallocate();
    v.allocate(length, 0);
    for (auto i = 0; i < v.getLength(); i++) {
        s >> v[i];
    }
    return s;
}

ostream& operator<<(ostream &s, Array &v) {
    s << v.getLength();
    for (auto i = 0; i < v.getLength(); i++) {
        s << v[i];
    }
    return s;
}

