# Final：What's the Number? CNN Accelerator Simulator

## 題目

以 C++11 實作 CNN 硬體行為模擬器。輸入是 28×28 浮點灰階圖，先量化成 FP8，再經兩層卷積與 fully connected 層，輸出 0..9 的分類預測、量化誤差、硬體能耗與計算 steps。

兩層卷積皆無 padding，stride 固定為 2，kernel 大小 K 為 3、5 或 7，單一測資兩層 K 相同。conv1 是 1→2 channels，conv2 是 2→4 channels；每輸出格對各輸入 channel 的 K×K 權重作 cross-correlation 再加 bias。不存在額外 ReLU 或 pooling 要求，不自動加入。輸出寬度分別是 `H1=(28-K)/2+1`、`H2=(H1-K)/2+1`，整數除法取 floor。

卷積必須使用課程的 `hardware_architecture.h/.cpp`。提供模組含 3×3 PE、兩條長 28 的 line buffer，weights 留在 PE、activation 向右傳、partial sums 向下傳。FC 可自行用 CPU 計算。

## 量化規則

FP8 使用 sign 1 bit、exponent 4 bits、mantissa 3 bits，exponent bias=7。先將 FP32 拆成符號與二進位 exponent，再將 mantissa 捨入到最近 1/8（半位向上）；捨入進位時調整 exponent。`abs(x)<0.015625` flush 至 0，不支援 subnormal；0 正規化為正零。大於有限最大正常值 240 時飽和至帶符號的 240，此為對一般輸入補充的界線；正式影像通常是 0..1。

每個非零像素的 relative error 是 `abs(original-decoded)/abs(original)`，原值為 0 的誤差定義為 0。所有 784 個像素平均後乘 100%，四捨五入到兩位小數。只有影像像素做 FP8 量化；權重、bias 與中間結果保持 FP32。

採最新 2026-01-01 版本與 subnormal 禁用公告。舊 bonus 文字範例含 1/127 倍數與小於 2^-6 的非零值，非此 E4M3 normal 規則；本解答依最新文字要求，而非沿用衝突資料。

## 輸入檔案

`cnn [input_directory] [output_directory]`，省略時採目前工作目錄；資料夾須已建立。各檔讀取逗號或空白分隔的有限 float，資料筆數必須符合以下形狀：

| 檔名 | 內容與順序 |
|---|---|
| img_label.txt | 28×28 像素，row-major |
| conv1_weight.txt | 2 組 K×K，各組是一個 output channel |
| conv1_bias.txt | 2 個 bias |
| conv2_weight.txt | 8 組 K×K，順序 output0/input0、output0/input1、output1/input0…… |
| conv2_bias.txt | 4 個 bias |
| fc_weight.txt | 10 組 M 權重，M=4×H2×H2，每組對應一個分類 |
| fc_bias.txt | 10 個 bias |

FC 輸入依 channel-major，再依每個 feature map row-major 攤平。FC 輸出為 bias + 所有 M 個 feature×weight，最大值對應預測；相同值取較小分類索引。

格式錯誤、缺檔、非有限數、kernel/資料筆數不符、輸出資料夾不存在均回傳 1 並在 stderr 顯示原因。推論本身不讀 stdin，不在 stdout 印額外訊息。

## 輸出檔案

`result_of_conv1.txt` 與 `result_of_conv2.txt` 按 channel 順序逐行寫出 feature maps，行內逗號分隔，最後一值無逗號，各值截斷至三位小數（向零截斷），固定保留三位。負數也向零截斷，不用一般四捨五入。`result_of_fc.txt` 是一行 10 值，格式相同。

`execution_result.txt` 依序為以下四行：

```text
Relative Error after Quantization: <兩位小數>%
Result of the Prediction: <0..9>
Energy Consumption: <一位小數>
Step Count: <整數>
```

能耗與 steps 必須直接呼叫 `return_energy()`、`return_step()`，包含兩層卷積，不包含 CPU FC。模組每次 `set_weight` 收 5、每條外部輸入收 10、line buffer 輸入收 2.5；注意本發佈版每次載入整個 weight matrix 收 5，依提供程式的實際行為，不自行改成每個 weight 收 5。

