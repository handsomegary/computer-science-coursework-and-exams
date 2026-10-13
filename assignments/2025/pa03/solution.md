# PA3：Big Number Energy Distribution Calculator

## 題目

給定一個 N×N 能量矩陣 A，以及 D×D 權重矩陣 W。在每個輸出位置以 stride=1 做 multiply–accumulate，矩陣邊界外的 A 視為 0。D 是奇數，padding 為 `(D-1)/2`，因此輸出仍是 N×N：

`Y[r,c] = sum(A[r+i-padding,c+j-padding] * W[i,j])`，i、j 各取 0..D-1。權重不翻轉，實際是 cross-correlation。

所有能量和權重可有 100 位，乘積會有最多 200 位，累加結果可能更長，故不得以 long long 或浮點數儲存完整數值。請自行實作大整數。輸入與輸出皆使用選定的 base-k，k=2..16，高於 9 的數字使用大寫 A..F。

## 輸入與防呆

依序為一行 base k、一行 N、N 行能量矩陣、一行 D、D 行權重矩陣。每列需恰好有指定數量的整數，不能有算式、括號、小數、非法進位數字或超過 100 位的數值。

輸入錯誤時印 `Invalid input! Please try again.`，要求重新輸入同一個欄位或整列，不把部分合法格子混入結果。基本版只接受非負值；`--bonus` 開啟負數支援。負號不計入 100 位限制，前導零計入輸入位數，結果會去掉前導零，`-000` 會正規化為 `0`。只接受大寫 A..F，`+` 符號未列入本實作語法。

原規格沒有提供 N、D 的明確上限。本實作補定 `1<=N<=100`、`1<=D<=99` 且 D 為奇數，避免不受控的記憶體配置；D 可以大於 N，邊界規則依然有效。若 EOF 使資料不完整，回傳 1；未知命令列參數也回傳 1。

## 輸出

預設印出歡迎訊息、每一步提示及結束訊息，結果矩陣左對齊。每格欄寬為「所有輸出值最大字串長度 +2」，負號計入長度；最後一欄不額外保留行尾空白。值之間至少有兩空白。

`--plain` 為方便自動核對而補充的模式，只印結果矩陣；輸入錯誤訊息仍保留。

## Bonus：負數與全域平均

`--bonus` 同時啟用 signed 大整數與全域平均。將所有 N² 個結果相加，再除以 N²，取最近整數。先對總和絕對值做整數長除，得到非負 quotient 與 remainder；當 `2*remainder>=divisor` 時，quotient 加 1，最後加回負號。因此恰好一半時向遠離零的方向進位，-0.5 得 -1，-1.5 得 -2。此為負數 tie 規則的明確補定，延伸正式規格所給的非負餘數比較法。

平均仍以原來的 base-k 輸出。基本版不輸出平均；bonus 預設在矩陣後印 `Global average:`，`--plain --bonus` 則只接著印平均值一行。

## 解法與報告

`BigNumber` 用 vector 保存低位在前的 base-k 數字，另保存負號，所有運算與輸出都用同一進位。相同符號相加，以逐位進位；不同符號相加，先比絕對值再以逐位借位相減，採較大絕對值的符號。每次運算完成都去掉高位零並清除負零。

乘法使用逐位長乘與進位。每個原子運算只涉及數字 0..15 與小 carry，完整值不會被縮成機器整數。MAC 每次取一個合法矩陣索引的乘積加入該輸出格，越界項為零。

平均除數是 N²（最多 10,000），從最高位往最低位做長除。此處 remainder 嚴格小於除數，計算 `remainder*base+digit` 可安全放在 long long；這不代表用 long long 保存大整數本身。

若每個數有 b 位，單次乘法 O(b²)，加法 O(b)，矩陣計算約 O(N²D²b²)，空間取決於矩陣大小與結果位數。大整數全程精確，沒有浮點誤差。

