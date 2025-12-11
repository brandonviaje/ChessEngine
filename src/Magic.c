#include "../include/Magic.h"

U64* attackTable = NULL;
SMagic bishopAttacks[64];
SMagic rookAttacks[64];
extern U64 occupied;

// generate relevant blocker masks for each square
U64 BishopMask(int square){
    U64 mask = 0ULL;
    int rank = square / 8, file = square % 8;

    // Up-Right
    for(int r = rank + 1, f = file + 1; r <= 6 && f <= 6; r++,f++)
        SetBit(mask,(r*8+f));
    
    // Up-Left
    for(int r = rank + 1, f = file - 1; r <= 6 && f >= 1; r++,f--)
        SetBit(mask,(r*8+f));
    
    // Down-Right
    for(int r = rank - 1, f = file + 1; r >= 1 && f <= 6; r--,f++)
        SetBit(mask,(r*8+f));
    
    // Down-Left
    for(int r = rank -1, f = file -1; r >= 1 && f >= 1; r--,f--)
        SetBit(mask,(r*8+f));

    return mask;
}

// generate relevant blocker masks for each square
U64 RookMask(int square) {
    U64 mask = 0ULL;
    int rank = square / 8, file = square % 8;

    // up: stop before edge 7
    for (int r = rank + 1; r <= 6; r++)
        SetBit(mask, (r * 8 + file));

    // down: stop before edge 0
    for (int r = rank - 1; r >= 1; r--) 
        SetBit(mask,(r * 8 + file));

    // right: stop before edge 7
    for (int f = file + 1; f <= 6; f++) 
        SetBit(mask, (rank * 8 + f));

    // left: stop before edge 0
    for (int f = file - 1; f >= 1; f--) 
        SetBit(mask, (rank * 8 + f));

    return mask;
}

// generate blocker bitboard from index
U64 GenerateBlockerFromIndex(int index, U64 mask) {
    U64 blockers = 0ULL;
    int bits[12]; // max 12 relevant bits
    int bitCount = 0;

    for (int i = 0; i < 64; i++) {
        if (mask & (1ULL << i))
            bits[bitCount++] = i;
    }

    for (int i = 0; i < bitCount; i++) {
        if (index & (1 << i))
            blockers |= 1ULL << bits[i];
    }
    return blockers;
}

// bishop sliding attack generation
U64 ComputeBishopAttacks(int square, U64 blockers) {
    U64 attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    // Up-Right
    for(int r = rank+1, f = file+1; r <=7 && f <=7; r++,f++) {
        SetBit(attacks,r*8+f);
        if (blockers & (1ULL << (r*8+f))) break;
    }
    // Up-Left
    for(int r = rank+1, f = file-1; r <=7 && f >=0; r++,f--) {
        SetBit(attacks,r*8+f);
        if (blockers & (1ULL << (r*8+f))) break;
    }
    // Down-Right
    for(int r = rank-1, f = file+1; r >=0 && f <=7; r--,f++) {
        SetBit(attacks,r*8+f);
        if (blockers & (1ULL << (r*8+f))) break;
    }
    // Down-Left
    for(int r = rank-1, f = file-1; r >=0 && f >=0; r--,f--) {
        SetBit(attacks,r*8+f);
        if (blockers & (1ULL << (r*8+f))) break;
    }
    return attacks;
}

// rook sliding attack generation
U64 ComputeRookAttacks(int square, U64 blockers) {
    U64 attacks = 0ULL;
    int rank = square / 8, file = square % 8;

    // Up
    for(int r = rank+1; r<=7; r++) {
        SetBit(attacks,r*8+file);
        if (blockers & (1ULL << (r*8+file))) break;
    }
    // Down
    for(int r = rank-1; r>=0; r--) {
        SetBit(attacks,r*8+file);
        if (blockers & (1ULL << (r*8+file))) break;
    }
    // Right
    for(int f = file+1; f<=7; f++) {
        SetBit(attacks,rank*8+f);
        if (blockers & (1ULL << (rank*8+f))) break;
    }
    // Left
    for(int f = file-1; f>=0; f--) {
        SetBit(attacks,rank*8+f);
        if (blockers & (1ULL << (rank*8+f))) break;
    }
    return attacks;
}

// fill out bishop table with attack
void FillBishopTable(int square) {
    U64 mask = bishopAttacks[square].mask;
    int numBits = __builtin_popcountll(mask);
    int tableSize = 1 << numBits;

    for (int i = 0; i < tableSize; i++) {
        U64 blockers = GenerateBlockerFromIndex(i, mask);
        U64 attack = ComputeBishopAttacks(square, blockers);
        int index = (blockers * bishopAttacks[square].magic) >> bishopAttacks[square].shift;
        bishopAttacks[square].ptr[index] = attack;
    }
}

// fill out rook table with attack
void FillRookTable(int square) {
    U64 mask = rookAttacks[square].mask;
    int numBits = __builtin_popcountll(mask);
    int tableSize = 1 << numBits;

    for (int i = 0; i < tableSize; i++) {
        U64 blockers = GenerateBlockerFromIndex(i, mask);
        U64 attack = ComputeRookAttacks(square, blockers);
        int index = (blockers * rookAttacks[square].magic) >> rookAttacks[square].shift;
        rookAttacks[square].ptr[index] = attack;
    }
}

void InitMagicBitBoards() {
    // calculate total table size
    int totalBishopSize = 0;
    int totalRookSize   = 0;

    for (int square = 0; square < 64; square++) {
        // add all possible blocker combos for the bishop square, same with rook square
        totalBishopSize += 1 << __builtin_popcountll(BishopMask(square));
        totalRookSize   += 1 << __builtin_popcountll(RookMask(square));
    }

    int totalSize = totalBishopSize + totalRookSize;

    attackTable = (U64*)malloc(totalSize * sizeof(U64));    // allocate attack table

    int bishopOffset = 0;
    int rookOffset   = totalBishopSize;                     // rook table starts after bishop tables

    // initialize SMagic structs
    for (int square = 0; square < 64; square++) {
        // Bishop
        bishopAttacks[square].ptr   = &attackTable[bishopOffset];
        bishopAttacks[square].mask  = BishopMask(square);
        bishopAttacks[square].magic = bishopMagics[square];
        int bishopBits =  __builtin_popcountll(bishopAttacks[square].mask);
        bishopAttacks[square].shift = 64 - bishopBits;
        bishopOffset += 1 << bishopBits;

        // Rook
        rookAttacks[square].ptr     = &attackTable[rookOffset];
        rookAttacks[square].mask    = RookMask(square);
        rookAttacks[square].magic   = rookMagics[square];
        int rookBits =  __builtin_popcountll(rookAttacks[square].mask);
        rookAttacks[square].shift   = 64 - rookBits;
        rookOffset += 1 << rookBits;
    }

    // fill all tables
    for (int square = 0; square < 64; square++) {
        FillBishopTable(square);
        FillRookTable(square);
    }
}

void CleanupMagicBitboards() {
    free(attackTable);          // dealloc mem
    attackTable = NULL;         // prevent dangling pointers
}