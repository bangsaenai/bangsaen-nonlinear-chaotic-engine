import os
import sys
import time

# ดึงตำแหน่ง Directory ปัจจุบันของไฟล์สคริปต์
base_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(base_dir)

import json
from core import libchaotic_kernel

def main():
    print("=" * 70)
    print("⚡ BANGSAEN AI LABS - ANTI-AI CHAOTIC ENGINE v4.1")
    print("🔒 Non-Linear Lorenz Attractor Subspace Verification")
    print("=" * 70)

    data_path = os.path.join(base_dir, "data", "chaotic_state_vectors.json")
    
    if not os.path.exists(data_path):
        print(f"❌ Error: Data file not found at {data_path}")
        return

    with open(data_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    vectors = data.get("chaotic_vectors", [])

    start_time = time.perf_counter()
    decoded = libchaotic_kernel.inspect_chaotic_logs(vectors)
    end_time = time.perf_counter()

    execution_time = (end_time - start_time) * 1000  # ms

    print("\n" + "=" * 70)
    print(f"🔓 DECODING RESULT (Execution Time: {execution_time:.2f} ms):")
    print("=" * 70)
    print(decoded)
    print("=" * 70 + "\n")

if __name__ == "__main__":
    main()