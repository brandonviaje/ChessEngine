#include "../include/Evaluate.h"

extern U64 bitboards[12];
extern int side;
extern int halfmove;
extern U64 occupied;

//  MATERIAL
int MaterialAndPosition(int phase)
{
    int score = 0;
    U64 bb;
    int sq;

    // Define base values based on phase
    int KnightVal = 320 - ((phase > 0) ? ((16 - phase / 8) * 2) : 0);

    if (KnightVal > 320)
        KnightVal = 320; // Clamp

    if (KnightVal < 300)
        KnightVal = 300;

    int RookVal = 500 + (16 - phase / 8) * 2;

    //  WHITE PIECES

    // Pawns
    bb = bitboards[P];
    while (bb)
    {
        sq = lsb(bb);
        score += 100 + PawnPST[sq];
        pop_lsb(&bb);
    }

    // Knights
    bb = bitboards[N];
    while (bb)
    {
        sq = lsb(bb);
        score += KnightVal + KnightPST[sq];
        pop_lsb(&bb);
    }

    // Bishops
    bb = bitboards[B];
    while (bb)
    {
        sq = lsb(bb);
        score += 330 + BishopPST[sq];
        pop_lsb(&bb);
    }

    // Rooks
    bb = bitboards[R];
    while (bb)
    {
        sq = lsb(bb);
        score += RookVal + RookPST[sq];
        pop_lsb(&bb);
    }

    // Queens
    bb = bitboards[Q];
    while (bb)
    {
        sq = lsb(bb);
        score += 900 + QueenPST[sq];
        pop_lsb(&bb);
    }

    // King
    bb = bitboards[K];
    if (bb)
    {
        sq = lsb(bb);
        score += 20000 + KingPST[sq];
    }

    //  BLACK PIECES

    // Pawns
    bb = bitboards[p];
    while (bb)
    {
        sq = lsb(bb);
        score -= (100 + PawnPST[Mirror(sq)]);
        pop_lsb(&bb);
    }

    // Knights
    bb = bitboards[n];
    while (bb)
    {
        sq = lsb(bb);
        score -= (KnightVal + KnightPST[Mirror(sq)]);
        pop_lsb(&bb);
    }

    // Bishops
    bb = bitboards[b];
    while (bb)
    {
        sq = lsb(bb);
        score -= (330 + BishopPST[Mirror(sq)]);
        pop_lsb(&bb);
    }

    // Rooks
    bb = bitboards[r];
    while (bb)
    {
        sq = lsb(bb);
        score -= (RookVal + RookPST[Mirror(sq)]);
        pop_lsb(&bb);
    }

    // Queens
    bb = bitboards[q];
    while (bb)
    {
        sq = lsb(bb);
        score -= (900 + QueenPST[Mirror(sq)]);
        pop_lsb(&bb);
    }

    // King
    bb = bitboards[k];
    if (bb)
    {
        sq = lsb(bb);
        score -= (20000 + KingPST[Mirror(sq)]);
    }

    return score;
}

//  POSITIONAL EVAL
int PositionalEval(int phase)
{
    int score = 0;
    for (int sq = 0; sq < 64; sq++)
    {
        // white pieces get positive score from their squares
        if (GetBit(bitboards[P], sq))
            score += PawnPST[sq];
        if (GetBit(bitboards[N], sq))
            score += KnightPST[sq];
        if (GetBit(bitboards[B], sq))
            score += BishopPST[sq];
        if (GetBit(bitboards[R], sq))
            score += RookPST[sq];
        if (GetBit(bitboards[Q], sq))
            score += QueenPST[sq];
        if (GetBit(bitboards[K], sq))
            score += KingPST[sq];

        // black pieces changes white score
        if (GetBit(bitboards[p], sq))
            score -= PawnPST[Mirror(sq)];
        if (GetBit(bitboards[n], sq))
            score -= KnightPST[Mirror(sq)];
        if (GetBit(bitboards[b], sq))
            score -= BishopPST[Mirror(sq)];
        if (GetBit(bitboards[r], sq))
            score -= RookPST[Mirror(sq)];
        if (GetBit(bitboards[q], sq))
            score -= QueenPST[Mirror(sq)];
        if (GetBit(bitboards[k], sq))
            score -= KingPST[Mirror(sq)];
    }
    return score;
}

// PAWNS

int PawnEval(int phase)
{
    int score = 0;
    int lateGameWeight = (128 - phase); // Higher number = later game

    // Isolated & Doubled
    score -= IsolatedPawns(bitboards[P]) * 12;
    score += IsolatedPawns(bitboards[p]) * 12;

    score -= DoubledPawns(bitboards[P]) * 10;
    score += DoubledPawns(bitboards[p]) * 10;

    // Backward
    score -= BackwardPawns(bitboards[P], bitboards[p], WHITE) * 4;
    score += BackwardPawns(bitboards[p], bitboards[P], BLACK) * 4;

    // Passed Pawns
    score += PassedPawns(bitboards[P], bitboards[p], WHITE) * 15 * lateGameWeight / 128;
    score -= PassedPawns(bitboards[p], bitboards[P], BLACK) * 15 * lateGameWeight / 128;

    // Check White D2 pawn blocked by D3 piece
    if ((bitboards[P] & (1ULL << D2)) && (occupied & (1ULL << D3)))
        score -= 8;
    if ((bitboards[P] & (1ULL << E2)) && (occupied & (1ULL << E3)))
        score -= 8;

    // Check Black D7 pawn blocked by D6 piece
    if ((bitboards[p] & (1ULL << D7)) && (occupied & (1ULL << D6)))
        score += 8;
    if ((bitboards[p] & (1ULL << E7)) && (occupied & (1ULL << E6)))
        score += 8;

    return score;
}

