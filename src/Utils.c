#include "../include/Utils.h"

U64 FileBBMask[8];
U64 AdjacentFilesMask[8];
U64 PassedPawnMask[2][64];
U64 KingShieldMask[2][64];

// count doubled pawns (more than 1 pawn on same file)
int DoubledPawns(U64 pawns)
{
    int count = 0;
    for (int f = 0; f < 8; f++)
    {
        U64 filePawns = pawns & FileBBMask[f]; // pawns on this file
        if (filePawns)
        {
            int n = PopCount(filePawns);
            if (n > 1)
                count += (n - 1); // penalty for extra pawns
        }
    }
    return count;
}

// count isolated pawns (no friendly pawns on adj files)
int IsolatedPawns(U64 pawns)
{
    int count = 0;
    U64 temp = pawns;
    while (temp)
    {
        int sq = lsb(temp);

        // no neighbors
        if ((AdjacentFilesMask[sq % 8] & pawns) == 0)
            count++; // isolated pawn

        pop_lsb(&temp);
    }
    return count;
}

// count backward pawns (blocked by enemy pawn in front)
int BackwardPawns(U64 pawns, U64 enemyPawns, int color)
{
    int count = 0;
    while (pawns)
    {
        int sq = __builtin_ctzll(pawns);
        int r = sq / 8, f = sq % 8;

        int front = (color == WHITE) ? r + 1 : r - 1;
        if (front >= 0 && front <= 7)
        {
            int fsq = front * 8 + f;
            if (GetBit(enemyPawns, fsq))
                count++; // backward pawn
        }

        pawns &= pawns - 1; // remove this pawn
    }
    return count;
}

// count passed pawns on board
int PassedPawns(U64 pawns, U64 enemyPawns, int color)
{
    int count = 0;
    U64 temp = pawns;

    while (temp)
    {
        int sq = lsb(temp); // get the least significant pawn

        // check if there are no enemy pawns ahead on this file or neighboring files
        if ((PassedPawnMask[color][sq] & enemyPawns) == 0)
            count++; // it's a passed pawn

        pop_lsb(&temp); // remove this pawn and process next
    }

    return count;
}

// check if square is defended by friendly pawn
int IsDefendedByPawn(int sq, int color)
{
    int enemyColor = color ^ 1;
    U64 defenders = pawnAttacks[enemyColor][sq] & bitboards[color == WHITE ? P : p];
    return (defenders != 0);
}

// check if square is defended
int IsDefended(int sq, int color)
{
    return IsSquareAttacked(sq, color);
}

int IsOutpost(int sq, int color)
{
    int enemy = color ^ 1;
    U64 enemyPawns = bitboards[enemy == WHITE ? P : p];

    // cannot be driven off by pawn
    if (pawnAttacks[enemy][sq] & enemyPawns)
        return 0;

    return IsDefendedByPawn(sq, color);
}

// mobility trap detection
int IsTrappedKnight(int sq)
{
    return (sq == A1 || sq == H1 || sq == A8 || sq == H8 || sq == A2 || sq == H2 || sq == A7 || sq == H7);
}

int BadBishops(U64 bishops, U64 pawns)
{
    // BB for light and dark squares
    U64 LightSquares = 0x55AA55AA55AA55AAULL;
    U64 DarkSquares = 0xAA55AA55AA55AA55ULL;

    // split bishops and pawns by light and dark squares
    U64 lightBishops = bishops & LightSquares;
    U64 darkBishops = bishops & DarkSquares;
    U64 lightPawns = pawns & LightSquares;
    U64 darkPawns = pawns & DarkSquares;

    int badness = 0;

    // light-squared bishop blocked by light pawns is bad
    if (lightBishops)
        badness += PopCount(lightPawns);

    // dark-squared bishop blocked by dark pawns is bad
    if (darkBishops)
        badness += PopCount(darkPawns);

    return badness;
}

// count rooks on open/semi-open files
int RooksOnOpenFiles(U64 rooks)
{
    int count = 0;
    for (int f = 0; f < 8; f++)
    {
        U64 file = 0x0101010101010101ULL << f;       // current file
        if (!(file & (bitboards[P] | bitboards[p]))) // no pawns on this file?
            count += PopCount(rooks & file);         // count rooks here
    }
    return count;
}

// count rooks on the 7th rank
int RooksOn7th(U64 rooks, int color)
{
    int rank = (color == WHITE) ? 6 : 1;              // 7th rank for each color
    return PopCount(rooks & (0xFFULL << (rank * 8))); // count rooks there
}

