# 2021 Final Project：Design Synthesis — 重建版，非教授原題

## 題目

給定 binary-coded state table，自動推導正反器的激勵函數與電路輸出。依狀態變數、輸入變數建立 K-map，輸出 `kmap_in.txt`，呼叫 `void top()` 產生最小 SOP 的 `kmap_out.txt`，重新讀入化簡結果，最後產生以 NOT、AND、OR 和正反器描述的 benchmark 電路。

一般版只使用 D 正反器，present state 位元加外部輸入的數量最多 4。更新問答要求盡量支援任意列順序與名稱，允許多輸入邏輯閘。所提供資料沒有另一份正式 Final bonus 題面；此 repo 的 bonus 是明示擴充：D/T/JK、最多 8 個 state+input 變數、選擇性不完整狀態表。

## 精確介面

```text
synth state_table.txt circuit.bench
synth_bonus state_table.txt circuit.bench [--allow-incomplete]
```

成功回傳 0，stdout 與 stderr 都為空；沒有互動輸入。格式、檔案或參數錯誤回傳 1，stderr 為 `Error: `、原因及換行。第二個參數是產生的 benchmark 檔。程式另外在**目前工作目錄**寫出固定檔名 `kmap_in.txt` 與 `kmap_out.txt`。輸入、輸出參數文字不得相同，兩個路徑的檔名部分都不能是 `kmap_in.txt` 或 `kmap_out.txt`，這項檢查忽略英文字母大小寫；不同 run 應使用不同目錄。

## 狀態表輸入

第一個宣告是 `Flip-Flop: D`；bonus 可用 `T` 或 `JK`。空白行忽略。四個 section 每個恰出現一次，各 section 的數值列依序配對形成同一筆狀態表列：

```text
//Present(A,B)
0 0
...
//Input(x)
0
...
//Next(A,B)
0 0
...
//Output(y)
0
...
```

`//Present` 是目前狀態，`//Input` 是外部輸入，`//Next` 是下一個狀態，`//Output` 是組合輸出。括號內以逗號分隔名稱，首字母為英文字母或 `_`，後續可為英數字或 `_`。只有 `0` 表示該 section 沒有節點，且後面不能有資料列。名稱列表不能空白。section 順序、同一狀態表所有 section 的列順序可任意；必須一致地重排每個 section 的資料列。Next 名稱必須與 Present 名稱集合相同，其欄位順序可以不同。

每列恰有該 section 名稱數量的位元，位元只接受文字 `0` 或 `1`。所有非空 section 的列數相同。完整表必須包含 present+input 的全部 2ⁿ 組合，不能重複。沒有變數時有 1 組組合；若所有 section 都是 `(0)`，接受沒有資料列的空電路。

Present、Input、Output 的名稱彼此不同，不能用 `CLK`，也不能與自動產生的激勵名稱衝突。D/T 以 `D`/`T` 加狀態名稱命名激勵，JK 以 `J`、`K` 各加狀態名稱。產生的中間線 `N1`、`N2`…會跳過已有名稱，反相線名稱是 `!` 加原名。

一般版 n≤4；bonus n≤8，激勵函數加 Output 函數總數不超過 10,000。bonus 的 `--allow-incomplete` 允許省略部分 present+input 組合，這些組合的激勵及輸出全作 don't-care；重複組合仍然是錯誤。未帶此選項的任何版本均拒絕不完整表。資料欄的 `2` 不是合法輸入位元；don't-care 由 JK 激勵或缺列規則自行產生。

## 激勵、K-map 與最小化

變數順序是 Present 名稱順序，再接 Input 名稱順序，首變數為最高位元。對一列的目前狀態 Q、下一狀態 Q′：

| 正反器 | 激勵函數 |
|---|---|
| D | D = Q′ |
| T（擴充） | T = Q XOR Q′ |
| JK（擴充） | Q=0 時 J=Q′、K=don't-care；Q=1 時 J=don't-care、K=!Q′ |

函數順序為每個 Present 狀態的激勵；JK 每個狀態先 J 再 K；最後為所有 Output。`kmap_in.txt` 第一行是函數數量，各函數占兩行：變數數 n；以及 2ⁿ 個格子的值。由上而下、左而右列印；row 取前 floor(n/2) 個變數，column 取後 ceil(n/2) 個變數，各方向以 Gray code 排列。值為 `0`、`1`，擴充可有 don't-care `2`。n=0 時第二行恰有 1 個值。

