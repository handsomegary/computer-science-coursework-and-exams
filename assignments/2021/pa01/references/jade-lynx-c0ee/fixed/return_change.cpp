// Repaired anonymous reference; original archival source is adjacent.
#include <iostream>
#include<string>
using namespace std;
                                    //   function begin
void price_set(int []);            //    set price
void data_set(string []);        //set data
void price_count(int [],string []);   //count price
bool price_judge(int [],int []);  //judge price
void price_payment(int [],int []);  //the method of payment
void voucher_show_remain(int []);//show remain vouchers
void voucher_show_use(int []);    //show use voucher
void voucher_show_change(int []);   //show change_voucher
int voucher_count(int[]);   //   count remain vouchers
void voucher_change_count(int,int []);  //voucher change



    //main beginning ********************************************************************
int main(void)
{
    int price[1]={0};
    int voucher[5]={5,5,5,5,5}; //1$,5$,10$,50$,100$±i¼Æ
    string  data[2];

    cout<<"*** Welcome to Quintuple Stimulus Vouchers System ***"<<endl;
    while(price[0]!=-1&&voucher_count(voucher)!=0)
    {
        price_set(price);
        if(price[0]==-1)
        {
            cout<<"Bye~"<<endl;
            break;
        }
        data_set(data);
        if (!cin) return 0;
        price_count(price,data);
        if(price_judge(price,voucher)==false)
        {
            cout<<"You don't have enough voucher to buy the good"<<endl<<endl;
            continue;
        }
        price_payment(price,voucher);
        if(voucher_count(voucher)!=0)            //voucher還有錢
        {
            voucher_show_remain(voucher);
            cout<<endl;
        }
        else                                     //vouchers為零時 顯示You do not remain any voucher，Bye~
        {
            cout<<"You do not remain any voucher"<<endl<<"Bye~"<<endl;
        }
    } //end while
    return 0;


}        //end of main  ********************************************************************



         //start to write function
