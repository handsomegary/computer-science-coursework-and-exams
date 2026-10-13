# 2021 PA4：MySpice 電阻電路分析

本題面依最新修訂的 MySpice 作業、Q&A、三組公開測資及中段合併補充整理。以下「本版補定」是可重現的儲存庫介面規則，不宣稱為原講義文字。

## 題目

設計直流電阻電路模擬器。電路由一個理想電壓源與 1–15 個依序連接的 stage 組成。每個 stage 有 1–15 個 `RS` 電阻形成串聯進入路徑，也有 1–15 個 `RP` 電阻形成從 stage 節點到地的串聯支路。`RP` 的「並聯」是指這條支路與後續 stage 支路並聯，並非同一條 `RP` 路徑內所有電阻直接並聯。

第一個 stage 的 `RS` 路徑從 `vdd` 到 `n1`，下一個 stage 的 `RS` 路徑從前一 stage 節點出發。原始電阻名稱 `RSy_z`、`RPy_z` 的 `y` 是原始 stage ID，`z` 是路徑內序號。`nY` 為 stage 節點；`nsY_Z`、`npY_Z` 為串聯／支路內節點。交換 stage 後保留電阻與節點名稱，只改變必要連線。

提供功能：讀取電路、求節點電壓與每個電阻電流、交換相鄰 stage、合併相鄰 stage、匯出電路與計算結果、結束。加分版另可匯出 Thevenin 等效結果。所有結果必須由目前電路計算。

## 檔案輸入

```text
stage 2

v1 vdd gnd 5V

RS1_1 vdd ns1_1 2K
RS1_2 ns1_1 n1 2K
RP1_1 n1 gnd 2K
RS2_1 n1 n2 2K
RP2_1 n2 np2_1 3K
RP2_2 np2_1 gnd 3K

end
```

每行以空白分欄。`stage` 後為 stage 數；電壓源一行依序為名稱、`vdd`、`gnd`、整數電壓；電阻一行依序為名稱、起點、終點、電阻值。電流正方向就是該行起點到終點，所有節點電壓皆以 `gnd` 為零。

官方電阻數值為 1–999 乘前綴，整體範圍 `1u..999G`，也可不帶前綴。支援 `u=10^-6`、`m=10^-3`、`K=10^3`、`Meg=10^6`、`G=10^9`；輸出依 Q&A 使用大寫 `K`。電壓不使用工程前綴，32-bit 有號整數可處理。

本版也接受小數或科學記號電阻、`k` 輸入，以及 `f/p/n/T/P` 前綴，供合併後阻值與匯出檔重新讀取；電阻必須為正且有限；擴充範圍若造成等效阻值、電流或電壓溢位／下溢，會回報錯誤而不輸出 NaN 或 infinity。官方數值範圍可完整處理。電壓可帶 `V` 或 `v`，也可無尾綴，包含零與負整數。UTF-8 BOM 與空白行省略，電阻列可重新排列，stage 順序由連線推導。輸入遇 `end` 或 `***calculation results***` 即結束電路部分，亦接受匯出檔的 `***circuits***` 標題。

parser 驗證 stage 數、各路徑長度、單一電壓源、唯一電阻名稱、完整且非循環的 `RS`／`RP` 路徑與節點連接度；孤立電阻、額外分支、錯誤連線和零／負電阻皆拒絕。讀取失敗不覆蓋已載入的電路。

## 操作與輸出

不帶命令列參數時執行互動菜單。啟動顯示 `*** Welcome to MySpice ***`，菜單為：

1. 輸入檔案。檔案不存在時顯示 `The file is not found.` 並重問。成功顯示 `Loading the file...` 與 `The input file successfully loaded!`。
2. 計算電壓與電流。輸出 `Calculating...`、成功文字及計算表。
3. 交換目前位置的兩個相鄰 stage。所有受到影響的 stage 進入連線都重新接線，包含交換位置之後的 stage；名稱跟著原 stage 移動。
4. 合併目前位置的兩個相鄰 stage，保留較前方的 `RS` 路徑與節點，移除後方 stage，換上新的單一 `RP` 電阻。
5. 指定檔名並匯出目前電路和重新計算的結果。
6. 顯示 `Bye~` 並結束。
7. 僅加分版提供：匯出 Thevenin 結果。

未載入電路時，計算、交換、合併及匯出都顯示精確文字 `There is no circuit`。不存在的位置會重問；不相鄰的位置也重問。只有一個 stage 時交換／合併直接顯示 `At least two stages are required.`。無效選項重問，EOF 直接正常結束。

計算表使用 38 個 `*` 作上下邊界，每列也是 38 個 ASCII 字元，名稱與值以空格對齊。預設顯示 `vdd` 和所有 stage 節點，符合公開計算表；每個內部節點仍有計算，批次 `--all-nodes` 可一併顯示。表中電壓尾綴 `v`、電流尾綴 `A`，非零值改成工程記號並四捨五入至三位小數；進位到 `1000.000` 時升級前綴。零顯示 `0.000`。負值以絕對量值決定前綴。

本版擴充輸出前綴 `f/p/n/T/P`，因為合法的大電阻可能產生很小的電流。15 級高衰減電路甚至可低於 femto，因此極端值以三位小數的科學記號保留非零結果；固定有限前綴無法表示所有合法電路。正式範例中的一般數值仍採講義的工程記號。

