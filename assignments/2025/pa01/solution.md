# PA1：Dinosaur Game

## 題目

實作離散版恐龍遊戲。恐龍永遠在 `x=0`；每一 tick 所有障礙向左移動一格，右端補進新障礙。畫面有 9 欄，基本版有 `y=0..3` 四列。恐龍正常高度 2，蹲下高度 1；位置以腳下那格的 y 表示。

障礙代碼 `0` 表示空白；`1` 是地面高 1 的仙人掌；`2` 是地面高 2 的仙人掌；`3` 是位於 y=1 的翼龍；`4` 是位於 y=2 的翼龍。每欄最多一個障礙。初始輸入的 8 位數依序放在 x=1..8，x=0 保證無障礙，前導零必須保留其位置意義。

動作 `0` 無操作：蹲下者站直，跳躍者繼續動作。`1` 跳躍：正常跳躍的接續四個 tick 腳下高度依序為 1、2、1、0，尚未結束不能再次跳躍。`2` 蹲下：立即取消跳躍並在下一 tick 回到地面，身高變為 1。每 tick 先收取合法動作，再收取右端新障礙。只檢查該 tick 最後所在格的碰撞，不檢查移動途中。

當初始配置為 -1，立即結束，輸出 `The game ended when it just started.`；後續障礙輸入 -1，立即勝利，不再推進動作或檢查下一 tick。碰撞則輸出 `What a pity, it was close :(`。結束時統一輸出 farewell。

## 輸入與輸出

基本版第一筆輸入是 -1 或 8 位障礙配置（以整數讀入後從右向左拆位）。其後重複輸入動作與單位障礙。障礙輸入保證合法；動作可能是浮點數，只有數值等於 0、1、2 才接受，故 `01` 與 `1.0` 有效，`0.1` 無效。非法動作需重新輸入，不推進時間；不能跳躍則另顯示跳躍錯誤訊息。

畫面由最高 y 向下輸出。每個格子為 `|` 加空格再加物件字元，最後補 `|`。恐龍顯示 D、仙人掌 C、翼龍 P、空白格再放一個空格。各提示與結束訊息均由程式完整提供。預設回顯讀入內容，與官方 `.txt` 畫面範例一致；使用 `--no-echo` 時，提示後不印輸入與換行。

## Bonus 1：雙層遊戲

用 `--double` 執行。畫面增加上層 `y=4..7`，並在 y=3 和 y=4 間印出天花板線。天花板不占用 y 座標。初始讀入順序是下層 8 位配置、上層 8 位配置、天花板 9 位配置；後續每 tick 讀入下層障礙、上層障礙、天花板單位配置。天花板 0 是缺口、1 是實體。

下層初始或後續輸入 -1 時立即結束，不繼續讀上層與天花板。上層障礙的四種代碼相對於上層地面 y=4 解讀。

第一次跳到 y=2 的頂點時，若下一欄 x=1 是缺口，允許再次跳躍，接續上升為 y=3、4、5、6，再下降至 5、4。若缺口不允許通行則拒絕再次跳躍。上升或下降過程不能任意重新跳躍；只有確實落在實體上層地板的 y=4 才能重新開始上層正常跳躍（5、6、5、4）。已站在上層而下一欄為缺口時，不操作會逐 tick 下降 3、2、1、0，下降時不能跳躍。身在 y>=4 時蹲下，若下一欄實體就直接落到 y=4；若下一欄是缺口則直接回 y=0。碰撞仍只檢查最後位置。

## Bonus 2：自動遊玩

用 `--auto` 執行，適用雙層規則。輸入不含動作，程式自行選擇合法動作並印到原本的動作提示後。最多 10,000 tick，保證存在可勝利方案；若資料違反此保證，回傳 2 並在 stderr 顯示沒有路徑。

## 限制與補定

採 C++11，只引入 `<iostream>`，沒有硬編任何測資結果。自動版的 10,000 tick 上限來自作業規格；本實作亦將手動版的保存畫面上限設為 10,000，超出則回報錯誤。資料提早 EOF、無法讀入數字、未知命令列選項皆回傳 1。正式文句以提供的 2025 規格、共同問題說明與公開文字範例互相核對。

