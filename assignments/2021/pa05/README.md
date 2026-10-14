# 2021 PA5：Logic Minimization

完整題目、規格補定、解法及實跑範例見 [solution.md](solution.md)。這份資料提供的是 QM/Petrick bonus 題面，沒有另一份可核對的 base 題面；本程式完成該題全部功能，另支援最多 16 變數的一般 SOP 輸入。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o logic_min
./logic_min input.eqn output.eqn
```

未給命令列參數時會依序詢問輸入及輸出檔名。只需 `main.cpp` 與同目錄 `logic.hpp`；C++11 標準函式庫即可。有效的完整 minterm 輸入支援 1–26 個變數，精確選擇最少乘積項，再比較 literal 數與固定排序。Exact cover 的時間及記憶體最差呈指數成長，並非大型 EDA 工具。

題面依課程提供的 2021 Logic Minimization bonus 說明、更新問答及三組公開測資重寫；演算法與程式碼為重新實作。公開測資的 Boolean 資料可於測試中核對；原始題面與公開測資 PDF 見下方連結。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-12-09）](originals/PA5_bonus_QM_Andy_20211209_1855.pdf)為準。提供資料中的 QM/Petrick bonus 題面。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [PA5_bonus_QM_Andy_20211209_1855.pdf](originals/PA5_bonus_QM_Andy_20211209_1855.pdf) |
| 公開測資／範例 | [PA5_testcase1.pdf](originals/PA5_testcase1.pdf) |
| 公開測資／範例 | [PA5_testcase2.pdf](originals/PA5_testcase2.pdf) |
| 公開測資／範例 | [PA5_testcase3.pdf](originals/PA5_testcase3.pdf) |
