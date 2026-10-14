#include <iostream>
#include <cstdlib>
#include <iomanip>
using namespace std;

void add();//this function is going to calculate the addition of two number.
void sub();//this function is going to calculate the subtraction of two number, the result is positive only.
void mul();////this function is going to calculate the multplication of two number.
void mean(char bray[3][101]);//this function is going to calculate the average of two number.

int main()
{
    int choosen;//stand for chosen function.

    while(true)
    {
        int i=1;
        while(i==1)//the following block is fool provent.
            {
            cout<<"----Welcome to big number calculator----"<<endl;
            cout<<"Please choose the function"<<endl;
            cout<<"1. Addition"<<endl;
            cout<<"2. Subtraction"<<endl;
            cout<<"3. Multiplication"<<endl;
            cout<<"0. Exit"<<endl;
            cout<<"--------------------------"<<endl;

            (cin>>choosen).get();

            if(choosen>3||choosen<0)
                {
                cout<<"Error! Please try again."<<endl<<endl;
                i=1;
                }
            else i=0;
            }
        switch(choosen)
        {
            case 1:add();break;
            case 2:sub();break;
            case 3:mul();break;


            default: cout<<"Good bye!";return 0;
        }
    }
    return 0;
}

void add()
{
    int aray[5][101]={0},k=0,l=0,sum[3][102]={0};//aray[1] is original data1 like 1234000...
    char bray[3][101]={'a'};                    //aray[2] is original data2 like 5678000....
    cout<<"----Now for Addition----"<<endl;     //aray[3] is arranged data1 like ...001234
    cout<<"Please enter two numbers\n";          //aray[4] is arranged data2 like ...005678
    cin.getline(bray[1],101);                    //sum[1] is the result of the addition.
    cin.getline(bray[2],101);                    //sum[2] is the carry in the calculation.
    for(int j=1;j<=2;j++)
    {
       for(int i=0;i<101;i++)
        {
            aray[j][i]=bray[j][i];
            switch(aray[j][i])
            {
            case 48: aray[j][i+k]=0;break;
            case 49: aray[j][i+k]=1;break;
            case 50: aray[j][i+k]=2;break;
            case 51: aray[j][i+k]=3;break;
            case 52: aray[j][i+k]=4;break;
            case 53: aray[j][i+k]=5;break;
            case 54: aray[j][i+k]=6;break;
            case 55: aray[j][i+k]=7;break;
            case 56: aray[j][i+k]=8;break;
            case 57: aray[j][i+k]=9;break;
            default: aray[j][i+k]=10;
            }
        }
    }
      for(int j=1;j<=2;j++) //the following three loop is aim to arrange the data from 12340000...to ...0001234
      {
          while(aray[j][100-k]==10&&aray[j][99-k]==10)k++;
       k++;
       for(int i=0;i<101-k;i++)
       {
           aray[j+2][k+i]=aray[j][i];
       }
       k=0;
      }

    for(int j=1;j<=4;j++)
        {
            for(int i=0;i<101;i++)if(aray[j][i]==10)aray[j][i]=0;
        }

    for(int i=0;i<101;i++) //the following loop is going to execute the calculation.
    {
        {
            sum[1][101-i]=(aray[3][100-i]+aray[4][100-i]+sum[2][101-i])%10;//sum
            sum[2][100-i]=(aray[3][100-i]+aray[4][100-i]+sum[2][101-i])/10;//carry
        }
    }
    sum[1][0]=sum[2][0];

 while(sum[1][l]==0&&sum[1][l+1]==0)l++;//the following loop is going to check the digit of the data in order to know how many number should I print and print them.
       cout<<"Result: ";
       for(int i=l+1;i<102;i++)
       {
           cout<<sum[1][i];
       }
        cout<<endl;
        mean(bray);//calculate the meant of two number.

}
void sub()//this function is similar to "add"and,  the variable and array can be compared to add
{
    int aray[5][101]={0},k=0,l=0,sub[3][102]={0};//the arrangement of aray is similar to "aray" in addition
    char bray[3][101]={'a'};                     //the arrangement of sub is similar to "add" in addition
    cout<<"----Now for Subtraction----"<<endl;
    cout<<"Please enter ywo numbers\n";
    cin.getline(bray[1],101);
    cin.getline(bray[2],101);

    for(int j=1;j<=2;j++)
    {
       for(int i=0;i<101;i++)
        {
            aray[j][i]=bray[j][i];
            switch(aray[j][i])
            {
            case 48: aray[j][i+k]=0;break;
            case 49: aray[j][i+k]=1;break;
            case 50: aray[j][i+k]=2;break;
            case 51: aray[j][i+k]=3;break;
            case 52: aray[j][i+k]=4;break;
            case 53: aray[j][i+k]=5;break;
            case 54: aray[j][i+k]=6;break;
            case 55: aray[j][i+k]=7;break;
            case 56: aray[j][i+k]=8;break;
            case 57: aray[j][i+k]=9;break;
            default: aray[j][i+k]=10;
            }
        }
    }



      for(int j=1;j<=2;j++)
      {
          while(aray[j][100-k]==10&&aray[j][99-k]==10)k++;
       k++;
       for(int i=0;i<101-k;i++)
       {
           aray[j+2][k+i]=aray[j][i];
       }
       k=0;
      }
       for(int j=1;j<=4;j++)
        {
            for(int i=0;i<101;i++)if(aray[j][i]==10)aray[j][i]=0;
        }

    for(int i=0;i<101;i++)//the most significant difference between the "add" and "sub" is there calculation process
    {
        {
            if((aray[3][100-i]-aray[4][100-i]-sub[2][100-i])<0) {sub[1][100-i]=aray[3][100-i]-aray[4][100-i]-sub[2][100-i]+10;sub[2][99-i]++;}
            else{sub[1][100-i]=aray[3][100-i]-aray[4][100-i]-sub[2][100-i];}

        }
      }

 while(sub[1][l]==0&&sub[1][l+1]==0)l++;

        cout<<"Result: ";
       for(int i=l+1;i<101;i++)
       {
           cout<<sub[1][i];
       }

    cout<<endl;
     mean(bray);
}

