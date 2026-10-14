// PA4 my spice
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
using namespace std;

struct resistor     //儲存電阻的information
{
    string resistor_name;
    string resistor_beginning;
    string resistor_end;
    float resistor_value;
    float resistor_current;
};
struct stage_information      //儲存stage的information
{
    vector <resistor> rs;
    vector <resistor> rp;

};

resistor voltage= {"0","0","0",0,0};  //儲存voltage的information
vector <stage_information> stage(16);
int stage_number=0;
// function********************************************************
int prompt(void);
float value_compute(string);

void  input_file(void);
int resister_value_count(string);

void swap_stage(void);
void merge_stage(void);
void calculation (void);
string adjust_prefix(float &);
int digit_count(float);

void ouput_file(void);
int main()
{
    int control;
    cout<<"*** Welcome to MySpice ***"<<endl;
    while(  (control=prompt())!=6   )
    {
        switch(control)
        {
        case 1:
            input_file();
            break;

        case 2:
            calculation();
            break;

        case 3:
            swap_stage();
            break;

        case 4:
            merge_stage();
            break;

        case 5:
            ouput_file();
            break;

        }

    }

}
int prompt(void)
{
    int control;
    cout<<"=========================================="<<endl;
    cout<<"Please type 1 to input file"<<endl;
    cout<<"Please type 2 to calculate voltage and current"<<endl;
    cout<<"Please type 3 to swap stage"<<endl;
    cout<<"Please type 4 to merge stage"<<endl;
    cout<<"Please type 5 to output file"<<endl;
    cout<<"Please type 6 to exit"<<endl;
    cout<<"Enter your selection Here:";
    (cin>>control).get();
    while(control>6||control<1)
    {
        cout<<"invlaid input .input again: ";
        (cin>>control).get();
    }
    if(control==6)
        cout<<"==========================================\nBye~";
    return control;
}


