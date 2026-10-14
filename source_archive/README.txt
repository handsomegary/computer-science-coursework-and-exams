原始資料保存

這裡保存本機提供的非編譯資料，即使重寫的完成版沒有使用它們。
原教授支援程式、header、模板、正式測資及五個作業 ZIP 仍在各作業
course_support/ 與 originals/，沒有被此歸檔替換。

manifest.json：30 份原 C++、1 份學生 header、13 份小資料與 2 份 CBP。
來源包含本地開發草稿與其他提交；內容未修復、未重新判定正確性。
匿名檔名、include、私人路徑與工程 XML 的變動有逐項紀錄，演算法的
識別字、數字與操作符保留。個人產生的 result/Circuit.bench 明標歷史
輸出，不能用來取代官方 golden。

notes/：七份提供的筆記／講解資料，包含教授 PA2 Solution、四份
手寫演算法掃描與原 Doc1.docx。原參考網站 Word 的全部可見文字和
三個完整網址保存成 UTF-8 TXT，不帶原 Word 的個人作者 metadata；
此項明確標為內容保存，沒有宣稱 DOCX 原始 bytes 被匿名後原樣保存。

reports/：既有兩個匿名來源的八份原學生報告，共 53 頁。保留原報告
內容與批注，只匿名身分；它們不是修正版報告，也不代表符合最新版。

29 張課堂截圖的教學內容另存於
../assignments/2021/pa04/originals/local_notes/classroom-notes.pdf。
學生 avatar、會議碼、私人瀏覽器 UI 已真正刪除，正文／程式像素保留；
原 PNG 與匿名 PDF 的對照及轉換在相鄰 classroom-notes.json。

學生自己編譯出的 bin/obj/exe、編譯暫存與可再生 IDE cache 不收錄。
教授隨附且作業使用的 checker 是原課程附件，按原檔保留。
其他來源未選定的學生報告／大型課程教材未在本輪新增，不稱此處為
所有課程資料夾的逐位元組鏡像。

下載後核對：python tools/check_source_archive.py
正式完成版與修正版的執行入口仍依根 README；請勿將歷史草稿作為
已驗證完成版直接使用。
