# PA1：Dinosaur Game（2025）

[完整題目、解法與程式碼](solution.md)。基本版、雙層 bonus 與自動遊玩 bonus 共用一個完整的狀態轉移實作，只引入 `<iostream>`。

```sh
g++ -std=c++11 -O2 main.cpp -o dinosaur
./dinosaur < input.txt
./dinosaur --double < double_input.txt
./dinosaur --auto < auto_input.txt
```

預設輸出含輸入回顯，以符合提供的終端畫面測資；`--no-echo` 可停用回顯。雙層版需先輸入下層 8 位、上層 8 位與天花板 9 位配置。自動版每個 tick 只讀下一組障礙／天花板，不讀動作。

官方兩份基本範例、一份雙層範例已與文字 golden 核對（內容一致，程式另外保留最後換行）。自動版的官方測資已產生合法勝利路徑。額外測試涵蓋非法浮點命令、重複跳躍、碰撞、直接勝利，以及 10,000 tick 上限。
