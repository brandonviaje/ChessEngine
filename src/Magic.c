#include "../include/Magic.h"
#include <stdlib.h>
#include <stdio.h>

U64 bishopMasks[64];
U64 rookMasks[64];
SMagic bishopAttacks[64];
SMagic rookAttacks[64];
extern U64 occupied;

// Generate relevant bit masks
U64 BishopMask(int square)
{
    U64 mask = 0ULL;
    int rank = square / 8, file = square % 8;

    for (int r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++, f++)
        SetBit(mask, r * 8 + f);
    for (int r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++, f--)
        SetBit(mask, r * 8 + f);
    for (int r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--, f++)
        SetBit(mask, r * 8 + f);
    for (int r = rank - 1, f = file - 1; r >= 1 && f >= 1; r--, f--)
        SetBit(mask, r * 8 + f);

    return mask;
}

// Generate relevant bit masks
U64 RookMask(int square)
{
    U64 mask = 0ULL;
    int rank = square / 8, file = square % 8;

    for (int r = rank + 1; r <= 6; r++)
        SetBit(mask, r * 8 + file);
    for (int r = rank - 1; r >= 1; r--)
        SetBit(mask, r * 8 + file);
    for (int f = file + 1; f <= 6; f++)
        SetBit(mask, rank * 8 + f);
    for (int f = file - 1; f >= 1; f--)
        SetBit(mask, rank * 8 + f);

    return mask;
}

// Generate blocker bitboard from index
U64 GetBlockerFromIndex(int index, U64 mask)
{
    U64 blockers = 0ULL;
    int bits = __builtin_popcountll(mask);

    for (int i = 0; i < bits; i++)
    {
        int bitPos = __builtin_ctzll(mask);
        
        if (index & (1 << i))
            blockers |= (1ULL << bitPos);

        mask &= mask - 1;
    }

    return blockers;
}

// Slow sliding attacks
U64 RookAttacksSlow(int sq, U64 blockers)
{
    U64 attacks = 0ULL;
    int r = sq / 8, f = sq % 8;

    for (int rr = r + 1; rr <= 7; rr++)
    {
        int s = rr * 8 + f;
        attacks |= 1ULL << s;
        if (blockers & (1ULL << s))
            break;
    }
    for (int rr = r - 1; rr >= 0; rr--)
    {
        int s = rr * 8 + f;
        attacks |= 1ULL << s;
        if (blockers & (1ULL << s))
            break;
    }
    for (int ff = f + 1; ff <= 7; ff++)
    {
        int s = r * 8 + ff;
        attacks |= 1ULL << s;
        if (blockers & (1ULL << s))
            break;
    }
    for (int ff = f - 1; ff >= 0; ff--)
    {
        int s = r * 8 + ff;
        attacks |= 1ULL << s;
        if (blockers & (1ULL << s))
            break;
    }

    return attacks;
}

U64 BishopAttacksSlow(int square, U64 blockers)
{
    U64 attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    for (int r = rank + 1, f = file + 1; r <= 7 && f <= 7; r++, f++)
    {
        int sq = r * 8 + f;
        attacks |= 1ULL << sq;
        if (blockers & (1ULL << sq))
            break;
    }
    for (int r = rank + 1, f = file - 1; r <= 7 && f >= 0; r++, f--)
    {
        int sq = r * 8 + f;
        attacks |= 1ULL << sq;
        if (blockers & (1ULL << sq))
            break;
    }
    for (int r = rank - 1, f = file + 1; r >= 0 && f <= 7; r--, f++)
    {
        int sq = r * 8 + f;
        attacks |= 1ULL << sq;
        if (blockers & (1ULL << sq))
            break;
    }
    for (int r = rank - 1, f = file - 1; r >= 0 && f >= 0; r--, f--)
    {
        int sq = r * 8 + f;
        attacks |= 1ULL << sq;
        if (blockers & (1ULL << sq))
            break;
    }

    return attacks;
}

// Initialize Magic Tables

void InitMagicTable(SMagic attacks[64], int isBishop)
{
    for (int sq = 0; sq < 64; sq++)
    {
        SMagic *magic = &attacks[sq];

        // Compute relevant blocker mask
        magic->mask = isBishop ? BishopMask(sq) : RookMask(sq);

        // Assign magic number
        magic->magic = isBishop ? bishopMagics[sq] : rookMagics[sq];

        // Shift for table index
        int bits = __builtin_popcountll(magic->mask);
        magic->shift = 64 - bits;

        // Allocate table of size 2^bits
        int tableSize = 1 << bits;
        magic->ptr = malloc(sizeof(U64) * tableSize);

        // Fill table using magic hash
        for (int i = 0; i < tableSize; i++)
        {
            U64 blockers = GetBlockerFromIndex(i, magic->mask);
            U64 attacksBB = isBishop ? BishopAttacksSlow(sq, blockers) : RookAttacksSlow(sq, blockers);
            int index = (blockers * magic->magic) >> magic->shift;
            magic->ptr[index] = attacksBB;
        }
    }
}

void InitMagic()
{
    InitMagicTable(rookAttacks, 0);
    InitMagicTable(bishopAttacks, 1);
}

// Cleanup Attack Tables

void CleanupMagic()
{
    for (int sq = 0; sq < 64; sq++)
    {
        free(rookAttacks[sq].ptr);
        free(bishopAttacks[sq].ptr);
    }
}
