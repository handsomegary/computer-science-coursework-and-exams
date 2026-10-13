# 2021 PA5：Logic Minimization — 重建版，非教授原題

## 題目

讀入以 `.eqn` 格式描述的單一 Boolean 函數，使用 Quine–McCluskey 求出 prime implicants，再以 Petrick's Method 選擇 cover，輸出與原函數等價、乘積項數最少的 SOP。`!`、`*`、`+` 分別表示 literal 的反相、AND、OR。

所提供的 2021 資料夾只有 QM/Petrick bonus 說明，沒有另一份獨立的 PA5 base 題面。此程式把該 bonus 全部實作，並把較一般的 SOP 當作明示擴充；沒有將未取得的另一份題目假裝成已驗證規格。

## 精確輸入與輸出

批次介面：`logic_min input.eqn output.eqn`。輸入與輸出檔名必須不同。成功回傳 0，stdout、stderr 都是空字串。未給參數時，stdout 依序印出 `Input .eqn file: `、`Output .eqn file: `，每次由 stdin 讀取一整行檔名。其他參數數量、檔案錯誤或格式錯誤回傳 1，stderr 為 `Error: ` 加錯誤原因與換行。

輸入前兩行為：

```text
INORDER = a b c d;
OUTORDER = output;
```

`INORDER` 包含 1–26 個不同的小寫英文字母，其排列順序決定位元順序，第一個變數為最高位元。`OUTORDER` 恰有一個識別字：首字母為英文字母或 `_`，其他字元可為英數字或 `_`。前兩行必須有 `;`，可以帶 `//` 註解。之後各行合起來是 `output = expression`，可換行，最後的 `;` 可有可無；每行 `//` 後的文字忽略。

官方更新問答指定 expression 是 **sum of minterms**：每個有效乘積項包含所有輸入變數各一次。minterm 可重複，程式會去重；literal 可寫成 `a` 或 `!a`。括號可包住乘積項。完整 minterms 在 26 變數時仍直接以稀疏集合處理，沒有建立完整的 2²⁶ 張真值表。

本重建版另外接受常數 `0`、`1`、literal 上的 `!0`、`!1`，以及使用 `+`、`*`、括號的 SOP 或可展開為 SOP 的式子，如 `(a+b)*!c`。`!` 僅能作用在單一 literal 或常數，不能寫 `!(a+b)`。AND 優先於 OR。當式子中有缺少變數的非零乘積項，一般式擴充限制為至多 16 變數；常數 `0`、`1` 本身不限於 16 變數。同一乘積項中的相反 literal 使該項為 0。

輸出前兩行逐字保留輸入，包括空白及註解，但換行統一為 LF。第三行為 `output = SOP` 加換行，沒有結尾分號。每個非恆真乘積項外加括號，literal 依 `INORDER` 排列，用 `*` 連接，項間用 `+`，不另加空白。恆假輸出 `0`，恆真輸出 `1`。

「最小」首先指乘積項數最少；若有多個解，本版其次選 literal 總數最少，再按 cube 的 `0/1/2` pattern 排序與索引選定固定答案，其中 `2` 代表 don't-care。官方允許項目或 literal 順序不同，只要等價且項數最少。原輸入沒有 don't-care 集合，求解器內部另外提供 don't-care 介面供狀態機擴充使用。

## 範例

範例一是公開四變數函數：ON 集合為 `{0,5,6,7,9,10,13,14,15}`。最少需要 5 項、14 個 literal。範例二展示一般 SOP 擴充；程式會消去矛盾項 `a*!a`，結果等價於 `(a+b)*!c`。範例檔內容、執行參數及產生檔案如下。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`input.eqn output.eqn`

輸入檔 `input.eqn`：
```text
INORDER = a b c d;
OUTORDER = output;
output = (!a*!b*!c*!d)+(!a*b*!c*d)+(!a*b*c*!d)+(!a*b*c*d)+(a*!b*!c*d)+(a*!b*c*!d)+(a*b*!c*d)+(a*b*c*!d)+(a*b*c*d)
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `output.eqn`：
```text
INORDER = a b c d;
OUTORDER = output;
output = (!a*!b*!c*!d)+(a*!c*d)+(a*c*!d)+(b*c)+(b*d)
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`input.eqn output.eqn`

輸入檔 `input.eqn`：
```text
INORDER = a b c;
OUTORDER = output;
output = (a+b)*!c + a*!a;
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `output.eqn`：
```text
INORDER = a b c;
OUTORDER = output;
output = (a*!c)+(b*!c)
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 解法

