# 計算機概論 I：作業、考古題與完整程式解答

涵蓋所提供資料的 **10 個作業專案**（2021 六案、2025 四案）、**2021 六份小考的 24 道可辨識題目**、期中與期末考後回憶題。每組題目有重寫規格、推導、完整 C++11 程式與可重現範例。基礎版與加分版分開說明；程式碼直接包含在 `solution.md` 與 [solutions.json](solutions.json) 的 `solution` 欄位中。

原資料中名為「2024」的作業資料夾，其正式文件是 **2025 Fall**，期末更新至 2026-01-01。本 repo 按正式題面的學期整理在 `assignments/2025`。不把不同年度題目混用。

這是依提供資料重新實作的學習解答。考後回憶並非完整正式試卷；缺附件、選項不全、舊答案錯誤、公開範例與後續更新互相衝突的部分，均在各份解答標示重建或補定。作業原報告另以匿名副本保存，來源身分不公開。

作業原始 PDF 按年度與專案存放於各作業的 `originals/`，各年度／作業主題面只保留最新版。已移除 8 份被取代的舊題面；保留 10 份目前題面及對應的補充說明、公開測資／範例，共 54 份官方 PDF。原檔內容及必要附件不變；版本依內文修訂差異與 PDF 日期交叉判定，以本次提供資料中的最新版為準。

主要 `solution.md` 與作業入口為重新實作的解答。另收錄 13 份經重新驗證的匿名修正版、對應的 13 份有誤原版，以及原先通過的兩份匿名 PA1 程式；詳見[修復與驗證紀錄](reports/reference-repairs.md)。來源以隨機代稱呈現，原版位元組保留；[課程原版 K-map 支援程式](assignments/2021/final/course_support/README.md)另列介面與限制。[初次審查紀錄](reports/reference-validation.md)保留修復前的判定。

本機提供的非編譯資料也保存於 [source_archive](source_archive/README.txt)：30 份原 C++、1 份學生 header、13 份小資料、2 份 CBP、7 份筆記／講解及 18 份匿名原報告（共 122 頁）。原草稿與歷史輸出明確標為未驗證，未混入完成版；教授原程式、header、模板、測資與五個 ZIP 另依原附件清單保存。保留依據是原資料的用途，不限於重寫解答實際用到的檔案。只省略學生編譯的 `bin/obj/exe`、編譯暫存及可再生 IDE cache。

[29 頁原課堂教學內容](assignments/2021/pa04/originals/local_notes/classroom-notes.pdf)從本機截圖保存，僅真正移除私人會議／瀏覽器 UI；[轉換對照](assignments/2021/pa04/originals/local_notes/classroom-notes.json)記錄原圖 SHA256 與保留區域。圖片筆記與原報告均保留原內容，沒有因程式碼重寫而被丟棄。匿名及格式轉換的檔案不宣稱為原始位元組。

作業說明所搭配的原附件另有[逐案核對清單](assignments/document-companions.md)與[原件映射／SHA256／引用頁碼](assignments/document-companions.json)。2021 PA4／PA5／Final 的原始文字案例、舊公開輸出、四份官方 Q&A 保存副本，以及 2025 四案原始發布紀錄 `說明.txt` 也已補齊；測試生成的資料不冒充原附件。2025 Final 的課程工作站 `cs114_final_judge` 未隨本機資料提供，已明列這項缺件。

## 2021 作業

目前題面與重要修訂差異見 [版本核對紀錄](assignments/version-notes.md)；最新版路徑與 SHA256 見 [specifications.json](assignments/specifications.json)。

