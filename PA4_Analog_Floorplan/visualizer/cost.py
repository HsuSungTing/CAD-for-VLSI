import matplotlib.pyplot as plt

# 儲存 iteration 和 value
iterations = []
values = []

# 讀取 .txt 檔案
with open('accepted_costs.txt', 'r', encoding='utf-8') as f:
    for line in f:
        line = line.strip()
        if not line:
            continue
        if line.startswith("Iteration"):
            try:
                parts = line.replace("Iteration", "").split(":")
                iter_num = int(parts[0].strip())
                value = float(parts[1].strip())
                iterations.append(iter_num)
                values.append(value)
            except Exception as e:
                print(f"解析失敗: {line}, 錯誤: {e}")

# 畫出折線圖
plt.figure(figsize=(10, 6))
plt.plot(iterations, values, marker='o', linestyle='-', color='red', markersize=3, label='Value')

# 設定圖表資訊
plt.title('Cost of Every SA Iteration')
plt.xlabel('SA Iteration')
plt.ylabel('Cost')
plt.grid(True)
plt.legend()
plt.tight_layout()

# 顯示圖表
plt.show()
