# 2021 PA1：五倍券付款

依提供的作業題目與參考程式完成，正式規格採最後修訂的 V2，以及五份公開測資和零元付款補充案例。本目錄提供重新整理的解法、匿名測試，以及下方列出的原始作業 PDF。

一般版 `main.cpp` 完成題目的大面額優先付款；加分版 `bonus.cpp` 另外尋找最少損失付款，並處理非法價格、種類、會員狀態與超大輸入。兩版共用 `vouchers.hpp`，入口分開編譯。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o pa01
g++ -std=c++11 -Wall -Wextra -Wpedantic bonus.cpp -o pa01_bonus
./pa01
./pa01_bonus
```

完整題目、精確輸入輸出、補定規則、演算法、程式碼與範例見 [solution.md](solution.md)。範例輸出不包含終端機的輸入回顯。可在儲存庫根目錄執行 `python tests/test_pa2021_01_03.py`，以獨立的可達金額模型與枚舉最佳化模型驗證兩版。

完成範圍包括逐次消費、三種折扣、整數截尾、庫存更新、不足額拒絕、零元消費、超額付款無找零，以及 `-1` 和券用盡的結束流程。整理時消除了浮點截尾誤差、總額與實際剩券不同步、付款分支重複與錯誤輸入遺留的問題。主要難點是「能夠剛好付清」與「必須改付大面額」的分界；以較小面額的可付金額來判斷，並用不同模型驗證。未另製作原課程格式的 PDF 報告；本 README 與題解提供可重跑的實作報告。建議原題統一各範例的縮排與非法輸入訊息，避免格式歧義。

## 原始作業 PDF

以下 PDF 保留來源檔名與原始內容；同一題目的不同修訂版分別保存。

| 類型 | 原始檔案 |
|---|---|
| 作業題面／修訂版 | [2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20210927_0038.pdf](originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20210927_0038.pdf) |
| 作業題面／修訂版 | [2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf](originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf) |
| 作業題面／修訂版 | [2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_V2_Andy_20210990_0022.pdf](originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_V2_Andy_20210990_0022.pdf) |
| 公開測資／範例 | [PA1_corner case_1.pdf](originals/PA1_corner%20case_1.pdf) |
| 公開測資／範例 | [PA1_testcase1.pdf](originals/PA1_testcase1.pdf) |
| 公開測資／範例 | [PA1_testcase2.pdf](originals/PA1_testcase2.pdf) |
| 公開測資／範例 | [PA1_testcase3.pdf](originals/PA1_testcase3.pdf) |
| 公開測資／範例 | [PA1_testcase4.pdf](originals/PA1_testcase4.pdf) |
| 公開測資／範例 | [PA1_testcase5.pdf](originals/PA1_testcase5.pdf) |
