#include "../include/Evaluate.h"

extern U64 bitboards[12];
extern int side;
extern int halfmove;
extern U64 occupied;

//  MATERIAL
int PieceValue(int piece, int phase)
{
    // phase = 0 (endgame) -> endgame weights, 128 = opening
    switch (piece)
    {
    case P:
    case p:
        return (phase * 100 + (128 - phase) * 120) / 128;
    case N:
    case n:
        return (phase * 320 + (128 - phase) * 310) / 128;
    case B:
    case b:
        return (phase * 330 + (128 - phase) * 350) / 128;
    case R:
    case r:
        return (phase * 500 + (128 - phase) * 510) / 128;
    case Q:
    case q:
        return 900;
    case K:
    case k:
        return 20000;
    default:
        return 0;
    }
}

// material based eval
int SimpleEval(int phase)
{
    int score = 0;

    // loop through white pieces
    score += PopCount(bitboards[P]) * PieceValue(P, phase);
    score += PopCount(bitboards[N]) * PieceValue(N, phase);
    score += PopCount(bitboards[B]) * PieceValue(B, phase);
    score += PopCount(bitboards[R]) * PieceValue(R, phase);
    score += PopCount(bitboards[Q]) * PieceValue(Q, phase);
    score += PopCount(bitboards[K]) * PieceValue(K, phase);

    // loop through black pieces
    score -= PopCount(bitboards[p]) * PieceValue(p, phase);
    score -= PopCount(bitboards[n]) * PieceValue(n, phase);
    score -= PopCount(bitboards[b]) * PieceValue(b, phase);
    score -= PopCount(bitboards[r]) * PieceValue(r, phase);
    score -= PopCount(bitboards[q]) * PieceValue(q, phase);
    score -= PopCount(bitboards[k]) * PieceValue(k, phase);

    return score;
}

//  PAWN STRUCTURE

// count passed pawns
int PassedPawns(U64 pawns, U64 enemyPawns, int color)
{
    int count = 0;
    while (pawns)
    {
        int sq = __builtin_ctzll(pawns);
        int rank = sq / 8;
        int file = sq % 8;
        U64 mask = 0ULL;

        // generate mask of squares in front of the pawn
        if (color == WHITE)
        {
            for (int r = rank + 1; r < 8; r++)
                for (int f = (file > 0 ? file - 1 : 0); f <= (file < 7 ? file + 1 : 7); f++)
                    mask |= 1ULL << (r * 8 + f);
        }
        else
        {
            for (int r = rank - 1; r >= 0; r--)
                for (int f = (file > 0 ? file - 1 : 0); f <= (file < 7 ? file + 1 : 7); f++)
                    mask |= 1ULL << (r * 8 + f);
        }

        if ((mask & enemyPawns) == 0)
            count++;
        pawns &= pawns - 1;
    }
    return count;
}

// count if there's doubled pawns on board
int DoubledPawns(U64 pawns)
{
    int count = 0;
    for (int f = 0; f < 8; f++)
    {
        U64 mask = 0x0101010101010101ULL << f;
        int n = PopCount(pawns & mask);
        if (n > 1)
            count += n - 1;
    }
    return count;
}

// count isolated pawns
int IsolatedPawns(U64 pawns)
{
    int count = 0;
    for (int f = 0; f < 8; f++)
    {
        U64 fileMask = 0x0101010101010101ULL << f;
        if (fileMask & pawns)
        {
            U64 adj = 0;
            if (f > 0)
                adj |= 0x0101010101010101ULL << (f - 1);
            if (f < 7)
                adj |= 0x0101010101010101ULL << (f + 1);
            if (!(adj & pawns))
                count += PopCount(fileMask & pawns);
        }
    }
    return count;
}

