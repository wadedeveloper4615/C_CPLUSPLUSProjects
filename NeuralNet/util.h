//============================================================================
// Name        : util.h
// Author      : Christopher D. Wade
// Version     : 1.0
// Copyright   : (c) 2020 Christopher D. Wade
// Description : Multihidden layer backpropagation net
//============================================================================
#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <initializer_list>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
using namespace std;

#define max(a,b)   (((a) > (b)) ? (a) : (b))
#define min(a,b)   (((a) < (b)) ? (a) : (b))
#define printf __mingw_printf

typedef long double NetworkType;
typedef NetworkType *NetworkTypePtr;
typedef NetworkTypePtr *NetworkTypePtrPtr;

NetworkType fRand(NetworkType fMin, NetworkType fMax);

#endif /* UTIL_H_ */
