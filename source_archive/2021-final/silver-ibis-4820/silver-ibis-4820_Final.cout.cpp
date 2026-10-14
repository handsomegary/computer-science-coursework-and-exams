#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include "../../../assignments/2021/final/course_support/kmap.cpp"
using namespace std;

class inform
{
    public:
    string name;
    int andgate[10][5];
    int orgate[22][5];
    int kmapoutput[10][6];
    int nextstatus[17];
    int andnumber=0,invertornum=0,common=0,*commonptr[10];
    int ornumber()
    {
        if(andnumber+common>=2)return 1;
        else return 0;
    }
};
void _2_variables(int,ofstream&),_3_variables(int,ofstream&),_4_variables(int,ofstream&);
void buildcircuit(int*);
void port_map(int*);;
inform bit[24];
inform out[20];
char port[3][20];
void kmapfunction(int);
int main()
{

    int array[5]={1,1,1,1,1};//determine which type of kmap would applied.1 for present 2 for input,4 for output
    int array_control=1;
    string filetitle="state_table_1.txt";
    //cout << "please enter the filetitle: ";
    //getline(cin,filetitle);state_table_1
    ifstream file1(filetitle,ios::in);
    //if(file1.is_open())cout<<"opened"<<endl;
    file1.is_open();
    while(!file1.eof())
    {
        char check,check2;
        file1>>check;
        while(check=='(')
        {
            file1>>check2;
            if(check2=='0')array[array_control]=0;
            if(check2==',')array[array_control]++;
            if(check2==')'){array_control++;break;}
        }

    }file1.close();
    ifstream file2(filetitle,ios::in);
    while(!file2.eof())
    {
        string check;
        file2>>check;
        if(check[2]=='N')
        {
            if(array[3]==2)
            {
                for(int i=0;i<pow(2,array[1]+array[2]);i++)
                {
                    file2>>bit[1].nextstatus[i]>>bit[2].nextstatus[i];
                }
            }
            if(array[3]==1)
            {
                for(int i=0;i<pow(2,array[1]+array[2]);i++)
                {
                    file2>>bit[1].nextstatus[i];
                }
            }
        }
        if(check[2]=='O')
        {
            if(array[3]==2)
            {
                for(int i=0;i<pow(2,array[1]+array[2]);i++)
                {
                    file2>>bit[3].nextstatus[i];

                }break;
            }
            if(array[3]==1)
            {
                for(int i=0;i<pow(2,array[1]+array[2]);i++)
                {
                    file2>>bit[3].nextstatus[i];
                }break;
            }
        }
    }file2.close();
    ofstream ofile("kmap_in.txt",ios::out);
    ofile<<array[1]+array[4]<<endl;
    for(int i=1;i<=array[1]+array[4];i++)
    {
        ofile<<array[1]+array[2]<<endl;
        if(array[1]+array[2] == 2)_2_variables(i,ofile);
        else if(array[1]+array[2] == 3)_3_variables(i,ofile);
        else if(array[1]+array[2] == 4)_4_variables(i,ofile);
        else cout<<"Error"<<endl;
    }
    top();
    kmapfunction(array[1]+array[2]);
    buildcircuit(array);
    return 0;
}

void _2_variables(int biz,ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[2]<<' '<<bit[biz].nextstatus[3]<<endl;
}

void _3_variables(int biz,ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[3]<<' '<<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4]<<' '<<bit[biz].nextstatus[5]<<' '<<bit[biz].nextstatus[7]<<' '<<bit[biz].nextstatus[6]<<endl;
}