一般匯出檔含 `***circuits***`、stage 數、電壓源及電阻列、`end`，再接 `***calculation results***` 與計算表。電阻列一律三位小數、前綴 `K` 大寫；電壓源保留整數。`end` 是本版增加的明確電路結尾，使結果檔可再次載入。畫面不印 `***calculation results***`。因電阻顯示為三位小數，重新讀取一般匯出檔會使用顯示後的阻值；精確數值驗證使用 `--raw`。

### 可重現的批次介面

```sh
./myspice input.txt result.txt [--swap i j] [--merge i j] [--raw] [--all-nodes]
./myspice_bonus input.txt thevenin.txt [--swap i j] [--merge i j] [--raw]
./myspice_bonus input.txt legacy.txt --legacy-thevenin
./myspice_bonus input.txt result.txt --circuit
```

前兩個參數為輸入與輸出檔名，位置操作依命令列順序執行。`i/j` 指目前排列位置，不是名稱中的原始 ID；反向輸入相鄰位置會先排序。成功結束碼為 0，標準輸出與錯誤皆空；輸入、位置或檔案錯誤結束碼為 1，原因寫至標準錯誤。一般版預設匯出電路與計算表；加分版預設匯出 Thevenin，加 `--circuit` 改為一般輸出，加 `--thevenin` 可再切回。

一般版 `--raw` 輸出 `input_resistance`、`vth`、`rth` 各一行，再依名稱排序輸出 `voltage 節點 值` 和 `current 電阻 值`。包含 `vdd`、`gnd` 與全部內部節點，使用 `long double` 的最大往返精度和科學記號。加分 Thevenin 的 `--raw` 則只輸出 `vth`、`rth`。單位分別為 Ohm、V、A。

## 解法與報告

程式分為 `Circuit`／`Stage`／`Resistor` 資料模型、檔案 parser、計算、接線修改、格式化與互動／批次入口。`main.cpp` 和 `bonus.cpp` 各自包含完整共用實作 `myspice.hpp`，分開編譯；必需的 `void calculation(...)`、`void swap_stage(...)`、`void merge_stage(...)` 均有實作。

### 電路計算

令第 i 級 `RS` 路徑總和為 `S_i`，`RP` 支路總和為 `P_i`。從 stage 節點向後看見的負載為 `L_i`：

```text
L_last = P_last
L_i = P_i || (S_(i+1) + L_(i+1))
R_input = S_1 + L_1
```

其中 `a || b = a*b/(a+b)`；實作改用 `min(a,b)/(1+min(a,b)/max(a,b))` 避免大數乘法。由電源向前，每級上游電壓 `U_i` 得到：

```text
I_RS_i = U_i / (S_i + L_i)
V_i = U_i * L_i / (S_i + L_i)
I_RP_i = V_i / P_i
U_(i+1) = V_i
```

每條路徑內各電阻電流相同。內部節點從接地端或 stage 節點往回累加 `I*R` 壓降，避免從大電壓連續相減時抹去微小剩餘電壓。資料與計算採 `long double`，所有修改後都重新求解，沒有舊結果快取。

讀檔因 map 查找為 `O(R log R)`，計算與重新接線為 `O(R)`，額外資料空間 `O(R)`；最多 15×(15+15)=450 個電阻。

### 交換與合併

交換直接移動兩個完整 `Stage`，再從 `vdd` 往後重接每級第一個 `RS` 的起點。內部連線、電阻名稱與 stage 節點名稱保持跟隨其原 stage。

合併相鄰的前級 A、後級 B：

```text
new_RP = P_A || (S_B + P_B)
```

保留 A 的串聯路徑與節點，刪除 A 的舊 `RP` 及整個 B，新的 `RP` 由 A 節點接地，再重接剩餘後級。新電阻名稱為 `RP` 加上合併時 A 的「目前位置」。例如原 stage 2 被換到位置 1 後與位置 2 合併，名稱仍是 `RP1`，而保留節點為 `n2`。如多次合併產生同名，本版附加 `_m1`、`_m2` 等唯一後綴；Q&A 保證正式測資不會有此衝突。

中段合併會改變全網等效電阻，因為合併公式移除的是 B 自身支路，而後續負載重新接到 A。它不是對全網總輸入電阻的等效化；這正是 Q&A 要求最後重新計算的原因。中段補充測資合併後 `RP1=8K`、`n1=4V`、`n3=2V`，本程式符合。

### Thevenin 加分版

觀察端點 A 為最後 stage 節點，B 為地；原電路的最後 `RP` 支路仍屬電路內部。開路表示沒有額外外接負載，所以 `Vth` 就是正常解的最後 stage 電壓。將理想電壓源短路後，從第一個 stage 向後求驅動電阻：

```text
D_0 = 0
D_i = P_i || (S_i + D_(i-1))
Rth = D_last
Vth = V_last
```

舊公開 bonus 檔的 `Vth=Vdd`、`Rth=R_input` 與最後節點的 A–B 端點並不一致。例如公開 case 1 交換、合併後為 5V 源接 2K 串聯與 3K 對地：最後節點為 3V，源短路後看見 `2K||3K=1.2K`；公開表卻寫 5V、5K。預設輸出物理結果 3V、1.2K。

