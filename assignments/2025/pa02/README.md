# PA2：Knight's Treasure Hunt（2025）

[完整題目、解法與程式碼](solution.md)。用遞迴、合法上界剪枝、可達區域分析及記憶化搜尋最高分路徑，包含成長守衛 bonus。

```sh
g++ -std=c++11 -O2 main.cpp -o knight
./knight < input.txt
./knight --bonus < bonus_input.txt
./knight --summary < input.txt
```

`--summary` 只輸出 `最佳分數 步數`，供機器讀取；預設輸出完整 replay 與棋盤。搜尋是精確的，未使用時間截止、隨機採樣或把上界當答案；大棋盤仍可能需要較長搜尋時間。測試包含 48 個獨立窮舉 oracle、官方基本範例與成長守衛範例。
