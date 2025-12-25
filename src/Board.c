#include "../include/Board.h"

// Bitboards
U64 bitboards[12];
U64 whitePieces;
U64 blackPieces;
U64 occupied;

// Game states
int side; // 0 = white, 1 = black
int enpassant;
unsigned char castle;
int halfmove;
int fullmove;

// Initialize  bitboards to 0s
void ResetBoardState()
{
    // Reset game state variables
    side = -1;
    enpassant = -1;
    castle = 0; // set default to no castle rights (if it is a '-')
    halfmove = 0;
    fullmove = 0;

    // Initialize bitboards for each chess piece
    for (int i = 0; i < 12; i++)
    {
        bitboards[i] = 0uLL;
    }

    // Initialize misc. bitboards to 0s
    whitePieces = 0uLL;
    blackPieces = 0uLL;
    occupied = 0uLL;
}

int CharToPiece(char c)
{
    switch (c)
    {
    case 'P':
        return P;
    case 'N':
        return N;
    case 'B':
        return B;
    case 'R':
        return R;
    case 'Q':
        return Q;
    case 'K':
        return K;
    case 'p':
        return p;
    case 'n':
        return n;
    case 'b':
        return b;
    case 'r':
        return r;
    case 'q':
        return q;
    case 'k':
        return k;
    default:
        return -1; // Did not find
    }
}

// Set Piece on square
void SetPiece(int piece, int square)
{
    SetBit(bitboards[piece], square);

    // update white or black bitboard
    if (piece <= K)
        SetBit(whitePieces, square);
    else
        SetBit(blackPieces, square);

    SetBit(occupied, square); // update occupied bitboard
}

void PrintBitBoard(U64 board)
{
    // Print ranks from 8 to 1
    for (int rank = 7; rank >= 0; rank--)
    {
        printf("%d  ", rank + 1); // Rank label on the left
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            printf(" %c", GetBit(board, square) ? '1' : '.');
        }
        printf("\n");
    }
    printf("\n    a b c d e f g h\n");
}

// Parse FEN String to capture the current game state
void ParseFEN(char *FEN)
{
    // Parse FEN into 6 fields
    char *fields[6];
    int i = 0;
    char *fen_copy = strdup(FEN);
    char *token = strtok(fen_copy, " ");

    while (token != NULL && i < 6)
    {
        fields[i++] = token;
        token = strtok(NULL, " ");
    }

    // Catch invalid FEN notation
    if (i < 6)
    {
        fprintf(stderr, "%s", "Error: Invalid FEN (missing fields)\n");
        free(fen_copy);
        exit(EXIT_FAILURE);
    }

    // Reset bitboards and game states
    ResetBoardState();

    // Initialize Board Layout
    int rank = 7;
    int file = 0;

    for (int i = 0; i < strlen(fields[0]); i++)
    {
        // Check if you reach a delimiter
        if (fields[0][i] == '/')
        {
            rank--;
            file = 0;
            continue;
        }

        // If you get an int n skip n squares
        if (isdigit(fields[0][i]))
        {
            file += fields[0][i] - '0';
            continue;
        }

        // Update corresponding bitboard
        int piece = CharToPiece(fields[0][i]);

        if (piece == -1)
        {
            fprintf(stderr, "Error: Invalid FEN Notation!\n");
            exit(EXIT_FAILURE);
        }

        // Calculate square index of the bitboard
        int square = rank * 8 + file;
        SetPiece(piece, square);

        file++; // increment file
    }

    side = (fields[1][0] == 'w') ? WHITE : BLACK; // Current side

    // Castling rights
    for (int j = 0; j < strlen(fields[2]); j++)
    {
        // determine current castling rights
        switch (fields[2][j])
        {
        case 'K':
            castle |= WHITE_CASTLE_K;
            break;
        case 'Q':
            castle |= WHITE_CASTLE_Q;
            break;
        case 'k':
            castle |= BLACK_CASTLE_K;
            break;
        case 'q':
            castle |= BLACK_CASTLE_Q;
            break;
        }
    }

    // En passant
    if (fields[3][0] == '-')
        enpassant = -1;
    else
    {
        int file_ep = fields[3][0] - 'a';
        int rank_ep = fields[3][1] - '1';
        enpassant = rank_ep * 8 + file_ep;
    }

    halfmove = atoi(fields[4]); // Halfmove clock
    fullmove = atoi(fields[5]); // Fullmove number
    free(fen_copy);             // dealloc memory
}

void BitboardsToVector(U64 bitboards[12], float x[768]) 
{
    for (int i = 0; i < 768; i++) x[i] = 0.0f;

    for (int piece = 0; piece < 12; piece++) 
    {
        // get bitboard for this piece
        U64 bb = bitboards[piece];

        // process all bits in bb
        while (bb) 
        {
            int sq = __builtin_ctzll(bb); // index of least-significant 1
            bb &= bb - 1;                 // clear that bit
            int idx = piece*64 + sq;
            x[idx] = 1.0f;
        }
    }
}