void mul()//this function is similar to "add" and, the variable and array can be compared to add
{
    cout<<"----Now for Multiplication----"<<endl;

     const int k=100;
    int aray[7][2*k]={{0}},h=0,m=0;/*  aray[1][] entered data1
                                        aray[2][] entered data2
                                        aray[3][] arranged data1
                                        aray[4][] arranged data2
                                        aray[5][] result
                                        aray[6][] carry*/
    char bray[3][2*k]={'a'},bray2[3][101]={'a'};
    cout<<"Please enter two numbers\n";
    cin.getline(bray[1],101);
    cin.getline(bray[2],101);
    for(int j=1;j<=2;j++)
    {
       for(int i=0;i<2*k;i++)
        {
            aray[j][i]=bray[j][i];
            bray2[j][i]=bray[j][i];
            switch(aray[j][i])
            {
            case 48: aray[j][i+h]=0;break;
            case 49: aray[j][i+h]=1;break;
            case 50: aray[j][i+h]=2;break;
            case 51: aray[j][i+h]=3;break;
            case 52: aray[j][i+h]=4;break;
            case 53: aray[j][i+h]=5;break;
            case 54: aray[j][i+h]=6;break;
            case 55: aray[j][i+h]=7;break;
            case 56: aray[j][i+h]=8;break;
            case 57: aray[j][i+h]=9;break;
            default: aray[j][i+h]=10;
            }
        }
    }

      for(int j=1;j<=2;j++)
      {
          while(aray[j][2*k-1-h]==10&&aray[j][2*k-2-h]==10)h++;
          h++;
       for(int i=0;i<2*k-h;i++)
       {
           aray[j+2][h+i]=aray[j][i];
       }
       h=0;
      }
      for(int j=1;j<=4;j++)
        {
            for(int i=0;i< 2*k;i++)if(aray[j][i]==10)aray[j][i]=0;
        }
   for (int i=0;i<k;i++)
{
     for (int j=0;j<2*k;j++)
    {
        aray[5][2*k-1-j]=aray[3][2*k-1-j]*aray[4][2*k-1-i]+ aray[5][2*k-1-j];

    }

    for(int l=0;l< 2*k;l++)
         {
         aray[3][l]=aray[3][l+1];

         }
        aray[3][2*k-1]=0;
}
for(int i=0;i< 2*k;i++)//the most significant difference between the "add" and "mult" is there calculation process
    {

        if(2*k-2-i<0);
        else aray[6][2*k-2-i]=(aray[5][2*k-1-i]+aray[6][2*k-1-i])/10;

        aray[5][2*k-1-i]=(aray[5][2*k-1-i]+aray[6][2*k-1-i])%10;
    }

while(aray[5][m]==0&&aray[5][m+1]==0)m++;
       cout<<"Result: ";
       for(int i=m+1;i<2*k;i++)
       {
           cout<<aray[5][i];
       }

cout<<endl;
 mean(bray2);


}

void mean(char bray[3][101])//this function is similar to "add" and, the variable and array can be compared to add
{
    int aray[5][101]={0},k=0,l=0,sum[3][102]={0},mean[3][102]={0};
    for(int j=1;j<=2;j++)
    {
       for(int i=0;i<101;i++)
        {
            aray[j][i]=bray[j][i];
            switch(aray[j][i])
            {
            case 48: aray[j][i+k]=0;break;
            case 49: aray[j][i+k]=1;break;
            case 50: aray[j][i+k]=2;break;
            case 51: aray[j][i+k]=3;break;
            case 52: aray[j][i+k]=4;break;
            case 53: aray[j][i+k]=5;break;
            case 54: aray[j][i+k]=6;break;
            case 55: aray[j][i+k]=7;break;
            case 56: aray[j][i+k]=8;break;
            case 57: aray[j][i+k]=9;break;
            default: aray[j][i+k]=10;
            }
        }
    }
      for(int j=1;j<=2;j++)
      {
          while(aray[j][100-k]==10&&aray[j][99-k]==10)k++;
       k++;
       for(int i=0;i<101-k;i++)
       {
           aray[j+2][k+i]=aray[j][i];
       }
       k=0;
      }

    for(int j=1;j<=4;j++)
        {
            for(int i=0;i<101;i++)if(aray[j][i]==10)aray[j][i]=0;
        }

    for(int i=0;i<101;i++)
    {
        {
            sum[1][101-i]=(aray[3][100-i]+aray[4][100-i]+sum[2][101-i])%10;//sum
            sum[2][100-i]=(aray[3][100-i]+aray[4][100-i]+sum[2][101-i])/10;//carry
        }
    }
    sum[1][0]=sum[2][0];

 while(sum[1][l]==0&&sum[1][l+1]==0)l++;
        for(int i=l+1;i<102;i++)//the most significant difference between the "add" and "mean" is there calculation process
       {
           if((sum[1][i]+mean[2][i])%2!=0&&i<102)mean[2][i+1]+=10;
           mean[1][i]=(sum[1][i]+mean[2][i])/2;
       }
       cout<<"Mean: ";
       if(mean[1][l+1]==0)l++;
       for(int i=l+1;i<102;i++)
       {
           cout<<mean[1][i];
       }
        cout<<endl;

}