自寫 `kmap.cpp` 的 `void top()` 讀入此檔，透過 QM 與 Petrick 求每個函數精確最少 SOP 項數，再比較 literal 總數和固定排序。它輸出 `kmap_out.txt`：每個函數先一行 `#1`、`#2`…，後面每一行一個 cube，每列恰 n 個空白分隔的 `0/1/2`，依原變數順序表示反相、正相及忽略 literal。恆假只有 `#k` 而沒有 cube；n>0 的恆真為全 `2` cube。n=0 恆真補定以單獨一行 `1` 表示，恆假仍沒有 cube。

## Benchmark 輸出

前五行為：

```text
# <數量> inputs
# <數量> outputs
# <數量> inverters
# <數量> flip-flops
# <AND+OR數量> gates (<AND數量> AND + <OR數量> OR)
```

之後空一行，外部輸入每個一行 `INPUT(name)`，空一行，外部輸出每個一行 `OUTPUT(name)`，再空一行列出連接。沒有對應 port 時該區只有空行。輸入不包含 present state 或 `CLK`。反相共用一個 `!name = NOT(name)`；相同乘積項跨函數共用 `Nx = AND(literal,...)`；兩個以上項為 `target = OR(term,...)`。所有 AND/OR 可有多輸入。

單一 literal、單一乘積項及常數使用 `target = BUFF(term)` 明示 alias。`0`、`1` 是常數。這是補定的 BENCH 相容連接語法；BUFF 不計入第五行 AND/OR 數，另在尾部以 `# <數量> buffers` 列出。沒有為單一項製造假的單輸入 AND/OR。正反器連接為 `A = D(DA,CLK)`，擴充為 `A = T(TA,CLK)` 或 `A = JK(JA,KA,CLK)`。最後一行 `#END`。

此版保證每個函數的 SOP 最小，並共用相同 NOT/AND 節點；沒有宣稱找到所有可因式分解的全電路最少 gate 數。官方問答允许不同 Nx 分配、AND 引數順序或等價化簡，只要連線結果正確。

## 範例

前兩個範例重寫公開 DFF 狀態表。第一例有 2 個 state、1 個 input、1 個 output；下一狀態為 A′=x(A+B)、B′=x(A+!B)，輸出 y=AB。其 SOP 可共用 xA、xB、x!B 三個 AND，總計 4 個 AND、2 個 OR、1 個 NOT。第二例是 A′=A+xy，沒有外部輸出。第三例為本版 JK 擴充，示範不完整表的 don't-care；只保證列出的兩個組合符合指定下一狀態及輸出。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`states.txt circuit.bench`

輸入檔 `states.txt`：
```text
Flip-Flop: D
//Present(A,B)
0 0
0 0
0 1
0 1
1 0
1 0
1 1
1 1
//Input(x)
0
1
0
1
0
1
0
1
//Next(A,B)
0 0
0 1
0 0
1 0
0 0
1 1
0 0
1 1
//Output(y)
0
0
0
0
0
0
1
1
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `kmap_in.txt`：
```text
3
3
0 0 1 0 0 1 1 0
3
0 1 0 0 0 1 1 0
3
0 0 0 0 0 0 1 1
```

產生檔 `kmap_out.txt`：
```text
#1
1 2 1
2 1 1
#2
1 2 1
2 0 1
#3
1 1 2
```

產生檔 `circuit.bench`：
```text
# 1 inputs
# 1 outputs
# 1 inverters
# 2 flip-flops
# 6 gates (4 AND + 2 OR)

INPUT(x)

OUTPUT(y)

!B = NOT(B)
N1 = AND(A,x)
N2 = AND(B,x)
N3 = AND(!B,x)
N4 = AND(A,B)
DA = OR(N1,N2)
DB = OR(N1,N3)
y = BUFF(N4)
A = D(DA,CLK)
B = D(DB,CLK)
# 1 buffers
#END
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`states.txt circuit.bench`

輸入檔 `states.txt`：
```text
Flip-Flop: D
//Present(A)
0
0
0
0
1
1
1
1
//Input(x,y)
0 0
0 1
1 0
1 1
0 0
0 1
1 0
1 1
//Next(A)
0
0
0
1
1
1
1
1
//Output(0)
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `kmap_in.txt`：
```text
1
3
0 0 1 0 1 1 1 1
```

產生檔 `kmap_out.txt`：
```text
#1
1 2 2
2 1 1
```

產生檔 `circuit.bench`：
```text
# 2 inputs
# 0 outputs
# 0 inverters
# 1 flip-flops
# 2 gates (1 AND + 1 OR)

