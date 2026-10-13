# 2021 PA5：Logic Minimization

完整題目、規格補定、解法及實跑範例見 [solution.md](solution.md)。這份資料提供的是 QM/Petrick bonus 題面，沒有另一份可核對的 base 題面；本程式完成該題全部功能，另支援最多 16 變數的一般 SOP 輸入。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o logic_min
./logic_min input.eqn output.eqn
```

未給命令列參數時會依序詢問輸入及輸出檔名。只需 `main.cpp` 與同目錄 `logic.hpp`；C++11 標準函式庫即可。有效的完整 minterm 輸入支援 1–26 個變數，精確選擇最少乘積項，再比較 literal 數與固定排序。Exact cover 的時間及記憶體最差呈指數成長，並非大型 EDA 工具。

題面依課程提供的 2021 Logic Minimization bonus 說明、更新問答及三組公開測資重寫；演算法與程式碼為重新實作。公開測資的 Boolean 資料可於測試中核對，未收錄原始講義、其他人的答案或任何個人資料。