void   input_file(void)
{
    cout<<"=========================================="<<endl;
    string file;

    cout<<"Please enter the name of the input file:";
    getline(cin,file);
    fstream  inputfile(file,ios::in);

    while(!inputfile)
    {
        cout<<"The file is not found."<<endl;
        cout<<"Please enter the name of the input file again:";
        getline(cin,file);
        inputfile.open(file,ios::in);

    }
    cout<<"Loading the file... \nThe input file successfully loaded!"<<endl;

    //  真的開始input*******************************************************************************************
    string garbage;
    resistor input;     //暫時儲存檔案裡的資訊
    string str_value;

    inputfile >>garbage>>stage_number;      //把stage存進garbage，stage數量存進stage number
    inputfile>>voltage.resistor_name>>voltage.resistor_beginning>>voltage.resistor_end>>str_value;
    voltage.resistor_value=value_compute(str_value);
    while(inputfile>>input.resistor_name>>input.resistor_beginning>>input.resistor_end>>str_value)
    {
        int stage_i;
        input.resistor_value=value_compute(str_value);       //  5k   -->    5000
        if(input.resistor_name[1]==83  )    //  series   resistor   ASCII 83  =  s
        {
            if(input.resistor_name[3]!=95)// stage >=10   ASCII 95  =  _
            {
                stage_i=input.resistor_name[2]-48+input.resistor_name[3]-48;//    which stage it belong to
            }
            else
            {
                stage_i=input.resistor_name[2]-48;
            }

            stage[stage_i].rs.push_back(input);
        }
        else                                  //  parallel   resistor
        {
            if(input.resistor_name[3]!=95)// stage >=10   ASCII 95  =  _
            {
                stage_i=input.resistor_name[2]-48+input.resistor_name[3]-48;//    which stage it belong to
            }
            else
            {
                stage_i=input.resistor_name[2]-48;//  which stage it belong to
            }
            stage[stage_i].rp.push_back(input);
        }
    }


}
float value_compute(string input)
{
    float after=0;
    int length=input.length();
    if(input[length-1]>=48&&input[length-1]<=57)   //0~~~~~~~9
    {
        after=stof(input);
    }
    else if(input[length-1]==75)     //   K
    {
        input.erase(input.length()-1);
        after=1000*stof(input);
    }
    else if(input[length-1]==103)   //Meg
    {
        input.erase(input.length()-3,3);
        after=1000000*stof(input);
    }
    else if(input[length-1]==71)   //G
    {
        input.erase(input.length()-1);
        after=1000000000*stof(input);
    }
    else if(input[length-1]==109)     // m
    {
        input.erase(input.length()-1);
        after=0.001*stof(input);
    }
    else if(input[length-1]==117)  // u
    {
        input.erase(input.length()-1);
        after=0.000001*stof(input);
    }
    else                                 //V
    {
        if(input[length-2]>=48&&input[length-2]<=57)   //0~~~~~~~9
        {
            input.erase(input.length()-1);
            after=stof(input);
        }
        else if(input[length-2]==75)     //   K
        {
            input.erase(input.length()-2,2);
            after=1000*stof(input);
        }
        else if(input[length-2]==103)   //Meg
        {
            input.erase(input.length()-4,4);
            after=1000000*stof(input);
        }
        else if(input[length-2]==71)   //G
        {
            input.erase(input.length()-2,2);
            after=1000000000*stof(input);
        }
        else if(input[length-2]==109)     // m
        {
            input.erase(input.length()-2,2);
            after=0.001*stof(input);
        }
        else if(input[length-2]==117)  // u
        {
            input.erase(input.length()-2,2);
            after=0.000001*stof(input);
        }

    }

    return after;
}
void swap_stage(void)
{
    cout<<"=========================================="<<endl;
    if(stage_number==0)
    {
        cout<<"There is no circuit to swap."<<endl;
        return;
    }
    if(stage_number==1)
    {
        cout<<"The stage number is less then two"<<endl;
        return;
    }
    int first,second;

    cout<<"Please enter the first stage you want to swap: " ;
    cin>>first;
    while(first>stage_number)
    {
        cout<<"The node doesn’t exist. Please enter again."<<endl;
        cout<<"Please enter the first stage you want to swap: ";
        cin>>first;
    }
    cout<<"Please enter the second stage you want to swap: " ;
    cin>>second;
    while(second>stage_number||second==first)
    {
        if(second>stage_number)
            cout<<"The node doesn’t exist. Please enter again."<<endl;
        else
            cout<<"The stage is the same."<<endl;
        cout<<"Please enter the second stage you want to swap: ";
        cin>>second;
    }
    if(second<first)    //確保   second>first
    {
        int hold=first;
        first=second;
        second=hold;
    }
    string beginning_node,mid_node,end_node;
    beginning_node=stage[first].rs[0].resistor_beginning;
    int index=stage[first].rs.size();
    mid_node=stage[first].rs[index-1].resistor_end;
    index=stage[second].rs.size();
    end_node=stage[second].rs[index-1].resistor_end;
    //開始交換****************************************************
    stage_information hold=stage[first];
    stage[first]=stage[second];
    stage[second]=hold;
    //改變節點****************************************************
    string record=stage[second].rs[0].resistor_beginning;
    stage[second].rs[0].resistor_beginning=end_node;
    cout<<stage[second].rs[0].resistor_name<<" is changing node from "<<record<<" to "<<end_node<<"..."<<endl;
    record=stage[first].rs[0].resistor_beginning;
    stage[first].rs[0].resistor_beginning=beginning_node;
    cout<<stage[first].rs[0].resistor_name<<" is changing node from "<<record<<" to "<<beginning_node<<"..."<<endl;
    if(  (second+1)<=stage_number)
    {
        record=stage[second+1].rs[0].resistor_beginning;
        stage[second+1].rs[0].resistor_beginning=mid_node;
        cout<<stage[second+1].rs[0].resistor_name<<" is changing node from "<<record<<" to "<<mid_node<<"..."<<endl;
    }
    cout<<"The stage is Successfully swapped!"<<endl;


}
void merge_stage(void)
{
    cout<<"=========================================="<<endl;
    if(stage_number==0)
    {
        cout<<"There is no circuit to merge."<<endl;
        return;
    }
    if(stage_number==1)
    {
        cout<<"The stage number is less then two"<<endl;
        return;
    }
    int first,second;

    cout<<"Please enter the first stage you want to merge: " ;
    cin>>first;
    while(first>stage_number)
    {
        cout<<"The node doesn’t exist. Please enter again."<<endl;
        cout<<"Please enter the first stage you want to merge: ";
        cin>>first;
    }
    cout<<"Please enter the second stage you want to merge: " ;
    cin>>second;
    while(second>stage_number||second==first)
    {
        if(second>stage_number)
            cout<<"The node doesn’t exist. Please enter again."<<endl;
        else
            cout<<"The stage is the same."<<endl;
        cout<<"Please enter the second stage you want to merge: ";
        cin>>second;
    }
    if(second<first)    //確保   second>first
    {
        int hold=first;
        first=second;
        second=hold;
    }
    //merge**********************************************
    float second_stage_value_count=0;
    float first_stage_value_count=0;
    float total_value;
    //算電阻********************************************
    for(auto n:stage[second].rs)     //second series加總
        second_stage_value_count+=n.resistor_value;
    for(auto n:stage[second].rp)    //second parallel加總
        second_stage_value_count+=n.resistor_value;
    for(auto n:stage[first].rp)     //first parallel加總
        first_stage_value_count+=n.resistor_value;
    //  total 加總
    total_value=1/((1/second_stage_value_count)+(1/first_stage_value_count));
    resistor merge_to_one;
    merge_to_one.resistor_name="RP"+to_string(first);
    merge_to_one.resistor_beginning=stage[first].rp[0].resistor_beginning;
    merge_to_one.resistor_end="gnd";
    merge_to_one.resistor_value=total_value;
    stage[first].rp.clear();      //清空所有rp
    stage[first].rp.push_back(merge_to_one);   //變成只有一個rp
    stage_number--;
    for(int i=second; i<=stage_number; i++)
    {
        stage[i]=stage[i+1];
    }
    stage[second].rs[0].resistor_beginning=merge_to_one.resistor_beginning;
    cout<<"=>"<<merge_to_one.resistor_name<<" "<<merge_to_one.resistor_beginning<<" "<<merge_to_one.resistor_end<<" ";
    string prefix=adjust_prefix(total_value);
    cout<<fixed<<showpoint<<setprecision(3)<<total_value<<prefix<<endl;
    cout<<"The stage is Successfully merged!"<<endl;





}
string adjust_prefix(float &value)
{
    string prefix[6]= {"u","m","","K","Meg","G"};
    int index=2;
    while(value<0.999 || value>999.999)
    {
        if(value<1)
        {
            value*=1000;
            index--;
        }
        else
        {
            value/=1000;
            index++;
        }
    }
    return prefix[index];

}
void calculation(void)
{
    cout<<"=========================================="<<endl;
    if(voltage.resistor_name=="0")
    {
        cout<<"There is no circuit to calculate."<<endl;
        return;
    }
    float ohm[stage_number+1];
    float node_voltage[stage_number+1];
    //initial-----------------------------------
    for(int i=0; i<stage_number+1; i++)
        ohm[i]=0;
    for(int i=0; i<stage_number+1; i++)
        node_voltage[i]=0;
    //--------------------------------------------
    float value=0;
    for(auto n:stage[stage_number].rs)
        value+=n.resistor_value;
    for(auto n:stage[stage_number].rp)
        value+=n.resistor_value;
    ohm[stage_number]=value;

    for(int i=stage_number-1; i>=1; i--)
    {
        float total_value=0;
        float parallel_value=0;
        float series_value=0;
        for(auto n:stage[i].rp)
            parallel_value+=n.resistor_value;
        for(auto n:stage[i].rs)
            series_value+=n.resistor_value;
        total_value=series_value+1/((1/parallel_value)+(1/ohm[i+1]));
        ohm[i]=total_value;
    }
    float total_current=voltage.resistor_value/ohm[1];

    for(int i=0; i<stage[1].rs.size(); i++)
        stage[1].rs[i].resistor_current=total_current;
    for(int i=1; i<=stage_number; i++)
    {
        float parallel_value=0;  //parallel resistor total value;
        float series_current=0;
        float parallel_current=0;

        if(i!=stage_number)
        {
            for(auto n:stage[i].rp)
                parallel_value+=n.resistor_value;
            series_current=total_current*(parallel_value/(ohm[i+1]+parallel_value));
            parallel_current=total_current-series_current;
            total_current=series_current;
            for(int j=0; j<stage[i].rp.size(); j++)
                stage[i].rp[j].resistor_current=parallel_current;
            for(int j=0; j<stage[i+1].rs.size(); j++)
                stage[i+1].rs[j].resistor_current=series_current;
        }
        else  //最後一個stage
        {

            for(int j=0; j<stage[i].rp.size(); j++)
                stage[i].rp[j].resistor_current=stage[i].rs[0].resistor_current;
        }

    }
    // calculate node voltage
    node_voltage[0]=voltage.resistor_value;
    for(int i=1; i<=stage_number; i++)
    {
        float series_total_ohm=0;
        for(auto n:stage[i].rs)
            series_total_ohm+=n.resistor_value;
        node_voltage[i]=node_voltage[i-1]-series_total_ohm*stage[i].rs[0].resistor_current;

    }
    //印出結果
    float adjust_value;
    string prefix;
    cout<<"Calculating... \nThe result is successfully calculated!\n";
    cout<<"**************************************"<<endl;
    cout<<"**          <<voltage>>             **"<<endl;
    adjust_value=voltage.resistor_value;
    prefix=adjust_prefix(adjust_value);
    cout<<fixed<<showpoint<<setprecision(3)<<"**  "<<setw(12)<<left<<"vdd"<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"v"<<"**"<<endl;
    adjust_value=node_voltage[2];
    for(int i=1; i<=stage_number; i++)
    {
        adjust_value=node_voltage[i];
        prefix=adjust_prefix(adjust_value);
        cout<<"**  "<<setw(12)<<left<<stage[i].rp[0].resistor_beginning<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"v"<<"**"<<endl;
    }
    cout<<"**           <<current>>            **"<<endl;
    for(int i=1; i<=stage_number; i++)
    {
        for(auto n:stage[i].rs)
        {
            adjust_value=n.resistor_current;
            prefix=adjust_prefix(adjust_value);
            cout<<"**  "<<setw(12)<<left<<n.resistor_name<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"A"<<"**"<<endl;
        }

        for(auto n:stage[i].rp)
        {
            adjust_value=n.resistor_current;
            prefix=adjust_prefix(adjust_value);
            cout<<"**  "<<setw(12)<<left<<n.resistor_name<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"A"<<"**"<<endl;
        }

    }
    cout<<"**"<<setw(36)<<right<<"**"<<endl;
    cout<<left<<setw(20)<<"**"<<"myspice2021     **"<<endl;
    cout<<"**************************************"<<endl;
}
int digit_count(float number)
{


    int counter=0;
    while(number>=0.999)
    {
        number/=10;
        counter++;
    }
    return counter;
}
void ouput_file(void)
{
    cout<<"=========================================="<<endl;
    if(voltage.resistor_name=="0")
    {
        cout<<"There is no circuit to ouput."<<endl;
        return;
    }

    cout<<"Please enter the name of the output file: ";
    string ouput_file_name;
    getline(cin,ouput_file_name);
    cout<<"Exporting the file... \nThe output file successfully exported!"<<endl;
    fstream ouputfile(ouput_file_name,ios::out);

    float ohm[stage_number+1];
    float node_voltage[stage_number+1];
    //initial-----------------------------------
    for(int i=0; i<stage_number+1; i++)
        node_voltage[i]=0;
    //
    node_voltage[0]=voltage.resistor_value;
    for(int i=1; i<=stage_number; i++)
    {
        float series_total_ohm=0;
        for(auto n:stage[i].rs)
            series_total_ohm+=n.resistor_value;
        node_voltage[i]=node_voltage[i-1]-series_total_ohm*stage[i].rs[0].resistor_current;
    }
    float adjust_value;
    string prefix;
    adjust_value=voltage.resistor_value;
    prefix=adjust_prefix(adjust_value);
    ouputfile<<"***circuits***"<<endl;
    ouputfile<<"stage "<<stage_number<<endl<<endl;
    ouputfile<<fixed<<showpoint<<setprecision(3)<<voltage.resistor_name<<" "<<voltage.resistor_beginning<<" "<<voltage.resistor_end<<" "<<adjust_value<<prefix<<"V"<<endl<<endl;
    for(int i=1; i<=stage_number; i++)
    {
        for(auto n:stage[i].rs)
        {
            adjust_value=n.resistor_value;
            prefix=adjust_prefix(adjust_value);
            ouputfile<<left<<fixed<<showpoint<<setprecision(3)<<setw(8)<<n.resistor_name<<setw(8)<<n.resistor_beginning<<setw(8)<<n.resistor_end<<adjust_value<<prefix<<endl;

        }
        for(auto n:stage[i].rp)
        {
            adjust_value=n.resistor_value;
            prefix=adjust_prefix(adjust_value);
            ouputfile<<fixed<<showpoint<<setprecision(3)<<setw(8)<<n.resistor_name<<setw(8)<<n.resistor_beginning<<setw(8)<<n.resistor_end<<adjust_value<<prefix<<endl;

        }

    }
    ouputfile<<endl<<"***calculation results***"<<endl;
    ouputfile<<"**************************************"<<endl;
    ouputfile<<"**          <<voltage>>             **"<<endl;
    adjust_value=voltage.resistor_value;
    prefix=adjust_prefix(adjust_value);
    ouputfile<<fixed<<showpoint<<setprecision(3)<<"**  "<<setw(12)<<left<<"vdd"<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"v"<<"**"<<endl;
    adjust_value=node_voltage[2];
    for(int i=1; i<=stage_number; i++)
    {
        adjust_value=node_voltage[i];
        prefix=adjust_prefix(adjust_value);
        ouputfile<<"**  "<<setw(12)<<left<<stage[i].rp[0].resistor_beginning<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"v"<<"**"<<endl;
    }
    ouputfile<<"**           <<current>>            **"<<endl;
    for(int i=1; i<=stage_number; i++)
    {
        for(auto n:stage[i].rs)
        {
            adjust_value=n.resistor_current;
            prefix=adjust_prefix(adjust_value);
            ouputfile<<"**  "<<setw(12)<<left<<n.resistor_name<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"A"<<"**"<<endl;
        }

        for(auto n:stage[i].rp)
        {
            adjust_value=n.resistor_current;
            prefix=adjust_prefix(adjust_value);
            ouputfile<<"**  "<<setw(12)<<left<<n.resistor_name<<adjust_value<<prefix<<setw(20-4-digit_count(adjust_value)-prefix.length())<<left<<"A"<<"**"<<endl;
        }

    }
    ouputfile<<"**"<<setw(36)<<right<<"**"<<endl;
    ouputfile<<left<<setw(20)<<"**"<<"myspice2021     **"<<endl;
    ouputfile<<"**************************************"<<endl;


}

