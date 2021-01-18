//============================================================================
// Name        : net.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef NET_H_
#define NET_H_

#include "util.h"
#include "array.h"
#include "matrix.h"

using namespace std;

typedef struct Settings {
        Array configuration;
        int NumPattern;
        int numberOfLayers;
        int numberOfWeights;
        int *trainingSetOrder;
        int outputLayerIndex;
        Matrix trainingInput;
        Matrix trainingOutput;
} Settings, *SettingsPtr, **SettingsPtrPtr;

class NeuralNetwork {
        int epoch;
        long memory;
        long double Error;
        NetworkType eta;
        NetworkType alpha;
        chrono::steady_clock::time_point tbegin;
        chrono::steady_clock::time_point tend;
        MatrixPtr Layers;
        MatrixPtr Weights;
        Matrix Expected;
        ArrayPtr Delta;
        MatrixPtr DeltaWeight;
        MatrixPtr outputLayer;
        SettingsPtr settings;
    public:
        NeuralNetwork(const int numPatterns, MatrixPtr in, MatrixPtr out, int config[], int size);
        virtual ~NeuralNetwork();
        void train(int epochLimit);
        void output();
    protected:
        double sigmoid(double x);
        double sigmoidDerivative(double x);
        void forward(int p);
        void computeError(int p);
        void backPropagate(int p);
        void randomizeInput();
};

#endif /* NET_H_ */
