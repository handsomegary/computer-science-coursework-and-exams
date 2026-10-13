# 2021 PA1：五倍券付款與最少損失加分版

本題面依提供的作業題目與參考程式重新整理，以下補定處是本儲存庫的明確規格，不宣稱為教授原文。採用最後修訂 V2、五份公開測資及零元付款案例。

## 題目

起始持有 100、50、10、5、1 元券各五張，總值 830 元。使用者反覆輸入商品原價與付款種類：紙券打九折，數位券打八折；數位券且為銀行會員時，再打九折，合計為原價的 72%。折扣計算完成後無條件去掉小數。

沒有現金，也不會收到找零。折扣後價格超過所有剩券的總值時，拒絕該次交易且庫存不變。成功付款後顯示已使用與剩下的券。每次交易可重新選擇券種和會員資格；兩種類型共用同一份券庫存。

一般版按下列確定規則付款：依面額由大到小，在目前面額先使用不超過尚需金額的最多張數。若還有目前面額的券，而且剩餘金額無法用較小面額的現有券剛好付清，就多付一張目前面額的券並結束付款。因此只剩一張 100、一張 50 和兩張 5 元券時，39 元會付一張 100 元券；剩一張 100、兩張 50 和兩張 5 元券時，156 元會付 200 元。

加分版選擇足以支付的組合，以「實付總值最小、張數最少、較大面額的張數依序較多」決定唯一答案。它保留一般版的折扣、無找零、庫存與提示流程，只改變付款選擇。

## 輸入與限制

輸入為空白分隔的 token。每次依序讀取：

1. 商品原價：`0..2147483647` 的十進位整數，或精確的 `-1` 結束。
2. 券種：`d` 或 `p`。
3. 若券種是 `d`，再讀會員狀態 `y` 或 `n`。

原價接受前導零與前導 `+`；不接受其他負數、分數或夾雜字母。券種與會員狀態必須是上述小寫單字元。券用盡立即結束，不讀取後續輸入。EOF 直接結束，已印出的提示保留。

每種券的張數始終為 `0..5`。折扣乘法用 `long long` 計算，最大中間值 `2147483647*90` 不會溢位；付款只在折扣價不超過剩券價值時執行，因此付款金額上限為 830。

## 輸出

啟動顯示 `*** Welcome to Quintuple Stimulus Vouchers System ***`。價格、種類與會員提示依範例顯示，提示本身不換行。折扣價顯示為 `The price you need to pay: N`。

非零成功付款顯示 `The payment is processed as follows:`，每個用到的面額一行，由大至小。剩券顯示 `The remaining vouchers are as follows:`，省略零張的面額。每行採四格縮排、張數、` $`、寬度三的右對齊面額、` voucher`。剩券表後空一行。

不足額顯示 `You don't have enough voucher to buy the good`，再空一行。輸入 `-1` 顯示 `Bye~`。券用盡則依序顯示 `You do not remain any voucher` 與 `Bye~`，各自一行；不印空的剩券表。零元付款不印付款標題或用券明細，直接顯示原來的剩券表，符合補充案例。

非法價格顯示 `Invalid good price. Please try again.` 並重問價格。非法券種／會員狀態分別顯示 `Invalid voucher type. Please try again.`／`Invalid member status. Please try again.`，僅重問該欄位。

## 解法與報告

折扣使用整數比例 `price*rate/100`，率為 90、80 或 72，避免浮點值在截尾邊界少一元。總剩值由券庫存重新計算，交易成功才一次扣除所選組合。

一般版用 `exactFrom` 檢查較小面額能否剛好支付。這組面額的每個大面額都是下一個面額的整數倍，因此大至小取不超過目標的最多張數可以判定可否剛好付款。當它不可行而目前面額尚有剩券，就加付一張。每次交易時間與額外空間皆為固定面額數的量級。

加分版枚舉每種券的 `0..現有張數`，最多 `6^5=7776` 個組合，根據三個排序條件選最優者；沒有預存答案表。遞迴深度五，時間 `O(∏(張數+1))`、額外空間 `O(5)`。

