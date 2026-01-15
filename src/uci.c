#include "../include/uci.h"

// parse go commands
void ParseGo(char *line) 
{
    int depth = -1;
    int movesToGo = 30;
    int moveTime = -1;
    int time = -1;
    int inc = 0;
    char *ptr = NULL;

    // check for infinite search
    if ((ptr = strstr(line, "infinite"))) 
    {
        //  set a huge time
        time = 99999999; 
    }

    // check for fixed depth
    if ((ptr = strstr(line, "depth"))) 
    {
        depth = atoi(ptr + 6);
    }

    // check for move time 
    if ((ptr = strstr(line, "movetime"))) 
    {
        moveTime = atoi(ptr + 9);
    }

    // check for game time 
    if ((ptr = strstr(line, "wtime")) && side == WHITE) 
    {
        time = atoi(ptr + 6);
    }

    if ((ptr = strstr(line, "btime")) && side == BLACK) 
    {
        time = atoi(ptr + 6);
    }

    // check for increment
    if ((ptr = strstr(line, "winc")) && side == WHITE) 
    {
        inc = atoi(ptr + 5);
    }
    if ((ptr = strstr(line, "binc")) && side == BLACK) 
    {
        inc = atoi(ptr + 5);
    }

    //  time management logic
    int timeToSpend = 0;

    if (moveTime != -1) 
    {
        timeToSpend = moveTime;
        if(timeToSpend > 10) timeToSpend -= 10; // safety buffer
    } 
    else if (time != -1) 
    {
        // simple management: divide remaining time by 20 moves + increment
        timeToSpend = (time / 20) + (inc / 2);
        
        // don't use too much time if we have a lot
        if (timeToSpend > 10000) timeToSpend = 10000; // cap at 10s for now to be safe
    } 
    else 
    {
        timeToSpend = 1000; // default 1 second if unknown
    }
    
    // if depth is fixed, use that. else use max depth
    if (depth == -1) depth = 64;

    printf("Thinking for %d ms...\n", timeToSpend);
    SearchPosition(depth, timeToSpend);
}

// parse position commands
void ParsePosition(char *lineIn) 
{
    lineIn += 9; // skip "position "
    char *ptrChar = lineIn;

    if (strncmp(lineIn, "startpos", 8) == 0) 
    {
        ParseFEN(starting_position);
    } 
    else if (strncmp(lineIn, "fen", 3) == 0) 
    {
        ptrChar += 4; // skip "fen "
        ParseFEN(ptrChar);
    }

    // parse "moves"
    ptrChar = strstr(lineIn, "moves");
    if (ptrChar != NULL) 
    {
        ptrChar += 6; // skip "moves "
        
        while(*ptrChar) 
        {
            MoveList list;
            GenerateMoves(&list);
            
            // parse algebraic move
            int from = (ptrChar[0] - 'a') + ((ptrChar[1] - '1') * 8);
            int to   = (ptrChar[2] - 'a') + ((ptrChar[3] - '1') * 8);
            
            // find the move in our list
            int moveFound = 0;

            for(int i = 0; i < list.count; i++) 
            {
                Move m = list.moves[i];
                if (m.from == from && m.to == to) 
                {
                    // check promotion
                    if (m.flags & FLAG_PROMOTION) 
                    {
                        // "a7a8q" -> 5th char is 'q'
                        char promChar = ptrChar[4];
                        int promPiece = 0;
                        if (promChar == 'q') promPiece = (side == WHITE) ? Q : q;
                        else if (promChar == 'r') promPiece = (side == WHITE) ? R : r;
                        else if (promChar == 'b') promPiece = (side == WHITE) ? B : b;
                        else if (promChar == 'n') promPiece = (side == WHITE) ? N : n;
                    }
                    
                    MakeMove(&list, i);
                    moveFound = 1;
                    break;
                }
            }
            
            // move to next move in string
            while(*ptrChar && *ptrChar != ' ') ptrChar++;
            ptrChar++;
        }
    }
}

void UciLoop() 
{
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    char line[INPUT_BUFFER];

    while (1) {
        memset(line, 0, sizeof(line));
        
        if (!fgets(line, INPUT_BUFFER, stdin))
            continue;

        if (line[0] == '\n')
            continue;

        if (strncmp(line, "isready", 7) == 0) 
        {
            printf("readyok\n");
            continue;
        } 
        else if (strncmp(line, "position", 8) == 0) 
        {
            ParsePosition(line);
        } 
        else if (strncmp(line, "ucinewgame", 10) == 0) 
        {
            ParsePosition("position startpos\n");
        } 
        else if (strncmp(line, "go", 2) == 0) 
        {
            ParseGo(line);
        } 
        else if (strncmp(line, "quit", 4) == 0) 
        {
            break;
        } 
        else if (strncmp(line, "uci", 3) == 0) 
        {
            printf("id name Viaje 1.0\n");
            printf("id author Brandon Viaje\n");
            printf("uciok\n");
        }
        else if (strncmp(line, "printboard", 3) == 0) 
        {
            PrintBitBoard(occupied);
        }
    }
}
