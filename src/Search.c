#include "../include/Search.h"

// global search state + killers
S_SEARCHINFO info;
Move killerMoves[MAX_PLY][2];

// current time in ms (used for soft time control)
int GetTimeMs() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec * 1000 + t.tv_usec / 1000;
}

// only check time every ~2k nodes to keep syscall overhead low
void CheckTime() {
    if (info.nodes % 2048 == 0) {
        if (GetTimeMs() > info.stopTime) {
            info.stopped = 1;
        }
    }
}

// reset all per-search state
void ClearSearch() {
    info.nodes = 0;
    info.stopped = 0;
    info.startTime = GetTimeMs();
    memset(killerMoves, 0, sizeof(killerMoves));
}

// main negamax alpha-beta
int AlphaBeta(int alpha, int beta, int depth) {

    // check time limits
    if ((info.nodes & 2047) == 0) 
        CheckTime();
    if (info.stopped) 
        return 0;

    // leaf -> quiescence search
    if (depth == 0) 
        return Quiescence(alpha, beta);

    info.nodes++;

    MoveList list;
    GenerateMoves(&list);

    int legalMoves = 0;
    int score = -INF;

    // grab killer moves for this ply
    Move k1 = {0}, k2 = {0};
    if (depth < MAX_PLY) 
    {
        k1 = killerMoves[depth][0];
        k2 = killerMoves[depth][1];
    }

    for (int i = 0; i < list.count; i++) 
    {
        PickNextMove(&list, i, k1, k2);
        Move m = list.moves[i];
        MakeMove(&list, i);

        // check for legality
        if (IsKingInCheck(side ^ 1)) {
            UndoMove(&list, i);
            continue;
        }

        legalMoves++;
        score = -AlphaBeta(-beta, -alpha, depth - 1);
        UndoMove(&list, i);

        if (info.stopped) return 0;

        // beta cutoff -> store quiet killer
        if (score >= beta) {
            if (m.captured == -1 && depth < MAX_PLY) {
                killerMoves[depth][1] = killerMoves[depth][0];
                killerMoves[depth][0] = m;
            }
            return beta;
        }

        if (score > alpha) alpha = score;
    }

    // check for mate or stalemate
    if (legalMoves == 0) {
        if (IsKingInCheck(side))
            return -MATE + (MAX_PLY - depth);
        else
            return 0;
    }

    return alpha;
}

// iterative deepening + root move selection
void SearchPosition(int maxDepth, int timeAllocatedMs) {

    ClearSearch();
    info.stopTime = info.startTime + timeAllocatedMs;

    Move bestMove = {0};
    int bestScore = 0;

    // deepen one ply at a time
    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++) {

        // don’t start a new depth if we’re already out of time
        if (GetTimeMs() >= info.stopTime) break;

        int alpha = -INF;
        int beta = INF;

        MoveList list;
        GenerateMoves(&list);

        Move depthBestMove = {0};
        int depthBestScore = -INF;

        // root loop 
        for (int i = 0; i < list.count; i++) {
            PickNextMove(&list, i, (Move){0}, (Move){0});
            MakeMove(&list, i);
            
            // check for legality
            if (IsKingInCheck(side ^ 1))
            {
                UndoMove(&list, i);
                continue;
            }

            int score = -AlphaBeta(-beta, -alpha, currentDepth - 1);
            UndoMove(&list, i);

            // depth incomplete -> throw it away
            if (info.stopped) break;

            if (score > depthBestScore) 
            {
                depthBestScore = score;
                depthBestMove = list.moves[i];
                alpha = score;
            }
        }

        if (info.stopped) break;

        // depth fully searched, update best move and score
        bestMove = depthBestMove;
        bestScore = depthBestScore;

        // uci info output
        long timeSpent = GetTimeMs() - info.startTime;
        if (timeSpent == 0) timeSpent = 1;

        printf(
            "info depth: %d score: %d CP nodes: %ld time: %ld nps: %ld ",
            currentDepth,
            bestScore,
            info.nodes,
            timeSpent,
            (info.nodes * 1000) / timeSpent
        );

        char f1 = 'a' + (bestMove.from % 8);
        char r1 = '1' + (bestMove.from / 8);
        char f2 = 'a' + (bestMove.to % 8);
        char r2 = '1' + (bestMove.to / 8);
        printf("pv %c%c%c%c\n", f1, r1, f2, r2);
    }

    // final uci bestmove
    char f1 = 'a' + (bestMove.from % 8);
    char r1 = '1' + (bestMove.from / 8);
    char f2 = 'a' + (bestMove.to % 8);
    char r2 = '1' + (bestMove.to / 8);
    printf("bestmove %c%c%c%c\n", f1, r1, f2, r2);
}

// quiescence search
int Quiescence(int alpha, int beta) {
    if ((info.nodes & 2047) == 0) CheckTime();
    if (info.stopped) return 0;

    info.nodes++;

    int score = Evaluate();

    if (score >= beta) return beta;
    if (score > alpha) alpha = score;

    MoveList list;
    GenerateMoves(&list);

    for (int i = 0; i < list.count; i++) {
        PickNextMove(&list, i, (Move){0}, (Move){0});
        Move m = list.moves[i];

        // only noisy moves
        if (m.captured == -1 && !(m.flags & FLAG_PROMOTION)) continue;

        MakeMove(&list, i);

        if (IsKingInCheck(side ^ 1)) {
            UndoMove(&list, i);
            continue;
        }

        int tempScore = -Quiescence(-beta, -alpha);
        UndoMove(&list, i);

        if (info.stopped) return 0;

        if (tempScore >= beta) return beta;
        if (tempScore > alpha) alpha = tempScore;
    }

    return alpha;
}
