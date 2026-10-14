#include <iostream>
#include <string.h>
using namespace std;

int main()
{
    while(true)
    {
        cout << "----Welcome to big number calculator----" << endl;
        cout << "Please choose the function" << endl;
        cout << "1. Addition" << endl;
        cout << "2. Subtraction" << endl;
        cout << "3. Multiplication" << endl;
        cout << "0. Exit" << endl;
        cout << "--------------------------" << endl;
        int option;
        (cin >> option).get();
        if(option == 0)
        {
            cout << "Good bye!";
            return 0;
        }
        else if(option > 3 || option < 0)
        {
            cout << "Error! Please try again." << endl << endl;

        }
        else{}

        if(option == 1)
        {
            int a[5][101] = {0}, k = 0, l = 0, sum[3][102]={0};
            char a1[3][101]={'\n'};
            cout<<"----Now for Addition----"<<endl;
            cout<<"Please enter two numbers.\n";
            cin.getline(a1[1],101);
            cin.getline(a1[2],101);
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0; i < 101; i++)
                {
                    a[j][i] = a1[j][i] - '0';
                    switch(a[j][i])
                    {
                    case 0:
                        a[j][i + k] = 0;
                        break;
                    case 1:
                        a[j][i + k] = 1;
                        break;
                    case 2:
                        a[j][i + k] = 2;
                        break;
                    case 3:
                        a[j][i + k] = 3;
                        break;
                    case 4:
                        a[j][i + k] = 4;
                        break;
                    case 5:
                        a[j][i + k] = 5;
                        break;
                    case 6:
                        a[j][i + k] = 6;
                        break;
                    case 7:
                        a[j][i + k] = 7;
                        break;
                    case 8:
                        a[j][i + k] = 8;
                        break;
                    case 9:
                        a[j][i + k] = 9;
                        break;
                    default:
                        a[j][i + k] = '\n';
                    }

                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(a[j][100 - k] == '\n' && a[j][99 - k] == '\n')
                {
                    k++;
                }
                k++;
                for(int i = 0; i < 101 - k; i++)
                {
                    a[j + 2][k + i] = a[j][i];
                }
                k = 0;
            }
            for(int j = 1;j <= 4; j++)
            {
                for(int i = 0;i < 101;i++)
                {
                    if(a[j][i] == '\n')
                    {
                        a[j][i]=0;
                    }
                }
            }
            for(int i = 0; i < 101; i++)
            {
                {
                    sum[1][101 - i]=(a[3][100-i] + a[4][100-i] + sum[2][101-i]) % 10;
                    sum[2][100 - i]=(a[3][100-i] + a[4][100-i] + sum[2][101-i]) / 10;
                }
            }
            sum[1][0] = sum[2][0];
            while(sum[1][l] == 0 && sum[1][l + 1] == 0)
            {
                l++;
            }
            cout<<"Result: ";
            if(l > 100) cout << "0";
            for(int i = l + 1; i < 102; i++)
            {
                cout << sum[1][i];
            }
            cout << endl;
            /*mean*/
            int b[5][101] = {0}, m = 0, n = 0, sum1[3][102] = {0}, mean[3][102] = {0};
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0; i < 101; i++)
                {
                    b[j][i] = a1[j][i] - '0';
                    switch(b[j][i])
                    {
                    case 0:
                        b[j][i + m]=0;
                        break;
                    case 1:
                        b[j][i + m] = 1;
                        break;
                    case 2:
                        b[j][i + m] = 2;
                        break;
                    case 3:
                        b[j][i + m] = 3;
                        break;
                    case 4:
                        b[j][i + m] = 4;
                        break;
                    case 5:
                        b[j][i + m] = 5;
                        break;
                    case 6:
                        b[j][i + m] = 6;
                        break;
                    case 7:
                        b[j][i + m] = 7;
                        break;
                    case 8:
                        b[j][i + m] = 8;
                        break;
                    case 9:
                        b[j][i + m] = 9;
                        break;
                    default:
                        b[j][i + m] = '\n';
                    }
                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(b[j][100 - m] == '\n' && b[j][99 - m] == '\n')
                {
                    m++;
                }
                m++;
                for(int i = 0; i < 101 - m; i++)
                {
                    b[j + 2][m + i] = b[j][i];
                }
                m = 0;
            }
            for(int j = 1; j <= 4; j++)
            {
                for(int i = 0;i < 101; i++)
                {
                    if(b[j][i] == '\n')
                    {
                        b[j][i] = 0;
                    }
                }
            }
            for(int i = 0; i < 101; i++)
            {
                {
                    sum1[1][101 - i] = (b[3][100 - i] + b[4][100 - i] + sum1[2][101 - i]) % 10;
                    sum1[2][100 - i] = (b[3][100 - i] + b[4][100 - i] + sum1[2][101 - i]) / 10;
                }
            }
            sum1[1][0] = sum1[2][0];
            while(sum1[1][n] == 0 && sum1[1][n+1] == 0)
            {
                n++;
            }
            for(int i = n + 1; i < 102; i++)
            {
                if((sum1[1][i] + mean[2][i]) % 2 != 0 && i < 102)
                {
                    mean[2][i + 1] += 10;
                }
                mean[1][i] = (sum1[1][i] + mean[2][i]) / 2;
            }
            cout << "Mean: ";
            if(mean[1][n + 1] == 0)
            {
                n++;
            }
            if(n > 100) cout << "0";
            for(int i = n + 1; i < 102; i++)
            {
                cout << mean[1][i];
            }
                cout << endl << endl;
        }
        else if(option == 2)
        {
            int a[5][101] = {0}, k = 0, l = 0, sub[3][102] = {0};
            char a1[3][101]={'\n'};
            cout << "----Now for Subtraction----" << endl;
            cout << "Please enter two numbers.\n";
            cin.getline(a1[1], 101);
            cin.getline(a1[2], 101);
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0; i < 101; i++)
                {
                    a[j][i] = a1[j][i] - '0';
                    switch(a[j][i])
                    {
                    case 0:
                        a[j][i + k] = 0;
                        break;
                    case 1:
                        a[j][i + k] = 1;
                        break;
                    case 2:
                        a[j][i + k] = 2;
                        break;
                    case 3:
                        a[j][i + k] = 3;
                        break;
                    case 4:
                        a[j][i + k] = 4;
                        break;
                    case 5:
                        a[j][i + k] = 5;
                        break;
                    case 6:
                        a[j][i + k] = 6;
                        break;
                    case 7:
                        a[j][i + k] = 7;
                        break;
                    case 8:
                        a[j][i + k] = 8;
                        break;
                    case 9:
                        a[j][i + k] = 9;
                        break;
                    default:
                        a[j][i + k] = '\n';
                    }
                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(a[j][100-k] == '\n' && a[j][99-k] == '\n')
                {
                    k++;
                }
                k++;
                for(int i = 0; i < 101 - k; i++)
                {
                    a[j + 2][k + i] = a[j][i];
                }
                k=0;
            }
            for(int j = 1; j <= 4; j++)
            {
                for(int i = 0;i < 101; i++)
                {
                    if(a[j][i] == '\n')
                    {
                        a[j][i] = 0;
                    }
                }
            }

            for(int i = 0; i < 101; i++)
            {
                {
                    if((a[3][100-i] - a[4][100-i] - sub[2][100-i]) < 0)
                    {
                        sub[1][100-i] = a[3][100-i] - a[4][100-i] - sub[2][100-i] + 10;
                        sub[2][99-i]++;
                    }
                    else
                    {
                        sub[1][100-i] = a[3][100-i] - a[4][100-i] - sub[2][100-i];
                    }

                }
            }

            while(sub[1][l] == 0 && sub[1][l+1] == 0)
            {
                l++;
            }
            cout << "Result: ";
            if(l > 100) cout << "0";
            for(int i = l + 1; i < 101; i++)
            {
                cout << sub[1][i];
            }
            cout << endl;
            /*mean*/
            int b[5][101] = {0}, m = 0, n = 0, sum1[3][102] = {0}, mean[3][102] = {0};
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0; i < 101; i++)
                {
                    b[j][i] = a1[j][i] - '0';
                    switch(b[j][i])
                    {
                    case 0:
                        b[j][i + m] = 0;
                        break;
                    case 1:
                        b[j][i + m] = 1;
                        break;
                    case 2:
                        b[j][i + m] = 2;
                        break;
                    case 3:
                        b[j][i + m] = 3;
                        break;
                    case 4:
                        b[j][i + m] = 4;
                        break;
                    case 5:
                        b[j][i + m] = 5;
                        break;
                    case 6:
                        b[j][i + m] = 6;
                        break;
                    case 7:
                        b[j][i + m] = 7;
                        break;
                    case 8:
                        b[j][i + m] = 8;
                        break;
                    case 9:
                        b[j][i + m] = 9;
                        break;
                    default:
                        b[j][i + m] = '\n';
                    }
                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(b[j][100 - m] == '\n' && b[j][99 - m] == '\n')
                {
                    m++;
                }
                m++;
                for(int i = 0; i < 101 - m; i++)
                {
                    b[j + 2][m + i] = b[j][i];
                }
                m = 0;
            }
            for(int j = 1; j <= 4; j++)
            {
                for(int i = 0; i < 101; i++)
                {
                    if(b[j][i] == '\n')
                    {
                        b[j][i] = 0;
                    }
                }
            }
            for(int i = 0; i < 101; i++)
            {
                {
                    sum1[1][101 - i] = (b[3][100 - i] + b[4][100 - i] + sum1[2][101 - i]) % 10;
                    sum1[2][100 - i] = (b[3][100 - i] + b[4][100 - i] + sum1[2][101 - i]) / 10;
                }
            }
            sum1[1][0] = sum1[2][0];
            while(sum1[1][n] == 0 && sum1[1][n + 1] == 0)
            {
                n++;
            }
            for(int i = l + 1; i < 102; i++)
            {
                if((sum1[1][i] + mean[2][i]) % 2 != 0 && i < 102)
                {
                    mean[2][i+1] += 10;
                }
                mean[1][i] = (sum1[1][i] + mean[2][i]) / 2;
            }
            cout << "Mean: ";
            if(mean[1][n + 1] == 0)
            {
                n++;
            }
            if(n > 100) cout << "0";
            for(int i = n + 1; i < 102; i++)
            {
                cout << mean[1][i];
            }
            cout << endl << endl;
        }
        else if(option == 3)
        {
            cout<<"----Now for Multiplication----"<<endl;
            const int k = 100;
            int a[7][2*k] = {{0}}, h = 0, m = 0;
            char b[3][2*k] = {'a'}, b1[3][101] = {'a'};
            cout << "Please enter two numbers\n";
            cin.getline(b[1],101);
            cin.getline(b[2],101);
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0; i < 2*k; i++)
                {
                    a[j][i] = b[j][i] - '0';
                    b1[j][i] = b[j][i];
                    switch(a[j][i])
                    {
                    case 0:
                        a[j][i + h] = 0;
                        break;
                    case 1:
                        a[j][i + h] = 1;
                        break;
                    case 2:
                        a[j][i + h] = 2;
                        break;
                    case 3:
                        a[j][i + h] = 3;
                        break;
                    case 4:
                        a[j][i + h] = 4;
                        break;
                    case 5:
                        a[j][i + h] = 5;
                        break;
                    case 6:
                        a[j][i + h] = 6;
                        break;
                    case 7:
                        a[j][i + h] = 7;
                        break;
                    case 8:
                        a[j][i + h] = 8;
                        break;
                    case 9:
                        a[j][i + h] = 9;
                        break;
                    default:
                        a[j][i + h] = 10;
                    }
                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(a[j][2*k - 1 - h] == 10 && a[j][2*k - 2 - h] == 10)
                {
                    h++;
                }
                h++;
                for(int i = 0; i < 2*k - h; i++)
                {
                    a[j + 2][h + i]=a[j][i];
                }
                h = 0;
            }
            for(int j = 1; j <= 4; j++)
            {
                for(int i = 0; i < 2*k; i++)
                {
                    if(a[j][i] == 10)
                    {
                        a[j][i] = 0;
                    }
                }
            }
            for (int i = 0; i < k; i++)
            {
                for (int j = 0; j < 2*k;j++)
                {
                    a[5][2*k - 1 - j] = a[3][2*k - 1 - j]*a[4][2*k-  1 - i] + a[5][2*k - 1 - j];

                }
                for(int l = 0; l < 2*k; l++)
                {
                    a[3][l] = a[3][l + 1];
                }
                a[3][2*k - 1]=0;
            }
            for(int i = 0; i < 2*k; i++)
            {
                if(2*k-2 - i < 0);
                else
                {
                    a[6][2*k - 2 - i] = (a[5][2*k - 1 - i] + a[6][2*k - 1 - i])/10;
                }
                a[5][2*k-1-i] = (a[5][2*k-1-i] + a[6][2*k-1-i])%10;
            }
            while(a[5][m] == 0 && a[5][m+1] == 0)
            {
                m++;
            }
            cout << "Result: ";
            if(m == 399)
            {
                cout << "0";
            }
            for(int i = m + 1; i < 2*k; i++)
            {
                cout << a[5][i];
            }
            cout << endl;
            /*mean*/
            int a1[5][101] = {0}, q = 0, n = 0, sum[3][102] = {0}, mean[3][102] = {0};
            for(int j = 1; j <= 2; j++)
            {
                for(int i = 0;i<101;i++)
                {
                    a1[j][i] = b1[j][i] - '0';
                    switch(a1[j][i])
                    {
                    case 0:
                        a1[j][i + q] = 0;
                        break;
                    case 1:
                        a1[j][i + q] = 1;
                        break;
                    case 2:
                        a1[j][i + q] = 2;
                        break;
                    case 3:
                        a1[j][i + q] = 3;
                        break;
                    case 4:
                        a1[j][i + q] = 4;
                        break;
                    case 5:
                        a1[j][i + q] = 5;
                        break;
                    case 6:
                        a1[j][i + q] = 6;
                        break;
                    case 7:
                        a1[j][i + q] = 7;
                        break;
                    case 8:
                        a1[j][i + q] = 8;
                        break;
                    case 9:
                        a1[j][i + q] = 9;
                        break;
                    default:
                        a1[j][i + q] = '\n';
                    }

                }
            }
            for(int j = 1; j <= 2; j++)
            {
                while(a1[j][100 - q] == 10 && a1[j][99 - q] == 10)
                {
                    q++;
                }
                q++;
                for(int i = 0; i < 101 - q; i++)
                {
                    a1[j + 2][q + i] = a1[j][i];
                }
                q = 0;
            }
            for(int j = 1; j <= 4; j++)
            {
                for(int i = 0;i < 101; i++)
                {
                    if(a1[j][i] == '\n')
                    {
                        a1[j][i] = 0;
                    }
                }
            }
            for(int i = 0; i < 101; i++)
            {
                {
                    sum[1][101 - i] = (a1[3][100 - i] + a1[4][100-i] + sum[2][101 - i]) % 10;
                    sum[2][100 - i] = (a1[3][100 - i] + a1[4][100-i] + sum[2][101 - i]) / 10;
                }
            }
            sum[1][0] = sum[2][0];
            while(sum[1][n] == 0 && sum[1][n + 1] == 0)
            {
                n++;
            }
            for(int i = n + 1; i < 102; i++)
            {
                if((sum[1][i] + mean[2][i]) % 2 != 0 && i < 102)
                {
                    mean[2][i + 1] += 10;
                }
                mean[1][i] = (sum[1][i] + mean[2][i]) / 2;
            }
            cout << "Mean: ";
            if(mean[1][n + 1] == 0)
            {
                n++;
            }
            if(n > 100) cout << "0";
            for(int i = n + 1; i < 102; i++)
            {
                cout << mean[1][i];
            }
            cout << endl << endl;
        }
    }
    /*int a[10] = {0};
    char a1[11] = {'\n'};
    cin.getline(a1,11);
    cout << strlen(a1) << endl;
    for (int p = 0, s = strlen(a1); p < 10; p++)
    {
        a[10 - s] = a1[p] - '0';
        s--;
    }
    for (int i = 0; i < 10; i++)
    {
        cout << a[i];
    }
    return 0;*/
}