## Bonus：PGM 影像前處理

`preprocess image.pgm [img_label_bonus.txt]` 選擇正式允許的 ASCII P2 格式，無需 OpenCV。P2 後是 width、height、最大像素值與 width×height 個整數灰階值，可以有 # 註解。補定尺寸各最多 4096、最大灰階值 1..65535，像素須在 0..maximum；至少應能处理官方的 200×200 圖。

自行在 pixel center 做雙線性 resize 到 28×28，超出影像邊緣採 clamp。反轉並除最大灰階值，得到 `1-gray/maximum`，再用同一 FP8 conversion。最後寫 28 行，每行 28 值，逗號分隔，保留十位小數。沒有加入規格未要求的二值化 threshold；若要辨識 bonus 結果，可將此文件作為主程式的 img_label.txt。

原說明中 bonus/bouns 拼字並存，此處預設統一為 `img_label_bonus.txt`，使用第二個參數可自訂輸出名。

## 解法：真實 PE 映射

將 K×K kernel 分成若干最多 3×3 blocks，缺少的權重以 0 補滿。對每個 output/input channel 與 kernel block，重設提供模組、載入一次 weights，再遍歷整個 output map，維持 weight-stationary。

對一個 block，row r 的輸入在 tick t 取該 block 的 column `2-t+r`。超出 0..2 或 kernel 範圍時送 0，連續執行 t=0..4 五個 cycles。由水平 activation 與垂直 psum 的延遲可得：在 t=4 時，最底列第 j 個 PE 收到各 row 的指定第 j 欄，故 bottom PE 6、7、8 各保存該欄的三列乘加結果。把這三個 PE 的輸出相加，就得到整個 block 的點積。

每個 output 像素都按此節奏送入，其第 4 tick 的最底列僅依赖当前五個 ticks，不需要每個像素重載權重。邊界 block 中未用的 row/column 都是 zero weight 或 zero activation，不產生多餘值。累加所有 blocks 與 input channels，最後即為完整 K×K卷積加 bias。沒有另一份 CPU 卷積作為真正答案。

在前兩條輸入需要送零時，掃描 line buffer 找出實際存有 0 的 slot，存在就從 buffer 輸入，沒有則從外部送零。從未偽造 buffer 資料或更改模組的 energy/step counters。

課程提供 hardware 檔保留全部 API、矩陣更新、能耗、cycle 與 callable check 行為，僅移除一個從未使用的局部 `sum` 宣告，以消除原檔編譯警告。供課程 judge 使用時，可換回原始未修改的 hardware 檔，數值與計數完全相同。

## 完成與驗證報告

基本推論、正確 FP8 flush、完整文件輸出、硬體指標與 PGM bonus 均完成。主要困難是較大 kernel 的 block 映射與 PE pipeline 時序；以獨立 CPU convolution oracle 和課程公開中間結果分層核對，避免分類恰好正確卻中間卷積錯誤。

三套官方 base 的 conv1/conv2/FC 全數值逐項一致、relative error 與预测一致。使用不同合成signed weights的測試另覆蓋 K=3/5/7，並檢查真實 steps 式 `5×ceil(K/3)^2×(2H1²+8H2²)`；PGM bonus 另用獨立插值及量化 oracle 檢查。這些是本地／CI 驗證，不是課程 checker 上的成績或排名。

## 範例

<!-- SAMPLES_BEGIN -->

### 範例 1（`main.cpp`）

輸入檔 `img_label.txt`：
```text
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
```

輸入檔 `conv1_weight.txt`：
```text
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
```

輸入檔 `conv2_weight.txt`：
```text
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
```

輸入檔 `conv1_bias.txt`：
```text
1,2
```

輸入檔 `conv2_bias.txt`：
```text
3,4,5,6
```

輸入檔 `fc_weight.txt`：
```text
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
```