INPUT(x)
INPUT(y)


N1 = AND(x,y)
DA = OR(A,N1)
A = D(DA,CLK)
# 0 buffers
#END
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`bonus.cpp`）

執行參數：`states.txt circuit.bench --allow-incomplete`

輸入檔 `states.txt`：
```text
Flip-Flop: JK
//Present(Q)
0
1
//Input(x)
0
1
//Next(Q)
1
0
//Output(out)
1
0
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `kmap_in.txt`：
```text
3
2
1 2 2 2
2
2 2 2 1
2
1 2 2 0
```

產生檔 `kmap_out.txt`：
```text
#1
2 2
#2
2 2
#3
0 2
```

產生檔 `circuit.bench`：
```text
# 1 inputs
# 1 outputs
# 1 inverters
# 1 flip-flops
# 0 gates (0 AND + 0 OR)

INPUT(x)

OUTPUT(out)

!Q = NOT(Q)
JQ = BUFF(1)
KQ = BUFF(1)
out = BUFF(!Q)
Q = JK(JQ,KQ,CLK)
# 3 buffers
#END
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 解法與核心

先解析 section 並核對名稱、寬度及列數；將各列的 present+input 編為唯一的整數地址，從 Next 名稱查找對應欄位。依激勵表填入 truth table，再以 Gray-grid 位置轉換輸出。`top()` 完成最小化後，主程式**重新讀取** `kmap_out.txt`，建立 inverter、共享 product、各目標 OR/BUFF、最後加上正反器。因此沒有繞過兩個官方中介檔流程。

以下為現有解法核心，完整程式在後方。此版是相容介面的重新實作，沒有直接複製原參考 `kmap.cpp`。

```cpp
if (t.ff == "D") t.functions[f++][address] = next;
else if (t.ff == "T") t.functions[f++][address] = q ^ next;
else {
    t.functions[f++][address] = q ? 2 : next;
    t.functions[f++][address] = q ? 1 - next : 2;
}
write_kmap(t);
top();
write_bench(t, read_minimized(t), output);
```

精確 QM/Petrick 的最差時間、空間為指數量級，n≤8 是格式與實用小電路上限，並非任意複雜函數的執行時間保證。常數、沒有 input、沒有 output、沒有 state、列重排及多輸出均有明確處理。

## 已知與補定、驗證報告

- 已知：DFF、present+input≤4、CLI 檔名、四種 section 與 `(0)`、K-map Gray 順序、`void top()`、中介 cube 格式、benchmark port / Nx / !name 格式、多輸入 gate、等價電路排序可不同。
- 補定：自己重寫 `kmap.cpp` 與精確求解器、常數/BUFF、零變數中介 marker、完整表與錯誤檢查、名字衝突檢查、共享 product、T/JK、n≤8、選擇性缺列 don't-care。沒有另一份原始 bonus 規則可聲稱遵循。
- 驗證方式：直接比較兩組公開 K-map 數值、獨立解析 benchmark 並按 D/T/JK 特性對全部列模擬下一狀態與輸出；隨機小表使用重新命名、打散列及 section、Next 欄位重排。另涵蓋 n=0、常數、無 port、8 變數、缺列模式及錯誤格式。SOP 核心另外以全部三變數函數的獨立 coverage DP 驗證最小性。

## 完整程式

`main.cpp`、`bonus.cpp` 為分開入口，各自需要 `synthesis.hpp`、`kmap.cpp`、`logic.hpp`。只編譯選定入口；不要同時連結兩個 main。

<!-- SOLUTION_CODE_BEGIN -->

### `bonus.cpp`

```cpp
#include "synthesis.hpp"
int main(int argc, char* argv[]) { return synthesis::run(argc, argv, true); }
```

### `kmap.cpp`

