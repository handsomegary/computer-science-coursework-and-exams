//PA2
#include <iostream>
#include <cmath>

using namespace std;
long long int decimal_to_binary(int);
long long int binary_to_decimal(long long int);
long long int binary_adder(long long int,long long int,int c=0);
void prompt(int);
void error_prompt(int);
void sum_error_prompt(int);

int main(void)
{
    int number1=0,number2=0,sum=0,sum_memory=0;
    long long int binary_number1=0,binary_number2=0,binary_sum=0,binary_sum_memory=0;
    int i=1;   //1st...2nd...3rd...4th...
    cout<<"*** Welcome to Positive Binary adder ***"<<endl;
    prompt(i);
    cin>>number1;
    if(number1==-1) cout<<"Bye"<<endl;

    while(number1<-1 || binary_to_decimal(decimal_to_binary(number1))!=number1)
    {
        error_prompt(i);
        cin>>number1;
        if(number1==-1)
        {
            cout<<"Bye"<<endl;
            break;
        }


    }//end while
    if(number1!=-1)
    {
        prompt(i+1);
        cin>>number2;
        if(number2==-1)
            cout<<"Bye"<<endl;
        else
        {
            sum=number1+number2;
            sum_memory=sum;
            while(number2<-1 || binary_to_decimal(decimal_to_binary(sum))!=sum ||binary_to_decimal(decimal_to_binary(number2))!=number2)
            {
                if(binary_to_decimal(decimal_to_binary(number2))!=number2 || number2<-1)
                    error_prompt(i+1);
                else if(binary_to_decimal(decimal_to_binary(sum))!=sum )
                    sum_error_prompt(i+1);

                cin>>number2;
                if(number2==-1)
                {
                    cout<<"Bye"<<endl;
                    break;
                }

                sum=number1+number2;
                sum_memory=sum;
            }//end while
            if(number2!=-1)
            {
                binary_number1=decimal_to_binary(number1);
                binary_number2=decimal_to_binary(number2);
                binary_sum    =binary_adder(binary_number1,binary_number2);
                binary_sum_memory=binary_sum;
                sum=binary_to_decimal(binary_sum);
                cout<<"Calculating... "<<binary_number1<<" + "<<binary_number2<<" = "<<binary_sum<<endl;
                cout<<"Partial Sum: "<<sum<<endl<<endl;
            }

        }
     }


        i=3;
    while(number1!=-1&&number2!=-1)
    {
        prompt(i);
        cin>>number1;
        sum=sum_memory+number1;
        while(number1<-1 || binary_to_decimal(decimal_to_binary(sum))!=sum || binary_to_decimal(decimal_to_binary(number1))!=number1 )
        {
            if(number1==-1)
                break;
            if(binary_to_decimal(decimal_to_binary(number1))!=number1 ||number1<-1 )
                error_prompt(i);

            else if(binary_to_decimal(decimal_to_binary(sum))!=sum )
                sum_error_prompt(i);
            cin>>number1;


            sum=sum_memory+number1;
        }//end while
        if(number1==-1)
        {
            cout<<"Bye"<<endl;
            break;
        }
        binary_number1=decimal_to_binary(number1);
        binary_sum    =binary_adder(binary_number1,binary_sum_memory);
        sum=binary_to_decimal(binary_sum);
        cout<<"Calculating... "<<binary_sum_memory<<" + "<<binary_number1<<" = "<<binary_sum<<endl;
        cout<<"Partial Sum: "<<sum<<endl<<endl;
        sum_memory=sum;
        binary_sum_memory=binary_sum;
        i++;
    }  //end while

    return 0;







}


long long int decimal_to_binary(int number)     //decimal translate to binary
{
    if(number/2!=0)
        return number%2+10*decimal_to_binary(number/2);
    else
        return (number%2);
}

long long int binary_to_decimal(long long int number)     //binary translate to decimal
{
     if(number/10!=0)
        return number%10+2*binary_to_decimal(number/10);
    else
        return (number%10);

}

long long int binary_adder(long long int number1,long long int number2,int c)           //binary adder
{
    int i=number1%10;
    int j=number2%10;

    if(number1!=0||number2!=0||c!=0)
        return (i+j+c)%2+10*binary_adder(number1/10,number2/10,(i+j+c)/2);
    else
        return 0;
}
void prompt(int i)
{
    switch(i)
    {
        case 1: cout<<"Please enter the 1st number in decimal: ";
            break;
        case 2: cout<<"Please enter the 2nd number in decimal: ";
            break;
        case 3: cout<<"Please enter the 3rd number in decimal: ";
            break;
        default:cout<<"Please enter the "<<i<<"th number in decimal: ";
    }
    return;
}
void error_prompt(int i)
{
    switch(i)
    {
        case 1: cout<<"The decimal number you entered is out of range. Please enter the 1st decimal number again: ";
            break;
        case 2: cout<<"The decimal number you entered is out of range. Please enter the 2nd decimal number again: ";
            break;
        case 3: cout<<"The decimal number you entered is out of range. Please enter the 3rd decimal number again: ";
            break;
        default:cout<<"The decimal number you entered is out of range. Please enter the "<<i<<"th decimal number again: ";
    }
    return;
}
void sum_error_prompt(int i)
{
    switch(i)
    {
        case 1: cout<<"The partial sum is out of range. Please enter the 1st decimal number again: ";
            break;
        case 2: cout<<"The partial sum is out of range. Please enter the 2nd decimal number again: ";
            break;
        case 3: cout<<"The partial sum is out of range. Please enter the 3rd decimal number again: ";
            break;
        default:cout<<"The partial sum is out of range. Please enter the "<<i<<"th decimal number again: ";
    }
    return;

}
