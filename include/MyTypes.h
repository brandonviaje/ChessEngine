#ifndef MYTYPES_H
#define MYTYPES_H

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

// FEN Notation
#define starting_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
#define kiwipete "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1"
#define position3 "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"
#define position4 "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"
#define position5 "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"
#define position6 "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"

// Color & Move Types
#define MAX_MOVES 256
#define FLAG_NONE 0
#define FLAG_ENPASSANT 1
#define FLAG_CASTLE_KINGSIDE 2
#define FLAG_CASTLE_QUEENSIDE 3
#define FLAG_PROMOTION 4
#define FLAG_DOUBLE_PUSH 5
#define WHITE 0
#define BLACK 1
#define WHITE_CASTLE_K 1
#define WHITE_CASTLE_Q 2
#define BLACK_CASTLE_K 4
#define BLACK_CASTLE_Q 8

// Macros
#define SetBit(board, square) ((board) |= (1ULL << (square)))
#define GetBit(board, square) ((board) & (1ULL << (square)))
#define PopBit(board, square) ((board) &= ~(1ULL << (square)))

// Typedef
typedef unsigned long long U64;

typedef struct
{
    int piece;     // piece type of bitboard
    int from;      // square index the piece moves from
    int to;        // square index the piece moves to
    int promotion; // piece type of promotted piece
    int captured;  // piece type of captured piece
    int flags;     // handles special moves
    int prevEnpassant;
    int prevCastle;
    int prevHalfMove;
} Move;

typedef enum
{
    P,
    N,
    B,
    R,
    Q,
    K, // white
    p,
    n,
    b,
    r,
    q,
    k // black
} Pieces;

#endif
