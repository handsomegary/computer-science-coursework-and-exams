# PA1：Dinosaur Game（2025）

下載後可從儲存庫根目錄直接生成範例輸入、編譯並保留真正的執行結果：

```sh
python tools/run_example.py assignments/2025/pa01 --all --output-dir build/examples/2025-pa01
```

使用已附的 `samples.json` 與本案原始碼，無須本機其他檔案；產物與命令保存在新目錄，現有目錄會被拒絕。用法與結果位置見[根目錄 README](../../../README.md#編譯與驗證)。下方手動指令的輸入檔名可自行選擇。

[完整題目、解法與程式碼](solution.md)。基本版、雙層 bonus 與自動遊玩 bonus 共用一個完整的狀態轉移實作，只引入 `<iostream>`。

```sh
g++ -std=c++11 -O2 main.cpp -o dinosaur
./dinosaur < input.txt
./dinosaur --double < double_input.txt
./dinosaur --auto < auto_input.txt
```

預設輸出含輸入回顯，以符合提供的終端畫面測資；`--no-echo` 可停用回顯。雙層版需先輸入下層 8 位、上層 8 位與天花板 9 位配置。自動版每個 tick 只讀下一組障礙／天花板，不讀動作。

官方兩份基本範例、一份雙層範例已與文字 golden 核對（內容一致，程式另外保留最後換行）。自動版的官方測資已產生合法勝利路徑。額外測試涵蓋非法浮點命令、重複跳躍、碰撞、直接勝利，以及 10,000 tick 上限。

## 官方下載附件與可直接執行的例子

[原版 Public Testcases.zip](originals/resources/Public%20Testcases.zip) 完整保留；[解壓測資](course_support/public_testcases) 保留包內結構、四份輸入、三份文字 golden、三份 PDF 及三份 DOCX。所有檔案的原名、位置與 SHA256 見 [resources.json](../resources.json)。在本目錄編譯後：

```sh
./dinosaur < "course_support/public_testcases/Basic testcase/PA1_in1.txt"
./dinosaur --double < "course_support/public_testcases/Bonus1 testcase/PA1_Bonus1_in1.txt"
./dinosaur --auto < "course_support/public_testcases/Bonus2 testcase/PA1_Bonus2_in1.txt" > auto.log
```

PowerShell 可用 `Get-Content "course_support/public_testcases/Basic testcase/PA1_in1.txt" | .\dinosaur.exe`。自動版的官方 [checker.out](course_support/checker.out) 是 Linux x86-64 程式；在 Linux／WSL／課程工作站依 [Checker 說明](originals/2025Fall_EE1003_IntroCS_PA1_Checker.pdf) 使用，參數順序是輸出 log 在前、輸入測資在後：

```sh
chmod u+x course_support/checker.out
course_support/checker.out auto.log "course_support/public_testcases/Bonus2 testcase/PA1_Bonus2_in1.txt"
```

本次保存 checker 原檔但未執行。從 repo 根目錄執行 `python tests/test_official_resources_2025.py`，可跨平台核對三份 golden 及自動版勝利結果。

## 作業說明（目前版本）

[原始發布紀錄 說明.txt](originals/release_notes/說明.txt) 按原名與原 bytes 保留，包含公開 ZIP、checker、影片、OJ 與補充文件的發布紀錄；外部課程影片／服務不冒充本機附件，詳見[附件核對清單](../../document-companions.md)。

以本次提供資料中最新的[作業題面（2025-09-25）](originals/2025Fall_EE1003_Intro_to_CS_PA1_Andy_20250925.pdf)為準。內文列 version 3（2025-09-21）；使用提供資料中 9/25 命名的修訂檔。

主題面只保留這個年度／作業的最新版；補充說明、Q&A、公開測資與必要附件依各自用途保留。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2025Fall_EE1003_Intro_to_CS_PA1_Andy_20250925.pdf](originals/2025Fall_EE1003_Intro_to_CS_PA1_Andy_20250925.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA1_Checker.pdf](originals/2025Fall_EE1003_IntroCS_PA1_Checker.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA1_Detailed_Explanation.pdf](originals/2025Fall_EE1003_IntroCS_PA1_Detailed_Explanation.pdf) |
| 補充說明 | [2025Fall_EE1003_IntroCS_PA1_Workstation_Info.pdf](originals/2025Fall_EE1003_IntroCS_PA1_Workstation_Info.pdf) |
| 補充說明 | [2025_0925_PA1_Common_Problem.pdf](originals/2025_0925_PA1_Common_Problem.pdf) |
| 補充說明 | [2025_PA1_OJ_Tutorial.pdf](originals/2025_PA1_OJ_Tutorial.pdf) |
