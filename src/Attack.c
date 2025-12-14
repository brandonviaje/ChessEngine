#include "../include/Attack.h"

// BitBoards
extern U64 bitboards[12];
extern U64 occupied;

// Precomputed Table
U64 knightAttacks[64];
U64 kingAttacks[64];
U64 pawnAttacks[2][64]; // 0 white // 1 black

extern U64 *attackTable;
extern SMagic bishopAttacks[64];
extern SMagic rookAttacks[64];
extern U64 occupied;

// Game State
extern int side;

int IsInCheck()
{
    int kingSquare = __builtin_ctzll(side == WHITE ? bitboards[K] : bitboards[k]); // get king square of current side
    return IsAttackSquare(side, kingSquare);                                       // check if the king square is getting attacked
}

int IsAttackSquare(int side, int square)
{
    return 0; // no enemy pieces attack this square
}

U64 GeneratePawnAttacks(int side, int square)
{
    // catch invalid square index
    if (square < 0 || square > 63)
    {
        fprintf(stderr, "Error: invalid square index: %d", square);
        exit(EXIT_FAILURE);
    }

    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    SetBit(bitboard, square);

    if (side == WHITE)
    {
        // get left and right captures, prevent wrapping
        attacks |= (bitboard << 7) & ~FILE_H;
        attacks |= (bitboard << 9) & ~FILE_A;
    }
    else
    {
        // get left and right captures, prevent wrapping
        attacks |= (bitboard >> 7) & ~FILE_A;
        attacks |= (bitboard >> 9) & ~FILE_H;
    }

    return attacks;
}

U64 GenerateKnightAttacks(int square)
{
    // catch invalid square index
    if (square < 0 || square > 63)
    {
        fprintf(stderr, "Error: invalid square index: %d", square);
        exit(EXIT_FAILURE);
    }

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
    // catch invalid square index
    if (square < 0 || square > 63)
    {
        fprintf(stderr, "Error: invalid square index: %d", square);
        exit(EXIT_FAILURE);
    }

    U64 attacks = 0ULL;
    U64 bitboard = 0ULL;

    SetBit(bitboard, square);

    // Possible king moves
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

U64 GenerateBishopAttacks(int square)
{
    U64 *atkptr = bishopAttacks[square].ptr;
    U64 blockers = occupied & bishopAttacks[square].mask;
    U64 index = (blockers * bishopAttacks[square].magic) >> bishopAttacks[square].shift;
    return atkptr[index];
}

U64 GenerateRookAttacks(int square)
{
    U64 *atkptr = rookAttacks[square].ptr;
    U64 blockers = occupied & rookAttacks[square].mask;
    U64 index = (blockers * rookAttacks[square].magic) >> rookAttacks[square].shift;
    return atkptr[index];
}

U64 GenerateQueenAttacks(int square)
{
    return GenerateBishopAttacks(square) | GenerateRookAttacks(square);
}

// Initialize precomputed attack tables
void InitTables()
{
    for (int square = 0; square < 63; square++)
    {
        pawnAttacks[WHITE][square] = GeneratePawnAttacks(WHITE, square);
        pawnAttacks[BLACK][square] = GeneratePawnAttacks(BLACK, square);
        knightAttacks[square] = GenerateKnightAttacks(square);
        kingAttacks[square] = GenerateKingAttacks(square);
    }
}