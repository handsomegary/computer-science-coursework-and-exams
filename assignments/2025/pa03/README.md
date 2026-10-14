# PA3：Big Number Energy Distribution Calculator（2025）

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2025/pa03 --all --output-dir build/examples/2025-pa03
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

[完整題目、解法與程式碼](solution.md)。自行實作 base 2–16 大整數加減、乘法與小整數除法，支援 100 位輸入、負數 bonus 與正確處理負數／半數的全域平均。

```sh
g++ -std=c++11 -O2 main.cpp -o energy
./energy < input.txt
./energy --bonus < signed_input.txt
./energy --plain --bonus < signed_input.txt
```

`--plain` 省略提示與結束訊息，只印結果矩陣；若加上 `--bonus`，矩陣後再印四捨五入平均。`big_number.h` 必須與 main.cpp 同目錄。測試透過 Python 任意精度整數獨立核對全部 15 種進位、乘加、負數四捨五入、100 位連續進位與輸入防呆。

下載此目錄的 `main.cpp`、`big_number.h` 即有全部編譯相依檔案；[samples.json](samples.json) 提供可重現的輸入輸出。官方測資位於課程 OJ，本次提供的資料沒有測資 ZIP 或 checker，因此沒有另稱已完整保存 OJ 題庫。從 repo 根目錄執行 `python tests/test_pa2025.py` 可使用已上傳的獨立算術測試，無須存取 OJ。

## 作業說明（目前版本）

[原始發布紀錄 說明.txt](originals/release_notes/說明.txt) 按原名與原 bytes 保留，包含 OJ 公開測資與影片發布紀錄。本機提供資料沒有下載的 OJ 題庫／checker，影片與 OJ 以外部資源分類；不以自製測試冒充原課程題庫。詳見[附件核對清單](../../document-companions.md)。

以本次提供資料中最新的[作業題面（2025-11-26）](originals/2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf)為準。提供資料中的 11/26 題面。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf](originals/2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf) |