輸入檔 `fc_bias.txt`：
```text
0,1,2,3,4,5,6,7,8,9
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `result_of_conv1.txt`：
```text
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000,1.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000,2.000
```

產生檔 `result_of_conv2.txt`：
```text
3.000,3.000,3.000,3.000,3.000,3.000
3.000,3.000,3.000,3.000,3.000,3.000
3.000,3.000,3.000,3.000,3.000,3.000
3.000,3.000,3.000,3.000,3.000,3.000
3.000,3.000,3.000,3.000,3.000,3.000
3.000,3.000,3.000,3.000,3.000,3.000
4.000,4.000,4.000,4.000,4.000,4.000
4.000,4.000,4.000,4.000,4.000,4.000
4.000,4.000,4.000,4.000,4.000,4.000
4.000,4.000,4.000,4.000,4.000,4.000
4.000,4.000,4.000,4.000,4.000,4.000
4.000,4.000,4.000,4.000,4.000,4.000
5.000,5.000,5.000,5.000,5.000,5.000
5.000,5.000,5.000,5.000,5.000,5.000
5.000,5.000,5.000,5.000,5.000,5.000
5.000,5.000,5.000,5.000,5.000,5.000
5.000,5.000,5.000,5.000,5.000,5.000
5.000,5.000,5.000,5.000,5.000,5.000
6.000,6.000,6.000,6.000,6.000,6.000
6.000,6.000,6.000,6.000,6.000,6.000
6.000,6.000,6.000,6.000,6.000,6.000
6.000,6.000,6.000,6.000,6.000,6.000
6.000,6.000,6.000,6.000,6.000,6.000
6.000,6.000,6.000,6.000,6.000,6.000
```

產生檔 `result_of_fc.txt`：
```text
0.000,1.000,2.000,3.000,4.000,5.000,6.000,7.000,8.000,9.000
```

產生檔 `execution_result.txt`：
```text
Relative Error after Quantization: 0.00%
Result of the Prediction: 9
Energy Consumption: 75170.0
Step Count: 3130
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

### 範例 2（`bonus.cpp`）

執行參數：`image.pgm img_label_bonus.txt`

輸入檔 `image.pgm`：
```text
P2
# one white pixel
1 1
255
255
```

標準輸入：
```text

```

標準輸出：
```text

```

產生檔 `img_label_bonus.txt`：
```text
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000,0.0000000000
```

結束碼：`0`。精確換行、空白及檔案內容以 `samples.json` 為準。

<!-- SAMPLES_END -->

## 完整 solution 程式碼

<!-- SOLUTION_CODE_BEGIN -->

### `bonus.cpp`

```cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "quantization.h"

void token(std::ifstream& input, char text[64]) {
    char c;
    while (input.get(c)) {
        if (c == '#') { while (input.get(c) && c != '\n') {} continue; }
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        int size = 0;
        do {
            if (size >= 63) throw "PGM token too long";
            text[size++] = c;
        } while (input.get(c) && c != ' ' && c != '\t' && c != '\r' && c != '\n');
        text[size] = '\0'; return;
    }
    throw "Truncated PGM input";
}
int number(std::ifstream& input) {
    char text[64]; token(input, text);
    long long value = 0;
    for (int i = 0; text[i]; ++i) {
        if (text[i] < '0' || text[i] > '9') throw "Invalid PGM number";
        value = value * 10 + text[i] - '0';
        if (value > 2147483647) throw "PGM number too large";
    }
    return static_cast<int>(value);
}
int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) { std::cerr << "Usage: preprocess image.pgm [img_label_bonus.txt]\n"; return 1; }
    try {
        std::ifstream input(argv[1]); if (!input) throw "Cannot open PGM image";
        char magic[64]; token(input, magic);
        if (magic[0] != 'P' || magic[1] != '2' || magic[2]) throw "Only ASCII P2 PGM is supported";
        int width = number(input), height = number(input), maximum = number(input);
        if (width <= 0 || height <= 0 || width > 4096 || height > 4096 || maximum <= 0 || maximum > 65535) throw "Invalid PGM dimensions or range";
        std::vector<int> pixels(width * height);
        for (int& pixel : pixels) { pixel = number(input); if (pixel < 0 || pixel > maximum) throw "Invalid pixel"; }
        std::ofstream output(argc == 3 ? argv[2] : "img_label_bonus.txt"); if (!output) throw "Cannot create output";
        output << std::fixed << std::setprecision(10);
        for (int r = 0; r < 28; ++r) {
            for (int c = 0; c < 28; ++c) {
                // Bilinear interpolation at pixel centers, clamped at the edges.
                double y = (r + 0.5) * height / 28.0 - 0.5, x = (c + 0.5) * width / 28.0 - 0.5;
                if (y < 0) y = 0;
                if (y > height - 1) y = height - 1;
                if (x < 0) x = 0;
                if (x > width - 1) x = width - 1;
                int y0 = static_cast<int>(y), x0 = static_cast<int>(x), y1 = y0 + (y0 + 1 < height), x1 = x0 + (x0 + 1 < width);
                double fy = y - y0, fx = x - x0;
                double gray = (1 - fy) * ((1 - fx) * pixels[y0 * width + x0] + fx * pixels[y0 * width + x1]) + fy * ((1 - fx) * pixels[y1 * width + x0] + fx * pixels[y1 * width + x1]);
                float normalized = static_cast<float>(1 - gray / maximum);
                if (c) output << ',';
                output << quantize(normalized);
            }
            output << '\n';
        }
    } catch (const char* error) { std::cerr << error << '\n'; return 1; }
    catch (...) { std::cerr << "Invalid PGM input\n"; return 1; }
}
```

