#include <iostream>
using namespace std;
long long dectobin (long long);
long long bintodec (long long);
long long binaryadd (long long, long long);
long long negativetwocompliment (long long);

int main()
{
    long long decimal1 = 0;
    long long decimal2 = 0;
    long long sum = 0;
    int n = 4;

    cout << "*** Welcome to Binary Adder and Subtractor ***" << endl;

    //1st number
    cout << "Please enter the 1st number in decimal: ";
    cin >> decimal1;
    if (decimal1 == -1)
    {
        cout << "Bye~";
        return 0;
    }
    while(decimal1 > 524287)
    {
        cout << "The decimal number you entered is out of range. Please enter the 1st decimal number again: ";
        cin >> decimal1;
    }

    //2nd number
    cout << "Please enter the 2nd number in decimal: ";
    cin >> decimal2;
    if (decimal2 == -1)
    {
        cout << "Bye~";
        return 0;
    }
    while(decimal2 > 524287)
    {
        cout << "The decimal number you entered is out of range. Please enter the 2nd decimal number again: ";
        cin >> decimal2;
    }
    while(decimal1 + decimal2 > 524287)
    {
        cout << "The partial sum is out of range. Please enter the 2nd decimal number again: ";
        cin >> decimal2;
    }
    sum = dectobin(decimal1);
    cout << "Calculating... " << sum << " + " << dectobin(decimal2) << " = " << binaryadd(sum, dectobin(decimal2)) << endl;
    sum = binaryadd(sum, dectobin(decimal2));
    cout << "Partial Sum: " << bintodec(sum) << endl << endl;

    //3rd number
    cout << "Please enter the 3rd number in decimal: ";
    cin >> decimal1;
    if (decimal1 == -1)
    {
        cout << "Bye~";
        return 0;
    }
    while(decimal1 > 524287)
    {
        cout << "The decimal number you entered is out of range. Please enter the 3rd decimal number again: ";
        cin >> decimal1;
    }
    while(decimal1 + decimal2 > 524287)
    {
        cout << "The partial sum is out of range. Please enter the 3rd decimal number again: ";
        cin >> decimal1;
    }
    cout << "Calculating... " << sum << " + " << dectobin(decimal1) << " = " << binaryadd(sum, dectobin(decimal1)) << endl;
    sum = binaryadd(sum, dectobin(decimal1));
    cout << "Partial Sum: " << bintodec(sum) << endl << endl;


    //4,5,6...th number
    while(true)
    {
        cout << "Please enter the " << n << "th number in decimal: ";
        cin >> decimal2;
        if (decimal2 == -1)
        {
            cout << "Bye~";
            break;
        }
        while(decimal2 > 524287)
        {
            cout << "The decimal number you entered is out of range. Please enter the " << n << "th decimal number again: ";
            cin >> decimal2;
        }
        while(decimal1 + decimal2 > 524287)
        {
            cout << "The partial sum is out of range. Please enter the " << n << "th decimal number again: ";
            cin >> decimal2;
        }
        n = n + 1;
        cout << "Calculating... " << sum << " + " << dectobin(decimal2) << " = " << binaryadd(sum, dectobin(decimal2)) << endl;
        sum = binaryadd(sum, dectobin(decimal2));
        cout << "Partial Sum: " << bintodec(sum) << endl << endl;
        cout << "Please enter the " << n << "th number in decimal: ";
        cin >> decimal1;
        if (decimal1 == -1)
        {
            cout << "Bye~";
            break;
        }
        while(decimal1 > 524287)
        {
            cout << "The decimal number you entered is out of range. Please enter the 3rd decimal number again: ";
            cin >> decimal1;
        }
        while(decimal1 + decimal2 > 524287)
        {
            cout << "The partial sum is out of range. Please enter the " << n << "th decimal number again: ";
            cin >> decimal1;
        }
        n = n + 1;
        cout << "Calculating... " << sum << "+" << dectobin(decimal1) << "=" << binaryadd(sum, dectobin(decimal1)) << endl;
        sum = binaryadd(sum, dectobin(decimal1));
        cout << "Partial Sum: " << bintodec(sum) << endl << endl;
    }
    return 0;
}

long long dectobin (long long number)//decimal to binary
{
    long long binary = 0;
    long long i = 1;
    while(number > 0)
    {
        binary += (number%2)*i;
        number = number/2;
        i *= 10;
    }
    return binary;
}
long long bintodec (long long binsum)
{
    long long decimal = 0;
    long long n = 1;
    while(binsum > 0)
    {
        decimal += (binsum%10)*n;
        binsum = binsum/10;
        n *= 2;
    }
    return decimal;
}
long long binaryadd (long long a1, long long a2)// addition of two binary numbers
{
	long long x = max(a1,a2);
    long long y = min(a1,a2);
    long long result = 0;
    int carry = 0;
    long long place = 1;
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
    return result;
}
long long negativetwocompliment (long long negative)
{
    long long twocompliment = 0;
    long long k = 1;
    while(negative != 0)
    {
        twocompliment += ((negative + 1)%2)*k;
        k *=10;
        negative /=2;
    }
    return twocompliment;
}
