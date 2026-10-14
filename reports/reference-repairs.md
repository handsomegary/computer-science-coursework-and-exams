# 匿名參考程式修復與重新驗證

本次修復 **13 份**不符最新版要求或有錯誤的 2021 參考程式，並同時保存 **13 份有誤原版**。使用隨機代稱與通用檔名，沒有公開姓名、學號或對照表；原版 C++ 位元組完整保留。原先通過的 PA1 兩份程式仍保留。主要重新實作解答沒有被參考程式取代。

最新版以[提供資料的版本核對](../assignments/version-notes.md)及課程補充／Q&A 為準。每份的原版、修正版、最新題面 SHA256 與測試統計見 [reference-repairs.json](reference-repairs.json)；下表連到各版本說明。

| 作業／代稱／版本 | 原版主要問題 | 修復與驗證 |
|---|---|---|
| [pa01 / jade-lynx-c0ee / basic](../assignments/2021/pa01/references/jade-lynx-c0ee/README.md) | 拒絕合法零元交易；初始條件讀取未初始化價格。 | 初始化價格並接受 0，保留原票券枚舉與庫存更新。 **306/306** 檢查通過。 |
| [pa01 / moss-lynx-5375 / early](../assignments/2021/pa01/references/moss-lynx-5375/README.md) | 早期稿含 system("pause")、不穩定輸入控制與不符題面提示；初次僅編譯和靜態檢查。 | 移除 pause，修正輸入、零元及提示，保留原付款方法。 **184/184** 檢查通過。 |
| [pa01 / jade-lynx-c0ee / return_change](../assignments/2021/pa01/references/jade-lynx-c0ee/README.md) | 同樣拒絕 0、讀取未初始化價格；輸入失敗缺少復原。 | 修正交易控制與輸入復原，保留原找零枚舉；驗證票券可用性及金額守恆。 **328/328** 檢查通過。 |
| [pa02 / jade-lynx-c0ee / basic](../assignments/2021/pa02/references/jade-lynx-c0ee/README.md) | 第 21／22／23 次提示用 th；範圍檢查和第 19 位遞迴乘 10 有有號溢位風險。 | 修正序數、合法範圍檢查及有界遞迴進位。 **298/298** 檢查通過。 |
| [pa02 / moss-lynx-5375 / basic](../assignments/2021/pa02/references/moss-lynx-5375/README.md) | 十進位輸入用 long long、以十進位 a += b 算累加值，違反最新限制；第 19 位 c*10 溢位。 | 輸入改用 int；轉換、位元全加器及回轉換均遞迴，使用有界 19 位容器。 **298/298** 檢查通過。 |
| [pa02 / moss-lynx-5375 / signed](../assignments/2021/pa02/references/moss-lynx-5375/README.md) | 混合正負數錯誤，例如 −11 印成 1100101；static、浮點判斷、非遞迴負數轉換和十進位累加違反限制。 | 修正二補數、符號延伸、零和與溢位；採遞迴有界位元容器，另提供舊範例寬度相容模式。 **366/366** 檢查通過。 |
| [pa03 / moss-lynx-5375 / basic](../assignments/2021/pa03/references/moss-lynx-5375/README.md) | 1−2 得到 99；借位和負差值符號錯誤。 | 保留十進位陣列，修正大小比較、借位、符號及輸出容量。 **1424/1424** 檢查通過。 |
| [pa03 / moss-lynx-5375 / division](../assignments/2021/pa03/references/moss-lynx-5375/README.md) | 負差值、相等減法和除法邊界失敗；1/2、7/7、100 位數/1 有反例。 | 修正共用加減乘；以十進位長除法重整商與餘數流程。 **2058/2058** 檢查通過。 |
| [pa03 / cobalt-heron-2230 / basic](../assignments/2021/pa03/references/cobalt-heron-2230/README.md) | 1−2 輸出 +1；100 個 9 加 1 得到 0；乘積截為低 100 位。 | 修正符號並保留 101 位和、200 位乘積與完整整數平均。 **1424/1424** 檢查通過。 |
| [pa04 / jade-lynx-c0ee / basic](../assignments/2021/pa04/references/jade-lynx-c0ee/README.md) | 兩位數 stage ID 只讀首位；合法 15 stage 崩潰；單 stage 索引、零／負值格式與重複載入有問題。 | 保留 ladder reduction 和相鄰合併；重整 ID／電阻解析、容量、輸出與載入狀態。 **64/64** 檢查通過。 |
| [pa04 / moss-lynx-5375 / basic](../assignments/2021/pa04/references/moss-lynx-5375/README.md) | 無前綴 Ω、u／G 電阻解析失敗或崩潰；格式函式缺回傳，合併索引和載入狀態不穩。 | 正確解析合法前綴與浮點電阻；修正合併、格式、重載及直接匯出。 **64/64** 檢查通過。 |
| [pa04 / moss-lynx-5375 / thevenin](../assignments/2021/pa04/references/moss-lynx-5375/README.md) | 同樣的電阻解析／輸出問題；操作後戴維寧結果可能沿用舊狀態。 | 修正一般版並依新電路重算戴維寧等效，以獨立 KCL 與物理模型驗證。 **64/64** 檢查通過。 |
| [final / jade-lynx-c0ee / basic](../assignments/2021/final/references/jade-lynx-c0ee/README.md) | 二變數 Gray-order 錯誤；常數 0／1、短行／容器越界、VLA、線名衝突及檔名別名可造成錯誤。 | 保留課程 K-map 流程，重整 section／真值表解析與 BENCH；處理常數、唯一線名和檔案身分檢查。 **254/254** 檢查通過。 |