### `hardware_architecture.cpp`

```cpp
#include <iostream>
#include <fstream>
#include "hardware_architecture.h"
using namespace std;

static float input_matrix[3][3];
static float psum_matrix[3][3];
static float weight_matrix[3][3];
static float output_matrix[3][3];
static float line_buffer_array_1[28];
static float line_buffer_array_2[28];
static float energy_counter = 0;
static int step_counter = 0;



// This function might be called randomly to verify that you are using this header file.
// Function you can't call in your main.cpp
void output(int number) {
    cout << "number = " << number << endl;
}

static bool energy_called = false;
static bool step_called = false;

// Returns energy_counter
// Revised: Function you might call in your main.cpp
__attribute__((noinline))
float return_energy() {
    energy_called = true;
    return energy_counter;
}

// Returns step counter
// Function you might call in your main.cpp
__attribute__((noinline))
int return_step() {
    step_called = true;
    return step_counter;
}

// Resets line buffer and matrix
// Function you might call in your main.cpp
void reset_buffer_and_matrix(){
    for(int item=0; item<28 ; item++){
        line_buffer_array_1[item] = 0;
        line_buffer_array_2[item] = 0;
    }
    for(int i = 0; i<3; i++){
        for(int j=0;j<3;j++){
            input_matrix[i][j]=0;
            psum_matrix[i][j]=0;
            weight_matrix[i][j]=0;
            output_matrix[i][j]=0;
        }
    }
}

// Updates the line buffers after pe_array computation
void update_line_buffer(float buffer_1_new, float buffer_2_new) {
    for(int item=27; item>0 ; item--){
        line_buffer_array_1[item] = line_buffer_array_1[item-1];
        line_buffer_array_2[item] = line_buffer_array_2[item-1];
    }
    line_buffer_array_1[0] = buffer_1_new;
    line_buffer_array_2[0] = buffer_2_new;
}

// Gets item from line buffer
// Function you might call in your main.cpp
float get_item_from_line_buffer(int item, int buffer_number){
    if (buffer_number == 1) {
        return line_buffer_array_1[item];
    }
    else {
        return line_buffer_array_2[item];
    }
}

// Updates input matrix(PE1, PE4, PE7)
// Function you might call in your main.cpp
float* input_selection(bool pe1_use_buffer, int lb_1_index, bool pe2_use_buffer, int lb_2_index, float new_data[3]){
    // for PE1
    if(pe1_use_buffer){
        input_matrix[0][0] = get_item_from_line_buffer(lb_1_index, 1);
        energy_counter = energy_counter + 2.5;
    }
    else if(!pe1_use_buffer){
        input_matrix[0][0] = new_data[0];
        energy_counter = energy_counter + 10;
    }
    // for PE4
    if(pe2_use_buffer){
        input_matrix[1][0] = get_item_from_line_buffer(lb_2_index, 2);
        energy_counter = energy_counter + 2.5;
    }
    else if(!pe2_use_buffer){
        input_matrix[1][0] = new_data[1];
        energy_counter = energy_counter + 10;
    }
    // for PE7
    input_matrix[2][0] = new_data[2];
    energy_counter = energy_counter + 10;
    float* pe_output = pe_array_computation();
    return pe_output;
}


// Updates psum matrix and input matrix after pe_array computation
void update_matrixs(float* pe_output_matrix){
    update_line_buffer(input_matrix[1][2],input_matrix[2][2]);
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            
            if(i + 1 < 3){
                psum_matrix[i+1][j] = pe_output_matrix[i*3 + j];
            }

            if(j!=2){
                input_matrix[i][2-j] = input_matrix[i][1-j];
            }
            else{
                input_matrix[i][0] = 0;
            }
        }
    }
}

// Sets weight matrix
// Function you might call in your main.cpp
void set_weight(float (*weight_input)[3]) {
    energy_counter = energy_counter + 5;
    for(int i = 0; i < 3; ++i) {
        for(int j = 0; j < 3; ++j) {
            weight_matrix[i][j] = weight_input[i][j];
        }
    }
}

// 3x3 PE array performing convolution computation
float* pe_array_computation(){
    int kernel_size = 3;
    static float output_matrix[3][3] = {0};
    for(int i = 0; i < kernel_size; i++){
        for(int j = 0; j < kernel_size; j++){
            output_matrix[i][j] = (input_matrix[i][j] * weight_matrix[i][j]) + psum_matrix[i][j];
        }
    }
    update_matrixs(&output_matrix[0][0]);
    step_counter ++;
    return &output_matrix[0][0];
}

struct CallLogger {
    ~CallLogger() {
        std::ofstream fout("/app/temp/.call_log");
        fout << "energy=" << energy_called << "\n";
        fout << "step=" << step_called << "\n";
    }
} _call_logger_instance;
```