為能重現舊公開表，`--legacy-thevenin` 明確改輸出 `Vdd` 和 `R_input`。它是兼容模式，不把這組舊值稱為最後節點的實際 Thevenin。互動加分版也可使用 `./myspice_bonus --legacy-thevenin`，再由菜單 7 匯出。

Thevenin 檔固定三行：標題 `***Thevenin equivalence circuit***`、`Rth = 工程數值`、`Vth = 工程數值v`，與舊公開檔的文字格式一致。

### 編譯與驗證

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror main.cpp -o myspice
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror bonus.cpp -o myspice_bonus
```

驗證使用獨立的一般節點 KCL 矩陣：由每個電阻的兩端與導納建立線性方程，再以 90 位精度 Decimal Gaussian elimination 求解所有內部節點，沒有重用 C++ 的 ladder 公式。電源置為零並在輸出節點注入 1A，所得端點電壓獨立驗證 `Rth`。所有電流由端點電壓差除以電阻求出。

測試包含 55 組固定種子隨機電路、零／負電壓、電阻列亂序、15 級且每條路徑 15 電阻、`1u..999G`、15 級極小衰減、三組公開操作、中段合併、源關閉的 Thevenin、舊表兼容、工程記號進位、內部節點顯示、結果重新讀入、多次合併唯一名稱、無電路防呆、缺檔重問、錯誤拓撲與非法位置。完整 PA4 測試 11 組、99 次程式執行通過；兩個入口均以 C++11 嚴格警告編譯通過。測試來源為本版程式與獨立模型，未執行原始 EXE。

主要困難是區分 `RP` 路徑內串聯與路徑間並聯、保留名稱但使用目前位置、處理中段合併改變負載，以及 Thevenin 圖和舊 bonus 表的差異。模型以端點建立路徑，修改後統一重新接線和計算；物理與兼容模式分開選擇，測試同時驗證兩者。建議題面對「並聯支路」直接畫出端點，並明確註記 Thevenin 觀察端口與是否移除負載，能減少這些歧義。

## 講義規則與本版補定

講義與更新可確認：1–15 stage、每條 `RS`／`RP` 路徑 1–15 電阻、工程前綴與三位小數、讀檔／計算／交換／合併／匯出／離開、缺電路文字、三個指定 `void` 函式、目前相鄰位置、合併公式、保留原元件名稱、大寫 `K`、只在檔案印結果標題、C++11、獨立加分入口及 Thevenin 圖。

本版補定：批次參數、反向相鄰位置先排序、完整拓撲驗證、EOF／非法選項恢復、內部節點輸出選項、精確 raw 輸出、額外工程前綴與極端科學記號、唯一 merge 名稱、匯出檔增加 `end`、固定 ASCII 表格間距、物理 Thevenin 預設及明示的舊公開值兼容。這是純電阻直流題，沒有元件電容／電感或暫態時間序列規格。

## 完整程式

兩個入口應各自與同目錄 `myspice.hpp` 編譯，不把兩個 `main` 放進同一執行檔。

<!-- SOLUTION_CODE_BEGIN -->

### `bonus.cpp`

```cpp
#include "myspice.hpp"

int main(int argc, char* argv[]) {
    return myspice::run(argc, argv, true);
}
```

### `main.cpp`

```cpp
#include "myspice.hpp"

