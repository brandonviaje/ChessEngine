#include "../include/Zobrist.h"

U64 pieceKeys[13][120];
U64 sideKey;
U64 castleKeys[16];
U64 enPassantKeys[120]; 
U64 positionKey;

U64 Rand64() 
{
    U64 r1 = (U64)(rand()) & 0xFFFF;
    U64 r2 = (U64)(rand()) & 0xFFFF;
    U64 r3 = (U64)(rand()) & 0xFFFF;
    U64 r4 = (U64)(rand()) & 0xFFFF;
    return r1 | (r2 << 16) | (r3 << 32) | (r4 << 48);
}

void InitZobrist() 
{
    // fill piece keys
    for(int piece = 0; piece < 13; piece++) 
    {
        for(int sq = 0; sq < 120; sq++) 
        {
            pieceKeys[piece][sq] = Rand64();
        }
    }

    // side key
    sideKey = Rand64();

    // castle keys
    for(int i = 0; i < 16; i++) 
    {
        castleKeys[i] = Rand64();
    }
    
    // en Passant keys  init all of them
    for(int i = 0; i < 120; i++) 
    {
        enPassantKeys[i] = Rand64();
    }
}

// Compute key from scratch 
U64 GeneratePosKey() 
{
    U64 finalKey = 0;

    // loop through all piece types
    for (int piece = 0; piece < 12; piece++) 
    {
        U64 bb = bitboards[piece];
        // scan the bitboard for pieces
        for (int sq = 0; sq < 64; sq++) 
        {
            if (GetBit(bb, sq)) 
            {
                finalKey ^= pieceKeys[piece][sq];
            }
        }
    }

    // hash Side
    if (side == WHITE) 
    {
        finalKey ^= sideKey;
    }

    // hash Castle
    finalKey ^= castleKeys[castle];

    // hash En Passant
    if (enpassant != -1) 
    {
        finalKey ^= enPassantKeys[enpassant];
    }

    return finalKey;
}

// Zobrist Helpers

void HashPiece(int piece, int square) 
{
    positionKey ^= pieceKeys[piece][square];
}

void HashCastle() 
{
    positionKey ^= castleKeys[castle];
}

void HashSide() 
{
    positionKey ^= sideKey;
}

void HashEnPassant() 
{
    if (enpassant != -1) 
    {
        positionKey ^= enPassantKeys[enpassant];
    }
}
