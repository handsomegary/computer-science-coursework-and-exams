# 2021 PA5：Logic Minimization

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/pa05 --all --output-dir build/examples/2021-pa05
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

完整題目、規格補定、解法及實跑範例見 [solution.md](solution.md)。這份資料提供的是 QM/Petrick bonus 題面，沒有另一份可核對的 base 題面；本程式完成該題全部功能，另支援最多 16 變數的一般 SOP 輸入。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o logic_min
./logic_min input.eqn output.eqn
```

未給命令列參數時會依序詢問輸入及輸出檔名。只需 `main.cpp` 與同目錄 `logic.hpp`；C++11 標準函式庫即可。有效的完整 minterm 輸入支援 1–26 個變數，精確選擇最少乘積項，再比較 literal 數與固定排序。Exact cover 的時間及記憶體最差呈指數成長，並非大型 EDA 工具。

題面依課程提供的 2021 Logic Minimization bonus 說明、更新問答及三組公開測資重寫；演算法與程式碼為重新實作。公開測資的 Boolean 資料可於測試中核對；原始題面與公開測資 PDF 見下方連結。

三套原始 [testX.eqn／testXout.eqn](course_support/public_testcases) 也已保留，可在本案目錄執行 `./logic_min course_support/public_testcases/test1.eqn new-result.eqn`，用新的結果檔名。原文件及等價性的比對方式見[附件核對紀錄](../../document-companions.md)；[官方公告／Q&A 的本機保存副本](originals/PA5相關問題.docx)包含 1/14 修正重複 output 項的公告，並未混入學生報告。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-12-09）](originals/PA5_bonus_QM_Andy_20211209_1855.pdf)為準。提供資料中的 QM/Petrick bonus 題面。

主題面只保留這個年度／作業的最新版；補充說明、Q&A、公開測資與必要附件依各自用途保留。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [PA5_bonus_QM_Andy_20211209_1855.pdf](originals/PA5_bonus_QM_Andy_20211209_1855.pdf) |
| 公開測資／範例 | [PA5_testcase1.pdf](originals/PA5_testcase1.pdf) |
| 公開測資／範例 | [PA5_testcase2.pdf](originals/PA5_testcase2.pdf) |
| 公開測資／範例 | [PA5_testcase3.pdf](originals/PA5_testcase3.pdf) |
