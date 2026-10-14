/*C++實現大整數運算包（加、減、乘、除、冪模、GCD、乘法逆*/
#include<iostream>
#include<string.h>
using namespace std;
void jiafa(char x[],char y[]);
void jianfa(char x[],char y[]);
int main (){ //頭鐵，列舉所有情況
	char n[2002]= {0}, m[2002]={0}, sum[2003] = {0} ,fuhao = 0;
	char n1[2002]= {0}, m1[2002]={0};
	cin >> fuhao >> n >> m;

	if (fuhao == '+' && n[0] != '-' && m[0] != '-') {
		jiafa(n,m);
	}
	if (fuhao == '+' && n[0] == '-' && m[0] == '-') {
		for (int i = 0; i < 2002; i ++){//因為存在符號，所以另開陣列存入數字
			n1[i] = n[i+1];
			m1[i] = m[i+1];
		}
		cout << "-";
		jiafa(n1,m1);
	}
	if (fuhao == '-' && n[0] != '-' && m[0] == '-'){
		for (int i = 0; i < 2002; i ++){
			m1[i] = m[i+1];
		}
		jiafa(n,m1);
	}
	if (fuhao == '-' && n[0] == '-' && m[0] != '-'){
		for (int i = 0; i < 2002; i ++){
			n1[i] = n[i+1];
		}
		cout << "-";
		jiafa(n1,m);
	}
	if (fuhao == '+' && n[0] != '-' && m[0] == '-'){
		for (int i = 0; i < 2002; i ++){
			m1[i] = m[i+1];
		}
		jianfa(n,m1);
	}
		if (fuhao == '+' && n[0] == '-' && m[0] != '-'){
		for (int i = 0; i < 2002; i ++){
			n1[i] = n[i+1];
		}
		jianfa(m,n1);
	}
	if (fuhao == '-' && n[0] == '-' && m[0] == '-'){
		for (int i = 0; i < 2002; i ++){
			n1[i] = n[i+1];
			m1[i] = m[i+1];
		}
		jianfa(m1,n1);
	}
	if (fuhao == '-' && n[0] != '-' && m[0] != '-'){
		jianfa(n,m);
	}
	return 0;
}


void jiafa(char x[],char y[]){
	int he[2002] = {0};
	int len1 = strlen(x), len2 = strlen(y);
	int len_max = len1 > len2 ? len1:len2;
	int jin = 0;
	for (int i = 0; i < len_max; i++){
		int a = 0,b = 0;
		if (i < len1) a = x[len1-i-1] - '0';
		if (i < len2) b = y[len2-i-1] - '0';
		int dight = a + b + jin;
		jin = 0;
		if (dight >= 10){
			dight -= 10;
			jin = 1;
		}
		he[len_max - i] = dight;
	}
	if (jin == 1) he[0] = 1;
	else he[0] = 0;
	for (int i = 0; i < len_max + 1; i ++){
		if (i == 0 && he[0] == 0) continue;
 		cout << he[i];
	}
}


void jianfa(char x[],char y[]){
	int he[2003] = {0};
	int len1 = strlen(x), len2 = strlen(y);
	int len_max = len1 > len2 ?len1:len2;
	for (int i = 0; i < len_max; i ++){
		int a = 0, b = 0;
		if (i < len1) a = x[len1-i-1] - '0';
		if (i < len2) b = y[len2-i-1] - '0';
		if (len1 > len2){
			int dight = a - b;
			if (dight >= 0) he[len_max - i] = dight ;
			else {
				for (int k = 2; len1 - i - k >= 0; k ++){
					if (x[len1-i-k] == 0) {
						x[len1-i-k] == 9;
						continue;
					}
					else {
						x[len1-i-k] -= 1;
						break;
					}
				}
				he[len_max - i] = 10 + dight;
			}
		}else if (len1 = len2) {
			int  z = 0;
			for (int j = 0; j < len_max ; j ++){
				if (x[j] > y[j]) {
					break;
				}
				else if (x[j] < y[j]){
					z = 1;
					break;
				}
				else continue;
			}
			if (z != 1){
				int dight = a - b;
				y[len2-i-1] = '0' - 1;
				if (dight >= 0) {
					he[len_max - i] = dight ;
				}
				else {
					for (int k = 2; len1 - i - k >= 0; k ++){
						if (x[len1-i-k] == 0) x[len1-i-k] == 9;
						else {
							x[len1-i-k] -= 1;
							break;
						}
					}
					he[len_max - i] = 10 + dight;
				}
			}
			if (z == 1) {
				he[0] = 1;
				int dight = b - a;
				x[len1-i-1] = '0' - 1;
				if (dight >= 0) he[len_max - i] = dight ;
				else {
					for (int k = 2; len2 - i - k >= 0; k ++){
						if (y[len2-i-k] == 0) y[len2-i-k] == 9;
						else {
							y[len2-i-k] -= 1;
							break;
						}
					}
					he[len_max - i] = 10 + dight;
				}
			}
		}else if (len1 < len2){
			he[0] = 1;
				int dight = b - a;
				if (dight >= 0) he[len_max - i] = dight ;
				else {
					for (int k = 2; len2 - i - k >= 0; k ++){
						if (y[len2-i-k] == 0) y[len2-i-k] == 9;
						else {
							y[len2-i-k] -= 1;
							break;
						}
					}
					he[len_max - i] = 10 + dight;
				}
		}
	}
	int xixi = 0;
	if (he[0] == 1) cout << "-";
	for (int i = 1; i < len_max + 1; i ++){
		if (he[i] == 0 && xixi == 0){
			if (i == len_max) cout << "0";
			else continue;
		}
		else {
			xixi = 1;
			cout << he[i];
		}
	}
}
