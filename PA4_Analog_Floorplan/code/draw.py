import matplotlib.pyplot as plt
import matplotlib.patches as patches

# 讀取輸入檔案（你可以改成讀取檔案的方式）
input_data = """
M3 21.40 34.10 (103.52 1.88 5 2)
M5 0.00 49.40 (81.40 15.40 1 10)
M6 0.00 32.22 (103.52 1.88 5 2)
M9 76.39 80.20 (25.93 2.28 2 2)
M10 25.40 80.20 (50.99 1.14 4 1)
M12 0.00 64.80 (81.40 15.40 1 10)
M14 49.40 12.32 (105.39 1.54 4 1)
M15 0.00 91.48 (13.40 4.56 1 4)
M16 0.00 0.00 (49.93 2.28 2 2)
M24 49.93 23.10 (122.99 1.74 4 1)
M26 0.00 28.74 (61.93 3.48 2 2)
M60 0.00 43.50 (25.93 2.82 2 3)
M63 31.33 81.34 (17.93 1.88 2 2)
M90 25.93 35.98 (105.39 1.54 4 1)
M98 0.00 3.08 (414.71 1.54 8 1)
M103 61.93 24.84 (41.93 4.70 2 5)
M104 67.19 82.48 (17.93 1.88 2 2)
M106 0.00 80.20 (25.40 11.28 1 12)
M107 49.26 81.34 (17.93 1.88 2 2)
M108 0.00 23.10 (49.93 5.64 2 6)
M109 0.00 21.56 (105.39 1.54 4 1)
M110 0.00 34.10 (21.40 9.40 1 10)
M111 0.00 10.78 (414.71 1.54 8 1)
M112 85.12 82.48 (3.40 0.94 1 1)
M113 81.40 55.56 (38.46 1.88 3 2)
M114 0.00 12.32 (49.40 9.24 1 6)
M115 0.00 46.32 (146.46 3.08 3 2)
M116 0.00 4.62 (104.33 6.16 2 4)
M117 49.93 0.00 (53.13 3.08 2 2)
M118 81.40 49.40 (27.00 6.16 1 4)
M119 13.40 91.48 (17.93 1.88 2 2)
"""

# 將資料轉為 block 的列表
blocks = []
for line in input_data.strip().split('\n'):
    parts = line.split()
    name = parts[0]
    x = float(parts[1])
    y = float(parts[2])
    width = float(parts[3].strip('('))
    height = float(parts[4])
    blocks.append((name, x, y, width, height))

# 畫圖
fig, ax = plt.subplots(figsize=(12, 10))

for name, x, y, w, h in blocks:
    rect = patches.Rectangle((x, y), w, h, linewidth=1, edgecolor='blue', facecolor='skyblue', alpha=0.5)
    ax.add_patch(rect)
    # 標上block名字（可調整偏移量以避免重疊）
    ax.text(x + w / 2, y + h / 2, name, ha='center', va='center', fontsize=8, color='black')

# 自動設定畫布邊界
all_x = [x + w for _, x, _, w, _ in blocks]
all_y = [y + h for _, _, y, _, h in blocks]
ax.set_xlim(0, max(all_x) + 10)
ax.set_ylim(0, max(all_y) + 10)
ax.set_aspect('equal')
ax.set_title('Block Placement Visualization')
ax.set_xlabel('X')
ax.set_ylabel('Y')
plt.grid(True)
plt.tight_layout()
plt.show()