核心決策片段（本儲存庫修訂解法）：

```cpp
used[i] = std::min(wallet[i], need / value[i]);
const int rest = need - used[i] * value[i];
if (used[i] < wallet[i] && !exactFrom(i + 1, rest, wallet, value)) {
    ++used[i];
}
need -= used[i] * value[i];
```

驗證包含公開五組交易、免費消費、整數上限、不足額、用盡、錯誤恢復，以及固定種子的連續交易。一般版的獨立模型枚舉較小券的所有可達金額；加分版模型枚舉所有合法組合，與 C++ 的完整輸出逐字比較。沒有執行原始 EXE。

## 講義規則與補定規則

講義可確認：五種面額各五張；三種折扣及截尾；大面額優先；無找零；不足額不交易；每次重選券種；`-1` 和券用盡結束；零元付款案例不印付款表；加分可做防呆與最少損失。

本儲存庫補定：允許原價零、前導零和 `+`；整個 token 驗證；非法輸入恢復文字；EOF 行為；bonus 的張數及大面額平手規則。公開範例各行縮排不一致，本版統一四格及面額寬度三；英文撇號使用 ASCII `'`，不保留 PDF 的彎引號或行末空白。

## 範例

第一組包含折扣、拒絕交易及大面額超付；第二組會消耗最後兩張一元券。第三組確認免費消費不扣券。最後兩組以同樣輸入比較一般版支付 300 元與加分版支付 295 元的差異。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

標準輸入：
```text
255
d
y
766
p
640
p
40
d
n
-1
```

標準輸出：
```text
*** Welcome to Quintuple Stimulus Vouchers System ***
Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 183
The payment is processed as follows:
    1 $100 voucher
    1 $ 50 voucher
    3 $ 10 voucher
    3 $  1 voucher
The remaining vouchers are as follows:
    4 $100 voucher
    4 $ 50 voucher
    2 $ 10 voucher
    5 $  5 voucher
    2 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 689
You don't have enough voucher to buy the good

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 576
The payment is processed as follows:
    4 $100 voucher
    3 $ 50 voucher
    2 $ 10 voucher
    1 $  5 voucher
    1 $  1 voucher
The remaining vouchers are as follows:
    1 $ 50 voucher
    4 $  5 voucher
    1 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 32
The payment is processed as follows:
    1 $ 50 voucher
The remaining vouchers are as follows:
    4 $  5 voucher
    1 $  1 voucher

Please enter the good price: Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

標準輸入：
```text
1000
p
789
d
y
766
p
289
p
3
d
n
```

標準輸出：
```text
*** Welcome to Quintuple Stimulus Vouchers System ***
Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 900
You don't have enough voucher to buy the good

Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 568
The payment is processed as follows:
    5 $100 voucher
    1 $ 50 voucher
    1 $ 10 voucher
    1 $  5 voucher
    3 $  1 voucher
The remaining vouchers are as follows:
    4 $ 50 voucher
    4 $ 10 voucher
    4 $  5 voucher
    2 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 689
You don't have enough voucher to buy the good

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 260
The payment is processed as follows:
    4 $ 50 voucher
    4 $ 10 voucher
    4 $  5 voucher
The remaining vouchers are as follows:
    2 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 2
The payment is processed as follows:
    2 $  1 voucher
You do not remain any voucher
Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

標準輸入：
```text
1
d
y
0
p
-1
```

標準輸出：
```text
*** Welcome to Quintuple Stimulus Vouchers System ***
Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 0
The remaining vouchers are as follows:
    5 $100 voucher
    5 $ 50 voucher
    5 $ 10 voucher
    5 $  5 voucher
    5 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 0
The remaining vouchers are as follows:
    5 $100 voucher
    5 $ 50 voucher
    5 $ 10 voucher
    5 $  5 voucher
    5 $  1 voucher

Please enter the good price: Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`main.cpp`）

標準輸入：
```text
399
d
n
408
d
y
999
p
-1
```

標準輸出：
```text
*** Welcome to Quintuple Stimulus Vouchers System ***
Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 319
The payment is processed as follows:
    3 $100 voucher
    1 $ 10 voucher
    1 $  5 voucher
    4 $  1 voucher
