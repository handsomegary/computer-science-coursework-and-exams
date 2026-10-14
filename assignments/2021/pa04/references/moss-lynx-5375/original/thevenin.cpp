#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

using namespace std;

struct stage {
    string frontname, backname;
    string positions[16];
    string positionp[16];
    string nodes1[16];
    string nodes2[16];
    string nodep[16];
    int rp[16] = { 0 };
    int rs[16] = { 0 };
    double frontnamevoltage = 0;
    double backnamevoltage = 0;
    double rpsum = 0;
    double rssum = 0;
    double scurrent = 0;
    double pcurrent = 0;
    int positionscounter = 0;
    int positionpcounter = 0;
};

typedef struct stage Stage;

Stage myspice[50];

string round(double value) {//ROUND
    string result;
    if (value >= 1e3 && value < 1e6) {
        value *= 1e-3;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "k";
        return result;
    }
    else if (value >= 1 && value < 1e3) {
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3);
        return result;
    }
    else if (value >= 1e-3 && value < 1) {
        value *= 1e3;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "m";
        return result;
    }
    else if (value >= 1e-6 && value < 1e-3) {
        value *= 1e6;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "u";
        return result;
    }
}
string roundV(double value) {//ROUND
    string result;
    if (value >= 1 && value < 1e3) {
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "V";
        return result;
    }
    else if (value >= 1e-3 && value < 1) {
        value *= 1e3;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "mV";
        return result;
    }
    else if (value >= 1e-6 && value < 1e-3) {
        value *= 1e6;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "uV";
        return result;
    }
}
string roundA(double value) {//ROUND
    string result;
    if (value >= 1 && value < 1e3) {
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "A";
        return result;
    }
    else if (value >= 1e-3 && value < 1) {
        value *= 1e3;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "mA";
        return result;
    }
    else if (value >= 1e-6 && value < 1e-3) {
        value *= 1e6;
        value = (int)(value * 1000 + 0.5) / 1000.0;
        result = (to_string(value)).substr(0, (to_string(value)).length() - 3) + "uA";
        return result;
    }
}

void output(int memory, int volt, Stage* myspice, string txt, double rm[]) {//FUNCTION5 and cout
    ofstream outputfileFile(txt, ios::out);
    outputfileFile << "***curcuits***\nstage " << memory << endl << endl;
    outputfileFile << "v1 vdd gnd " << roundV(volt) << endl << endl;
    myspice[0].backname = "vdd";
    for (int x = 1;x < memory + 1; x++) {
        for (int y = 1;y <= myspice[x].positionscounter;y++) {
            if (y == 1) {
                outputfileFile << myspice[x].positions[y] << "  " << myspice[x - 1].backname << "  " << myspice[x].nodes2[y] << "  " << round(myspice[x].rs[y]) << endl;
            }
            else if (y == myspice[x].positionscounter) {
                outputfileFile << myspice[x].positions[y] << "  " << myspice[x].nodes1[y] << "  " << myspice[x].backname << "  " << round(myspice[x].rs[y]) << endl;
            }
            else {
                outputfileFile << myspice[x].positions[y] << "  " << myspice[x].nodes1[y] << "  " << myspice[x].nodes2[y] << "  " << round(myspice[x].rs[y]) << endl;
            }
        }
        for (int y = 1;y <= myspice[x].positionpcounter;y++) {
            if (myspice[x].positionpcounter == 1) {
                outputfileFile << myspice[x].positionp[y] << "  " << myspice[x].backname << "  gnd  " << round(myspice[x].rpsum) << endl;
                break;
            }
            outputfileFile << myspice[x].positionp[y] << "  " << myspice[x].nodep[y] << "  " << round(myspice[x].rp[y]) << endl;
        }
    }
    outputfileFile << "\n***calculation results*** \n**************************************\n";
    outputfileFile << left << setw(12) << "**" << setw(24) << "<<voltage>>" << "**" << endl << setw(4) << "**";
    rm[memory] = myspice[memory].rpsum + myspice[memory].rssum;
    myspice[1].frontnamevoltage = volt;
    for (int x = memory - 1;x > 0;x--) {
        rm[x] = myspice[x].rssum + (1 / ((1 / rm[x + 1]) + (1 / myspice[x].rpsum)));
    }
    for (int x = 1;x < memory + 1;x++) {
        myspice[x].backnamevoltage = myspice[x].frontnamevoltage - (myspice[x].frontnamevoltage / rm[x]) * myspice[x].rssum;
        myspice[x + 1].frontnamevoltage = myspice[x].backnamevoltage;
    }
    outputfileFile << setw(12) << "vdd" << setw(20) << roundV(volt) << "**" << endl;
    for (int x = 1;x < memory + 1;x++) {
        outputfileFile << setw(4) << "**" << setw(12) << myspice[x].backname << setw(20) << roundV(myspice[x].backnamevoltage) << "**" << endl;
    }
    for (int x = 1;x < memory + 1;x++) {
        myspice[x].scurrent = (myspice[x].frontnamevoltage - myspice[x].backnamevoltage) / myspice[x].rssum;
        myspice[x].pcurrent = myspice[x].backnamevoltage / myspice[x].rpsum;
    }
    outputfileFile << setw(12) << "**" << setw(24) << "<<current>>" << "**" << endl;
    for (int x = 1;x < memory + 1; x++) {
        for (int y = 1;y <= myspice[x].positionscounter;y++) {
            outputfileFile << setw(4) << "**" << setw(12) << myspice[x].positions[y] << setw(20) << roundA(myspice[x].scurrent) << "**" << endl;
        }
        for (int y = 1;y <= myspice[x].positionpcounter;y++) {
            outputfileFile << setw(4) << "**" << setw(12) << myspice[x].positionp[y] << setw(20) << roundA(myspice[x].pcurrent) << "**" << endl;
        }
    }
    outputfileFile << "**                                  **\n";
    outputfileFile << "**                  myspice2021     **\n";
    outputfileFile << "**************************************\n";
}

