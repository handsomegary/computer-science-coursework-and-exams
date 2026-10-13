# PA3：大整數計算機題解與報告

依提供的作業題目與參考程式整理。題目明確規則與公開測資作為依據，未提供的錯誤恢復及 bonus 格式在下文另列。本文件不附原始講義或個人資料。

## 題面與輸入輸出

一般版反覆顯示選單，`1` 加法、`2` 減法、`3` 乘法、`0` 結束。選擇運算後讀取兩個正整數 `a`、`b`，每個輸入最多 100 位，以 array、vector 或 string 儲存所有數字。

依選項精確計算 `a+b`、`a-b` 或 `a×b`，先輸出 `Result: ...`，再輸出 `Mean: floor((a+b)/2)`。兩輸入相等的減法結果為零，第一數較小時結果為負；這不等於接受負的輸入值。加法最多 101 位，乘法最多 200 位。

bonus 增加 `4` 除法。商為 `floor(a/b)`，餘數為 `a%b`，分別顯示 `Result:` 和 `Remainder:`，再顯示兩數的平均值。商或餘數可以是零；輸入仍必須為正數。

題目給定的選單及錯誤訊息如下，bonus 在乘法與結束之間增加 `4. Division`：

```text
----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
```

- 加法標題：`----Now for Addition----`。
- 減法標題：`----Now for Subtraction ----`，依公開測資保留最後四個減號前的一格空白。
- 乘法標題：`----Now for Multiplication----`。
- bonus 除法標題：`----Now for Division----`。
- 輸入提示：`Please enter the two numbers.`。
- 錯誤：`Error! Please try again.`；選項 `0` 的告別訊息：`Good bye!`。

結果或錯誤訊息之後保留一個空白行，再顯示完整選單。題目範例中的鍵盤輸入不由程式回顯，標準輸出只包含程式印出的提示與結果。

## 防呆與規格邊界

運算數接受最多 100 個數字字元且數值大於零的字串。前導零可接受，但仍計入原始 100 位限制；輸出正規化為無多餘前導零的表示法。零、負數、帶加號數字、小數、非數字及過長輸入都拒絕。

選單以字串讀入後逐位驗證，因此字母、混合字元及巨大整數不會讓輸入串流卡住或使機器整數溢位。`+1` 和 `001` 等合法整數寫法可選加法，越界選項回到選單。

題目要求選單防呆；bonus 要求處理負數、超出範圍及錯誤輸入。本版一般入口也共用同一防呆：讀完一組兩個值後驗證，錯誤輸出同一訊息並回選單，不輸出部分結果。bonus 的商、餘數標籤沒有官方範例，採用上述格式。所有輸入以空白分隔，遇到 EOF 直接結束，不把缺少的值當成零。

題目的「四則運算、沒有括號」沒有改變明確的選單與兩運算數輸入規則，因此不需要額外的算式剖析器。負的減法結果以 `-` 開頭輸出；bonus 的負數防呆指負的輸入值應被拒絕。

## 解法

### 1. 統一大整數表示法

每個數字儲存為十進位 `std::string`。移除前導零後，零只表示為 `"0"`，其餘數字以非零字元開頭。比較時先比較長度，長度相同再依字典序比較，便得到整數大小關係。

不把完整數值轉成 `int`、`long long` 或浮點數。小型整數只保存單位數字、進位、借位、商的單一位數，以及陣列的有限累加值。

### 2. 加法與平均值

加法從個位向左逐位計算，將兩位數字與上一位進位相加，保留 `%10` 的數字並把 `/10` 作為下一位進位。輸出位數先反向建立，再反轉。最後進位也加入，所以兩個 100 位輸入的和不會被截斷。

平均值先求精確的和，再從最高位做除二。每一步將上一位餘數乘十加上目前位數，輸出該值除二的商。處理完個位後不輸出剩餘的餘數，恰好得到題目要求的無條件捨去小數。

