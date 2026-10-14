#include <iostream>
#include <cmath>

using namespace std;

int ray[5][31]={{0},{0},{0},{0},{0}};

int xo(int a, int b)  //definition of Xor gate
    {
    if(a==b) return 1;
    else return 0;
    }

int an(int a, int b)  //definition of And gate
    {
    if(a==0||b==0) return 0;
    else return 1;
    }

void binary( int dec,int j)  //this function is going to convert the decimal number into binary then print it.
{

    if(dec>0)
        {
        for(int i=0;i<=30;i++)
            {
             ray[j][30-i]=dec%2;
             dec/=2;
            }
        }
     else//this part is going to convert the decimal number into  2's complement  form.
        {
            if(dec==-1) cout<<"11";
            else
            {
                dec*=-1;
                dec--;
                for(int i=0;i<=30;i++)
                {
                ray[j][30-i]=(dec+1)%2;
                dec/=2;
                }
            }
        }
    int g=0;
    while(ray[j][g]-ray[j][g+1]==0)g++;

    for(int t=g-1;t<30;t++)
        {       cout<<ray[j][t+1];     }
}

void fullader(int a,int b,int c,int d=0)   //this function is full adder.
{
    ray[3][d]=xo(xo(a,b),c);//sum
    c=an(a,b)+an(b,c)+an(a,c);//cout
    if (c>0) ray[4][d-1]=1;
    else ray[4][d-1]=0;
}

void result()   //the function is going to display  the final result by combing all the full adders.
{
    for(int j=0;j<=30;j++)
    {
        fullader(ray[1][30-j],ray[2][30-j],ray[4][30-j],30-j);
    }
     ray[3][30]=xo(xo(ray[1][30],ray[2][30]),ray[4][30]);
     int g=0;
     while(ray[3][g]-ray[3][g+1]==0)g++;

     for(int t=g;t<30;t++)
     {       cout<<ray[3][t+1];     }
}

void c2(int c1)        //this function is going to display the ordinal information.
{
    switch (c1)
    {
    case 1: cout<<"1st";break;
    case 2: cout<<"2sc";break;
    case 3: cout<<"3rd";break;
    default:cout<<c1<<"th";break;
    }

}

int main()
{
   long long int c1=1,dec,dec_s=0;

    cout<< "*** Welcome to Binary adder Subtractor ***"<<endl;
    while(dec!=0)
    {


        cout <<"Please enter the ";
        c2(c1);
        cout<<" number in decimal: ";
        cin >> dec;

        if(dec==0) break;
        while(dec>=2147483647||dec<-2147483647)
        {
            cout<<"The decimal number you entered is out of range. Please enter the ";
            c2(c1);
            cout<<"\ndecimal number again: ";
            cin >> dec;

        }

        while (dec_s+dec>=2147483647)
        {
           cout<<"The partial sum is out of range. Please enter the ";
            c2(c1);
            cout<<"\ndecimal number again: ";
            cin >> dec;
        }


        if(dec_s!=0)
        {   cout<<"Calculating... "; binary(dec_s,1); cout<< " + " ; binary(dec,2);cout <<" = ";binary(dec_s+dec,3);cout<<endl;
            cout<<"Partial sum: "<<dec+dec_s<<endl<<endl;

        }dec_s+=dec;


        c1++;
        for(int i=1;i<=4;i++)
        {
            for(int j=0;j<=30;j++)
            {
                ray[i][j]=0;
            }
        }
    }
    cout<<"bye~";

}
