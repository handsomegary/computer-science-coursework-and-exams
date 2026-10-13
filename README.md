# 計算機概論 I：作業、考古題與完整程式解答

涵蓋所提供資料的 **10 個作業專案**（2021 六案、2025 四案）、**2021 六份小考的 24 道可辨識題目**、期中與期末考後回憶題。每組題目有重寫規格、推導、完整 C++11 程式與可重現範例。基礎版與加分版分開說明；程式碼直接包含在 `solution.md` 與 [solutions.json](solutions.json) 的 `solution` 欄位中。

原資料中名為「2024」的作業資料夾，其正式文件是 **2025 Fall**，期末更新至 2026-01-01。本 repo 按正式題面的學期整理在 `assignments/2025`。不把不同年度題目混用。

這是依提供資料重新實作的學習解答。考後回憶並非完整正式試卷；缺附件、選項不全、舊答案錯誤、公開範例與後續更新互相衝突的部分，均在各份解答標示重建或補定。沒有上傳原始試卷掃描、個人作答報告或個人識別資料。

## 2021 作業

| 專案 | 內容 | 完整題目、解答與程式 |
|---|---|---|
| PA1 | 五倍券找零、加分版票券選擇 | [solution](assignments/2021/pa01/solution.md) |
| PA2 | 遞迴二進位加法、溢位檢查 | [solution](assignments/2021/pa02/solution.md) |
| PA3 | 大數加減乘、加分版除法與餘數 | [solution](assignments/2021/pa03/solution.md) |
| PA4 | 電阻梯形電路、交換合併、戴維寧等效 | [solution](assignments/2021/pa04/solution.md) |
| PA5 | Quine–McCluskey 與 Petrick 精確最小 SOP | [solution](assignments/2021/pa05/solution.md) |
| Final | 狀態表 → K-map → 邏輯電路、DFF 與明示 T/JK 擴充 | [solution](assignments/2021/final/solution.md) |

PA5 的資料只有 QM/Petrick 加分題面，實作完整該題；沒有憑空聲稱另一份基礎題也經核對。2021 Final 沒有獨立加分題面，T/JK 是明示擴充。

## 2025 作業

| 專案 | 內容 | 完整題目、解答與程式 |
|---|---|---|
| PA1 | Dinosaur Game、雙層場景、自動規劃 | [solution](assignments/2025/pa01/solution.md) |
| PA2 | Knight's Treasure Hunt、精確回溯、守衛範圍成長 | [solution](assignments/2025/pa02/solution.md) |
| PA3 | 多進位大數能源分配與平均 | [solution](assignments/2025/pa03/solution.md) |
| Final | What’s the Number CNN PE 模擬器、FP8、PGM 輸入 | [solution](assignments/2025/final/solution.md) |

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

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
python tools/sync_solutions.py --check
python tests/test_solution_book.py
python tests/test_pa2021_01_03.py
python tests/test_pa2021_04_05_final.py
python tests/test_pa2025.py
python tests/test_quizzes.py
python tests/test_exams.py
```

CMake 產生每個入口各自的執行檔，不將不同 main 連在一起。使用其他編譯器可設定環境變數 `CXX`，測試也接受 `--cxx`。檔案輸入輸出測試在臨時目錄執行。邏輯最小化與騎士不重複路徑是精確組合搜尋，最差時間可能很長；程式不把格式上限說成任意輸入的速度保證。

獨立測試使用 Python 大整數、窮舉小棋盤、nodal 電路解、真值表／coverage DP、狀態機模擬與 CPU CNN 等模型比對。`test_solution_book.py` 另外從解答頁抽出完整程式、重新編譯並執行所有公開範例，因此連同題解內的程式與範例一起檢查。GitHub Actions 在 Linux 執行全部編譯與測試。

## 解答資料格式

`solutions.json` 的每筆問題群包含題目與解析、`solution.code`（主入口全文）、`solution.files`（全部必要原始碼全文）、`solution.variants`（各入口與編譯檔案）及 `examples`（標準輸入輸出、參數、輸入檔、產生檔）。`samples.json` 是精確範例的依據。修改程式或範例後執行 `python tools/sync_solutions.py`，同步所有題解與 JSON；CI 會拒絕未同步的版本。
