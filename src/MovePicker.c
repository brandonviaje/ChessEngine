#include "../include/MovePicker.h"

// mvv-lva table, attacker vs victim priority for captures
const int mvv_lva[6][6] = {
    {105, 205, 305, 405, 505, 605},
    {104, 204, 304, 404, 504, 604},
    {103, 203, 303, 403, 503, 603},
    {102, 202, 302, 402, 502, 602},
    {101, 201, 301, 401, 501, 601},
    {100, 200, 300, 400, 500, 600}};

// normalize piece indices for table lookup
int GetPieceIndex(int piece)
{
    return piece % 6;
}

// check if two moves are the same, used for killers
int SameMove(Move a, Move b)
{
    return (a.from == b.from && a.to == b.to);
}

// score a move for move ordering, accepts killers
int ScoreMove(Move m, Move k1, Move k2)
{
    // captures first, always fire high
    if (m.captured != -1)
    {
        int victim = GetPieceIndex(m.captured);
        int attacker = GetPieceIndex(m.piece);
        return 10000 + mvv_lva[attacker][victim];
    }

    // promotions next, still juicy
    if (m.flags & FLAG_PROMOTION)
        return 9000;

    // killer moves matter too, give them priority
    if (SameMove(m, k1))
        return 8000;
    if (SameMove(m, k2))
        return 7000;

    // quiet moves get nothing special
    return 0;
}

// pick next best move to try, based on scoring
void PickNextMove(MoveList *list, int moveNum, Move k1, Move k2)
{
    int bestScore = -1;
    int bestIndex = moveNum;

    for (int i = moveNum; i < list->count; i++)
    {
        int score = ScoreMove(list->moves[i], k1, k2);

        if (score > bestScore)
        {
            bestScore = score;
            bestIndex = i;
        }
    }

    // swap the best move up, so next search tries it first
    if (bestIndex != moveNum)
    {
        Move temp = list->moves[moveNum];
        list->moves[moveNum] = list->moves[bestIndex];
        list->moves[bestIndex] = temp;
    }
}
