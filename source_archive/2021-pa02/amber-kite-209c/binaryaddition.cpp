#include <iostream>
using namespace std;
//(11011,100101001100010101)


int main()
{
    unsigned long long a1;
    unsigned long long a2;
    cin >> a1 >> a2;
	unsigned long long x = max(a1,a2);
    unsigned long long y = min(a1,a2);
    unsigned long long result = 0;
    int carry = 0;
    unsigned long long place = 1;
    while(x >= 0)
    {
        a1 = x%10;
        a2 = y%10;
        result = ((a1+a2+carry)%2)*place + result;
        carry = (a1+a2+carry)/2;
        x = x/10;
        y = y/10;
        place = place*10;
        if(x == 0 && carry == 0)
            break;
    }
    cout << "Binary Addition = " << result << endl;
    return 0;
}
