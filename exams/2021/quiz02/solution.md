# 2021 Quiz 2：偽碼、運算符優先序與最大公因數

依提供的考古題目重新整理。現存兩頁只有 Q2–Q5，沒有臆造 Q1；選項順序沿用 PDF，錯誤語法僅列於題解，沒有加入可編譯的分支。

## Q2：課本慣例的偽碼不包括什麼？

選項 A 宣告（Declarations）；B 輸入／輸出；C 演算法；D 控制結構。原題答案 **A**。此課本以自然語言和控制結構描述演算法，而省略 C++ 型別／變數宣告；輸入輸出與分支、迴圈都可以描述。

這是課本採用的偽碼慣例，不是所有偽碼格式的普遍禁令。某些偽碼記法也允許宣告。driver 以最大公因數的偽碼展示演算法內容，不假稱它是 C++ 的原始程式。

## Q3：哪個選項不能在 `answer==7` 時輸出 correct，否則輸出 incorrect？

```cpp
// A
if (answer == 7) cout << "correct";
else cout << "incorrect";
// B
cout << answer == 7 ? "correct" : "incorrect";
// C
cout << (answer == 7 ? "correct" : "incorrect");
// D
answer == 7 ? cout << "correct" : cout << "incorrect";
```

答案 **B**。`<<` 比 `==`、`?:` 更早結合，因此 B 的左段變成 `(cout << answer) == 7`；在 C++11 中，輸出串流不能如此與整數比較，這是無法編譯的表達式。C 用括號讓條件運算先產生文字，再輸出。D 的兩個分支分別執行一次輸出，也是合法的。

示範只真正執行 A、C、D；B 輸出一行明確的編譯錯誤說明，不嘗試執行錯誤片段。

## Q4：`x` 初值為 3，哪個選項使它變成 7？

| 選項 | 內容 | 結果 |
|---|---|---|
| A | `x ++ 4;` | 無效語法；`++` 不接右運算元 |
| B | `x += 4;` | 7 |
| C | `x =+ 4;` | 指定正的 4，不是複合指定 |
| D | `x + 4 = x;` | 左邊不是可指定物件 |

答案 **B**。C 的原題寫法是 `=+`，不是 `*=` 或 `+=`；它相當於 `x = (+4)`。

## Q5：輸入 `a=165, b=987`，下列演算法輸出什麼？

```cpp
while (b != 0) {
    t = b;
    b = a % b;
    a = t;
}
cout << a << endl;
```

選項 A 21；B 7；C 165；D 3。答案 **D**。`(a,b)` 依序為 `(165,987)`、`(987,165)`、`(165,162)`、`(162,3)`、`(3,0)`；退出時 a 為最大公因數 3。迴圈先檢查 b 非零，所以不會取模零。

## 示範介面與解法

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz02`。執行 `./quiz02 2|3|4|5`。Q2、Q4 無 stdin；Q3 讀一個 32 位有號整數；Q5 讀兩個 `0..2147483647` 的整數，以 Euclid 迴圈計算，`gcd(0,0)` 補定為 0，其餘零值依 `gcd(n,0)=n`。Q5 時間 `O(log(max(a,b)))`、空間 `O(1)`。

輸出中的 A/B/C/D 標籤與 Q2 的偽碼是本儲存庫提供的教學介面。正常 exit code 0，錯誤輸入 1，無題號或未知題號 2；輸出沒有輸入回顯。獨立測試以 Python `math.gcd` 核對不同順序、零值、相等、互質和範圍邊界。

## 範例

四組例子依序涵蓋四題，最後一組實際計算原題的最大公因數。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
INPUT a, b
WHILE b is not zero
    replace (a, b) with (b, a modulo b)
OUTPUT a
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3`

標準輸入：
```text
7
```

標準輸出：
```text
A: correct
B: ill-formed in C++11
C: correct
D: correct
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text

```

標準輸出：
```text
B: 7
C: 4
A and D: ill-formed
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text
165 987
```

標準輸出：
```text
3
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz02 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        std::cout << "INPUT a, b\nWHILE b is not zero\n"
                     "    replace (a, b) with (b, a modulo b)\nOUTPUT a\n";
    } else if (mode == "3") {
        int answer;
        if (!(std::cin >> answer)) return 1;
        std::cout << "A: ";
        if (answer == 7) std::cout << "correct\n";
        else std::cout << "incorrect\n";
        std::cout << "B: ill-formed in C++11\nC: "
                  << (answer == 7 ? "correct" : "incorrect") << "\nD: ";
        answer == 7 ? std::cout << "correct" : std::cout << "incorrect";
        std::cout << '\n';
    } else if (mode == "4") {
        int x = 3;
        x += 4;
        std::cout << "B: " << x << '\n';
        x = 3;
        x = +4;
        std::cout << "C: " << x << "\nA and D: ill-formed\n";
    } else if (mode == "5") {
        long long a, b;
        if (!(std::cin >> a >> b) || a < 0 || b < 0 ||
            a > 2147483647LL || b > 2147483647LL) return 1;
        while (b != 0) {
            const long long t = b;
            b = a % b;
            a = t;
        }
        std::cout << a << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
