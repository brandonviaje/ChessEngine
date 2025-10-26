#ifndef MASKS_H
#define MASKS_H

typedef unsigned long long U64;
typedef enum {
    P, N, B, R, Q, K,   // white
    p, n, b, r, q, k    // black
} Pieces;

// Rank masks
static const U64 rank1 = 0x00000000000000FFULL;
static const U64 rank2 = 0x000000000000FF00ULL;
static const U64 rank3 = 0x0000000000FF0000ULL;
static const U64 rank4 = 0x00000000FF000000ULL;
static const U64 rank5 = 0x000000FF00000000ULL;
static const U64 rank6 = 0x0000FF0000000000ULL;
static const U64 rank7 = 0x00FF000000000000ULL;
static const U64 rank8 = 0xFF00000000000000ULL;

// File masks
static const U64 fileA = 0x0101010101010101ULL; 
static const U64 fileB = 0x0202020202020202ULL; 
static const U64 fileC = 0x0404040404040404ULL; 
static const U64 fileD = 0x0808080808080808ULL; 
static const U64 fileE = 0x1010101010101010ULL; 
static const U64 fileF = 0x2020202020202020ULL; 
static const U64 fileG = 0x4040404040404040ULL; 
static const U64 fileH = 0x8080808080808080ULL;

#endif