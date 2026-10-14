# 作業說明版本核對

目前版本以本次提供的課程文件為範圍，依內文修訂紀錄、內容差異與 PDF 日期交叉判定。完整目前題面清單與 SHA256 見 [specifications.json](specifications.json)；每個作業 README 直接連到目前題面，八份舊題面保存在各自的 `originals/archive/`。

| 作業 | 核對後採用的更新與重要規則 |
|---|---|
| [2021 PA1](2021/pa01/README.md) | 採 10/17 V2。9/30 的 V2 與 10/17 版運算規格相同，10/17 修正報告檔名範例。一般版仍遵守大面額優先；最少付款損失屬 bonus。零元付款由補充案例確認。 |
| [2021 PA2](2021/pa02/README.md) | 採 11/14 版。10/28 起要求十進位→二進位、二進位加法、二進位→十進位三個遞迴函式，禁止 global／static；結果不能直接用十進位加減算出。Q&A 允許十進位加減僅用於偵測溢位。11/14 修正提示與範例位元。 |
| [2021 PA3](2021/pa03/README.md) | 採 11/23 版；此版移除「64 bits＝4 bytes」誤植。正整數運算元最多 100 位，乘積可能達 200 位；每次運算都須輸出截尾的兩數平均。 |
| [2021 PA4](2021/pa04/README.md) | 採 2022/1/4 版，仍屬 2021 Fall。12/28 規定每條 RS／RP 路徑 1～15 個電阻，1/4 規定 1～15 個 stage。RP 支路內的電阻依序串接。Q&A 規定相鄰 stage 才可交換／合併，包含中段合併；工程記號使用 `u,m,K,Meg,G` 或無前綴。 |
| [2021 PA5](2021/pa05/README.md) | 提供資料只有 12/9 命名的 QM/Petrick bonus 題面。1/14 Q&A 公告公開輸出重複項已修正；本次提供的 testcase 2、3 PDF 日期已為 2022/1/14，保留這兩份修正版。 |
| [2021 Final](2021/final/README.md) | 題面搭配 2022/1/12 補充說明與 Q&A。present state＋input 最多四個變數，output 數量可為零；檔名由 argv 取得、以檔案輸入輸出。課程提供的 `kmap.cpp` 以 `top()` 整合，原版與驗證結果見 [course_support](2021/final/course_support/README.md)。 |
| [2025 PA1](2025/pa01/README.md) | 提供的 9/25 命名題面列 version 3（9/21），搭配 9/27 Common Problem、Checker、OJ 與 Workstation 說明。 |
| [2025 PA2](2025/pa02/README.md) | 題面為 10/15 版，另搭配 10/27 Detailed Explanation／Checker 與 11/3 Hint。 |
| [2025 PA3](2025/pa03/README.md) | 提供的題面為 11/26 版；驗證與排版規則以該檔為準。 |
| [2025 Final](2025/final/README.md) | 採 version 3（2026/1/1），另搭配 Appendix 與 12/24 Checker。 |

課程公告中的舊範例可能互相矛盾，因此程式驗證同時依明文規格與獨立數學模型判斷；公開範例通過不等於全部規格通過。
