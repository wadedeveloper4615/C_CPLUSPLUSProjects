//============================================================================
// Name        : net.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "net.h"
#include "matrix.h"

NeuralNetwork::NeuralNetwork(const int numPatterns, MatrixPtr in, MatrixPtr out, int config[], int size) {
    eta = 0.5;
    alpha = 0.9;
    Error = 0;
    epoch = 0;
    settings = new Settings;
    settings->NumPattern = numPatterns;
    settings->numberOfLayers = size;
    settings->numberOfWeights = settings->numberOfLayers - 1;
    settings->outputLayerIndex = settings->numberOfLayers - 1;
    settings->trainingSetOrder = new int[settings->NumPattern + 1];

    settings->configuration.allocate(settings->numberOfLayers, 0);
    for (auto i = 0; i < settings->numberOfLayers; i++) {
        settings->configuration[i] = config[i];
    }

    settings->trainingOutput.allocate(settings->NumPattern, settings->configuration[settings->outputLayerIndex], 0);
    for (auto p = 0; p < settings->NumPattern; p++) {
        for (auto j = 0; j < settings->configuration[settings->outputLayerIndex]; j++) {
            settings->trainingOutput[p][j] = (*out)[p][j];
        }
    }

    settings->trainingInput.allocate(settings->NumPattern, settings->configuration[0], 0);
    for (auto p = 0; p < settings->NumPattern; p++) {
        for (auto j = 0; j < settings->configuration[0]; j++) {
            settings->trainingInput[p][j] = (*in)[p][j];
        }
    }

    Expected.allocate(settings->NumPattern + 1, settings->configuration[settings->outputLayerIndex] + 1, 0);
    memory += Expected.getSize();

    Layers = new Matrix[settings->numberOfLayers];
    for (auto i = 0; i < settings->numberOfLayers; i++) {
        Layers[i].allocate(settings->NumPattern + 1, settings->configuration[i] + 1, 0);
        memory += Layers[i].getSize();
    }

    Weights = new Matrix[settings->numberOfWeights];
    for (auto i = 0; i < settings->numberOfWeights; i++) {
        Weights[i].allocate(settings->configuration[i] + 1, settings->configuration[i + 1] + 1, -1.0, 1.0);
        memory += Weights[i].getSize();
    }

    Delta = new Array[settings->numberOfWeights];
    for (auto i = 0; i < settings->numberOfWeights; i++) {
        Delta[i].allocate(settings->configuration[i + 1] + 1, 0);
        memory += Delta[i].getSize();
    }

    DeltaWeight = new Matrix[settings->numberOfWeights];
    for (auto i = 0; i < settings->numberOfWeights; i++) {
        DeltaWeight[i].allocate(settings->configuration[i] + 1, settings->configuration[i + 1] + 1, 0);
        memory += DeltaWeight[i].getSize();
    }

    outputLayer = &Layers[settings->outputLayerIndex];
}

NeuralNetwork::~NeuralNetwork() {
    Expected.deallocate();

    for (auto i = 0; i < settings->numberOfLayers; i++) {
        Layers[i].deallocate();
    }
    delete[] Layers;

    for (auto i = 0; i < settings->numberOfWeights; i++) {
        Weights[i].deallocate();
    }
    delete[] Weights;

    for (auto i = 0; i < settings->numberOfWeights; i++) {
        Delta[i].deallocate();
    }
    delete[] Delta;

    for (auto i = 0; i < settings->numberOfWeights; i++) {
        DeltaWeight[i].deallocate();
    }
    delete[] DeltaWeight;

    settings->trainingInput.deallocate();
    settings->trainingOutput.deallocate();
}

double NeuralNetwork::sigmoid(double x) {
    return 1 / (1 + exp(-x));
}

double NeuralNetwork::sigmoidDerivative(double x) {
    return x * (1 - x);
}

void NeuralNetwork::randomizeInput() {
    for (auto p = 1; p <= settings->NumPattern; p++) {
        settings->trainingSetOrder[p] = p;
    }
    for (auto p = 1; p <= settings->NumPattern; p++) {
        int np = p + ((NetworkType) rand() / ((NetworkType) RAND_MAX + 1)) * (settings->NumPattern + 1 - p);
        int op = settings->trainingSetOrder[p];
        settings->trainingSetOrder[p] = settings->trainingSetOrder[np];
        settings->trainingSetOrder[np] = op;
    }
}