## 解法與報告

狀態包含腳下 y、上升階段、是否蹲下。`transition` 是所有模式唯一的動作規則來源；`collision` 則逐一檢查恐龍占用的格子，因此蹲下可以躲過低空翼龍，卻不能躲過地面仙人掌。

雙層模式使用相同畫面與碰撞函式，僅增加障礙列與天花板。天花板判斷使用移動前的 x=1，與移動後的 x=0 相同，避免常見的一 tick 偏移。

自動版先讀入完整障礙時間序列，再在「時間 × 56 個編碼狀態」上做動態規劃。每個已可到達狀態嘗試三種動作，僅保存合法且不碰撞的下一狀態與 predecessor。最後從任一可達終點反向取回命令，正向回放完整畫面。這會涵蓋所有合法狀態，無須猜測一個有限的前瞻視窗；若存在勝利路徑就一定能找到。時間 O(T × 56 × 3)，空間 O(T × 56)，足以處理規格的 10,000 tick。

完成基本版與兩項 bonus。主要困難是雙跳、上層落地、缺口自然下降和蹲下瞬間落地必須有明確不同狀態；共用轉移函式避免自動版與手動版的規則分歧。測試以官方文字畫面和額外邊界為準，不代表已送上課程 OJ。

## 範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

標準輸入：
```text
-1
```

標準輸出：
```text
======Welcome to the Dinosaur Game======
Please enter the initial obstacle setup of the Dinosaur Game. Enter -1 to quit: -1
The game ended when it just started.
======Thanks for playing Dinosaur Game, see you next time======
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`--double`

標準輸入：
```text
-1
```

標準輸出：
```text
======Welcome to the Dinosaur Game======
Please enter the first layer initial obstacle setup of the Dinosaur Game. Enter -1 to quit: -1
The game ended when it just started.
======Thanks for playing Dinosaur Game, see you next time======
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`--auto`

標準輸入：
```text
0
0
111111111
-1
```

