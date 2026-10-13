# PA2：Knight's Treasure Hunt

## 題目

給定 N×N 棋盤，騎士從唯一的 S 格出發，依西洋棋騎士規則每次移動「一方向兩格、另一方向一格」。每次合法移動得 1 分並消耗 1 能量。不可超出棋盤、踩到 # 或重訪任何格。能量為 0 時不可再移動；總移動不得超過 L。可以主動提早停止，不需走完所有格。

格子 B 額外增加 S 分；E 額外恢復 V 能量；X 額外扣 P 分；O 是一般格。能量恢復採「舊能量 -1 +V」，不設初始能量上限。起點分數為 0、能量為輸入的 E。請找出所有合法路徑中的最高總分，印出一條達到最高分的路徑、每步 replay 與路徑棋盤。答案路徑及步數可能不唯一，分數才是最佳化目標。

## 輸入

第一行 N、L、起始能量 E；第二行獎勵 S、補充能量 V、炸彈扣分 P；其後 N 行，每行 N 個以空白分隔的格子符號。基本版只允許 S、B、E、X、#、O。

限制是 `1<=N<=20`、`1<=L,E<=100`、`1<=S,V,P<=10`。必須使用遞迴與剪枝；可用標準函式庫，但不得宣告自訂 class 或 struct。程式遵守此規則，以固定大小陣列保存棋盤、目前路徑及最佳路徑。

## 輸出

預設先印歡迎訊息與輸入提示，之後印出 `Best score:` 與 `Steps:`。replay 從第 0 步開始，每行列出步數、`(x,y)`、事件種類、目前分數及目前能量。座標原點在左下角；輸入第一行是棋盤最上方，因此 `x=column`、`y=N-1-row`。事件類型為 start、normal、bonus、energy、bomb。

最後重新印棋盤。起點維持 S，已走過的其他格顯示步數 1..K，未走過的格顯示原符號。每格欄寬是 `2 + 最終步數的十進位位數`，以 `setw` 左對齊。提示不回顯鍵盤輸入。

`--summary` 是額外提供的簡潔介面，只輸出 `最佳分數 步數` 及換行。未知參數、無效維度、數值超出規格、錯誤格子、缺少或多個起點都回傳 1，並寫 stderr。

## Bonus：成長守衛

用 `--bonus` 開啟，可加入 A 格作為守衛種子。守衛可以有多個，範圍同時成長，以種子為中心：第 0 步半徑 0（1×1）、第 5 步半徑 1（3×3）、第 10 步半徑 2（5×5）、第 15 步起半徑 3（7×7），超出棋盤的部分忽略。

騎士完成第 t 次移動後，立即套用該步成長。若目的格在任一守衛當時範圍內，整條路徑無效，即使原格曾有 bonus 或 energy 也不能走入。第 4、9、14 步規劃下一步時必須看成長後的目的格。

replay 在第 0、5、10、15 步後各印一條正式規格的守衛通知，通知在同一行；第 20 步以後不再重複。最後棋盤的未訪問格若被守衛覆蓋，就顯示 A；已訪問格仍保留 S 或步號。

## 解法與正確性

先預計算每格八種可能鄰居，並計算每格第一次被守衛覆蓋的步數。其後以遞迴 DFS 枚舉所有合法移動；每次移動先標記 visited、更新分數及能量，遞迴完成後撤銷標記。每個狀態都允許停止，當分數高於目前最佳值就保存路徑。

剪枝先以 BFS 計算每格最早可能抵達的距離 d；第 step+d 步已被守衛覆蓋的格不可能到達。這裡忽略能量而保留所有可達格，只會把可能性放寬，不會排除合法路徑。針對每個候選未來步數 m，只統計 d<=m 的格子與物品。

