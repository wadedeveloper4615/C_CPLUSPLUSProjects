//============================================================================
// Name        : util.cpp
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#include "util.h"

NetworkType fRand(NetworkType fMin, NetworkType fMax) {
    NetworkType f = (NetworkType) rand() / (NetworkType) RAND_MAX;
    return fMin + f * (fMax - fMin);
}
