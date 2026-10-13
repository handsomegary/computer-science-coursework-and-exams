# 2021 PA2：遞迴二進位累加與二補數

本題面依提供的作業題目與參考程式重整，採 2021 年最後修訂版與公開測資。下列採用規則是可執行的儲存庫規格，不宣稱為教授原文。

## 題目

輸入第一個十進位整數作為被加數，再輸入第二個及後續整數作為加數。每次加法都顯示二進位算式與十進位部分和；下一輪把前一次的部分和當作被加數。使用遞迴實作十進位轉二進位、二進位全加器與二進位轉十進位。數值加減須以位元規則與全加器完成，不能直接用十進位 `+` 或 `-` 算總和。

一般版只接受非負數，`-1` 在任何輸入位置結束。加分版接受正負數，以二補數加法完成帶符號累加，`0` 在任何輸入位置結束；所以加分版可以輸入 `-1`，也可以產生零部分和。

## 輸入、限制與位寬採用規則

輸入為空白分隔的整數 token，可有一個前導 `+`／`-` 和前導零；必須整個 token 都是合法十進位整數。先驗證 32 位有號整數範圍，輸入值存於 `int`；二進位數字容器存於 `long long int`。

一般版的輸入與部分和皆為 `0..524287`。因為二進位字串作為 0/1 十進位數字存放，19 個 `1` 是 `1111111111111111111`，可存入 `long long`；20 位首位 `1` 就超過上限。因此一般版雖然介面寫 32 位整數，實際有效範圍是 `2^19-1`。

加分版採公開測資所用的「絕對值位數再加一個符號位」：正數輸出 `0` 再接一般二進位；負數以 `bit_length(abs(n))+1` 位的二補數輸出。因此 `8` 是 `01000`、`-19` 是 `101101`、`-1` 是 `11`、`-4` 是 `1100`。零部分和顯示 `00`。負數輸入與部分和下限是 `-262143`，正數上限仍是 `524287`；正數的前導零不改變數字容器的儲存值，負數的首位 `1` 則會占位。

文字敘述的「負數最少位數」與公開測資的 `-1=11`、`-4=1100` 有矛盾：數學上最少可為 `1` 和 `100`。本版選擇與公開測資相符的上述表示法。另一份測資把 `-451231` 的失敗稱為部分和溢位，但該輸入自身已需 20 個非零前導二補數位；本版先拒絕該輸入，再檢查部分和。正式範例中前一部分和 `77` 的二進位誤寫成 `01010001`（81），本版顯示正確 `01001101`。

無全域或 static 變數，不使用字串／陣列／`bool` 本地變數。所有變數與參數為 `int` 或 `long long int`。`++`、`--` 只用於位數與輸入序號，乘除只用於數位欄位轉換；二進位相加用 XOR/AND/OR 與遞迴。單目負號只表示讀入或解碼的符號，不用于算兩數之差。

## 輸出與錯誤恢復

一般版啟動顯示 `*** Welcome to Positive Binary adder ***`，加分版顯示 `*** Welcome to Binary Adder and Subtractor ***`。第 N 個輸入提示為 `Please enter the Nth number in decimal: `，其中序數正確使用 `1st`、`2nd`、`3rd`、`11th`、`12th`、`13th`、`21st` 等。提示不換行。

每次有效運算顯示 `Calculating... A + B = C` 及 `Partial Sum: D`，再空一行。A 是之前部分和、B 是這次加數、C 是結果的二進位表示，D 是十進位結果。

格式錯誤、32 位越界或二進位容器不能存放時，顯示 `The decimal number you entered is out of range. Please enter the Nth decimal number again: `。若加數有效但累加後結果超出本版範圍，顯示 `The partial sum is out of range. Please enter the Nth decimal number again: `。錯誤不改部分和、不增加輸入序號，重讀同一個位置。任何重讀位置也先識別結束值，顯示 `Bye` 換行後結束。EOF 直接結束，不臆造輸入或結束值。

## 解法與報告

十進位轉換遞迴處理 `n/2`，把上一層數字乘十後接上最低位；由於乘十結果的最低 bit 為零，可以用 OR 接上 0/1。反向轉換以 `value/10` 遞迴，每層乘二再以 OR 接上數字。

全加器從個位開始，`sum=a^b^carry`，`nextCarry=(a&b)|(a&carry)|(b&carry)`。高位遞迴完成後乘十、接上當前位。固定處理 19 個儲存位；一般版以最後 carry 判斷溢位。加分版另以 `int` 保存符號位：負數的低 19 位先反相加一，正數符號零，兩符號位與最後 carry 再執行一次全加器。因為前導零無須占數字容器，正數仍可保存 19 位有效數字。結果依符號解碼，並檢查輸出位寬是否可儲存。

