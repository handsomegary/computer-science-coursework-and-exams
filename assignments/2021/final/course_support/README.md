# 課程提供的 K-map 程式

[kmap.cpp](kmap.cpp) 是提供資料中供 Final Project 使用的課程支援程式，保留來源檔的內容與編碼。2022-01-12 的[Final 補充說明](../originals/2021Fall_EE1003_IntroCS_Final_Project_supplymental_document.pdf)在「Karnaugh Map Program User Guide」明載課程會提供 `kmap.cpp`，並以 `void top()` 整合。文件能確認是課程提供，沒有證據能確定是教授本人撰寫。

將此檔與自己的程式放在同一資料夾，使用 `#include "kmap.cpp"` 後呼叫 `top()`。它讀取目前工作目錄的 `kmap_in.txt`，並寫出 `kmap_out.txt`；沒有獨立的 `main()`。輸入支援 2、3、4 個變數，格值為 0／1；輸出 SOP 各 literal 用 0、1、2 表示，其中 2 代表該變數不出現在乘積項。

驗證使用 C++11 與 `-Wall -Wextra -Wpedantic`，沒有編譯警告。全部 2～4 變數的 **65,808 個布林函數**均通過逐列真值表等價性檢查；另與獨立 exact-cover 模型比較 314 個案例，其中 54 個輸出不是最少乘積項／literal。因此它適合作為課程原版介面參考，不能當作精確最小 SOP 求解器。例：三變數、minterms `{0,2,4,6}` 的函數只需一個 literal，此程式會輸出兩項、三個 literal。

本專案的主要解答使用[另行實作的相容介面](../kmap.cpp)與精確 QM/Petrick 求解器；課程原版保存在本資料夾供比較。

SHA256：`9244d692c0376befd7ae49498f5262fd77c9e0a3b4dc280805d5bd8178d39e1d`。

在儲存庫根目錄執行 `python tests/test_course_kmap.py` 可重跑上述驗證；可用 `--cxx` 指定 C++11 編譯器路徑。
