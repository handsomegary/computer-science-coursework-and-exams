# 作業說明所提及的原附件

本次從十個作業的完整題面、補充文件、公開案例與公告核對檔案參照。原附件與重新實作的解答、測試產生器分開保存。[document-companions.json](document-companions.json) 記錄全部 107 份原附件／ZIP 成員的原來源映射、用途、大小、SHA256、引用文件與頁碼／段落／行號，另核對 65 份目前保留的 PDF／DOCX／原始發布紀錄（主題面各年度／作業只留最新版）；每個文件中的檔名參照均有分類。這不是把測試產生的資料當成原下載檔案。

| 作業 | 本機原附件的保存範圍 |
|---|---|
| 2021 PA1／PA3 | 題面與公開案例 PDF 已保留；提供資料沒有另附官方 starter／header／輸入 ZIP。題面中的學生提交檔名是要自己建立的程式，不是漏掉的原附件。 |
| 2021 PA2 | 題面、公開案例與 [11/04 Q&A 及 11/02、10/28 修訂公告的原保存副本](2021/pa02/originals/PA2%20Positive%20Binary%20Adder.docx)。較早公告不覆蓋 11/14 最新題面；沒有另附官方 starter／header／輸入 ZIP。 |
| 2021 PA4 | [input1／2／3、output1／2／3、bonus1／2](2021/pa04/course_support/public_testcases)、[middle merge 的原 input.txt](2021/pa04/course_support/public_testcases/merge_middle/input.txt)、[公告與 Q&A](2021/pa04/originals/PA4相關問答.docx)。 |
| 2021 PA5 | [三套 testX.eqn／testXout.eqn 原件](2021/pa05/course_support/public_testcases)、[含 1/14 公開輸出修正公告的 Q&A](2021/pa05/originals/PA5相關問題.docx)。 |
| 2021 Final | [test/state_table_1／2.txt 及原名 circuit_1／2.bench](2021/final/course_support/public_testcases)、[課程原版 kmap.cpp](2021/final/course_support/kmap.cpp)、[官方 Q&A 的本機保存副本](2021/final/originals/相關問題.docx)。 |
| 2025 PA1 | Public Testcases.zip、完整解壓輸入／輸出／PDF／DOCX、Linux checker 原件。 |
| 2025 PA2 | PA2_STUDENT.zip、原 makefile、checker、八份輸入、兩份原本為空檔的 starter、Final Explanation。 |
| 2025 PA3 | 題面內的範例已保留；提供資料沒有另附官方 OJ 測資 ZIP 或 checker。 |
| 2025 Final | 三個 ZIP 原件與全部權重、bias、影像、公開 golden、bonus PGM／PNG、歷史量化結果，以及原版硬體 API .h／.cpp。 |

2025 的各壓縮包成員與空結果目錄的 `.gitkeep` 清單另見 [resources.json](2025/resources.json)。`.gitkeep` 是為 Git 保存空目錄而新增的標記，不是原下載包檔案。四份 2021 Q&A Word 是提供資料中本機保存的官方公告副本，完整保留原 bytes 與匯出 metadata；不宣稱是教師原生發布的 DOCX。

2025 四個作業根目錄的原始 `說明.txt` 也按原名保存：[PA1](2025/pa01/originals/release_notes/說明.txt)、[PA2](2025/pa02/originals/release_notes/說明.txt)、[PA3](2025/pa03/originals/release_notes/說明.txt)、[Final](2025/final/originals/release_notes/說明.txt)。它們包含題面／ZIP／API 的發布與更新紀錄；Final 的 12/22 flush-subnormal、12/25 hardware API 更新也完整保留。課程影片、OJ、選用的 `/usr/local/bin/enable-opencv` 環境設定以 `remote_external` 分類，沒有補造本機影片、OJ 題庫或 workstation script，也不將它們列為此重寫版執行所需檔案。

本清單只核對正式說明與附件；非編譯的本地筆記、草稿與原學生報告另保存於 [source_archive](../source_archive/README.txt)，沒有因重寫解答不使用就略去。`Doc1.docx` 以原位元組保存並標本地筆記；參考網站文件的全部可見內容／三個網址保存為去除作者 metadata 的 TXT。它們不冒充官方必要附件。正式附件 manifest 的排除紀錄只表示不屬本清單，並指向另存分類。

原始輸出可能有舊格式或數值限制，manifest 明列 `historical_expected`：PA4 bonus 的原 Rth／Vth 使用舊公開表定義；PA4 的 K/k、空格及換行格式不一致；PA5 可以有等價的不同排列；Final 的 benchmark 使用課程格式與隱含 CLK；2025 歷史量化與能耗／step golden 不代表最新版的量化規則或更佳排程必須使用相同數字。原 bytes 保留，正確性由各案最新版說明與獨立驗證判定。

`kmap_in.txt`、`kmap_out.txt`、`result.txt` 等通常是執行後產生的檔案。`state_table.txt`／`input.txt` 也可能是題面的示意輸入檔名；原 public case 名稱已保留，manifest 給出對應路徑。PA4 公開流程中的 `myspice.txt` 故意不存在，用來示範重新輸入，沒有補造這個檔案。Middle merge 的輸出在 PDF 第 3 頁；本機學生 `output.txt` 數值不同，因此未冒充官方 golden 上傳。

仍然未提供的原工具是 **2025 Final 的 `cs114_final_judge`**：Checker 文件第 2 頁要求在課程工作站執行，但本機提供的資料沒有這個 binary/script。此項以 `not_provided` 明列；repo 的本機 CNN、公開案例及獨立測試可以執行，不能將它們說成原工作站 grader。

從 repo 根目錄可核對下載後的完整性：

```sh
python tools/check_document_companions.py
```

工具檢查每個附件與說明的路徑／SHA256、檔名參照分類、引用位置及明列的缺件。通過表示已保存的原件完整；會同時印出 `NOT PROVIDED`，不把未提供的工具藏在通過訊息中。
