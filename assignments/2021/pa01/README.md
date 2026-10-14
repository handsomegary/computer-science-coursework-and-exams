# 2021 PA1：五倍券付款

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/pa01 --all --output-dir build/examples/2021-pa01
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

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

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-10-17）](originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf)為準。V2；內文更新日為 2021-09-30，10/17 檔修正報告檔名範例。

主題面只保留這個年度／作業的最新版；補充說明、Q&A、公開測資與必要附件依各自用途保留。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf](originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf) |
| 公開測資／範例 | [PA1_corner case_1.pdf](originals/PA1_corner%20case_1.pdf) |
| 公開測資／範例 | [PA1_testcase1.pdf](originals/PA1_testcase1.pdf) |
| 公開測資／範例 | [PA1_testcase2.pdf](originals/PA1_testcase2.pdf) |
| 公開測資／範例 | [PA1_testcase3.pdf](originals/PA1_testcase3.pdf) |
| 公開測資／範例 | [PA1_testcase4.pdf](originals/PA1_testcase4.pdf) |
| 公開測資／範例 | [PA1_testcase5.pdf](originals/PA1_testcase5.pdf) |


## 經驗證的匿名參考程式

[moss-lynx-5375](references/moss-lynx-5375/README.md) 收錄一般版與最少損失擴充版；附測試範圍、介面差異與原碼 SHA256。

## 匿名參考程式與有誤原版

最新版修復、原版問題與可重跑測試見[總驗證紀錄](../../../reports/reference-repairs.md)。各來源將有誤 `original/` 與通過驗證的 `fixed/` 分開保存：

- [jade-lynx-c0ee](references/jade-lynx-c0ee/README.md)
- [moss-lynx-5375](references/moss-lynx-5375/README.md)
