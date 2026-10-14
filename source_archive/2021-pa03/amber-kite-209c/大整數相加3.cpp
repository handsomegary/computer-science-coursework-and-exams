/*第一行：輸入一個符號，表示要進行的運算；
第二行第三行每行為一個大整數（長度小於 2000），表示要進行加減操作的兩個數。 注意：整數既可以為正數也可以為負數。

解題思路：列舉，無腦硬錘。*/
#include<iostream>
#include<cstring>
using namespace std;
int main()
{
	//定義變數
	char x1[200]={'\0'};//兩個不超過200位的加數
	char x2[200]={'\0'};
	int carry[201]={0};//進位
	int result[201]={0};//和
	int i;//迴圈變數
	//輸入
	cin>>x1>>x2;
	//求兩char陣列長度
	int len1=strlen(x1);
	int len2=strlen(x2);
	//反序
	strrev(x1);
	strrev(x2);
	//確定兩序列長度的最大值
	int lenmax=len1*(len1>=len2)+len2*(len1<len2);
	//把char型別的加數陣列轉化為int
	int x11[200]={0};
	for(i=0;i<=len1-1;i++)
	{
		x11[i]=x1[i]-'0';
	}
	int x22[200]={0};
	for(i=0;i<=len2-1;i++)
	{
		x22[i]=x2[i]-'0';
	}
	//不考慮進位對應位相加
	for(i=0;i<=lenmax-1;i++)
	{
		result[i]=x11[i]+x22[i];
		result[i+1]=0;//用於防止超過lenmax後的一位
	}
	//考慮進位 完成最終求和
	for(i=0;i<=lenmax+1;i++)
	{
		carry[i+1]=result[i]/10;
		result[i]=result[i]%10+carry[i];
	}
	//若最高位為0,則不輸出最高位
	if(result[lenmax]==0)
		i=lenmax-1;
	else
		i=lenmax;
	for(;i>=0;i--)
		cout<<result[i];
	cout<<endl;
return 0;
}