The remaining vouchers are as follows:
    2 $100 voucher
    5 $ 50 voucher
    4 $ 10 voucher
    4 $  5 voucher
    1 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 293
The payment is processed as follows:
    2 $100 voucher
    2 $ 50 voucher
The remaining vouchers are as follows:
    3 $ 50 voucher
    4 $ 10 voucher
    4 $  5 voucher
    1 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): The price you need to pay: 899
You don't have enough voucher to buy the good

Please enter the good price: Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 5（`bonus.cpp`）

標準輸入：
```text
399
d
n
408
d
y
-1
```

標準輸出：
```text
*** Welcome to Quintuple Stimulus Vouchers System ***
Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 319
The payment is processed as follows:
    3 $100 voucher
    1 $ 10 voucher
    1 $  5 voucher
    4 $  1 voucher
The remaining vouchers are as follows:
    2 $100 voucher
    5 $ 50 voucher
    4 $ 10 voucher
    4 $  5 voucher
    1 $  1 voucher

Please enter the good price: Please enter the type of vouchers (d/p): Are you a member of NCU Bank (y/n): The price you need to pay: 293
The payment is processed as follows:
    2 $100 voucher
    1 $ 50 voucher
    4 $ 10 voucher
    1 $  5 voucher
The remaining vouchers are as follows:
    4 $ 50 voucher
    3 $  5 voucher
    1 $  1 voucher

Please enter the good price: Bye~
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整程式碼

一般版與加分版入口分開；共用標頭仍是兩份完整解法的一部分。

<!-- SOLUTION_CODE_BEGIN -->

### `bonus.cpp`

```cpp
#include "vouchers.hpp"

int main() {
    return runVouchers(true);
}
```

### `main.cpp`

```cpp
#include "vouchers.hpp"

int main() {
    return runVouchers(false);
}
```

### `vouchers.hpp`

```cpp
#ifndef VOUCHERS_HPP
#define VOUCHERS_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <iostream>
#include <string>

// Validate a whole token before accumulating: even a very long invalid token
// cannot overflow an intermediate integer or corrupt the next input.
inline bool parsePrice(const std::string& token, long long& value) {
    if (token == "-1") {
        value = -1;
        return true;
    }
    std::size_t i = 0;
    if (!token.empty() && token[0] == '+') ++i;
    if (i == token.size()) return false;
    value = 0;
    for (; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') return false;
        const int digit = token[i] - '0';
        if (value > (2147483647LL - digit) / 10) return false;
        value = value * 10 + digit;
    }
    return true;
}

inline int walletValue(const std::array<int, 5>& wallet,
                       const std::array<int, 5>& value) {
    int total = 0;
    for (int i = 0; i < 5; ++i) total += wallet[i] * value[i];
    return total;
}

inline bool exactFrom(int first, int need, const std::array<int, 5>& wallet,
                      const std::array<int, 5>& value) {
    // Every denomination is a multiple of the next one, so a maximal greedy
    // selection can decide whether the smaller available vouchers pay exactly.
    for (int i = first; i < 5; ++i) {
        const int count = std::min(wallet[i], need / value[i]);
        need -= count * value[i];
    }
    return need == 0;
}

inline std::array<int, 5> originalPayment(
    int need, const std::array<int, 5>& wallet,
    const std::array<int, 5>& value) {
    std::array<int, 5> used = {{0, 0, 0, 0, 0}};
    for (int i = 0; i < 5 && need > 0; ++i) {
        used[i] = std::min(wallet[i], need / value[i]);
        const int rest = need - used[i] * value[i];
        // If exact smaller payment is impossible, the current larger voucher
        // must be preferred even when smaller vouchers would lose less money.
        if (used[i] < wallet[i] && !exactFrom(i + 1, rest, wallet, value)) {
            ++used[i];
        }
        need -= used[i] * value[i];
    }
    return used;
}