```cpp
#ifndef COURSE_KMAP_CPP
#define COURSE_KMAP_CPP
#include "logic.hpp"
#include <fstream>
#include <sstream>

inline std::uint32_t gray_assignment(unsigned n, std::uint32_t position) {
    const unsigned columns = (n + 1) / 2;
    const std::uint32_t low = (std::uint32_t(1) << columns) - 1;
    const std::uint32_t row = position >> columns, col = position & low;
    return ((row ^ (row >> 1)) << columns) | (col ^ (col >> 1));
}
// Self-written compatible replacement for the documented kmap.cpp interface.
inline void top() {
    std::ifstream in("kmap_in.txt");
    if (!in) throw std::runtime_error("cannot open kmap_in.txt");
    int count_value;
    if (!(in >> count_value) || count_value < 0 || count_value > 10000)
        throw std::runtime_error("invalid K-map function count");
    const unsigned count = static_cast<unsigned>(count_value);
    std::ostringstream result;
    for (unsigned f = 0; f < count; ++f) {
        int n_value;
        if (!(in >> n_value) || n_value < 0 || n_value > 8)
            throw std::runtime_error("invalid K-map variable count");
        const unsigned n = static_cast<unsigned>(n_value);
        std::vector<std::uint32_t> on, dc;
        for (std::uint32_t i = 0; i < (std::uint32_t(1) << n); ++i) {
            int value;
            if (!(in >> value) || value < 0 || value > 2) throw std::runtime_error("invalid K-map value");
            if (value == 1) on.push_back(gray_assignment(n, i));
            if (value == 2) dc.push_back(gray_assignment(n, i));
        }
        const std::vector<logic::Cube> answer = logic::minimize(n, on, dc);
        result << '#' << f + 1 << '\n';
        for (const logic::Cube& c : answer) {
            const std::string p = logic::pattern(c, n);
            for (unsigned j = 0; j < n; ++j) result << (j ? " " : "") << p[j];
            // For zero-variable functions a 1 is a unique constant-one marker.
            if (n == 0) result << '1';
            result << '\n';
        }
    }
    std::string excess;
    if (in >> excess) throw std::runtime_error("extra data in kmap_in.txt");
    std::ofstream out("kmap_out.txt");
    if (!out || !(out << result.str())) throw std::runtime_error("cannot write kmap_out.txt");
}
#endif
```

### `logic.hpp`

