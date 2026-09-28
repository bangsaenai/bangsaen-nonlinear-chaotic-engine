import json
import math
import ctypes
import random

def get_local_seed():
    class HW_PROFILE_INFOA(ctypes.Structure):
        _fields_ = [
            ("dwDockInfo", ctypes.c_ulong),
            ("szHwProfileGuid", ctypes.c_char * 39),
            ("szHwProfileName", ctypes.c_char * 80)
        ]
    
    info = HW_PROFILE_INFOA()
    if ctypes.windll.advapi32.GetCurrentHwProfileA(ctypes.byref(info)):
        guid = info.szHwProfileGuid.decode('utf-8')
        hash_val = 5381
        for c in guid:
            hash_val = (((hash_val << 5) + hash_val) + ord(c)) & 0xFFFFFFFF
        x = ((hash_val & 0xFF) / 255.0) * 10.0 + 1.0
        y = (((hash_val >> 8) & 0xFF) / 255.0) * 10.0 - 5.0
        z = (((hash_val >> 16) & 0xFF) / 255.0) * 10.0 + 10.0
        return x, y, z
    return 1.0, 1.0, 1.0

def encode_secret_hardened(text, target_size=256):
    x, y, z = get_local_seed()
    sigma, rho, beta, dt = 10.0, 28.0, 8.0 / 3.0, 0.01
    
    raw_bytes = text.encode('utf-8')
    data_length = len(raw_bytes)
    
    payload = bytes([data_length & 0xFF, (data_length >> 8) & 0xFF]) + raw_bytes
    padding_needed = max(0, target_size - len(payload))
    random_padding = bytes([random.randint(0, 255) for _ in range(padding_needed)])
    full_stream = payload + random_padding

    encoded_data = {
        "engine_version": "v4.1-CHAOTIC-ANTI-AI-HARDENED",
        "chaotic_vectors": []
    }

    prev_byte = 0x5A
    for b in full_stream:
        dynamic_steps = 3 + (prev_byte % 6)
        for _ in range(dynamic_steps):
            dx = sigma * (y - x) * dt
            dy = (x * (rho - z) - y) * dt
            dz = (x * y - beta * z) * dt
            x += dx
            y += dy
            z += dz
        
        chaotic_mask = ((int)(abs(x * 1337.0) + abs(y * 7331.0) + abs(z * 9999.0)) ^ prev_byte) & 0xFF
        enc_val = float(b ^ chaotic_mask)
        
        # Save ONLY 1D Scalar (Zero Side-Channel Leakage)
        encoded_data["chaotic_vectors"].append(round(enc_val, 4))
        prev_byte = b
        
    return encoded_data

if __name__ == "__main__":
    secret = "[EP.26 ARENA // BANGSAEN AI LABS] Hardened Anti-AI Chaotic Engine v4.1 | Deception Pipeline Active"
    data_structure = encode_secret_hardened(secret, target_size=256)
    
    with open("data/chaotic_state_vectors.json", "w", encoding="utf-8") as f:
        json.dump(data_structure, f, indent=2)
    print("✅ Generated Hardened Anti-AI Data Logs Successfully!")