核心全加器片段（本儲存庫修訂解法）：

```cpp
int digit = left ^ right ^ carry;
int nextCarry = (left & right) | (left & carry) | (right & carry);
--width;
long long int higher = binaryAdder(a / 10, b / 10, nextCarry,
                                   width, finalCarry);
return (higher * 10) | digit;
```

三個數值轉換／加法遞迴深度均不超過 19，時間與呼叫堆疊為 `O(位數)`。符號位檢查避免把丟掉的進位當成合法結果。整個 token 逐字元以 `int` 讀取；巨長錯誤 token 不會塞入數值變數，並會完整消耗後再重問。ASCII 字元的低四位取得數字值，位元進位函式建立十進位輸入值，沒有用十進位加法算部分和。

獨立 Python 整數模型驗證五組基本公開測資、加分正負交換、相消結果零、兩側有效邊界、輸入／部分和溢位、不完整 token、巨長 token 和 EOF，以及每個輸入位置的 sentinel。固定種子隨機序列逐次驗證部分和與完整提示；序數測試至少到第 24 個輸入。

## 講義可確認與本儲存庫補定

講義可確認：三個函式必須遞迴；禁止全域、static 與其他本地資料型別；輸入 `int`、二進位數字 `long long int`；二進位計算；逐次部分和；一般版 `-1`、加分版 `0` 結束；錯誤重讀同一位置；二補數、符號延伸與正數前導零。

本儲存庫補定：整 token 驗證和 EOF 行為、零部分和 `00`、負數的公開測資位寬優先規則、明確推導有效上下限、修正錯誤範例數值及錯誤類別。所有計算使用算術正確的算式，不重現原文件的筆誤。

## 範例

第一組示範多次累加；第二組到達一般版上限後拒絕溢位，輸入零仍能保留上限；第三組確認非法 token 可恢復且重讀時可結束。加分第一組的 `8 + (-19) = -11` 再加 `-11`；另一組示範負的二次方位寬、相消為零與 `0` 結束。

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

標準輸入：
```text
100
200
300
400
500
600
700
800
900
-1
```

標準輸出：
```text
*** Welcome to Positive Binary adder ***
Please enter the 1st number in decimal: Please enter the 2nd number in decimal: Calculating... 1100100 + 11001000 = 100101100
Partial Sum: 300

Please enter the 3rd number in decimal: Calculating... 100101100 + 100101100 = 1001011000
Partial Sum: 600

Please enter the 4th number in decimal: Calculating... 1001011000 + 110010000 = 1111101000
Partial Sum: 1000

Please enter the 5th number in decimal: Calculating... 1111101000 + 111110100 = 10111011100
Partial Sum: 1500

Please enter the 6th number in decimal: Calculating... 10111011100 + 1001011000 = 100000110100
Partial Sum: 2100

Please enter the 7th number in decimal: Calculating... 100000110100 + 1010111100 = 101011110000
Partial Sum: 2800

Please enter the 8th number in decimal: Calculating... 101011110000 + 1100100000 = 111000010000
Partial Sum: 3600

Please enter the 9th number in decimal: Calculating... 111000010000 + 1110000100 = 1000110010100
Partial Sum: 4500

Please enter the 10th number in decimal: Bye
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`main.cpp`）

標準輸入：
```text
123
321
456
654
-456
-456
12321456
4522733
522733
78987
0
-1
```

標準輸出：
```text
*** Welcome to Positive Binary adder ***
Please enter the 1st number in decimal: Please enter the 2nd number in decimal: Calculating... 1111011 + 101000001 = 110111100
Partial Sum: 444

Please enter the 3rd number in decimal: Calculating... 110111100 + 111001000 = 1110000100
Partial Sum: 900

Please enter the 4th number in decimal: Calculating... 1110000100 + 1010001110 = 11000010010
Partial Sum: 1554

Please enter the 5th number in decimal: The decimal number you entered is out of range. Please enter the 5th decimal number again: The decimal number you entered is out of range. Please enter the 5th decimal number again: The decimal number you entered is out of range. Please enter the 5th decimal number again: The decimal number you entered is out of range. Please enter the 5th decimal number again: Calculating... 11000010010 + 1111111100111101101 = 1111111111111111111
Partial Sum: 524287

Please enter the 6th number in decimal: The partial sum is out of range. Please enter the 6th decimal number again: Calculating... 1111111111111111111 + 0 = 1111111111111111111
Partial Sum: 524287

