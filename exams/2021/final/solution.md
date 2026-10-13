# 2021 期末考：重寫題目、解法與程式

## 資料範圍

依考後回憶及另附參考程式重寫。回憶中的部分附件缺失，以下明示重建或補定，不把新題面假稱為原試卷逐字題目。編譯環境為 C++11，字元追蹤採 ASCII。

## 1. 術語

| 名詞 | 解釋 |
|---|---|
| character constant | 字元常數，例如 `'A'`；與字串 `"A"` 的型別及表示不同 |
| delimiter | 區隔資料的字元，例如 CSV 的逗號或輸入行的換行 |
| DBMS | 管理資料庫的系統，處理儲存、查詢、更新、權限及交易 |
| binary scope resolution | `::` 使用左右兩側指定作用域，例如 `std::cout` 或 `Class::member`；也可用 `::name` 指全域作用域 |
| driver | 呼叫並展示待測函式／元件的測試或示範程式 |
| walking off an array | 超出合法陣列元素範圍；形成合法尾後指標與解參考尾後指標不同，後者是未定義行為 |

## 2. 三種參數傳遞

**重寫題目：** 比較值、參考與指標，示範對呼叫者資料的影響。

傳值 `int` 複製數值；`int&` 參考綁定原變數；`int*` 複製指標值，透過它可修改指向的變數，使用前需確保指向有效物件。指標參數不是自動的參考傳遞。`passing` 由 5 開始，依序得到 5、6、7。

## 3. 字元指標追蹤

**重建依據：** 回憶題面缺原附件，採同資料夾另附的三字元指標程式重建。初值 `blocks={'A','B','C'}`、`ptr=blocks`。

| 指令（取值至 temp） | temp | ptr 索引／資料改變 |
|---|---|---|
| `blocks[0]` | A | 0 |
| `*(blocks+2)` | C | 0 |
| `*(ptr+1)` | B | 0 |
| `*ptr` | A | 0 |
| `ptr=blocks+1; *ptr` | B | 1 |
| `*(ptr+1)` | C | 1 |
| `ptr=blocks; *++ptr` | B | 1：先移指標再取值 |
| `++*ptr` | C | 1：blocks[1] 由 B 變 C |
| `*ptr++` | C | 2：取舊位置的值，再移指標 |
| `*ptr` | C | 2 |
| `*blocks+1` | B | 2：A 的數值加 1，不是移動指標 |

`temp=&*ptr` 的右側為 char*，不能賦給 char；`ptr=temp&` 語法不合法。這兩行只供診斷，不放進可編譯程式。`*ptr++` 解析成 `*(ptr++)`，而 `++*ptr` 解析成 `++(*ptr)`。

## 4. 不用索引存取三維陣列

**重建題目：** 使用另附參考程式中的 `int array_test[2][3][4]` 與 24 個初值，透過指標加總全部元素。宣告可使用方括號，元素存取不可使用 `a[i]`。

先透過 `*(*(array_test+plane)+row)` 取得該列的 int 指標，再以 `*(arrayPtr+col)` 存取四個元素。每個 int 指標的算術範圍都限於同一列；避免拿第一個元素的 int* 直接跨過多個巢狀子陣列。總和為 206，時間 O(元素數)，額外空間 O(1)。

## 5. strtok 字串切割

**補定輸入介面：** 讀取完整一行，以空格、逗號、句點、減號、分號、驚嘆號與問號（`" ,.-;!?"`）切割，每個非空 token 印一行。

strtok 會改寫可寫入的字元緩衝區，不能直接傳字串常數。第一次傳 buffer，接下來傳 nullptr 繼續；連續分隔符會被略過，不產生空 token。程式複製到 vector<char> 並加上 NUL；本練習拒絕輸入內嵌 NUL。strtok 有內部狀態，巢狀交錯切割需要其他方法。

## 6. 類別建構與成員函式

**重寫題目：** 類別具有兩個 private int，建構子接收兩數，成員函式回傳其和。main 從標準輸入讀取兩數並印結果。

使用初始化列表與 const 成員函式；相加前轉 long long，因此兩個 INT_MAX 的和也能正確表示。讀取失败回傳 1。`class` 模式完成此題。

## 7. 計算陣列長度

真實陣列可用 `sizeof(array)/sizeof(array[0])`，本例五個元素得到 5。當參數已調整為指標時，sizeof 得到的是指標大小，不能用這個公式推回長度。需要另傳元素數量或使用有大小資訊的容器。

## 8. C 字串比較

**題目：** 比較 `National Central University` 與 `National Tsing Hua University`，分析 strcmp 全字串、strncmp 前 8 字元、反向 strncmp 前 14 字元。

