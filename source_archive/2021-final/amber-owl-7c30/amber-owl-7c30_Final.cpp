//final_project_Design synthesis
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include "amber-owl-7c30_final.h"

using namespace std;
void inputfile(char*);
void swap(int* const , int* const );
void circuit(char*);

vector<int> Present_A(100,8);
vector<int> Present_B(100,8);
vector<int> Present_C(100,8);
vector<int> Input_x(100,8);
vector<int> Input_y(100,8);
vector<int> Input_z(100,8);
vector<int> Next_A(100,8);
vector<int> Next_B(100,8);
vector<int> Next_C(100,8);
vector<int> Output(100,8);
string a;
string input;
string nextt;
string output;


int main(int argc , char *argv[])////////main funtion分割command line
{
    cout << "We have " << argc << " arguments" << endl;
    for (int i = 0 ; i < argc ; i++)
    {
        cout << "[" << i << "]" << argv[i] << endl;
    }
    Kmap k;




    inputfile(argv[1]);
    k.top();
    circuit(argv[2]);
    return 0;
}

void inputfile(char* argvv)////////////輸入state_table.txt 傳出kmap_in.txt
{

    ifstream inclientfile(argvv , ios::in);



    inclientfile.seekg(12);

    ////////傳入present/////////////


    inclientfile >> a;


    int arraysize = 0;

    if (a.length() == 12)
    {

        int num1 = 0;

        while(inclientfile >> num1 )
        {

            Present_A[arraysize] = num1;
            arraysize++;
        }


    }
    else if (a.length() == 14)
    {

        int num1 = 0;
        int num2 = 0;

        while(inclientfile >> num1 >> num2)
        {

            Present_A[arraysize] = num1;
            Present_B[arraysize] = num2;
            arraysize++;
        }


    }
    else if (a.length() == 16)
    {

        int num1 = 0;
        int num2 = 0;
        int num3 = 0;


        while(inclientfile >> num1 >> num2 >> num3)
        {

            Present_A[arraysize] = num1;
            Present_B[arraysize] = num2;
            Present_C[arraysize] = num3;
            arraysize++;
        }

    }

    ////////////傳入input//////////////////////

    inclientfile.clear();


    inclientfile >> input;


    if (input.length() == 10)
    {

        int num1 = 0;
        int i = 0;


        while(inclientfile >> num1 )
        {

            Input_x[i] = num1;
            i++;
        }

    }
    else if (input.length() == 12)
    {


        int num1 =1;
        int num2 =1;
        int i = 0;


        while(inclientfile >> num1 >> num2)
        {

            Input_x[i] = num1;
            Input_y[i] = num2;
            i++;

        }

    }
    else if (input.length() == 14)
    {


        int num1 =0;
        int num2 =0;
        int num3 =0;
        int i = 0;


        while(inclientfile >> num1 >> num2)
        {

            Input_x[i] = num1;
            Input_y[i] = num2;
            Input_z[i] = num3;
            i++;

        }

    }

    ////////////////傳入next///////////////////////



    inclientfile.clear();


    inclientfile >> nextt;



    if (nextt.length() == 9)
    {

        int num1 = 0;
        int i = 0;


        while(inclientfile >> num1 )
        {

            Next_A[i] = num1;
            i++;
        }

    }
    else if (nextt.length() == 11)
    {

        int num1 = 0;
        int num2 = 0;
        int i = 0;


        while(inclientfile >> num1 >> num2 )
        {

            Next_A[i] = num1;
            Next_B[i] = num2;
            i++;
        }

    }
    else if (nextt.length() == 13)
    {

        int num1 = 0;
        int num2 = 0;
        int num3 = 0;
        int i = 0;


        while(inclientfile >> num1 >> num2 >> num3)
        {

            Next_A[i] = num1;
            Next_B[i] = num2;
            Next_C[i] = num3;
            i++;
        }

    }

    ////////////傳入output//////////////////////



    inclientfile.clear();


    inclientfile >> output;


    int much_output = 0;
    if (output[9] != '0')
    much_output = (output.length()-11)/2 + 1;



    int num1 = 0;
    int i = 0;

    if (output[9] != '0')
    {

        while(inclientfile >> num1 )
        {

            Output[i] = num1;
            i++;
        }
    }

    int final_output[much_output][arraysize] ;

    int u = 0;
    int v = 0;

    for (int i = 0 ; i < much_output ; i++)
    {
        u = v;
        for (int j = 0 ; j < arraysize ; j++)
        {
            final_output[i][j] = Output[u];
            u += much_output;
        }
        v++;
    }



    int num_of_f = 0;///////////算Function數量


    if (nextt.length() == 9)
    {
        num_of_f += 1;
    }
    if (nextt.length() == 11)
    {
        num_of_f += 2;
    }
    if (nextt.length() == 13)
    {
        num_of_f += 3;
    }

    num_of_f +=  much_output;

    ofstream outclientfile("kmap_in.txt" , ios::out);

    outclientfile << num_of_f << endl;

    int num_of_v = 0;//////////////算varibles數量

    if ((a.length() == 12))
    {
        num_of_v += 1;
    }
    if ((a.length() == 14))
    {
        num_of_v += 2;
    }
    if ((a.length() == 16))
    {
        num_of_v += 3;
    }
    if (input.length() == 10)
    {
        num_of_v += 1;
    }
    if (input.length() == 12)
    {
        num_of_v += 2;
    }
    if (input.length() == 14)
    {
        num_of_v += 3;
    }

    if (arraysize == 8)/////////對換index順序
    {
        swap(&Next_A[2] , &Next_A[3]);
        swap(&Next_B[2] , &Next_B[3]);
        swap(&Next_C[2] , &Next_C[3]);
        swap(&Next_A[7] , &Next_A[6]);
        swap(&Next_B[7] , &Next_B[6]);
        swap(&Next_C[7] , &Next_C[6]);
        for (int i = 0 ; i < much_output ; i++)
        {
            swap(&final_output[i][2] , &final_output[i][3]);
            swap(&final_output[i][7] , &final_output[i][6]);
        }
    }
    if (arraysize == 16)
    {
        swap(&Next_A[2] , &Next_A[3]);
        swap(&Next_B[2] , &Next_B[3]);
        swap(&Next_C[2] , &Next_C[3]);
        swap(&Next_A[7] , &Next_A[6]);
        swap(&Next_B[7] , &Next_B[6]);
        swap(&Next_C[7] , &Next_C[6]);
        swap(&Next_A[11] , &Next_A[10]);
        swap(&Next_B[11] , &Next_B[10]);
        swap(&Next_C[11] , &Next_C[10]);
        swap(&Next_A[15] , &Next_A[14]);
        swap(&Next_B[15] , &Next_B[14]);
        swap(&Next_C[15] , &Next_C[14]);
        swap(&Next_A[12] , &Next_A[8]);
        swap(&Next_B[12] , &Next_B[8]);
        swap(&Next_C[12] , &Next_C[8]);
        swap(&Next_A[13] , &Next_A[9]);
        swap(&Next_B[13] , &Next_B[9]);
        swap(&Next_C[13] , &Next_C[9]);
        swap(&Next_A[15] , &Next_A[11]);
        swap(&Next_B[15] , &Next_B[11]);
        swap(&Next_C[15] , &Next_C[11]);
        swap(&Next_A[14] , &Next_A[10]);
        swap(&Next_B[14] , &Next_B[10]);
        swap(&Next_C[14] , &Next_C[10]);
        for (int i = 0 ; i < much_output ; i++)
        {
            swap(&final_output[i][2] , &final_output[i][3]);
            swap(&final_output[i][7] , &final_output[i][6]);
            swap(&final_output[i][11] , &final_output[i][10]);
            swap(&final_output[i][15] , &final_output[i][14]);
            swap(&final_output[i][12] , &final_output[i][8]);
            swap(&final_output[i][13] , &final_output[i][9]);
            swap(&final_output[i][15] , &final_output[i][11]);
            swap(&final_output[i][14] , &final_output[i][10]);
        }


    }

    int yy = 0;



    for (int i = 0 ; i < num_of_f ; i++)
    {
        outclientfile << num_of_v <<endl;

        if (i ==0 )
        {
            for (int j = 0 ; j < arraysize; j++)
            {
                if (j != arraysize-1)
                outclientfile << Next_A[j] << " ";
                else
                outclientfile << Next_A[j] << " "<< endl;
            }

        }
        else if (i ==1 && Next_B[0] != 8)
        {
            for (int j = 0 ; j < arraysize; j++)
            {
                if (j != arraysize-1)
                outclientfile << Next_B[j]<< " ";
                else
                outclientfile << Next_B[j] << " "<< endl;
            }
        }
        else if (i ==2 && Next_C[0] != 8)
        {
            for (int j = 0 ; j < arraysize; j++)
            {
                if (j != arraysize-1)
                outclientfile << Next_C[j]<< " ";
                else
                outclientfile << Next_C[j] << " "<< endl;
            }
        }
        else
        {
            for (int j = 0 ; j < arraysize; j++)
            {
                if (j != arraysize-1)
                outclientfile << final_output[yy][j]<< " ";
                else
                outclientfile << final_output[yy][j]<< " "<<endl;
            }
            yy++;
        }

    }




}

