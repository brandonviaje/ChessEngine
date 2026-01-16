#ifndef UTILS_H
#define UTILS_H

#include "Attack.h"
#include "Board.h"
#include "MyTypes.h"
#include "PieceSquareTable.h"

extern U64 FileBBMask[8];
extern U64 AdjacentFilesMask[8];
extern U64 PassedPawnMask[2][64];
extern U64 KingShieldMask[2][64];

enum Square
{
    A1,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8
};

int DoubledPawns(U64 pawns);
int IsolatedPawns(U64 pawns);
int BackwardPawns(U64 pawns, U64 enemyPawns, int color);
int PassedPawns(U64 pawns, U64 enemyPawns, int color);
int IsDefendedByPawn(int sq, int color);
int IsDefended(int sq, int color);
int IsOutpost(int sq, int color);
int IsTrappedKnight(int sq);
int BadBishops(U64 bishops, U64 pawns);
int RooksOnOpenFiles(U64 rooks);
int RooksOn7th(U64 rooks, int color);
int ConnectedRooks(U64 rooks);
int EarlyQueenDeveloped(int color);
int KingPawnShield(U64 king, U64 pawns, int color);
int KingCentralization(U64 king);
void InitEvalMasks();

#endif
