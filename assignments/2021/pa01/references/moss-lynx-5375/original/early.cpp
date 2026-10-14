#include <iostream>

using namespace std;

int main()
{
    int hundreddlrs = 5;
    int fiftydlrs = 5;
    int tendlrs = 5;
    int fivedlrs = 5;
    int onedlrs = 5;
    int gdprice;
    char dp;
    char yn;
    int pocket;

    cout << "*** Welcome to Quintuple Stimulus Vouchers System ***" << endl;
    while (true)
    {
        cout << "Please enter the good price: ";
        cin >> gdprice;
        if (gdprice == -1)
        {
            break;
        }
        cout << "Please enter the type of vouchers (d/p): ";
        cin >> dp;
        if (dp == 'd')
        {
            cout << "Are you a member of NCU Bank (y/n): ";
            cin >> yn;

            if (yn == 'y')
            {
                gdprice *= 0.72;
            }
            else
            {
                gdprice *= 0.8;
            }
        }
        else
        {
            gdprice *= 0.9;
        }
        cout << "The price you need to pay: " << gdprice << endl;
        pocket = 100 * hundreddlrs + 50 * fiftydlrs + 10 * tendlrs + 5 * fivedlrs + 1 * onedlrs;
        if (pocket < gdprice)
        {
            cout << "You don't have enough voucher to buy the good" << endl
                 << endl;
            continue;
        }
        cout << "The payment is processed as follows:" << endl;
        int mh = hundreddlrs, mff = fiftydlrs, mt = tendlrs, mf = fivedlrs, mo = onedlrs, mgd = gdprice;
        while (hundreddlrs > 0 && gdprice >= 100)
        {
            gdprice -= 100;
            hundreddlrs -= 1;
        }

        while (fiftydlrs > 0 && gdprice >= 50)
        {
            gdprice -= 50;
            fiftydlrs -= 1;
        }
        while (tendlrs > 0 && gdprice >= 10)
        {
            gdprice -= 10;
            tendlrs -= 1;
        }
        while (fivedlrs > 0 && gdprice >= 5)
        {
            gdprice -= 5;
            fivedlrs -= 1;
        }
        while (onedlrs > 0 && gdprice >= 1)
        {
            gdprice -= 1;
            onedlrs -= 1;
        }

        if (gdprice != 0)
        {
            hundreddlrs = mh, fiftydlrs = mff, tendlrs = mt, fivedlrs = mf, onedlrs = mo, gdprice = mgd;
            while (gdprice > 0 && hundreddlrs > 0)
            {
                gdprice -= 100;
                hundreddlrs -= 1;
            }

            while (fiftydlrs > 0 && gdprice > 0)
            {
                gdprice -= 50;
                fiftydlrs -= 1;
            }
            while (tendlrs > 0 && gdprice > 0)
            {
                gdprice -= 10;
                tendlrs -= 1;
            }
            while (fivedlrs > 0 && gdprice > 0)
            {
                gdprice -= 5;
                fivedlrs -= 1;
            }
            while (onedlrs > 0 && gdprice > 0)
            {
                gdprice -= 1;
                onedlrs -= 1;
            }
        }

        if (mh - hundreddlrs > 0)
            cout << "   " << mh - hundreddlrs << " $100 voucher" << endl;
        if (mff - fiftydlrs > 0)
            cout << "   " << mff - fiftydlrs << " $ 50 voucher" << endl;
        if (mt - tendlrs > 0)
            cout << "   " << mt - tendlrs << " $ 10 voucher" << endl;
        if (mf - fivedlrs > 0)
            cout << "   " << mf - fivedlrs << " $  5 voucher" << endl;
        if (mo - onedlrs > 0)
            cout << "   " << mo - onedlrs << " $  1 voucher" << endl;
        cout << "The remaining vouchers are as follows:" << endl;
        if (hundreddlrs > 0)
            cout << "   " << hundreddlrs << " $100 voucher" << endl;
        if (fiftydlrs > 0)
            cout << "   " << fiftydlrs << " $ 50 voucher" << endl;
        if (tendlrs > 0)
            cout << "   " << tendlrs << " $ 10 voucher" << endl;
        if (fivedlrs > 0)
            cout << "   " << fivedlrs << " $  5 voucher" << endl;
        if (onedlrs > 0)
            cout << "   " << onedlrs << " $  1 voucher" << endl;
        cout << endl;
        if (hundreddlrs == 0 && fiftydlrs == 0 && tendlrs == 0 && fivedlrs == 0 && onedlrs == 0)
        {
            cout << "You do not remain any voucher" << endl;
            break;
        }
    }
    cout << "Bye~" << endl;

    system("pause");
}