基本版與兩項 bonus 均完成。負數平均最容易犯的錯是在負商上直接加正 1，因此本實作先把非負商進位，再套符號。測試使用 Python 整數獨立建立所有進位的 MAC 與平均答案，並涵蓋 -0、-0.5、100 位平方及無效整列重新輸入。

## 範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`--plain`

標準輸入：
```text
16
2
A B
C D
1
F
```

標準輸出：
```text
96  A5
B4  C3
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`--plain --bonus`

標準輸入：
```text
10
2
-1 -1
0 0
1
1
```

標準輸出：
```text
-1  -1
0   0
-1
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 solution 程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `big_number.h`

```cpp
#ifndef ENERGY_BIG_NUMBER_H
#define ENERGY_BIG_NUMBER_H
#include <vector>
#include <string>
#include <algorithm>

// Digits are little-endian in the selected radix, with a separate sign.
class BigNumber {
    int base;
    bool negative;
    std::vector<int> digits;
    void normalize() {
        while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0) negative = false;
    }
    int compare_absolute(const BigNumber& b) const {
        if (digits.size() != b.digits.size()) return digits.size() > b.digits.size() ? 1 : -1;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i)
            if (digits[i] != b.digits[i]) return digits[i] > b.digits[i] ? 1 : -1;
        return 0;
    }
public:
    explicit BigNumber(int radix = 10) : base(radix), negative(false), digits(1, 0) {}
    BigNumber(const std::string& text, int radix) : base(radix), negative(text[0] == '-') {
        for (int i = static_cast<int>(text.size()) - 1; i >= (negative ? 1 : 0); --i)
            digits.push_back(text[i] <= '9' ? text[i] - '0' : text[i] - 'A' + 10);
        normalize();
    }
    std::string str() const {
        std::string text = negative ? "-" : "";
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) text += "0123456789ABCDEF"[digits[i]];
        return text;
    }
    BigNumber operator+(const BigNumber& b) const {
        BigNumber result(base); result.digits.clear();
        if (negative == b.negative) {
            int carry = 0;
            for (std::size_t i = 0; i < std::max(digits.size(), b.digits.size()) || carry; ++i) {
                int value = carry + (i < digits.size() ? digits[i] : 0) + (i < b.digits.size() ? b.digits[i] : 0);
                result.digits.push_back(value % base); carry = value / base;
            }
            result.negative = negative;
        } else {
            int comparison = compare_absolute(b);
            const BigNumber& large = comparison >= 0 ? *this : b;
            const BigNumber& small = comparison >= 0 ? b : *this;
            int borrow = 0;
            for (std::size_t i = 0; i < large.digits.size(); ++i) {
                int value = large.digits[i] - borrow - (i < small.digits.size() ? small.digits[i] : 0);
                borrow = value < 0; if (borrow) value += base;
                result.digits.push_back(value);
            }
            result.negative = large.negative;
        }
        result.normalize(); return result;
    }
    BigNumber operator*(const BigNumber& b) const {
        BigNumber result(base); result.digits.assign(digits.size() + b.digits.size() + 1, 0);
        for (std::size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (std::size_t j = 0; j < b.digits.size() || carry; ++j) {
                int value = result.digits[i + j] + carry + digits[i] * (j < b.digits.size() ? b.digits[j] : 0);
                result.digits[i + j] = value % base; carry = value / base;
            }
        }
        result.negative = negative != b.negative; result.normalize(); return result;
    }
    BigNumber rounded_average(int divisor) const {
        BigNumber result(base); result.digits.assign(digits.size(), 0);
        long long remainder = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            long long value = remainder * base + digits[i];
            result.digits[i] = static_cast<int>(value / divisor); remainder = value % divisor;
        }
        // Round the magnitude first; this handles negative quotients and -0.5.
        if (remainder * 2 >= divisor) {
            int carry = 1;
            for (std::size_t i = 0; carry; ++i) {
                if (i == result.digits.size()) result.digits.push_back(0);
                int value = result.digits[i] + carry; result.digits[i] = value % base; carry = value / base;
            }
        }
        result.negative = negative; result.normalize(); return result;
    }
};
#endif
```

### `main.cpp`

