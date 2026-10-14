//final project
#include <iostream>
#include "kmap.cpp"
#include <fstream>
#include <string>
#include <vector>

using namespace std;
struct state
{

    int variable_number;
    string variable_name;
    vector <string> information;

}ps,input,ns,output;
void k_in_file(void);
void k_out_file(void);

template <class T>
void process_bool(vector <T> &,string );   // 1 2 1==>AX
template <class T>
void process_bool_2(vector <T> &,vector <string> & );  //BX==>N1
int main(int argc, char *argv[])
{

     extern string argv2;
     argv2=argv[2];
    string file_first_line_garbage,line="",name="",hold="";
    fstream state_file(argv[1],ios::in);

    getline(state_file,file_first_line_garbage);
    //吃 state_table
    while(!state_file.eof())
    {
        getline(state_file,line);
        if(line=="")
            continue;

        if(line[2]=='P')
        {
            name=line;

        }

        if(line[2]=='I')
        {
            name=line;

        }
        if(line[2]=='N')
        {
            name=line;

        }
        if(line[2]=='O')
        {
            name=line;

        }
        //*******************
        if(name[2]=='P')  //ps
        {

            ps.information.push_back(line);
        }
        if(name[2]=='I')  //input
        {

            input.information.push_back(line);
        }
        if(name[2]=='N')  //ns
        {

            ns.information.push_back(line);
        }
        if(name[2]=='O')  //ouput
        {

            output.information.push_back(line);
        }

    }
    // 計算 ps ns input output 變數數量
    if(ps.information[0][10]=='0')
        ps.variable_number=0;
    else
        ps.variable_number=(ps.information[0].length()-10)/2;

    if(input.information[0][8]=='0')
        input.variable_number=0;
    else
        input.variable_number=(input.information[0].length()-8)/2;

    if(ns.information[0][7]=='0')
        ns.variable_number=0;
    else
        ns.variable_number=(ns.information[0].length()-7)/2;

    if(output.information[0][9]=='0')
        output.variable_number=0;
    else
        output.variable_number=(output.information[0].length()-9)/2;


    ps.variable_name.assign(ps.information[0],10,ps.information[0].length()-1-10);
    //取得變數名稱   ex.   //Present(A,B)  ==>  A,B
    if(input.variable_number!=0)
        input.variable_name.assign(input.information[0],8,input.information[0].length()-1-8);
    else
        input.variable_name="";
    if(output.variable_number!=0)
        output.variable_name.assign(output.information[0],9,output.information[0].length()-1-9);
    else
        output.variable_name="";
    k_in_file();
    top();
    k_out_file();


    return 0;
}
string argv2;