### `hardware_architecture.h`

```cpp
#ifndef HARDWARE_ARCHITECTURE_H
#define HARDWARE_ARCHITECTURE_H

// This function might be called randomly to verify that you are using this header file
// Function you can't call in your main.cpp
void output(int number);

// Returns energy_counter
// Revised: Function you might call in your main.cpp
float return_energy();

// Returns step counter
// Function you might call in your main.cpp
int return_step();

// Resets line buffer and matrix
// Function you might call in your main.cpp
void reset_buffer_and_matrix();


// Updates the line buffers after pe_array computation
void update_line_buffer(float buffer_1_new, float buffer_2_new);

// Gets item from line buffer
// Function you might call in your main.cpp
float get_item_from_line_buffer(int item, int buffer_number);

// Updates input matrix(PE1, PE4, PE7) from data loader or line buffer
// Function you might call in your main.cpp
float*input_selection(bool pe1_use_buffer, int lb_1_index, bool pe2_use_buffer, int lb_2_index, float new_data[3]);


// Updates psum matrix and input matrix after pe_array computation
void update_matrixs(float* output_matrix); 

// Sets weight matrix
// Function you might call in your main.cpp
void set_weight(float (*weight_input)[3]);


// 3x3 PE array performing convolution computation
float* pe_array_computation(); 


#endif
```

### `main.cpp`

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <iomanip>
#include "hardware_architecture.h"
#include "quantization.h"

