# 2021 Final Project：Design Synthesis

[solution.md](solution.md) 包含完整重建題目、K-map 中介格式、benchmark 格式、實跑範例及解法報告。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o synth
./synth state_table.txt circuit.bench
g++ -std=c++11 -Wall -Wextra -Wpedantic bonus.cpp -o synth_bonus
./synth_bonus state_table.txt circuit.bench
```

兩個入口各自編譯，不能把兩個 main 同時連結。`synthesis.hpp` 會 include 自寫的 `kmap.cpp`，呼叫 `void top()`，確實輸出並重新讀入 `kmap_in.txt` / `kmap_out.txt`，再輸出電路。這些中介檔固定寫在目前工作目錄；請為每個 run 使用自己的目錄。

一般版按官方 DFF、最多 4 個 present+input 變數，並支援任意列/section 順序與名稱。擴充版支援 D/T/JK、最多 8 個變數；加 `--allow-incomplete` 可把未提供的組合當 don't-care。資料中沒有另一份正式 bonus 題面，這些是明示擴充。`logic.hpp` 使用自寫 QM/Petrick 精確 SOP 最小化；沒有發佈原參考程式或個資。

官方 2021 題面、補充說明、更新問答與兩組公開案例提供本版格式依據。電路線名或 gate 輸入順序不必與公開答案相同，連線邏輯必須等價。

## 原始作業 PDF

以下 PDF 保留來源檔名與原始內容；同一題目的不同修訂版分別保存。

| 類型 | 原始檔案 |
|---|---|
| 作業題面／修訂版 | [2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf](originals/2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf) |
| 補充說明 | [2021Fall_EE1003_IntroCS_Final_Projec_Workstation_Info.pdf](originals/2021Fall_EE1003_IntroCS_Final_Projec_Workstation_Info.pdf) |
| 補充說明 | [2021Fall_EE1003_IntroCS_Final_Project_supplymental_document.pdf](originals/2021Fall_EE1003_IntroCS_Final_Project_supplymental_document.pdf) |
| 公開測資／範例 | [Final_Project_public_testcase_1.pdf](originals/Final_Project_public_testcase_1.pdf) |
| 公開測資／範例 | [Final_Project_public_testcase_2.pdf](originals/Final_Project_public_testcase_2.pdf) |