void k_in_file(void)   //寫進 k_in_file
{
    fstream kmap_file_out("kmap_in.txt",ios::out);
    int psandinput_number,nsandoutput_number,bound=2,count1=0,count2=0;
    psandinput_number=ps.variable_number+input.variable_number;
    nsandoutput_number=ns.variable_number+output.variable_number;
    for(int i=1;i<psandinput_number;i++)  //計算k map size  ex. 2*2  2*4  4*4
        bound*=2;
    int output_array[bound+1];
    count1=bound/2;

    kmap_file_out<<nsandoutput_number<<endl;

        // 處理 NA NB.....
        for(int j=1;j<=ns.variable_number;j++)
        {

            for(int i=0;i<bound+1;i++)  //initial 0
                output_array[i]=0;
            for(int i=1;i<ns.information.size();i++)
            {

                if(ns.information[i][(j-1)*2]=='1')  //看哪一格是 1
                {
                    count2=0;
                    count1=bound/2;
                    for(int k=1;k<=ps.variable_number;k++)
                    {
                        count2+=(ps.information[i][(k-1)*2]-'0')*count1;
                        count1/=2;
                    }

                    for(int k=1;k<=input.variable_number;k++)
                    {
                        count2+=(input.information[i][(k-1)*2]-'0')*count1;
                        count1/=2;
                    }
                    //符合k_map的排法(不用第0格)
                    switch(count2)
                    {
                        case 0:output_array[1]=1;
                            break;
                        case 1:output_array[2]=1;
                            break;
                        case 2:output_array[4]=1;
                            break;
                        case 3:output_array[3]=1;
                            break;
                        case 4:output_array[5]=1;
                            break;
                        case 5:output_array[6]=1;
                            break;
                        case 6:output_array[8]=1;
                            break;
                        case 7:output_array[7]=1;
                            break;
                        case 8:output_array[13]=1;
                            break;
                        case 9:output_array[14]=1;
                            break;
                        case 10:output_array[16]=1;
                            break;
                        case 11:output_array[15]=1;
                            break;
                        case 12:output_array[9]=1;
                            break;
                        case 13:output_array[10]=1;
                            break;
                        case 14:output_array[12]=1;
                            break;
                        case 15:output_array[11]=1;
                            break;

                    }

                }//end if

            }
        kmap_file_out<<psandinput_number<<endl;
        for(int i=1;i<=bound;i++)
            kmap_file_out<<output_array[i]<<" ";
        kmap_file_out<<endl;

        }
        //處理 ouput  y z ...
        for(int j=1;j<=output.variable_number;j++)
        {

            for(int i=0;i<bound+1;i++)  //initial 0
                output_array[i]=0;
            for(int i=1;i<output.information.size();i++)
            {

                if(output.information[i][(j-1)*2]=='1')  //看哪一格是 1
                {
                    count2=0;
                    count1=bound/2;
                    for(int k=1;k<=ps.variable_number;k++)
                    {
                        count2+=(ps.information[i][(k-1)*2]-'0')*count1;
                        count1/=2;
                    }

                    for(int k=1;k<=input.variable_number;k++)
                    {
                        count2+=(input.information[i][(k-1)*2]-'0')*count1;
                        count1/=2;
                    }
                    //符合k_map的排法(不用第0格)
                    switch(count2)
                    {
                        case 0:output_array[1]=1;
                            break;
                        case 1:output_array[2]=1;
                            break;
                        case 2:output_array[4]=1;
                            break;
                        case 3:output_array[3]=1;
                            break;
                        case 4:output_array[5]=1;
                            break;
                        case 5:output_array[6]=1;
                            break;
                        case 6:output_array[8]=1;
                            break;
                        case 7:output_array[7]=1;
                            break;
                        case 8:output_array[13]=1;
                            break;
                        case 9:output_array[14]=1;
                            break;
                        case 10:output_array[16]=1;
                            break;
                        case 11:output_array[15]=1;
                            break;
                        case 12:output_array[9]=1;
                            break;
                        case 13:output_array[10]=1;
                            break;
                        case 14:output_array[12]=1;
                            break;
                        case 15:output_array[11]=1;
                            break;

                    }

                }//end if

            }
        kmap_file_out<<psandinput_number<<endl;
        for(int i=1;i<=bound;i++)
            kmap_file_out<<output_array[i]<<" ";
        kmap_file_out<<endl;

        }
}
void k_out_file(void)   //讀入  k_out_file
{
    fstream k_file_in("kmap_out.txt",ios::in);
    fstream result(argv2,ios::out);
    int psandinput_number,nsandoutput_number;
    psandinput_number=ps.variable_number+input.variable_number;
    nsandoutput_number=ns.variable_number+output.variable_number;
    string var,out_var;  //variable name
    for(int i=0;i<ps.variable_name.length();i++)   //x,y==>xy
    {
        if(ps.variable_name[i]!=',')
            var+=ps.variable_name[i];
    }
    for(int i=0;i<input.variable_name.length();i++)
    {
        if(input.variable_name[i]!=',')
            var+=input.variable_name[i];
    }
    //out var ex.na,nb,y...
    for(int i=0;i<ps.variable_name.length();i++)
    {
        if(ps.variable_name[i]!=',')
            out_var+=ps.variable_name[i];
    }
    for(int i=0;i<output.variable_name.length();i++)
    {
        if(output.variable_name[i]!=',')
            out_var+=output.variable_name[i];
    }
    struct bool_function
    {
        vector <string> bool_information;
    };
    vector <bool_function> function_value;

    struct bool_function hold;
    string temp;
    getline(k_file_in,temp);
    //吃 k_out.txt
    while(!k_file_in.eof())
    {
        getline(k_file_in,temp);
        if(temp[0]!='#')
            hold.bool_information.push_back(temp);
        if(temp[0]=='#'||k_file_in.eof())
        {
            function_value.push_back(hold);
            hold.bool_information.clear();
        }
    }
    int len=function_value.size();
    int len2=function_value[len-1].bool_information.size();
    function_value[len-1].bool_information.pop_back();
    //---------------------------------------------------------------------------
    int repeat_gate=0,inverter_count=0,and_gate=0,or_gate=0;
    int inverter_array[psandinput_number];
    for(int i=0;i<psandinput_number;i++)
        inverter_array[i]=0;

   //計算 inverter數量
    for(auto n:function_value)
        for(auto m:n.bool_information)
        {
            for(int i=0;i<m.length();i=i+2)
                if(m[i]=='0')
                    inverter_array[i/2]=1;
        }
     for(int i=0;i<psandinput_number;i++)
        inverter_count+=inverter_array[i];

    //
    vector <string> origin_gate;
    vector <string> no_repeat_gate;
    bool flag=true;
    process_bool<bool_function>(function_value,var);
    for(auto n:function_value)
        for(auto m:n.bool_information)
        {
           origin_gate.push_back(m);
        }

// 判斷有沒有repeat 的gate
    for(auto k:origin_gate)
    {
         for(auto n:no_repeat_gate)
        {
            if(k==n)
            {
                flag=false;
                    break;
            }
            else
                flag=true;
        }
        if(flag==true)
            no_repeat_gate.push_back(k);
    }


// --------------------寫進BENCH裡------------------------------------------
    result<<"# "<<input.variable_number<<" input"<<endl;
    result<<"# "<<output.variable_number<<" ouput"<<endl;
    if(inverter_count<2)
        result<<"# "<<inverter_count<<" inverter"<<endl;
    else
        result<<"# "<<inverter_count<<" inverters"<<endl;
    if(ns.variable_number<2)
        result<<"# "<<ns.variable_number<<" flip-flop"<<endl;
    else
        result<<"# "<<ns.variable_number<<" flip-flops"<<endl;
    for(int i=0;i<function_value.size();i++)
    {
        if(function_value[i].bool_information.size()>1)
        or_gate++;
    }

    for(int i=0;i<no_repeat_gate.size();i++)
    {
        if(no_repeat_gate[i].length()>1)
            and_gate++;
    }
    int total_gate=and_gate+or_gate;
    int index1=1,index2=3;
    string str1[]={""," AND"," ANDs"," OR"," ORs"};
    if(and_gate>1)
        index1=2;
    if(or_gate>1)
        index2=4;
    string str2;
    if(or_gate==0)
        str2="("+to_string(and_gate)+str1[index1]+")";
    else
        str2="("+to_string(and_gate)+str1[index1]+ " + " + to_string(or_gate)+str1[index2]+")";
    if(total_gate>1)
        result<<"# "<<total_gate<<" gates"<<str2<<endl<<endl;
    else
        result<<"# "<<total_gate<<" gate"<<str2<<endl<<endl;
    if(input.variable_number!=0)
    {
         for(int i=0;i<input.variable_name.length();i=i+2)
        result<<"INPUT("<<input.variable_name[i]<<")"<<endl;
        result<<endl;
    }

    if(output.variable_number!=0)
    {
        for(int i=0;i<output.variable_name.length();i=i+2)
        result<<"OUTPUT("<<output.variable_name[i]<<")"<<endl;
        result<<endl;
    }
    for(int i=0;i<psandinput_number;i++)
    {
        if(inverter_array[i]!=0)
        {
            result<<"!"<<var[i]<<" = NOT("<<var[i]<<")"<<endl;
        }
    }
    vector<string> N;
    for(int i=0;i<function_value.size();i++)
    {
        process_bool_2<string>(function_value[i].bool_information,N);

    }
    result.close();
    result.open(argv2,ios::app);
    for(int i=0;i<ns.variable_number;i++)
    {
        if(function_value[i].bool_information.size()>1)
        {
            result<<"D"<<out_var[i]<<" = OR(";
            for(int j=0;j<function_value[i].bool_information.size();j++)
            {
                if(j!=function_value[i].bool_information.size()-1)
                    result<<function_value[i].bool_information[j]<<',';
                else
                     result<<function_value[i].bool_information[j]<<')'<<endl;
            }
        }
        else
            result<<"D"<<out_var[i]<<" = AND("<<function_value[i].bool_information[0]<<")"<<endl;

    }
    for(int i=0;i<ns.variable_number;i++)
    {
        result<<out_var[i]<<" = "<<"D(D"<<out_var[i]<<",CLK)"<<endl;
    }
    for(int i=ns.variable_number;i<out_var.length();i++)
    {
        if(function_value[i].bool_information.size()>1)
        {
            result<<out_var[i]<<" = OR(";
            for(int j=0;j<function_value[i].bool_information.size();j++)
            {
                if(j!=function_value[i].bool_information.size()-1)
                    result<<function_value[i].bool_information[j]<<',';
                else
                     result<<function_value[i].bool_information[j]<<')'<<endl;
            }
        }
        else
            result<<out_var[i]<<" = AND("<<function_value[i].bool_information[0]<<")"<<endl;

    }
    result<<"#END"<<endl;
    result.close();

}