每個 cube 用 `bits` 與 `mask` 表示：mask 的 1 表示該位置保留 literal，0 表示可忽略；bits 表示保留位置的值。兩個 mask 相同且僅一個固定 bit 不同的 cube 合併，移除那個固定 bit。每輪未參與合併的 cube 是 prime implicant；只涵蓋 don't-care、未涵蓋任何 ON 的 prime 不列入 cover。

接著建立 prime chart。只有一個 prime 能涵蓋的 minterm 決定 essential prime。刪除被 essential 涵蓋的行後，每個未覆蓋 minterm 都是一個「可選 prime 的和」。Petrick 的乘法把它們相乘，用排序過的 prime 索引集合代表一項，集合聯集代表相乘，並用 `X + XY = X` 吸收嚴格超集合。最後按上述成本與固定排序選答案。這是 exact cover，不是貪心近似。

現有解法的核心表示與合併片段如下；題面沒有給一段可直接沿用的求解器，此處為本實作。

```cpp
bool covers(std::uint32_t minterm) const {
    return (minterm & mask) == bits;
}
// current contains the same-mask partner:
combined.insert(logic::Cube(c.bits, c.mask & ~bit));
// Petrick absorption: a selected set contained in term makes term redundant.
if (logic::subset(kept, term)) absorbed = true;
```

有限組合邏輯的精確最小 SOP 最差需要指數時間和空間。此版不聲稱任意密集 26 變數函數都能快速完成；26 變數是資料格式與位元表示的範圍，公開測資是稀疏 minterms。一般 SOP 擴充最多枚舉 2¹⁶ 個不同輸入組合，重複展開的項仍可能增加工作量。

## 已知規則、補定與驗證報告

- 已知：C++11、1–26 個小寫變數、前兩行保留、AND–OR / minterm 輸入、QM 加 Petrick、最少 SOP 項、允許等價排序。
- 補定：CLI 加互動檔名介面、常數、一般式的 16 變數限制、語法驗證、literal 次要最佳化、固定排序、錯誤碼及訊息格式。
- 驗證：逐一跑完所有 256 個三變數函數，用獨立的全部 3ⁿ cubes 枚舉加 coverage DP 比較最少項與最少 literal；公開四變數例及 28 個隨機四變數函數也與此模型一致。7、13 變數公開 ON 集合以全部真值組合核對等價性；另驗證 26 變數稀疏集合、常數、一般式、註解與錯誤輸入。

## 完整程式

`main.cpp` 是唯一入口，`logic.hpp` 是所需的完整求解器；依 README 命令編譯 main 即可。

