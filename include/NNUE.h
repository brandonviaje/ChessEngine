#ifndef NNUE_H
#define NNUE_H

#include "MyTypes.h"
#include "Board.h"

float ClippedReLU(float x, float maxVal);
void LoadBinary(const char* path, float* array, size_t n);
void NNUELoadWeights(const char* folder);
float NNUEForward(const float* x);
void BitboardsToVector(U64 bitboards[12], float x[768]);

#endif