typedef std::vector<std::vector<std::vector<float>>> Tensor;
std::vector<float> read_values(const std::string& path) {
    std::ifstream input(path.c_str());
    if (!input) throw std::string("Cannot open input file: ") + path;
    std::vector<float> values;
    std::string token;
    char c;
    while (input.get(c)) {
        if (c == ',' || c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            if (!token.empty()) {
                std::size_t consumed = 0; float value = std::stof(token, &consumed);
                if (consumed != token.size() || !std::isfinite(value)) throw std::string("Invalid numeric input");
                values.push_back(value); token.clear();
            }
        } else token += c;
    }
    if (!token.empty()) {
        std::size_t consumed = 0; float value = std::stof(token, &consumed);
        if (consumed != token.size() || !std::isfinite(value)) throw std::string("Invalid numeric input");
        values.push_back(value);
    }
    return values;
}
void require_size(const std::vector<float>& values, int size) {
    if (static_cast<int>(values.size()) != size) throw std::string("Wrong data shape");
}
int kernel_size(const std::vector<float>& weights, int sets) {
    for (int k = 3; k <= 7; k += 2) if (static_cast<int>(weights.size()) == sets * k * k) return k;
    throw std::string("Kernel must be 3, 5 or 7");
}
int zero_in_buffer(int buffer) {
    for (int i = 0; i < 28; ++i) if (get_item_from_line_buffer(i, buffer) == 0) return i;
    return -1;
}
Tensor convolution(const Tensor& input, const std::vector<float>& weights, const std::vector<float>& biases, int outputs, int k) {
    int channels = static_cast<int>(input.size()), size = static_cast<int>(input[0].size());
    int out_size = (size - k) / 2 + 1;
    Tensor result(outputs, std::vector<std::vector<float>>(out_size, std::vector<float>(out_size, 0)));
    require_size(biases, outputs); require_size(weights, outputs * channels * k * k);
    for (int oc = 0; oc < outputs; ++oc) {
        for (int r = 0; r < out_size; ++r) for (int c = 0; c < out_size; ++c) result[oc][r][c] = biases[oc];
        for (int ic = 0; ic < channels; ++ic)
            for (int br = 0; br < k; br += 3) for (int bc = 0; bc < k; bc += 3) {
                float block[3][3] = {};
                for (int r = 0; r < 3; ++r) for (int c = 0; c < 3; ++c)
                    if (br + r < k && bc + c < k) block[r][c] = weights[((oc * channels + ic) * k + br + r) * k + bc + c];
                reset_buffer_and_matrix(); set_weight(block);
                for (int r = 0; r < out_size; ++r) for (int c = 0; c < out_size; ++c) {
                    float* output = 0;
                    // Reverse columns and skew rows, aligning all nine products
                    // at the bottom three PEs on cycle 4. No CPU convolution.
                    for (int tick = 0; tick < 5; ++tick) {
                        float values[3] = {};
                        bool unused[3] = {};
                        for (int row = 0; row < 3; ++row) {
                            int column = 2 - tick + row;
                            unused[row] = column < 0 || column >= 3 || br + row >= k || bc + column >= k;
                            if (!unused[row]) values[row] = input[ic][2 * r + br + row][2 * c + bc + column];
                        }
                        int z1 = unused[0] ? zero_in_buffer(1) : -1, z2 = unused[1] ? zero_in_buffer(2) : -1;
                        output = input_selection(z1 >= 0, z1 >= 0 ? z1 : 0, z2 >= 0, z2 >= 0 ? z2 : 0, values);
                    }
                    result[oc][r][c] += output[6] + output[7] + output[8];
                }
            }
    }
    return result;
}
double truncate3(float value) { return std::trunc(static_cast<double>(value) * 1000) / 1000; }
void write_tensor(const std::string& path, const Tensor& tensor) {
    std::ofstream output(path.c_str()); if (!output) throw std::string("Cannot create output file: ") + path;
    output << std::fixed << std::setprecision(3);
    for (std::size_t channel = 0; channel < tensor.size(); ++channel)
        for (std::size_t r = 0; r < tensor[channel].size(); ++r) {
            for (std::size_t c = 0; c < tensor[channel][r].size(); ++c) { if (c) output << ','; output << truncate3(tensor[channel][r][c]); }
            output << '\n';
        }
}
std::string directory(std::string path) {
    if (!path.empty() && path.back() != '/' && path.back() != '\\') path += '/';
    return path;
}
int main(int argc, char** argv) {
    if (argc > 3) { std::cerr << "Usage: cnn [input_directory] [output_directory]\n"; return 1; }
    std::string input_dir = argc >= 2 ? directory(argv[1]) : "", output_dir = argc >= 3 ? directory(argv[2]) : "";
    try {
        std::vector<float> image = read_values(input_dir + "img_label.txt"); require_size(image, 784);
        Tensor input(1, std::vector<std::vector<float>>(28, std::vector<float>(28)));
        double error = 0;
        for (int p = 0; p < 784; ++p) {
            float value = quantize(image[p]); input[0][p / 28][p % 28] = value;
            if (image[p] != 0) error += std::fabs(static_cast<double>(image[p]) - value) / std::fabs(image[p]);
        }
        std::vector<float> w1 = read_values(input_dir + "conv1_weight.txt"), w2 = read_values(input_dir + "conv2_weight.txt");
        int k = kernel_size(w1, 2); if (kernel_size(w2, 8) != k) throw std::string("Kernel sizes must match");
        Tensor first = convolution(input, w1, read_values(input_dir + "conv1_bias.txt"), 2, k);
        Tensor second = convolution(first, w2, read_values(input_dir + "conv2_bias.txt"), 4, k);
        write_tensor(output_dir + "result_of_conv1.txt", first); write_tensor(output_dir + "result_of_conv2.txt", second);
        std::vector<float> flat;
        for (const auto& channel : second) for (const auto& row : channel) for (float value : row) flat.push_back(value);
        std::vector<float> fw = read_values(input_dir + "fc_weight.txt"), fb = read_values(input_dir + "fc_bias.txt");
        require_size(fw, static_cast<int>(flat.size()) * 10); require_size(fb, 10);
        float classes[10]; int prediction = 0;
        std::ofstream fc((output_dir + "result_of_fc.txt").c_str()); if (!fc) throw std::string("Cannot create FC output");
        fc << std::fixed << std::setprecision(3);
        for (int c = 0; c < 10; ++c) {
            classes[c] = fb[c];
            for (std::size_t p = 0; p < flat.size(); ++p) classes[c] += flat[p] * fw[c * flat.size() + p];
            if (classes[c] > classes[prediction]) prediction = c;
            if (c) fc << ',';
            fc << truncate3(classes[c]);
        }
        fc << '\n';
        std::ofstream metrics((output_dir + "execution_result.txt").c_str()); if (!metrics) throw std::string("Cannot create metrics output");
        metrics << std::fixed << std::setprecision(2) << "Relative Error after Quantization: " << error * 100 / 784 << "%\nResult of the Prediction: " << prediction << '\n';
        metrics << std::setprecision(1) << "Energy Consumption: " << return_energy() << "\nStep Count: " << return_step() << '\n';
    } catch (const std::string& error) { std::cerr << error << '\n'; return 1; }
    catch (...) { std::cerr << "Invalid numeric input\n"; return 1; }
}
```

### `quantization.h`

```cpp
#ifndef CNN_QUANTIZATION_H
#define CNN_QUANTIZATION_H
#include <cmath>

// E4M3 normal numbers, exponent bias 7; no subnormal support.
inline unsigned char encode_fp8(float value) {
    if (value == 0 || std::fabs(value) < 0.015625f) return 0;
    int sign = value < 0 ? 128 : 0, exponent;
    float fraction = std::frexp(std::fabs(value), &exponent) * 2;
    int field = exponent - 1 + 7;
    int mantissa = static_cast<int>(std::floor((fraction - 1) * 8 + 0.5f));
    if (mantissa == 8) { mantissa = 0; ++field; }
    if (field >= 15) return static_cast<unsigned char>(sign | 119);
    return static_cast<unsigned char>(sign | (field << 3) | mantissa);
}
inline float decode_fp8(unsigned char bits) {
    int exponent = (bits >> 3) & 15, mantissa = bits & 7;
    if (exponent == 0) return 0;
    float value = std::ldexp(1.0f + mantissa / 8.0f, exponent - 7);
    return bits & 128 ? -value : value;
}
inline float quantize(float value) { return decode_fp8(encode_fp8(value)); }
#endif
```

<!-- SOLUTION_CODE_END -->
