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

公開 ZIP 是原資料中的測試依據，完整權重與圖像沒有複製到此 repo。repo 內的驗證使用可重現的合成資料與獨立 CPU oracle，涵蓋三種 kernel、signed 卷積、FC、輸出格式及 bonus 插值。

## 原始作業 PDF

以下 PDF 保留來源檔名與原始內容；同一題目的不同修訂版分別保存。

| 類型 | 原始檔案 |
|---|---|
| 作業題面／修訂版 | [2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf](originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Andy_20260101.pdf) |
| 補充說明 | [2025Fall_EE1003_Final_Project_Appendix1_Header_File_20251125.pdf](originals/2025Fall_EE1003_Final_Project_Appendix1_Header_File_20251125.pdf) |
| 補充說明 | [2025Fall_EE1003_Intro_to_CS_Final_Project_Checker_20251224.pdf](originals/2025Fall_EE1003_Intro_to_CS_Final_Project_Checker_20251224.pdf) |