標準輸出：
```text
======Welcome to the Dinosaur Game======
Please enter the first layer initial obstacle setup of the Dinosaur Game. Enter -1 to quit: 00000000
Please enter the second layer initial obstacle setup of the Dinosaur Game: 00000000
Please enter the initial distribution of gap in the ceiling, 0 for gap, 1 for complete ceiling: 111111111
|  |  |  |  |  |  |  |  |  |
|  |  |  |  |  |  |  |  |  |
|  |  |  |  |  |  |  |  |  |
|  |  |  |  |  |  |  |  |  |
============================
|  |  |  |  |  |  |  |  |  |
|  |  |  |  |  |  |  |  |  |
| D|  |  |  |  |  |  |  |  |
| D|  |  |  |  |  |  |  |  |
Please enter a command to control the dinosaur, 0 for no action, 1=jump, 2=crouch: 0
Please enter the next round's first layer obstacle setup of the Dinosaur Game. Enter -1 to quit: -1
Congratulations, you WIN the game!!!
======Thanks for playing Dinosaur Game, see you next time======
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 solution 程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>

// All three modes use the same transition function and collision detector.
const int LIMIT = 10000;
static int maps[LIMIT + 2][3][9];
static int raw[LIMIT + 2][3];
static signed char parent_state[LIMIT + 2][56];
static signed char parent_command[LIMIT + 2][56];
static int commands[LIMIT + 2];
bool echo_input = true;

bool equal(const char* a, const char* b) {
    while (*a && *a == *b) { ++a; ++b; }
    return *a == *b;
}
void number(int n, int width) {
    if (n < 0) { std::cout << n; return; }
    int divisor = 1;
    for (int i = 1; i < width; ++i) divisor *= 10;
    while (divisor) { std::cout << n / divisor; n %= divisor; divisor /= 10; }
}
void decode(int value, int output[9], int size) {
    for (int x = size - 1; x >= 0; --x) { output[x] = value % 10; value /= 10; }
}
int encode(int y, int mode, bool crouch) { return y * 8 + mode * 2 + crouch; }

// mode 1: first ascent/apex; mode 2: ascent/apex on the upper floor;
// mode 0: standing or descending. The map has not shifted yet.
int transition(int state, int command, const int ceiling[9], bool layers) {
    int y = state / 8, mode = (state % 8) / 2;
    if (command == 1) {
        if (y == 0) return encode(1, 1, false);
        if (layers && y == 2 && mode == 1 && ceiling[1] == 0)
            return encode(3, 2, false);
        if (layers && y == 4 && mode == 0 && ceiling[0] && ceiling[1])
            return encode(5, 2, false);
        return -1;
    }
    if (command == 2) return encode(layers && y >= 4 && ceiling[1] ? 4 : 0, 0, true);
    if (mode == 1 && y == 1) return encode(2, 1, false);
    if (mode == 2 && y < 6) return encode(y + 1, 2, false);
    if (y == 0 || (layers && y == 4 && ceiling[1])) return encode(y, 0, false);
    return encode(y - 1, 0, false);
}
char obstacle(int code, int y) {
    if ((code == 1 && y == 0) || (code == 2 && y <= 1)) return 'C';
    if ((code == 3 && y == 1) || (code == 4 && y == 2)) return 'P';
    return ' ';
}
bool collision(int state, const int map[3][9], bool layers) {
    int y = state / 8, height = state % 2 ? 1 : 2;
    for (int k = 0; k < height; ++k) {
        int row = y + k, floor = row / 4;
        if (floor < (layers ? 2 : 1) && obstacle(map[floor][0], row % 4) != ' ') return true;
    }
    return false;
}
void draw(int state, const int map[3][9], bool layers) {
    int y = state / 8, height = state % 2 ? 1 : 2;
    for (int row = layers ? 7 : 3; row >= 0; --row) {
        for (int x = 0; x < 9; ++x) {
            char c = x == 0 && row >= y && row < y + height ? 'D' : obstacle(map[row / 4][x], row % 4);
            std::cout << "| " << c;
        }
        std::cout << "|\n";
        if (layers && row == 4) {
            for (int x = 0; x < 9; ++x) std::cout << (map[2][x] ? "===" : "=  ");
            std::cout << "=\n";
        }
    }
}
void farewell(const char* message) {
    std::cout << message << "\n======Thanks for playing Dinosaur Game, see you next time======\n";
}
const char* command_prompt = "Please enter a command to control the dinosaur, 0 for no action, 1=jump, 2=crouch: ";
void setup_prompt(bool initial, int layer, bool layers) {
    if (!layers) std::cout << (initial ? "Please enter the initial obstacle setup of the Dinosaur Game. Enter -1 to quit: " : "Please enter the next round's obstacle setup of the Dinosaur Game. Enter -1 to quit: ");
    else if (layer == 0) std::cout << (initial ? "Please enter the first layer initial obstacle setup of the Dinosaur Game. Enter -1 to quit: " : "Please enter the next round's first layer obstacle setup of the Dinosaur Game. Enter -1 to quit: ");
    else if (layer == 1) std::cout << (initial ? "Please enter the second layer initial obstacle setup of the Dinosaur Game: " : "Please enter the next round's second layer obstacle setup of the Dinosaur Game: ");
    else std::cout << (initial ? "Please enter the initial distribution of gap in the ceiling, 0 for gap, 1 for complete ceiling: " : "Please enter the next round's ceiling or gap status: ");
}
void echo(int value, bool initial, int layer) {
    if (echo_input) { number(value, initial ? (layer == 2 ? 9 : 8) : 1); std::cout << '\n'; }
}
bool read_map(int tick, bool initial, bool layers, bool autoplay) {
    for (int layer = 0; layer < (layers ? 3 : 1); ++layer) {
        if (!autoplay) setup_prompt(initial, layer, layers);
        if (!(std::cin >> raw[tick][layer])) return false;
        if (!autoplay) echo(raw[tick][layer], initial, layer);
        if (layer == 0 && raw[tick][0] == -1) return true;
        if (initial) {
            if (layer == 2) decode(raw[tick][layer], maps[tick][layer], 9);
            else decode(raw[tick][layer], maps[tick][layer] + 1, 8);
        } else {
            for (int x = 0; x < 8; ++x) maps[tick][layer][x] = maps[tick - 1][layer][x + 1];
            maps[tick][layer][8] = raw[tick][layer];
        }
    }
    return true;
}
int main(int argc, char** argv) {
    bool layers = false, autoplay = false;
    for (int i = 1; i < argc; ++i) {
        if (equal(argv[i], "--double")) layers = true;
        else if (equal(argv[i], "--auto")) layers = autoplay = true;
        else if (equal(argv[i], "--no-echo")) echo_input = false;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    std::cout << "======Welcome to the Dinosaur Game======\n";
    if (!read_map(0, true, layers, autoplay)) return 1;
    if (autoplay) { setup_prompt(true, 0, true); echo(raw[0][0], true, 0); }
    if (raw[0][0] == -1) { farewell("The game ended when it just started."); return 0; }
    if (!autoplay) {
        int state = 0;
        draw(state, maps[0], layers);
        for (int tick = 1; tick <= LIMIT; ++tick) {
            std::cout << command_prompt;
            int next = -1;
            while (next < 0) {
                double command;
                if (!(std::cin >> command)) return 1;
                if (echo_input) std::cout << command << '\n';
                if (command != 0 && command != 1 && command != 2)
                    std::cout << "The dinosaur doesn't understand this command, try another one: ";
                else {
                    next = transition(state, static_cast<int>(command), maps[tick - 1][2], layers);
                    if (next < 0) std::cout << "The dinosaur cannot jump again, please enter another command: ";
                }
            }
            if (!read_map(tick, false, layers, false)) return 1;
            if (raw[tick][0] == -1) { farewell("Congratulations, you WIN the game!!!"); return 0; }
            state = next;
            if (collision(state, maps[tick], layers)) { farewell("What a pity, it was close :("); return 0; }
            draw(state, maps[tick], layers);
        }
        std::cerr << "Too many ticks\n"; return 1;
    }
    int count = 0;
    while (count <= LIMIT && read_map(count + 1, false, true, true)) {
        if (raw[count + 1][0] == -1) break;
        if (count == LIMIT) { std::cerr << "Too many ticks\n"; return 1; }
        ++count;
    }
    if (raw[count + 1][0] != -1) { std::cerr << "Missing terminator or too many ticks\n"; return 1; }
    for (int t = 0; t <= count; ++t) for (int s = 0; s < 56; ++s) parent_state[t][s] = -1;
    parent_state[0][0] = 0;
    for (int t = 1; t <= count; ++t)
        for (int s = 0; s < 56; ++s) if (parent_state[t - 1][s] >= 0)
            for (int c = 0; c < 3; ++c) {
                int next = transition(s, c, maps[t - 1][2], true);
                if (next >= 0 && !collision(next, maps[t], true) && parent_state[t][next] < 0) {
                    parent_state[t][next] = static_cast<signed char>(s);
                    parent_command[t][next] = static_cast<signed char>(c);
                }
            }
    int state = -1;
    for (int s = 0; s < 56; ++s) if (parent_state[count][s] >= 0) { state = s; break; }
    if (state < 0) { std::cerr << "No winning path exists\n"; return 2; }
    for (int t = count; t > 0; --t) { commands[t] = parent_command[t][state]; state = parent_state[t][state]; }
    for (int layer = 1; layer < 3; ++layer) { setup_prompt(true, layer, true); echo(raw[0][layer], true, layer); }
    draw(0, maps[0], true);
    for (int t = 1; t <= count + 1; ++t) {
        int c = t <= count ? commands[t] : 0;
        std::cout << command_prompt << c << '\n';
        for (int layer = 0; layer < (t <= count ? 3 : 1); ++layer) { setup_prompt(false, layer, true); echo(raw[t][layer], false, layer); }
        if (t <= count) { state = transition(state, c, maps[t - 1][2], true); draw(state, maps[t], true); }
    }
    farewell("Congratulations, you WIN the game!!!");
}
```

<!-- SOLUTION_CODE_END -->
