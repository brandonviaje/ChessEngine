#include "../include/Attack.h"
#include <stdio.h>
#include <stdlib.h>

// Bitboards
extern U64 bitboards[12];
extern U64 occupied;

// Precomputed Tables
U64 knightAttacks[64];
U64 kingAttacks[64];
U64 pawnAttacks[2][64]; // 0 white, 1 black

extern SMagic bishopAttacks[64];
extern SMagic rookAttacks[64];
extern int side;

// Non sliding attacks

U64 GeneratePawnAttacks(int side, int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SetBit(bitboard, square);

    if (side == WHITE)
    {
        attacks |= (bitboard << 7) & ~FILE_H; // left capture
        attacks |= (bitboard << 9) & ~FILE_A; // right capture
    }
    else
    {
        attacks |= (bitboard >> 9) & ~FILE_H; // left capture
        attacks |= (bitboard >> 7) & ~FILE_A; // right capture
    }

    return attacks;
}

U64 GenerateKnightAttacks(int square)
{
    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SetBit(bitboard, square);

    attacks |= (bitboard << 6) & ~(FILE_G | FILE_H);
    attacks |= (bitboard << 10) & ~(FILE_A | FILE_B);
    attacks |= (bitboard << 15) & ~FILE_H;
    attacks |= (bitboard << 17) & ~FILE_A;
    attacks |= (bitboard >> 6) & ~(FILE_A | FILE_B);
    attacks |= (bitboard >> 10) & ~(FILE_G | FILE_H);
    attacks |= (bitboard >> 15) & ~FILE_A;
    attacks |= (bitboard >> 17) & ~FILE_H;

    return attacks;
}

U64 GenerateKingAttacks(int square)
{

    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;
    SetBit(bitboard, square);

    attacks |= (bitboard << 8);           // N
    attacks |= (bitboard >> 8);           // S
    attacks |= (bitboard << 1) & ~FILE_A; // E
    attacks |= (bitboard >> 1) & ~FILE_H; // W
    attacks |= (bitboard << 9) & ~FILE_A; // NE
    attacks |= (bitboard << 7) & ~FILE_H; // NW
    attacks |= (bitboard >> 9) & ~FILE_H; // SW
    attacks |= (bitboard >> 7) & ~FILE_A; // SE

    return attacks;
}

// Sliding Attacks

U64 GetRookAttacks(int square, U64 blockers)
{
    SMagic *magic = &rookAttacks[square];
    U64 relevantBlockers = blockers & magic->mask;
    int index = (int)((relevantBlockers * magic->magic) >> magic->shift);
    return magic->ptr[index];
}

U64 GetBishopAttacks(int square, U64 blockers)
{
    SMagic *magic = &bishopAttacks[square];
    U64 relevantBlockers = blockers & magic->mask;
    int index = (int)((relevantBlockers * magic->magic) >> magic->shift);
    return magic->ptr[index];
}

// Initialize tables

void InitPawnAttacks()
{
    for (int square = 0; square < 64; square++)
    {
        pawnAttacks[WHITE][square] = GeneratePawnAttacks(WHITE, square);
        pawnAttacks[BLACK][square] = GeneratePawnAttacks(BLACK, square);
    }
}

void InitKnightAttacks()
{
    for (int square = 0; square < 64; square++)
        knightAttacks[square] = GenerateKnightAttacks(square);
}

void InitKingAttacks()
{
    for (int square = 0; square < 64; square++)
        kingAttacks[square] = GenerateKingAttacks(square);
}

void InitAttacks()
{
    InitPawnAttacks();
    InitKingAttacks();
    InitKnightAttacks();
    InitMagic();
}