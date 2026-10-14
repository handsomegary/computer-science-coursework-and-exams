# PA3：Big Number Energy Distribution Calculator（2025）

[完整題目、解法與程式碼](solution.md)。自行實作 base 2–16 大整數加減、乘法與小整數除法，支援 100 位輸入、負數 bonus 與正確處理負數／半數的全域平均。

```sh
g++ -std=c++11 -O2 main.cpp -o energy
./energy < input.txt
./energy --bonus < signed_input.txt
./energy --plain --bonus < signed_input.txt
```

`--plain` 省略提示與結束訊息，只印結果矩陣；若加上 `--bonus`，矩陣後再印四捨五入平均。`big_number.h` 必須與 main.cpp 同目錄。測試透過 Python 任意精度整數獨立核對全部 15 種進位、乘加、負數四捨五入、100 位連續進位與輸入防呆。

## 原始作業 PDF

以下 PDF 保留來源檔名與原始內容；同一題目的不同修訂版分別保存。

| 類型 | 原始檔案 |
|---|---|
| 作業題面／修訂版 | [2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf](originals/2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf) |
