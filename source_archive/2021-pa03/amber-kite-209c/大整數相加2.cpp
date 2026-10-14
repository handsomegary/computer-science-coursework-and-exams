/*首先要解決的就是儲存 200 位整數的問題。顯然，任何 C/C++固有型別的變數都無法儲存它。
最直觀的想法是可以用一個字串來儲存它。字串本質上就是一個字元陣列，因此為了程式設計更方便，我們也可以用陣列 unsigned an[200]來儲存一個 200 位的整數，讓 an[0]存
放個位數， an[1]存放十位數， an[2]存放百位數……
    那麼如何實現兩個大整數相加呢？方法很簡單，就是模擬小學生列豎式做加法，從個位開始逐位相加，超過或達到 10 則進位。
也就是說，用 unsigned an1[201]儲存第一個數，用unsigned an2[200]表示第二個數，然後逐位相加，相加的結果直接存放在 an1 中。要注意處理進位。
另外， an1 陣列長度定為 201，是因為兩個 200 位整數相加，結果可能會有 201 位。實際程式設計時，不一定要費心思去把陣列大小定得正好合適，稍微開大點也無所謂，以免不小心沒有算準這個“正好合適”的數值，而導致陣列小了，產生越界錯誤。*/
#include <iostream>
#include <cstring>
#define MAX_LEN 200
int an1[MAX_LEN+10];
int an2[MAX_LEN+10];
char szLine1[MAX_LEN+10];
char szLine2[MAX_LEN+10];
using namespace std;
int main()
{
	cin >> szLine1;
	cin >> szLine2;
	int i,j;
	memset(an1,0,sizeof(an1));
	memset(an2,0,sizeof(an2));
	int nLen1 = strlen(szLine1);
	//下面將 szLine1 中儲存的字串形式的整數轉換到 an1 中去，an1就是個位
	j = 0;
	for(i=nLen1-1;i>=0;i--){
		an1[j++]=szLine1[i]-'0';
	}
	int nLen2 = strlen(szLine2);
	j = 0;
	for(i=nLen2-1;i>=0;i--){
		an2[j++]=szLine2[i]-'0';
	}
	for(i=0;i<MAX_LEN;i++){
		an1[i] += an2[i];//逐位相加
		if(an1[i] >= 10){//看是否要進位
			an1[i] -= 10;
			an1[i+1]++;//進位
		}
	}
	bool bStartOutput = false; //此變數用於跳過多餘的 0
	for(i=MAX_LEN;i>=0;i--){
		if(bStartOutput){
			cout<<an1[i];//如果多餘的 0 已經都跳過，則輸出
		}else if(an1[i]){
			cout<<an1[i];
			bStartOutput=true;//碰到第一個非 0 的值，就說明多餘的 0 已經都跳過
		}
	}
	cout<<endl;
	return 0;
}
