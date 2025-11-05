#ifndef MYTYPES_H
#define MYTYPES_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

// FEN Notation
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define attack_position "rnbqkbnr/pp1p1ppp/8/2p1p3/3P4/8/PPP1PPPP/RNBQKBNR w KQkq - 0 1"
#define knight_attack_position "rnbqkbnr/1ppppppp/8/8/8/p7/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define bishop_attack_position "rnbqkbnr/1ppp1ppp/8/8/1P1P4/p3p3/P1P1PPPP/RNBQKBNR w KQkq - 0 1"
#define rook_attack_position "8/8/8/3pP3/3R4/3N1B2/8/8 w - - 0 1"
#define queen_attack_position "8/8/3p4/2nP4/3Q4/3N1B2/8/8 w - - 0 1"
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R b KQkq - 0 1"

// Typedef
typedef unsigned long long U64;

typedef enum {
    P, N, B, R, Q, K,   // white
    p, n, b, r, q, k    // black
} Pieces;

typedef struct {
    int from;    // square index the piece moves from
    int to;      // square index the piece moves to
    int flags;   // bitmask for promotions, en passant, castling, etc.
} Move;

#endif