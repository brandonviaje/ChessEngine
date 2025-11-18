#include "../include/Magic.h"

U64 attackTable[];
SMagic bishopAttacks[64];
SMagic rookAttacks[64];

extern U64 occupied;

U64 GenerateBishopAttacks(int square) {
   U64* atkptr = bishopAttacks[square].ptr;
   U64 blockers = occupied & bishopAttacks[square].mask;    // AND the current attack table with the relevant bits
   blockers *= bishopAttacks[square].magic;                 // multiply mask with magic number
   blockers >>= bishopAttacks[square].shift;                // shift n bits to clear garbage bits
   return atkptr[blockers];
}

U64 GenerateRookAttacks(int square) {
   U64* atkptr = rookAttacks[square].ptr;
   U64 blockers = occupied & rookAttacks[square].mask;  // AND the current occupied board with the relevant bits
   blockers *= rookAttacks[square].magic;               // multiply mask with magic number
   blockers >>= rookAttacks[square].shift;              // shift n bits to clear garbage bits
   return atkptr[blockers];
}

U64 GenerateQueenAttacks(int square){
    return GenerateBishopAttacks(square) | GenerateRookAttacks(square); // queen attacks are the union of bishop and rook attacks
}

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

// gets optimal magic number to multiply mask by
U64 GetMagicNumber(){
    U64 candidate = 0ULL;
    return candidate;
}

void InitMagicBitBoards(){
    int rookOffset = 0;
    int bishopOffset = 0;
    for(int square = 0; square <64; square++){
        // init bishop moves
        bishopAttacks[square].ptr = &attackTable[bishopOffset];
        bishopAttacks[square].mask = BishopMask(square);
        bishopAttacks[square].magic = GetMagicNumber();
        int relevantBits = __builtin_popcountll(bishopAttacks[square].mask);
        bishopAttacks[square].shift = 64 - relevantBits;

        // init rook moves
        rookAttacks[square].ptr = &attackTable[rookOffset];
        rookAttacks[square].mask = RookMask(square);
        rookAttacks[square].magic = GetMagicNumber();
        int relevantBits = __builtin_popcountll(rookAttacks[square].mask);
        rookAttacks[square].shift = 64 - relevantBits;
    }
}