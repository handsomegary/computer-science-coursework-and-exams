#include <iostream>
#include <cstring>
#include <fstream>
#include <cmath>
using namespace std;

class inform//presume there will be two bit for status,two bit for input in maxium since Kmap function support 4 variable at most.
{
    public:
    string name;
    string andgate[10];//how many or gate will connect to and gate.
    string orgate[6];//the input connect to or gate.
    int kmapoutput[10][6];//store the result of the calculation of kmap output.
    int nextstatus[17];


};
void case2variable(int),case3variable(int),case4variable(int);
inform bit[4];//bit[3] is output,presume there will be only one output.
inform out[20];//if there will be more more than one output, this array will be applied.
int main()
{

    int aray[5]={1,1,1,1,1};//determine which type of kmap would applied.1 for present 2 for input,4 for output
    int aray_control=1;
    //string filetitle="state_table_2",txt=".txt";
    //cout << "please enter the filetitle: ";
    //getline(cin,filetitle);
    ifstream file1("state_table_2.txt",ios::in);
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
    ifstream file2("state_table_2.txt",ios::in);
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


    cout<<aray[1]+aray[4]<<"<--numbers of kmap"<<endl;
    for(int i=1;i<=aray[1]+aray[4];i++)
    {
       cout<<aray[1]+aray[2]<<"<--numbers of variable"<<endl;
       switch(aray[1]+aray[2])
    {
        case 2:case2variable(i);break;
        case 3:case3variable(i);break;
        case 4:case4variable(i);break;
        default: cout<<"Error"<<endl;break;
    }
    }



    return 0;
}

void case2variable(int biz)
{
    //cout<<"now is case 2"<<endl;
    cout<<bit[biz].nextstatus[0]<<bit[biz].nextstatus[1]<<bit[biz].nextstatus[2]<<bit[biz].nextstatus[3]<<endl;



}

void case3variable(int biz)
{
    //cout<<"now is case 3"<<endl;
    cout<<bit[biz].nextstatus[0]<<bit[biz].nextstatus[1]<<bit[biz].nextstatus[3]<<bit[biz].nextstatus[2]
        <<bit[biz].nextstatus[4]<<bit[biz].nextstatus[5]<<bit[biz].nextstatus[7]<<bit[biz].nextstatus[6]<<endl;
}


void case4variable(int biz)
{
    //cout<<"now is case 4"<<endl;
    cout<<bit[biz].nextstatus[0]<<bit[biz].nextstatus[1]<<bit[biz].nextstatus[3]<<bit[biz].nextstatus[2]
        <<bit[biz].nextstatus[4]<<bit[biz].nextstatus[5]<<bit[biz].nextstatus[7]<<bit[biz].nextstatus[6]
        <<bit[biz].nextstatus[12]<<bit[biz].nextstatus[13]<<bit[biz].nextstatus[15]<<bit[biz].nextstatus[14]
        <<bit[biz].nextstatus[8]<<bit[biz].nextstatus[9]<<bit[biz].nextstatus[11]<<bit[biz].nextstatus[10]<<endl;
}