void circuit(char* argvv)//////////////////匯入kmap_out 轉成bench檔電路
{
    ifstream inclientfile("kmap_out.txt" , ios::in);

    ofstream outclientfile(argvv , ios::out);

    int input_num = 0;

    if (input.length() == 10)
    {
        input_num += 1;
    }
    else if (input.length() == 12)
    {
        input_num += 2;
    }
    else if (input.length() == 14)
    {
        input_num += 3;
    }

    int much_output = 0;


    if (output[9] != '0')
    much_output = (output.length()-11)/2 + 1;

    int num_of_n = 0;


    if (nextt.length() == 9)
    {
        num_of_n += 1;
    }
    if (nextt.length() == 11)
    {
        num_of_n += 2;
    }
    if (nextt.length() == 13)
    {
        num_of_n += 3;
    }


    int inv_num = 0;

    string v[4] = {""};//input的順序

    v[0] = a[10];

    if(a.length() == 12)
    {
        if (input.length() == 10)
        {
            v[1] = input[8];
        }
        if (input.length() == 12)
        {
            v[1] = input[8];
            v[2] = input[10];
        }
        if (input.length() == 14)
        {
            v[1] = input[8];
            v[2] = input[10];
            v[3] = input[12];
        }
    }
    if(a.length() == 14)
    {
        v[1] = a[12];
        if (input.length() == 10)
        {
            v[2] = input[8];
        }
        if (input.length() == 12)
        {
            v[2] = input[8];
            v[3] = input[10];
        }
    }
    if(a.length() == 16)
    {
        v[1] = a[12];
        v[2] = a[14];
        if (input.length() == 10)
        {
            v[3] = input[8];
        }
    }




    string str;

    while(getline(inclientfile,str))////////////計算inv數量
    {
        for (int i = 0 ; i < str.length() ; i++)
        {
            if (str[i] == '0')
            {
                inv_num ++;
            }
        }
    }

    int ff_num = 0;

    if (a.length() == 12)
    {
        ff_num += 1;
    }
    else if (a.length() == 14)
    {
        ff_num += 2;
    }
    else if (a.length() == 16)
    {
        ff_num += 3;
    }

    inclientfile.clear();
    inclientfile.seekg(0);

    int and_num = 0;
    int or_num = 0;

    struct GATE
    {
        string node[4];
        string out;


    };



    int counter = 0;
    int ORR = 0;
    string sss[100];


    while(getline(inclientfile,sss[counter]))/////////////計算and和or數量
    {
        int have_and = 0;
        if (sss[counter][0] == '#')
        {
            counter++;
            if (counter != 0)
            {
                if (ORR >= 2)
                {
                    or_num++;

                }
                ORR = 0;
            }
            continue;
        }

        for (int i = 0 ; i <  sss[counter].length() ; i++)
        {
            if  (sss[counter][i] == '1' || sss[counter][i] == '0')
            {
                have_and ++;
            }
        }
        if (have_and >= 2)
        and_num++;
        counter++;
        ORR++;

    }

    if (ORR >= 2)
    {
        or_num++;

    }



    string input_1 = "";
    string input_2 = "";
    string input_3 = "";

    input_1 = input[8];



    if (input.length() == 12)
    {
        input_2 = input[10];
    }
    if (input.length() == 14)
    {
        input_2 = input[10];
        input_3 = input[12];
    }

    for (int i = 0 ; i < and_num ; i++)
    {

    }

    inclientfile.clear();
    inclientfile.seekg(0);

    counter = 0;
    GATE and_gate[100];
    GATE or_gate[100];

    string output_name[100] = {""};

    int oo = 9;

    for(int i = 0 ; i < much_output ; i++)
    {
        output_name[i] = output[oo];
        oo+=2;
    }



///////////////////////輸出格式/////////////////////////////////


    outclientfile << "# " << input_num << " inputs" << endl;
    outclientfile << "# " << much_output << " outputs" << endl;
    outclientfile << "# " << inv_num << " inverter" << endl;
    outclientfile << "# " << ff_num << " flip-flop" << endl;
    outclientfile << "# " << and_num+or_num << " gates (" << and_num ;
    if (and_num == 1)
    outclientfile << " AND + " ;
    else
    outclientfile << " ANDs + " ;
    outclientfile << or_num ;
    if (or_num == 1)
    outclientfile << " OR)"<< endl;
    else
    outclientfile << " ORs)"<< endl;
    outclientfile << endl;
    if (input_1 != "")
    outclientfile << "INPUT(" << input_1  << ")"<< endl;
    if (input_2 != "")
    outclientfile << "INPUT(" << input_2  << ")"<< endl;
    if (input_3 != "")
    outclientfile << "INPUT(" << input_3  << ")"<< endl;
    outclientfile << endl;
    if (output[9] != '0')
    {
        for (int i = 0 ; i < much_output ; i++)
        {
            outclientfile << "OUTPUT(" << output_name[i] << ")" << endl;
        }
        outclientfile << endl;
    }
    int k = 0;
    int q = 0;
    int or_node = 0;
    int row = 0;
    int now_output = 0;
    int and_node = 0;





    while(getline(inclientfile,sss[counter]))///////////////////////////計算所有gate的節點
    {
        if (sss[counter][0] == '#')
        {
            if (counter != 0)
            {
                for (int i = 0 ; i < or_node ; i++)
                {
                    or_gate[k].node[i] = and_gate[q].out;


                    q++;
                }



                if (row == 1)
                {
                    if (sss[counter][1] - '0' <= num_of_n+1)
                    {
                        and_gate[counter-1].out = "D" + v[k];
                    }
                    else
                    {
                        and_gate[counter-1].out = output_name[now_output];
                        now_output++;


                    }
                    and_node--;

                }
                else
                {
                    if (sss[counter][1] - '0' <= num_of_n+1)
                    {
                        or_gate[k].out = "D" + v[k];
                    }
                    else
                    {
                        or_gate[k].out = output_name[now_output];
                        now_output++;

                    }
                }
                k++;

            }
            or_node = 0;
            row = 0;
            continue;
        }
        int w = 0;
        for (int i = 0; i <  sss[counter].length() ; i+=2 )
        {
            if  (sss[counter][i] == '0')
            {
                and_gate[counter].node[w] = "!" + v[i/2];
                and_gate[counter].out = "N" + to_string(and_node+1);
                outclientfile << "!" << v[i/2] << " = NOT(" << v[i/2] << ")" << endl;
                w++;


            }
            if  (sss[counter][i] == '1')
            {
                and_gate[counter].node[w] =  v[i/2];
                and_gate[counter].out = "N" + to_string(and_node+1);
                w++;


            }
        }
        if (and_gate[counter].node[1] == "")
        {
            and_gate[counter].out = and_gate[counter].node[0];
        }

        row++;
        or_node++;
        counter++;
        and_node++;
    }


    for (int i = 0 ; i < or_node ; i++)
    {

        or_gate[k].node[i] = and_gate[q].out;
        if (output[9] == '0')
        or_gate[k].out = "D" + v[k];
        else
        or_gate[k].out = output_name[now_output];
        q++;
    }



    if (row == 1)
    {
        and_gate[counter-1].out = output_name[now_output];
    }

    /////////////////////輸出格式///////////////////////////////////




    for (int i = 0 ; i < counter ; i++)
    {
        if (and_gate[i].node[1] != "" && and_gate[i].node[2] == "" && and_gate[i].node[3] == "")
        {
            outclientfile << and_gate[i].out << " = AND(" << and_gate[i].node[0] << "," << and_gate[i].node[1] << ")" << endl;
        }
        if (and_gate[i].node[1] != "" && and_gate[i].node[2] != "" && and_gate[i].node[3] == "")
        {
            outclientfile << and_gate[i].out << " = AND(" << and_gate[i].node[0] << "," << and_gate[i].node[1] << "," << and_gate[i].node[2] << ")" << endl;
        }
        if (and_gate[i].node[1] != "" && and_gate[i].node[2] != "" && and_gate[i].node[3] != "")
        {
            outclientfile << and_gate[i].out << " = AND(" << and_gate[i].node[0] << "," << and_gate[i].node[1] << "," << and_gate[i].node[2] << "," << and_gate[i].node[3] << ")" << endl;
        }
    }

    for (int i = 0 ; i < 100 ; i++)
    {
        if (or_gate[i].node[1] != "" && or_gate[i].node[2] == "" && or_gate[i].node[3] == "")
        {
            outclientfile  << or_gate[i].out << " = OR(" << or_gate[i].node[0] << "," << or_gate[i].node[1] << ")" << endl;
        }
        if (or_gate[i].node[1] != "" && or_gate[i].node[2] != "" && or_gate[i].node[3] == "")
        {
            outclientfile  << or_gate[i].out << " = OR(" << or_gate[i].node[0] << "," << or_gate[i].node[1] << "," << or_gate[i].node[2] << ")" << endl;
        }
        if (or_gate[i].node[1] != "" && or_gate[i].node[2] != "" && or_gate[i].node[3] != "")
        {
            outclientfile  << or_gate[i].out << " = OR(" << or_gate[i].node[0] << "," << or_gate[i].node[1] << "," << or_gate[i].node[2] << "," << or_gate[i].node[3] << ")" << endl;
        }
    }

    if (a.length() == 12)
    {
       outclientfile  << v[0] << " = D(D" << v[0] << ",CLK)" << endl;
    }
    if (a.length() == 14)
    {
       outclientfile  << v[0] << " = D(D" << v[0] << ",CLK)" << endl;
       outclientfile  << v[1] << " = D(D" << v[1] << ",CLK)" << endl;
    }
    if (a.length() == 16)
    {
       outclientfile  << v[0] << " = D(D" << v[0] << ",CLK)" << endl;
       outclientfile  << v[1] << " = D(D" << v[1] << ",CLK)" << endl;
       outclientfile  << v[2] << " = D(D" << v[2] << ",CLK)" << endl;
    }

    outclientfile  << "#END" << endl;


}

void swap(int* const element1, int* const element2)//////////對換swap function
{
    int hold = *element1;
    *element1 = *element2;
    *element2 = hold;

}
