# 2021 Quiz 4：列舉、遞迴、模板與函式引數

依提供的考古題目重新整理。現存兩頁只有 Q2–Q5，完整重寫這四題與解析，保留選項字母，不推測 Q1。

## Q2：哪個列舉宣告不合法？

```cpp
// A
enum person { me = 1, you = 2, them = 3 };
// B
enum person { me, you, them };
// C
enum person { me = 0, you = 0, them = 0 };
// D
enum person { me, you, me };
```

答案 **D**。同一列舉內不能重複宣告 enumerator 名稱 `me`；不同名稱可有同一個值，所以 C 合法。A 的值是 1、2、3；B 沒指定時從 0 依序遞增，值是 0、1、2；C 的三個值都是 0。driver 將三個合法宣告分別放在獨立區塊，避免三段原選項的名稱彼此衝突。

## Q3：`mystery(4)` 的回傳值？

```cpp
int mystery(int number) {
    if (number <= 1) return 1;
    return number * mystery(number - 1);
}
```

選項 A 6；B 12；C 24；D 1。答案 **C**。它遞迴計算 `4*3*2*1=24`。終止分支在 1，不是把 4 直接乘以 3 就結束。driver 擴充允許 `0..12`，維持此遞迴：0 回傳 1，12! 為 479001600，可存 32 位 int；不接受會造成溢位的 13。

## Q4：函式模板的兩次回傳值？

```cpp
template<class T>
T maximum(T value1, T value2) {
    if (value1 > value2) return value1;
    return value2;
}
maximum(2, 5);
maximum(2.3, 5.2);
```

選項 A 2 與 2.3；B 5 與型別不一致錯誤；C 5 與 5.2；D 兩個錯誤訊息。答案 **C**。第一個呼叫推導 T 為 int；第二個的兩個引數同為 double，推導 T 為 double，因此不會發生型別衝突。模板不是只能在整份程式選一種型別。若同一個呼叫混用 `maximum(2,5.2)`，才需另外處理推導衝突。

## Q5：引數與參數列表哪個項目不必相同？

選項 A 數量；B 名稱；C 型別；D 以上三種都必須相同。答案 **B**。呼叫端變數名稱不必和被呼叫函式的參數名稱一樣。原題採沒有預設引數、可變參數或特殊轉換的一般匹配情境；C++ 一般呼叫也可有允許的型別轉換，所以「型別必須文字完全一樣」不是普遍規則。

driver 用呼叫端 `x,y` 呼叫參數名為 `a,b` 的 `sumPair`，計算和值，顯示名稱不同仍可正常傳值。

## 示範介面與補定規則

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz04`；執行 `./quiz04 2|3|4|5`。Q2、Q4 無 stdin；Q3 讀一個 `0..12` 的 int；Q5 讀兩個 `-1000000000..1000000000` 的 int，和值在 int 範圍內。Q3 複雜度 `O(n)` 時間、`O(n)` 遞迴空間；其他模式為固定工作量。

合法示範各有清楚標籤；Q2 的無效 D 只輸出解釋，不編譯重名宣告。正常 exit code 0，錯誤數值或不完整輸入 1，無或未知題號 2；無輸入回顯。獨立測試以 Python factorial 核對所有允許 n，並核對引數名稱不影響數值。

## 範例

每題各一例；Q3 的 4! 與 Q4 兩種模板實例均實際計算。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
A: 1 2 3
B: 0 1 2
C: 0 0 0
D: ill-formed (duplicate enumerator name)
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3`

標準輸入：
```text
4
```

標準輸出：
```text
24
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text

```

標準輸出：
```text
int maximum: 5
double maximum: 5.2
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text
3 4
```

標準輸出：
```text
7
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>
#include <string>

int mystery(int number) {
    return number <= 1 ? 1 : number * mystery(number - 1);
}

template<class T>
T maximum(T value1, T value2) {
    return value1 > value2 ? value1 : value2;
}

int sumPair(int a, int b) {
    return a + b;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz04 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        {
            enum person {me = 1, you = 2, them = 3};
            std::cout << "A: " << me << ' ' << you << ' ' << them << '\n';
        }
        {
            enum person {me, you, them};
            std::cout << "B: " << me << ' ' << you << ' ' << them << '\n';
        }
        {
            enum person {me = 0, you = 0, them = 0};
            std::cout << "C: " << me << ' ' << you << ' ' << them << '\n';
        }
        std::cout << "D: ill-formed (duplicate enumerator name)\n";
    } else if (mode == "3") {
        int number;
        if (!(std::cin >> number) || number < 0 || number > 12) return 1;
        std::cout << mystery(number) << '\n';
    } else if (mode == "4") {
        std::cout << "int maximum: " << maximum(2, 5)
                  << "\ndouble maximum: " << maximum(2.3, 5.2) << '\n';
    } else if (mode == "5") {
        int x, y;
        if (!(std::cin >> x >> y) || x < -1000000000 || x > 1000000000 ||
            y < -1000000000 || y > 1000000000) return 1;
        std::cout << sumPair(x, y) << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
