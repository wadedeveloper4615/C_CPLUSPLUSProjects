//============================================================================
// Name        : matrix.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "matrix.h"
#include "array.h"

Matrix::Matrix() {
    elements = NULL;
    rows = 0;
    cols = 0;
    size = 0;
}

Matrix::Matrix(int rows, int cols) {
    allocate(rows, cols);
}

Matrix::Matrix(int rows, int cols, NetworkType value) {
    allocate(rows, cols, value);
}

Matrix::Matrix(initializer_list<initializer_list<NetworkType>> list) {
    size = sizeof(elements);
    rows = list.size();
    cols = (list.begin())->size();
    elements = new NetworkTypePtr[rows];
    size += rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++) {
            elements[i][j] = ((list.begin() + i)->begin())[j];
        }
    }
}

Matrix::Matrix(const Array &v1, const Array &v2) {
    rows = v1.length;
    cols = v2.length;
    size = sizeof(NetworkTypePtrPtr);
    elements = new NetworkTypePtr[rows];
    size = rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++)
            elements[i][j] = v1.elements[i] * v2.elements[j];
    }
}

Matrix::Matrix(Array &v1, Array &v2) {
    rows = v1.length;
    cols = v2.length;
    size = sizeof(NetworkTypePtrPtr);
    elements = new NetworkTypePtr[rows];
    size = rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++)
            elements[i][j] = v1.elements[i] * v2.elements[j];
    }
}

Matrix::Matrix(const Matrix &list) {
    rows = list.rows;
    cols = list.cols;
    size = sizeof(NetworkTypePtrPtr);
    elements = new NetworkTypePtr[rows];
    size += rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++) {
            elements[i][j] = list[i][j];
        }
    }
}

Matrix::~Matrix() {
    deallocate();
}

NetworkTypePtr& Matrix::operator[](int index) const {
    return elements[index];
}

Matrix& Matrix::operator=(const Matrix &m1) {
    deallocate();
    allocate(m1.rows, m1.cols);
    size = m1.rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < m1.rows; i++) {
        elements[i] = new NetworkType[m1.cols];
        size += m1.cols * sizeof(NetworkType);
        for (auto j = 0; j < m1.cols; j++) {
            elements[i][j] = m1[i][j];
        }
    }
    return *this;
}

Matrix& Matrix::operator+(const Matrix &m1) {
    Matrix *sum = new Matrix(m1.rows, m1.cols);
    for (auto i = 0; i < m1.rows; i++) {
        for (auto j = 0; j < m1.cols; j++) {
            (*sum)[i][j] = m1[i][j] + elements[i][j];
        }
    }
    return *sum;
}

Matrix& Matrix::operator-(const Matrix &m1) {
    Matrix *sum = new Matrix(m1.rows, m1.cols);
    for (auto i = 0; i < m1.rows; i++) {
        for (auto j = 0; j < m1.cols; j++) {
            (*sum)[i][j] = m1[i][j] - elements[i][j];
        }
    }
    return *sum;
}

Matrix& Matrix::operator+=(const Matrix &m1) {
    for (auto i = 0; i < rows && i < m1.rows; i++) {
        for (auto j = 0; j < cols && j < m1.cols; j++) {
            elements[i][j] += m1[i][j];
        }
    }
    return *this;
}

istream& operator>>(istream &s, Matrix &v) {
    int row, column;
    s >> row;
    s >> column;
    v.deallocate();
    v.allocate(row, column);
    for (auto i = 0; i < v.getRows(); i++) {
        for (auto j = 0; j < v.getCols(); j++) {
            s >> v[i][j];
        }
    }
    return s;
}

ostream& operator<<(ostream &s, Matrix &v) {
    s << v.getRows();
    s << v.getCols();
    for (auto i = 0; i < v.getRows(); i++) {
        for (auto j = 0; j < v.getCols(); j++) {
            s << v[i][j];
        }
    }
    return s;
}

void Matrix::allocate(int rows, int cols, double low, double high) {
    this->rows = rows;
    this->cols = cols;
    size = sizeof(elements);
    elements = new NetworkTypePtr[rows];
    size += rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++) {
            elements[i][j] = fRand(low, high);
        }
    }
}

void Matrix::allocate(int rows, int cols, NetworkType value) {
    this->rows = rows;
    this->cols = cols;
    size = sizeof(elements);
    elements = new NetworkTypePtr[rows];
    size += rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
        for (auto j = 0; j < cols; j++) {
            elements[i][j] = value;
        }
    }
}

void Matrix::allocate(int rows, int cols) {
    this->rows = rows;
    this->cols = cols;
    size = sizeof(elements);
    elements = new NetworkTypePtr[rows];
    size += rows * sizeof(NetworkTypePtr);
    for (auto i = 0; i < rows; i++) {
        elements[i] = new NetworkType[cols];
        size += cols * sizeof(NetworkType);
    }
}

void Matrix::deallocate() {
    for (auto i = 0; i < rows; i++) {
        delete[] elements[i];
    }
    delete[] elements;
    elements = NULL;
}

long Matrix::getSize() {
    return size;
}

long Matrix::getRows() {
    return rows;
}

void Matrix::setRows(long rows) {
    this->rows = rows;
}

long Matrix::getCols() {
    return cols;
}

void Matrix::setCols(long cols) {
    this->cols = cols;
}

NetworkTypePtrPtr Matrix::getElements() {
    return elements;
}

int Matrix::depth() {
    return rows;
}
int Matrix::width() {
    return cols;
}

Array Matrix::colslice(int col) {
    Array temp(rows);
    for (int i = 0; i < rows; i++)
        temp.elements[i] = elements[i][col];
    return temp;
}

Array Matrix::rowslice(int row) {
    Array temp(cols);
    for (int i = 0; i < cols; i++)
        temp.elements[i] = elements[row][i];
    return temp;
}