```cpp
#ifndef COURSE_LOGIC_HPP
#define COURSE_LOGIC_HPP
#include <algorithm>
#include <cstdint>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

// A fixed bit in mask is a literal; a zero bit is a don't-care.
namespace logic {
struct Cube {
    std::uint32_t bits, mask;
    Cube(std::uint32_t b = 0, std::uint32_t m = 0) : bits(b & m), mask(m) {}
    bool operator<(const Cube& c) const {
        return mask < c.mask || (mask == c.mask && bits < c.bits);
    }
    bool operator==(const Cube& c) const { return bits == c.bits && mask == c.mask; }
    bool covers(std::uint32_t m) const { return (m & mask) == bits; }
};
inline unsigned popcount(std::uint32_t n) {
    unsigned c = 0;
    while (n) { n &= n - 1; ++c; }
    return c;
}
inline std::string pattern(const Cube& c, unsigned n) {
    std::string s;
    for (unsigned i = n; i > 0; --i) {
        const std::uint32_t b = std::uint32_t(1) << (i - 1);
        s += !(c.mask & b) ? '2' : (c.bits & b ? '1' : '0');
    }
    return s;
}
inline bool subset(const std::vector<unsigned>& a, const std::vector<unsigned>& b) {
    return std::includes(b.begin(), b.end(), a.begin(), a.end());
}
inline std::vector<Cube> minimize(unsigned n, std::vector<std::uint32_t> on,
                                  std::vector<std::uint32_t> dc = std::vector<std::uint32_t>()) {
    if (n > 26) throw std::runtime_error("at most 26 Boolean variables are supported");
    const std::uint32_t full = (std::uint32_t(1) << n) - 1;
    std::sort(on.begin(), on.end());
    on.erase(std::unique(on.begin(), on.end()), on.end());
    if (on.empty()) return std::vector<Cube>();
    std::set<Cube> current, primes;
    for (std::uint32_t m : on) {
        if (m > full) throw std::runtime_error("minterm outside variable range");
        current.insert(Cube(m, full));
    }
    for (std::uint32_t m : dc) {
        if (m > full) throw std::runtime_error("don't-care outside variable range");
        current.insert(Cube(m, full));
    }
    // Quine-McCluskey: combine cubes with exactly one differing fixed bit.
    while (!current.empty()) {
        std::set<Cube> combined, used;
        for (const Cube& c : current) {
            for (unsigned i = 0; i < n; ++i) {
                const std::uint32_t bit = std::uint32_t(1) << i;
                if (!(c.mask & bit) || (c.bits & bit)) continue;
                const Cube other(c.bits | bit, c.mask);
                if (current.count(other)) {
                    used.insert(c); used.insert(other);
                    combined.insert(Cube(c.bits, c.mask & ~bit));
                }
            }
        }
        for (const Cube& c : current) if (!used.count(c)) primes.insert(c);
        current.swap(combined);
    }
    std::vector<Cube> p;
    for (const Cube& c : primes) {
        for (std::uint32_t m : on) if (c.covers(m)) { p.push_back(c); break; }
    }
    std::sort(p.begin(), p.end(), [n](const Cube& a, const Cube& b) {
        return pattern(a, n) < pattern(b, n);
    });
    std::vector<std::vector<unsigned> > chart;
    std::set<unsigned> essential;
    for (std::uint32_t m : on) {
        std::vector<unsigned> choices;
        for (unsigned i = 0; i < p.size(); ++i) if (p[i].covers(m)) choices.push_back(i);
        if (choices.empty()) throw std::runtime_error("uncovered minterm");
        if (choices.size() == 1) essential.insert(choices[0]);
        chart.push_back(choices);
    }
    std::vector<std::vector<unsigned> > clauses;
    for (const std::vector<unsigned>& row : chart) {
        bool done = false;
        for (unsigned i : row) if (essential.count(i)) done = true;
        if (!done) clauses.push_back(row);
    }
    std::sort(clauses.begin(), clauses.end(), [](const std::vector<unsigned>& a,
                                                const std::vector<unsigned>& b) {
        return a.size() < b.size() || (a.size() == b.size() && a < b);
    });
    clauses.erase(std::unique(clauses.begin(), clauses.end()), clauses.end());
    // Petrick's method: multiply sums of prime indices. X + XY = X
    // absorbs strict supersets after each multiplication.
    std::vector<std::vector<unsigned> > products(1);
    for (const std::vector<unsigned>& clause : clauses) {
        std::set<std::vector<unsigned> > next;
        for (const std::vector<unsigned>& product : products) {
            for (unsigned choice : clause) {
                std::vector<unsigned> term = product;
                const auto pos = std::lower_bound(term.begin(), term.end(), choice);
                if (pos == term.end() || *pos != choice) term.insert(pos, choice);
                next.insert(term);
            }
        }
        std::vector<std::vector<unsigned> > ordered(next.begin(), next.end());
        std::sort(ordered.begin(), ordered.end(), [](const std::vector<unsigned>& a,
                                                    const std::vector<unsigned>& b) {
            return a.size() < b.size() || (a.size() == b.size() && a < b);
        });
        products.clear();
        for (const std::vector<unsigned>& term : ordered) {
            bool absorbed = false;
            for (const std::vector<unsigned>& kept : products) {
                if (subset(kept, term)) { absorbed = true; break; }
            }
            if (!absorbed) products.push_back(term);
        }
    }
    std::vector<unsigned> best;
    unsigned best_literals = 0;
    bool have_best = false;
    for (std::vector<unsigned> term : products) {
        term.insert(term.end(), essential.begin(), essential.end());
        std::sort(term.begin(), term.end());
        unsigned literals = 0;
        for (unsigned i : term) literals += popcount(p[i].mask);
        if (!have_best || term.size() < best.size() ||
            (term.size() == best.size() && (literals < best_literals ||
             (literals == best_literals && term < best)))) {
            best = term; best_literals = literals; have_best = true;
        }
    }
    std::vector<Cube> result;
    for (unsigned i : best) result.push_back(p[i]);
    return result;
}
inline std::string sop(const std::vector<Cube>& cubes, const std::vector<std::string>& names) {
    if (cubes.empty()) return "0";
    std::string expression;
    for (const Cube& c : cubes) {
        if (!expression.empty()) expression += "+";
        if (!c.mask) { expression += "1"; continue; }
        expression += "(";
        bool first = true;
        for (unsigned i = 0; i < names.size(); ++i) {
            const std::uint32_t bit = std::uint32_t(1) << (names.size() - 1 - i);
            if (!(c.mask & bit)) continue;
            if (!first) expression += "*";
            if (!(c.bits & bit)) expression += "!";
            expression += names[i]; first = false;
        }
        expression += ")";
    }
    return expression;
}
} // namespace logic
#endif
```

### `main.cpp`

```cpp
#include "synthesis.hpp"
int main(int argc, char* argv[]) { return synthesis::run(argc, argv, false); }
```

