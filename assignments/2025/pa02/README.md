# PA2：Knight's Treasure Hunt（2025）

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2025/pa02 --all --output-dir build/examples/2025-pa02
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

[完整題目、解法與程式碼](solution.md)。用遞迴、合法上界剪枝、可達區域分析及記憶化搜尋最高分路徑，包含成長守衛 bonus。

```sh
g++ -std=c++11 -O2 main.cpp -o knight
./knight < input.txt
./knight --bonus < bonus_input.txt
./knight --summary < input.txt
```

`--summary` 只輸出 `最佳分數 步數`，供機器讀取；預設輸出完整 replay 與棋盤。搜尋是精確的，未使用時間截止、隨機採樣或把上界當答案；大棋盤仍可能需要較長搜尋時間。測試包含 48 個獨立窮舉 oracle、官方基本範例與成長守衛範例。

## 官方下載附件與可直接執行的例子

[原版 PA2_STUDENT.zip](originals/resources/PA2_STUDENT.zip) 及 [解壓 starter](course_support/starter) 包含基本、bonus 各四份公開輸入、原 makefile 與 Linux checker。兩份 `PA2.cpp`／`PA2_bonus.cpp` 在原 ZIP 中就是 0 bytes，供填寫自己的程式；完整解答使用本目錄 `main.cpp`，不需先填 starter。空的 `Result/` 以 `.gitkeep` 保留，下載後仍可寫入測試結果。原名、位置與 SHA256 見 [resources.json](../resources.json)。在本目錄編譯後：

```sh
./knight < course_support/starter/PA2/Testcase/PA2_PublicCase1_10s.txt > basic.log
./knight --bonus < course_support/starter/PA2_bonus/Testcase/PA2_bonus_PublicCase1_10s.txt > bonus.log
```

PowerShell 可用 `Get-Content course_support/starter/PA2/Testcase/PA2_PublicCase1_10s.txt | .\knight.exe`。原 [checker](course_support/starter/checker) 是 Linux x86-64 ELF，供 Linux／WSL／課程工作站使用；PA2 參數順序是測資在前、輸出 log 在後：

```sh
chmod u+x course_support/starter/checker
course_support/starter/checker course_support/starter/PA2/Testcase/PA2_PublicCase1_10s.txt basic.log
course_support/starter/checker course_support/starter/PA2_bonus/Testcase/PA2_bonus_PublicCase1_10s.txt bonus.log
```

若依課程原流程自己寫題目，將程式填入 starter 的對應 `.cpp`，在 `course_support/starter/` 執行 `make all`／`make all_bonus`、`make run_pa2`／`make run_pa2_bonus`、`make SHELL=/bin/bash check_pa2`／`make SHELL=/bin/bash check_pa2_bonus`。checker target 使用 bash 語法，因此明確指定 shell。原 makefile 需要 GNU make、bash、timeout、GNU grep、date；不要在空 starter 上直接要求編譯。詳見 [Checker 說明](originals/2025Fall_EE1003_IntroCS_PA2_Checker.pdf)。本次保存 checker 原檔但未執行。

補齊的 [Final Explanation](originals/2025Fall_EE1003_IntroCS_PA2_Final_Explanation.pdf) 是 2025 年 11 月課程官方解題補充，並非新版題面。從 repo 根目錄執行 `python tests/test_official_resources_2025.py`，可驗證八份公開輸入的合法 replay，兩份 4×4 範例另由獨立窮舉求最佳分數。

## 作業說明（目前版本）

[原始發布紀錄 說明.txt](originals/release_notes/說明.txt) 按原名與原 bytes 保留，包含 PA2_STUDENT ZIP、checker、Hint／Final Explanation 與課程影片發布；外部影片不冒充本機附件，詳見[附件核對清單](../../document-companions.md)。

以本次提供資料中最新的[作業題面（2025-10-15）](originals/2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf)為準。搭配後續 Detailed Explanation、Checker 與 Hint。

主題面只保留這個年度／作業的最新版；補充說明、Q&A、公開測資與必要附件依各自用途保留。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf](originals/2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Checker.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Checker.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Detailed_Explanation.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Detailed_Explanation.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA2_Hint.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Hint.pdf) |
| 解題補充 | [2025Fall_EE1003_IntroCS_PA2_Final_Explanation.pdf](originals/2025Fall_EE1003_IntroCS_PA2_Final_Explanation.pdf) |
