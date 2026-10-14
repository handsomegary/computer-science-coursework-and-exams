# Final：CNN Accelerator Simulator（2025）

[完整題目、解法、硬體介面與程式碼](solution.md)。使用課程提供的 3×3 PE API 完成兩層卷積，包含 FP8 量化、FC、實際硬體能耗／週期與 PGM 前處理 bonus。

```sh
g++ -std=c++11 -O2 main.cpp hardware_architecture.cpp -o cnn
./cnn input_directory output_directory
g++ -std=c++11 -O2 bonus.cpp -o preprocess
./preprocess image.pgm img_label_bonus.txt
```

輸入與輸出資料夾必須已存在；省略參數表示目前目錄。CNN 讀入七個固定名稱的權重／影像檔，寫出四個結果檔。bonus 選擇規格允許的 ASCII P2 PGM，不需要 OpenCV；自行實作雙線性 resize、顏色反轉、正規化與 FP8 轉換。

以 2026-01-01 更新的作業規格與 2025-12-22 說明為準：`abs(x)<2^-6` 的值 flush 至 0。舊 bonus ZIP 文字圖出現 1/127 倍數且保留 subnormal，與此規則不一致；此處依最新 FP8 規格，不把舊文字圖當作新版 quantization 的 golden。

三套官方基本測資的 conv1、conv2、FC 逐值與公開 golden 完全一致，量化誤差與預測也一致。實際 PE API 回報如下；沒有以 CPU 卷積代替 PE 後另算假的能耗。

| 公開案例 | kernel | 新能耗 | 新 steps | 公開 golden 能耗 | 公開 golden steps |
|---|---:|---:|---:|---:|---:|
| 01 | 3 | 75170.0 | 3130 | 192290.0 | 6408 |
| 02 | 7 | 294825.0 | 14130 | 697160.0 | 23232 |
| 03 | 5 | 187400.0 | 8320 | 946380.0 | 31536 |

三個官方 ZIP 原檔與完整解壓內容已提供：基本案例的全部權重／影像／golden、bonus PGM／PNG／歷史文字圖，以及歷史量化結果都在 `course_support/`。原名、位置、用途與 SHA256 見 [resources.json](../resources.json)；包內 `Bouns`／`bouns` 拼字也完整保留。另有可重現合成資料與獨立 CPU oracle，涵蓋三種 kernel、signed 卷積、FC、輸出格式及 bonus 插值。

## 官方下載附件與可直接執行的例子

| 原版下載包 | 解壓後內容 |
|---|---|
| [Final_Project_Public_Testcase.zip](originals/resources/Final_Project_Public_Testcase.zip) | [三套 CNN 輸入與原 golden](course_support/public_testcases) |
| [Final_Project_Bouns_Public_Testcase.zip](originals/resources/Final_Project_Bouns_Public_Testcase.zip) | [三套 P2 PGM、PNG 及歷史量化文字圖](course_support/bonus_testcases) |
| [Data after Quantization.zip](originals/resources/Data%20after%20Quantization.zip) | [三套歷史量化結果](course_support/quantization_examples) |

在本目錄編譯後，建立自己的輸出目錄，避免覆寫保存的 golden：

```sh
mkdir output01
./cnn course_support/public_testcases/Testcase01 output01
./preprocess course_support/bonus_testcases/Testcase01/img_label.pgm output01/img_label_bonus.txt
```

PowerShell 改用 `.\cnn.exe` 與 `.\preprocess.exe`。Testcase02、Testcase03 可用同樣方式執行；輸入資料夾各有七個必要檔案，CNN 輸出四個結果檔。

課程 PE API 的 byte-exact 原檔保存在 [course_support/hardware_architecture.h](course_support/hardware_architecture.h) 與 [course_support/hardware_architecture.cpp](course_support/hardware_architecture.cpp)。本目錄可直接編譯的 API 副本只移除原版未使用的區域變數，介面及運算行為相同；不需額外下載 header。

從 repo 根目錄執行 `python tests/test_official_resources_2025.py`，會檢查所有原檔 hash，逐值比對三套官方 conv1／conv2／FC golden，核對量化誤差與預測，並以最新版 FP8／雙線性插值 oracle 核對三張官方 PGM。能耗與 steps 是目前 PE 排程的實際 API 計數，與舊 golden 的較慢排程不同。歷史量化文字圖保留作比較，不作為最新版 bonus 的正確答案。

## 作業說明（目前版本）

以本次提供資料中最新的[作業題面（2026-01-01）](originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf)為準。內文 version 3（2026-01-01）；搭配 12/24 Checker 與 Appendix。

原始檔名與 PDF 內容完整保留。後續補充說明／Q&A 與題面一起核對；舊題面另存於歷史版本，不作為目前預設規格。

| 類型 | 原始檔案 |
|---|---|
| 目前題面 | [2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf](originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf) |
| 補充說明 | [2025Fall_EE1003_Final_Project_Appendix1_Header_File_20251125.pdf](originals/2025Fall_EE1003_Final_Project_Appendix1_Header_File_20251125.pdf) |
| 補充說明 | [2025Fall_EE1003_Intro_to_CS_Final_Project_Checker_20251224.pdf](originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Checker_20251224.pdf) |