// count backward pawns
int BackwardPawns(U64 pawns, U64 enemyPawns, int color)
{
    int count = 0;
    while (pawns)
    {
        int sq = __builtin_ctzll(pawns);
        int rank = sq / 8, file = sq % 8;
        U64 mask = 0ULL;
        if (color == WHITE)
        {
            if (rank < 7)
                mask |= 1ULL << ((rank + 1) * 8 + file);
        }
        else
        {
            if (rank > 0)
                mask |= 1ULL << ((rank - 1) * 8 + file);
        }
        if (mask & enemyPawns)
            count++;
        pawns &= pawns - 1;
    }
    return count;
}

//  check how many pawns are in front of the king
int KingPawnShield(U64 king, U64 pawns, int color)
{
    if (!king) // no king? then GG
        return 0;

    int sq = __builtin_ctzll(king); // king square
    int rank = sq / 8, file = sq % 8;

    U64 shield = 0ULL;

    if (color == WHITE) // white moves up
    {
        if (rank + 1 <= 7)
            shield |= 1ULL << ((rank + 1) * 8 + file); // pawn just ahead
        if (rank + 2 <= 7)
            shield |= 1ULL << ((rank + 2) * 8 + file); // pawn two steps ahead
        if (file > 0)
            shield |= 1ULL << ((rank + 1) * 8 + (file - 1)); // left side
        if (file < 7)
            shield |= 1ULL << ((rank + 1) * 8 + (file + 1)); // right side
    }
    else // black moves down
    {
        if (rank - 1 >= 0)
            shield |= 1ULL << ((rank - 1) * 8 + file); // pawn just ahead
        if (rank - 2 >= 0)
            shield |= 1ULL << ((rank - 2) * 8 + file); // pawn two steps ahead
        if (file > 0)
            shield |= 1ULL << ((rank - 1) * 8 + (file - 1)); // left side
        if (file < 7)
            shield |= 1ULL << ((rank - 1) * 8 + (file + 1)); // right side
    }

    return PopCount(shield & pawns); // count how many pawns are keeping the king safe
}

