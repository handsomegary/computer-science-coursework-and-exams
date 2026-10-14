# PA3：大整數計算機（2021）

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/pa03 --all --output-dir build/examples/2021-pa03
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

依提供的作業題目與參考程式整理，修正輸入失敗、索引越界及大數運算問題。一般版與 bonus 版分別提供獨立程式入口，共用 `bignum.hpp` 的十進位字串運算。

## 題目規則

- 一般版選單：`1` 加法、`2` 減法、`3` 乘法、`0` 結束；bonus 版增加 `4` 除法。
- 每次選擇運算後輸入兩個正整數，依序視為 `a`、`b`。每個輸入最多 100 個十進位數字，所有數字以字串或陣列儲存。
- 顯示選定運算的精確整數結果，以及 `floor((a+b)/2)`。平均值捨去小數部分。
- 減法是 `a-b`，允許負結果；相等時顯示 `0`。加法可能有 101 位，乘法可能有 200 位。
- bonus 除法顯示整數商與餘數，滿足 `a = b × quotient + remainder` 且 `0 ≤ remainder < b`；仍輸出兩數的平均值。
- 每次完成計算或發生輸入錯誤後，空一行並重新顯示完整選單。選單輸入 `0` 顯示 `Good bye!`，立即結束。

## 防呆與未指定處的處理

- 運算數只接受數字 `0`–`9` 組成的正整數字串；拒絕零、負號、加號、小數、非數字及超過 100 位的輸入。
- 接受前導零，但 100 位限制以原始數字字串長度計算；輸出移除前導零。
- 選單接受一般整數寫法，例如 `+1`、`001`；只接受該版本提供的選項，拒絕非整數與任意長度的越界值。
- 錯誤統一顯示 `Error! Please try again.`，回到完整選單；運算輸入錯誤時會先讀完該次的兩個輸入值，不輸出結果或平均值。
- 輸入依空白分隔，可以分行或放在同一行。遇到 EOF 直接結束，不補造輸入，也不顯示由 `0` 觸發的告別訊息。
- 減法標題依公開測資保留 `Subtraction` 後的空格：`----Now for Subtraction ----`。一般版的提示與結果格式依題目及公開測資。
- bonus 未提供輸出範例，因此沿用既有提示格式，使用 `Result:` 表示商、`Remainder:` 表示餘數。一般版也共用上述防呆。

## 編譯與執行

在本資料夾執行：

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o pa03_main
g++ -std=c++11 -Wall -Wextra -Wpedantic bonus.cpp -o pa03_bonus
./pa03_main
./pa03_bonus
```

Windows 編譯後可執行 `pa03_main.exe` 或 `pa03_bonus.exe`。編譯時保留同資料夾的 `bignum.hpp`。若課程平台要求只提交一個 `.cpp`，可把標頭內容合併到對應入口檔，保留該版的一個 `main`；一般版與 bonus 版仍分別提交。

## 檔案

- [main.cpp](main.cpp)：一般版入口。
- [bonus.cpp](bonus.cpp)：bonus 版入口。
- [bignum.hpp](bignum.hpp)：輸入驗證、字串大整數核心及互動流程。
- [solution.md](solution.md)：題面、解法、報告與完整程式碼，包含四組實跑範例。
- [samples.json](samples.json)：逐例記錄輸入、標準輸出、標準錯誤輸出、結束碼及該入口所需來源檔。

程式只讀取標準輸入、寫入標準輸出，不建立運算結果檔案。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-11-23）](originals/PA3_20211123_2330_Andy.pdf)為準。11/23 修訂版。

主題面只保留這個年度／作業的最新版；補充說明、Q&A、公開測資與必要附件依各自用途保留。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [PA3_20211123_2330_Andy.pdf](originals/PA3_20211123_2330_Andy.pdf) |
| 公開測資／範例 | [PA3_testcase1.pdf](originals/PA3_testcase1.pdf) |
| 公開測資／範例 | [PA3_testcase2.pdf](originals/PA3_testcase2.pdf) |
| 公開測資／範例 | [PA3_testcase3.pdf](originals/PA3_testcase3.pdf) |


## 匿名參考程式與有誤原版

最新版修復、原版問題與可重跑測試見[總驗證紀錄](../../../reports/reference-repairs.md)。各來源將有誤 `original/` 與通過驗證的 `fixed/` 分開保存：

- [cobalt-heron-2230](references/cobalt-heron-2230/README.md)
- [moss-lynx-5375](references/moss-lynx-5375/README.md)
