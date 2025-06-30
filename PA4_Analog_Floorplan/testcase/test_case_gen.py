import random

def generate_entry(index):
    name = f"M{index}"
    segments = []

    num_segments = random.choice([1,5,50, 30, 40, 10, 100,150,300])  # 模仿資料中的常見組合

    for _ in range(num_segments):
        pos = round(random.uniform(3, 300), 2)     # 隨機產生位置（第一個數字）
        duration = round(pos * random.uniform(0.01, 0.2), 2)  # 模仿 duration 趨勢
        start = random.choice([1, 2, 3, 4, 6, 8, 10, 12])      # 起始時間
        repeat = random.choice([1, 21, 50, 14, 5, 8, 10])  # 重複次數
        segments.append(f"({pos} {duration} {start} {repeat})")

    return f"{name} {' '.join(segments)}"

# 寫入 output.txt
with open("output.txt", "w") as f:
    for i in random.sample(range(1, 500000), 10000):  # 這裡改回 35 筆，避免超出 range 錯誤
        f.write(generate_entry(i) + "\n")