// Make Move Function
void MakeMove(MoveList *list, int index)
{
    // Catch move index out of bounds
    if (index < 0 || index >= MAX_MOVES)
    {
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = list->moves[index].piece;
    int from = list->moves[index].from;
    int to = list->moves[index].to;
    int captured = list->moves[index].captured;
    int promotedPiece = list->moves[index].promotion;
    int moveFlag = list->moves[index].flags;

    // Catch square index out of bounds
    if (from < 0 || from > 63 || to < 0 || to > 63)
    {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", from, to);
        exit(EXIT_FAILURE);
    }

    // Create bit mask of from and to square
    U64 fromMask = 1ULL << from;
    U64 toMask = 1ULL << to;
    list->moves[index].prevCastle = castle;
    list->moves[index].prevEnpassant = enpassant;
    enpassant = -1;

    switch (moveFlag)
    {
    case FLAG_CASTLE_KINGSIDE:
    {

        MovePiece(piece, fromMask, toMask); // move piecec from -> to
        U64 fromRook, toRook;               // mask for a specific rooks position, mask to place the rook at that position

        // kingside castle white
        if (side == WHITE)
        {
            fromRook = 1ULL << 7;           // h1 rook
            toRook = 1ULL << (to - 1);      // f1
            MovePiece(R, fromRook, toRook); // move rook from -> to
        }
        else
        {                                   // kingside castle black
            fromRook = 1ULL << 63;          // h8 rook
            toRook = 1ULL << (to - 1);      // f8
            MovePiece(r, fromRook, toRook); // move rook from -> to
        }
        break;
    }
    case FLAG_CASTLE_QUEENSIDE:
    {
        MovePiece(piece, fromMask, toMask);
        U64 fromRook, toRook; // mask for a from position and to position for rooks

        // Update corresponding bitboards
        if (side == WHITE)
        {
            fromRook = 1ULL << 0;           // a1 rook
            toRook = 1ULL << (to + 1);      // d1
            MovePiece(R, fromRook, toRook); // move rook from -> to
        }
        else
        {
            fromRook = 1ULL << 56;          // a8 rook
            toRook = 1ULL << (to + 1);      // d8
            MovePiece(r, fromRook, toRook); // move rook from -> to
        }
        break;
    }

    case FLAG_DOUBLE_PUSH:
    {
        MovePiece(piece, fromMask, toMask);
        enpassant = side == WHITE ? to - 8 : to + 8; // set enpassant square
        break;
    }

    case FLAG_ENPASSANT:
    {

        MovePiece(piece, fromMask, toMask);

        // remove captured piece from its bitboard: behind the pawn that just moved
        int capturedSquare = side == WHITE ? to - 8 : to + 8;
        U64 capturedMask = 1ULL << capturedSquare;
        int capPawn = (side == WHITE) ? p : P;
        bitboards[capPawn] &= ~capturedMask;

        // Update corresponding bitboards
        if (side == WHITE)
            blackPieces &= ~capturedMask;
        else
            whitePieces &= ~capturedMask;

        occupied &= ~capturedMask;
        break;
    }

    case FLAG_PROMOTION:
    {

        // Check if the move made is a capture
        if (captured != -1)
        {
            // remove captured piece from its bitboard
            bitboards[captured] &= ~toMask;

            // Update corresponding captured pieces bitboard
            if (captured <= K)
            {
                whitePieces &= ~toMask;
            }
            else
            {
                blackPieces &= ~toMask;
            }
            occupied &= ~toMask;
        }
        // remove pawn from from-square
        bitboards[piece] &= ~fromMask;
        occupied &= ~fromMask;

        if (piece <= K)
            whitePieces &= ~fromMask;
        else
            blackPieces &= ~fromMask;

        // place promoted piece
        RestorePiece(promotedPiece, toMask);
        break;
    }

    default:
    {

        // check if move made is a capture
        if (captured != -1)
        {
            // remove captured piece from its bitboard
            bitboards[captured] &= ~toMask;

            // Update corresponding captured pieces bitboard
            if (captured <= K)
            {
                whitePieces &= ~toMask;
            }
            else
            {
                blackPieces &= ~toMask;
            }
            occupied &= ~toMask;
        }

        MovePiece(piece, fromMask, toMask);
        break;
    }
    }
    UpdateCastlingRights(piece, from, to, captured);
    side ^= 1; // flip sides
}

// Undo Move Function
void UndoMove(MoveList *list, int index)
{
    // Catch move index out of bounds
    if (index < 0 || index >= MAX_MOVES)
    {
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = list->moves[index].piece;
    int from = list->moves[index].from;
    int to = list->moves[index].to;
    int captured = list->moves[index].captured;
    int promotedPiece = list->moves[index].promotion;
    int moveFlag = list->moves[index].flags;

    // get prev side
    int prevSide = side ^ 1;

    // Catch square index out of bounds
    if (from < 0 || from >= 64 || to < 0 || to >= 64)
    {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", from, to);
        exit(EXIT_FAILURE);
    }

    // bitmasks of from and to squares
    U64 fromMask = 1ULL << from;
    U64 toMask = 1ULL << to;
    enpassant = list->moves[index].prevEnpassant;
    castle = list->moves[index].prevCastle;

    switch (moveFlag)
    {
    case FLAG_CASTLE_KINGSIDE:
    {
        MovePiece(piece, toMask, fromMask); // move king back
        U64 fromRook, toRook;

        if (prevSide == WHITE)
        {                              // undoing White's move
            fromRook = 1ULL << 7;      // h1 rook
            toRook = 1ULL << (to - 1); // f1
            MovePiece(R, toRook, fromRook);
        }
        else
        {                              // undoing Black's move
            fromRook = 1ULL << 63;     // h8 rook
            toRook = 1ULL << (to - 1); // f8
            MovePiece(r, toRook, fromRook);
        }
        break;
    }

    case FLAG_CASTLE_QUEENSIDE:
    {
        MovePiece(piece, toMask, fromMask); // Move king back
        U64 fromRook, toRook;

        if (prevSide == WHITE)
        {                              // undoing White's move
            fromRook = 1ULL << 0;      // a1 rook
            toRook = 1ULL << (to + 1); // d1
            MovePiece(R, toRook, fromRook);
        }
        else
        {                              // undoing Black's move
            fromRook = 1ULL << 56;     // a8 rook
            toRook = 1ULL << (to + 1); // d8
            MovePiece(r, toRook, fromRook);
        }
        break;
    }

    case FLAG_DOUBLE_PUSH:
    {
        MovePiece(piece, toMask, fromMask); // move piece back
        enpassant = list->moves[index].prevEnpassant;
        break;
    }

    case FLAG_ENPASSANT:
    {
        MovePiece(piece, toMask, fromMask);                       // Move pawn back
        int capturedSquare = prevSide == WHITE ? to - 8 : to + 8; // get captured square
        U64 capturedMask = 1ULL << capturedSquare;
        int capPawn = (prevSide == WHITE) ? p : P;
        RestorePiece(capPawn, capturedMask);
        break;
    }

    case FLAG_PROMOTION:
    {
        bitboards[promotedPiece] &= ~toMask; // remove promoted piece
        occupied &= ~toMask;

        if (promotedPiece <= K)
            whitePieces &= ~toMask;
        else
            blackPieces &= ~toMask;

        RestorePiece(piece, fromMask); // restore pawn

        if (captured != -1)
            RestorePiece(captured, toMask); // restore captured piece
        break;
    }

    default:
    {
        MovePiece(piece, toMask, fromMask); // move piece back
        if (captured != -1)
            RestorePiece(captured, toMask); // restore captured piece
        break;
    }
    }
    side ^= 1; // flip side
}

// Move a piece from one square to another
void MovePiece(int piece, U64 fromMask, U64 toMask)
{
    bitboards[piece] &= ~fromMask; // clear piece from old square
    bitboards[piece] |= toMask;    // set to new square

    // set corresponding bitboards
    if (piece <= K)
    { // white piece
        whitePieces &= ~fromMask;
        whitePieces |= toMask;
    }
    else
    { // black piece
        blackPieces &= ~fromMask;
        blackPieces |= toMask;
    }

    occupied &= ~fromMask;
    occupied |= toMask;
}

// restore a piece to a square
void RestorePiece(int piece, U64 mask)
{
    bitboards[piece] |= mask; // set bitboard using that mask

    // set corresponding bitboards
    if (piece <= K)
        whitePieces |= mask;
    else
        blackPieces |= mask;
    occupied |= mask;
}

// update castling rights after a move
void UpdateCastlingRights(int piece, int from, int to, int captured)
{
    // king moves remove both castling rights for that side
    if (piece == K)
        castle &= ~(WHITE_CASTLE_K | WHITE_CASTLE_Q);
    else if (piece == k)
        castle &= ~(BLACK_CASTLE_K | BLACK_CASTLE_Q);

    // rook moves
    if (piece == R)
    {
        if (from == 0)
            castle &= ~WHITE_CASTLE_Q;
        else if (from == 7)
            castle &= ~WHITE_CASTLE_K;
    }
    else if (piece == r)
    {
        if (from == 56)
            castle &= ~BLACK_CASTLE_Q;
        else if (from == 63)
            castle &= ~BLACK_CASTLE_K;
    }

    // rook captures
    if (captured == R)
    {
        if (to == 0)
            castle &= ~WHITE_CASTLE_Q;
        else if (to == 7)
            castle &= ~WHITE_CASTLE_K;
    }
    else if (captured == r)
    {
        if (to == 56)
            castle &= ~BLACK_CASTLE_Q;
        else if (to == 63)
            castle &= ~BLACK_CASTLE_K;
    }
}