Please enter the 7th number in decimal: Bye
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 3（`main.cpp`）

標準輸入：
```text
--1
1x
+
-2147483648
+5
6
-1
```

標準輸出：
```text
*** Welcome to Positive Binary adder ***
Please enter the 1st number in decimal: The decimal number you entered is out of range. Please enter the 1st decimal number again: The decimal number you entered is out of range. Please enter the 1st decimal number again: The decimal number you entered is out of range. Please enter the 1st decimal number again: The decimal number you entered is out of range. Please enter the 1st decimal number again: Please enter the 2nd number in decimal: Calculating... 101 + 110 = 1011
Partial Sum: 11

Please enter the 3rd number in decimal: Bye
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 4（`bonus.cpp`）

標準輸入：
```text
8
-19
-11
0
```

標準輸出：
```text
*** Welcome to Binary Adder and Subtractor ***
Please enter the 1st number in decimal: Please enter the 2nd number in decimal: Calculating... 01000 + 101101 = 10101
Partial Sum: -11

Please enter the 3rd number in decimal: Calculating... 10101 + 10101 = 101010
Partial Sum: -22

Please enter the 4th number in decimal: Bye
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 5（`bonus.cpp`）

標準輸入：
```text
1
-1
-1
-1
-2
-4
0
```

標準輸出：
```text
*** Welcome to Binary Adder and Subtractor ***
Please enter the 1st number in decimal: Please enter the 2nd number in decimal: Calculating... 01 + 11 = 00
Partial Sum: 0

Please enter the 3rd number in decimal: Calculating... 00 + 11 = 11
Partial Sum: -1

Please enter the 4th number in decimal: Calculating... 11 + 11 = 110
Partial Sum: -2

Please enter the 5th number in decimal: Calculating... 110 + 110 = 1100
Partial Sum: -4

Please enter the 6th number in decimal: Calculating... 1100 + 1100 = 11000
Partial Sum: -8

Please enter the 7th number in decimal: Bye
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `binary.hpp`

```cpp
#ifndef BINARY_HPP
#define BINARY_HPP

#include <iostream>

// All variables and parameters are int / long long int. There are no arrays,
// strings, static variables or global variables. Decimal digit containers are
// long long int; their digits themselves are binary 0/1 digits.
inline long long int bitwiseAdd(long long int a, long long int b) {
    if (b == 0) return a;
    return bitwiseAdd(a ^ b, (a & b) << 1);
}

inline long long int decimalToBinary(int value) {
    if (value == 0) return 0;
    return (decimalToBinary(value / 2) * 10) | (value % 2);
}

inline int binaryToDecimal(long long int value) {
    if (value == 0) return 0;
    return (binaryToDecimal(value / 10) * 2) |
           static_cast<int>(value % 10);
}

inline long long int binaryAdder(long long int a, long long int b,
                                 int carry, int width, int& finalCarry) {
    if (width == 0) {
        finalCarry = carry;
        return 0;
    }
    int left = static_cast<int>(a % 10);
    int right = static_cast<int>(b % 10);
    int digit = left ^ right ^ carry;
    int nextCarry = (left & right) | (left & carry) | (right & carry);
    --width;
    long long int higher = binaryAdder(a / 10, b / 10, nextCarry,
                                       width, finalCarry);
    return (higher * 10) | digit;
}

inline long long int inverted(long long int number, int width) {
    if (width == 0) return 0;
    int digit = static_cast<int>(number % 10) ^ 1;
    --width;
    return (inverted(number / 10, width) * 10) | digit;
}

inline long long int encodedSigned(int number, int width) {
    if (number >= 0) return decimalToBinary(number);
    int magnitude = -number; // Unary sign only; subtraction uses the full adder.
    int carry = 0;
    return binaryAdder(inverted(decimalToBinary(magnitude), width), 1,
                       0, width, carry);
}

inline int decodedSigned(long long int packed, int sign) {
    if (sign == 0) return binaryToDecimal(packed);
    int carry = 0;
    long long int magnitude = binaryAdder(inverted(packed, 19), 1, 0,
                                          19, carry);
    // A separate sign bit allows a leading zero on positive 19-digit values.
    // The low 19 bits of -2^19 are zero; do not mistake that value for zero.
    if (carry) return -524288;
    return -binaryToDecimal(magnitude);
}

inline int magnitudeBits(int value) {
    if (value == 0) return 0;
    int width = magnitudeBits(value / 2);
    return ++width;
}