void merge(Stage* myspice, int merge1, int merge2, int& memory) {//FUNCTION4 and cout
    if (merge1 > merge2) {
        int tmpp = merge1;
        merge1 = merge2;
        merge2 = tmpp;
    }
    myspice[merge1].positionpcounter = 1;
    myspice[merge1].positionp[1] = "RP" + to_string(merge1);
    myspice[merge1].rpsum = 1 / ((1 / myspice[merge1].rpsum) + (1 / (myspice[merge2].rssum + myspice[merge2].rpsum)));
    for (int x = merge2;x < memory;x++) {
        myspice[merge2] = myspice[merge2 + 1];
    }
    memory -= 1;
    cout << "Resistors are merged into RP" << merge1 << ".\n";
    cout << "=>RP" << merge1 << " " << myspice[merge1].backname << " gnd " << round(myspice[merge1].rpsum) << endl;
    cout << "The stage is Successfully merged!\n";
}

void swap(Stage* myspice, int swap1, int swap2, int memory) {//FUNCTION3 and cout
    if (swap1 > swap2) {
        int tmpp = swap1;
        swap1 = swap2;
        swap2 = tmpp;
    }
    Stage tmp = myspice[swap1];
    myspice[0].backname = "vdd";
    cout << myspice[swap1].positions[1] << " is changing node from " << myspice[swap1 - 1].backname << " to " << myspice[swap2].backname << "..." << endl;
    cout << myspice[swap2].positions[1] << " is changing node from " << myspice[swap1].backname << " to " << myspice[swap1 - 1].backname << "..." << endl;
    if (swap2 < memory) {
        cout << myspice[swap2 + 1].positions[1] << " is changing node from " << myspice[swap2].backname << " to " << myspice[swap1].backname << "..." << endl;
    }
    myspice[swap1] = myspice[swap2];
    myspice[swap2] = tmp;
    cout << "The stage is Successfully swapped!" << endl;
}

