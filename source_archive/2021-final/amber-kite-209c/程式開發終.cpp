#include <iostream>
#include <cstring>
#include <fstream>
#include <cmath>
#include "../../../assignments/2021/final/course_support/kmap.cpp"
using namespace std;

class inform//presume there will be two bit for status,two bit for input in maxium since Kmap function support 4 variable at most.
{
    public:
    string name;
    int andgate[10][5];//how many or gate will connect to and gate.
    int orgate[22][5];//the input connect to or gate.//2 DFF+20output
    int kmapoutput[10][6];//store the result of the calculation of kmap output.
    int nextstatus[17];
    int andnumber=0,invertornum=0,common=0,*commonptr[10];
    int ornumber()
    {
        if(andnumber+common>=2)return 1;
        else return 0;
    }
};
void case2variable(int,ofstream&),case3variable(int,ofstream&),case4variable(int,ofstream&);
void buildcircuit(int*);
void mapport(int*);;
inform bit[24];//output will following the input.
inform out[20];
char port[3][20];//1 for input,2 for output.
void setkamp(int);
int main()
{

    int aray[5]={1,1,1,1,1};//determine which type of kmap would applied.1 for present 2 for input,4 for output
    int aray_control=1;
    string filetitle="state_table_1",txt=".txt";
    //cout << "please enter the filetitle: ";
    //getline(cin,filetitle);state_table_1
    ifstream file1(filetitle+txt,ios::in);
    if(file1.is_open())cout<<"successfull"<<endl;
    while(!file1.eof())
    {
        char check,check2;
        file1>>check;
        while(check=='(')
        {
            file1>>check2;
            if(check2=='0')aray[aray_control]=0;
            if(check2==',')aray[aray_control]++;
            if(check2==')'){aray_control++;break;}
        }

    }file1.close();
    ifstream file2(filetitle+txt,ios::in);
    while(!file2.eof())
    {
        string check;
        file2>>check;
        //cout<<check<<endl;
        if(check[2]=='N')
        {

            if(aray[3]==2)
            {
                for(int i=0;i<pow(2,aray[1]+aray[2]);i++)
                {
                    file2>>bit[1].nextstatus[i]>>bit[2].nextstatus[i];

                }
            }
            if(aray[3]==1)
            {
                for(int i=0;i<pow(2,aray[1]+aray[2]);i++)
                {
                    file2>>bit[1].nextstatus[i];
                }
            }
        }
        if(check[2]=='O')
        {
            //cout<<"check6"<<endl;
                if(aray[3]==2)
            {
                for(int i=0;i<pow(2,aray[1]+aray[2]);i++)
                {
                    file2>>bit[3].nextstatus[i];

                }break;
            }
            if(aray[3]==1)
            {
                for(int i=0;i<pow(2,aray[1]+aray[2]);i++)
                {
                    file2>>bit[3].nextstatus[i];
                }break;
            }
        }



    }file2.close();
    /*for(int i=1;i<=4;i++)
    {
        cout<<aray[i]<<" ";
    }cout<<endl;//testbench*/

    //output kmap_input.txt
    /*for(int i=0;i<pow(2,aray[1]+aray[2]);i++)
    {
        cout<<bit[2].nextstatus[i];
    }cout<<endl;//testbench for nextstatus;*/

    string outputtitle;

    /*cout<<"Please enter file title: ";
    cin>>outputtitle;*/
    ofstream ofile("kmap_in.txt",ios::out);
    ofile<<aray[1]+aray[4]<<endl;
    for(int i=1;i<=aray[1]+aray[4];i++)
    {
       ofile<<aray[1]+aray[2]<<endl;
       switch(aray[1]+aray[2])
    {
        case 2:case2variable(i,ofile);break;
        case 3:case3variable(i,ofile);break;
        case 4:case4variable(i,ofile);break;
        default: cout<<"Error"<<endl;break;
    }
    }
    top();
    setkamp(aray[1]+aray[2]);
    buildcircuit(aray);
    return 0;
}


//------the following is going to build function----
void case2variable(int biz,ofstream &ofile)
{
    //cout<<"now is case 2"<<endl;
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[2]<<' '<<bit[biz].nextstatus[3]<<endl;
}





void case3variable(int biz,ofstream &ofile)
{
    //cout<<"now is case 3"<<endl;
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[3]<<' '<<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4]<<' '<<bit[biz].nextstatus[5]<<' '<<bit[biz].nextstatus[7]<<' '<<bit[biz].nextstatus[6]<<endl;
}


void case4variable(int biz,ofstream &ofile)
{
    //cout<<"now is case 4"<<endl;
    ofile<<bit[biz].nextstatus[0]<<' ' <<bit[biz].nextstatus[1] <<' '<<bit[biz].nextstatus[3]<<' ' <<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4] <<' '<<bit[biz].nextstatus[5] <<' '<<bit[biz].nextstatus[7] <<' '<<bit[biz].nextstatus[6]<<' '
         <<bit[biz].nextstatus[12]<<' '<<bit[biz].nextstatus[13]<<' '<<bit[biz].nextstatus[15]<<' '<<bit[biz].nextstatus[14]<<' '
         <<bit[biz].nextstatus[8] <<' '<<bit[biz].nextstatus[9] <<' '<<bit[biz].nextstatus[11]<<' '<<bit[biz].nextstatus[10]<<endl;
}

