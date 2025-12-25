import pandas as pd
import h5py
import numpy as np
import os
import pre_processing

output_folder = "training_data"
os.makedirs(output_folder, exist_ok=True)

# Process a chunk of data 
def process_chunk(df_chunk: pd.DataFrame) -> tuple[np.ndarray, np.ndarray]:
    
    X_white_list, X_black_list, y_list = [], [], []

    for fen, eval_str in zip(df_chunk['FEN'], df_chunk['Evaluation']):
        try:
            # preprocess each FEN and evaluation for both perspectives
            x_white, y = pre_processing.preprocess_fen_eval(fen, eval_str)
            x_black, _ = pre_processing.preprocess_fen_eval(fen, eval_str, perspective='black')

            # only keep non-mate positions
            if y is not None:  
                X_white_list.append(x_white)
                X_black_list.append(x_black)
                y_list.append(y)
        
        # catch errors in preprocessing
        except Exception as e:
            print("Skipped row due to error:", e)       

    return (
    np.array(X_white_list, dtype=np.uint8),
    np.array(X_black_list, dtype=np.uint8),
    np.array(y_list, dtype=np.float32)
    )

# Read and process CSV in chunks
chunksize = 100000

for i, chunk in enumerate(pd.read_csv("chessData.csv", chunksize=chunksize)):

    X_white_chunk, X_black_chunk, y_chunk = process_chunk(chunk)
    file_path = os.path.join(output_folder, f"chunk_{i:03d}.h5")

    # Save each chunk as H5
    with h5py.File(file_path, "w") as f:
        f.create_dataset("X_white", data=X_white_chunk, compression="gzip")
        f.create_dataset("X_black", data=X_black_chunk, compression="gzip")
        f.create_dataset("y", data=y_chunk, compression="gzip")
