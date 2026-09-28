import json
import time
import os
import sys

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from core import libchaotic_kernel

def main():
    print("=" * 70)
    print("⚡ BANGSAEN AI LABS - ANTI-AI CHAOTIC ENGINE v4.0")
    print("🔒 Non-Linear Lorenz Attractor Subspace Verification")
    print("=" * 70)
    
    data_path = os.path.join(os.path.dirname(__file__), '..', 'data', 'chaotic_state_vectors.json')
    with open(data_path, 'r', encoding='utf-8') as f:
        data_json = json.load(f)
        vectors = data_json["chaotic_vectors"]
        
    start_time = time.time()
    decoded = libchaotic_kernel.inspect_chaotic_logs(vectors)
    elapsed = (time.time() - start_time) * 1000

    print(f"\n======================================================================")
    print(f"🔓 DECODING RESULT (Execution Time: {elapsed:.2f} ms):")
    print(f"======================================================================")
    print(decoded)
    print(f"======================================================================\n")

if __name__ == "__main__":
    main()