| 專案 | 內容 | 完整題目、解答與程式 | 原始 PDF |
|---|---|---|---|
| PA1 | 五倍券找零、加分版票券選擇 | [solution](assignments/2021/pa01/solution.md) | [最新題面](assignments/2021/pa01/originals/2021Fall_EE1003_IntroCS_PA1_Quintuple-Stimulus-Voicher_Andy_20211017_0400.pdf) · [說明與附件](assignments/2021/pa01/originals) |
| PA2 | 遞迴二進位加法、溢位檢查 | [solution](assignments/2021/pa02/solution.md) | [最新題面](assignments/2021/pa02/originals/PA2_20211114_1530_Andy.pdf) · [說明與附件](assignments/2021/pa02/originals) |
| PA3 | 大數加減乘、加分版除法與餘數 | [solution](assignments/2021/pa03/solution.md) | [最新題面](assignments/2021/pa03/originals/PA3_20211123_2330_Andy.pdf) · [說明與附件](assignments/2021/pa03/originals) |
| PA4 | 電阻梯形電路、交換合併、戴維寧等效 | [solution](assignments/2021/pa04/solution.md) | [最新題面](assignments/2021/pa04/originals/PA4_MySpice_HO_20220104_1730.pdf) · [說明與附件](assignments/2021/pa04/originals) |
| PA5 | Quine–McCluskey 與 Petrick 精確最小 SOP | [solution](assignments/2021/pa05/solution.md) | [最新題面](assignments/2021/pa05/originals/PA5_bonus_QM_Andy_20211209_1855.pdf) · [說明與附件](assignments/2021/pa05/originals) |
| Final | 狀態表 → K-map → 邏輯電路、DFF 與明示 T/JK 擴充 | [solution](assignments/2021/final/solution.md) | [最新題面](assignments/2021/final/originals/2021Fall_EE1003_IntroCS_Final_Project_Andy_20211210_0030.pdf) · [說明與附件](assignments/2021/final/originals) |

PA5 的資料只有 QM/Petrick 加分題面，實作完整該題；沒有憑空聲稱另一份基礎題也經核對。2021 Final 沒有獨立加分題面，T/JK 是明示擴充。

## 2025 作業

| 專案 | 內容 | 完整題目、解答與程式 | 原始 PDF |
|---|---|---|---|
| PA1 | Dinosaur Game、雙層場景、自動規劃 | [solution](assignments/2025/pa01/solution.md) | [最新題面](assignments/2025/pa01/originals/2025Fall_EE1003_Intro_to_CS_PA1_Andy_20250925.pdf) · [說明與附件](assignments/2025/pa01/originals) |
| PA2 | Knight's Treasure Hunt、精確回溯、守衛範圍成長 | [solution](assignments/2025/pa02/solution.md) | [最新題面](assignments/2025/pa02/originals/2025Fall_EE1003_Intro_to_CS_PA2_Andy_20251015.pdf) · [說明與附件](assignments/2025/pa02/originals) |
| PA3 | 多進位大數能源分配與平均 | [solution](assignments/2025/pa03/solution.md) | [最新題面](assignments/2025/pa03/originals/2025Fall_EE1003_Intro_to_CS_PA3_Andy_20251126.pdf) · [說明與附件](assignments/2025/pa03/originals) |
| Final | What’s the Number CNN PE 模擬器、FP8、PGM 輸入 | [solution](assignments/2025/final/solution.md) | [最新題面](assignments/2025/final/originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf) · [說明與附件](assignments/2025/final/originals) |

## 官方下載附件與 Windows 下載

2025 的官方下載附件也已完整保存：[原檔與解壓內容 SHA256 清單](assignments/2025/resources.json)。五個 ZIP 原檔在各案 `originals/resources/`，解壓後的檔案在 `course_support/`，可直接使用，無須另找本機資料。

| 專案 | 下載後可用的官方附件 |
|---|---|
| PA1 | [基本、雙層、自動版公開測資與文件](assignments/2025/pa01/course_support/public_testcases)、[Linux checker](assignments/2025/pa01/course_support/checker.out) |
| PA2 | [八份測資、blank starter、makefile、Linux checker](assignments/2025/pa02/course_support/starter)，以及 [Final Explanation](assignments/2025/pa02/originals/2025Fall_EE1003_IntroCS_PA2_Final_Explanation.pdf) |
| Final | [三套完整權重、影像與 golden](assignments/2025/final/course_support/public_testcases)、[PGM／PNG bonus 案例](assignments/2025/final/course_support/bonus_testcases)、[歷史量化結果](assignments/2025/final/course_support/quantization_examples)、[原版硬體 API](assignments/2025/final/course_support) |

PA2 原下載包中的兩份 starter 程式本來就是空檔，供自己填寫；本 repo 的完整解答在各案 `main.cpp`。2025 PA3 沒有提供可下載的官方 OJ 測資，程式相依的 header 與自製測試均已附齊。課程 checker 是 Linux x86-64 ELF，供 Linux／WSL 或課程工作站使用；跨平台驗證指令不依賴這些 binary。官方 checker 的原檔已保存，本次未執行。

Windows 建議在較短的目錄下載，以免原始 PDF 長檔名超過 Windows 的路徑限制；原檔名不需修改：

```powershell
git -c core.longpaths=true clone https://github.com/handsomegary/computer-science-coursework-and-exams.git
cd computer-science-coursework-and-exams
git config core.longpaths true
```

## 2021 考古題