### 3. 減法

先比較兩數大小，再以較大的數減較小的數。逐位減法借位時加十，最後移除前導零。若原輸入 `a<b`，在結果前加負號；相等時輸出單一 `0`。借位運算永遠在足夠大的數上進行，不會把負數當成無號數字陣列處理。

### 4. 乘法

使用 `n+m` 個 `int` 保存反向數字槽。每對輸入數字的乘積累加到對應位數，然後從個位往上處理進位。兩個 100 位數最多需要 200 位結果。每個槽最多累加 100 個 `9×9`，配合進位仍遠小於 `int` 範圍。

### 5. bonus 長除法

從被除數最高位往最低位讀取，維持目前餘數。把下一位接到餘數後，反覆減去除數，減去的次數就是這一位商。

每次讀入下一位前維持 `0 ≤ r < b`；讀入數字 `d` 後 `10r+d < 10b`，所以單一商位最多減九次。這是逐位長除法，執行次數不依整個商的大小增加，即使商有 100 位也能處理。最後保留的餘數滿足 `a = bq+r` 與 `0 ≤ r < b`。

## 正確性與成本

各運算都以十進位逐位表示進位或借位，且保留足夠位數。正規化只移除前導零，不改變數值；平均的最後餘數被捨去；除法以每位餘數不變量保證商與餘數正確。

令兩輸入位數為 `n`、`m`：

| 運算 | 時間 | 額外空間 |
|---|---|---|
| 比較、加減、除二與平均值 | `O(n+m)` 上界 | `O(n+m)` |
| 乘法 | `O(nm)` | `O(n+m)` |
| 長除法 | `O(nm)` | `O(n+m)` |

輸入上限為 100 位。程式不使用內建整數保存整個商、不反覆減去除數直到整個商算完，也不使用浮點近似。

## 驗證與改進

一般版與 bonus 版皆以 C++11、`-Wall -Wextra -Wpedantic` 編譯，無警告。公開三組測資逐字核對提示、結果、平均值、非法選項與結束訊息。另以獨立 Python 整數模型核對隨機加減乘、長除法、連續進位與借位、負或零減法結果、100 位輸入、101 位和、200 位乘積、零商、零餘數、原始 101 位輸入拒絕、前導零、錯誤輸入恢復及 EOF。

將原參考程式重複的加法與平均值整理為共用函式；以字串長度及受控迴圈取代固定陣列索引猜測，避免讀寫越界。輸入失敗透過字串驗證與 EOF 分支處理。bonus 採用逐位長除法，避免機器整數商溢位與依商大小增長的執行時間。

以下範例由兩個編譯後的程式實際執行擷取，`samples.json` 保存同一組輸入、輸出與結束碼；公開三組範例依原題測資，bonus 範例為補充驗證案例。

## 實跑範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

標準輸入：
```text
1
11112222333344445555
77776666555544443333
5
3
1111111111
5
0
```

標準輸出：
```text
----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Addition----
Please enter the two numbers.
Result: 88888888888888888888
Mean: 44444444444444444444

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Error! Please try again.

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Multiplication----
Please enter the two numbers.
Result: 5555555555
Mean: 555555558

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Good bye!
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

標準輸入：
```text
2
111111111
33333333
5
3
29292929
300000003
0
```

標準輸出：
```text
----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Subtraction ----
Please enter the two numbers.
Result: 77777778
Mean: 72222222

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Error! Please try again.

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Multiplication----
Please enter the two numbers.
Result: 8787878787878787
Mean: 164646466

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Good bye!
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

標準輸入：
```text
2
9999999999
1212121212
5
3
29292929
3
0
```