void _4_variables(int biz,ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' ' <<bit[biz].nextstatus[1] <<' '<<bit[biz].nextstatus[3]<<' ' <<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4] <<' '<<bit[biz].nextstatus[5] <<' '<<bit[biz].nextstatus[7] <<' '<<bit[biz].nextstatus[6]<<' '
         <<bit[biz].nextstatus[12]<<' '<<bit[biz].nextstatus[13]<<' '<<bit[biz].nextstatus[15]<<' '<<bit[biz].nextstatus[14]<<' '
         <<bit[biz].nextstatus[8] <<' '<<bit[biz].nextstatus[9] <<' '<<bit[biz].nextstatus[11]<<' '<<bit[biz].nextstatus[10]<<endl;
}

void kmapfunction(int variable)
{
    string absorb;
    ifstream kmap_in("kmap_out.txt",ios::in);
    kmap_in.is_open();
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
                    {
                        a=0;
                        bit[bits].common++;
                        bit[bits].commonptr[1]=comptr[i];
                        i=contorl2;
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
void buildcircuit(int *array)
{
    int temp=0,Andnumber=0,Ornumber=0;
    string xyz="xyz";
    //ofstream cout("Circuit.bench",ios::out);
    if(*(array+2)>1)cout<<"# "<<*(array+2)<<" inputs"<<endl;
    else cout<<"# "<<*(array+2)<<" input"<<endl;
    if(*(array+4)>1)cout<<"# "<<*(array+4)<<" inputs"<<endl;
    else cout<<"# "<<*(array+4)<<" output"<<endl;
    for(int i=1;i<=3;i++)temp+=bit[i].invertornum;
    cout<<"# "<<temp<<" inverter"<<endl;temp=0;
    if(*(array+1)>1)cout<<"# "<<*(array+1)<<" flip-flops"<<endl;
    else cout<<"# "<<*(array+1)<<" flip-flop"<<endl;
    for(int i=1;i<=20;i++)
    {
       Andnumber+=bit[i].andnumber;Ornumber+=bit[i].ornumber();
    }
    if((Andnumber+Ornumber)>1)cout<<"# "<<Andnumber+Ornumber<<" gates "<<'(';
    else cout<<"# "<<Andnumber+Ornumber<<" gate "<<'(';
    if(Andnumber>1)cout<<Andnumber<<" ANDs+ ";
    else cout<<Andnumber<<" AND + ";
    if(Ornumber>1)cout<<Ornumber<<" ORs)"<<endl<<endl;
    else cout<<Ornumber<<" OR)"<<endl<<endl;
    for(int i=0;i<*(array+2);i++)cout<<"INPUT("<<xyz[i]<<")"<<endl;
    cout<<endl;
    if(*(array+4));
    else for(int i=0;i<*(array+4);i++)cout<<"OUTPUT("<<xyz[i]<<")"<<endl;
    port_map(array);
}

void port_map( int*array)
{
    int control=1;
    string ABxyz="ABxyz",Axyz="AXYZ";
    //ofstream ofile("Circuit.bench",ios::app);
    for(int i=1;i<=*(array+1)+*(array+4);i++)
    {
        for(int j=1;j<=bit[i].andnumber;j++)
        {
            cout<<"N"<<control<<" = "<<"AND(";
            for(int k=1;k<=*(array+1)+*(array+2);k++)
            {
                if(*(array+1)==1&&bit[i].andgate[j][k]!=2)
                {
                    if(bit[i].andgate[j][k]==0)cout<<'!';
                    cout<<Axyz[k-1]<<',';bit[i].andgate[j][0]=control;
                }
                if(*(array+1)==2&&bit[i].andgate[j][k]!=2)
                {
                    if(bit[i].andgate[j][k]==0)cout<<'!';
                    cout<<ABxyz[k-1]<<',';bit[i].andgate[j][0]=control;
                }
            }cout<<"\b)"<<endl;
            control++;
        }
    }
    for(int i=1;i<=*(array+1)+*(array+4);i++)
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
    for(int i=1;i<=*(array+1);i++)
    {
        cout<<ABxyz[i-1]<<" = D(D"<<ABxyz[i-1]<<",CLK)"<<endl;
    }
    cout<<"#END";
}