結果依序為負、零、正。前八字元都是 `National`；完整比較第一個差異為 C 與 T。標準只保證返回值的正負或零，不保證恰為 −17 或 +17。程式將結果正規化成 −1、0、1，方便跨平台檢查。

## 9. 選擇題解答

回憶記錄缺失或含不合法片段的項目依語意解答；只有可辨識的選項保留字母。

| 主題 | 解答與原因 |
|---|---|
| 資料階層 | bit → byte → field → record → file，由小到大 |
| 結構陣列 | 已宣告 `struct Data` 後可用 `Data MyArray[10];` |
| ofstream 的 `!` | c：測試串流失敗狀態，並不關閉檔案；關檔用 close 或解構 |
| 不合法指標 | 未初始化、nullptr、失效指標解參考均不合法；一般變數讀值不是指標解參考，記錄選項 d |
| 指標型別 | 不相容型別不能任意直接賦值；物件指標可轉 void*。顯式轉型並不保證其後解參考安全，記錄選項 b |
| selection sort | c：完成前 i 輪後，前 i 個元素是最小的 i 個並已遞增排列（以一為首輪計數） |
| setfill | c：填補字元設定持續有效；setw 寬度通常只影響下一次格式化輸出 |
| typedef | a：替現有型別建立別名，不宣告具有全新表示的新型別 |
| 二維陣列 | 原記錄 `Int b[2,2]` 不合法。重建為 `int b[2][2]={{1},{3,4}};`，`b[1][0]` 為 3，省略的初值補 0 |
| cin.getline(buf,30) | b：預設分隔符為換行；最多存 29 個字元加 NUL，過長會設定失敗狀態 |
| 結構指標成員 | b：`pt->x_center` 等同 `(*pt).x_center`，pt 必須有效 |

`selection` 模式以 7,3,5,1 示範三輪選擇排序。`fill`、`matrix`、`member` 提供對應可執行練習。

## 10. const 指標

`int *const myPtr=&x` 是「指標本身不能改指向」，可用 `*myPtr` 修改 x。`const int *ptr` 則限制透過此指標修改值，但指標可以改指向。`constptr` 模式將 x 從 7 改為 9。

## 11. 檔案讀取追蹤

**重建依據：** 另附程式先寫入 `123 456`，之後以 `char`、`int` 為一組讀兩次。char 格式化讀取預設略過空白但只取一字元。

第一次取字元 `'1'`，接著整數讀到 23；第二次略過空白取 `'4'`，接著讀到 56。輸出四行：`character:1`、`integer:23`、`character:4`、`integer:56`。程式在執行目錄寫入 `integers.txt`，先關閉寫入串流再開啟讀取串流，並檢查開檔及讀寫成功。

## 可重現範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`passing`

標準輸入：
```text

```

標準輸出：
```text
5
6
7
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`pointers`

標準輸入：
```text

```

標準輸出：
```text
A
C
B
A
B
C
B
C
C
C
B
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`array3d`

標準輸入：
```text

```

標準輸出：
```text
206
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`tokens`

標準輸入：
```text
 Hello,,world!C++ - pointers? 
```

標準輸出：
```text
Hello
world
C++
pointers
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 5（`main.cpp`）

執行參數：`class`

標準輸入：
```text
2147483647 2147483647
```

標準輸出：
```text
4294967294
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 6（`main.cpp`）

執行參數：`length`

標準輸入：
```text

```

標準輸出：
```text
5
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 7（`main.cpp`）

執行參數：`compare`

標準輸入：
```text

```

標準輸出：
```text
-1
0
1
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 8（`main.cpp`）

執行參數：`selection`

標準輸入：
```text

```

標準輸出：
```text
1 3 5 7
1 3 5 7
1 3 5 7
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 9（`main.cpp`）

執行參數：`fill`

標準輸入：
```text

```

標準輸出：
```text
0007 0008
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 10（`main.cpp`）

執行參數：`matrix`

標準輸入：
```text

```

標準輸出：
```text
3
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 11（`main.cpp`）

執行參數：`member`

標準輸入：
```text

```

標準輸出：
```text
3 3
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 12（`main.cpp`）

執行參數：`constptr`

標準輸入：
```text

```

標準輸出：
```text
9
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 13（`main.cpp`）

執行參數：`file`

標準輸入：
```text

```

標準輸出：
```text
character:1
integer:23
character:4
integer:56
```