int main(int argc, char* argv[]) {
    return myspice::run(argc, argv, false);
}
```

### `myspice.hpp`

```cpp
#ifndef MYSPICE_HPP
#define MYSPICE_HPP

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace myspice {

struct Resistor {
    std::string name, from, to;
    long double ohms;
};

struct Stage {
    std::string node;
    std::vector<Resistor> series, shunt;
};

struct Circuit {
    std::string source_name;
    long double voltage;
    std::vector<Stage> stages;
    Circuit() : source_name("v1"), voltage(0) {}
};

struct Result {
    long double input_resistance, vth, rth;
    std::map<std::string, long double> voltage, current;
};

inline std::string trim(const std::string& value) {
    const std::string white = " \t\r\n";
    const std::size_t first = value.find_first_not_of(white);
    if (first == std::string::npos) return "";
    return value.substr(first, value.find_last_not_of(white) - first + 1);
}

inline int integer(const std::string& token) {
    std::istringstream in(token);
    long long value = 0;
    char extra = 0;
    if (!(in >> value) || (in >> extra) || value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max()) {
        throw std::runtime_error("an integer is required");
    }
    return static_cast<int>(value);
}

inline long double number(const std::string& token, bool resistance) {
    errno = 0;
    char* last = NULL;
    const long double mantissa = std::strtold(token.c_str(), &last);
    if (last == token.c_str() || errno == ERANGE || !std::isfinite(mantissa)) {
        throw std::runtime_error("invalid numeric value: " + token);
    }
    const std::string suffix(last);
    long double scale = 1;
    if (resistance) {
        if (suffix == "f") scale = 1e-15L;
        else if (suffix == "p") scale = 1e-12L;
        else if (suffix == "n") scale = 1e-9L;
        else if (suffix == "u") scale = 1e-6L;
        else if (suffix == "m") scale = 1e-3L;
        else if (suffix == "K" || suffix == "k") scale = 1e3L;
        else if (suffix == "Meg") scale = 1e6L;
        else if (suffix == "G") scale = 1e9L;
        else if (suffix == "T") scale = 1e12L;
        else if (suffix == "P") scale = 1e15L;
        else if (!suffix.empty()) throw std::runtime_error("invalid resistor prefix: " + suffix);
        if (!(mantissa > 0)) throw std::runtime_error("resistance must be positive");
    } else {
        if (!suffix.empty() && suffix != "V" && suffix != "v") {
            throw std::runtime_error("source voltage accepts only V or v");
        }
        if (std::floor(mantissa) != mantissa || mantissa < std::numeric_limits<int>::min() ||
            mantissa > std::numeric_limits<int>::max()) {
            throw std::runtime_error("source voltage must fit a signed 32-bit integer");
        }
    }
    const long double value = mantissa * scale;
    if (!std::isfinite(value) || (resistance && value == 0)) {
        throw std::runtime_error("numeric value is outside the supported range");
    }
    return value;
}

inline long double total(const std::vector<Resistor>& path) {
    long double value = 0;
    for (std::size_t i = 0; i < path.size(); ++i) {
        value += path[i].ohms;
        if (!std::isfinite(value)) throw std::runtime_error("path resistance is outside the supported range");
    }
    return value;
}

inline long double parallel(long double a, long double b) {
    if (!(a > 0) || !(b > 0) || !std::isfinite(a) || !std::isfinite(b)) {
        throw std::runtime_error("equivalent resistance is outside the supported range");
    }
    if (a > b) std::swap(a, b);
    const long double equivalent = a / (1 + a / b);
    if (equivalent == 0) throw std::runtime_error("equivalent resistance underflows the supported range");
    return equivalent;
}

// Path topology determines stage order, including circuits exported after swaps/merges.
inline Circuit read_circuit(std::istream& input) {
    Circuit circuit;
    int count = 0;
    bool have_count = false, have_source = false;
    std::vector<Resistor> resistors;
    std::set<std::string> names;
    std::string line;
    while (std::getline(input, line)) {
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        line = trim(line);
        if (line.empty() || line == "***circuits***") continue;
        if (line == "end" || line == "***calculation results***") break;
        std::istringstream fields(line);
        std::vector<std::string> words;
        std::string word;
        while (fields >> word) words.push_back(word);
        if (words[0] == "stage") {
            if (have_count || words.size() != 2) throw std::runtime_error("invalid stage header");
            count = integer(words[1]);
            if (count < 1 || count > 15) throw std::runtime_error("stage count must be 1..15");
            have_count = true;
        } else if (words[0][0] == 'v' || words[0][0] == 'V') {
            if (have_source || words.size() != 4 || words[1] != "vdd" || words[2] != "gnd") {
                throw std::runtime_error("one voltage source from vdd to gnd is required");
            }
            circuit.source_name = words[0];
            circuit.voltage = number(words[3], false);
            have_source = true;
        } else {
            if (words.size() != 4 || words[0].size() < 3 || words[0][0] != 'R' ||
                (words[0][1] != 'S' && words[0][1] != 'P') || words[1] == words[2]) {
                throw std::runtime_error("invalid resistor line");
            }
            if (!names.insert(words[0]).second) throw std::runtime_error("duplicate resistor name");
            Resistor resistor = {words[0], words[1], words[2], number(words[3], true)};
            resistors.push_back(resistor);
        }
    }
    if (!have_count || !have_source) throw std::runtime_error("stage header and voltage source are required");
    std::map<std::string, std::size_t> series_from, shunt_from;
    for (std::size_t i = 0; i < resistors.size(); ++i) {
        std::map<std::string, std::size_t>& paths =
            resistors[i].name[1] == 'S' ? series_from : shunt_from;
        if (!paths.insert(std::make_pair(resistors[i].from, i)).second) {
            throw std::runtime_error("ambiguous resistor path");
        }
    }
    std::set<std::size_t> used;
    std::set<std::string> junctions;
    std::string start = "vdd";
    for (int i = 0; i < count; ++i) {
        Stage stage;
        std::string node = start;
        do {
            const std::map<std::string, std::size_t>::const_iterator found = series_from.find(node);
            if (found == series_from.end() || !used.insert(found->second).second) {
                throw std::runtime_error("broken or cyclic series path");
            }
            const Resistor& resistor = resistors[found->second];
            stage.series.push_back(resistor);
            if (stage.series.size() > 15) throw std::runtime_error("each series path must have 1..15 resistors");
            node = resistor.to;
        } while (shunt_from.find(node) == shunt_from.end());
        stage.node = node;
        if (node == "vdd" || node == "gnd" || !junctions.insert(node).second) {
            throw std::runtime_error("invalid stage junction");
        }
        do {
            const std::map<std::string, std::size_t>::const_iterator found = shunt_from.find(node);
            if (found == shunt_from.end() || !used.insert(found->second).second) {
                throw std::runtime_error("broken or cyclic shunt path");
            }
            const Resistor& resistor = resistors[found->second];
            stage.shunt.push_back(resistor);
            if (stage.shunt.size() > 15) throw std::runtime_error("each shunt path must have 1..15 resistors");
            node = resistor.to;
        } while (node != "gnd");
        circuit.stages.push_back(stage);
        start = stage.node;
    }
    if (used.size() != resistors.size() || series_from.find(start) != series_from.end()) {
        throw std::runtime_error("stage count does not match the ladder topology");
    }
    // Internal nodes belong to exactly one resistor path. Cross-connections are not ladders.
    std::map<std::string, int> incidence;
    for (std::size_t i = 0; i < resistors.size(); ++i) {
        ++incidence[resistors[i].from];
        ++incidence[resistors[i].to];
    }
    for (std::map<std::string, int>::const_iterator it = incidence.begin(); it != incidence.end(); ++it) {
        if (it->first != "vdd" && it->first != "gnd" && junctions.count(it->first) == 0 && it->second != 2) {
            throw std::runtime_error("an internal node must connect exactly two resistors");
        }
    }
    if (incidence["vdd"] != 1 || incidence["gnd"] != count) {
        throw std::runtime_error("invalid ladder terminals");
    }
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        if (incidence[circuit.stages[i].node] != (i + 1 == circuit.stages.size() ? 2 : 3)) {
            throw std::runtime_error("invalid stage junction connections");
        }
    }
    return circuit;
}

