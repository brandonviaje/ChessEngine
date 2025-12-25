#include "../include/NNUE.h"

float W1[256*768];  // Layer1 weights
float b1[256];      // Layer1 biases

float W2[32*256];   // Layer2 weights
float b2[32];       // Layer2 biases

float W3[32];       // Layer3 weights
float b3;           // output bias

// Clipped ReLU activation function
float ClippedReLU(float x, float max_val)
{
    if (x < 0.0f) return 0.0f;
    if (x > max_val) return max_val;
    return x;
}

void LoadBinary(const char* path, float* array, size_t n) 
{
    FILE* f = fopen(path, "rb");
    if (!f) { perror(path); exit(1); }
    fread(array, sizeof(float), n, f);
    fclose(f);
}

void NNUELoadWeights(const char* folder) 
{
    
    char path[256];         // load weights and biases from binary files

    snprintf(path, sizeof(path), "%s/W1.bin", folder);
    LoadBinary(path, W1, 256*768);
    
    snprintf(path, sizeof(path), "%s/b1.bin", folder);
    LoadBinary(path, b1, 256);

    snprintf(path, sizeof(path), "%s/W2.bin", folder);
    LoadBinary(path, W2, 32*256);

    snprintf(path, sizeof(path), "%s/b2.bin", folder);
    LoadBinary(path, b2, 32);

    snprintf(path, sizeof(path), "%s/W3.bin", folder);
    LoadBinary(path, W3, 32);

    snprintf(path, sizeof(path), "%s/b3.bin", folder);
    LoadBinary(path, &b3, 1);
}

// NNUE forward pass implementation using pretrained weights on pytorch
float NNUEForward(const float* x) 
{

    // hidden layers and output
    float h1[256];
    float h2[32];
    float out;

    // Layer1: 768 -> 256
    for (int i = 0; i < 256; i++) 
    {
        float sum = b1[i];
        for (int j = 0; j < 768; j++) 
        {
            sum += W1[i*768 + j] * x[j];
        }
        h1[i] = ClippedReLU(sum, 127.0f);
    }

    // Layer2: 256 -> 32
    for (int i = 0; i < 32; i++) 
    {
        float sum = b2[i];
        for (int j = 0; j < 256; j++) 
        {
            sum += W2[i*256 + j] * h1[j];
        }
        h2[i] = ClippedReLU(sum, 127.0f);
    }

    // Layer3: 32 -> 1
    out = b3;
    for (int j = 0; j < 32; j++) 
    {
        out += W3[j] * h2[j];
    }

    return out;
}