<!-- SOLUTION_CODE_BEGIN -->

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
#include "logic.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
std::string trim(const std::string& s) {
    const std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
std::string uncomment(const std::string& s) { return s.substr(0, s.find("//")); }
std::vector<std::string> header(const std::string& text, const std::string& label) {
    const std::string s = trim(uncomment(text));
    const std::size_t eq = s.find('='), semi = s.find(';');
    if (eq == std::string::npos || semi == std::string::npos || semi < eq ||
        trim(s.substr(0, eq)) != label || !trim(s.substr(semi + 1)).empty())
        throw std::runtime_error("invalid " + label + " header");
    std::istringstream in(s.substr(eq + 1, semi - eq - 1));
    std::vector<std::string> names;
    std::string name;
    while (in >> name) names.push_back(name);
    return names;
}
class Parser {
    std::string text;
    std::size_t pos;
    const std::vector<std::string>& names;
    void space() { while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos; }
    bool take(char c) { space(); if (pos < text.size() && text[pos] == c) { ++pos; return true; } return false; }
    std::vector<logic::Cube> atom() {
        if (take('(')) {
            std::vector<logic::Cube> r = sum();
            if (!take(')')) throw std::runtime_error("missing ')' in expression");
            return r;
        }
        const bool neg = take('!');
        space();
        if (pos >= text.size()) throw std::runtime_error("missing literal");
        const char c = text[pos++];
        if (c == '0' || c == '1') {
            const bool value = (c == '1') != neg;
            return value ? std::vector<logic::Cube>(1, logic::Cube()) : std::vector<logic::Cube>();
        }
        const auto it = std::find(names.begin(), names.end(), std::string(1, c));
        if (it == names.end()) throw std::runtime_error("unknown variable or invalid literal");
        const std::uint32_t bit = std::uint32_t(1) << (names.size() - 1 - (it - names.begin()));
        return std::vector<logic::Cube>(1, logic::Cube(neg ? 0 : bit, bit));
    }
    std::vector<logic::Cube> product() {
        std::vector<logic::Cube> result = atom();
        while (take('*')) {
            const std::vector<logic::Cube> right = atom();
            std::set<logic::Cube> combined;
            for (const logic::Cube& a : result) for (const logic::Cube& b : right) {
                if (!((a.bits ^ b.bits) & a.mask & b.mask))
                    combined.insert(logic::Cube(a.bits | b.bits, a.mask | b.mask));
            }
            result.assign(combined.begin(), combined.end());
        }
        return result;
    }
    std::vector<logic::Cube> sum() {
        std::vector<logic::Cube> result = product();
        while (take('+')) {
            const std::vector<logic::Cube> right = product();
            result.insert(result.end(), right.begin(), right.end());
        }
        return result;
    }
public:
    Parser(const std::string& s, const std::vector<std::string>& ns) : text(s), pos(0), names(ns) {}
    std::vector<logic::Cube> run() {
        std::vector<logic::Cube> r = sum();
        take(';'); space();
        if (pos != text.size()) throw std::runtime_error("unexpected expression suffix");
        std::sort(r.begin(), r.end()); r.erase(std::unique(r.begin(), r.end()), r.end());
        return r;
    }
};
void solve(const std::string& input, const std::string& output) {
    std::ifstream in(input.c_str());
    if (!in) throw std::runtime_error("cannot open input file");
    std::string h1, h2;
    if (!std::getline(in, h1) || !std::getline(in, h2)) throw std::runtime_error("missing headers");
    if (!h1.empty() && h1.back() == '\r') h1.pop_back();
    if (!h2.empty() && h2.back() == '\r') h2.pop_back();
    const std::vector<std::string> vars = header(h1, "INORDER");
    const std::vector<std::string> outs = header(h2, "OUTORDER");
    if (vars.empty() || vars.size() > 26 || outs.size() != 1)
        throw std::runtime_error("requires 1..26 input variables and one output");
    std::set<std::string> unique;
    for (const std::string& v : vars) {
        if (v.size() != 1 || v[0] < 'a' || v[0] > 'z' || !unique.insert(v).second)
            throw std::runtime_error("inputs must be distinct lowercase letters");
    }
    if (outs[0].empty() || !(std::isalpha(static_cast<unsigned char>(outs[0][0])) || outs[0][0] == '_'))
        throw std::runtime_error("invalid output identifier");
    for (char c : outs[0]) if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_'))
        throw std::runtime_error("invalid output identifier");
    std::string expression, line;
    while (std::getline(in, line)) expression += uncomment(line) + " ";
    const std::size_t eq = expression.find('=');
    if (eq == std::string::npos || trim(expression.substr(0, eq)) != outs[0])
        throw std::runtime_error("output assignment does not match OUTORDER");
    const std::vector<logic::Cube> cubes = Parser(expression.substr(eq + 1), vars).run();
    const std::uint32_t full = (std::uint32_t(1) << vars.size()) - 1;
    std::vector<std::uint32_t> on;
    bool constant_one = false;
    for (const logic::Cube& c : cubes) if (c.mask == 0) constant_one = true;
    std::vector<logic::Cube> answer;
    if (constant_one) answer.push_back(logic::Cube());
    else {
        for (const logic::Cube& c : cubes) {
            if (c.mask != full && vars.size() > 16)
                throw std::runtime_error("general SOP extension is limited to 16 variables; use full minterms");
            const std::uint32_t free = full & ~c.mask;
            std::uint32_t part = free;
            do { on.push_back(c.bits | part); if (part == 0) break; part = (part - 1) & free; } while (true);
        }
        answer = logic::minimize(static_cast<unsigned>(vars.size()), on);
    }
    std::ofstream out(output.c_str());
    if (!out) throw std::runtime_error("cannot open output file");
    out << h1 << '\n' << h2 << '\n' << outs[0] << " = " << logic::sop(answer, vars) << '\n';
    if (!out) throw std::runtime_error("failed writing output file");
}
}
int main(int argc, char* argv[]) {
    try {
        std::string input, output;
        if (argc == 1) {
            std::cout << "Input .eqn file: ";
            if (!std::getline(std::cin, input)) throw std::runtime_error("missing input filename");
            std::cout << "Output .eqn file: ";
            if (!std::getline(std::cin, output)) throw std::runtime_error("missing output filename");
        } else if (argc == 3) { input = argv[1]; output = argv[2]; }
        else throw std::runtime_error("usage: logic_min input.eqn output.eqn");
        if (input == output) throw std::runtime_error("input and output filenames must differ");
        solve(input, output);
        return 0;
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << '\n'; return 1; }
}
```

<!-- SOLUTION_CODE_END -->