//  KNIGHTS

int KnightEval()
{
    int score = 0;

    // Calculate ALL Pawn Attacks at once.
    U64 whitePawnAttacks = ((bitboards[P] & ~FILE_A) << 7) | ((bitboards[P] & ~FILE_H) << 9);
    U64 blackPawnAttacks = ((bitboards[p] & ~FILE_A) >> 9) | ((bitboards[p] & ~FILE_H) >> 7);

    // WHITE KNIGHTS
    U64 bb = bitboards[N];
    while (bb)
    {
        int sq = lsb(bb);

        if (IsOutpost(sq, WHITE))
            score += 25;
        if (IsDefendedByPawn(sq, WHITE))
            score += 6;
        if (IsTrappedKnight(sq))
            score -= 80;
        if (!IsDefended(sq, WHITE))
            score -= 8;

        // mobility: valid moves NOT attacked by enemy pawns
        U64 moves = knightAttacks[sq] & ~blackPawnAttacks;
        score += PopCount(moves);

        pop_lsb(&bb);
    }

    // BLACK KNIGHTS
    bb = bitboards[n];
    while (bb)
    {
        int sq = lsb(bb);

        if (IsOutpost(sq, BLACK))
            score -= 25;
        if (IsDefendedByPawn(sq, BLACK))
            score -= 6;
        if (IsTrappedKnight(sq))
            score += 80;
        if (!IsDefended(sq, BLACK))
            score += 8;

        U64 moves = knightAttacks[sq] & ~whitePawnAttacks;
        score -= PopCount(moves);

        pop_lsb(&bb);
    }

    // trap: White Knight on C3 blocked
    // check specific squares
    if ((bitboards[N] & (1ULL << C3)) && (bitboards[P] & (1ULL << C2)) && (bitboards[P] & (1ULL << D4)) && !(bitboards[P] & (1ULL << E4)))
    {
        score -= 10;
    }

    return score;
}

//  BISHOPS

int BishopEval(int phase)
{
    int score = 0;

    if (PopCount(bitboards[B]) >= 2)
        score += (phase > 64 ? 25 : 50);
    if (PopCount(bitboards[b]) >= 2)
        score -= (phase > 64 ? 25 : 50);

    score -= BadBishops(bitboards[B], bitboards[P]) * 4;
    score += BadBishops(bitboards[b], bitboards[p]) * 4;

    return score;
}

//  ROOKS

int RookEval()
{
    int score = 0;

    score += RooksOnOpenFiles(bitboards[R]) * 15;
    score -= RooksOnOpenFiles(bitboards[r]) * 15;

    score += RooksOn7th(bitboards[R], WHITE) * 25;
    score -= RooksOn7th(bitboards[r], BLACK) * 25;

    if (ConnectedRooks(bitboards[R]))
        score += 8;
    if (ConnectedRooks(bitboards[r]))
        score -= 8;

    return score;
}

//  QUEEN

int QueenEval()
{
    int score = 0;

    if (EarlyQueenDeveloped(WHITE))
        score -= 10;
    if (EarlyQueenDeveloped(BLACK))
        score += 10;

    return score;
}

//  KING

int KingEval(int phase)
{
    int score = 0;

    if (phase > 64)
    {
        score += KingPawnShield(bitboards[K], bitboards[P], WHITE) * 10;
        score -= KingPawnShield(bitboards[k], bitboards[p], BLACK) * 10;
    }
    else
    {
        score += KingCentralization(bitboards[K]) * 5;
        score -= KingCentralization(bitboards[k]) * 5;
    }

    return score;
}

//  PHASE

int GamePhase()
{
    int phase = 128;
    phase -= PopCount(bitboards[N]);
    phase -= PopCount(bitboards[B]);
    phase -= PopCount(bitboards[R]) * 2;
    phase -= PopCount(bitboards[Q]) * 4;
    if (phase < 0)
        phase = 0;
    return phase;
}

//  EVAL

int Evaluate()
{
    int phase = GamePhase();
    int score = 0;

    score += MaterialAndPosition(phase);
    score += PositionalEval(phase);
    score += PawnEval(phase);
    score += KnightEval();
    score += BishopEval(phase);
    score += RookEval();
    score += QueenEval();
    score += KingEval(phase);

    score -= score * halfmove / 200;

    return (side == WHITE) ? score : -score;
}

void PrintEvaluation()
{
    printf("Evaluation: %d centipawns\n", Evaluate());
}

void TraceEvaluation()
{
    int phase = GamePhase();
    int score = 0;

    printf("\n EVALUATION BREAKDOWN \n");

    int matPos = MaterialAndPosition(phase);
    printf("Material & PST:  %d\n", matPos);
    score += matPos;

    int pawns = PawnEval(phase);
    printf("Pawn Structure:  %d\n", pawns);
    score += pawns;

    int knights = KnightEval();
    printf("Knight Activity: %d\n", knights);
    score += knights;

    int bishops = BishopEval(phase);
    printf("Bishop Pair/Bad: %d\n", bishops);
    score += bishops;

    int rooks = RookEval();
    printf("Rook Open/Conn:  %d\n", rooks);
    score += rooks;

    int queens = QueenEval();
    printf("Queen EarlyDev:  %d\n", queens);
    score += queens;

    int kings = KingEval(phase);
    printf("King Safety:     %d\n", kings);
    score += kings;

    // Tempo / Drawishness
    int finalScore = (side == WHITE) ? score : -score;
    printf("--\n");
    printf("Total Raw:       %d\n", score);
    printf("Final (Side):    %d\n", finalScore);
    printf("--\n\n");
}