騎士每次改變棋盤黑白格顏色，故未來 m 步需要同色 `floor(m/2)` 格、異色 `ceil(m/2)` 格。若可用格數不足，或 m 超過「目前能量 + 所有可能能量格數×V」，該 m 不可能達成。對兩種顏色分別樂觀取最多 bonus，並算出格數不夠時至少必須踩到的 bomb 數，得到 `score+m+S*可收取bonus數-P*至少bomb數`。所有可行 m（也包含停止的 m=0）的最大值就是上界。這個估計忽略真正路線限制與部分未來守衛阻擋，因而不會低估真實可達最高分；只有上界不超過目前最佳分數時才剪枝。

候選動作先嘗試高分／補能量格，平手優先後續選擇較少的格，類似 Warnsdorff 規則；這只改變搜尋順序，不排除路徑。N<=8 時以 visited bit mask 與目前格做記憶化。visited 集合已決定步數、分數與能量，故相同 key 的未來搜尋等價。超過一百萬 entries 時清空快取，只增加重算，不影響最佳答案。

精確搜尋最壞情況仍為指數時間；本題加入障礙、能量、分數與成長範圍後，不能以一般棋盤 tour 的存在性取代路徑搜尋。程式沒有硬性的時間截止或「找到一條看似好路徑就提前停止」的近似做法。

## 完成與驗證報告

基本規則、完整 replay、對齊路徑棋盤與成長 bonus 均已實作。主要困難是不能過度剪枝，因此上界保留所有可能未來補能量與 bonus，再以獨立的小棋盤窮舉驗證。測試檢查最大分數、騎士合法性、不重訪、能量、坐標轉換與守衛成長，而非只比對一條固定路徑。

## 範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

標準輸入：
```text
1 1 1
3 2 1
S
```

標準輸出：
```text
Welcome to Knight's Treasure Hunt!!
Enter board size N, step limit, and starting energy (e.g., 7 25 5): Enter values for Bonus (B), Energy (E), and Bomb penalty (X) (e.g., 5 4 10): Enter the board with 1 lines, each with 1 tokens separated by spaces:

----The Result Of Knight's Treasure Hunt----
Best score: 0
Steps: 0

----The Replay Of Knight's Treasure Hunt----
Step  (x,y)     Type        Score     Energy
------------------------------------------------
0     (0,0)     start       0         1

----The Best Path Grid Of Knight's Treasure Hunt----
S  
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`--summary`

標準輸入：
```text
3 3 1
3 2 1
S # #
# # B
# # #
```

標準輸出：
```text
4 1
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`--bonus --summary`

標準輸入：
```text
3 3 1
3 2 1
S # #
# # B
A # #
```