void setkamp(int variable)
{
    string absorb;
    cout<<"setmap"<<endl;
    ifstream kmap_in("kmap_out.txt",ios::in);
    if(kmap_in.is_open())
    cout<<"successfull"<<endl;
    int contorl2=0;
    string previous[50];
    int*comptr[50];
    while(!kmap_in.eof())
    {
        int bits,control,a=1;
        string input;
        getline(kmap_in,input);
        if(input[0]=='#'){bits=(input[1]-'0');control=1;}
        else
        {
            previous[contorl2]=input;
            comptr[contorl2] = &bit[bits].andgate[control][0];
            for(int i=0;i<contorl2;i++)
            {
                if(previous[i]==input)
                    {a=0;bit[bits].common++;
                    bit[bits].commonptr[1]=comptr[i];i=contorl2;
                }

                else a=1;
            }contorl2++;

            if(a==1&&input[0]>='0')for(int i=0;i<variable;i++)
            {
                bit[bits].andgate[control][i+1]=input[2*i]-48;if(input[2*i]=='0')bit[bits].invertornum++;
            }
            if(a==1&&input[0]>='0')control+=1;
            if(a==1&&input[0]>='0')bit[bits].andnumber++;
        }
    }
}
void buildcircuit(int *aray)
{
    int temp=0,Andnumber=0,Ornumber=0;
    string xyz="xyz";
    ofstream cicuit("Circuit.bench",ios::out);
    cout<<"# "<<*(aray+2)<<" input"<<endl;
    cout<<"# "<<*(aray+4)<<" output"<<endl;
    for(int i=1;i<=3;i++)temp+=bit[i].invertornum;
    cout<<"# "<<temp<<" invertor"<<endl;temp=0;
    cout<<"# "<<*(aray+1)<<" flip-flops"<<endl;
    for(int i=1;i<=20;i++)
    {
       Andnumber+=bit[i].andnumber;Ornumber+=bit[i].ornumber();
    }
    cout<<"# "<<Andnumber+Ornumber<<" gate "<<'('<<Andnumber<<" ANDs + "<<Ornumber<<" ORs )"<<endl<<endl;
    cout<<"INPUT(";for(int i=0;i<*(aray+2);i++)cout<<xyz[i];cout<<")"<<endl<<endl;
    cout<<"OUTPUT(";for(int i=0;i<*(aray+4);i++)cout<<xyz[i];cout<<")"<<endl;
    mapport(aray);

}

void mapport( int*aray)
{
    int control=1;
    string ABxyz="ABxyz",Axyz="AXYZ";
    for(int i=1;i<=*(aray+1)+*(aray+4);i++)//map the port of DFF with andgate
    {
        for(int j=1;j<=bit[i].andnumber;j++)
        {
            cout<<"N"<<control<<" = "<<"And(";
            for(int k=1;k<=*(aray+1)+*(aray+2);k++)
            {
              if(*(aray+1)==1&&bit[i].andgate[j][k]!=2)
              {
                    if(bit[i].andgate[j][k]==0)cout<<'!';
                    cout<<Axyz[k-1]<<',';
                    bit[i].andgate[j][0]=control;
              }
              if(*(aray+1)==2&&bit[i].andgate[j][k]!=2)
              {
                    if(bit[i].andgate[j][k]==0)cout<<'!';
                    cout<<ABxyz[k-1]<<',';
                    bit[i].andgate[j][0]=control;
              }
            }cout<<"\b)"<<endl;
            control++;
        }
    }
    for(int i=1;i<=*(aray+1)+*(aray+4);i++)//map the of DFF with orgate;
    {
        if(bit[i].ornumber()>0)
        {
            cout<<"D"<<ABxyz[i-1]<<" = OR(";
            if(bit[i].ornumber()==1)for(int j=1;j<=bit[i].andnumber;j++)
            {

            cout<<"N"<<bit[i].andgate[j][0]<<',';

            }if(bit[i].common>0)cout<<"N"<<*bit[i].commonptr[1]<<',';cout<<"\b)"<<endl;
        }
        else
        {
            if(bit[i].common>0)cout<<"D"<<ABxyz[i-1]<<" =  N"<<*bit[i].commonptr[1]<<endl;
            else cout<<"D"<<ABxyz[i-1]<<" = N"<<bit[i].andgate[1][0]<<endl;
        }

    }
    for(int i=1;i<=*(aray+1);i++)//map the port of final DFF
    {
        cout<<ABxyz[i-1]<<" = D(D"<<ABxyz[i-1]<<",CLK)"<<endl;
    }
}