void calculate(int memory, Stage* myspice, int volt, double rm[]) {//FUNCTION2 and cout
    cout << "***calculation results***\n";
    cout << "Calculating... \nThe result is successfully calculated!\n**************************************\n";
    cout << left << setw(12) << "**" << setw(24) << "<<voltage>>" << "**" << endl << setw(4) << "**";
    rm[memory] = myspice[memory].rpsum + myspice[memory].rssum;
    myspice[1].frontnamevoltage = volt;
    for (int x = memory - 1;x > 0;x--) {
        rm[x] = myspice[x].rssum + (1 / ((1 / rm[x + 1]) + (1 / myspice[x].rpsum)));
    }
    for (int x = 1;x < memory + 1;x++) {
        myspice[x].backnamevoltage = myspice[x].frontnamevoltage - (myspice[x].frontnamevoltage / rm[x]) * myspice[x].rssum;
        myspice[x + 1].frontnamevoltage = myspice[x].backnamevoltage;
    }
    cout << setw(12) << "vdd" << setw(20) << roundV(volt) << "**" << endl;
    for (int x = 1;x < memory + 1;x++) {
        cout << setw(4) << "**" << setw(12) << myspice[x].backname << setw(20) << roundV(myspice[x].backnamevoltage) << "**" << endl;
    }
    for (int x = 1;x < memory + 1;x++) {
        myspice[x].scurrent = (myspice[x].frontnamevoltage - myspice[x].backnamevoltage) / myspice[x].rssum;
        myspice[x].pcurrent = myspice[x].backnamevoltage / myspice[x].rpsum;
    }
    cout << setw(12) << "**" << setw(24) << "<<current>>" << "**" << endl;
    for (int x = 1;x < memory + 1; x++) {
        for (int y = 1;y <= myspice[x].positionscounter;y++) {
            cout << setw(4) << "**" << setw(12) << myspice[x].positions[y] << setw(20) << roundA(myspice[x].scurrent) << "**" << endl;
        }
        for (int y = 1;y <= myspice[x].positionpcounter;y++) {
            cout << setw(4) << "**" << setw(12) << myspice[x].positionp[y] << setw(20) << roundA(myspice[x].pcurrent) << "**" << endl;
        }
    }
    cout << "**                                  **\n";
    cout << "**                  myspice2021     **\n";
    cout << "**************************************\n";
}
void calculate1(int memory, Stage* myspice, int volt, double rm[]) {
    rm[memory] = myspice[memory].rpsum + myspice[memory].rssum;
    myspice[1].frontnamevoltage = volt;
    for (int x = memory - 1;x > 0;x--) {
        rm[x] = myspice[x].rssum + (1 / ((1 / rm[x + 1]) + (1 / myspice[x].rpsum)));
    }
    for (int x = 1;x < memory + 1;x++) {
        myspice[x].backnamevoltage = myspice[x].frontnamevoltage - (myspice[x].frontnamevoltage / rm[x]) * myspice[x].rssum;
        myspice[x + 1].frontnamevoltage = myspice[x].backnamevoltage;
    }
}
void Thevenin(Stage* myspice, int memory, int volt) {
    // calculate1(myspice, memory, volt);
    double ReT = 0;
    ReT = myspice[1].rssum * myspice[1].rpsum / (myspice[1].rssum + myspice[1].rpsum);
    for (int i = 2;i <= memory;i++) {
        ReT += myspice[i].rssum;
        ReT = myspice[i].rpsum * ReT / (myspice[i].rpsum + ReT);
    }
    string FileName;
    cout << "Please enter the name of the output file: ";
    cin >> FileName;
    cout << "Exporting the file... " << endl << "The bonus file successfully exported!" << endl;
    ofstream bounsCircuit(FileName, ios::out);
    bounsCircuit << "***Thevenin equivalence circuit***" << endl << "Rth = " << round(ReT) << endl << "Vth = " << roundV(myspice[memory].backnamevoltage);

}
int main() {
    string txt;//TXT NAME
    string trash; char trash2; //DONTCARE
    string position; //R NAME
    string unitW; //UNIT
    string unitv; //UNIT
    string node1; //NODE NAME
    string node2; //NODE NAME
    char need; //S OR P
    int vm[16];
    double rm[16] = { 0 };//並聯
    int whichone = 0;
    double volt;//volt
    double r;// r
    int colum;//whichstage
    int memory = 0;//manystage
    int row;//whichroe
    int functiontype;
    int counter = 0;
    int swap1;//swap
    int swap2;//swap
    int merge1;//merge
    int merge2;//merge
    cout << "*** Welcome to MySpice ***" << endl;
    while (1) {
    here4545:
        cout << "==========================================" << endl;
        cout << "Please type 1 to input file" << endl;
        cout << "Please type 2 to calculate voltage and current" << endl;
        cout << "Please type 3 to swap stage" << endl;
        cout << "Please type 4 to merge stage" << endl;
        cout << "Please type 5 to output file" << endl;
        cout << "Please type 6 to exit" << endl;
        cout << "Please type 7 to output Thevenin equivalence circuit " << endl;
        cout << "Enter your selection Here:";
        cin >> functiontype;
        cout << "==========================================" << endl;
        while (functiontype == 1) {////FUNCTION1
            if (counter == 0) {
                cout << "Please enter the name of the input file: ";
            }
            if (counter != 0) {
                cout << "Please enter the name of the input file again: ";
            }
            cin >> txt;
            ifstream inClientFile(txt, ios::in);
            if (!inClientFile) {
                counter = 1;
                cout << "The file is not found." << endl;
                continue;
            }
            else {
                counter = 2;
                cout << "Loading the file... " << endl;
                cout << "The input file successfully loaded!" << endl;
                inClientFile >> trash >> memory;
                inClientFile >> trash >> trash >> trash >> volt >> unitv;//unit change
                if (unitv == "uV") {
                    volt *= 1e-6;
                }
                if (unitv == "mV") {
                    volt *= 1e-3;
                }
                if (unitv == "KV") {
                    volt *= 1e3;
                }
                if (unitv == "MegV") {
                    volt *= 1e6;
                }
                if (unitv == "GV") {
                    volt *= 1e9;
                }
                while (inClientFile >> trash2 >> need >> colum >> trash2 >> row >> node1 >> node2 >> r >> unitW) {
                    if (colum == 1) {
                        myspice[1].frontname = "vdd";
                        myspice[1].backname = "n1";
                    }
                    if (colum != 1) {
                        myspice[colum].frontname = "n" + to_string(colum - 1);
                        myspice[colum].backname = "n" + to_string(colum);
                    }
                    if (need == 'S') {
                        myspice[colum].nodes1[row] = node1;
                        myspice[colum].nodes2[row] = node2;
                        myspice[colum].positionscounter += 1;
                        myspice[colum].positions[row] = "RS" + to_string(colum) + "_" + to_string(row);
                        //unit change
                        if (unitW == "u") {
                            r *= 1e-6;
                        }
                        if (unitW == "m") {
                            r *= 1e-3;
                        }
                        if (unitW == "K") {
                            r *= 1e3;
                        }
                        if (unitW == "Meg") {
                            r *= 1e6;
                        }
                        if (unitW == "G") {
                            r *= 1e9;
                        }
                        myspice[colum].rs[row] = r;
                        myspice[colum].rssum += r;
                    }
                    if (need == 'P') {
                        myspice[colum].nodep[row] = node1 + " " + node2;
                        myspice[colum].positionpcounter += 1;
                        myspice[colum].positionp[row] = "RP" + to_string(colum) + "_" + to_string(row);
                        if (unitW == "u") {
                            r *= 1e-6;
                        }
                        if (unitW == "m") {
                            r *= 1e-3;
                        }
                        if (unitW == "K") {
                            r *= 1e3;
                        }
                        if (unitW == "Meg") {
                            r *= 1e6;
                        }
                        if (unitW == "G") {
                            r *= 1e9;
                        }
                        myspice[colum].rp[row] = r;
                        myspice[colum].rpsum += r;
                    }
                }
                break;
            }
        }
        if (functiontype == 2) {
            if (memory == 0) {
                cout << "There is no circuit\n";
                goto here4545;
            }
            calculate(memory, myspice, volt, rm);
        }
        if (functiontype == 3) {
            if (memory == 0) {
                cout << "There is no circuit\n";
                goto here4545;
            }
        here3:
            cout << "Please enter the first stage you want to swap: ";
            cin >> swap1;
            if (swap1 > memory) {
                cout << "The node doesn't exist. Please enter again\n";
                goto here3;
            }
        here4:
            cout << "Please enter the second stage you want to swap: ";
            cin >> swap2;
            if (swap2 > memory) {
                cout << "The node doesn't exist. Please enter again\n";
                goto here4;
            }
            swap(myspice, swap1, swap2, memory);
        }
        if (functiontype == 4) {
            if (memory == 0) {
                cout << "There is no circuit\n";
                goto here4545;
            }
        here:
            cout << "Please enter the first stage you want to merge: ";
            cin >> merge1;
            if (merge1 > memory) {
                cout << "The node doesn't exist. Please enter again\n";
                goto here;
            }
        here2:
            cout << "Please enter the second stage you want to merge: ";
            cin >> merge2;
            if (merge2 > memory) {
                cout << "The node doesn't exist. Please enter again\n";
                goto here2;
            }
            merge(myspice, merge1, merge2, memory);
        }
        if (functiontype == 5) {
            if (memory == 0) {
                cout << "There is no circuit\n";
                goto here4545;
            }
            cout << "Please enter the name of the output file :";
            cin >> txt;
            cout << "Exporting the file... \nThe output file successfully exported!\n";
            output(memory, volt, myspice, txt, rm);
        }
        if (functiontype == 6) {
            cout << "Bye~";
            break;
        }
        if (functiontype == 7) {
            Thevenin(myspice, memory, volt);
        }
    }
}