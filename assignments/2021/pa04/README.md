# 2021 PA4 — MySpice

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2021/pa04 --all --output-dir build/examples/2021-pa04
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

完成電阻 ladder 的讀檔、電壓／電流計算、相鄰 stage 交換與合併、電路及結果匯出，以及獨立 Thevenin 加分入口。採 2022-01-04 的最新修訂與 Q&A；`RP` 路徑內的多個電阻是依序串接。

```sh
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror main.cpp -o myspice
g++ -std=c++11 -Wall -Wextra -Wpedantic -Werror bonus.cpp -o myspice_bonus
./myspice
./myspice_bonus
```

批次模式直接讀檔、依序修改電路並產生結果，成功時標準輸出與標準錯誤皆為空：

```sh
./myspice input.txt result.txt --swap 1 2 --merge 1 2
./myspice input.txt raw.txt --raw
./myspice_bonus input.txt thevenin.txt
./myspice_bonus input.txt legacy.txt --legacy-thevenin
```

一般版批次預設輸出電路與計算表，加分版批次預設輸出 Thevenin；加分版可加 `--circuit` 輸出一般結果。`--all-nodes` 包含路徑內節點，`--raw` 提供高精度數值。交換與合併使用目前排列位置，接受相鄰位置的任一輸入順序；合併保留較前方 stage。

Thevenin 預設觀察最後 stage 節點對地，包含原電路最後的 `RP` 支路，與講義圖中 A、B 端點一致。舊公開 bonus 數值把 `Vdd` 和總輸入電阻當成 `Vth`、`Rth`；`--legacy-thevenin` 明確提供這個舊表的兼容行為。兩者的差異與數學推導見 [完整題面、解法與報告](solution.md)。

`main.cpp`、`bonus.cpp` 各自包含 `myspice.hpp`，應分開編譯。範例檔、執行參數和精確輸出都收在 [samples.json](samples.json)。獨立測試位於儲存庫 `tests/test_pa2021_04_05_final.py`，使用一般節點矩陣與 Gaussian elimination 驗證，而非複製程式的 ladder 遞推。

原始公開附件也已完整保留在 [course_support/public_testcases](course_support/public_testcases)：三份 `inputX.txt`、三份 `outputX.txt`、兩份 `bonusX.txt`，以及 middle merge 的 [input.txt](course_support/public_testcases/merge_middle/input.txt)。例如在本案目錄編譯後，可執行 `./myspice course_support/public_testcases/input1.txt new-result.txt --swap 1 2 --merge 1 2`；請用新的結果檔名。這些是提供資料的原 bytes，與 `samples.json` 的生成案例分開；原輸出的格式差異及 bonus 舊值見[附件核對紀錄](../../document-companions.md)。[PA4 官方公告／Q&A 的原本機保存副本](originals/PA4相關問答.docx)包含 input/output 與 bonus 釋出及勘誤公告。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2022-01-04）](originals/PA4_MySpice_HO_20220104_1730.pdf)為準。1/4 修訂版；仍屬 2021 Fall。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [PA4_MySpice_HO_20220104_1730.pdf](originals/PA4_MySpice_HO_20220104_1730.pdf) |
| 補充說明 | [PA4_sample_code_Stud.pdf](originals/PA4_sample_code_Stud.pdf) |
| 公開測資／範例 | [PA4_Public_Case_01.pdf](originals/PA4_Public_Case_01.pdf) |
| 公開測資／範例 | [PA4_Public_Case_02.pdf](originals/PA4_Public_Case_02.pdf) |
| 公開測資／範例 | [PA4_Public_Case_03.pdf](originals/PA4_Public_Case_03.pdf) |
| 公開測資／範例 | [PA4_bonus_Case_01.pdf](originals/PA4_bonus_Case_01.pdf) |
| 公開測資／範例 | [PA4_bonus_Case_02.pdf](originals/PA4_bonus_Case_02.pdf) |
| 公開測資／範例 | [PA4_merge(middle)_example.pdf](originals/PA4_merge%28middle%29_example.pdf) |

### 歷史題面

| 原始版本 |
|---|
| [PA4_MySpice_Andy_20211207_0200.pdf](originals/archive/PA4_MySpice_Andy_20211207_0200.pdf) |
| [PA4_MySpice_HO_20211223_2300.pdf](originals/archive/PA4_MySpice_HO_20211223_2300.pdf) |
| [PA4_MySpice_HO_20211228_2330.pdf](originals/archive/PA4_MySpice_HO_20211228_2330.pdf) |

## 匿名參考程式與有誤原版

最新版修復、原版問題與可重跑測試見[總驗證紀錄](../../../reports/reference-repairs.md)。各來源將有誤 `original/` 與通過驗證的 `fixed/` 分開保存：

- [jade-lynx-c0ee](references/jade-lynx-c0ee/README.md)
- [moss-lynx-5375](references/moss-lynx-5375/README.md)
