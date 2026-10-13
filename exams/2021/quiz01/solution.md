# 2021 Quiz 1：輸入、識別字與條件判斷

依提供的考古題目重新整理。現存兩頁只包含 Q2–Q5，以下完整涵蓋這四題；沒有重建未提供的 Q1。保留 PDF 的選項順序，題面以繁中改寫，不上傳原卷或其他作者原檔。

## Q2：哪個不是有效的 C++ 識別字？

| 選項 | 內容 |
|---|---|
| A | `my*Value` |
| B | `_AAA1` |
| C | `widthee` |
| D | `m_x` |

答案 **A**。`*` 是運算符，不能在一般識別字中。其餘三個符合英文字母、數字、底線構成且不以數字開頭的詞法規則。另須區分「詞法有效」與「可由使用者任意取用」：B 以底線加大寫字母開頭，屬實作保留名稱，實際程式不要使用它作自己的宣告。本示範僅檢查固定的字串，沒有宣告這個保留名稱。

## Q3：哪行可能改變 `number2`？

| 選項 | 程式 |
|---|---|
| A | `number1 = number2;` |
| B | `sum = number1 + number2;` |
| C | `std::cin >> number2;` |
| D | `std::cout << number2;` |

答案 **C**。輸入運算會把讀取值寫入 `number2`；A 改的是左邊 `number1`，B 改的是 `sum`，D 只是輸出。若輸入剛好等於舊值，C 不一定讓數值不同，但它是四個選項中具有寫入能力的那一個。

## Q4：程式的輸出是哪一個？

初始值為 `a=4, b=12, c=37, d=51`，依序執行：

```cpp
if (a < b) cout << "a < b" << endl;
if (a > b) cout << "a > b" << endl;
if (d <= c) cout << "d <= c" << endl;
if (c != d) cout << "c != d" << endl;
```

選項 A：三行 `a < b`、`d <= c`、`c != d`；B：兩行 `a < b`、`c != d`；C：兩行 `a > b`、`c != d`；D：兩行 `a < b`、`c < d`。

答案 **B**。四次比較分別為真、假、假、真，所以只輸出第一與第四行。

## Q5：輸入 100、2 後原程式輸出什麼？

原題的關鍵程式是：

```cpp
if (number1 > number2)
    cout << number1 << " is larger than " << number2 << endl;
cout << number1 << " is bigger than " << number2 << endl;
if (number1 = number2)
    cout << number1 << " is equal to " << number2 << endl;
```

選項 A：`100 is larger than 2`、`2 is bigger than 2`；B：`100 is larger than 2`、`100 is bigger than 2`；C：`100 is larger than 2`、`2 is equal to 2`；D：`100 is larger than 2`、`100 is bigger than 2`、`2 is equal to 2`。

追蹤原行為的答案 **D**。第二行不受第一個 `if` 控制。最後一個條件是指定，不是相等比較：它先把 `number1` 改成 2，指定運算的值也是 2，轉成條件為真，因此輸出 `2 is equal to 2`。若 `number2` 是 0，指定仍然發生但條件為假。

修正版只把該條件改成 `number1 == number2`，保留其他流程，用同一筆原輸入重跑；100 與 2 不相等，便不輸出相等行。程式在模擬指定時額外用括號表達這是有意追蹤原題，避免把警告誤認為修正版的錯誤。

## 示範介面與補定規則

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz01`。執行 `./quiz01 2|3|4|5` 指定題號；Q2、Q4 不需 stdin，Q3 讀一個 32 位有號整數，Q5 讀兩個。Q3 示範初始 `number1=1, number2=2`，A/B 接續執行，C 讀入新值，D 為公平比較再重設 `number2=2`。Q5 依序印 `Original:` 與 `Corrected:`。這些標題與擴充介面是本儲存庫補定，不是原題要求。

讀不到完整數值或越界，exit code 為 1；缺少或未知題號為 2；正常為 0。沒有題目提示或輸入回顯。可用 `python tests/test_quizzes.py` 驗證固定選項、原追蹤、零值指定與改正後的比較。

## 範例

四組例子分別涵蓋四題；Q5 同時顯示原題答案與比較修正的差異。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
A: invalid
B: valid
C: valid
D: valid
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3`

標準輸入：
```text
9
```

標準輸出：
```text
A: number2=2
B: sum=4; number2=2
C: number2=9
D: 2; number2=2
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text

```

標準輸出：
```text
a < b
c != d
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text
100 2
```

標準輸出：
```text
Original:
100 is larger than 2
100 is bigger than 2
2 is equal to 2
Corrected:
100 is larger than 2
100 is bigger than 2
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <cctype>
#include <iostream>
#include <string>

bool identifier(const std::string& value) {
    if (value.empty()) return false;
    for (std::size_t i = 0; i < value.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(value[i]);
        if (!(std::isalpha(c) || c == '_' || (i != 0 && std::isdigit(c))))
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz01 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        const std::string choices[] = {"my*Value", "_AAA1", "widthee", "m_x"};
        for (int i = 0; i < 4; ++i)
            std::cout << static_cast<char>('A' + i) << ": "
                      << (identifier(choices[i]) ? "valid" : "invalid") << '\n';
    } else if (mode == "3") {
        int number1 = 1, number2 = 2;
        number1 = number2;
        std::cout << "A: number2=" << number2 << '\n';
        int sum = number1 + number2;
        std::cout << "B: sum=" << sum << "; number2=" << number2 << '\n';
        if (!(std::cin >> number2)) return 1;
        std::cout << "C: number2=" << number2 << '\n';
        number2 = 2;
        std::cout << "D: " << number2 << "; number2=" << number2 << '\n';
    } else if (mode == "4") {
        int a = 4, b = 12, c = 37, d = 51;
        if (a < b) std::cout << "a < b\n";
        if (a > b) std::cout << "a > b\n";
        if (d <= c) std::cout << "d <= c\n";
        if (c != d) std::cout << "c != d\n";
    } else if (mode == "5") {
        int first, second;
        if (!(std::cin >> first >> second)) return 1;
        int number1 = first, number2 = second;
        std::cout << "Original:\n";
        if (number1 > number2)
            std::cout << number1 << " is larger than " << number2 << '\n';
        std::cout << number1 << " is bigger than " << number2 << '\n';
        if ((number1 = number2))
            std::cout << number1 << " is equal to " << number2 << '\n';
        number1 = first;
        std::cout << "Corrected:\n";
        if (number1 > number2)
            std::cout << number1 << " is larger than " << number2 << '\n';
        std::cout << number1 << " is bigger than " << number2 << '\n';
        if (number1 == number2)
            std::cout << number1 << " is equal to " << number2 << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