void price_set(int price[])
{
    cout << "Please enter the good price:";
    string token;
    while (cin >> token) {
        if (token == "-1") { price[0] = -1; return; }
        long long value = 0;
        bool valid = !token.empty();
        for (char digit : token) {
            if (digit < '0' || digit > '9' || value > 214748364LL ||
                (value == 214748364LL && digit > '7')) {
                valid = false;
                break;
            }
            value = value * 10 + (digit - '0');
        }
        if (valid) { price[0] = static_cast<int>(value); return; }
        cout << "Please enter the correct price:";
    }
    price[0] = -1;
}
void data_set(string data[])        //set data
{
   do
    {
        cout<<"Please enter the type of vouchers (d/p):";
        if (!(cin>>data[0])) return;
        if(data[0]!="d"&&data[0]!="p")
            cout<<"Please enter the correct type of vouchers"<<endl;
        if(data[0]=="d")
        {
            do
            {
                cout<<"Are you a member of NCU Bank (y/n):";
                if (!(cin>>data[1])) return;
                if(data[1]!="y"&&data[1]!="n")
                    cout<<"Please input the correct data"<<endl;
            }while(data[1]!="y"&&data[1]!="n");

        }
        else
        {
            data[1]="n";
        }
    }while(data[0]!="d"&&data[0]!="p");
    return;
}
void price_count(int price[],string data[])   //count price
{
    if(data[0]=="p")
        price[0]=static_cast<int>(static_cast<long long>(price[0])*90/100);
    else if(data[1]=="y")
            price[0]=static_cast<int>(static_cast<long long>(price[0])*72/100);
        else
            price[0]=static_cast<int>(static_cast<long long>(price[0])*80/100);

    cout<<"The price you need to pay:"<<price[0]<<endl;
    return;
}
bool price_judge(int price[],int voucher[])  //judge price
{
    if(price[0]>voucher_count(voucher))
        return false;
    else
        return true;
}
void price_payment(int price[],int voucher[])  //the method of payment  using brute_force
{   if (price[0] == 0) return;
    int sum;
    int sum_max=price[0];
    int voucher_quantity,voucher_quantity_min=25;
    int voucher_pay[5]={0,0,0,0,0};
    for(int i=0;i<=voucher[0];i++)                      //1$
    {
        for(int j=0;j<=voucher[1];j++)                  //5$
        {
            for(int k=0;k<=voucher[2];k++)              //10$
            {
                for(int h=0;h<=voucher[3];h++)           //50$
                {
                    for(int e=0;e<=voucher[4];e++)       //100$
                    {
                        voucher_quantity=i+j+k+h+e;
                        sum=i*1+j*5+k*10+h*50+e*100;
                        if(sum==price[0]&&voucher_quantity_min>=voucher_quantity)
                        {
                            voucher_quantity_min=voucher_quantity;
                            voucher_pay[0]=i;
                            voucher_pay[1]=j;
                            voucher_pay[2]=k;
                            voucher_pay[3]=h;
                            voucher_pay[4]=e;
                        } //end   if

                    }      //end for e loop
                }          //end for h loop
             }              //end for k loop
         }                   //end for j loop
     }                     //end for i loop
     if(voucher_pay[0]!=0||voucher_pay[1]!=0||voucher_pay[2]!=0||voucher_pay[3]!=0||voucher_pay[4]!=0)    // judge if price =voucher
     {
        voucher_show_use(voucher_pay);
        voucher[0]=voucher[0]-voucher_pay[0];
        voucher[1]=voucher[1]-voucher_pay[1];
        voucher[2]=voucher[2]-voucher_pay[2];
        voucher[3]=voucher[3]-voucher_pay[3];
        voucher[4]=voucher[4]-voucher_pay[4];

        return;
     }                                                   //vouchers !=price
     for(int i=0;i<=voucher[0];i++)                      //1$
     {
        for(int j=0;j<=voucher[1];j++)                  //5$
        {
            for(int k=0;k<=voucher[2];k++)              //10$
            {
                for(int h=0;h<=voucher[3];h++)           //50$
                {
                    for(int e=0;e<=voucher[4];e++)       //100$
                    {
                        voucher_quantity=i+j+k+h+e;      //張數
                        sum=i*1+j*5+k*10+h*50+e*100;
                        if(sum>price[0]&&voucher_quantity_min>=voucher_quantity)
                            if(sum_max<=sum)
                            {
                                sum_max=sum;
                                voucher_quantity_min=voucher_quantity;
                                voucher_pay[0]=i;
                                voucher_pay[1]=j;
                                voucher_pay[2]=k;
                                voucher_pay[3]=h;
                                voucher_pay[4]=e;
                            } //end   if

                    }      //end for e loop
                }          //end for h loop
             }              //end for k loop
         }                   //end for j loop
     }                     //end for i loop
     int difference=sum_max-price[0];    //差額計算
     int voucher_change[5]={0,0,0,0,0};
     if(price[0]!=0)
     {
        voucher_show_use(voucher_pay);
        voucher_change_count(difference,voucher_change);
        voucher_show_change(voucher_change);
     }

     voucher[0]=voucher[0]-voucher_pay[0]+voucher_change[0];
     voucher[1]=voucher[1]-voucher_pay[1]+voucher_change[1];
     voucher[2]=voucher[2]-voucher_pay[2]+voucher_change[2];
     voucher[3]=voucher[3]-voucher_pay[3]+voucher_change[3];
     voucher[4]=voucher[4]-voucher_pay[4]+voucher_change[4];

     return;

}
void voucher_show_remain(int voucher[])    //show vouchers
{
    cout<<"The remaining vouchers are as follows:"<<endl;
    if(voucher[4]!=0)
        cout<<"    "<<voucher[4]<<" $ 100 voucher"<<endl;
    if(voucher[3]!=0)
        cout<<"    "<<voucher[3]<<" $  50 voucher"<<endl;
    if(voucher[2]!=0)
        cout<<"    "<<voucher[2]<<" $  10 voucher"<<endl;
    if(voucher[1]!=0)
        cout<<"    "<<voucher[1]<<" $   5 voucher"<<endl;
    if(voucher[0]!=0)
        cout<<"    "<<voucher[0]<<" $   1 voucher"<<endl;
    return;

}
void voucher_show_use(int voucher[])
{
    cout<<"The payment is processed as follows:"<<endl;
    if(voucher[4]!=0)
        cout<<"    "<<voucher[4]<<" $ 100 voucher"<<endl;
    if(voucher[3]!=0)
        cout<<"    "<<voucher[3]<<" $  50 voucher"<<endl;
    if(voucher[2]!=0)
        cout<<"    "<<voucher[2]<<" $  10 voucher"<<endl;
    if(voucher[1]!=0)
        cout<<"    "<<voucher[1]<<" $   5 voucher"<<endl;
    if(voucher[0]!=0)
        cout<<"    "<<voucher[0]<<" $   1 voucher"<<endl;
    return;
}
int voucher_count(int voucher[])     //count remain vouchers
{
    int sum;
    sum=voucher[0]*1+voucher[1]*5+voucher[2]*10+voucher[3]*50+voucher[4]*100;
    return sum;
}
void voucher_change_count(int difference,int voucher[])
{
    int fifty,ten,five,one;
    voucher[4]=difference/100;
    fifty=difference%100;
    voucher[3]=fifty/50;
    ten=fifty%50;
    voucher[2]=ten/10;
    five=ten%10;
    voucher[1]=five/5;
    one=five%5;
    voucher[0]=one;
    return;
}
void voucher_show_change(int voucher[])
{
    cout<<"Change is as follows:"<<endl;
    if(voucher[4]!=0)
        cout<<"    "<<voucher[4]<<" $ 100 voucher"<<endl;
    if(voucher[3]!=0)
        cout<<"    "<<voucher[3]<<" $  50 voucher"<<endl;
    if(voucher[2]!=0)
        cout<<"    "<<voucher[2]<<" $  10 voucher"<<endl;
    if(voucher[1]!=0)
        cout<<"    "<<voucher[1]<<" $   5 voucher"<<endl;
    if(voucher[0]!=0)
        cout<<"    "<<voucher[0]<<" $   1 voucher"<<endl;
    return;
}
