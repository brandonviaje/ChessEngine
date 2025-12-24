import chess
import numpy as np

"""
Converts a FEN string and Stockfish evaluation into:
- X: 768 one-hot vector representation of the board
- y: numeric evaluation

Mate scores are skipped
"""
def preprocess_fen_eval(fen: str, eval_str: str):

    # Convert FEN to 768-vector 
    board = chess.Board(fen)
    X = np.zeros(768, dtype=np.uint8)
    
    for square, piece in board.piece_map().items():

        piece_type = piece.piece_type - 1                   # 0-5: pawn -> king
        color_offset = 0 if piece.color else 6              # 0 = White, 6 = Black

        idx = (piece_type + color_offset) * 64 + square     # calculate index
        X[idx] = 1                                          # one-hot encode using 768-vector

    eval_str = str(eval_str).strip()                        # convert evaluation to numeric 
    
    # check for mate notation
    if "#" in eval_str:
        return None, None                                   # skip mate positions
    
    y = float(eval_str)                                     # numeric evaluation                   

    return X, y