inline Circuit load(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("The file is not found.");
    return read_circuit(file);
}

inline Result solve(const Circuit& circuit) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    const std::size_t count = circuit.stages.size();
    std::vector<long double> load_at(count);
    for (std::size_t k = count; k > 0; --k) {
        const std::size_t i = k - 1;
        const long double shunt = total(circuit.stages[i].shunt);
        load_at[i] = i + 1 == count ? shunt :
            parallel(shunt, total(circuit.stages[i + 1].series) + load_at[i + 1]);
    }
    Result result;
    result.input_resistance = total(circuit.stages[0].series) + load_at[0];
    if (!std::isfinite(result.input_resistance)) throw std::runtime_error("input resistance is outside the supported range");
    result.voltage["vdd"] = circuit.voltage;
    result.voltage["gnd"] = 0;
    long double upstream_voltage = circuit.voltage;
    long double driving_resistance = 0;
    for (std::size_t i = 0; i < count; ++i) {
        const Stage& stage = circuit.stages[i];
        const long double series = total(stage.series);
        const long double shunt = total(stage.shunt);
        const long double resistance = series + load_at[i];
        if (!std::isfinite(resistance)) throw std::runtime_error("stage resistance is outside the supported range");
        const long double feed_current = upstream_voltage / resistance;
        const long double node_voltage = upstream_voltage * (load_at[i] / resistance);
        const long double shunt_current = node_voltage / shunt;
        if (!std::isfinite(feed_current) || !std::isfinite(shunt_current) || !std::isfinite(node_voltage)) {
            throw std::runtime_error("calculation is outside the supported numeric range");
        }
        if (upstream_voltage != 0 && node_voltage == 0) {
            throw std::runtime_error("voltage underflows the supported numeric range");
        }
        // Add drops from the lower end; subtraction can erase tiny internal voltages.
        long double value = node_voltage;
        for (std::size_t j = stage.series.size(); j > 0; --j) {
            const Resistor& resistor = stage.series[j - 1];
            result.current[resistor.name] = feed_current;
            result.voltage[resistor.to] = value;
            value += feed_current * resistor.ohms;
        }
        value = 0;
        for (std::size_t j = stage.shunt.size(); j > 0; --j) {
            const Resistor& resistor = stage.shunt[j - 1];
            result.current[resistor.name] = shunt_current;
            result.voltage[resistor.to] = value;
            value += shunt_current * resistor.ohms;
        }
        upstream_voltage = node_voltage;
        // Short the ideal source, then look from each successive junction to ground.
        driving_resistance = parallel(shunt, series + driving_resistance);
    }
    result.vth = upstream_voltage;
    result.rth = driving_resistance;
    return result;
}

inline std::string engineering(long double value, const std::string& unit = "") {
    static const char* prefixes[] = {"f", "p", "n", "u", "m", "", "K", "Meg", "G", "T", "P"};
    if (value == 0) return "0.000" + unit;
    int index = 5;
    long double magnitude = std::fabs(value);
    while (magnitude < 1 && index > 0) { magnitude *= 1000; --index; }
    while (magnitude >= 1000 && index < 10) { magnitude /= 1000; ++index; }
    if (magnitude < 1 || magnitude >= 1000) {
        // Very deep ladders can attenuate below femto: preserve the nonzero value.
        std::ostringstream out;
        out << std::scientific << std::setprecision(3) << value << unit;
        return out.str();
    }
    if (std::floor(magnitude * 1000 + 0.5L) >= 1000000 && index < 10) {
        magnitude /= 1000;
        ++index;
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << (value < 0 ? -magnitude : magnitude)
        << prefixes[index] << unit;
    return out.str();
}

inline void row(std::ostream& out, const std::string& left, const std::string& right = "") {
    out << "** " << std::left << std::setw(13) << left << std::setw(20) << right << "**\n";
}

inline void calculation(const Circuit& circuit, std::ostream& out, bool all_nodes = false) {
    if (circuit.stages.empty()) { out << "There is no circuit\n"; return; }
    const Result result = solve(circuit);
    out << "**************************************\n";
    row(out, "<<voltage>>");
    row(out, "vdd", engineering(circuit.voltage, "v"));
    if (all_nodes) {
        for (std::map<std::string, long double>::const_iterator it = result.voltage.begin(); it != result.voltage.end(); ++it) {
            if (it->first != "vdd" && it->first != "gnd") row(out, it->first, engineering(it->second, "v"));
        }
    } else {
        for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
            row(out, circuit.stages[i].node, engineering(result.voltage.at(circuit.stages[i].node), "v"));
        }
    }
    row(out, "<<current>>");
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        const Stage& stage = circuit.stages[i];
        for (std::size_t j = 0; j < stage.series.size(); ++j) row(out, stage.series[j].name, engineering(result.current.at(stage.series[j].name), "A"));
        for (std::size_t j = 0; j < stage.shunt.size(); ++j) row(out, stage.shunt[j].name, engineering(result.current.at(stage.shunt[j].name), "A"));
    }
    row(out, "");
    row(out, "myspice2021");
    out << "**************************************\n";
}