inline void showSignedBinary(int number) {
    if (number >= 0) {
        std::cout << '0' << decimalToBinary(number);
    } else {
        int width = magnitudeBits(-number);
        ++width;
        std::cout << encodedSigned(number, width);
    }
}

inline int whitespace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
}

// Consume exactly one whitespace-delimited token without storing characters.
// Return 1 for a signed 32-bit integer, -1 for a bad token, 0 for clean EOF.
inline int readDecimal(int& value) {
    int c = std::cin.get();
    while (c != -1 && whitespace(c)) c = std::cin.get();
    if (c == -1) return 0;
    int negative = 0;
    if (c == '-' || c == '+') {
        negative = c == '-';
        c = std::cin.get();
    }
    long long int magnitude = 0;
    int digits = 0;
    int valid = 1;
    while (c != -1 && !whitespace(c)) {
        if (c < '0' || c > '9') {
            valid = 0;
        } else {
            ++digits;
            // ASCII decimal digits have their numeric value in the low nibble.
            int digit = c & 15;
            if (magnitude > 214748364LL ||
                (magnitude == 214748364LL && digit > (negative ? 8 : 7))) {
                valid = 0;
            } else if (valid) {
                magnitude = bitwiseAdd(magnitude * 10, digit);
            }
        }
        c = std::cin.get();
    }
    if (!valid || digits == 0) return -1;
    value = static_cast<int>(negative ? -magnitude : magnitude);
    return 1;
}

inline void ordinal(long long int index) {
    std::cout << index;
    long long int lastTwo = index % 100;
    if (lastTwo >= 11 && lastTwo <= 13) std::cout << "th";
    else if (index % 10 == 1) std::cout << "st";
    else if (index % 10 == 2) std::cout << "nd";
    else if (index % 10 == 3) std::cout << "rd";
    else std::cout << "th";
}

inline void prompt(long long int index, int error) {
    if (error == 1)
        std::cout << "The decimal number you entered is out of range. ";
    if (error == 2)
        std::cout << "The partial sum is out of range. ";
    std::cout << "Please enter the ";
    ordinal(index);
    if (error) std::cout << " decimal number again: ";
    else std::cout << " number in decimal: ";
}

inline int runBinaryAdder(int bonus) {
    if (bonus)
        std::cout << "*** Welcome to Binary Adder and Subtractor ***\n";
    else
        std::cout << "*** Welcome to Positive Binary adder ***\n";
    long long int index = 1;
    long long int partial = 0;
    int partialDecimal = 0;
    int error = 0;
    while (1) {
        prompt(index, error);
        int number = 0;
        int status = readDecimal(number);
        if (status == 0) return 0;
        if (status == 1 && number == (bonus ? 0 : -1)) {
            std::cout << "Bye\n";
            return 0;
        }
        if (status != 1 || (bonus && (number < -262143 || number > 524287)) ||
            (!bonus && (number < 0 || number > 524287))) {
            error = 1;
            continue;
        }
        long long int addend = bonus ? encodedSigned(number, 19)
                                     : decimalToBinary(number);
        if (index == 1) {
            partial = addend;
            partialDecimal = number;
            ++index;
            error = 0;
            continue;
        }
        int carry = 0;
        long long int next = binaryAdder(partial, addend, 0, 19, carry);
        int nextSign = (partialDecimal < 0) ^ (number < 0) ^ carry;
        int nextDecimal = bonus ? decodedSigned(next, nextSign) : binaryToDecimal(next);
        int overflow = bonus
            ? ((partialDecimal < 0 && number < 0 && nextDecimal >= 0) ||
               (partialDecimal >= 0 && number >= 0 && nextDecimal < 0) ||
               nextDecimal < -262143 || nextDecimal > 524287)
            : carry;
        if (overflow) {
            error = 2;
            continue;
        }
        std::cout << "Calculating... ";
        if (bonus) showSignedBinary(partialDecimal);
        else std::cout << partial;
        std::cout << " + ";
        if (bonus) showSignedBinary(number);
        else std::cout << addend;
        std::cout << " = ";
        if (bonus) showSignedBinary(nextDecimal);
        else std::cout << next;
        std::cout << "\nPartial Sum: " << nextDecimal << "\n\n";
        partial = next;
        partialDecimal = nextDecimal;
        ++index;
        error = 0;
    }
}

#endif
```

### `bonus.cpp`

```cpp
#include "binary.hpp"

int main() {
    return runBinaryAdder(1);
}
```

### `main.cpp`

```cpp
#include "binary.hpp"

int main() {
    return runBinaryAdder(0);
}
```

<!-- SOLUTION_CODE_END -->