void NeuralNetwork::output() {
    printf("\nNETWORK DATA - EPOCH %d\tError = %2.9Lf\n\nPat\t", epoch, Error);
    for (auto i = 1; i <= settings->configuration[0]; i++) {
        printf("Input%-4d\t", i);
    }
    for (auto k = 1; k <= settings->configuration[settings->outputLayerIndex]; k++) {
        printf("Target%-4d\tOutput%-4d\t", k, k);
    }
    for (auto p = 1; p <= settings->NumPattern; p++) {
        printf("\n%d\t", p);
        for (auto i = 1; i <= settings->configuration[0]; i++) {
            printf("%2.9Lf\t", Layers[0][p][i]);
        }
        for (auto k = 1; k <= settings->configuration[settings->outputLayerIndex]; k++) {
            printf("%2.9Lf\t%2.9Lf\t", Expected[p][k], (*outputLayer)[p][k]);
        }
    }
    double t = ((double) chrono::duration_cast<chrono::microseconds>(tend - tbegin).count()) / 1000000.0;
    printf("\n\nTraining Elapsed Time = %6.4f seconds\n", t);
    printf("Training Memory Used = %6.2f kilobytes\n", memory / 1024.0);
    printf("Network Layers Used = %d\n", settings->numberOfLayers);
    printf("Training Examples Used = %d\n", settings->NumPattern);
    long numberOfNeurons = 0;
    for (auto k = 0; k < settings->numberOfLayers; k++) {
        numberOfNeurons += settings->configuration[k];
    }
    long numberOfConnections = 0;
    for (auto k = 0; k < settings->numberOfWeights; k++) {
        numberOfConnections += Weights[k].getRows() * Weights[k].getCols();
    }
    double totalConnections = ((double) (numberOfConnections * epoch * settings->NumPattern));
    double connPerSec = totalConnections / t;
    printf("Total connection Used = %6.1f million\n", totalConnections / 1000000.0);
    printf("Neurons Used = %ld\n", numberOfNeurons);
    printf("Neuron Connections Used = %ld\n", numberOfConnections);
    printf("Connections Speed = %6.2f million connections/second\n", connPerSec / 1000000.0);
}

void NeuralNetwork::forward(int p) {
    for (auto k = 1; k <= settings->numberOfLayers - 1; k++) {
        for (auto j = 1; j <= settings->configuration[k]; j++) {
            NetworkType accumulate = Weights[k - 1][0][j];
            for (auto i = 1; i <= settings->configuration[k - 1]; i++) {
                accumulate += Layers[k - 1][p][i] * Weights[k - 1][i][j];
            }
            Layers[k][p][j] = sigmoid(accumulate);
        }
    }
}

void NeuralNetwork::computeError(int p) {
    for (auto i = 1; i <= settings->configuration[settings->outputLayerIndex]; i++) {
        NetworkType diff = (Expected[p][i] - (*outputLayer)[p][i]);
        Error += 0.5 * diff * diff;
        Delta[settings->outputLayerIndex - 1][i] = diff * sigmoidDerivative((*outputLayer)[p][i]);
    }
    for (auto k = settings->numberOfLayers - 3; k >= 0; k--) {
        for (auto j = 1; j <= settings->configuration[k + 1]; j++) {
            NetworkType accumulate = 0.0;
            for (auto i = 1; i <= settings->configuration[k + 2]; i++) {
                accumulate += Weights[k + 1][j][i] * Delta[k + 1][i];
            }
            Delta[k][j] = accumulate * sigmoidDerivative(Layers[k + 1][p][j]);
        }
    }
}

void NeuralNetwork::backPropagate(int p) {
    for (auto k = settings->numberOfLayers - 2; k >= 0; k--) {
        for (auto j = 1; j <= settings->configuration[k + 1]; j++) {
            DeltaWeight[k][0][j] = eta * Delta[k][j] + alpha * DeltaWeight[k][0][j];
            Weights[k][0][j] += DeltaWeight[k][0][j];
            for (auto i = 1; i <= settings->configuration[k]; i++) {
                DeltaWeight[k][i][j] = eta * Layers[k][p][i] * Delta[k][j] + alpha * DeltaWeight[k][i][j];
                Weights[k][i][j] += DeltaWeight[k][i][j];
            }
        }
    }
}

void NeuralNetwork::train(int epochLimit) {
    tbegin = chrono::steady_clock::now();
    for (auto p = 0; p < settings->NumPattern; p++) {
        for (auto j = 0; j < settings->configuration[settings->outputLayerIndex]; j++) {
            Expected[p + 1][j + 1] = settings->trainingOutput[p][j];
        }
    }
    for (auto p = 0; p < settings->NumPattern; p++) {
        for (auto j = 0; j < settings->configuration[0]; j++) {
            Layers[0][p + 1][j + 1] = settings->trainingInput[p][j];
        }
    }
    for (epoch = 0; epoch < epochLimit; epoch++) {
        randomizeInput();
        Error = 0.0;
        for (auto np = 1; np <= settings->NumPattern; np++) {
            auto p = settings->trainingSetOrder[np];
            forward(p);
            computeError(p);
            backPropagate(p);
        }
        if (epoch % 100 == 0)
            printf("\nEpoch %-5d :   Error = %2.9Lf", epoch, Error);
        if (Error < 0.0004)
            break;
    }
    tend = chrono::steady_clock::now();
}
