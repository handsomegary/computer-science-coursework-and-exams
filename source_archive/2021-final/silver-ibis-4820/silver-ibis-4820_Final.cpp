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
    int andgate[10][6];
    int orgate[22][5];
    int kmapoutput[10][6];
    int nextstatus[17];
    int andnumber=0,invertornum=0,common=0,*commonptr[10],single=0;
    int ornumber()
    {
        if(andnumber+common+single>=2)return 1;
        else return 0;
    }
};
void _2_variables(int,ofstream&),_3_variables(int,ofstream&),_4_variables(int,ofstream&);
void buildcircuit(int*, char*argv[]);
void mapport(int*,ofstream&);
inform bit[24];
string outputname[20];
string inputadDFF[4];
int N=0;
char port[3][20];
void setkmap(int);
void logisdesign1(int,int);
void logisdesign2(int,int);
void logisdesign3(int,int);
int main(int argc, char*argv[])
{

    int array[5]={1,1,1,1,1};
    int array_control=1;
    ifstream file1(argv[1],ios::in);
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
    ifstream file2(argv[1],ios::in);
    while(!file2.eof())
    {
        string check;
        file2>>check;
        if(check[2]=='N')
        {

            for(int i=1;i<=array[1];i++)
            {
                inputadDFF[i]=check[5+2*i];
            }
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
        if(check[2]=='I')
        {
            for(int i=1;i<=array[2];i++)
            {
                inputadDFF[array[1]+i]=check[6+2*i];
            }
        }
        if(check[2]=='O')
        {
            for(int i=1;i<=array[4];i++)
            {
                outputname[i]=check[7+2*i];
            }

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
    setkmap(array[1]+array[2]);
    buildcircuit(array, argv);
    return 0;
}
void _2_variables(int biz, ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[2]<<' '<<bit[biz].nextstatus[3]<<endl;
}
void _3_variables(int biz, ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' '<<bit[biz].nextstatus[1]<<' '<<bit[biz].nextstatus[3]<<' '<<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4]<<' '<<bit[biz].nextstatus[5]<<' '<<bit[biz].nextstatus[7]<<' '<<bit[biz].nextstatus[6]<<endl;
}
void _4_variables(int biz, ofstream &ofile)
{
    ofile<<bit[biz].nextstatus[0]<<' ' <<bit[biz].nextstatus[1] <<' '<<bit[biz].nextstatus[3]<<' ' <<bit[biz].nextstatus[2]<<' '
         <<bit[biz].nextstatus[4] <<' '<<bit[biz].nextstatus[5] <<' '<<bit[biz].nextstatus[7] <<' '<<bit[biz].nextstatus[6]<<' '
         <<bit[biz].nextstatus[12]<<' '<<bit[biz].nextstatus[13]<<' '<<bit[biz].nextstatus[15]<<' '<<bit[biz].nextstatus[14]<<' '
         <<bit[biz].nextstatus[8] <<' '<<bit[biz].nextstatus[9] <<' '<<bit[biz].nextstatus[11]<<' '<<bit[biz].nextstatus[10]<<endl;
}
void setkmap(int variable)
{
    string absorb;
    ifstream kmap_in("kmap_out.txt",ios::in);
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
                    a=0;bit[bits].common++;
                    bit[bits].commonptr[1]=comptr[i];i=contorl2;
                }

                else a=1;
            }contorl2++;
            int t=0;
            if(a==1&&input[0]>='0')
            for(int i=0;i<variable;i++)
            {
                bit[bits].andgate[control][i+1]=input[2*i]-48;if(input[2*i]=='0')bit[bits].invertornum++;
                if(input[2*i]=='0'||input[2*i]=='1')t++;
            }
            if(a==1&&input[0]>='0')control+=1;
            if(a==1&&input[0]>='0')bit[bits].andnumber++;
            if(a==1&&input[0]>='0'&&t==1){bit[bits].andnumber--;bit[bits].single++;bit[bits].andgate[control-1][5]=5;}
        }
    }
}
void buildcircuit(int *array, char *argv[])
{
    int temp=0,Andnumber=0,Ornumber=0;
    ofstream bench(argv[2],ios::out);
    bench<<"# "<<*(array+2)<<" input";if(*(array+2)>1)bench<<'s';bench<<endl;
    bench<<"# "<<*(array+4)<<" output";if(*(array+4)>1)bench<<'s';bench<<endl;
    for(int i=1;i<=3;i++)temp+=bit[i].invertornum;
    bench<<"# "<<temp<<" inverter";if(temp>1)bench<<'s';bench<<endl;;temp=0;
    bench<<"# "<<*(array+1)<<" flip-flop";if(*(array+1)>1)bench<<'s';bench<<endl;
    for(int i=1;i<=20;i++)
    {
       Andnumber+=bit[i].andnumber;Ornumber+=bit[i].ornumber();
    }
    bench<<"# "<<Andnumber+Ornumber<<" gate";
    if(Andnumber+Ornumber>1)bench<<"s";bench<<'('<<Andnumber<<"AND";if(Andnumber>1)bench<<"s";
    bench<<" + "<<Ornumber<<"OR";if(Ornumber>1)bench<<"s ";bench<<')'<<endl<<endl;
    for(int i=1;i<=*(array+2);i++)bench<<"INPUT("<<inputadDFF[i+*(array+1)]<<')'<<endl;
    bench<<endl;
    for(int i=1;i<=*(array+4);i++)bench<<"OUTPUT("<<outputname[i]<<")"<<endl;
    bench<<endl;
    mapport(array,bench);
}
void logisdesign1(int a, int b)
{
    cout<<a+b;
}
void logisdesign2(int c, int d)
{
    cout<<c+d;
}
void logisdesign3(int e, int f)
{
    cout<<e+f;
}
void mapport( int*array, ofstream &bench)
{
    int control=1;

    for(int i=1;i<=*(array+1)+*(array+2);i++)
    {
        int Not=0,e;
        for(int j=1;j<=bit[i].andnumber+bit[i].single;j++)
        {
            for(int k=1;k<=*(array+1)+*(array+2);k++)
            {
                if(bit[i].andgate[j][k]==0){Not=1;e=k;}
            }

        }
        if(Not==1)bench<<'!'<<inputadDFF[e]<<" = "<<"NOT("<<inputadDFF[e]<<')'<<endl;

    }
    for(int i=1;i<=*(array+1)+*(array+4);i++)
    {
        for(int j=1;j<=bit[i].andnumber+bit[i].single;j++)
        {
            bench<<"N"<<control<<" = ";
            if(bit[i].andgate[j][5]!=5)bench<<"AND(";
            int care=0,carecontrol=1;
            for(int u=1;u<=*(array+1)+*(array+2);u++)
            {
              if(bit[i].andgate[j][u]!=2)care++;
            }

            for(int k=1;k<=*(array+1)+*(array+2);k++)
            {
                if(bit[i].andgate[j][k]!=2)
                {
                    if(bit[i].andgate[j][k]==0)bench<<'!';
                    bench<<inputadDFF[k];
                    if(bit[i].andgate[j][5]!=5)
                    {if(carecontrol<care)bench<<',';carecontrol++;}
                    bit[i].andgate[j][0]=control;
                }
            }
            if(bit[i].andgate[j][5]!=5)bench<<")"<<endl;
            else bench<<endl;
            control++;
        }
    }
    for(int i=1;i<=*(array+1)+*(array+4);i++)
    {
        if(bit[i].ornumber()>0)
        {
            if(i<=*(array+1))
            {
               bench<<"D"<<inputadDFF[i]<<" = OR(";
            if(bit[i].ornumber()==1)for(int j=1;j<=bit[i].andnumber+bit[i].single;j++)
            {

            bench<<"N"<<bit[i].andgate[j][0];
            if(j<bit[i].andnumber+bit[i].single) bench<<',';

            }if(bit[i].common>0)bench<<",N"<<*bit[i].commonptr[1];bench<<")"<<endl;
            }
            else
            {
               bench<<outputname[i]<<" = OR(";
            if(bit[i].ornumber()==1)for(int j=1;j<=bit[i].andnumber+bit[i].single;j++)
            {
            bench<<"N"<<bit[i].andgate[j][0];
            if(j<bit[i].andnumber+bit[i].single) bench<<',';
            }
            if(bit[i].common>0)bench<<",N"<<*bit[i].commonptr[1];bench<<")"<<endl;
            }
        }
        else
        {
            if(i<=*(array+1))
            {
              if(bit[i].common>0)bench<<"D"<<inputadDFF[i]<<" =  N"<<*bit[i].commonptr[1]<<endl;
              else bench<<"D"<<inputadDFF[i]<<" = N"<<bit[i].andgate[1][0]<<endl;
            }
            else
            {
            if(bit[i].common>0)bench<<outputname[i]<<" =  N"<<*bit[i].commonptr[1]<<endl;
            else bench<<outputname[i-*(array+1)]<<" = N"<<bit[i].andgate[1][0]<<endl;
            }
        }
    }

    for(int i=1;i<=*(array+1);i++)
    {
        bench<<inputadDFF[i]<<" = D(D"<<inputadDFF[i]<<",CLK)"<<endl;
    }
    bench<<"#END";
}