```cpp
#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include "big_number.h"

bool bonus = false, plain = false;
bool next_line(std::string& line) { return static_cast<bool>(std::getline(std::cin, line)); }
void invalid() { std::cout << "Invalid input! Please try again.\n"; }
bool read_integer(const std::string& prompt, int low, int high, bool odd, int& value) {
    std::string line;
    while (true) {
        if (!plain) std::cout << prompt;
        if (!next_line(line)) return false;
        std::istringstream input(line); std::string extra;
        if ((input >> value) && !(input >> extra) && value >= low && value <= high && (!odd || value % 2 == 1)) return true;
        invalid();
    }
}
bool valid_number(const std::string& number, int base) {
    std::size_t start = !number.empty() && number[0] == '-' ? 1 : 0;
    if (number.size() <= start || number.size() - start > 100 || (start && !bonus)) return false;
    for (std::size_t i = start; i < number.size(); ++i) {
        char c = number[i]; int digit = c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1);
        if (digit < 0 || digit >= base) return false;
    }
    return true;
}
bool read_matrix(int size, int base, const char* name, std::vector<BigNumber>& matrix) {
    if (!plain) std::cout << "Now enter the " << size << 'x' << size << ' ' << name << " matrix (base-" << base << "), rows one by one:\n";
    for (int r = 0; r < size; ++r) {
        while (true) {
            if (!plain) std::cout << "Row " << r + 1 << " (enter " << size << " values separated by spaces, then press Enter):\n";
            std::string line; if (!next_line(line)) return false;
            std::istringstream input(line); std::vector<std::string> tokens; std::string token;
            bool correct = true;
            while (input >> token) { tokens.push_back(token); if (!valid_number(token, base)) correct = false; }
            if (!correct || static_cast<int>(tokens.size()) != size) { invalid(); continue; }
            for (const std::string& text : tokens) matrix.push_back(BigNumber(text, base));
            break;
        }
    }
    return true;
}
int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--bonus") bonus = true;
        else if (option == "--plain") plain = true;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    if (!plain) std::cout << "---- Big Number Energy Distribution Calculator ----\n---------------------------------------------------\n";
    int base, n, d;
    if (!read_integer("Enter the base (2 <= k <= 16): base-k = ", 2, 16, false, base) || !read_integer("Enter N (size of energy matrix, N x N): ", 1, 100, false, n)) return 1;
    std::vector<BigNumber> energy, weights;
    if (!read_matrix(n, base, "energy", energy)) return 1;
    if (!plain) std::cout << '\n';
    if (!read_integer("Enter D (size of weight matrix, D x D): ", 1, 99, true, d) || !read_matrix(d, base, "weight", weights)) return 1;
    std::vector<BigNumber> output(n * n, BigNumber(base));
    int pad = d / 2;
    for (int r = 0; r < n; ++r) for (int c = 0; c < n; ++c)
        for (int i = 0; i < d; ++i) for (int j = 0; j < d; ++j) {
            int rr = r + i - pad, cc = c + j - pad;
            if (rr >= 0 && rr < n && cc >= 0 && cc < n) output[r * n + c] = output[r * n + c] + energy[rr * n + cc] * weights[i * d + j];
        }
    int width = 0;
    for (const BigNumber& value : output) width = std::max(width, static_cast<int>(value.str().size()) + 2);
    if (!plain) std::cout << "\n--- Computation complete: Energy distribution result (" << n << 'x' << n << ", base-" << base << ") ---\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (c == n - 1) std::cout << output[r * n + c].str();
            else std::cout << std::left << std::setw(width) << output[r * n + c].str();
        }
        std::cout << '\n';
    }
    if (bonus) {
        BigNumber sum(base); for (const BigNumber& value : output) sum = sum + value;
        if (!plain) std::cout << "\nGlobal average:\n";
        std::cout << sum.rounded_average(n * n).str() << '\n';
    }
    if (!plain) std::cout << "\nProgram finished.\n";
}
```

<!-- SOLUTION_CODE_END -->
