import random

def generate_net_data(num_nets=4000, num_components=200, min_comps=2, max_comps=50):
    net_data = []

    for net_id in range(1, num_nets + 1):
        # 隨機選擇要包含的元件數量
        num_comps = random.randint(min_comps, max_comps)
        # 隨機選擇元件，保證不重複
        comps = random.sample(range(1, num_components + 1), num_comps)
        # 產生元件名稱（例如 c14）
        comp_names = [f"c{c}" for c in sorted(comps)]
        # 組合成一行資料
        net_line = f"NET n{net_id} {{ {' '.join(comp_names)} }}"
        net_data.append(net_line)

    return net_data

# 產生資料並寫入檔案
if __name__ == "__main__":
    nets = generate_net_data()
    
    # 寫入 net_data.txt 檔案
    with open("net_data.txt", "w") as f:
        for net in nets:
            f.write(net + "\n")

    print("資料已成功寫入 net_data.txt")