inline void rewire(Circuit& circuit) {
    std::string upstream = "vdd";
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        circuit.stages[i].series[0].from = upstream;
        upstream = circuit.stages[i].node;
    }
}

inline void adjacent(const Circuit& circuit, int& first, int& second) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    if (first < 1 || second < 1 || static_cast<std::size_t>(first) > circuit.stages.size() ||
        static_cast<std::size_t>(second) > circuit.stages.size()) throw std::runtime_error("The node doesn't exist.");
    if (std::abs(first - second) != 1) throw std::runtime_error("The stages must be adjacent.");
    if (first > second) std::swap(first, second);
}

inline void swap_stage(Circuit& circuit, int first, int second, std::ostream* messages = NULL) {
    adjacent(circuit, first, second);
    std::vector<Resistor> before;
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) before.push_back(circuit.stages[i].series[0]);
    std::swap(circuit.stages[first - 1], circuit.stages[second - 1]);
    rewire(circuit);
    if (messages != NULL) {
        std::map<std::string, std::string> after;
        for (std::size_t i = 0; i < circuit.stages.size(); ++i) after[circuit.stages[i].series[0].name] = circuit.stages[i].series[0].from;
        for (std::size_t i = 0; i < before.size(); ++i) {
            if (before[i].from != after[before[i].name]) *messages << before[i].name << " is changing node from " << before[i].from << " to " << after[before[i].name] << " ...\n";
        }
        *messages << "The stage is successfully swapped!\n";
    }
}

inline void merge_stage(Circuit& circuit, int first, int second, std::ostream* messages = NULL) {
    adjacent(circuit, first, second);
    Stage& keep = circuit.stages[first - 1];
    const Stage& remove = circuit.stages[second - 1];
    const long double equivalent = parallel(total(keep.shunt), total(remove.series) + total(remove.shunt));
    std::ostringstream stem;
    stem << "RP" << first;
    std::string name = stem.str();
    std::set<std::string> remaining_names;
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        if (i == static_cast<std::size_t>(first - 1) || i == static_cast<std::size_t>(second - 1)) continue;
        for (std::size_t j = 0; j < circuit.stages[i].shunt.size(); ++j) remaining_names.insert(circuit.stages[i].shunt[j].name);
    }
    unsigned int suffix = 1;
    while (remaining_names.count(name) != 0) {
        std::ostringstream unique;
        unique << stem.str() << "_m" << suffix++;
        name = unique.str();
    }
    const Resistor merged = {name, keep.node, "gnd", equivalent};
    keep.shunt.assign(1, merged);
    circuit.stages.erase(circuit.stages.begin() + second - 1);
    rewire(circuit);
    if (messages != NULL) {
        *messages << "Resistors are merged into " << name << ".\n=> " << name << ' ' << merged.from
            << " gnd " << engineering(equivalent) << "\nThe stage is successfully merged!\n";
    }
}

inline void write_netlist(const Circuit& circuit, std::ostream& out) {
    out << "***circuits***\nstage " << circuit.stages.size() << "\n\n" << circuit.source_name
        << " vdd gnd " << std::fixed << std::setprecision(0) << circuit.voltage << "V\n\n";
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        const Stage& stage = circuit.stages[i];
        for (std::size_t k = 0; k < 2; ++k) {
            const std::vector<Resistor>& path = k == 0 ? stage.series : stage.shunt;
            for (std::size_t j = 0; j < path.size(); ++j) {
                out << path[j].name << ' ' << path[j].from << ' ' << path[j].to << ' ' << engineering(path[j].ohms) << '\n';
            }
        }
    }
    out << "end\n\n";
}

inline void raw_result(const Circuit& circuit, std::ostream& out) {
    const Result result = solve(circuit);
    out << std::scientific << std::setprecision(std::numeric_limits<long double>::max_digits10);
    out << "input_resistance " << result.input_resistance << "\nvth " << result.vth << "\nrth " << result.rth << '\n';
    for (std::map<std::string, long double>::const_iterator it = result.voltage.begin(); it != result.voltage.end(); ++it) out << "voltage " << it->first << ' ' << it->second << '\n';
    for (std::map<std::string, long double>::const_iterator it = result.current.begin(); it != result.current.end(); ++it) out << "current " << it->first << ' ' << it->second << '\n';
}