| 題組 | 主題 | 完整重寫題目與解答 |
|---|---|---|
| 小考 1 | 識別字、輸入、比較與賦值 | [solution](exams/2021/quiz01/solution.md) |
| 小考 2 | 偽碼、條件運算子、複合賦值、Euclid | [solution](exams/2021/quiz02/solution.md) |
| 小考 3 | 迴圈、浮點計數、輸出格式 | [solution](exams/2021/quiz03/solution.md) |
| 小考 4 | enum、遞迴、模板、參數 | [solution](exams/2021/quiz04/solution.md) |
| 小考 5 | 陣列、排序、稀疏矩陣 | [solution](exams/2021/quiz05/solution.md) |
| 小考 6 | 檔案、seekg、字串轉數字、struct | [solution](exams/2021/quiz06/solution.md) |
| 期中 | 遞迴、堆疊、質因數、除錯、運算追蹤 | [solution](exams/2021/midterm/solution.md) |
| 期末 | 指標、三維陣列、字串、類別、檔案追蹤 | [solution](exams/2021/final/solution.md) |

## 編譯與驗證

每個資料夾的 README 有單獨編譯與使用方式。需要 C++11 編譯器；所有 Python 工具僅使用標準函式庫，建議 Python 3.12。Python 檔負責獨立測試與解答同步，作業及考古題程式都是 C++。

若要下載後直接跑出並保留結果，使用 [run_example.py](tools/run_example.py)。它從各案 `samples.json` 寫出真正的輸入檔／標準輸入，直接編譯 repo 內的 C++ 原始碼，再保存實際 stdout、stderr、產生檔、編譯紀錄、命令與逐項比對結果。預期答案放在獨立 `expected/`，不會充當程式的實際輸出。

從 repo 根目錄執行：

```sh
python tools/run_example.py --list
python tools/run_example.py assignments/2021/pa04 --example 1 --output-dir build/examples/pa04
python tools/run_example.py assignments/2021/final --all --output-dir build/examples/synthesis
python tools/run_example.py --all-assignments --output-dir build/examples/all-assignments
```

`--list` 列出全部 18 個題組及從 1 開始的範例編號。最後一條指令會執行十個作業的全部已發表範例；不是只印測試通過訊息。結果在指定目錄下的 `<題組>/example-XX/run/`，stdout 與 stderr 在相鄰的 `stdout.txt`／`stderr.txt`；`report.json` 記錄真正的 exit code 與比對。輸出目錄必須尚未存在，重跑時請改用新目錄。可加 `--cxx` 或設定 `CXX` 選擇編譯器；Windows 的 g++ 版本會使用靜態 runtime。`--prepare` 只產生輸入與執行計畫，會明確標示未執行，也不回報通過。

各案手動指令中的 `input.txt`、`input.eqn`、`state_table.txt` 等是自選輸入檔名；範例內容已附在 `samples.json`，可用上面的工具直接取得，不需要本機其他私人資料。2025 官方測資另可依各案 README 的 `course_support/` 路徑直接執行。

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
python tools/sync_solutions.py --check
python tools/check_document_companions.py
python tests/test_solution_book.py
python tests/test_pa2021_01_03.py
python tests/test_pa2021_04_05_final.py
python tests/test_pa2025.py
python tests/test_official_resources_2025.py
python tests/test_quizzes.py
python tests/test_exams.py
```

CMake 產生每個入口各自的執行檔，不將不同 main 連在一起。使用其他編譯器可設定環境變數 `CXX`，測試也接受 `--cxx`。檔案輸入輸出測試在臨時目錄執行。邏輯最小化與騎士不重複路徑是精確組合搜尋，最差時間可能很長；程式不把格式上限說成任意輸入的速度保證。

獨立測試使用 Python 大整數、窮舉小棋盤、nodal 電路解、真值表／coverage DP、狀態機模擬與 CPU CNN 等模型比對。`test_solution_book.py` 另外從解答頁抽出完整程式、重新編譯並執行所有公開範例，因此連同題解內的程式與範例一起檢查。GitHub Actions 在 Linux 執行全部編譯與測試。

## 解答資料格式

`solutions.json` 的每筆問題群包含題目與解析、`solution.code`（主入口全文）、`solution.files`（全部必要原始碼全文）、`solution.variants`（各入口與編譯檔案）及 `examples`（標準輸入輸出、參數、輸入檔、產生檔）。`samples.json` 是精確範例的依據。修改程式或範例後執行 `python tools/sync_solutions.py`，同步所有題解與 JSON；CI 會拒絕未同步的版本。
