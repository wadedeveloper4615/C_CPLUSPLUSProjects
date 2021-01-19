//============================================================================
// Name        : arrayPair.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "arrayPair.h"

ArrayPair::ArrayPair() {
    flag = 0;
}

ArrayPair::ArrayPair(int rows, int cols) {
    flag = 0;
}

ArrayPair::ArrayPair(const Array &A, const Array &B) {
    flag = 0;
    a = A;
    b = B;
}

ArrayPair::ArrayPair(const ArrayPair &AB) {
    flag = 0;
    a = AB.a;
    b = AB.b;
}

ArrayPair::~ArrayPair() {
}

ArrayPair& ArrayPair::operator=(const ArrayPair &v1) {
    a = v1.a;
    b = v1.b;
    return *this;
}

int ArrayPair::operator==(const ArrayPair &v1) {
    return (a == v1.a) && (b == v1.b);
}

istream& operator>>(istream &s, ArrayPair &v) {
    s >> v.a >> v.b;
    return s;
}

ostream& operator<<(ostream &s, ArrayPair &v) {
    s << v.a << v.b;
    return s;
}

Array ArrayPair::getA() {
    return a;
}

Array ArrayPair::getB() {
    return b;
}
