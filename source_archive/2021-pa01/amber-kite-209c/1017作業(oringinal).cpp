#include <iostream>
#include <string>
using namespace std;

int main()
{
    int goodprice, hundred_dollars, fifty_dollars, ten_dollars, five_dollars, one_dollars, a, b;
    hundred_dollars = 5 ;
    fifty_dollars = 5 ;
    ten_dollars = 5 ;
    five_dollars = 5 ;
    one_dollars = 5 ;
    a = 0 ;
    b = 0 ;
    int remain;
    remain = 830;
    string vouchers, member, str1 = "d", str2 = "p", str3 = "y", str4 = "n";

    cout << " *** Welcome to Quintuple Stimulus Vouchers System *** " << endl;
    while(remain > 0)
{
    cout << "Please enter the good price: " ;
    cin >> goodprice;
    if (goodprice < 0)
        {cout << "Bye~";return 0;}
    cout << "Please enter the type of vouchers (d/p): " ;
    cin >> vouchers;

    if (vouchers.compare (str1) == 0)
        {goodprice = goodprice * 1 ;
        cout << "Are you a member of NCU Bank (y/n): " ;
        cin >> member;
        if (member.compare (str3) == 0){ goodprice = goodprice * 0.72 ;}
        else if (member.compare(str4) == 0){ goodprice = goodprice * 0.8;}}
    else if (vouchers.compare(str2) == 0){goodprice = goodprice * 0.9;}
    else {goodprice = goodprice * 1;}
    cout << remain << endl;
    cout << goodprice << endl;
    goodprice = goodprice/1 ;
    remain = remain - goodprice ;

    if (remain > 0)
    {
        cout << "The price you need to pay: " << goodprice << endl;
    if(0 < hundred_dollars && hundred_dollars <= goodprice/100)
        {goodprice = goodprice -  hundred_dollars*100;
        cout << hundred_dollars << " $100 voucher" << endl;
        hundred_dollars = 0 ;}
        else if (hundred_dollars > goodprice/100)
        {cout << goodprice/100 << " $100 voucher" << endl;
        hundred_dollars = hundred_dollars - goodprice/100 ;
        goodprice = goodprice%100 ;}
    if (0 < fifty_dollars && fifty_dollars <= goodprice/50)
        {goodprice = goodprice - fifty_dollars*50 ;
        cout << fifty_dollars << "$ 50 vouchers" << endl;
        fifty_dollars = 0 ;}
        else if (goodprice < 50 && goodprice > ten_dollars*10 + five_dollars*5 + one_dollars*1 && goodprice < fifty_dollars*50 + hundred_dollars*100)
        {
            if (hundred_dollars > 0)
                {while (goodprice/100 < hundred_dollars)
                {a = a + 1 ;
                hundred_dollars = a ;
                goodprice = goodprice/100 ;
                cout << hundred_dollars << " $100 voucher" << endl;}}
            else if (fifty_dollars > 0)
                {while (goodprice/50 < fifty_dollars)
                {b = b + 1 ;
                goodprice = goodprice/50 ;
                fifty_dollars = fifty_dollars - 1 ;
                cout << b << " $ 50 voucher " << endl;}}
        }
        else if (fifty_dollars > goodprice/50)
        {cout << goodprice/50 << " $ 50 voucher" << endl;
        fifty_dollars = fifty_dollars - goodprice/50 ;
        goodprice = goodprice%50 ;}

    if (goodprice == 0);
        else if (0 < ten_dollars && ten_dollars <= goodprice/10)
        {goodprice = goodprice - ten_dollars*10 ;
        cout << ten_dollars << " $ 10 voucher" << endl;
        ten_dollars = 0 ;}
        else if (ten_dollars > goodprice/10)
        {cout << goodprice/10 << " $ 10 voucher" << endl;
        ten_dollars = ten_dollars - goodprice/10 ;
        goodprice = goodprice%10 ;}
    if (goodprice == 0);
        else if (0 < five_dollars && five_dollars <= goodprice/5)
        {goodprice = goodprice - five_dollars*5 ;
        cout << five_dollars << " $  5 voucher" << endl;
        five_dollars = 0 ;}
        else if (five_dollars > goodprice/5)
        {cout << goodprice/5 << " $  5 voucher" << endl;
        five_dollars = five_dollars - goodprice/5;
        goodprice = goodprice%5 ;}
    if (goodprice == 0);
        else if (0 < one_dollars && one_dollars <= goodprice)
        {one_dollars = one_dollars - goodprice;
        cout << one_dollars << " $  1 voucher" << endl;}
        else if (one_dollars > goodprice)
        {cout << goodprice << " $  1 voucher" << endl;
        one_dollars = one_dollars - goodprice ;}
    cout << "The remaining vouchers are as follows: " << endl;
    if (hundred_dollars > 0)
    {cout << hundred_dollars << " $100 voucher" << endl;}
    if (fifty_dollars > 0)
    {cout << fifty_dollars << " $ 50 voucher" << endl;}
    if (ten_dollars > 0)
    {cout << ten_dollars << " $ 10 voucher" << endl;}
    if (five_dollars > 0)
    {cout << five_dollars << " $  5 voucher" << endl;}
    if (one_dollars > 0)
    {cout << one_dollars << " $  1 voucher" << endl;}
    cout << endl << endl;
    }
    else if (remain <= 0)
        {cout << "You don't have enough voucher to buy the good." << endl << endl << endl;
        remain = remain + goodprice ;}


    else if(hundred_dollars == 0 && fifty_dollars == 0 && ten_dollars == 0 && five_dollars == 0 && one_dollars == 0)
    {cout << "You do not remain any voucher. Bye~" ; return 0;}
}
    return 0;

}
