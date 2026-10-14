#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

int inputfile(float*, float*);
void calculation(float, float);
void swap_stage(int);
void merge_stage(float*);
void outputfile(int, int);
void unit(float);//一般的單位函數
void unit2(float, ofstream&);//輸出檔案用的單位函數

class stageinform
{
    public:
    int stageorder,marge=0;
    float rp_resistant[16]={0},rs_resistant[16]={0},voltage,rstotal,rptotal;//陳柏翰都從1號開始所以設16個
    float resistant_series()//把同一個stage串聯的電阻加起來
    {
        int temp=0;
        for(int i=1;i<=15;i++)temp+=rs_resistant[i];
        return temp;
    }
    float resistant_parallel()//把同一個stage並聯的電阻加起來
    {
        int temp=0;
        for(int i=1;i<=15;i++)temp+=rp_resistant[i];
        return temp;
    }
    string paranode[20],serinode[20];
};
stageinform stage[20];
void voltage_for_all_stage(int,int);
;

int main()
{
    int select;
    float initialvoltage,stagenumber;
    stage[0].paranode[1]="vdd";
    cout<<"*** Welcome to MySpice ***"<<endl;
    while(true)
    {
        cout << "=========================================="<<endl;
        cout << "Please type 1 to input file" << endl;
        cout << "Please type 2 to calculate voltage and current" << endl;
        cout << "Please type 3 to swap stage" << endl;
        cout << "Please type 4 to merge stage" << endl;
        cout << "Please type 5 to output file"<<endl;
        cout << "Please type 6 to exit " << endl;
        cout<<"Enter your selection Here: ";
        cin>>select;
        cin.get();
        cout << "=========================================="<<endl;
        switch(select)
        {
            case 1:inputfile(&initialvoltage,&stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 2:calculation(initialvoltage,stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 3:swap_stage(stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 4:merge_stage(&stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 5:outputfile(initialvoltage,stagenumber);break;
            case 6:cout<<"Bye~~";return 0;
            default: cout<<"please enter again";
        }

    }

    return 0;
}

int inputfile(float *initialvoltage, float *stagenumber)
{
    string filetitle;
    const string source="../../amber-kite-209c/PA4/data/";

    while(true)
    {
        cout<<"Please enter the name of the input file: ";
        getline(cin,filetitle);
        ifstream file1(source+filetitle,ios::in);
        if(file1.is_open())
        {
            cout<<"Loading the file..."<<endl<<"The input file successfully loaded!"<<endl;
            file1.close();//看檔案是否存在
            break;
        }
        else cout<<"The file is not found"<<endl;
    }
    ifstream file(source+filetitle,ios::in);
    while(!file.eof())
    {
        string a;
        file>>a;
        if(a=="stage")file>>*stagenumber;
        if(a=="gnd"){file>>*initialvoltage;file.seekg(0);break;}
    }
    while(!file.eof())
    {
        int control=1,numberofstage;
        char check;

        file>>check;
        if(check=='R')//讀到R後直接得到電阻的位置資訊
        {
            char inputtitle[15],Resistant[15];//inputtitle指該電阻的stage
            string Node,Vsource;
            int digit=0;
            float resistant=0;
            file>>inputtitle>>Vsource>>Node>>Resistant;
            stage[inputtitle[1]-48].stageorder=inputtitle[1]-48;//把stage存進stageorder裡面
            while(Resistant[digit]>=48&&Resistant[digit]<=57) digit++;//看電阻有幾位數
            for(int i=1;i<=digit;i++)
            {
                resistant+=(Resistant[i-1]-48)*pow(10,digit-i);
            }
            switch(Resistant[digit])
            {
                case 'G': resistant*=pow(10,9);break;
                case 'M': resistant*=pow(10,6);break;
                case 'K': resistant*=1000;break;
                case 'm': resistant/=1000;break;
                case 'u': resistant/=1000000;break;
                default: resistant*1;break;
            }//算電阻值
            if(inputtitle[0]=='S')
            {
                stage[inputtitle[1]-48].rs_resistant[inputtitle[3]-48]=resistant;//存串聯電阻
                stage[inputtitle[1]-48].serinode[inputtitle[3]-48]=Vsource;//存端點
            }
            else if(inputtitle[0]=='P')
            {
                stage[inputtitle[1]-48].rp_resistant[inputtitle[3]-48]=resistant;//存並聯電阻
                stage[inputtitle[1]-48].paranode[inputtitle[3]-48]=Vsource;//存端點
            }
        }
    }
    return 0;
}

void voltage_for_all_stage(int stagenumber, int voltage)
{
    float temp[2][16];
    int temp_re=0;
    float resistant_series[16], resistant_parallel[16], retotal[16], voltageofstage[16];//[]內為stage數
    for(int i=1; i<=10; i++)//將未轉折的電線上的電阻加在一起
    {
        resistant_parallel[i]=stage[i].resistant_parallel();
    }
    for(int i=1; i<=10; i++)//將未轉折的電線上的電阻加在一起
    {
        resistant_series[i]=stage[i].resistant_series();
    }

    voltageofstage[0]=voltage;
    //先算尾端等效電阻
    retotal[stagenumber]=1/(1/(resistant_series[stagenumber]+resistant_parallel[stagenumber])+1/resistant_parallel[stagenumber-1]);
    for(int i=1;i<stagenumber;i++)
    {
        retotal[stagenumber-i]=1/(1/(resistant_series[stagenumber-i]+retotal[stagenumber-i+1])+1/resistant_parallel[stagenumber-i-1]);
    }//把剩下等校電阻算出來
    temp_re=retotal[2]+resistant_series[1];
    for(int i=1;i<=stagenumber;i++)//算每一個端點的電壓
    {
        voltageofstage[i]=voltageofstage[i-1]-(voltageofstage[i-1]*resistant_series[i]/(retotal[i+1]+resistant_series[i]));
        if(i==stagenumber)voltageofstage[i]=voltageofstage[i-1]-(voltageofstage[i-1]*resistant_series[i]/(resistant_parallel[i]+resistant_series[i]));//末端點電壓
    }
    for(int i=1;i<=stagenumber;i++)temp[0][i]=stage[i].resistant_parallel();
    for(int i=1;i<=stagenumber;i++)temp[1][i]=stage[i].resistant_series();
    for(int i=1;i<=stagenumber;i++)stage[i].voltage=voltageofstage[i];
    for(int i=1;i<=stagenumber;i++)stage[i].rptotal=temp[0][i];//算好的電阻丟回class裡面
    for(int i=1;i<=stagenumber;i++)stage[i].rstotal=temp[1][i];//算好的電阻丟回class裡面
}

void calculation(float initialvoltage, float stagenumber)
{
    cout<<"Calculating..."<<endl<<"The result is successfully calculated!"<<endl;
    for(int i=1; i<=38; i++)cout<<"*";
    cout<<endl;
    cout<<"**          <<voltage>>             **"<<endl;
    int control1=1;
    cout<<"**"<<"  vdd         ";
    unit(initialvoltage);
    cout<<"v              **"<<endl;
    while(stage[control1].voltage!=0)
    {
        cout<<"**  "<<"n"<<stage[control1].stageorder<<"          ";
        unit(stage[control1].voltage);//把voltage_for_all_stage的電壓拿進來用
        cout<<"v"<<"              **"<<endl;
        control1++;//陸續把剩下端點拿進來印
    }
    cout<<"**          <<current>>             **"<<endl;
    for(int i=1; i<=stagenumber; i++)
    {
        int control2=1;
        if(i==1)//印出經過交換或是融合後第一個stage的電阻的電流
        {

            while(stage[1].rs_resistant[control2]!=0)//0是一開始初始化的條件所以拿來判斷該電阻是否存在
            {
                cout<<"**  "<<"RS"<<stage[1].stageorder<<"_"<<control2<<"       ";
                unit((initialvoltage-stage[1].voltage)/stage[1].rstotal);
                cout<<"A           **"<<endl;
                control2++;
            }
            control2=1;
            while(stage[1].rp_resistant[control2]!=0)//0是一開始初始化的條件所以拿來判斷該電阻是否存在
            {
                cout<<"**  ";
                if(stage[1].marge==1)cout<<"RP1  ";//檢查是不是merge後的電阻
                else cout<<"RP"<<stage[1].stageorder<<"_"<<control2;
                cout<<"       ";
                unit(stage[1].voltage/stage[1].rptotal);
                cout <<"A           **"<<endl;
                control2++;
            }
        }
        else//印出經過交換或是融合後第二個stage以及之後的電阻的電流
        {
            while(stage[i].rs_resistant[control2]!=0)//0是一開始初始化的條件所以拿來判斷該電阻是否存在
            {
                cout<<"**  "<<"RS"<<stage[i].stageorder<<"_"<<control2<<"       ";
                unit((stage[i-1].voltage-stage[i].voltage)/stage[i].rstotal);
                cout<<"A           **"<<endl;
                control2++;
            }
            control2=1;
            while(stage[i].rp_resistant[control2]!=0)//0是一開始初始化的條件所以拿來判斷該電阻是否存在
            {
                cout<<"**  ";
                if(stage[i].marge==1)cout<<"RP"<<i<<"  ";//檢查是不是merge後的電阻
                else cout<<"RP"<<stage[i].stageorder<<"_"<<control2;
                cout<<"       ";
                unit((stage[i].voltage)/stage[i].rptotal);
                cout<<"A           **"<<endl;
                control2++;
            }
        }
    }
    cout<<"**                                  **"<<endl;
    cout<<"**                  myspice2021     **"<<endl;
    cout<<"**************************************"<<endl;
}

void unit(float target)//取log後看幾位數
{
    float log_target=0;
    int log_target2;
    log_target=log10(target);
    log_target=floor(log_target/3);//下高斯
    log_target+=3;
    log_target2=log_target;
    switch(log_target2)
    {
        case 1:cout<<setprecision(3)<<fixed<<target*pow(10,6)<<"u";break;
        case 2:cout<<setprecision(3)<<fixed<<target*pow(10,3)<<"m";break;
        case 4:cout<<setprecision(3)<<fixed<<target/pow(10,3)<<"k";break;
        case 5:cout<<setprecision(3)<<fixed<<target/pow(10,6)<<"Meg";break;
        case 6:cout<<setprecision(3)<<fixed<<target/pow(10,9)<<"G";break;
        default:cout<<fixed<<setprecision(3)<<target;
    }
}

void unit2(float target, ofstream &ofile)//取log後看幾位數，然後輸出檔
{
    float log_target=0;
    int log_target2;
    log_target=log10(target);
    log_target=floor(log_target/3);
    log_target+=3;
    log_target2=log_target;
    switch(log_target2)
    {
        case 1:ofile<<setprecision(3)<<fixed<<target*pow(10,6)<<"u";break;
        case 2:ofile<<setprecision(3)<<fixed<<target*pow(10,3)<<"m";break;
        case 4:ofile<<setprecision(3)<<fixed<<target/pow(10,3)<<"K";break;
        case 5:ofile<<setprecision(3)<<fixed<<target/pow(10,6)<<"Meg";break;
        case 6:ofile<<setprecision(3)<<fixed<<target/pow(10,9)<<"G";break;
        default:ofile<<fixed<<setprecision(3)<<target;
    }
}

void swap_stage(int stagenumber)
{
    stageinform temp;
    int first,second;
    string nodetemp;

    while(true)
    {
        cout<<"Please enter first stage you want to change: ";
        cin>>first;
        if(first<=stagenumber&&first>0)break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }
    while(true)
    {
        cout<<"Please enter second stage you want to change: ";
        cin>>second;
        if(second<=stagenumber&&second>0)break;
        cout<<"The node doesn't exist. Please enter again."<<endl;
    }

    cout<<"RS"<<stage[first].stageorder<<"_1"<<" is changing node from "<<stage[first-1].paranode[1]<<" to "<<stage[second].paranode[1]<<"..."<<endl;
    cout<<"RS"<<stage[second].stageorder<<"_1"<<" is changing node from "<<stage[second-1].paranode[1]<<" to "<<stage[first-1].paranode[1]<<"..."<<endl;
    if(second+1<=stagenumber)cout<<"RS"<<stage[second+1].stageorder<<"_1"<<" is changing node from "<<stage[second].paranode[1]<<" to "<<stage[first].paranode[1]<<"..."<<endl;
    //萬一交換的端點不是尾端，就要把右者的右邊的電路端點也要換
    temp=stage[first];
    stage[first]=stage[second];
    stage[second]=temp;
    nodetemp=stage[first].serinode[1];
    stage[first].serinode[1]=stage[second].serinode[1];
    stage[second].serinode[1]=nodetemp;
    cout<<"The stage is successfully swapped!"<<endl;
}

void merge_stage(float*stagenumber)
{
    int first,second;
    cout<<"Please enter the first stage you want to merge: ";
    cin>>first;
    cout<<"Please enter the second stage you want to merge: ";
    cin>>second;
    for(int i=1;i<=16;i++)stage[first].rp_resistant[i]=0;
    stage[first].rp_resistant[1]=1/((1/stage[first].rptotal)+1/(stage[second].rstotal+stage[second].rptotal));//融合電阻運算(一個ㄇ拆成左一根跟右上引號相加)
    stage[first].marge=1;//output印出的時候拿來判斷
    cout<<"Resistors are merged into RP"<<first<<"."<<endl;
    cout<<"=> RP"<<first<<" n"<<stage[first].stageorder<<" gnd ";
    unit(stage[first].rp_resistant[1]);
    cout<<endl;
    cout<<"The stage is successfully merged!"<<endl;
     for(int i=second;i<=18;i++)
    {
        stage[i]=stage[i+1];
    }
    *stagenumber-=1;
}

void outputfile(int initialvoltage, int stagenumber)
{
     int control1=1,control3=1;
    const string source="../../amber-kite-209c/PA4/result/";
    string outputfiletitle;
    cout<<"Please enter the name of the output file: ";
    getline(cin,outputfiletitle);
    ofstream ofile(source+outputfiletitle,ios::out);
    cout<<"Exporting file...\nThe output file successfully exported!"<<endl;
    ofile<<"***circuits***"<<endl;
    ofile<<"stage "<<stagenumber<<endl<<endl;
    ofile<<"v1 vdd gnd "<<initialvoltage<<"V"<<endl<<endl;

    for(int i=1;i<=stagenumber;i++)
    {
        int j1=2,j2=1;
        if(stage[i].rs_resistant[j1]<=0)
        {
            ofile<<"RS"<<stage[i].stageorder<<left<<setw(5)<<"_1"<<left<<setw(8)<<stage[i-1].paranode[1]<<left<<setw(8)<<stage[i].paranode[1];
            unit2(stage[i].rs_resistant[1],ofile) ;
            ofile<<endl;
        }
        else
        {
            ofile<<"RS"<<stage[i].stageorder<<left<<setw(5)<<"_1"<<left<<setw(8)<<stage[i-1].paranode[1]<<left<<setw(8)<<stage[i].serinode[j1];
            unit2(stage[i].rs_resistant[j1],ofile) ;
            ofile<<endl;
        }
        while(stage[i].rs_resistant[j1+1]!=0)
        {
            ofile<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(4)<<j1<<left<<setw(8)<<stage[i].serinode[j1]<<left<<setw(8)<<stage[i].serinode[j1+1];
            unit2(stage[i].rs_resistant[j1],ofile) ;
            ofile<<endl;
            j1++;
        }
        if(stage[i].rs_resistant[j1]>0)
        {
            ofile<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(4)<<j1<<left<<setw(8)<<stage[i].serinode[j1]<<left<<setw(8)<<stage[i].paranode[1];
            unit2(stage[i].rs_resistant[j1],ofile) ;
            ofile<<endl;
        }
        while(stage[i].rp_resistant[j2+1]!=0)
        {
            ofile<<"RP";
            if(stage[i].marge==1)ofile<<left<<setw(6)<<i;//如果經過融合會多需要兩個空格
            else ofile<<stage[i].stageorder<<"_"<<left<<setw(4)<<j2;
            ofile<<left<<setw(8)<<stage[i].paranode[j2]<<left<<setw(8)<<stage[i].paranode[j2+1];
            unit2(stage[i].rp_resistant[j2],ofile);
            ofile<<endl;
            j2++;
        };
        ofile<<"RP";
        if(stage[i].marge==1)ofile<<left<<setw(6)<<i;//如果經過融合會多需要兩個空格
        else ofile<<stage[i].stageorder<<"_"<<left<<setw(4)<<j2;
        ofile<<left<<setw(8)<<stage[i].paranode[j2]<<setw(8)<<"gnd";
        unit2(stage[i].rp_resistant[j2],ofile);
        ofile<<endl;
    }
    ofile<<endl<<"***calculation results***"<<endl;
    for(int i=1;i<=38;i++)ofile<<"*";
    ofile<<endl;
    ofile<<"**			<<voltage>>				**"<<endl;
    ofile<<"**  "<<setw(28)<<"vdd";
    unit2(initialvoltage,ofile);
    ofile<<"v				**"<<endl;
    while(stage[control1].voltage!=0)
    {
        ofile<<"**  "<<"n"<<setw(27)<<stage[control1].stageorder;
        unit2(stage[control1].voltage,ofile);
        ofile<<"v				**"<<endl;
        control1++;
    }
    ofile<<"**			<<current>>				**"<<endl;
    for(int i=1;i<=stagenumber;i++)
    {
        int control2=1;
        if(i==1)//第一個stage為special case
        {

            while(stage[1].rs_resistant[control2]!=0)
            {
                ofile<<"**     "<<"RS"<<stage[1].stageorder<<"_"<<left<<setw(13)<<control2;
                unit2((initialvoltage-stage[1].voltage)/stage[1].rstotal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }control2=1;
            while(stage[1].rp_resistant[control2]!=0)
            {
                ofile<<"**     ";if(stage[i].marge==1)ofile<<"RP"<<left<<setw(15)<<i;//如果經過融合會多需要兩個空格
                else ofile<<"RP"<<stage[1].stageorder<<"_"<<left<<setw(13)<<control2;
                unit2(stage[1].voltage/stage[1].rptotal,ofile);
                ofile <<"A                               **"<<endl;
                control2++;
            }
        }
        else
        {
            while(stage[i].rs_resistant[control2]!=0)
            {
                ofile<<"**     "<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(13)<<control2;
                unit2((stage[i-1].voltage-stage[i].voltage)/stage[i].rstotal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }control2=1;
            while(stage[i].rp_resistant[control2]!=0)
            {
                ofile<<"**     ";if(stage[i].marge==1)ofile<<"RP"<<left<<setw(15)<<i;//如果經過融合會多需要兩個空格
                else ofile<<"RP"<<stage[i].stageorder<<"_"<<left<<setw(13)<<control2;
                unit2((stage[i].voltage)/stage[i].rptotal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }
        }
    }
    ofile<<"**			        				**"<<endl;
    ofile<<"**					myspice2021		**"<<endl;
    ofile<<"**************************************"<<endl;
}
