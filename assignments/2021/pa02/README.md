# 2021 PA2：二進位累加與二補數加分版

原 [PA2 Positive Binary Adder.docx](originals/PA2%20Positive%20Binary%20Adder.docx) 保存 11/04 官方 Q&A、11/02 溢位判斷公告與 10/28 修訂公告；按原 bytes 保存，來源與 SHA256 見[附件核對紀錄](../../document-companions.md)。較早公告供歷史核對，目前規格仍以 11/14 最新 PDF 為準。

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/pa02 --all --output-dir build/examples/2021-pa02
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

依提供的作業題目與參考程式完成，以 2021 年最後修訂規格和公開測資為準。一般版 `main.cpp` 使用 `-1` 結束，加分版 `bonus.cpp` 接受負數並使用 `0` 結束；兩版共用 `binary.hpp`，分別編譯。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o pa02
g++ -std=c++11 -Wall -Wextra -Wpedantic bonus.cpp -o pa02_bonus
./pa02
./pa02_bonus
```

完整題目、限制與二補數範例歧義的採用規則見 [solution.md](solution.md)。執行 `python tests/test_pa2021_01_03.py` 可重跑獨立整數模型、19 位儲存邊界、逐次累加、所有輸入位置的結束值、非法 token 與英文序數驗證。完整範例不包含輸入回顯。

完成範圍包括三個指定遞迴函式、二進位全加器、累計部分和、加分版符號延伸及二補數、可儲存範圍、防呆與重讀。同時遵守沒有全域／static 變數，所有本地變數和函式參數僅 `int`／`long long int`，輸入值放 `int`、二進位 0/1 數字放 `long long int`。沒有用字串、陣列、浮點數或十進位加減取代題目要求的數值運算。

整理時解決了迴圈取代必需遞迴、用前兩個輸入而非累計值檢查、錯誤分支漏判 sentinel、二進位溢位和第 11/12/13/21 等序數問題。難點是 0/1 數字以十進位欄位儲存後只容納 19 位，以及負數符號延伸後的進位與溢位；以固定寬度全加器和獨立符號位處理。README 與題解提供文字報告，未另產生含個人身分的 PDF。建議原題明確統一「最少位數」和負的二次方範例。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-11-14）](originals/PA2_20211114_1530_Andy.pdf)為準。11/14 修訂版。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [PA2_20211114_1530_Andy.pdf](originals/PA2_20211114_1530_Andy.pdf) |
| 公開測資／範例 | [PA2_1_測資.pdf](originals/PA2_1_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_2_測資.pdf](originals/PA2_2_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_3_測資.pdf](originals/PA2_3_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_4_測資.pdf](originals/PA2_4_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_5_測資.pdf](originals/PA2_5_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_Bonus_1_測資.pdf](originals/PA2_Bonus_1_%E6%B8%AC%E8%B3%87.pdf) |
| 公開測資／範例 | [PA2_Bonus_2_測資.pdf](originals/PA2_Bonus_2_%E6%B8%AC%E8%B3%87.pdf) |

### 歷史題面

| 原始版本 |
|---|
| [PA2_20211018_0410_Andy.pdf](originals/archive/PA2_20211018_0410_Andy.pdf) |
| [PA2_20211028_2006_Andy.pdf](originals/archive/PA2_20211028_2006_Andy.pdf) |

## 匿名參考程式與有誤原版

最新版修復、原版問題與可重跑測試見[總驗證紀錄](../../../reports/reference-repairs.md)。各來源將有誤 `original/` 與通過驗證的 `fixed/` 分開保存：

- [jade-lynx-c0ee](references/jade-lynx-c0ee/README.md)
- [moss-lynx-5375](references/moss-lynx-5375/README.md)
