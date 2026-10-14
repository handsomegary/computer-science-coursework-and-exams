#include <iostream>
#include<string>

using namespace std;
void show_prompt(void);

void show_add(void);
void add_calculator(int[],int[]);

void show_sub(void);
void sub_calculator(int[],int[]);

void show_mul(void);
void mul_calculator(int[],int[]);

void show_mean(int[],int[]);

int main()
{
    int control;
    while(true)
    {
        show_prompt();
        (cin>>control).get();
        if(control<=0||control>3)
            if(control==0)
            {
                cout<<"Good bye!"<<endl;
                break;
            }
            else
            {
                cout<<"Error! Please try again."<<endl<<endl;
                continue;
            }
        switch(control)
        {
            case 1:show_add();
                break;
            case 2:show_sub();
                break;
            case 3:show_mul();
                break;
        }
    }
    return 0;

}
void show_prompt(void)   //提示訊息
{
    cout<<"----Welcome to big number calculator----"<<endl;
    cout<<"Please choose the function"<<endl;
    cout<<"1. Addition"<<endl;
    cout<<"2. Subtraction"<<endl;
    cout<<"3. Multiplication"<<endl;
    cout<<"0. Exit"<<endl;
    cout<<"--------------------------"<<endl;

}
void show_add(void)  //加法顯示
{
    string a,b;
    int add[100]={0},aug[100]={0};
    cout<<"----Now for Addition----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    getline(cin,a);
    getline(cin,b);
    for(int i=0;i<a.length()/2;i++)
    {
        swap(a[i], a[a.length()-1-i]);
    }

    for(int i=0;i<b.length()/2;i++)
    {
        swap(b[i], b[b.length()-1-i]);
    }
    for(int i=0;i<a.length();i++)
    {
        add[i]=a[i]-'0';

    }
    for(int i=0;i<b.length();i++)
    {
        aug[i]=b[i]-'0';
    }
    cout<<"Result: ";
    add_calculator(add,aug);
    cout<<"Mean: ";
    show_mean(add,aug);
    cout<<endl<<endl;



}
void add_calculator(int a[],int b[])  //加法
{
    int carry=0,c[100]={0},start;
   for(int i=0;i<100;i++)
   {
       c[i]=a[i]+b[i]+carry;
       carry=c[i]/10;
       c[i]%=10;
   }
   for(int i=99;i>=0;i--)
   {
       if(c[i]!=0)
       {
            start=i;
            break;
       }
       if(i==0)  start=0;
   }
   for(int i=start;i>=0;i--)
   {
       cout<<c[i];
   }
   cout<<endl;
   return;

}


