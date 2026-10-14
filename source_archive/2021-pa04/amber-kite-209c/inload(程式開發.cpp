#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>
using namespace std;
class stageinform
{
    public:
    float rp_resistant[10]={0},rs_resistant[10]={0},voltage,rstotal,rptoatal;
    float resistant_series()
    {    int temp=0;
        for(int i=1;i<=10;i++)temp+=rs_resistant[i];
        return temp;
    }
    float resistant_parallel()
    {    int temp=0;
        for(int i=1;i<=10;i++)temp+=rp_resistant[i];
        return temp;
    }
};
void voltage_for_all_stage(int,int);
void func5(int,int);
stageinform stage[20];

int main()//function1: input file.
{
    //initialize variable
    string filetitle;
    const string source="PA4/data/",tet=".txt";
    int initialvoltage,stagenumber;

    cout<<"Please enter the file name: ";
    getline(cin,filetitle);
    ifstream file(source+filetitle+tet,ios::in);
   //initialize variable

   //information classify.
    while(!file.eof())
    {
        string a;
        file>>a;
        if(a=="stage")file>>stagenumber;
        if(a=="gnd"){file>>initialvoltage;file.seekg(0);break;}
    }
    while(!file.eof())
    {
        int control=1,numberofstage;
        char check;

        file>>check;
        if(check=='R')
        {
        char inputtitle[15],Vsource[5],Node[7],Resistant[10];
        int digit=0;
        float resistant=0;
        file>>inputtitle>>Vsource>>Node>>Resistant;
        cout<<inputtitle<<" "<<Vsource<<" "<<Node<<" "<<Resistant<<endl;
        while(Resistant[digit]>=48&&Resistant[digit]<=57) digit++;
        for(int i=1;i<=digit;i++)
        {
        resistant+=(Resistant[i-1]-48)*pow(10,digit-i);
        }
        switch(Resistant[digit])
        {
        case 'k': resistant*=1000;break;
        case 'm': resistant/=1000;break;
        default: resistant*1;break;
        }
        if(inputtitle[0]=='S')stage[inputtitle[1]-48].rs_resistant[inputtitle[3]-48]=resistant;//class the resistant
        else if(inputtitle[0]=='P')stage[inputtitle[1]-48].rp_resistant[inputtitle[3]-48]=resistant;
        }
    }
    //information classify.


    voltage_for_all_stage(stagenumber,initialvoltage);

    //test bord

    func5(initialvoltage,stagenumber);

    return 0;

    //test bord
}

void voltage_for_all_stage(int stagenumber,int voltage)//calculate voltage of each node
{

    float temp[2][10];
    int temp_re=0;
    float resistant_series[10],resistant_parallel[10],retotal[10],voltageofstage[10];
    for(int i=1;i<=10;i++)
    {
        resistant_parallel[i]=stage[i].resistant_parallel();
    }
      for(int i=1;i<=10;i++)
    {
        resistant_series[i]=stage[i].resistant_series();
    }

    voltageofstage[0]=voltage;

    retotal[stagenumber]=1/(1/(resistant_series[stagenumber]+resistant_parallel[stagenumber])+1/resistant_parallel[stagenumber-1]);
    for(int i=1;i<stagenumber;i++)
    {
        retotal[stagenumber-i]=1/(1/(resistant_series[stagenumber-i]+retotal[stagenumber-i+1])+1/resistant_parallel[stagenumber-i-1]);
    }
      temp_re=retotal[2]+resistant_series[1];

    for(int i=1;i<=stagenumber;i++)
    {
        voltageofstage[i]=voltageofstage[i-1]-(voltageofstage[i-1]*resistant_series[i]/(retotal[i+1]+resistant_series[i]));
        if(i==stagenumber)voltageofstage[i]=voltageofstage[i-1]-(voltageofstage[i-1]*resistant_series[i]/(resistant_parallel[i]+resistant_series[i]));
    }

    for(int i=1;i<=stagenumber;i++)temp[0][i]=stage[i].resistant_parallel();
    for(int i=1;i<=stagenumber;i++)temp[1][i]=stage[i].resistant_series();

     for(int i=1;i<=stagenumber;i++)stage[i].voltage=voltageofstage[i];

    for(int i=1;i<=stagenumber;i++)stage[i].rptoatal=temp[0][i];
    for(int i=1;i<=stagenumber;i++)stage[i].rstotal=temp[1][i];
}

void func5(int initialvoltage,int stagenumber)
{
    for(int i=1;i<=33;i++)cout<<"*";
    cout<<endl;
    cout<<"**  <<voltage>>  **"<<endl;
    int control1=1;
    cout<<"**"<<" vdd  "<<initialvoltage<<"v **"<<endl;
    while(stage[control1].voltage!=0)
    {
        cout<<"**  "<<"n"<<control1<<"  "<<stage[control1].voltage<<"v"<<"  **"<<endl;
        control1++;
    }
    cout<<"**  <<current>>  **"<<endl;
    for(int i=1;i<=stagenumber;i++)
    {
        int control2=1;
       if(i==1)
       {

            while(stage[1].rs_resistant[control2]!=0)
        {
            cout<<"** "<<"RS"<<1<<"_"<<control2<<"  "<<(initialvoltage-stage[1].voltage)/stage[1].rstotal<<" **"<<endl;
            control2++;
        }
         while(stage[1].rp_resistant[control2]!=0)
        {
            cout<<"** "<<"RP"<<1<<"_"<<control2<<"  "<<stage[1].voltage/stage[1].rptoatal <<"**"<<endl;
            control2++;
        }
       }
        else
            {
                while(stage[i].rs_resistant[control2]!=0)
                {
                cout<<"** "<<"RS"<<i<<"_"<<control2<<"  "<<(stage[i-1].voltage-stage[i].voltage)/stage[i].rstotal<<" **"<<endl;
                control2++;
                }
                control2=1;
                while(stage[i].rp_resistant[control2]!=0)
                {
                cout<<"** "<<"RP"<<i<<"_"<<control2<<"  "<<(stage[i].voltage)/stage[i].rptoatal<<" **"<<endl;
                control2++;
                }

            }


    }

}






