#include "BitBoard.h"

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
void ResetBoardState(){
    // Reset game state variables
    side = -1;
    enpassant = -1;
    castle = 0; // set default to no castle rights (if it is a '-')
    halfmove = 0;
    fullmove = 0;
    
    // Initialize bitboards for each chess piece
    for(int i = 0; i < 12; i++){
        bitboards[i] = 0uLL;
    }

    // Initialize misc. bitboards to 0s
    whitePieces = 0uLL;
    blackPieces = 0uLL;
    occupied = 0uLL;
}

int CharToPiece(char c){
    switch(c){
        case 'P': return P;
        case 'N': return N;
        case 'B': return B;
        case 'R': return R;
        case 'Q': return Q;
        case 'K': return K;
        case 'p': return p;
        case 'n': return n;
        case 'b': return b;
        case 'r': return r;
        case 'q': return q;
        case 'k': return k;
        default: return -1; // Did not find
    }
}

// Set Piece on square
void SetPiece(int piece, int square) {
    bitboards[piece] |= 1ULL << square;      

    // update white or black bitboard
    if (piece <= K) whitePieces |= 1ULL << square;
    else  blackPieces |= 1ULL << square;

    occupied |= 1ULL << square; // update occupied bitboard
}

void PrintBitboard(U64 board) {
    printf("\n");
    // Print ranks from 8 to 1
    for (int rank = 7; rank >= 0; rank--) {
        printf("%d  ", rank + 1); // Rank label on the left
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;
            if (board & (1ULL << square))
                printf("1 ");
            else
                printf(". ");
        }
        printf("\n");
    }

    // Print file labels at the bottom
    printf("\n   ");
    for (int file = 0; file < 8; file++)
        printf("%c ", 'a' + file);
    printf("\n\n");
}

// Parse FEN String to capture the current game state
void ParseFEN(char * FEN){
    // Parse FEN into 6 fields
    char *fields[6];
    int i = 0;
    char *fen_copy = strdup(FEN);
    char *token = strtok(fen_copy, " ");

    while(token != NULL && i < 6){
        fields[i++] = token;
        token = strtok(NULL, " ");
    }

    // Catch invalid FEN notation
    if(i < 6){
        fprintf(stderr, "%s", "Error: Invalid FEN (missing fields)\n");
        free(fen_copy);
        exit(EXIT_FAILURE);
    }

    // Reset bitboards and game states
    ResetBoardState();

    // Initialize Board Layout
    int rank = 7;
    int file = 0;

    for(int i = 0; i < strlen(fields[0]); i++){
        // Check if you reach a delimiter
        if(fields[0][i] == '/'){
            rank--;
            file = 0;
            continue;
        }
        
        // If you get an int n skip n squares 
        if(isdigit(fields[0][i])){
            file += fields[0][i] -'0';
            continue;
        }

        // Update corresponding bitboard
        int piece = CharToPiece(fields[0][i]);

        if(piece == -1){
            fprintf(stderr, "Error: Invalid FEN Notation!\n");
            exit(EXIT_FAILURE);
        }

        // Calculate square index of the bitboard
        int square = rank * 8 + file;
        SetPiece(piece,square);

        file++; // increment file
    }

    side = (fields[1][0] == 'w') ? WHITE : BLACK; // Current side

    // Castling rights
    for (int j = 0; j < strlen(fields[2]); j++) {
        // bitmask to determine current castling rights
        switch(fields[2][j]) {
            case 'K': castle |= 1 << 0; break; // White kingside
            case 'Q': castle |= 1 << 1; break; // White queenside
            case 'k': castle |= 1 << 2; break; // Black kingside
            case 'q': castle |= 1 << 3; break; // Black queenside
        }
    }

    // En passant 
    if (fields[3][0] == '-') enpassant = -1;
    else {
        int file_ep = fields[3][0] - 'a';
        int rank_ep = fields[3][1] - '1';
        enpassant = rank_ep * 8 + file_ep;
    }

    halfmove = atoi(fields[4]); // Halfmove clock
    fullmove = atoi(fields[5]); // Fullmove number 
    free(fen_copy); // dealloc memory
}

