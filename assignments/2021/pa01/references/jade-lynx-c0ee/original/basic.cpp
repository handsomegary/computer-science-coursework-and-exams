//PA1
#include <iostream>
#include<string>
using namespace std;
                                    //   function begin
void price_set(int []);            //    set price
void data_set(string []);        //set data
void price_count(int [],string []);   //count price
bool price_judge(int [],int []);  //judge price
void price_payment(int [],int []);  //the method of payment
void voucher_show_remain(int []);
void voucher_show_use(int []);    //show vouchers
int voucher_count(int[]);   //   count remain vouchers



    //main beginning ********************************************************************
int main(void)
{
    int price[1];                // 價格
    int voucher[5]={5,5,5,5,5}; //1$,5$,10$,50$,100$張數
    string  data[2];                  //(d/p)、(y/n)

    cout<<"*** Welcome to Quintuple Stimulus Vouchers System ***"<<endl;
    while(price[0]!=-1&&voucher_count(voucher)!=0)
    {
        price_set(price);
        if(price[0]==-1)          //價格為-1結束迴圈
        {
            cout<<"Bye~"<<endl;
            break;
        }
        data_set(data);            //輸入資料
        price_count(price,data);    //價格計算
        if(price_judge(price,voucher)==false)       //判斷是否有足夠的錢的付，為否即回到一開始
        {
            cout<<"You don't have enough voucher to buy the good"<<endl<<endl;
            continue;
        }
        price_payment(price,voucher);            //付錢方法
        if(voucher_count(voucher)!=0)            //剩下的錢為零即不顯示張數
        {
            voucher_show_remain(voucher);
            cout<<endl;
        }
        else                 //vouchers為零時 顯示You do not remain any voucher，Bye~
        {
            cout<<"You do not remain any voucher"<<endl<<"Bye~"<<endl;
        }
    } //end while
    return 0;


}        //end of main  ********************************************************************



         //start to write function
void price_set(int price[])              //    set price
{
    double price1;
    cout<<"Please enter the good price:";
    do
    {
         cin>>price1;
         price[0]=price1;
         if((price[0]<=0&&price[0]!=-1)||price1>price[0])
         {
             cout<<"Please enter the correct price:";
         }
    }while((price[0]<=0&&price[0]!=-1)||price1>price[0]);

    return;
}
void data_set(string data[])        //set data
{
    do
    {
        cout<<"Please enter the type of vouchers (d/p):";
        cin>>data[0];
        if(data[0]!="d"&&data[0]!="p")
            cout<<"Please enter the correct type of vouchers"<<endl;
        if(data[0]=="d")
        {
            do
            {
                cout<<"Are you a member of NCU Bank (y/n):";
                cin>>data[1];
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
        price[0]=price[0]*0.9;
    else if(data[1]=="y")
            price[0]=price[0]*0.8*0.9;
        else
            price[0]=price[0]*0.8;

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
{   int sum;                                       //voucher 的總和
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
                        voucher_quantity=i+j+k+h+e;     //voucher 張數計算
                        sum=i*1+j*5+k*10+h*50+e*100;    //voucher 總和計算=
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
     if(voucher_pay[0]!=0||voucher_pay[1]!=0||voucher_pay[2]!=0||voucher_pay[3]!=0||voucher_pay[4]!=0)
     {
        if(price[0]!=0)
            voucher_show_use(voucher_pay);
        voucher[0]=voucher[0]-voucher_pay[0];
        voucher[1]=voucher[1]-voucher_pay[1];
        voucher[2]=voucher[2]-voucher_pay[2];
        voucher[3]=voucher[3]-voucher_pay[3];
        voucher[4]=voucher[4]-voucher_pay[4];

        return;
     }                                                   //vouchers != price
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
                        voucher_quantity=i+j+k+h+e;     //張數S
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
       if(price[0]!=0)
           voucher_show_use(voucher_pay);
       voucher[0]=voucher[0]-voucher_pay[0];
       voucher[1]=voucher[1]-voucher_pay[1];
       voucher[2]=voucher[2]-voucher_pay[2];
       voucher[3]=voucher[3]-voucher_pay[3];
       voucher[4]=voucher[4]-voucher_pay[4];
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