void show_sub(void)   //減法顯示
{
    string a,b;
    int minuend[100]={0},subtrahend[100]={0};
    cout<<"----Now for Subtraction ----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    getline(cin,a);
    getline(cin,b);


    if(a.length()>b.length())       //判斷誰大
    {
        for(int i=0;i<a.length()/2;i++)
        {
            swap(a[i], a[a.length()-1-i]);
        }

        for(int i=0;i<b.length()/2;i++)
        {
            swap(b[i], b[b.length()-1-i]);
        }
         for(int i=0;i<a.length();i++)
        {
            minuend[i]=a[i]-'0';

        }
        for(int i=0;i<b.length();i++)
        {
           subtrahend[i]=b[i]-'0';
        }
    }
    else if(a.length()<b.length())
    {
        for(int i=0;i<a.length()/2;i++)
        {
            swap(a[i], a[a.length()-1-i]);
        }

        for(int i=0;i<b.length()/2;i++)
        {
            swap(b[i], b[b.length()-1-i]);
        }
         for(int i=0;i<b.length();i++)
        {
            minuend[i]=b[i]-'0';

        }
        for(int i=0;i<a.length();i++)
        {
            subtrahend[i]=a[i]-'0';
        }
    }
    else if(a>=b)
    {
        for(int i=0;i<a.length()/2;i++)
        {
            swap(a[i], a[a.length()-1-i]);
        }

        for(int i=0;i<b.length()/2;i++)
        {
            swap(b[i], b[b.length()-1-i]);
        }
       for(int i=0;i<a.length();i++)
        {
            minuend[i]=a[i]-'0';

        }
        for(int i=0;i<b.length();i++)
        {
           subtrahend[i]=b[i]-'0';
        }
    }
    else
    {
        for(int i=0;i<a.length()/2;i++)
        {
            swap(a[i], a[a.length()-1-i]);
        }

        for(int i=0;i<b.length()/2;i++)
        {
            swap(b[i], b[b.length()-1-i]);
        }
        for(int i=0;i<b.length();i++)
        {
            minuend[i]=b[i]-'0';

        }
        for(int i=0;i<a.length();i++)
        {
            subtrahend[i]=a[i]-'0';
        }

    }


    cout<<"Result: ";
    sub_calculator(minuend,subtrahend);
    cout<<"Mean: ";
    show_mean(minuend,subtrahend);
    cout<<endl<<endl;
}
void sub_calculator(int a[],int b[])  //減法
{
    int borrow=0,c[100]={0},start;

   for(int i=0;i<100;i++)
   {
       c[i]=a[i]-b[i]-borrow;
       if(c[i]<0)
       {
            borrow=1;
            c[i]+=10;
       }
       else
       {
           borrow=0;
       }

   }
   for(int i=99;i>=0;i--)
   {
       if(c[i]!=0)
       {
            start=i;
            break;
       }
       if(i==0)  start=0;
   }
   for(int i=start;i>=0;i--)
   {
       cout<<c[i];
   }
   cout<<endl;
   return;
}

void show_mul(void)    //顯示乘法
{

    string a,b;
    int  multiplicand[100]={0},multiplicator[100]={0};
    cout<<"----Now for Multiplication----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    getline(cin,a);
    getline(cin,b);
    for(int i=0;i<a.length()/2;i++)
    {
        swap(a[i], a[a.length()-1-i]);
    }

    for(int i=0;i<b.length()/2;i++)
    {
        swap(b[i], b[b.length()-1-i]);
    }
    for(int i=0;i<a.length();i++)
    {
         multiplicand[i]=a[i]-'0';

    }
    for(int i=0;i<b.length();i++)
    {
        multiplicator[i]=b[i]-'0';
    }
    cout<<"Result: ";
    mul_calculator( multiplicand,multiplicator);
    cout<<"Mean: ";
    show_mean(multiplicand,multiplicator);
    cout<<endl<<endl;
}
void mul_calculator(int a[],int b[])   //乘法
{
     int carry=0,c[200]={0},start;
   for(int i=0;i<100;i++)  //乘法規律
        for(int j=0;j<100;j++)
   {
       c[i+j]=a[i]*b[j]+c[i+j];
   }
    for(int i=0;i<100;i++)
   {
       c[i]+=carry;
       carry=c[i]/10;
       c[i]%=10;
   }
   for(int i=99;i>=0;i--)
   {
       if(c[i]!=0)
       {
            start=i;
            break;
       }
       if(i==0)  start=0;
   }
   for(int i=start;i>=0;i--)
   {
       cout<<c[i];
   }
   cout<<endl;
   return;
}
void show_mean(int a[],int b[])
{
    int carry=0,c[100],start;
   for(int i=0;i<100;i++)
   {
       c[i]=a[i]+b[i]+carry;
       carry=c[i]/10;
       c[i]%=10;
   }
    for(int i=99;i>=0;i--)
   {
       if(c[i]!=0)
       {
            start=i;
            break;
       }
   }
   int counter=0;
   for(int i=start;i>=0;i--)
   {
       c[i]+=counter;
       if(c[i]%2!=0)
       {
          c[i]-=1;
          counter=10;
       }
       else
       {
           counter=0;
       }
   }
   for(int i=99;i>=0;i--)
   {
       if(c[i]!=0)
       {
            start=i;
            break;
       }
       if(i==0)  start=0;
   }
   for(int i=start;i>=0;i--)
   {
       c[i]/=2;
       cout<<c[i];
   }
    return;

}