標準輸出：
```text
----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Subtraction ----
Please enter the two numbers.
Result: 8787878787
Mean: 5606060605

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Error! Please try again.

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
----Now for Multiplication----
Please enter the two numbers.
Result: 87878787
Mean: 14646466

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
0. Exit
--------------------------
Good bye!
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`bonus.cpp`）

標準輸入：
```text
4
12345678901234567890
97
4
17
50
4
12300
100
4
12
0
0
```

標準輸出：
```text
----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
4. Division
0. Exit
--------------------------
----Now for Division----
Please enter the two numbers.
Result: 127275040218913071
Remainder: 3
Mean: 6172839450617283993

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
4. Division
0. Exit
--------------------------
----Now for Division----
Please enter the two numbers.
Result: 0
Remainder: 17
Mean: 33

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
4. Division
0. Exit
--------------------------
----Now for Division----
Please enter the two numbers.
Result: 123
Remainder: 0
Mean: 6200

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
4. Division
0. Exit
--------------------------
----Now for Division----
Please enter the two numbers.
Error! Please try again.

----Welcome to big number calculator----
Please choose the function
1. Addition
2. Subtraction
3. Multiplication
4. Division
0. Exit
--------------------------
Good bye!
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `bignum.hpp`

```cpp
#ifndef PA03_BIGNUM_HPP
#define PA03_BIGNUM_HPP

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace pa03 {

// Canonical numbers have no leading zeroes, except for the number "0".
inline std::string normalize(const std::string& value) {
    const std::size_t first = value.find_first_not_of('0');
    return first == std::string::npos ? "0" : value.substr(first);
}

inline bool parse_positive(const std::string& token, std::string& value) {
    if (token.empty() || token.size() > 100) {
        return false;
    }
    for (std::size_t i = 0; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') {
            return false;
        }
    }
    value = normalize(token);
    return value != "0";
}

inline int compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return a.size() < b.size() ? -1 : 1;
    }
    return a == b ? 0 : (a < b ? -1 : 1);
}

inline std::string add(const std::string& a, const std::string& b) {
    std::string result;
    std::size_t i = a.size();
    std::size_t j = b.size();
    int carry = 0;
    while (i != 0 || j != 0 || carry != 0) {
        int digit = carry;
        if (i != 0) {
            digit += a[--i] - '0';
        }
        if (j != 0) {
            digit += b[--j] - '0';
        }
        result.push_back(static_cast<char>('0' + digit % 10));
        carry = digit / 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// Requires a >= b; both inputs are canonical nonnegative integers.
inline std::string subtract_magnitude(const std::string& a,
                                      const std::string& b) {
    std::string result;
    std::size_t i = a.size();
    std::size_t j = b.size();
    int borrow = 0;
    while (i != 0) {
        int digit = a[--i] - '0' - borrow;
        if (j != 0) {
            digit -= b[--j] - '0';
        }
        borrow = digit < 0 ? 1 : 0;
        if (borrow != 0) {
            digit += 10;
        }
        result.push_back(static_cast<char>('0' + digit));
    }
    std::reverse(result.begin(), result.end());
    return normalize(result);
}

inline std::string subtract(const std::string& a, const std::string& b) {
    return compare(a, b) < 0 ? "-" + subtract_magnitude(b, a)
                             : subtract_magnitude(a, b);
}

inline std::string multiply(const std::string& a, const std::string& b) {
    if (a == "0" || b == "0") {
        return "0";
    }
    std::vector<int> digits(a.size() + b.size(), 0);
    for (std::size_t i = 0; i < a.size(); ++i) {
        for (std::size_t j = 0; j < b.size(); ++j) {
            digits[i + j] += (a[a.size() - 1 - i] - '0') *
                             (b[b.size() - 1 - j] - '0');
        }
    }
    for (std::size_t i = 0; i + 1 < digits.size(); ++i) {
        digits[i + 1] += digits[i] / 10;
        digits[i] %= 10;
    }
    std::string result;
    for (std::size_t i = digits.size(); i != 0; --i) {
        result.push_back(static_cast<char>('0' + digits[i - 1]));
    }
    return normalize(result);
}

inline std::string halve(const std::string& value) {
    std::string result;
    int remainder = 0;
    for (std::size_t i = 0; i < value.size(); ++i) {
        const int digit = remainder * 10 + value[i] - '0';
        result.push_back(static_cast<char>('0' + digit / 2));
        remainder = digit % 2;
    }
    return normalize(result);
}

// Decimal long division: each quotient digit needs at most nine subtractions.
// Requires b > 0, as guaranteed by the input validation.
inline std::pair<std::string, std::string> divide(const std::string& a,
                                                const std::string& b) {
    std::string quotient;
    std::string remainder = "0";
    for (std::size_t i = 0; i < a.size(); ++i) {
        remainder = normalize(remainder + a[i]);
        int digit = 0;
        while (compare(remainder, b) >= 0) {
            remainder = subtract_magnitude(remainder, b);
            ++digit;
        }
        quotient.push_back(static_cast<char>('0' + digit));
    }
    return std::make_pair(normalize(quotient), remainder);
}

inline bool parse_choice(const std::string& token, int maximum, int& choice) {
    std::size_t i = 0;
    bool negative = false;
    if (!token.empty() && (token[0] == '+' || token[0] == '-')) {
        negative = token[0] == '-';
        ++i;
    }
    if (i == token.size()) {
        return false;
    }
    choice = 0;
    for (; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') {
            return false;
        }
        choice = choice * 10 + token[i] - '0';
        if (choice > maximum) {
            return false;
        }
    }
    return !negative || choice == 0;
}

inline void print_menu(bool bonus) {
    std::cout << "----Welcome to big number calculator----\n"
              << "Please choose the function\n"
              << "1. Addition\n"
              << "2. Subtraction\n"
              << "3. Multiplication\n";
    if (bonus) {
        std::cout << "4. Division\n";
    }
    std::cout << "0. Exit\n--------------------------\n";
}

inline int run_calculator(bool bonus) {
    for (;;) {
        print_menu(bonus);
        std::string token;
        if (!(std::cin >> token)) {
            return 0;
        }
        int choice = 0;
        if (!parse_choice(token, bonus ? 4 : 3, choice)) {
            std::cout << "Error! Please try again.\n\n";
            continue;
        }
        if (choice == 0) {
            std::cout << "Good bye!\n";
            return 0;
        }
        if (choice == 1) {
            std::cout << "----Now for Addition----\n";
        } else if (choice == 2) {
            std::cout << "----Now for Subtraction ----\n";
        } else if (choice == 3) {
            std::cout << "----Now for Multiplication----\n";
        } else {
            std::cout << "----Now for Division----\n";
        }
        std::cout << "Please enter the two numbers.\n";
        std::string raw_a;
        std::string raw_b;
        if (!(std::cin >> raw_a >> raw_b)) {
            return 0;
        }
        std::string a;
        std::string b;
        if (!parse_positive(raw_a, a) || !parse_positive(raw_b, b)) {
            std::cout << "Error! Please try again.\n\n";
            continue;
        }
        if (choice == 1) {
            std::cout << "Result: " << add(a, b) << '\n';
        } else if (choice == 2) {
            std::cout << "Result: " << subtract(a, b) << '\n';
        } else if (choice == 3) {
            std::cout << "Result: " << multiply(a, b) << '\n';
        } else {
            const std::pair<std::string, std::string> result = divide(a, b);
            std::cout << "Result: " << result.first << '\n'
                      << "Remainder: " << result.second << '\n';
        }
        std::cout << "Mean: " << halve(add(a, b)) << "\n\n";
    }
}

}  // namespace pa03

#endif
```

### `bonus.cpp`

```cpp
#include "bignum.hpp"

int main() {
    return pa03::run_calculator(true);
}
```

### `main.cpp`

```cpp
#include "bignum.hpp"

int main() {
    return pa03::run_calculator(false);
}
```

<!-- SOLUTION_CODE_END -->
