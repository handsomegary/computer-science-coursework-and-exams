# 2021 Quiz 5：陣列、插入排序與稀疏矩陣

依提供的考古題目重寫。現存兩頁只包含 Q2–Q5；Q4 的原程式有計數與判定錯誤，下文分別說明「追蹤原題」與「正確實作」，不以修改後結果代替原題答案。

## Q2：哪種陣列初始化不正確？

```cpp
// A
int n[5] = {0, 7, 0, 3, 8, 2};
// B
int n[5] = {7};
// C
int n[] = {0, 7, 0, 3, 8, 2};
// D
int n[5] = {9, 1, 9};
```

答案 **A**。五個元素卻提供六個 initializer，不能編譯。B 未列出的四格補零；C 由六個 initializer 推導陣列大小為六；D 後兩格補零。driver 真正建立 B、C、D，A 只列錯誤說明。

## Q3：插入排序第 i 次處理後，哪個不變量成立？

選項 A 第 i 個位置空著；B 第 i 個元素在最終位置；C 最後 i 個元素已排序；D 前 i 個元素已排序。答案 **D**。每次把目前元素插入先前已排序的前綴，因此前綴有序，但更後面的新元素仍可能使它移位。

driver 使用從 0 起算的下標 i，插入 `values[i]` 後顯示 `prefix i+1:`；題目的 i 是已處理前綴長度，兩種計數方式描述同一不變量。使用 `>` 移位，等值不互換，因此是穩定排序。時間最壞 `O(n²)`，原地操作額外 `O(1)` 空間（輸入陣列本身另存於 vector）。

## Q4：輸入 `2 2 0 1 1 1` 時原程式印什麼？

前兩數是列、欄數，其後四數是矩陣元素。原碼的重要分支為：

```cpp
if (arr[i][j] == 0) ctr += 2;
if (ctr >= (r * c) / 2)
    cout << "The given matrix is sparse matrix.\n";
else
    cout << "The given matrix is not a sparse matrix.\n";
cout << "There are " << ctr << " number of zeros in the matrix.\n";
```

選項 A 稀疏，零元素數為 2；B 不稀疏，零元素數為 1；C 不稀疏，零元素數為 2；D 稀疏，零元素數為 1。**追蹤原題答案 A**：實際一個零卻讓 ctr 變成 2，`2>=4/2` 為真，所以錯印稀疏且零數為 2。

題目註解定義稀疏矩陣是「零元素多於非零元素」，正確計數應每次加一，判斷應為 `zeros > r*c-zeros`。因此這一矩陣正確結果是不稀疏、零數 1；剛好各半也不符合「多於」。driver 分兩行顯示 `Original:` 與 `Corrected:`，可重現原錯誤並驗證改正，不把原題答案 B 當成原程式的實際輸出。

## Q5：陣列傳入函式時，哪個說法錯？

選項 A 函式總是知道被傳入陣列的大小；B 傳入第一元素的地址；C 可以把陣列名稱當引數；D 函式能修改陣列的元素值。答案 **A**。一般 `int values[]` 參數會調整為指標，陣列長度不會自動隨指標附帶。非 const 指標可修改元素，因此 D 在一般可修改陣列的情境成立；const 參數則另有限制。

driver 明確傳入指標與長度 `arraySum(values.data(), values.size())`，逐元素計算總和，沒有用 `sizeof(pointer)` 猜長度。

## 示範介面與補定規則

編譯：`g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o quiz05`；執行 `./quiz05 2|3|4|5`。Q2 無 stdin。Q3、Q5 讀 `N` 與 N 個元素，`1<=N<=20`、元素 `-1000000..1000000`；Q3 顯示每次插入後的整個陣列及最後排序，Q5 顯示明傳長度與總和。Q4 讀 `rows columns` 和逐列元素，列／欄各 `1..10`，元素為 32 位有號整數。

