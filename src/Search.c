#include "../include/Search.h"

// search context & heuristics
S_SEARCHINFO info;
Move killerMoves[MAX_PLY][2];
Move pvTable[MAX_PLY][MAX_PLY];
int pvLength[MAX_PLY];

// linux timer wrapper
int GetTimeMs()
{
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

// poll system time, minimize syscalls
void CheckTime()
{
    if (info.nodes % 2048 == 0)
    {
        if (GetTimeMs() > info.stopTime)
        {
            info.stopped = 1;
        }
    }
}

// reset per-search state
void ClearSearch()
{
    info.nodes = 0;
    info.stopped = 0;
    info.startTime = GetTimeMs();
    memset(killerMoves, 0, sizeof(killerMoves));
}

// negamax alpha-beta
int AlphaBeta(int alpha, int beta, int depth, int ply)
{
    // stack overflow guard
    if (ply >= MAX_PLY)
        return Evaluate();

    if ((info.nodes & 2047) == 0)
        CheckTime();

    if (info.stopped)
        return 0;

    // tt probe
    int score = -INF;
    int ttMovePacked = 0;
    int ttScore = -INF;

    if (ReadTT(positionKey, &ttMovePacked, &ttScore, alpha, beta, depth))
    {
        return ttScore;
    }

    // horizon reached -> q-search
    if (depth == 0)
        return Quiescence(alpha, beta, ply);

    info.nodes++;

    MoveList list;
    GenerateMoves(&list);

    int legalMoves = 0;
    int alphaOrig = alpha;
    Move bestMove = {0};

    // retrieve killers for ordering
    Move k1 = killerMoves[ply][0];
    Move k2 = killerMoves[ply][1];

    for (int i = 0; i < list.count; i++)
    {
        PickNextMove(&list, i, k1, k2, ttMovePacked);

        Move m = list.moves[i];
        MakeMove(&list, i);

        if (IsKingInCheck(side ^ 1))
        {
            UndoMove(&list, i);
            continue;
        }

        legalMoves++;
        score = -AlphaBeta(-beta, -alpha, depth - 1, ply + 1);
        UndoMove(&list, i);

        if (info.stopped)
            return 0;

        // fail-high (beta cutoff)
        if (score >= beta)
        {
            // update killers if quiet move caused cutoff
            if (m.captured == -1)
            {
                killerMoves[ply][1] = killerMoves[ply][0];
                killerMoves[ply][0] = m;
            }

            // tt store: lower bound
            int packed = m.from | (m.to << 6);
            WriteTT(positionKey, packed, beta, depth, TT_FLAG_BETA);

            return beta;
        }

        // pv update
        if (score > alpha)
        {
            alpha = score;
            bestMove = m;
        }
    }

    // mate / stalemate detection
    if (legalMoves == 0)
    {
        if (IsKingInCheck(side))
            return -MATE + ply; // prefer shorter mates
        else
            return 0;
    }

    // tt store: exact or upper bound
    int packedBest = bestMove.from | (bestMove.to << 6);

    if (alpha > alphaOrig)
    {
        WriteTT(positionKey, packedBest, alpha, depth, TT_FLAG_EXACT);
    }
    else
    {
        WriteTT(positionKey, packedBest, alpha, depth, TT_FLAG_ALPHA);
    }

    return alpha;
}

// iterative deepening driver
void SearchPosition(int maxDepth, int timeAllocatedMs)
{

    ClearSearch();
    info.stopTime = info.startTime + timeAllocatedMs;

    Move bestMove = {0};
    int bestScore = 0;

    // id loop
    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++)
    {

        // time check before next iteration
        if (GetTimeMs() >= info.stopTime)
            break;

        int alpha = -INF;
        int beta = INF;

        MoveList list;
        GenerateMoves(&list);

        Move depthBestMove = {0};
        int depthBestScore = -INF;

        // root move loop
        for (int i = 0; i < list.count; i++)
        {
            PickNextMove(&list, i, (Move){0}, (Move){0}, 0);
            MakeMove(&list, i);

            // legality check
            if (IsKingInCheck(side ^ 1))
            {
                UndoMove(&list, i);
                continue;
            }

            int score = -AlphaBeta(-beta, -alpha, currentDepth - 1, 1);
            UndoMove(&list, i);

            // discard incomplete results
            if (info.stopped)
                break;

            if (score > depthBestScore)
            {
                depthBestScore = score;
                depthBestMove = list.moves[i];
                alpha = score;
            }
        }

        if (info.stopped)
            break;

        bestMove = depthBestMove;
        bestScore = depthBestScore;

        // uci output
        long timeSpent = GetTimeMs() - info.startTime;
        if (timeSpent == 0)
            timeSpent = 1;

        printf(
            "info depth: %d score: %d CP nodes: %ld time: %ld nps: %ld ",
            currentDepth,
            bestScore,
            info.nodes,
            timeSpent,
            (info.nodes * 1000) / timeSpent);

        if (bestMove.flags & FLAG_PROMOTION)
        {
            printf("%c", GetPromotionChar(bestMove.promotion));
        }

        char f1 = 'a' + (bestMove.from % 8);
        char r1 = '1' + (bestMove.from / 8);
        char f2 = 'a' + (bestMove.to % 8);
        char r2 = '1' + (bestMove.to / 8);
        printf("pv %c%c%c%c\n", f1, r1, f2, r2);
    }

    char f1 = 'a' + (bestMove.from % 8);
    char r1 = '1' + (bestMove.from / 8);
    char f2 = 'a' + (bestMove.to % 8);
    char r2 = '1' + (bestMove.to / 8);
    printf("bestmove %c%c%c%c\n", f1, r1, f2, r2);
}

// quiescence search (resolve noisy moves)
int Quiescence(int alpha, int beta, int ply)
{
    if (ply >= MAX_PLY)
        return Evaluate();

    if ((info.nodes & 2047) == 0)
        CheckTime();

    if (info.stopped)
        return 0;

    info.nodes++;

    // stand-pat score
    int score = Evaluate();

    if (score >= beta)
        return beta;
    if (score > alpha)
        alpha = score;

    MoveList list;
    GenerateMoves(&list);

    for (int i = 0; i < list.count; i++)
    {
        // no killers in q-search
        PickNextMove(&list, i, (Move){0}, (Move){0}, 0);
        Move m = list.moves[i];

        // filter quiet moves
        if (m.captured == -1 && !(m.flags & FLAG_PROMOTION))
            continue;

        MakeMove(&list, i);

        if (IsKingInCheck(side ^ 1))
        {
            UndoMove(&list, i);
            continue;
        }

        int tempScore = -Quiescence(-beta, -alpha, ply + 1);
        UndoMove(&list, i);

        if (info.stopped)
            return 0;

        if (tempScore >= beta)
            return beta;
        if (tempScore > alpha)
            alpha = tempScore;
    }

    return alpha;
}
