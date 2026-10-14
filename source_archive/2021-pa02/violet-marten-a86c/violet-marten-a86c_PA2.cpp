#include <iostream>
#include <cmath>

using namespace std;

int ray[5][31]={{0},{0},{0},{0},{0}};  /*the first row is going to keep the binary value of the accumulation .
                                          the second row is going to keep the value entered.
                                          the third row is going to keep the sum through full adders.
                                          the forth row is going to keep the cout though full adder.*/
long long int paw(int a,int b)//the function is going to calculate the degreed.
{
    if(b==0) return 1;
    return a*paw(a,b-1);
}

int xo(int a, int b)  //definition of Xor gate
    {
    if(a==b) return 0;
    else return 1;
    }

int an(int a, int b)  //definition of And gate
    {
    if(a==0||b==0) return 0;
    else return 1;
    }
long long int  binary( int dec,int j)  //this function is going to convert the decimal number then print it.
{
    long long int bin=0,deg=0,dec2=dec;
    for(int i=0;i<=30;i++)
        {
            ray[j][30-i]=dec%2;
            dec/=2;
        }
    while(dec2>0)
    {
     bin+=(dec2%2)*paw(10,deg);//the recursion is applied to calculating the degreed.
     dec2=dec2/2;
     deg++;

    }return (bin);
}
 void fulladder(int a,int b,int c,int d)   //this function is full adder.
{
    ray[3][d]=xo(xo(a,b),c);//sum
    c=an(a,b)+an(b,c)+an(a,c);//cout
    if (c>0) ray[4][d-1]=1;
    else ray[4][d-1]=0;
}


long long int result()   //the function is going to display  the final result by combing all the full adders.
{
    long long int bin=0;
    for(int j=0;j<=30;j++)
    {
        fulladder(ray[1][30-j],ray[2][30-j],ray[4][30-j],30-j);
    }
     ray[3][30]=xo(xo(ray[1][30],ray[2][30]),ray[4][30]);
     int g=0;
     while(ray[3][g]-ray[3][g+1]==0)g++;

     for(int t=g;t<30;t++)
     {       bin+=ray[3][t+1]*paw(10,29-t);     } //the recursion is applied to calculate the degreed//store the
     return bin;
}


int  dec2()             //this function is going to convert the result from binary to decimal.
{   int dec=0;
    for(int i=0;i<=30;i++)
    {
        dec+=ray[3][i]*paw(2,30-i);  //the recursion is applied to calculate the degreed
    }
    return dec;
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
   long  int c1=1,dec=0,dec_s=0;

    cout<< "*** Welcome to Positive Binary adder ***"<<endl;
    while(dec!=-1)
    {


        cout <<"please enter the ";
        c2(c1);
        cout<<" number in decimal: ";
        cin >> dec;

        if(dec==-1) break;
        while(dec>=2147483647||dec<0)
        {
            cout<<"The decimal number you entered is out of range. Please enter the ";
            c2(c1);
            cout<<"\ndecimal number again: ";
            cin >> dec;

        }

        while (dec_s+dec>=524288)
        {
           cout<<"The partial sum is out of range. Please enter the ";
            c2(c1);
            cout<<"\ndecimal number again: ";
            cin >> dec;
        }

        ;
        if(dec_s!=0)
        {   cout<<"Calculating... "<<binary(dec_s,1)<< " + " << binary(dec,2);cout <<" = "<<result()<<endl;
            cout<<"Partial sum: "<<dec2()<<endl<<endl;

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
