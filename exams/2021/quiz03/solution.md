# 2021 Quiz 3：迴圈與串流格式

依提供的考古題目與 Q2 補充圖片重整。現存 PDF 一頁包含 Q2–Q5；沒有補出 Q1。下文選項順序採 PDF，並標明補充圖片的排列差異。

## Q2：哪一種是不良程式習慣？

PDF 選項 A 控制結構前後留空行；B 縮排控制結構內的敘述；C 用浮點數作計數迴圈的計數器；D 巢狀多個迴圈。答案 **C**。補充圖片排列為 A 縮排、B 浮點計數器、C 巢狀迴圈、D 空行，所以同一個正確概念在那張圖片是 **B**，不是答案互相衝突。

浮點小數可能不能精確表示；重複累加與終止比較可能讓次數偏差。示範在常見 IEEE 754 binary64 `double` 環境，用 `value=0; value<1.0; value+=0.1` 會跑 11 次；整數 `i=0; i<10; ++i` 則確定跑 10 次。浮點本來適合數值運算，但精確控制迭代次數應用整數。

## Q3：以下 `do-while` 會印什麼？

```cpp
int counter = 1;
do {
    cout << counter << " ";
} while (counter++ < 10);
```

選項 A 1 到 10；B 語法錯誤；C 1 到 11；D 1 到 9。答案 **A**。第一次不需先檢查就印 1。後置 `++` 先拿舊值比較再遞增；當迴圈已印 10 時，條件以舊值 10 比較 `10<10` 為假，counter 最後變成 11，但不會再印出它。

## Q4：哪個 `for` 標頭無效？

```cpp
// A：照圖片保留額外的分號
for (int i = 0; int j = 5; ; i++)
// B：所有選項都無效
// C
int i = 0; for (; i < 10; i++)
// D
for (int i = 0; i < 10; i++)
```

答案 **A**。圖片 A 在 `j=5` 後有兩個分號，形成四個欄位；標準 `for` 標頭需要三個欄位、兩個分號。C 合法省略起始敘述，D 是一般形式，因此 B 錯。

原因不是「條件不能宣告變數」：C++11 的條件可以有宣告。刪掉 A 額外分號後，`for(int i=0; int j=5; i++)` 語法可以成立，但條件每次以 5 判斷為真，不適合當有限計數迴圈。driver 不執行這段可能無窮且會整數溢位的迴圈，只示範 C 與 D 的 0 到 9。

## Q5：哪個格式設定不是持續有效？

選項 A 都不是；B `setprecision`；C `fixed`；D `setw`。答案 **D**。`setw` 設定下一次相關格式化輸出的欄位寬度，數值輸出後寬度會重設。`fixed` 與 precision 則保持到再次改變設定為止。

示範先輸出 `setw(5)` 的 7，再未設定寬度的 8；結果為 `[    7][8]`。再用 `fixed` 加 `setprecision(2)` 連續輸出 1.25 與 2.5，可見第二個仍有兩位小數 `2.50`。

## 示範介面與補定規則

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz03`；執行 `./quiz03 2|3|4|5`，四種模式都不讀 stdin。driver 在原 Q3 的尾端加換行，原迴圈保留每個數字後的一格。Q4 額外以 C/D 標籤區分兩段。Q2 的浮點次數示範以 IEEE 754 binary64 的一般編譯為前提，不聲稱所有浮點格式或最佳化選項都相同。

正常 exit code 0，無題號或未知題號 2。四題依序核對計數、後置遞增邊界、合法 for 形式與格式狀態。錯誤片段只在文件中保留，沒有偽造成功執行結果。

## 範例

每題各一例；輸出不包含輸入回顯。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
floating iterations: 11
integer iterations: 10
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3`

標準輸入：
```text

```

標準輸出：
```text
1 2 3 4 5 6 7 8 9 10 
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text

```

標準輸出：
```text
C: 0 1 2 3 4 5 6 7 8 9 
D: 0 1 2 3 4 5 6 7 8 9 
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text

```

標準輸出：
```text
[    7][8]
1.25 2.50
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz03 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        int floatingCount = 0;
        for (double value = 0; value < 1.0; value += 0.1) ++floatingCount;
        int integerCount = 0;
        for (int i = 0; i < 10; ++i) ++integerCount;
        std::cout << "floating iterations: " << floatingCount
                  << "\ninteger iterations: " << integerCount << '\n';
    } else if (mode == "3") {
        int counter = 1;
        do {
            std::cout << counter << ' ';
        } while (counter++ < 10);
        std::cout << '\n';
    } else if (mode == "4") {
        std::cout << "C: ";
        int i = 0;
        for (; i < 10; ++i) std::cout << i << ' ';
        std::cout << "\nD: ";
        for (int j = 0; j < 10; ++j) std::cout << j << ' ';
        std::cout << '\n';
    } else if (mode == "5") {
        std::cout << '[' << std::setw(5) << 7 << "][" << 8 << "]\n";
        std::cout << std::fixed << std::setprecision(2) << 1.25 << ' '
                  << 2.5 << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