// Make Move Function
void MakeMove(int index){
    // Catch move index out of bounds
    if (index < 0 || index >= MAX_MOVES){
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = moveList[index].piece;
    int from = moveList[index].from;
    int to = moveList[index].to;
    int captured = moveList[index].captured;
    int promotedPiece = moveList[index].promotion;
    int moveFlag = moveList[index].flags;

    // Catch square index out of bounds
    if (from < 0 || from >= 64 || to < 0 || to >= 64) {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", from, to);
        exit(EXIT_FAILURE);
    }

    // Create bit mask of from and to square
    U64 fromMask = 1ULL << from;
    U64 toMask = 1ULL << to;

    switch (moveFlag){
        case FLAG_CASTLE_KINGSIDE: {

            bitboards[piece] ^= fromMask;  // remove piece from bitboard
            bitboards[piece] ^= toMask;    // move piece on bitboard    
            U64 rookMask, moveRookMask;     // mask for a specific rooks position, mask to place the rook at that position

            // kingside castle white
            if(side == WHITE){
                rookMask = 1ULL << 7;            // h1 rook
                moveRookMask = 1ULL << (to - 1); // f1
                bitboards[R] ^= rookMask;
                bitboards[R] ^= moveRookMask;
                whitePieces ^= rookMask;
                whitePieces ^= moveRookMask;
            }else{ // kingside castle black
                rookMask = 1ULL << 63;           // h8 rook
                moveRookMask = 1ULL << (to - 1); // f8
                bitboards[r] ^= rookMask;
                bitboards[r] ^= moveRookMask;
                blackPieces ^= rookMask;
                blackPieces ^= moveRookMask;
            }

            // update king piece
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            }else{
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            occupied &= ~rookMask;
            occupied |= moveRookMask;
            break;
        }
        case FLAG_CASTLE_QUEENSIDE: {

            bitboards[piece] ^= fromMask;   // remove piece from bitboard
            bitboards[piece] ^= toMask;     // move piece on bitboard  
            U64 rookMask, moveRookMask;     // mask for a specific rooks position, mask to place the rook at that position
            
            // Update corresponding bitboards
            if(side == WHITE){
                rookMask = 1ULL << 0;            // a1 rook
                moveRookMask = 1ULL << (to + 1); // d1
                bitboards[R] ^= rookMask;
                bitboards[R] ^= moveRookMask;
                whitePieces ^= rookMask;
                whitePieces ^= moveRookMask;
            }else{
                rookMask = 1ULL << 56;           // a8 rook
                moveRookMask = 1ULL << (to + 1); // d8
                bitboards[r] ^= rookMask;
                bitboards[r] ^= moveRookMask;
                blackPieces ^= rookMask;
                blackPieces ^= moveRookMask;
            }

            // update king piece
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            }else{
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            occupied &= ~rookMask;
            occupied |= moveRookMask;
            break;
        }
        case FLAG_ENPASSANT: {
            bitboards[piece] ^= fromMask; // remove piece using fromMask
            bitboards[piece] ^= toMask;  // add piece using toMask

            // Update the corresponding color bitboard
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            }else{
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // remove captured piece from its bitboard: behind the pawn that it just went to
            U64 capturedSquare = side == WHITE ? to - 8 : to + 8;
            U64 capturedMask = 1ULL << capturedSquare;
            bitboards[captured] ^= capturedMask;

            // Update corresponding bitboards
            if (captured <= K){
                whitePieces ^= capturedMask;
            }else{
                blackPieces ^= capturedMask;
            }

            occupied ^= capturedMask;
            break;
        }

        case FLAG_PROMOTION: {
            bitboards[piece] ^= fromMask; // remove piece using fromMask
            bitboards[promotedPiece] ^= toMask; // add promoted piece using toMasks

            // Update the corresponding color bitboard
            if (promotedPiece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            }else{
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // Check if the move made is a capture
            if(captured != -1){
                // remove captured piece from its bitboard
                bitboards[captured] ^= toMask;

                // Update corresponding captured pieces bitboard
                if (captured <= K){
                    whitePieces ^= toMask;
                }else{
                    blackPieces ^= toMask;
                }
            }
            break;
        }

        default: {
            bitboards[piece] ^= fromMask; // remove piece using fromMask
            bitboards[piece] ^= toMask;  // add piece using toMask

            // update the corresponding color bitboard
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            }else{
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // check if move made is a capture
            if(captured != -1){
                // remove captured piece from its bitboard
                bitboards[captured] ^= toMask;

                // Update corresponding captured pieces bitboard
                if (captured <= K){
                    whitePieces ^= toMask;
                }else{
                    blackPieces ^= toMask;
                }
            }
            break;
        }
    }

    // update occupied bitboard, from square gets cleared, piece gets added
    occupied &= ~fromMask;
    occupied |= toMask;
    side ^= 1;
}

// Undo Move Function
void UndoMove(int index) {
    // Catch move index out of bounds
    if (index < 0 || index >= MAX_MOVES) {
        fprintf(stderr, "%s", "Error: Invalid Move Index\n");
        exit(EXIT_FAILURE);
    }

    int piece = moveList[index].piece;
    int from = moveList[index].from;
    int to = moveList[index].to;
    int captured = moveList[index].captured;
    int promotedPiece = moveList[index].promotion;
    int moveFlag = moveList[index].flags;

     // Catch square index out of bounds
    if (from < 0 || from >= 64 || to < 0 || to >= 64) {
        fprintf(stderr, "Error: Invalid square index (%d -> %d)\n", from, to);
        exit(EXIT_FAILURE);
    }

    // bitmasks of from and to squares
    U64 fromMask = 1ULL << from;
    U64 toMask = 1ULL << to;

    switch (moveFlag) {
        case FLAG_CASTLE_KINGSIDE: {
            // move king back
            bitboards[piece] ^= toMask;
            bitboards[piece] ^= fromMask;

            U64 rookMask, moveRookMask;
            if (side == BLACK) { // undoing White's move
                rookMask = 1ULL << 7;           // h1 rook
                moveRookMask = 1ULL << (to - 1); // f1
                bitboards[R] ^= moveRookMask;
                bitboards[R] ^= rookMask;
                whitePieces ^= moveRookMask;
                whitePieces ^= rookMask;
            } else { // undoing Black's move
                rookMask = 1ULL << 63;          // h8 rook
                moveRookMask = 1ULL << (to - 1); // f8
                bitboards[r] ^= moveRookMask;
                bitboards[r] ^= rookMask;
                blackPieces ^= moveRookMask;
                blackPieces ^= rookMask;
            }

            // Restore king's color bitboard
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            } else {
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // Update occupied
            occupied &= ~moveRookMask;
            occupied |= rookMask;
            break;
        }

        case FLAG_CASTLE_QUEENSIDE: {
            // Move king back
            bitboards[piece] ^= toMask;
            bitboards[piece] ^= fromMask;

            U64 rookMask, moveRookMask;
            if (side == BLACK) { // undoing White's move
                rookMask = 1ULL << 0;           // a1 rook
                moveRookMask = 1ULL << (to + 1); // d1
                bitboards[R] ^= moveRookMask;
                bitboards[R] ^= rookMask;
                whitePieces ^= moveRookMask;
                whitePieces ^= rookMask;
            } else { // undoing Black's move
                rookMask = 1ULL << 56;           // a8 rook
                moveRookMask = 1ULL << (to + 1); // d8
                bitboards[r] ^= moveRookMask;
                bitboards[r] ^= rookMask;
                blackPieces ^= moveRookMask;
                blackPieces ^= rookMask;
            }

            // undo king's color bitboard
            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            } else {
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // Update occupied
            occupied &= ~moveRookMask;
            occupied |= rookMask;
            break;
        }

        case FLAG_ENPASSANT: {
            // Move pawn back
            bitboards[piece] ^= toMask;
            bitboards[piece] ^= fromMask;

            if (piece <= K){
                whitePieces ^= toMask;
                whitePieces ^= fromMask;
            } else {
                blackPieces ^= toMask;
                blackPieces ^= fromMask;
            }

            // undo captured pawn behind the square
            U64 capturedSquare = side == BLACK ? to - 8 : to + 8; // opposite side
            U64 capturedMask = 1ULL << capturedSquare;
            bitboards[captured] ^= capturedMask;

            if (captured <= K) whitePieces ^= capturedMask;
            else blackPieces ^= capturedMask;

            // Update occupied
            occupied |= capturedMask;
            break;
        }

        case FLAG_PROMOTION: {
            // remove promoted piece
            bitboards[promotedPiece] ^= toMask;
            bitboards[piece] ^= fromMask; // restore pawn

            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            } else {
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // restore captured piece
            if (captured != -1) {
                bitboards[captured] ^= toMask;
                if (captured <= K) whitePieces ^= toMask;
                else blackPieces ^= toMask;
            }

            if (captured != -1) occupied |= toMask;
            break;
        }

        default: {
            bitboards[piece] ^= toMask;
            bitboards[piece] ^= fromMask;

            if (piece <= K){
                whitePieces ^= fromMask;
                whitePieces ^= toMask;
            } else {
                blackPieces ^= fromMask;
                blackPieces ^= toMask;
            }

            // restore captured piece 
            if (captured != -1) {
                bitboards[captured] ^= toMask;
                if (captured <= K) whitePieces ^= toMask;
                else blackPieces ^= toMask;
            }
            if (captured != -1) occupied |= toMask;
            break;
        }
    }

    // Update occupied
    occupied &= ~toMask;
    occupied |= fromMask;
    side ^= 1;
}