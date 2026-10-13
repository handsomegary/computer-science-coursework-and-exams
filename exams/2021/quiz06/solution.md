# 2021 Quiz 6：檔案位置、數值轉換與結構初始化

依提供的考古題目重整。現存一頁包含 Q2–Q5，沒有 Q1。尤其 Q5 的舊式答案須依本儲存庫使用的 C++11 修正；文件保留原命題意圖，不把過時限制寫成現行語法。

## Q2：C++ 檔案處理應包含哪組標頭？

選項 A `<cstdio>`, `<iostream>`, `<fstream>`；B `<cstdio>`, `<fstream>`；C `<iostream>`, `<fstream>`；D `<cstdio>`, `<iostream>`。原題預期 **C**：`fstream` 提供 C++ 檔案串流，`iostream` 提供常見的標準輸入／輸出串流；`cstdio` 是 C 風格 stdio 介面，這種寫法不需要它。

精確地說，A 只是多 include 一個標頭，仍可以編譯；單用 C++ 檔案串流而不使用 cin/cout 時，只需 `fstream`。因此 C 是本題課程情境的精簡組合，不能說 A 本身是無效程式。

## Q3：十位元組檔案，當前位置為 byte #1，哪個 seek 操作與其他不同？

```cpp
// A
fileObject.seekg(1, ios::cur);
// B
fileObject.seekg(8, ios::end);
// C
fileObject.seekg(2, ios::beg);
// D
fileObject.seekg(2);
```

答案 **B**。採原題的位元組位置為從 0 起算的 offset，A 為 `1+1=2`，C 和 D 為絕對位置 2。B 是檔尾後方 8 位元組，即 `10+8=18`，不是向前倒退八個位元組；想定位 2 應用 `seekg(-8,ios::end)`。

定位到 EOF 後面不是立即讀取資料；本示範只查 `tellg`，不從 offset 18 讀取。每個選項都先把位置重設為 1，以免讓前一操作影響後一個。輸入檔只讀取、不修改。

## Q4：`atof`、`atoi`、`atol` 主要差別是什麼？

選項 A 回傳型別；B 引數；C 效率；D 標頭。答案 **A**。三者都接收 C 字串，分別回傳 double、int、long；共同可用 `<cstdlib>`。例如 `-12.5` 會得到 double 的 -12.5，另兩個整數轉換讀到小數點前得到 -12。

這些函式不是完整的嚴格輸入驗證工具，尤其整數越界不能安全套用。本 driver 在呼叫它們之前先限制文字為一般小數格式與有限的 `[-1000000,1000000]` 數值，避開整數越界、NaN、無限大與未定義行為。

## Q5：結構變數不能以哪種方法初始化？

選項 A 對個別資料成員指定值；B 類似陣列的成員 initializer list；C 在 struct 定義內指定使用者的成員值；D 由另一個同型別 struct 指定。

原題舊式命題預期 **C**，但在 **C++11 已不能無條件判 C 非法**：非 static 資料成員可以有預設成員初始化，例如：

```cpp
struct Defaulted {
    int first = 1;
    int second = 2;
};
Defaulted value;
```

一般 Plain struct 可用 `{1,2}` 聚合初始化；同型別物件可複製初始化；宣告後逐成員賦值也合法，只是嚴格術語稱它為 assignment，不是初始化。因此原題若指定 C++11 並廣義詢問四種「取得值」的方法，就沒有原先預期的唯一非法選項。

另外 C++11 具有預設成員初始化的這個 Defaulted 不符合該版本的 aggregate 條件；不能把 Plain 的 `{1,2}` 聚合初始化直接套到有預設成員初始化的型別再說它也一定合法。本 driver 用 Plain 示範 A/B/D，用 Defaulted 示範 C，實際驗證兩個不同情境。

## 示範介面、限制與補定規則

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz06`。Q2、Q5 執行 `./quiz06 2`／`./quiz06 5`，不需 stdin。Q3 執行 `./quiz06 3 sample.dat`，檔案必須恰為十位元組；例如純 ASCII `abcdefghij`，不加換行，不需 stdin，不產生或覆寫檔案。Q4 執行 `./quiz06 4`，stdin 一個最多 32 字元的有限十進位小數 token，可含前導正負號和一個小數點，至少一個數字；不接受指數、其他字元、絕對值超過一百萬。

正常 exit code 0，錯誤數值、未提供／無法開啟十位元組檔案 1，無／未知題號或錯誤參數數量 2。示範標籤、檔名參數、安全轉換範圍皆為本儲存庫補定。測試將 fixture 放在暫存目錄，核對四種定位、檔案未被修改、轉換結果與 C++11 的四種結構賦值／初始化情境。

## 範例

每題各一例。Q3 附匿名十位元組 fixture，僅用於只讀定位。Q5 展示 C++11 下 C 可行，而非重現舊式非法結論。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
iostream: standard input/output
fstream: file streams
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3 sample.dat`

輸入檔 `sample.dat`：
```text
abcdefghij
```

標準輸入：
```text

```

標準輸出：
```text
A: 2
B: 18
C: 2
D: 2
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text
-12.5
```

標準輸出：
```text
atof (double): -12.5
atoi (int): -12
atol (long): -12
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text

```

標準輸出：
```text
A: 1 2
B: 1 2
C (C++11): 1 2
D: 1 2
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct Plain {
    int first;
    int second;
};

struct Defaulted {
    int first = 1;
    int second = 2;
};

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: quiz06 2|3|4|5 [input-file]\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode != "3" && argc != 2) return 2;
    if (mode == "2") {
        std::cout << "iostream: standard input/output\nfstream: file streams\n";
    } else if (mode == "3") {
        if (argc != 3) return 1;
        std::ifstream input(argv[2], std::ios::binary);
        if (!input) return 1;
        input.seekg(0, std::ios::end);
        if (input.tellg() != std::streampos(10)) return 1;
        input.seekg(1, std::ios::beg);
        input.seekg(1, std::ios::cur);
        std::cout << "A: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(8, std::ios::end);
        std::cout << "B: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(2, std::ios::beg);
        std::cout << "C: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(2);
        std::cout << "D: " << input.tellg() << '\n';
    } else if (mode == "4") {
        std::string token;
        if (!(std::cin >> token)) return 1;
        if (token.size() > 32) return 1;
        int digits = 0, points = 0;
        for (std::size_t i = 0; i < token.size(); ++i) {
            if (i == 0 && (token[i] == '+' || token[i] == '-')) continue;
            if (token[i] == '.') ++points;
            else if (token[i] >= '0' && token[i] <= '9') ++digits;
            else return 1;
        }
        if (digits == 0 || points > 1) return 1;
        std::istringstream parser(token);
        double value;
        if (!(parser >> value) || !parser.eof() || !std::isfinite(value) ||
            std::fabs(value) > 1000000) return 1;
        std::cout << "atof (double): " << std::atof(token.c_str())
                  << "\natoi (int): " << std::atoi(token.c_str())
                  << "\natol (long): " << std::atol(token.c_str()) << '\n';
    } else if (mode == "5") {
        Plain a;
        a.first = 1;
        a.second = 2;
        const Plain b = {1, 2};
        const Defaulted c;
        const Plain d = b;
        std::cout << "A: " << a.first << ' ' << a.second
                  << "\nB: " << b.first << ' ' << b.second
                  << "\nC (C++11): " << c.first << ' ' << c.second
                  << "\nD: " << d.first << ' ' << d.second << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