// king
int KingCentralization(U64 king)
{
    if (!king)
        return 0;
    int sq = __builtin_ctzll(king);
    int rank = sq / 8, file = sq % 8;
    return (4 - abs(3 - rank)) + (4 - abs(3 - file));
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

// MOBILITY: How many squares a piece can reach, weighted by type
int Mobility(U64 pieces, int pieceType)
{
    int score = 0;
    while (pieces)
    {
        // get first piece
        int sq = __builtin_ctzll(pieces);
        U64 attacks = 0ULL;
        // grab attack mask for this piece
        switch (pieceType)
        {
        case N:
            attacks = knightAttacks[sq];
            break;
        case B:
            attacks = GetBishopAttacks(sq, occupied);
            break;
        case R:
            attacks = GetRookAttacks(sq, occupied);
            break;
        case Q:
            attacks = GetBishopAttacks(sq, occupied) | GetRookAttacks(sq, occupied);
            break;
        default:
            attacks = 0;
            break;
        }
        // give pieces weight based on piece type
        int weight = (pieceType == N) ? 1 : (pieceType == B) ? 2
                                        : (pieceType == R)   ? 3
                                        : (pieceType == Q)   ? 4
                                                             : 0;
        score += PopCount(attacks) * weight;
        pieces &= pieces - 1; // pop LSB and process next one
    }
    return score;
}

// CENTRALIZATION: Bonus for being near the center
int CentralizationBonus(U64 pieces, int pieceType)
{
    int score = 0;
    while (pieces)
    {
        int sq = __builtin_ctzll(pieces);
        int r = sq / 8, f = sq % 8;
        int dist = abs(3 - r) + abs(3 - f);
        int mult = (pieceType == R || pieceType == Q) ? 5 : 10;
        score += (4 - dist) * mult;
        pieces &= pieces - 1;
    }
    return score;
}

//  MISC
int HasBishopPair(U64 bishops) { return PopCount(bishops) >= 2; }

int RooksOnOpenFiles(U64 rooks)
{
    int count = 0;
    for (int f = 0; f < 8; f++)
    {
        U64 fileMask = 0x0101010101010101ULL << f;
        U64 pawnsOnFile = fileMask & (bitboards[P] | bitboards[p]);
        if (!pawnsOnFile)
            count += PopCount(rooks & fileMask);
    }
    return count;
}

int RooksOn7th(U64 rooks, int color)
{
    int rank = (color == WHITE) ? 6 : 1;
    U64 mask = 0xFFULL << (rank * 8);
    return PopCount(rooks & mask);
}

// count how many bits in BB
int PopCount(U64 bb)
{
    int count = 0;
    while (bb)
    {
        bb &= bb - 1;
        count++;
    }
    return count;
}

// flip square for black’s POV
int Mirror(int square)
{
    return (7 - square / 8) * 8 + (square % 8);
}

//  GAME PHASE
int GamePhase()
{
    int phase = 128;
    int totalMaterial = 0;
    totalMaterial += PopCount(bitboards[N]) * 1;
    totalMaterial += PopCount(bitboards[B]) * 1;
    totalMaterial += PopCount(bitboards[R]) * 2;
    totalMaterial += PopCount(bitboards[Q]) * 4;
    phase -= totalMaterial;
    if (phase < 0)
        phase = 0;
    return phase;
}

//  FULL EVALUATION
int Evaluate()
{
    int phase = GamePhase();

    int score = SimpleEval(phase) + PositionalEval(phase);

    // Pawn structure
    score += (PassedPawns(bitboards[P], bitboards[p], WHITE) - PassedPawns(bitboards[p], bitboards[P], BLACK)) * 50;
    score -= (DoubledPawns(bitboards[P]) - DoubledPawns(bitboards[p])) * 20;
    score -= (IsolatedPawns(bitboards[P]) - IsolatedPawns(bitboards[p])) * 20;
    score -= (BackwardPawns(bitboards[P], bitboards[p], WHITE) - BackwardPawns(bitboards[p], bitboards[P], BLACK)) * 15;

    // Bishop pair
    score += HasBishopPair(bitboards[B]) ? 30 : 0;
    score -= HasBishopPair(bitboards[b]) ? 30 : 0;

    // Rooks
    score += RooksOnOpenFiles(bitboards[R]) * 25;
    score -= RooksOnOpenFiles(bitboards[r]) * 25;

    score += RooksOn7th(bitboards[R], WHITE);
    score -= RooksOn7th(bitboards[r], BLACK);

    // King safety / activity
    score += KingPawnShield(bitboards[K], bitboards[P], WHITE) * 10;
    score -= KingPawnShield(bitboards[k], bitboards[p], BLACK) * 10;

    score += KingCentralization(bitboards[K]) * 5;
    score -= KingCentralization(bitboards[k]) * 5;

    // Mobility & centralization
    score += Mobility(bitboards[N], N) + CentralizationBonus(bitboards[N], N);
    score -= Mobility(bitboards[n], N) + CentralizationBonus(bitboards[n], N);

    score += Mobility(bitboards[B], B) + CentralizationBonus(bitboards[B], B);
    score -= Mobility(bitboards[b], B) + CentralizationBonus(bitboards[b], B);

    score += Mobility(bitboards[R], R) + CentralizationBonus(bitboards[R], R);
    score -= Mobility(bitboards[r], R) + CentralizationBonus(bitboards[r], R);

    score += Mobility(bitboards[Q], Q) + CentralizationBonus(bitboards[Q], Q);
    score -= Mobility(bitboards[q], Q) + CentralizationBonus(bitboards[q], Q);

    // 50-move halfmove damping
    score -= score * halfmove / 200;

    return (side == WHITE) ? score : -score;
}

void PrintEvaluation()
{
    printf("Evaluation: %d centipawns\n", Evaluate());
}