#include <iostream>
#include <cmath>

using namespace std;

//function
long long int decimaltobinary(long long int a) { //recursion about decimal to binary
    if (a == 0) {
        return 0;
    }
    else {
        return(a % 2 + 10 * decimaltobinary(a / 2));
    }
}
long long int binarytodecimal(long long int a) { //recursion about binary to decimal 
    if (a == 0) {
        return 0;
    }
    else {
        return(a % 10 + 2 * binarytodecimal(a / 10));
    }
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
int main() {
    long long int a, b; //enter 
    long long int N = 1; //which time
    int tf; //true false
    cout << "*** Welcome to Positive Binary adder ***" << endl;
    cout << "Please enter the 1st number in decimal: ";

    while (1) {  //first time enter
        cin >> a;
        if (a == -1) { //quit
            cout << "Bye";
            tf = 0;
            break;
        }
        else if (a < 0 || a >= 524288) { //first time foolproof
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
            if (b == -1) { //quit
                cout << "Bye";
                tf = 0;
                break;
            }
            else if (b < 0 || b >= 524288) { //N times enter foolproof
                stupidone(N);
                continue;
            }
            if (a + b >= 524288) {
                stupidtwo(N);
                continue;
            }
            else break;
        }
        if (b == -1)break;
        cout << "Calculating... " << decimaltobinary(a) << " + " << decimaltobinary(b) << " = " << binaryadder(decimaltobinary(a), decimaltobinary(b), 1) << endl;
        N++;
        cout << "Partial Sum: " << binarytodecimal(binaryadder(decimaltobinary(a), decimaltobinary(b), 1)) << endl << endl;
        a += b;
    }
}