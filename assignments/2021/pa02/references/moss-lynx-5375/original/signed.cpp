#include <iostream>
#include <cmath>

using namespace std;

//function prototype
long long int power(long long int a, long long int q) {
    long long int k = 1;
    while (q >= 1) {
        k *= a;
        q -= 1;
    }
    return k;
}
long long int carry(long long int a, long long int c) {
    long long int memory;
    if (a == 0)
        return 0;
    if (a % 10 >= 2) {
        memory = (a % 10 - 2) * c;
        a /= 10;
        a += 1;
    }
    else {
        memory = a % 10 * c;
        a /= 10;
    }
    return (memory + carry(a, c * 10));
}
long long int d2bpositive(long long int a) { //recursion about decimal to binary
    if (a == 0) {
        return 0;
    }
    else {
        return(a % 2 + 10 * d2bpositive(a / 2));
    }
}
long long int d2bnegative(long long int x) {

    long long int y = 0;
    long long int z;
    long long int w = 1;
    long long int k;//幾位數
    k = log2(x) + 1;
    while (x > 0) {
        z = x % 2;
        if (z == 1) {
            y += z * w * 0;
            x /= 2;
            w *= 10;
        }
        else {
            y += w;
            x /= 2;
            w *= 10;
        }
    }
    y += 1;;
    y += power(10, k);
    y = carry(y, 1);
    return y;
}
void timee(long long int g) { //st nd rd th...
    switch (g % 10) {
    case 1:
        switch (g % 100 == 11) {
        case 1:
            cout << g << "th";
            break;
        default:
            cout << g << "st";
        }
        break;
    case 2:
        switch (g % 100 == 12) {
        case 1:
            cout << g << "th";
            break;
        default:
            cout << g << "nd";
        }
        break;
    case 3:
        switch (g % 100 == 13) {
        case 1:
            cout << g << "th";
            break;
        default:
            cout << g << "rd";
        }
        break;
    default:
        cout << g << "th";
    }
}
void stupidone(long long int g) { //space saving about wrong enter(enter out of range)
    cout << "The decimal number you entered is out of range. Please enter the ";
    timee(g);
    cout << " decimal number again: ";
}
void stupidtwo(long long int g) { //space saving about wrong enter(sum out of range)
    cout << "The partial sum is out of range. Please enter the ";
    timee(g);
    cout << " decimal number again: ";
}
long long int binaryadder(long long int a, long long int b, long long int c) {
    long long int memory;
    if (a == 0 && b == 0)
        return 0;
    if (a % 10 + b % 10 >= 2) {
        memory = (a % 10 + b % 10 - 2) * c;
        a /= 10;
        b /= 10;
        a += 1;
    }
    else {
        memory = (a % 10 + b % 10) * c;
        a /= 10;
        b /= 10;
    }
    return (memory + binaryadder(a, b, c * 10));
}
unsigned long long int binaryaddernegative(long long int a, long long int b, long long int c) {
    long long int special;
    if (log10(b) > log10(a)) {
        long long int i = log10(a);
        long long int k = log10(b);
        for (;i < k;i++) {
            special = power(10, (i + 1));
            a += special;
        }
    }
    if (log10(b) < log10(a)) {
        long long int i = log10(b);
        long long int k = log10(a);
        for (;i < k;i++) {
            special = power(10, (i + 1));
            b += special;
        }
    }
    long long int memory;
    if (a == 0 && b == 0)
        return 0;
    if (a % 10 + b % 10 >= 2) {
        memory = (a % 10 + b % 10 - 2) * c;
        a /= 10;
        b /= 10;
        a += 1;
    }
    else {
        memory = (a % 10 + b % 10) * c;
        a /= 10;
        b /= 10;
    }
    return (memory + binaryadder(a, b, c * 10));
}
int main() {
    static long long int a, b; //enter 
    long long int N = 1; //which time
    int tf; //true false
    cout << "*** Welcome to Binary Adder and Subtractor ***" << endl;
    cout << "Please enter the 1st number in decimal: ";

    while (1) {  //first time enter
        cin >> a;
        if (a == 0) { //quit
            cout << "Bye";
            tf = 0;
            break;
        }
        else if (a <= -524288 || a >= 524288) { //first time foolproof
            stupidone(N);
            continue;
        }
        else {
            tf = 1;
            break;
        }
    }
    N++;
    while (tf) { //N times enter 
        cout << "Please enter the ";
        timee(N);
        cout << " number in decimal: ";
        while (cin >> b) {
            if (b == 0) { //quit
                cout << "Bye";
                tf = 0;
                break;
            }
            else if (b <= -524288 || b >= 524288) { //N times enter foolproof
                stupidone(N);
                continue;
            }
            if (a + b >= 524288 || a + b <= -524288) {
                stupidtwo(N);
                continue;
            }
            else break;
        }
        if (b == 0)break;
        // + + + 
        if (a > 0 && b > 0) {
            cout << "Calculating... " << "0" << d2bpositive(a) << " + " << "0" << d2bpositive(b) << " = " << "0" << binaryadder(d2bpositive(a), d2bpositive(b), 1) << endl;
        }
        // + + -  or - + + 
        if (a > 0 && b < 0 && (a + b) > 0) {
            if (log10(binaryaddernegative(d2bpositive(a), d2bnegative(-b), 1)) > log10(d2bnegative(-b))) {
                cout << "Calculating... " << "0" << d2bpositive(a) << " + " << d2bnegative(-b) << " = " << "0" << binaryaddernegative(d2bpositive(a), d2bnegative(-b), 1) - power(10, log10(binaryaddernegative(d2bpositive(a), d2bnegative(-b), 1))) << endl;
            }
            else cout << "Calculating... " << "0" << d2bpositive(a) << " + " << d2bnegative(-b) << " = " << "0" << binaryaddernegative(d2bpositive(a), d2bnegative(-b), 1) << endl;
        }
        if (a > 0 && b < 0 && (a + b) < 0) {
            cout << "Calculating... " << "0" << d2bpositive(a) << " + " << d2bnegative(-b) << " = " << binaryaddernegative(d2bpositive(a), d2bnegative(-b), 1) << endl;
        }
        if (a < 0 && b > 0 && (a + b) > 0) {
            if (log10(binaryaddernegative(d2bnegative(-a), d2bpositive(b), 1)) > log10(d2bnegative(-a))) {
                cout << "Calculating... " << d2bnegative(-a) << " + " << "0" << d2bpositive(b) << " = " << "0" << binaryaddernegative(d2bnegative(-a), d2bpositive(b), 1) - power(10, log10(binaryaddernegative(d2bnegative(-a), d2bpositive(b), 1))) << endl;
            }
            else cout << "Calculating... " << d2bnegative(-a) << " + " << "0" << d2bpositive(b) << " = " << "0" << binaryaddernegative(d2bnegative(-a), d2bpositive(b), 1) << endl;
        }
        if (a < 0 && b > 0 && (a + b) < 0) {
            cout << "Calculating... " << d2bnegative(-a) << " + " << "0" << d2bpositive(b) << " = " << binaryaddernegative(d2bnegative(-a), d2bpositive(b), 1) << endl;
        }
        //0的情況
        if (a + b == 0) {
            if (a < 0 && b > 0) {
                cout << "Calculating... " << d2bnegative(-a) << " + " << "0" << d2bpositive(b) << " = " << binaryadder(d2bnegative(-a), d2bpositive(b), 1) - power(10, log10(binaryadder(d2bnegative(-a), d2bpositive(b), 1))) << endl;
            }
            else
                cout << "Calculating... " << 0 << d2bpositive(a) << " + " << d2bnegative(-b) << " = " << binaryadder(d2bpositive(a), d2bnegative(-b), 1) - power(10, log10(binaryadder(d2bpositive(a), d2bnegative(-b), 1))) << endl;
        }
        //負數二補數加法
        if (a < 0 && b < 0 && b <= a && -(a + b) < power(2, (log10(d2bnegative(-b))))) {
            cout << "Calculating... " << d2bnegative(-a) << " + " << d2bnegative(-b) << " = " << binaryaddernegative(d2bnegative(-a), d2bnegative(-b), 1) - power(10, log10(d2bnegative(-b)) + 1) << endl;
        }
        if (a < 0 && b < 0 && b <= a && -(a + b) >= power(2, (log10(d2bnegative(-b))))) {
            cout << "Calculating... " << d2bnegative(-a) << " + " << d2bnegative(-b) << " = " << binaryaddernegative(d2bnegative(-a), d2bnegative(-b), 1) << endl;
        }
        if (a < 0 && b < 0 && a < b && -(a + b) < power(2, (log10(d2bnegative(-a))))) {
            cout << "Calculating... " << d2bnegative(-a) << " + " << d2bnegative(-b) << " = " << binaryaddernegative(d2bnegative(-a), d2bnegative(-b), 1) - power(10, log10(d2bnegative(-a)) + 1) << endl;
        }
        if (a < 0 && b < 0 && a < b && -(a + b) >= power(2, (log10(d2bnegative(-a))))) {
            cout << "Calculating... " << d2bnegative(-a) << " + " << d2bnegative(-b) << " = " << binaryaddernegative(d2bnegative(-a), d2bnegative(-b), 1) << endl;
        }
        if (a == 0) {
            if (b > 0) {
                cout << "Caculating... " << 0 << " + " << 0 << d2bpositive(b) << " = " << 0 << d2bpositive(b) << endl;
            }
            else {
                cout << "Caculating... " << 0 << " + " << d2bnegative(-b) << " = " << d2bnegative(-b) << endl;
            }
        }
        a += b;
        N++;
        cout << "Partial Sum: " << a << endl << endl;
    }
}