### `synthesis.hpp`

```cpp
#ifndef COURSE_SYNTHESIS_HPP
#define COURSE_SYNTHESIS_HPP
#include "kmap.cpp"
#include <cctype>
#include <iostream>
#include <map>

namespace synthesis {
inline std::string trim(const std::string& s) {
    const std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
inline bool identifier(const std::string& s) {
    if (s.empty() || !(std::isalpha(static_cast<unsigned char>(s[0])) || s[0] == '_')) return false;
    for (char c : s) if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) return false;
    return true;
}
struct Section {
    std::vector<std::string> names;
    std::vector<std::vector<int> > rows;
};
struct Table {
    std::string ff;
    Section present, input, next, output;
    unsigned variables;
    std::vector<std::vector<int> > functions;
    std::vector<std::string> targets;
};
inline Section& section(Table& t, const std::string& label) {
    if (label == "Present") return t.present;
    if (label == "Input") return t.input;
    if (label == "Next") return t.next;
    if (label == "Output") return t.output;
    throw std::runtime_error("unknown state-table section");
}
inline Table read_table(const std::string& filename, bool extended, bool incomplete) {
    std::ifstream in(filename.c_str());
    if (!in) throw std::runtime_error("cannot open state table");
    Table t;
    Section* active = nullptr;
    std::set<std::string> seen;
    bool have_ff = false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.compare(0, 10, "Flip-Flop:") == 0) {
            if (have_ff) throw std::runtime_error("duplicate Flip-Flop declaration");
            t.ff = trim(line.substr(10)); have_ff = true; active = nullptr;
            if (t.ff != "D" && !(extended && (t.ff == "T" || t.ff == "JK")))
                throw std::runtime_error("unsupported flip-flop type");
            continue;
        }
        if (line.compare(0, 2, "//") == 0) {
            const std::size_t left = line.find('('), right = line.find(')');
            if (left == std::string::npos || right == std::string::npos || right < left ||
                !trim(line.substr(right + 1)).empty()) throw std::runtime_error("invalid section header");
            const std::string label = trim(line.substr(2, left - 2));
            active = &section(t, label);
            if (!seen.insert(label).second) throw std::runtime_error("duplicate section");
            const std::string names = trim(line.substr(left + 1, right - left - 1));
            if (names != "0") {
                std::istringstream list(names);
                std::string name;
                std::set<std::string> unique;
                while (std::getline(list, name, ',')) {
                    name = trim(name);
                    if (!identifier(name) || !unique.insert(name).second)
                        throw std::runtime_error("invalid or duplicate node name");
                    active->names.push_back(name);
                }
                if (active->names.empty() || (!names.empty() && names.back() == ','))
                    throw std::runtime_error("empty node name");
            }
            continue;
        }
        if (!active || active->names.empty()) throw std::runtime_error("row outside nonempty section");
        std::istringstream row(line);
        std::vector<int> values;
        std::string token;
        while (row >> token) {
            if (token != "0" && token != "1") throw std::runtime_error("state-table values must be 0 or 1");
            values.push_back(token[0] - '0');
        }
        if (values.size() != active->names.size()) throw std::runtime_error("incorrect row width");
        active->rows.push_back(values);
    }
    if (!have_ff || seen.size() != 4) throw std::runtime_error("missing state-table declaration or section");
    if (std::set<std::string>(t.present.names.begin(), t.present.names.end()) !=
        std::set<std::string>(t.next.names.begin(), t.next.names.end()))
        throw std::runtime_error("Next nodes must match Present nodes");
    t.variables = static_cast<unsigned>(t.present.names.size() + t.input.names.size());
    if (t.variables > (extended ? 8U : 4U)) throw std::runtime_error("too many state/input variables");
    if (t.output.names.size() + (t.ff == "JK" ? 2 : 1) * t.present.names.size() > 10000)
        throw std::runtime_error("too many output functions");
    std::set<std::string> used;
    used.insert("CLK");
    for (const Section* s : std::vector<const Section*>{&t.present, &t.input, &t.output})
        for (const std::string& name : s->names)
            if (!used.insert(name).second) throw std::runtime_error("node names must be globally distinct and not CLK");
    for (const std::string& name : t.present.names) {
        if (t.ff == "JK") { t.targets.push_back("J" + name); t.targets.push_back("K" + name); }
        else t.targets.push_back(t.ff + name);
    }
    for (const std::string& target : t.targets)
        if (!used.insert(target).second) throw std::runtime_error("node conflicts with flip-flop excitation name");
    t.targets.insert(t.targets.end(), t.output.names.begin(), t.output.names.end());
    std::size_t row_count = 0;
    bool have_row_count = false;
    for (const Section* s : std::vector<const Section*>{&t.present, &t.input, &t.next, &t.output}) {
        if (!s->names.empty()) {
            if (!have_row_count) { row_count = s->rows.size(); have_row_count = true; }
            if (row_count != s->rows.size()) throw std::runtime_error("section row counts differ");
        }
    }
    if (!row_count && t.variables == 0 && t.targets.empty()) row_count = 1;
    if (!row_count) throw std::runtime_error("state table has no rows");
    t.functions.assign(t.targets.size(), std::vector<int>(std::size_t(1) << t.variables, 2));
    std::set<std::uint32_t> assigned;
    for (std::size_t r = 0; r < row_count; ++r) {
        std::uint32_t address = 0;
        for (int v : (t.present.names.empty() ? std::vector<int>() : t.present.rows[r])) address = address * 2 + v;
        for (int v : (t.input.names.empty() ? std::vector<int>() : t.input.rows[r])) address = address * 2 + v;
        if (!assigned.insert(address).second) throw std::runtime_error("duplicate state/input combination");
        std::size_t f = 0;
        for (unsigned i = 0; i < t.present.names.size(); ++i) {
            const auto it = std::find(t.next.names.begin(), t.next.names.end(), t.present.names[i]);
            const int q = t.present.rows[r][i], next = t.next.rows[r][it - t.next.names.begin()];
            if (t.ff == "D") t.functions[f++][address] = next;
            else if (t.ff == "T") t.functions[f++][address] = q ^ next;
            else { t.functions[f++][address] = q ? 2 : next; t.functions[f++][address] = q ? 1 - next : 2; }
        }
        for (unsigned i = 0; i < t.output.names.size(); ++i) t.functions[f++][address] = t.output.rows[r][i];
    }
    if (!incomplete && assigned.size() != (std::size_t(1) << t.variables))
        throw std::runtime_error("missing state/input combinations (bonus --allow-incomplete can mark them don't-care)");
    return t;
}
inline void write_kmap(const Table& t) {
    std::ofstream out("kmap_in.txt");
    if (!out) throw std::runtime_error("cannot write kmap_in.txt");
    out << t.functions.size() << '\n';
    for (const std::vector<int>& function : t.functions) {
        out << t.variables << '\n';
        for (std::uint32_t i = 0; i < (std::uint32_t(1) << t.variables); ++i)
            out << (i ? " " : "") << function[gray_assignment(t.variables, i)];
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed writing kmap_in.txt");
}
inline std::vector<std::vector<logic::Cube> > read_minimized(const Table& t) {
    std::ifstream in("kmap_out.txt");
    if (!in) throw std::runtime_error("cannot open kmap_out.txt");
    std::vector<std::vector<logic::Cube> > functions(t.functions.size());
    std::size_t index = 0;
    bool have_function = false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') {
            std::istringstream marker(line.substr(1));
            int number;
            std::string rest;
            if (!(marker >> number) || (marker >> rest) || number < 1 ||
                static_cast<std::size_t>(number) != index + 1 ||
                static_cast<std::size_t>(number) > functions.size())
                throw std::runtime_error("invalid minimized function label");
            ++index; have_function = true; continue;
        }
        if (!have_function) throw std::runtime_error("cube before function label");
        std::istringstream row(line);
        std::string token;
        logic::Cube c;
        unsigned width = 0;
        while (row >> token) {
            if (token != "0" && token != "1" && token != "2") throw std::runtime_error("invalid minimized cube");
            if (t.variables == 0) {
                if (token != "1" || width) throw std::runtime_error("invalid constant-one marker");
            } else {
                c.bits = (c.bits << 1) | (token == "1" ? 1 : 0);
                c.mask = (c.mask << 1) | (token != "2" ? 1 : 0);
            }
            ++width;
        }
        if (width != (t.variables ? t.variables : 1)) throw std::runtime_error("invalid minimized cube width");
        functions[index - 1].push_back(c);
    }
    if (index != functions.size()) throw std::runtime_error("missing minimized function");
    return functions;
}
inline std::string joined(const std::vector<std::string>& names) {
    std::string s;
    for (const std::string& n : names) { if (!s.empty()) s += ','; s += n; }
    return s;
}
inline void write_bench(const Table& t, const std::vector<std::vector<logic::Cube> >& functions,
                        const std::string& filename) {
    std::vector<std::string> variables = t.present.names;
    variables.insert(variables.end(), t.input.names.begin(), t.input.names.end());
    std::set<std::string> reserved(variables.begin(), variables.end());
    reserved.insert(t.targets.begin(), t.targets.end()); reserved.insert("CLK");
    std::map<std::string, std::string> products;
    std::set<std::string> inverted;
    std::vector<std::string> gates, equations;
    unsigned wire = 1, and_count = 0, or_count = 0, buffer_count = 0;
    for (unsigned f = 0; f < functions.size(); ++f) {
        std::vector<std::string> terms;
        for (const logic::Cube& c : functions[f]) {
            std::vector<std::string> literals;
            for (unsigned i = 0; i < variables.size(); ++i) {
                const std::uint32_t bit = std::uint32_t(1) << (variables.size() - 1 - i);
                if (!(c.mask & bit)) continue;
                std::string literal = variables[i];
                if (!(c.bits & bit)) { inverted.insert(literal); literal = "!" + literal; }
                literals.push_back(literal);
            }
            if (literals.empty()) terms.push_back("1");
            else if (literals.size() == 1) terms.push_back(literals[0]);
            else {
                const std::string key = joined(literals);
                if (!products.count(key)) {
                    std::string net;
                    do { net = "N" + std::to_string(wire++); } while (reserved.count(net));
                    reserved.insert(net); products[key] = net;
                    gates.push_back(net + " = AND(" + key + ")"); ++and_count;
                }
                terms.push_back(products[key]);
            }
        }
        if (terms.size() > 1) { equations.push_back(t.targets[f] + " = OR(" + joined(terms) + ")"); ++or_count; }
        else {
            equations.push_back(t.targets[f] + " = BUFF(" + (terms.empty() ? "0" : terms[0]) + ")");
            ++buffer_count;
        }
    }
    std::ofstream out(filename.c_str());
    if (!out) throw std::runtime_error("cannot write benchmark");
    out << "# " << t.input.names.size() << " inputs\n# " << t.output.names.size() << " outputs\n# "
        << inverted.size() << " inverters\n# " << t.present.names.size() << " flip-flops\n# "
        << and_count + or_count << " gates (" << and_count << " AND + " << or_count << " OR)\n\n";
    for (const std::string& name : t.input.names) out << "INPUT(" << name << ")\n";
    out << '\n';
    for (const std::string& name : t.output.names) out << "OUTPUT(" << name << ")\n";
    out << '\n';
    for (const std::string& name : inverted) out << '!' << name << " = NOT(" << name << ")\n";
    for (const std::string& gate : gates) out << gate << '\n';
    for (const std::string& eq : equations) out << eq << '\n';
    for (const std::string& name : t.present.names) {
        out << name << " = " << t.ff << '(';
        if (t.ff == "JK") out << 'J' << name << ",K" << name;
        else out << t.ff << name;
        out << ",CLK)\n";
    }
    // BUFF nodes are explicit aliases/constants, not AND/OR gates.
    out << "# " << buffer_count << " buffers\n#END\n";
    if (!out) throw std::runtime_error("failed writing benchmark");
}
inline bool intermediate_name(const std::string& path) {
    std::string name = path.substr(path.find_last_of("/\\") == std::string::npos ?
                                  0 : path.find_last_of("/\\") + 1);
    for (char& c : name) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return name == "kmap_in.txt" || name == "kmap_out.txt";
}
inline int run(int argc, char* argv[], bool extended) {
    try {
        bool incomplete = false;
        if (extended && argc == 4 && std::string(argv[3]) == "--allow-incomplete") incomplete = true;
        else if (argc != 3) throw std::runtime_error("usage: synth state_table.txt circuit.bench [--allow-incomplete (bonus)]");
        const std::string input = argv[1], output = argv[2];
        if (input == output || intermediate_name(input) || intermediate_name(output))
            throw std::runtime_error("input/output conflict with intermediate files");
        const Table t = read_table(input, extended, incomplete);
        write_kmap(t); top();
        write_bench(t, read_minimized(t), output);
        return 0;
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << '\n'; return 1; }
}
} // namespace synthesis
#endif
```

<!-- SOLUTION_CODE_END -->