template <class T>
void process_bool(vector <T> &f,string var)//  121==>AX...
{
    string str="";
    for(int i=0;i<f.size();i++)
    {
        str="";
        for(int k=0;k<f[i].bool_information.size();k++)
        {
            str="";
            for(int j=0;j<f[i].bool_information[k].size();j=j+2)
            {
                if(f[i].bool_information[k][j]=='0')
                    str=str+'!'+var[j/2]+',';
                else if(f[i].bool_information[k][j]=='1')
                    str=str+var[j/2]+',';

            }

            f[i].bool_information[k]=str.substr(0,str.length()-1);
        }
    }
}
template <class T>
void process_bool_2(vector <T> &f,vector <string> &N )  //  AX==>N1   BX==>N2........
{

    fstream result(argv2,ios::app);
    int index;
    static int N_index=0;
    if(f.size()<2)
    {
        result.close();
        return;
    }

    else
    for(int i=0;i<f.size();i++)
    {
        if(f[i].size()==1)
            continue;
        if(f[i].size()==2&&f[i][0]=='!')
            continue;
        bool flag=true;
        for(int j=0;j<N.size();j++)
        {
            if(f[i]==N[j])
            {
                index=j;
                flag=false;
                break;
            }

        }
        if(flag==true)
        {
            N.push_back(f[i]);
            N_index++;
            result<<"N"<<to_string(N_index)<<" = "<<"AND("<<f[i]<<")"<<endl;
            f[i]="N"+to_string(N_index);
        }
        else
        {
            f[i]="N"+to_string(index+1);
        }
    }
    result.close();
}
