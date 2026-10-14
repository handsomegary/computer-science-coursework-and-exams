# 匿名期末專題參考程式：jade-lynx-c0ee

[original/basic.cpp](original/basic.cpp) 保留有誤原版；[fixed/basic.cpp](fixed/basic.cpp) 是修復版本。原版內沒有個人姓名、學號或本機路徑，因此只改匿名目錄與通用檔名，原始位元組保留。兩邊的 `kmap.cpp` 是相同的薄包裝，連到[課程提供的原版模組](../../course_support/README.md)，補齊作業要求的相依檔案。

| 原版問題 | 修復行為 |
|---|---|
| 對所有行直接存取 `line[2]`，單欄位的短行可越界；固定字元偏移也無法解析較長名稱。 | 按 section 標題與 token 解析，檢查行寬、完整組合與名稱；支援多字元名稱、section／資料列亂序及 Next 欄位重排。 |
| 二變數 2×2 K-map 仍使用三／四變數的固定交換表。 | 依行／列位數產生 Gray-code 排列，先以 present＋input 組合重建真值表。 |
| 常數 0 的空 term 清單會索引越界；常數 1 產生空 `AND()`；EOF／空行解析有越界風險。 | 安全解析 `#` 與 cube，常數輸出明確的 `BUFF(0)`／`BUFF(1)`；單一 literal 也明確用 buffer 連接。 |
| 使用變長陣列，不屬標準 C++11。 | 使用有界尺寸的 vector，移除 VLA。 |
| 預設 N1／DA 等線名可能與使用者 signal 衝突。 | 所有內部線名檢查唯一性，保留 `CLK` 作為隱含時鐘，拒絕同名資料 signal。 |
| 檔名別名或中間檔同名可覆寫輸入；課程模組寫出失敗可能留下舊結果。 | 檢查絕對路徑與既有檔案身分（含 hard link），禁止輸入／輸出／中間檔互相別名；呼叫 `top()` 前檢查新 K-map 輸出可寫，並檢查關檔結果。 |

這是對原流程的實質重整：保留 `state` section、`k_in_file()`、課程 `top()`、`k_out_file()`、`process_bool()`／`process_bool_2()` 及共用乘積項的架構，重寫不安全的解析與 BENCH 產生部分。修復程式沒有包含本儲存庫主要解答。使用[目前題面與 1/12 補充資料](../../README.md)；正常執行只有檔案輸入輸出，錯誤以非零狀態退出。

課程模組僅直接支援 2～4 個變數；本修復對零／一變數複製無關的 dummy 維度，呼叫相同原版模組，再投影掉 dummy literals。仍遵守一般版最多四個 present＋input 變數。原版模組的結果邏輯等價，但不保證最少 SOP 項數，限制沿用課程來源說明。

```sh
g++ -std=c++11 -O2 -Wall -Wextra -Wpedantic -Werror -pedantic-errors fixed/basic.cpp -o synth
./synth states.txt circuit.bench
# 在儲存庫根目錄：
python tests/test_repaired_final.py --exhaustive --with-originals
```

GCC 14.2.0、C++11 的 `-O0`／`-O2` 均零警告通過 **127 組／構建**，合計 254 組：包含原先 28 組、亂序與名稱衝突、常數、零／一變數、空 output，以及十組失敗時檔案保持不變的情境。`--exhaustive` 在合成的 BENCH 中逐列驗證全部 **65,808 個 2～4 變數布林輸出函數**，同時檢查正確 INPUT／OUTPUT 宣告及唯一驅動線名。

原版初次正常構建曾通過 19/28；本輪另啟用 `_GLIBCXX_ASSERTIONS` 的原版構建在 28 組都失敗，暴露了短行／容器索引的未定義行為。歷史來源仍完整保留，這些結果不視為通過的版本。測試不執行下載的 EXE，只執行本次從原始碼重新編譯的程式。

| 版本 | SHA256 |
|---|---|
| 原版 | `6f881c0e5a6aef730980455b0b6b38a44ce37f35d435098f77c0278c24ba020a` |
| 修復版 | `92d710c3e9e01132398cf3525f37bf2ff93de1731a2e7aa882eefea4051ad0cb` |
