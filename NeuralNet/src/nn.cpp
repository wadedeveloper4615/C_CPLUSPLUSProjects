//============================================================================
// Name        : nn.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "matrix.h"
#include "net.h"

using namespace std;

void test1() {
    const int numPatterns = 4;
    Matrix trainingInput = { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } };
    Matrix trainingOutput = { { 0 }, { 1 }, { 1 }, { 0 } };
    int config[] = { 2, 20, 25, 1 };

    NeuralNetwork net(numPatterns, &trainingInput, &trainingOutput, config, sizeof(config) / sizeof(int));
    net.train(100000);
    net.output();
}

void test2() {
    Array a1 = { 1, 2, 3, 4, 5 };
    Array a2 = a1;
    Array a3 = { 1, 2, 3, 4, 5 };
    Array a4 = a1 + a3;
    cout << a1 << endl;
    cout << a2 << endl;
    cout << a1[3] << endl;
    cout << a4 << endl;
    a1 += a3;
    cout << a1 << endl;
    Array a5 = a3 * 3;
    cout << a5 << endl;
    int a6 = a3 * a3;
    cout << a6 << endl;
    int a7 = a3 == a3;
    cout << a7 << endl;
    int a8 = a3 == a1;
    cout << a8 << endl;
}

void test3() {
    Matrix a1 = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
    Matrix a2 = a1;
    Matrix a3 = a1 + a2;
    Matrix a4 = { { 1, 2 }, { 3, 4 }, { 5, 6 }, { 7, 8 } };
    a4 += a1;
    cout << a1 << endl;
    cout << a2 << endl;
    cout << a3 << endl;
    cout << a1[0][0] << endl;
    cout << a4 << endl;
}

int main() {
    test1();
    printf("\n\nGoodbye!\n\n");
    return 1;
}
