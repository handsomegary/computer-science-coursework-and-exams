// Repaired anonymous reference. See ../README.md for bugs and provenance.
#include <iostream>
#include<string>

using namespace std;

// Validate before converting characters to decimal digits. The latest handout
// permits positive inputs of at most 100 digits; leading zeroes are normalized.
bool valid_number(const string& value) {
    if (value.empty() || value.size() > 100) return false;
    bool nonzero = false;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] < '0' || value[i] > '9') return false;
        if (value[i] != '0') nonzero = true;
    }
    return nonzero;
}
string normalized(string value) {
    size_t first = value.find_first_not_of('0');
    return first == string::npos ? "0" : value.substr(first);
}
// Preserve integer menu spellings such as +1, 01 and -0 without relying on
// native integer conversion, stream failure recovery, or signed overflow.
bool selection_value(const string& value, int maximum, int& selection) {
    if (value.empty()) return false;
    size_t position = 0;
    bool negative = false;
    if (value[0] == '+' || value[0] == '-') {
        negative = value[0] == '-';
        position = 1;
    }
    if (position == value.size()) return false;
    int result = 0;
    for (; position < value.size(); ++position) {
        if (value[position] < '0' || value[position] > '9') return false;
        // result is <= maximum before multiplication; maximum is only 3/4.
        result = result * 10 + value[position] - '0';
        if (result > maximum) return false;
    }
    if (negative && result != 0) return false;
    selection = result;
    return true;
}
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
    string selection;
    while(true)
    {
        show_prompt();
        if (!(cin >> selection)) break;
        if (!selection_value(selection, 3, control)) {
            cout << "Error! Please try again." << endl << endl;
            continue;
        }
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
    int add[101]={0},aug[101]={0};
    cout<<"----Now for Addition----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    if (!(cin >> a >> b)) return;
    if (!valid_number(a) || !valid_number(b)) {
        cout << "Error! Please try again." << endl << endl;
        return;
    }
    a = normalized(a);
    b = normalized(b);
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
    int carry=0,c[101]={0},start;
   for(int i=0;i<101;i++)
   {
       c[i]=a[i]+b[i]+carry;
       carry=c[i]/10;
       c[i]%=10;
   }
   for(int i=100;i>=0;i--)
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
    int minuend[101]={0},subtrahend[101]={0};
    cout<<"----Now for Subtraction ----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    if (!(cin >> a >> b)) return;
    if (!valid_number(a) || !valid_number(b)) {
        cout << "Error! Please try again." << endl << endl;
        return;
    }
    a = normalized(a);
    b = normalized(b);


    const bool negative = a.size() < b.size() ||
                          (a.size() == b.size() && a < b);
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
    if (negative) cout << "-";
    sub_calculator(minuend,subtrahend);
    cout<<"Mean: ";
    show_mean(minuend,subtrahend);
    cout<<endl<<endl;
}
void sub_calculator(int a[],int b[])  //減法
{
    int borrow=0,c[101]={0},start;

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
    int  multiplicand[101]={0},multiplicator[101]={0};
    cout<<"----Now for Multiplication----"<<endl;
    cout<<"Please enter the two numbers."<<endl;
    if (!(cin >> a >> b)) return;
    if (!valid_number(a) || !valid_number(b)) {
        cout << "Error! Please try again." << endl << endl;
        return;
    }
    a = normalized(a);
    b = normalized(b);
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
    for(int i=0;i<200;i++)
   {
       c[i]+=carry;
       carry=c[i]/10;
       c[i]%=10;
   }
   for(int i=199;i>=0;i--)
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
    int carry=0,c[101]={0},start=0;
   for(int i=0;i<101;i++)
   {
       c[i]=a[i]+b[i]+carry;
       carry=c[i]/10;
       c[i]%=10;
   }
    for(int i=100;i>=0;i--)
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
   for(int i=100;i>=0;i--)
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