## 驗證範圍與結果

- **PA1／PA2：** `-O0`／`-O2` 合計 1,780 次通過；有號 PA2 舊範例相容模式另 366 次。PA1 另枚舉／固定種子產生 1,331 組任意錢包狀態，共 2,662 次付款性質檢查通過。包括原先所有適用案例、公開範例、邊界和連續交易。
- **PA3：** 三份來源於 `-O0`／`-O2` 共 4,906 次通過，使用 Python 大整數、整數平均與 `divmod`；原版另重現 10 個失敗證據。涵蓋 100 位輸入、101 位和、200 位乘積及除法邊界。
- **PA4：** 三份來源共 192 個情境紀錄、195 次程式執行通過；90 位 Decimal nodal KCL 模型核對電壓／電流與戴維寧等效。包含公開六例、15 stage、各合法前綴、操作後直接匯出、重載及極小／零／負值；原版各重現一個錯誤。
- **Final：** `-O0`／`-O2` 各 127 組通過。每個構建在產生的 BENCH 逐列驗證全部 65,808 個 2～4 變數輸出函數，並核對 ports／唯一驅動線名。包含二變數 Gray-order、常數、多字元名稱、亂序欄位、零／一變數及 10 種錯誤時檔案保持不變的情境。

使用 GCC 14.2.0、標準 C++11 與嚴格警告；修正版零編譯警告。只執行本次從原始碼重新編譯的程式。原版失敗與修正版通過分開標記：PA1 早期稿原版只做靜態檢查；Final 原版正常構建曾通過 19/28，另啟用容器 assertion 的構建暴露越界後為 0/28。這些紀錄不代表課程成績，也不是所有可能輸入的形式化證明。

## 題面矛盾與來源限制

PA2 最新題面要求負數「最少位數」，舊範例卻令 −1=`11`、−4=`1100`。修正版預設真正最短二補數（−1=`1`、−4=`100`），另以 `-DCOURSE_WIDTH=1` 提供範例寬度相容模式，兩者各自驗證。PA4 依官方有序 ladder netlist 格式與物理電路運算；不宣稱支援任意 SPICE，舊加分範例有不一致數值時不將錯值作為判準。

Final 保留課程原版 K-map 模組，確認邏輯等價，但沿用[不保證精確最小 SOP 的限制](../assignments/2021/final/course_support/README.md)。其中一份 PA3 的來源標示不一致，使用第三個代稱 `cobalt-heron-2230`，沒有斷言作者。

## 重跑方式

在儲存庫根目錄執行（Python 僅需標準函式庫）：

```sh
python tests/test_repaired_pa1_pa2.py
python tests/test_repaired_pa1_pa2.py --course-width --only-signed
python tests/test_repaired_pa1_wallets.py
python tests/test_repaired_pa3.py
python tests/test_repaired_pa4.py
python tests/test_repaired_final.py --exhaustive
```

可用 `--cxx` 或 `CXX` 指定編譯器。PA3、Final 可加 `--with-originals`，PA4 可加 `--originals` 重現已知失敗；一般 CI 只把修正版當通過項目。初次尚未修復的審查見[歷史驗證紀錄](reference-validation.md)。
