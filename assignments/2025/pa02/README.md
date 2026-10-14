# PA2：Knight's Treasure Hunt（2025）

[完整題目、解法與程式碼](solution.md)。用遞迴、合法上界剪枝、可達區域分析及記憶化搜尋最高分路徑，包含成長守衛 bonus。

```sh
g++ -std=c++11 -O2 main.cpp -o knight
./knight < input.txt
./knight --bonus < bonus_input.txt
./knight --summary < input.txt
```

`--summary` 只輸出 `最佳分數 步數`，供機器讀取；預設輸出完整 replay 與棋盤。搜尋是精確的，未使用時間截止、隨機採樣或把上界當答案；大棋盤仍可能需要較長搜尋時間。測試包含 48 個獨立窮舉 oracle、官方基本範例與成長守衛範例。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2025-10-15）](originals/2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf)為準。搭配後續 Detailed Explanation、Checker 與 Hint。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf](originals/2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Checker.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Checker.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Detailed_Explanation.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Detailed_Explanation.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Hint.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Hint.pdf) |
