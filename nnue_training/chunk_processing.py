import pandas as pd
import h5py
import numpy as np
import os
import pre_processing

output_folder = "training_data"
os.makedirs(output_folder, exist_ok=True)

# Process a chunk of data 
def process_chunk(df_chunk: pd.DataFrame) -> tuple[np.ndarray, np.ndarray]:
    
    X_list, y_list = [], []

    for fen, eval_str in zip(df_chunk['FEN'], df_chunk['Evaluation']):
        try:
            # preprocess each FEN and evaluation
            X, y = pre_processing.preprocess_fen_eval(fen, eval_str)

            # only keep non-mate positions
            if X is not None:  
                X_list.append(X)
                y_list.append(y)
            else:   
                # skip mate positions
                continue
        
        # catch 
        except Exception as e:
            print("Skipped row due to error:", e)       

    return np.array(X_list, dtype=np.uint8), np.array(y_list, dtype=np.float32)

chunksize = 100000

for i, chunk in enumerate(pd.read_csv("chessData.csv", chunksize=chunksize)):

    X_chunk, y_chunk = process_chunk(chunk)
    file_path = os.path.join(output_folder, f"chunk_{i:03d}.h5")

    # Save each chunk as H5
    with h5py.File(file_path, "w") as f:
        f.create_dataset("X", data=X_chunk, compression="gzip")
        f.create_dataset("y", data=y_chunk, compression="gzip")
