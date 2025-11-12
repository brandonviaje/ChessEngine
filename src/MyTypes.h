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
#define make_move "rnbqkbnr/p1pppppp/8/1p6/2P5/8/8/8 w KQkq - 0 1"
#define en_passant "8/8/8/3pP3/8/8/8/8 w - d6 0 1"
#define pawn_promotion "8/4P3/8/8/8/8/4p3/8 w - - 0 1"
#define castling_position "r3k2r/8/8/8/8/8/8/R3K2R w KQkq - 0 1"
#define kiwipete "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"

// Color & Move Types
#define MAX_MOVES      256
#define FLAG_NONE        0
#define FLAG_ENPASSANT   1
#define FLAG_CASTLE_KINGSIDE  2
#define FLAG_CASTLE_QUEENSIDE 3
#define FLAG_PROMOTION   4
#define WHITE            0
#define BLACK            1

// Typedef
typedef unsigned long long U64;

typedef enum {
    P, N, B, R, Q, K,   // white
    p, n, b, r, q, k    // black
} Pieces;

typedef struct{
    int piece;
    int from;    // square index the piece moves from
    int to;      // square index the piece moves to
    int promotion;
    int captured;
    int flags; // handles special moves
} Move;

#endif