正常 exit code 0，超界／缺少資料 1，無或未知題號 2。標籤、通用輸入及改正版是本儲存庫補定。獨立模型核對每一步已排序前綴、原零數雙倍錯誤、空零／半數／全零邊界與陣列總和。

## 範例

四組各涵蓋一題，Q4 清楚顯示原題與修正的差異，沒有輸入回顯。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`2`

標準輸入：
```text

```

標準輸出：
```text
A: ill-formed (six initializers for five elements)
B: 7 0 0 0 0
C: 0 7 0 3 8 2
D: 9 1 9 0 0
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

執行參數：`3`

標準輸入：
```text
5 4 2 4 -1 3
```

標準輸出：
```text
prefix 2: 2 4 4 -1 3
prefix 3: 2 4 4 -1 3
prefix 4: -1 2 4 4 3
prefix 5: -1 2 3 4 4
sorted: -1 2 3 4 4
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

執行參數：`4`

標準輸入：
```text
2 2 0 1 1 1
```

標準輸出：
```text
Original: sparse; reported zeros: 2
Corrected: not sparse; zeros: 1
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`5`

標準輸入：
```text
3 7 -2 5
```

標準輸出：
```text
explicit size: 3
sum: 10
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 C++11 示範程式

<!-- SOLUTION_CODE_BEGIN -->

### `main.cpp`

```cpp
#include <iostream>
#include <string>
#include <vector>

bool readArray(std::vector<int>& values) {
    int count;
    if (!(std::cin >> count) || count < 1 || count > 20) return false;
    values.resize(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i)
        if (!(std::cin >> values[i]) || values[i] < -1000000 || values[i] > 1000000)
            return false;
    return true;
}

void printArray(const std::vector<int>& values) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) std::cout << ' ';
        std::cout << values[i];
    }
    std::cout << '\n';
}

int arraySum(const int values[], std::size_t count) {
    int sum = 0;
    for (std::size_t i = 0; i < count; ++i) sum += values[i];
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz05 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        const int b[5] = {7};
        const int c[] = {0, 7, 0, 3, 8, 2};
        const int d[5] = {9, 1, 9};
        std::cout << "A: ill-formed (six initializers for five elements)\nB:";
        for (int value : b) std::cout << ' ' << value;
        std::cout << "\nC:";
        for (int value : c) std::cout << ' ' << value;
        std::cout << "\nD:";
        for (int value : d) std::cout << ' ' << value;
        std::cout << '\n';
    } else if (mode == "3") {
        std::vector<int> values;
        if (!readArray(values)) return 1;
        for (std::size_t i = 1; i < values.size(); ++i) {
            const int key = values[i];
            std::size_t j = i;
            while (j > 0 && values[j - 1] > key) {
                values[j] = values[j - 1];
                --j;
            }
            values[j] = key;
            std::cout << "prefix " << i + 1 << ": ";
            printArray(values);
        }
        std::cout << "sorted: ";
        printArray(values);
    } else if (mode == "4") {
        int rows, columns;
        if (!(std::cin >> rows >> columns) || rows < 1 || rows > 10 ||
            columns < 1 || columns > 10) return 1;
        int zeros = 0;
        for (int i = 0; i < rows * columns; ++i) {
            int value;
            if (!(std::cin >> value)) return 1;
            if (value == 0) ++zeros;
        }
        const int original = zeros * 2;
        std::cout << "Original: "
                  << (original >= rows * columns / 2 ? "sparse" : "not sparse")
                  << "; reported zeros: " << original << '\n';
        std::cout << "Corrected: "
                  << (zeros > rows * columns - zeros ? "sparse" : "not sparse")
                  << "; zeros: " << zeros << '\n';
    } else if (mode == "5") {
        std::vector<int> values;
        if (!readArray(values)) return 1;
        std::cout << "explicit size: " << values.size()
                  << "\nsum: " << arraySum(values.data(), values.size()) << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
```

<!-- SOLUTION_CODE_END -->
