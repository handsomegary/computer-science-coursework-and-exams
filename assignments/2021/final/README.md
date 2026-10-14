# 2021 Final Project：Design Synthesis

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/final --all --output-dir build/examples/2021-final
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

[solution.md](solution.md) 包含完整重建題目、K-map 中介格式、benchmark 格式、實跑範例及解法報告。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic main.cpp -o synth
./synth state_table.txt circuit.bench
g++ -std=c++11 -Wall -Wextra -Wpedantic bonus.cpp -o synth_bonus
./synth_bonus state_table.txt circuit.bench
```

兩個入口各自編譯，不能把兩個 main 同時連結。`synthesis.hpp` 會 include 自寫的 `kmap.cpp`，呼叫 `void top()`，確實輸出並重新讀入 `kmap_in.txt` / `kmap_out.txt`，再輸出電路。這些中介檔固定寫在目前工作目錄；請為每個 run 使用自己的目錄。

一般版按官方 DFF、最多 4 個 present+input 變數，並支援任意列/section 順序與名稱。擴充版支援 D/T/JK、最多 8 個變數；加 `--allow-incomplete` 可把未提供的組合當 don't-care。資料中沒有另一份正式 bonus 題面，這些是明示擴充。`logic.hpp` 使用自寫 QM/Petrick 精確 SOP 最小化。

官方 2021 題面、補充說明、更新問答與兩組公開案例提供本版格式依據。電路線名或 gate 輸入順序不必與公開答案相同，連線邏輯必須等價。

原始兩組案例的 [test/state_table_1.txt／state_table_2.txt](course_support/public_testcases/test) 與同目錄上層的兩份原名 `circuit_1(可以用記事本開啟).bench`／`circuit_2(可以用記事本開啟).bench` 已原樣保留。可先在本案編譯 `synth`，建立全新的 `own-run/` 後，進入該目錄執行 `../synth ../course_support/public_testcases/test/state_table_1.txt circuit.bench`；中介檔與結果都留在自己的目錄。原件的映射與格式限制見[附件核對紀錄](../../document-companions.md)；[官方 Q&A 的本機保存副本](originals/相關問題.docx)另附原公告內容。

[課程提供的原版 kmap.cpp](course_support/README.md)另存於 `course_support/`，附來源依據、使用方式及等價性／最小性驗證結果。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2021-12-10）](originals/2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf)為準。搭配 2022-01-12 補充說明與課程 Q&A。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf](originals/2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf) |
| 補充說明 | [2021Fall_EE1003_IntroCS_Final_Projec_Workstation_Info.pdf](originals/2021Fall_EE1003_IntroCS_Final_Projec_Workstation_Info.pdf) |
| 補充說明 | [2021Fall_EE1003_IntroCS_Final_Project_supplymental_document.pdf](originals/2021Fall_EE1003_IntroCS_Final_Project_supplymental_document.pdf) |
| 公開測資／範例 | [Final_Project_public_testcase_1.pdf](originals/Final_Project_public_testcase_1.pdf) |
| 公開測資／範例 | [Final_Project_public_testcase_2.pdf](originals/Final_Project_public_testcase_2.pdf) |

## 匿名參考程式與有誤原版

最新版修復、原版問題與可重跑測試見[總驗證紀錄](../../../reports/reference-repairs.md)。各來源將有誤 `original/` 與通過驗證的 `fixed/` 分開保存：

- [jade-lynx-c0ee](references/jade-lynx-c0ee/README.md)