標準輸出：
```text
4 1
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 solution 程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>
#include <iomanip>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <cstdlib>

// Fixed arrays keep the search state explicit; no user-defined class or struct.
int n, limit_steps, initial_energy, bonus_value, energy_value, penalty;
char board[400];
int neighbors[400][8], degree_count[400], blocked_from[400];
bool visited[400];
int path[101], best_path[101], best_score = 0, best_steps = 0;
bool bonus_mode = false, summary_only = false;
std::unordered_set<std::string> seen;
const int dr[8] = {-2, -2, -1, -1, 1, 1, 2, 2};
const int dc[8] = {-1, 1, -2, 2, -2, 2, -1, 1};

bool safe(int p, int step) { return board[p] != '#' && step < blocked_from[p]; }
int score_gain(int p) { return 1 + (board[p] == 'B' ? bonus_value : 0) - (board[p] == 'X' ? penalty : 0); }
int min2(int a, int b) { return a < b ? a : b; }

// The connected component and optimistic item selection form an admissible bound.
int upper_bound(int current, int step, int energy, int score) {
    bool marked[400] = {};
    int distance[400] = {};
    int queue[400], front = 0, back = 0;
    queue[back++] = current; marked[current] = true;
    int counts[101][2] = {}, bonuses[101][2] = {}, energies[101] = {}, bombs[101][2] = {};
    int current_color = (current / n + current % n) % 2;
    while (front < back) {
        int p = queue[front++];
        for (int k = 0; k < degree_count[p]; ++k) {
            int q = neighbors[p][k];
            int d = distance[p] + 1;
            if (marked[q] || visited[q] || d > limit_steps - step || !safe(q, step + d)) continue;
            marked[q] = true; queue[back++] = q;
            distance[q] = d;
            int color = ((q / n + q % n) % 2) != current_color;
            ++counts[d][color];
            bonuses[d][color] += board[q] == 'B'; energies[d] += board[q] == 'E'; bombs[d][color] += board[q] == 'X';
        }
    }
    int maximum = score;
    for (int m = 1; m <= limit_steps - step; ++m) {
        for (int color = 0; color < 2; ++color) {
            counts[m][color] += counts[m-1][color];
            bonuses[m][color] += bonuses[m-1][color];
            bombs[m][color] += bombs[m-1][color];
        }
        energies[m] += energies[m-1];
        int need[2] = {m / 2, (m + 1) / 2};
        if (need[0] > counts[m][0] || need[1] > counts[m][1] || m > energy + energies[m] * energy_value) continue;
        int collect = 0, forced_bombs = 0;
        for (int color = 0; color < 2; ++color) {
            collect += min2(need[color], bonuses[m][color]);
            int clean = counts[m][color] - bombs[m][color];
            if (need[color] > clean) forced_bombs += need[color] - clean;
        }
        int bound = score + m + bonus_value * collect - penalty * forced_bombs;
        if (bound > maximum) maximum = bound;
    }
    return maximum;
}
void search(int current, int step, int energy, int score) {
    if (score > best_score) {
        best_score = score; best_steps = step;
        for (int i = 0; i <= step; ++i) best_path[i] = path[i];
    }
    if (step == limit_steps || energy == 0 || upper_bound(current, step, energy, score) <= best_score) return;
    if (n <= 8) {
        // Visited set determines steps, score and energy, so these need no key fields.
        std::string key(10, '\0');
        for (int p = 0; p < n * n; ++p) if (visited[p]) key[p / 8] = static_cast<char>(static_cast<unsigned char>(key[p / 8]) | (1u << (p % 8)));
        key[8] = static_cast<char>(current); key[9] = static_cast<char>(current >> 8);
        if (!seen.insert(key).second) return;
        // Evicting memo entries affects runtime only, never correctness.
        if (seen.size() > 1000000) seen.clear();
    }
    int candidates[8], priorities[8], count = 0;
    for (int k = 0; k < degree_count[current]; ++k) {
        int q = neighbors[current][k];
        if (visited[q] || !safe(q, step + 1)) continue;
        int onward = 0;
        for (int j = 0; j < degree_count[q]; ++j) {
            int p = neighbors[q][j];
            if (!visited[p] && safe(p, step + 2)) ++onward;
        }
        int priority = score_gain(q) * 32 + (board[q] == 'E' ? energy_value * 16 : 0) - onward;
        int index = count++;
        while (index > 0 && priorities[index - 1] < priority) {
            candidates[index] = candidates[index - 1]; priorities[index] = priorities[index - 1]; --index;
        }
        candidates[index] = q; priorities[index] = priority;
    }
    for (int k = 0; k < count; ++k) {
        int q = candidates[k];
        visited[q] = true; path[step + 1] = q;
        search(q, step + 1, energy - 1 + (board[q] == 'E' ? energy_value : 0), score + score_gain(q));
        visited[q] = false;
    }
}
void notice(int step) {
    if (!bonus_mode) return;
    if (step == 0) std::cout << "<!> OH NO <!> ANDY HAS AWAKENED!!! The FINAL COUNTDOWN begins...\n";
    if (step == 5) std::cout << "<!> WARNING <!> ANDY IS ENRAGED!!! Run if you can...\n";
    if (step == 10) std::cout << "<!> DANGER <!> ANDY GROWS STRONGER!!! You feel the end is near...\n";
    if (step == 15) std::cout << "<!> CRITICAL <!> ANDY UNLEASHES CHAOS!!! Farewell, hero. This will be the end of your journey...\n";
}
int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--bonus") bonus_mode = true;
        else if (option == "--summary") summary_only = true;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    if (!summary_only) std::cout << "Welcome to Knight's Treasure Hunt!!\nEnter board size N, step limit, and starting energy (e.g., 7 25 5): ";
    if (!(std::cin >> n >> limit_steps >> initial_energy) || n < 1 || n > 20 || limit_steps < 1 || limit_steps > 100 || initial_energy < 1 || initial_energy > 100) { std::cerr << "Invalid input\n"; return 1; }
    if (!summary_only) std::cout << "Enter values for Bonus (B), Energy (E), and Bomb penalty (X) (e.g., 5 4 10): ";
    if (!(std::cin >> bonus_value >> energy_value >> penalty) || bonus_value < 1 || bonus_value > 10 || energy_value < 1 || energy_value > 10 || penalty < 1 || penalty > 10) { std::cerr << "Invalid input\n"; return 1; }
    if (!summary_only) std::cout << "Enter the board with " << n << " lines, each with " << n << " tokens separated by spaces:\n";
    int start = -1;
    for (int p = 0; p < n * n; ++p) {
        if (!(std::cin >> board[p]) || (std::string("SBEX#O").find(board[p]) == std::string::npos && !(bonus_mode && board[p] == 'A'))) { std::cerr << "Invalid board\n"; return 1; }
        if (board[p] == 'S') { if (start != -1) { std::cerr << "Multiple starts\n"; return 1; } start = p; }
        blocked_from[p] = 101;
    }
    if (start < 0) { std::cerr << "Missing start\n"; return 1; }
    for (int p = 0; p < n * n; ++p) {
        int r = p / n, c = p % n;
        for (int k = 0; k < 8; ++k) {
            int rr = r + dr[k], cc = c + dc[k];
            if (rr >= 0 && rr < n && cc >= 0 && cc < n) neighbors[p][degree_count[p]++] = rr * n + cc;
        }
        if (bonus_mode) for (int q = 0; q < n * n; ++q) if (board[q] == 'A') {
            int distance = std::max(std::abs(r - q / n), std::abs(c - q % n));
            if (distance <= 3) blocked_from[p] = min2(blocked_from[p], distance * 5);
        }
    }
    visited[start] = true; path[0] = best_path[0] = start;
    search(start, 0, initial_energy, 0);
    if (summary_only) { std::cout << best_score << ' ' << best_steps << '\n'; return 0; }
    std::cout << "\n----The Result Of Knight's Treasure Hunt----\nBest score: " << best_score << "\nSteps: " << best_steps << "\n\n----The Replay Of Knight's Treasure Hunt----\n";
    std::cout << std::left << std::setw(6) << "Step" << std::setw(10) << "(x,y)" << std::setw(12) << "Type" << std::setw(10) << "Score" << "Energy\n------------------------------------------------\n";
    int score = 0, energy = initial_energy, index[400];
    std::fill(index, index + n * n, -1);
    for (int s = 0; s <= best_steps; ++s) {
        int p = best_path[s]; index[p] = s;
        std::string type = "start";
        if (s) {
            score += score_gain(p); energy -= 1;
            if (board[p] == 'E') { energy += energy_value; type = "energy"; }
            else if (board[p] == 'B') type = "bonus";
            else if (board[p] == 'X') type = "bomb";
            else type = "normal";
        }
        std::string coordinates = "(" + std::to_string(p % n) + "," + std::to_string(n - 1 - p / n) + ")";
        std::cout << std::setw(6) << s << std::setw(10) << coordinates << std::setw(12) << type << std::setw(10) << score << energy << '\n';
        notice(s);
    }
    int width = 2 + static_cast<int>(std::to_string(best_steps).size());
    std::cout << "\n----The Best Path Grid Of Knight's Treasure Hunt----\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            int p = r * n + c;
            std::string token = index[p] == 0 ? "S" : (index[p] > 0 ? std::to_string(index[p]) : (best_steps >= blocked_from[p] ? "A" : std::string(1, board[p])));
            std::cout << std::setw(width) << token;
        }
        std::cout << '\n';
    }
}
```

<!-- SOLUTION_CODE_END -->