inline void searchPayment(int index, int paid, int count, int need,
                          const std::array<int, 5>& wallet,
                          const std::array<int, 5>& value,
                          std::array<int, 5>& candidate,
                          std::array<int, 5>& best,
                          int& bestPaid, int& bestCount) {
    if (index == 5) {
        if (paid < need) return;
        if (paid < bestPaid || (paid == bestPaid && count < bestCount) ||
            (paid == bestPaid && count == bestCount && candidate > best)) {
            bestPaid = paid;
            bestCount = count;
            best = candidate;
        }
        return;
    }
    for (int n = 0; n <= wallet[index]; ++n) {
        candidate[index] = n;
        searchPayment(index + 1, paid + n * value[index], count + n, need,
                      wallet, value, candidate, best, bestPaid, bestCount);
    }
}

inline std::array<int, 5> minimumLossPayment(
    int need, const std::array<int, 5>& wallet,
    const std::array<int, 5>& value) {
    std::array<int, 5> candidate = {{0, 0, 0, 0, 0}};
    std::array<int, 5> best = candidate;
    int bestPaid = 831;
    int bestCount = 26;
    searchPayment(0, 0, 0, need, wallet, value, candidate, best,
                  bestPaid, bestCount);
    return best;
}

inline void showVouchers(const std::array<int, 5>& counts,
                         const std::array<int, 5>& value) {
    for (int i = 0; i < 5; ++i) {
        if (counts[i] != 0) {
            std::cout << "    " << counts[i] << " $" << std::setw(3)
                      << value[i] << " voucher\n";
        }
    }
}

inline int runVouchers(bool minimizeLoss) {
    const std::array<int, 5> value = {{100, 50, 10, 5, 1}};
    std::array<int, 5> wallet = {{5, 5, 5, 5, 5}};
    std::cout << "*** Welcome to Quintuple Stimulus Vouchers System ***\n";
    while (walletValue(wallet, value) != 0) {
        std::cout << "Please enter the good price: ";
        std::string token;
        if (!(std::cin >> token)) return 0;
        long long price;
        if (!parsePrice(token, price)) {
            std::cout << "Invalid good price. Please try again.\n";
            continue;
        }
        if (price == -1) {
            std::cout << "Bye~\n";
            return 0;
        }
        std::string type;
        do {
            std::cout << "Please enter the type of vouchers (d/p): ";
            if (!(std::cin >> type)) return 0;
            if (type != "d" && type != "p")
                std::cout << "Invalid voucher type. Please try again.\n";
        } while (type != "d" && type != "p");
        int rate = 90;
        if (type == "d") {
            std::string member;
            do {
                std::cout << "Are you a member of NCU Bank (y/n): ";
                if (!(std::cin >> member)) return 0;
                if (member != "y" && member != "n")
                    std::cout << "Invalid member status. Please try again.\n";
            } while (member != "y" && member != "n");
            rate = member == "y" ? 72 : 80;
        }
        const long long discounted = price * rate / 100;
        std::cout << "The price you need to pay: " << discounted << '\n';
        if (discounted > walletValue(wallet, value)) {
            std::cout << "You don't have enough voucher to buy the good\n\n";
            continue;
        }
        if (discounted != 0) {
            const std::array<int, 5> used = minimizeLoss
                ? minimumLossPayment(static_cast<int>(discounted), wallet, value)
                : originalPayment(static_cast<int>(discounted), wallet, value);
            std::cout << "The payment is processed as follows:\n";
            showVouchers(used, value);
            for (int i = 0; i < 5; ++i) wallet[i] -= used[i];
        }
        if (walletValue(wallet, value) == 0) {
            std::cout << "You do not remain any voucher\nBye~\n";
            return 0;
        }
        std::cout << "The remaining vouchers are as follows:\n";
        showVouchers(wallet, value);
        std::cout << '\n';
    }
    return 0;
}

#endif
```

<!-- SOLUTION_CODE_END -->
