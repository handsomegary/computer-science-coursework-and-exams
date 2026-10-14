#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

int func1(float*,float*);
void func2(float,float);
void func3(int);
void func4(float*);
void func5(int,int);
void unity(float);
void unity2(float,ofstream&);

class stageinform

{
    public:
    int stageorder,marge=0;
    float rp_resistant[16]={0},rs_resistant[16]={0},voltage,rstotal,rptoatal;
    float resistant_series()//total resistant of series.
    {    int temp=0;
        for(int i=1;i<=10;i++)temp+=rs_resistant[i];
        return temp;
    }
    float resistant_parallel()
    {    int temp=0;
        for(int i=1;i<=10;i++)temp+=rp_resistant[i];
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
    while(true)
    {
        cout<<"*** Welcome to MySpice ***"<<endl;
        cout << "========================================="<<endl;
        cout << "Please type 1 to input file" << endl;
        cout << "Please type 2 to calculate voltage and current" << endl;
        cout << "Please type 3 to swamp stage" << endl;
        cout << "Please type 4 to merge stage" << endl;
        cout << "Please type 5 to output file"<<endl;
        cout << "Please type 6 to exit " << endl;
        cout<<"Enter your selection Here: ";
        cin>>select;
        cin.get();
        cout << "========================================="<<endl;
        switch(select)
        {
            case 1:func1(&initialvoltage,&stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 2:func2(initialvoltage,stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 3:func3(stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 4:func4(&stagenumber);voltage_for_all_stage(stagenumber,initialvoltage);break;
            case 5:func5(initialvoltage,stagenumber);break;
            case 6:cout<<"Bye~~";return 0;
            default: cout<<"please enter again";
        }

    }

    return 0;
}

int func1(float *initialvoltage,float*stagenumber)
{

     //initialize variable
    string filetitle;
    const string source="PA4/data.txt/",tet=".txt";

    while(true)
    {
        cout<<"Please enter the name of the input file: ";
        getline(cin,filetitle);
        ifstream file1(source+filetitle+tet,ios::in);
        //initialize variable
        if(file1.is_open()){cout<<"Loading the file..."<<endl<<"The input file successfully loaded!"<<endl;file1.close();break;}
        else cout<<"The file is not found"<<endl;
    }

    ifstream file(source+filetitle+tet,ios::in);

   //information classify.

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
        if(check=='R')
        {
        char inputtitle[15],Resistant[15];
        string Node,Vsource;
        int digit=0;
        float resistant=0;
        file>>inputtitle>>Vsource>>Node>>Resistant;
        //cout<<inputtitle<<" "<<Vsource<<" "<<Node<<" "<<Resistant<<endl;

        stage[inputtitle[1]-48].stageorder=inputtitle[1]-48;
        while(Resistant[digit]>=48&&Resistant[digit]<=57) digit++;
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
        }
        if(inputtitle[0]=='S')
        {
            stage[inputtitle[1]-48].rs_resistant[inputtitle[3]-48]=resistant;//class the resistant
            stage[inputtitle[1]-48].serinode[inputtitle[3]-48]=Vsource;
        }

        else if(inputtitle[0]=='P')
        {
        stage[inputtitle[1]-48].rp_resistant[inputtitle[3]-48]=resistant;
        stage[inputtitle[1]-48].paranode[inputtitle[3]-48]=Vsource;
        }

        }
    }

//cout<< stage[3].rs_resistant[3]<<endl;

    return 0;


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

    voltageofstage[0]=voltage;//initial voltage.

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

void func2(float initialvoltage,float stagenumber)
{
    cout<<"*** circuit ***"<<endl;
    cout<<"stage "<<stagenumber<<endl<<endl;
    cout<<"v1 vdd gnd "<<initialvoltage<<endl<<endl;

     /*   for(int i=1;i<=stagenumber;i++)
    {
     int j1=2,j2=2;

        if(i==1) cout<<"RS"<<stage[i].stageorder<<"_1"<<"    vdd"<<"    "<<stage[i].serinode[1]<<endl;
        else cout<<"RS"<<stage[i].stageorder<<"_1"<<"    n"<<stage[i-1].stageorder<<"    "<<stage[i].serinode[1]<<endl;
        while(stage[i].rs_resistant[j1]!=0)
        {
        cout<<"RS"<<stage[i].stageorder<<"_"<<j1<<"    "<<stage[i].serinode[j1-1]<<"    "<<stage[i].serinode[j1]<<endl;
        j1++;
        }
        j1=2;
        cout<<"RP"<<stage[i].stageorder<<"_1"<<"    n"<<stage[i].stageorder<<"    "<<stage[i].paranode[1]<<endl;
        while(stage[i].rp_resistant[j2]!=0)
        {
        cout<<"RP"<<stage[i].stageorder<<"_"<<j2<<"    "<<stage[i].paranode[j2-1]<<"    "<<stage[i].paranode[j2]<<endl;
        j2++;
        }
    }*/

    cout<<"Calculating..."<<endl<<"The result is successfully calculated!"<<endl;
    for(int i=1;i<=38;i++)cout<<"*";
    cout<<endl;
    cout<<"**          <<voltage>>             **"<<endl;
    int control1=1;
    cout<<"**"<<"  vdd         ";unity(initialvoltage);cout<<"v              **"<<endl;
    while(stage[control1].voltage!=0)
    {
        cout<<"**  "<<"n"<<stage[control1].stageorder<<"  ";unity(stage[control1].voltage);cout<<"v"<<"  **"<<endl;
        control1++;
    }
    cout<<"**          <<current>>             **"<<endl;
    for(int i=1;i<=stagenumber;i++)
    {
        int control2=1;
        if(i==1)
        {

            while(stage[1].rs_resistant[control2]!=0)
            {
                cout<<"**  "<<"RS"<<stage[1].stageorder<<"_"<<control2<<"       ";unity((initialvoltage-stage[1].voltage)/stage[1].rstotal);cout<<"A           **"<<endl;
                control2++;
            }
            control2=1;
            while(stage[1].rp_resistant[control2]!=0)
            {
                cout<<"**  ";if(stage[1].marge==1)cout<<"RP1  ";else cout<<"RP"<<stage[1].stageorder<<"_"<<control2;cout<<"       ";unity(stage[1].voltage/stage[1].rptoatal);cout <<"A           **"<<endl;
                control2++;
            }
        }
        else
        {
            while(stage[i].rs_resistant[control2]!=0)
            {
                cout<<"**  "<<"RS"<<stage[i].stageorder<<"_"<<control2<<"       ";unity((stage[i-1].voltage-stage[i].voltage)/stage[i].rstotal);cout<<"A           **"<<endl;
                control2++;
            }
            control2=1;
            while(stage[i].rp_resistant[control2]!=0)
            {
                cout<<"**  ";
                if(stage[i].marge==1)cout<<"RP"<<i<<"  ";else cout<<"RP"<<stage[i].stageorder<<"_"<<control2;cout<<"       ";unity((stage[i].voltage)/stage[i].rptoatal);cout<<"A           **"<<endl;
                control2++;
            }
        }
    }
    cout<<"**                                  **"<<endl;
    cout<<"**                  myspice2021     **"<<endl;
    cout<<"**************************************"<<endl;
}

void unity(float target)
{
    float log_target=0;
    int log_target2;
    log_target=log10(target);
    log_target=floor(log_target/3);
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

void unity2(float target,ofstream &ofile)
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

void func3(int stagenumber)
{
    stageinform temp;
    int first,second;
    string nodetemp;

    while(true)
    {
        cout<<"Please enter first stage you want to change: ";
        cin>>first;
        if(first<=stagenumber&&first>0)break;
        cout<<"The node doesn't. Please enter again."<<endl;
    }
        while(true)
    {
        cout<<"Please enter second stage you want to change: ";
        cin>>second;
        if(second<=stagenumber&&first>0)break;
        cout<<"The node doesn't. Please enter again."<<endl;
    }

    cout<<"RS"<<stage[first].stageorder<<"_1"<<" is changing node from "<<stage[first-1].paranode[1]<<" to "<<stage[second].paranode[1]<<endl;
    cout<<"RS"<<stage[second].stageorder<<"_1"<<" is changing node from "<<stage[second-1].paranode[1]<<" to "<<stage[first-1].paranode[1]<<endl;
    if(second+1<=stagenumber)cout<<"RS"<<stage[second+1].stageorder<<"_1"<<" is changing node from "<<stage[second].paranode[1]<<" to "<<stage[first].paranode[1]<<endl;
    temp=stage[first];stage[first]=stage[second];stage[second]=temp;
    nodetemp=stage[first].serinode[1];stage[first].serinode[1]=stage[second].serinode[1];stage[second].serinode[1]=nodetemp;
}

void func5(int initialvoltage,int stagenumber)
{
    int control1=1,control3=1;
    const string source="PA4/result/";
    string outputfiletitle;
    cout<<"Please enter the name of the output file: ";
    getline(cin,outputfiletitle);
    ofstream ofile(source+outputfiletitle,ios::out);
    cout<<"Exporting file...\nThe output file successfully exported"<<endl;//following is going to output the fuile

    ofile<<"***circuits***"<<endl;
    ofile<<"stage "<<stagenumber<<endl<<endl;
    ofile<<"v1 vdd gnd "<<initialvoltage<<"V"<<endl<<endl;

    for(int i=1;i<=stagenumber;i++)
    {
        int j1=2,j2=1;
        if(stage[i].rs_resistant[j1]<=0)
        {
            ofile<<"RS"<<stage[i].stageorder<<left<<setw(5)<<"_1"<<left<<setw(8)<<stage[i-1].paranode[1]<<left<<setw(8)<<stage[i].paranode[1];
            unity2(stage[i].rs_resistant[1],ofile);
            ofile<<endl;
        }
        else{ofile<<"RS"<<stage[i].stageorder<<left<<setw(5)<<"_1"<<left<<setw(8)<<stage[i-1].paranode[1]<<left<<setw(8)<<stage[i].serinode[j1];
        unity2(stage[i].rs_resistant[j1],ofile) ;ofile<<endl;}
        while(stage[i].rs_resistant[j1+1]!=0)
        {
            ofile<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(4)<<j1<<left<<setw(8)<<stage[i].serinode[j1]<<left<<setw(8)<<stage[i].serinode[j1+1];
            unity2(stage[i].rs_resistant[j1],ofile) ;
            ofile<<endl;
            j1++;
        }
        if(stage[i].rs_resistant[j1]>0)
        {
            ofile<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(4)<<j1<<left<<setw(8)<<stage[i].serinode[j1]<<left<<setw(8)<<stage[i].paranode[1];
            unity2(stage[i].rs_resistant[j1],ofile);
            ofile<<endl;
        }
        while(stage[i].rp_resistant[j2+1]!=0)
        {
            ofile<<"RP"<<stage[i].stageorder;
            if(stage[i].marge!=1)ofile<<"_"<<left<<setw(4)<<j2;
            ofile<<left<<setw(8)<<stage[i].paranode[j2]<<left<<setw(8)<<stage[i].paranode[j2+1];
            unity2(stage[i].rp_resistant[j2],ofile);
            ofile<<endl;
            j2++;
        };
        ofile<<"RP"<<stage[i].stageorder;
        if(stage[i].marge!=1)ofile<<"_"<<left<<setw(4)<<j2;ofile<<left<<setw(8)<<stage[i].paranode[j2]<<left<<setw(8)<<"gnd";
        unity2(stage[i].rp_resistant[j2],ofile);
        ofile<<endl;
    }

    ofile<<endl;
    ofile<<"***calculation results***"<<endl;
    for(int i=1;i<=38;i++)ofile<<"*";
    ofile<<endl<<"**			<<voltage>>				**"<<endl;
    ofile<<"**  "<<setw(28)<<"vdd";unity2(initialvoltage,ofile);
    ofile<<"v				**"<<endl;
    while(stage[control1].voltage!=0)
    {
        ofile<<"**  "<<"n"<<left<<setw(27)<<stage[control1].stageorder;
        unity2(stage[control1].voltage,ofile);
        ofile<<"v"<<"				**"<<endl;
        control1++;
    }
    ofile<<"**			<<current>>				**"<<endl;
    for(int i=1;i<=stagenumber;i++)
    {
        int control2=1;
        if(i==1)
        {
            while(stage[1].rs_resistant[control2]!=0)
            {
                ofile<<"**	"<<"RS"<<stage[1].stageorder<<"_"<<left<<setw(12)<<control2;
                unity2((initialvoltage-stage[1].voltage)/stage[1].rstotal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }control2=1;
            while(stage[1].rp_resistant[control2]!=0)
            {
                ofile<<"**	"<<"RP"<<stage[1].stageorder;
                if(stage[i].marge!=1)ofile<<"_"<<left<<setw(12)<<control2;
                unity2(stage[1].voltage/stage[1].rptoatal,ofile);
                ofile <<"A                               **"<<endl;
                control2++;
            }
        }
        else
        {
            while(stage[i].rs_resistant[control2]!=0)
            {
                ofile<<"**	"<<"RS"<<stage[i].stageorder<<"_"<<left<<setw(12)<<control2;
                unity2((stage[i-1].voltage-stage[i].voltage)/stage[i].rstotal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }
            control2=1;
            while(stage[i].rp_resistant[control2]!=0)
            {
                ofile<<"**	"<<"RP"<<stage[i].stageorder;
                if(stage[i].marge!=1)ofile<<"_"<<left<<setw(12)<<control2;
                unity2((stage[i].voltage)/stage[i].rptoatal,ofile);
                ofile<<"A                               **"<<endl;
                control2++;
            }

        }

    }
    ofile<<"**			        				**"<<endl;
    ofile<<"**					myspice2021		**"<<endl;
    ofile<<"**************************************"<<endl;
}

void func4(float*stagenumber)
{
    int first,second;
    cout<<"Please enter the first stage you want to merge: ";
    cin>>first;
    cout<<"Please enter the second stage you want to merge: ";
    cin>>second;
    for(int i=1;i<=10;i++)stage[first].rp_resistant[i]=0;
    stage[first].rp_resistant[1]=1/((1/stage[first].rptoatal)+1/(stage[second].rstotal+stage[second].rptoatal));
    stage[first].marge=1;
    cout<<"Resistors are merged into RP"<<first<<"."<<endl;
    cout<<"=> RP"<<first<<" n"<<stage[first].stageorder<<" gnd ";unity(stage[first].rp_resistant[1]);cout<<endl;
    cout<<"The stage is successfully merged!"<<endl;
     for(int i=second;i<=18;i++)
    {
        stage[i]=stage[i+1];
    }
    *stagenumber-=1;
    //cout<<*stagenumber<<endl;
    //cout<<stage[first].rp_resistant[1]<<endl;
}