// check if at least 2 rooks are on same file or rank
int ConnectedRooks(U64 rooks)
{
    if (PopCount(rooks) < 2)
        return 0;

    int sq1 = __builtin_ctzll(rooks); // first rook
    rooks &= rooks - 1;               // remove first rook
    int sq2 = __builtin_ctzll(rooks); // second rook

    // connected if same rank or same file
    return (sq1 / 8 == sq2 / 8 || sq1 % 8 == sq2 % 8);
}

// check if queen is developed early
int EarlyQueenDeveloped(int color)
{
    int sq = __builtin_ctzll(bitboards[color == WHITE ? Q : q]);
    return (color == WHITE) ? (sq < A2) : (sq > H7);
}

// king safety structure
int KingPawnShield(U64 king, U64 pawns, int color)
{
    if (!king)
        return 0;
    int sq = lsb(king);

    // KingShieldMask should contain the 3 squares in front of the king
    return PopCount(KingShieldMask[color][sq] & pawns);
}

// check how centralized king is
int KingCentralization(U64 king)
{
    int sq = __builtin_ctzll(king); // get king square
    int r = sq / 8, f = sq % 8;

    // dist from center (rank 3, file 3). closer is better
    return (4 - abs(3 - r)) + (4 - abs(3 - f));
}

// init masks for files, adjacent files, king shields, and passed pawns
void InitEvalMasks()
{
    // file masks and adj file masks
    for (int f = 0; f < 8; f++)
    {
        FileBBMask[f] = FILE_A << f; // shift A file to get B, C, etc.

        AdjacentFilesMask[f] = 0;
        if (f > 0)
            AdjacentFilesMask[f] |= FileBBMask[f - 1];
        if (f < 7)
            AdjacentFilesMask[f] |= FileBBMask[f + 1];
    }

    //  king shield masks
    for (int sq = 0; sq < 64; sq++)
    {
        int r = sq / 8, f = sq % 8;
        KingShieldMask[WHITE][sq] = 0;
        KingShieldMask[BLACK][sq] = 0;

        // white king shield
        if (r < 7)
        {
            KingShieldMask[WHITE][sq] |= (1ULL << ((r + 1) * 8 + f)); // front
            if (f > 0)
                KingShieldMask[WHITE][sq] |= (1ULL << ((r + 1) * 8 + (f - 1))); // front-left
            if (f < 7)
                KingShieldMask[WHITE][sq] |= (1ULL << ((r + 1) * 8 + (f + 1))); // front-right
        }

        // black king shield
        if (r > 0)
        {
            KingShieldMask[BLACK][sq] |= (1ULL << ((r - 1) * 8 + f)); // front
            if (f > 0)
                KingShieldMask[BLACK][sq] |= (1ULL << ((r - 1) * 8 + (f - 1))); // front-right
            if (f < 7)
                KingShieldMask[BLACK][sq] |= (1ULL << ((r - 1) * 8 + (f + 1))); // front-left
        }
    }

    //  passed pawn masks
    for (int sq = 0; sq < 64; sq++)
    {
        int r = sq / 8, f = sq % 8;
        PassedPawnMask[WHITE][sq] = 0;
        PassedPawnMask[BLACK][sq] = 0;

        // white: squares ahead on same & adjacent files
        for (int rr = r + 1; rr < 8; rr++)
        {
            PassedPawnMask[WHITE][sq] |= (1ULL << (rr * 8 + f));
            if (f > 0)
                PassedPawnMask[WHITE][sq] |= (1ULL << (rr * 8 + (f - 1)));
            if (f < 7)
                PassedPawnMask[WHITE][sq] |= (1ULL << (rr * 8 + (f + 1)));
        }

        // black: squares behind on same & adjacent files
        for (int rr = r - 1; rr >= 0; rr--)
        {
            PassedPawnMask[BLACK][sq] |= (1ULL << (rr * 8 + f));
            if (f > 0)
                PassedPawnMask[BLACK][sq] |= (1ULL << (rr * 8 + (f - 1)));
            if (f < 7)
                PassedPawnMask[BLACK][sq] |= (1ULL << (rr * 8 + (f + 1)));
        }
    }
}

char GetPromotionChar(int promotedPiece)
{
    if (promotedPiece == Q || promotedPiece == q)
        return 'q';
    if (promotedPiece == R || promotedPiece == r)
        return 'r';
    if (promotedPiece == B || promotedPiece == b)
        return 'b';
    if (promotedPiece == N || promotedPiece == n)
        return 'n';

    return ' ';
}