inline void thevenin(const Circuit& circuit, std::ostream& out, bool legacy, bool raw) {
    const Result result = solve(circuit);
    const long double voltage = legacy ? circuit.voltage : result.vth;
    const long double resistance = legacy ? result.input_resistance : result.rth;
    if (raw) {
        out << std::scientific << std::setprecision(std::numeric_limits<long double>::max_digits10)
            << "vth " << voltage << "\nrth " << resistance << '\n';
    } else {
        out << "***Thevenin equivalence circuit***\nRth = " << engineering(resistance)
            << "\nVth = " << engineering(voltage, "v") << '\n';
    }
}

inline void save(const Circuit& circuit, const std::string& filename, bool is_thevenin,
                 bool legacy, bool raw, bool all_nodes) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    // Validate all calculations before opening/truncating the requested output.
    solve(circuit);
    std::ofstream output(filename.c_str());
    if (!output) throw std::runtime_error("cannot open output file: " + filename);
    if (is_thevenin) thevenin(circuit, output, legacy, raw);
    else if (raw) raw_result(circuit, output);
    else {
        write_netlist(circuit, output);
        output << "***calculation results***\n";
        calculation(circuit, output, all_nodes);
    }
    if (!output) throw std::runtime_error("cannot write output file: " + filename);
}

inline void menu(std::ostream& out, bool bonus) {
    out << "==========================================\n"
        << "Please type 1 to input file\nPlease type 2 to calculate voltage and current\n"
        << "Please type 3 to swap stage\nPlease type 4 to merge stage\n"
        << "Please type 5 to output file\nPlease type 6 to exit\n";
    if (bonus) out << "Please type 7 to output Thevenin equivalence circuit\n";
    out << "Enter your selection Here: ";
}

inline bool prompt(std::istream& in, std::ostream& out, const std::string& text, std::string& reply) {
    out << text;
    if (!std::getline(in, reply)) return false;
    reply = trim(reply);
    return true;
}

inline bool stage_prompt(std::istream& in, std::ostream& out, const Circuit& circuit,
                         const std::string& verb, bool first, int other, int& answer) {
    std::string line;
    const std::string which = first ? "first" : "second";
    while (prompt(in, out, "Please enter the " + which + " stage you want to " + verb + ": ", line)) {
        try {
            answer = integer(line);
            if (answer < 1 || static_cast<std::size_t>(answer) > circuit.stages.size()) {
                out << "The node doesn't exist. Please enter again.\n";
            } else if (!first && std::abs(answer - other) != 1) {
                out << "The stages must be adjacent. Please enter again.\n";
            } else return true;
        } catch (const std::exception&) {
            out << "The node doesn't exist. Please enter again.\n";
        }
    }
    return false;
}

inline int interactive(bool bonus, bool legacy, std::istream& in, std::ostream& out) {
    Circuit circuit;
    std::string line;
    out << "*** Welcome to MySpice ***\n";
    for (;;) {
        menu(out, bonus);
        if (!std::getline(in, line)) return 0;
        int selection = 0;
        try { selection = integer(trim(line)); }
        catch (const std::exception&) { out << "Invalid selection.\n"; continue; }
        out << "==========================================\n";
        if (selection == 6) { out << "Bye~\n"; return 0; }
        if (selection == 1) {
            bool again = false;
            for (;;) {
                if (!prompt(in, out, again ? "Please enter the name of the input file again: " :
                    "Please enter the name of the input file: ", line)) return 0;
                std::ifstream file(line.c_str());
                if (!file) { out << "The file is not found.\n"; again = true; continue; }
                out << "Loading the file...\n";
                try {
                    Circuit loaded = read_circuit(file);
                    circuit = loaded;
                    out << "The input file successfully loaded!\n";
                    break;
                } catch (const std::exception& error) {
                    out << "Invalid circuit: " << error.what() << '\n';
                    again = true;
                }
            }
        } else if ((selection >= 2 && selection <= 5) || (bonus && selection == 7)) {
            if (circuit.stages.empty()) { out << "There is no circuit\n"; continue; }
            if (selection == 2) {
                out << "Calculating...\nThe result is successfully calculated!\n";
                calculation(circuit, out);
            } else if (selection == 3 || selection == 4) {
                if (circuit.stages.size() < 2) { out << "At least two stages are required.\n"; continue; }
                int first = 0, second = 0;
                const std::string verb = selection == 3 ? "swap" : "merge";
                if (!stage_prompt(in, out, circuit, verb, true, 0, first) ||
                    !stage_prompt(in, out, circuit, verb, false, first, second)) return 0;
                if (selection == 3) swap_stage(circuit, first, second, &out);
                else merge_stage(circuit, first, second, &out);
            } else {
                if (!prompt(in, out, "Please enter the name of the output file: ", line)) return 0;
                out << "Exporting the file...\n";
                try {
                    save(circuit, line, selection == 7, legacy, false, false);
                    out << (selection == 7 ? "The bonus file successfully exported!\n" : "The output file successfully exported!\n");
                } catch (const std::exception& error) { out << error.what() << '\n'; }
            }
        } else out << "Invalid selection.\n";
    }
}

