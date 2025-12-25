import chess
import numpy as np

"""
Converts a FEN string and Stockfish evaluation into:
- X: 768 one-hot vector representation of the board from given perspective
- y: numeric evaluation

Mate scores are skipped
"""

# Preprocess evaluation score
def preprocess_y(eval_val):
    y = float(eval_val) / 100.0      # centipawns -> pawns
    y = np.clip(y, -10.0, 10.0)      # clamp extreme values
    y = y / 10.0                     # scale to [-1, 1]
    return y

# Preprocess FEN and evaluation
def preprocess_fen_eval(fen: str, eval_str: str, perspective='white'):
    # Convert FEN to 768-vector 
    board = chess.Board(fen)
    X = np.zeros(768, dtype=np.uint8)

    # populate one-hot encoding
    for square, piece in board.piece_map().items():
        
        piece_color = piece.color                               # original piece color  
        square_idx = square                                     # original square index       

        # Adjust for perspective    
        if perspective == 'black':
            piece_color = not piece.color                       # flip piece color
            square_idx = chess.square_mirror(square)            # flip square index

        piece_type = piece.piece_type - 1                       # piece type index (0-5)
        color_offset = 0 if piece_color else 6                  # 0 = White, 6 = Black
        idx = (piece_type + color_offset) * 64 + square_idx     # calculate index
        X[idx] = 1                                              # set one-hot using calculated index

    eval_str = str(eval_str).strip()                            # convert evaluation to numeric 

    # check for mate notation
    if "#" in eval_str:
        return None, None
    
    y = preprocess_y(eval_str)                                   # preprocess evaluation score
    
    return X, y