產生檔 `integers.txt`：
```text
123 456
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整參考程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
#include <limits>

void by_value(int value) { ++value; }
void by_reference(int& value) { ++value; }
void by_pointer(int* value) { if (value != nullptr) ++*value; }
class Sum {
    int first_, second_;
public:
    Sum(int first, int second) : first_(first), second_(second) {}
    long long value() const { return static_cast<long long>(first_) + second_; }
};
int sign(int value) { return (value > 0) - (value < 0); }
int main(int argc, char* argv[]) {
    if (argc != 2) { std::cerr << "Usage: final MODE\n"; return 1; }
    const std::string mode = argv[1];
    if (mode == "passing") {
        int value = 5;
        by_value(value); std::cout << value << '\n';
        by_reference(value); std::cout << value << '\n';
        by_pointer(&value); std::cout << value << '\n';
    } else if (mode == "pointers") {
        char blocks[3] = {'A', 'B', 'C'};
        char* ptr = blocks;
        std::cout << blocks[0] << '\n' << *(blocks + 2) << '\n' << *(ptr + 1) << '\n' << *ptr << '\n';
        ptr = blocks + 1;
        std::cout << *ptr << '\n' << *(ptr + 1) << '\n';
        ptr = blocks;
        char temp = *++ptr; std::cout << temp << '\n';
        temp = ++*ptr; std::cout << temp << '\n';
        temp = *ptr++; std::cout << temp << '\n';
        temp = *ptr; std::cout << temp << '\n';
        temp = static_cast<char>(*blocks + 1); std::cout << temp << '\n';
    } else if (mode == "array3d") {
        int array_test[2][3][4] = {3,4,2,3,0,-3,9,11,23,12,23,2,13,4,56,3,5,9,3,5,5,1,4,9};
        long long sum = 0;
        int* arrayPtr = nullptr;
        for (int plane = 0; plane < 2; ++plane) for (int row = 0; row < 3; ++row) {
            arrayPtr = *(*(array_test + plane) + row);
            for (int col = 0; col < 4; ++col) sum += *(arrayPtr + col);
        }
        std::cout << sum << '\n';
    } else if (mode == "tokens") {
        std::string line;
        if (!std::getline(std::cin, line)) return 1;
        if (line.find('\0') != std::string::npos) return 1;
        std::vector<char> buffer(line.begin(), line.end());
        buffer.push_back('\0');
        for (char* token = std::strtok(buffer.data(), " ,.-;!?"); token != nullptr; token = std::strtok(nullptr, " ,.-;!?"))
            std::cout << token << '\n';
    } else if (mode == "class") {
        int first, second;
        if (!(std::cin >> first >> second)) return 1;
        std::cout << Sum(first, second).value() << '\n';
    } else if (mode == "length") {
        int values[] = {1,2,3,4,5};
        std::cout << sizeof(values) / sizeof(values[0]) << '\n';
    } else if (mode == "compare") {
        const char* a = "National Central University";
        const char* b = "National Tsing Hua University";
        std::cout << sign(std::strcmp(a,b)) << '\n' << sign(std::strncmp(a,b,8)) << '\n' << sign(std::strncmp(b,a,14)) << '\n';
    } else if (mode == "selection") {
        int values[] = {7,3,5,1};
        for (int i = 0; i < 3; ++i) {
            int smallest = i;
            for (int j = i + 1; j < 4; ++j) if (values[j] < values[smallest]) smallest = j;
            const int saved = values[i]; values[i] = values[smallest]; values[smallest] = saved;
            for (int j = 0; j < 4; ++j) std::cout << (j == 0 ? "" : " ") << values[j];
            std::cout << '\n';
        }
    } else if (mode == "fill") {
        std::cout << std::setfill('0') << std::setw(4) << 7 << ' ' << std::setw(4) << 8 << '\n';
    } else if (mode == "matrix") {
        int b[2][2] = {{1}, {3,4}};
        std::cout << b[1][0] << '\n';
    } else if (mode == "member") {
        struct Point { int x_center; int y_center; };
        Point point = {3,4};
        Point* pt = &point;
        std::cout << pt->x_center << ' ' << (*pt).x_center << '\n';
    } else if (mode == "constptr") {
        int x = 7;
        int* const myPtr = &x;
        *myPtr = 9;
        std::cout << x << '\n';
    } else if (mode == "file") {
        const char* filename = "integers.txt";
        { std::ofstream output(filename); if (!output) return 1; output << 123 << ' ' << 456; if (!output) return 1; }
        std::ifstream input(filename);
        char character;
        int integer;
        for (int i = 0; i < 2; ++i) {
            if (!(input >> character >> integer)) return 1;
            std::cout << "character:" << character << "\ninteger:" << integer << '\n';
        }
    } else { std::cerr << "Unknown mode\n"; return 1; }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