inline int run(int argc, char* argv[], bool bonus) {
    try {
        if (argc == 1) return interactive(bonus, false, std::cin, std::cout);
        if (argc == 2 && bonus && std::string(argv[1]) == "--legacy-thevenin") {
            return interactive(bonus, true, std::cin, std::cout);
        }
        if (argc < 3) throw std::runtime_error("usage: program input.txt output.txt [--swap i j] [--merge i j] [--raw] [--all-nodes] [--circuit] [--thevenin] [--legacy-thevenin]");
        Circuit circuit = load(argv[1]);
        bool is_thevenin = bonus, legacy = false, raw = false, all_nodes = false;
        for (int i = 3; i < argc; ++i) {
            const std::string option(argv[i]);
            if (option == "--swap" || option == "--merge") {
                if (i + 2 >= argc) throw std::runtime_error("a stage operation requires two positions");
                const int first = integer(argv[++i]);
                const int second = integer(argv[++i]);
                if (option == "--swap") swap_stage(circuit, first, second);
                else merge_stage(circuit, first, second);
            } else if (option == "--raw") raw = true;
            else if (option == "--all-nodes") all_nodes = true;
            else if (option == "--circuit") is_thevenin = false;
            else if (option == "--thevenin" && bonus) is_thevenin = true;
            else if (option == "--legacy-thevenin" && bonus) { legacy = true; is_thevenin = true; }
            else throw std::runtime_error("unknown option: " + option);
        }
        save(circuit, argv[2], is_thevenin, legacy, raw, all_nodes);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

}  // namespace myspice
#endif
```

<!-- SOLUTION_CODE_END -->

## 範例

第一組重現公開 case 1 的交換與合併，第二、三組用同一電路比較物理 Thevenin 與舊表兼容；第四組展示中段合併，最後一組展示未載入電路時的互動防呆。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

執行參數：`circuit.txt result.txt --swap 1 2 --merge 1 2`

輸入檔 `circuit.txt`：
```text
stage 2

v1 vdd gnd 5V

RS1_1 vdd ns1_1 2K
RS1_2 ns1_1 n1 2K
RP1_1 n1 gnd 2K
RS2_1 n1 n2 2K
RP2_1 n2 np2_1 3K
RP2_2 np2_1 gnd 3K

end
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `result.txt`：
```text
***circuits***
stage 1

v1 vdd gnd 5V

RS2_1 vdd n2 2.000K
RP1 n2 gnd 3.000K
end

***calculation results***
**************************************
** <<voltage>>                      **
** vdd          5.000v              **
** n2           3.000v              **
** <<current>>                      **
** RS2_1        1.000mA             **
** RP1          1.000mA             **
**                                  **
** myspice2021                      **
**************************************
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`bonus.cpp`）

執行參數：`circuit.txt result.txt --swap 1 2 --merge 1 2`

輸入檔 `circuit.txt`：
```text
stage 2

v1 vdd gnd 5V

RS1_1 vdd ns1_1 2K
RS1_2 ns1_1 n1 2K
RP1_1 n1 gnd 2K
RS2_1 n1 n2 2K
RP2_1 n2 np2_1 3K
RP2_2 np2_1 gnd 3K

end
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `result.txt`：
```text
***Thevenin equivalence circuit***
Rth = 1.200K
Vth = 3.000v
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`bonus.cpp`）

執行參數：`circuit.txt result.txt --swap 1 2 --merge 1 2 --legacy-thevenin`

輸入檔 `circuit.txt`：
```text
stage 2

v1 vdd gnd 5V

RS1_1 vdd ns1_1 2K
RS1_2 ns1_1 n1 2K
RP1_1 n1 gnd 2K
RS2_1 n1 n2 2K
RP2_1 n2 np2_1 3K
RP2_2 np2_1 gnd 3K

end
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `result.txt`：
```text
***Thevenin equivalence circuit***
Rth = 5.000K
Vth = 5.000v
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

執行參數：`circuit.txt result.txt --merge 1 2`

輸入檔 `circuit.txt`：
```text
stage 3
v1 vdd gnd 6V
RS1_1 vdd n1 2K
RP1_1 n1 gnd 16K
RS2_1 n1 n2 8K
RP2_1 n2 gnd 8K
RS3_1 n2 n3 4K
RP3_1 n3 gnd 4K
end
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `result.txt`：
```text
***circuits***
stage 2

v1 vdd gnd 6V

RS1_1 vdd n1 2.000K
RP1 n1 gnd 8.000K
RS3_1 n1 n3 4.000K
RP3_1 n3 gnd 4.000K
end

***calculation results***
**************************************
** <<voltage>>                      **
** vdd          6.000v              **
** n1           4.000v              **
** n3           2.000v              **
** <<current>>                      **
** RS1_1        1.000mA             **
** RP1          500.000uA           **
** RS3_1        500.000uA           **
** RP3_1        500.000uA           **
**                                  **
** myspice2021                      **
**************************************
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 5（`main.cpp`）

標準輸入：
```text
2
6
```

標準輸出：
```text
*** Welcome to MySpice ***
==========================================
Please type 1 to input file
Please type 2 to calculate voltage and current
Please type 3 to swap stage
Please type 4 to merge stage
Please type 5 to output file
Please type 6 to exit
Enter your selection Here: ==========================================
There is no circuit
==========================================
Please type 1 to input file
Please type 2 to calculate voltage and current
Please type 3 to swap stage
Please type 4 to merge stage
Please type 5 to output file
Please type 6 to exit
Enter your selection Here: ==========================================
Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->
