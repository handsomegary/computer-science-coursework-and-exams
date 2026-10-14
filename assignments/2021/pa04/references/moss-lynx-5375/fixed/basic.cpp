#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <stdexcept>
using namespace std;

// Repairs retain the submission's stage/path model and ladder reduction.
// Output uses extra prefixes below u: valid 999G resistors can produce pA.
long double value_compute(string token) {
    if (!token.empty() && (token.back()=='V' || token.back()=='v')) token.pop_back();
    size_t used=0;
    long double value=stold(token,&used);
    string suffix=token.substr(used);
    long double factor=1;
    if (suffix=="u") factor=1e-6L;
    else if (suffix=="m") factor=1e-3L;
    else if (suffix=="K" || suffix=="k") factor=1e3L;
    else if (suffix=="Meg") factor=1e6L;
    else if (suffix=="G") factor=1e9L;
    else if (suffix=="n") factor=1e-9L;
    else if (suffix=="p") factor=1e-12L;
    else if (suffix=="f") factor=1e-15L;
    else if (suffix=="T") factor=1e12L;
    else if (suffix=="P") factor=1e15L;
    else if (!suffix.empty()) throw runtime_error("unknown unit");
    value*=factor;
    if (!isfinite(value)) throw runtime_error("nonfinite value");
    return value;
}
long double parallel_value(long double a,long double b) {
    // Avoid product overflow and preserve very small branches.
    if (a>b) std::swap(a,b);
    return a/(1+a/b);
}
string format_value(long double value,const string& unit="") {
    if (!isfinite(value)) throw runtime_error("nonfinite result");
    if (value==0) return "0.000"+unit;
    const string prefixes[]={"f","p","n","u","m","","K","Meg","G","T","P"};
    int exponent=0;
    long double magnitude=fabsl(value);
    while (magnitude<1 && exponent>-5) {magnitude*=1000;value*=1000;--exponent;}
    while (magnitude>=999.9995L && exponent<5) {magnitude/=1000;value/=1000;++exponent;}
    ostringstream result;
    if (magnitude<1 || magnitude>=999.9995L) {
        // A 15-stage valid ladder can attenuate below the smallest prefix.
        value*=powl(1000.0L,exponent);
        result<<scientific<<setprecision(3)<<value<<unit;
    } else result<<fixed<<setprecision(3)<<value<<prefixes[exponent+5]<<unit;
    return result.str();
}
void result_row(ostream& out,const string& name,const string& value) {
    out<<"** "<<left<<setw(12)<<name<<setw(20)<<value<<" **\n";
}
int selection() {
    cout<<"==========================================\n"
        <<"Please type 1 to input file\nPlease type 2 to calculate voltage and current\n"
        <<"Please type 3 to swap stage\nPlease type 4 to merge stage\n"
        <<"Please type 5 to output file\nPlease type 6 to exit\n";
    
    cout<<"Enter your selection Here:";
    int choice=6;
    if (!(cin>>choice)) return 6;
    return choice;
}
bool stage_pair(int count,const string& verb,int& first,int& second) {
    if (count<2) {cout<<"At least two stages are required.\n";return false;}
    cout<<"Please enter the first stage you want to "<<verb<<": ";
    while (cin>>first) {
        if (first>=1 && first<=count) break;
        cout<<"The node doesn't exist. Please enter again.\n"
            <<"Please enter the first stage you want to "<<verb<<": ";
    }
    if (!cin) return false;
    cout<<"Please enter the second stage you want to "<<verb<<": ";
    while (cin>>second) {
        if (second>=1 && second<=count && abs(second-first)==1) break;
        cout<<"Choose an existing adjacent stage. Please enter again.\n"
            <<"Please enter the second stage you want to "<<verb<<": ";
    }
    if (!cin) return false;
    if (first>second) std::swap(first,second);
    return true;
}
bool input_filename(string& name) { return bool(getline(cin>>ws,name)); }

struct stage {
    string frontname,backname;
    string positions[16],positionp[16];
    string nodes1[16],nodes2[16],nodep[16];
    long double rp[16]={0},rs[16]={0};
    long double frontnamevoltage=0,backnamevoltage=0;
    long double rpsum=0,rssum=0,scurrent=0,pcurrent=0;
    int positionscounter=0,positionpcounter=0;
};
typedef struct stage Stage;
Stage myspice[50];

void input_file(int& memory,long double& volt) {
    string filename;cout<<"Please enter the name of the input file: ";
    while (input_filename(filename)) {
        ifstream file(filename.c_str());
        if (!file) {cout<<"The file is not found.\nPlease enter the name of the input file again: ";continue;}
        try {
            Stage loaded[16];
            string word,a,b,token;
            int count=0;
            if (!(file>>word>>count) || word!="stage" || count<1 || count>15) throw runtime_error("stage range");
            if (!(file>>word>>a>>b>>token)) throw runtime_error("voltage line");
            long double source=value_compute(token);
            while (file>>word && word!="end") {
                if (!(file>>a>>b>>token)) throw runtime_error("resistor line");
                size_t underscore=word.find('_');
                int colum=stoi(word.substr(2,underscore-2));
                int row=underscore==string::npos?1:stoi(word.substr(underscore+1));
                if (colum<1 || colum>count || row<1 || row>15) throw runtime_error("resistor index");
                long double resistance=value_compute(token);
                if (!(resistance>0)) throw runtime_error("resistance must be positive");
                Stage& item=loaded[colum];
                if (word[1]=='S') {
                    if (!item.positions[row].empty()) throw runtime_error("duplicate resistor index");
                    item.nodes1[row]=a;item.nodes2[row]=b;
                    item.positions[row]=word;item.rs[row]=resistance;
                    item.rssum+=resistance;++item.positionscounter;
                    if (row==1) item.frontname=a;
                    item.backname=b;
                } else {
                    if (!item.positionp[row].empty()) throw runtime_error("duplicate resistor index");
                    item.nodep[row]=a+" "+b;item.positionp[row]=word;
                    item.rp[row]=resistance;item.rpsum+=resistance;++item.positionpcounter;
                }
            }
            for (int i=1;i<=count;++i) {
                if (loaded[i].positionscounter<1 || loaded[i].positionpcounter<1) throw runtime_error("missing path");
                for (int j=1;j<=loaded[i].positionscounter;++j) if (loaded[i].positions[j].empty()) throw runtime_error("series index gap");
                for (int j=1;j<=loaded[i].positionpcounter;++j) if (loaded[i].positionp[j].empty()) throw runtime_error("shunt index gap");
            }
            for (int i=1;i<16;++i) myspice[i]=loaded[i];
            memory=count;volt=source;
            cout<<"Loading the file...\nThe input file successfully loaded!\n";return;
        } catch (const exception& error) {cout<<"Invalid circuit: "<<error.what()<<"\nPlease enter the name of the input file again: ";}
    }
}
void compute(int memory,Stage* stages,long double volt,long double rm[]) {
    // The original backward rm recurrence, then a stable forward voltage divider.
    for (int x=memory;x>=1;--x) {
        long double load=x==memory?stages[x].rpsum:parallel_value(stages[x].rpsum,rm[x+1]);
        rm[x]=stages[x].rssum+load;
    }
    long double before=volt;
    for (int x=1;x<=memory;++x) {
        Stage& item=stages[x];
        long double load=x==memory?item.rpsum:parallel_value(item.rpsum,rm[x+1]);
        item.frontnamevoltage=before;item.scurrent=before/rm[x];
        item.backnamevoltage=before*(load/rm[x]);
        item.pcurrent=item.backnamevoltage/item.rpsum;
        before=item.backnamevoltage;
    }
}
void print_results(ostream& out,int memory,Stage* stages,long double volt,long double rm[]) {
    compute(memory,stages,volt,rm);
    out<<"**************************************\n";result_row(out,"<<voltage>>","");
    result_row(out,"vdd",format_value(volt,"v"));
    for (int x=1;x<=memory;++x) result_row(out,stages[x].backname,format_value(stages[x].backnamevoltage,"v"));
    result_row(out,"<<current>>","");
    for (int x=1;x<=memory;++x) {
        for (int y=1;y<=stages[x].positionscounter;++y) result_row(out,stages[x].positions[y],format_value(stages[x].scurrent,"A"));
        for (int y=1;y<=stages[x].positionpcounter;++y) result_row(out,stages[x].positionp[y],format_value(stages[x].pcurrent,"A"));
    }
    result_row(out,"","");result_row(out,"myspice2021","");out<<"**************************************\n";
}
void calculation(int memory,Stage* stages,long double volt,long double rm[]) {
    cout<<"Calculating...\nThe result is successfully calculated!\n";
    print_results(cout,memory,stages,volt,rm);
}
void reconnect(Stage* stages,int memory) {
    string before="vdd";
    for (int x=1;x<=memory;++x) {
        stages[x].frontname=before;stages[x].nodes1[1]=before;
        before=stages[x].backname;
    }
}
void swap_stage(Stage* stages,int first,int second,int memory) {
    std::swap(stages[first],stages[second]);reconnect(stages,memory);
    cout<<"The stage is successfully swapped!\n";
}
void merge_stage(Stage* stages,int first,int second,int& memory) {
    Stage& item=stages[first];
    item.rpsum=parallel_value(item.rpsum,stages[second].rssum+stages[second].rpsum);
    item.positionpcounter=1;item.positionp[1]="RP"+to_string(first);item.rp[1]=item.rpsum;
    item.nodep[1]=item.backname+" gnd";
    // Shift each following element; the original repeatedly assigned only second.
    for (int x=second;x<memory;++x) stages[x]=stages[x+1];
    stages[memory]=Stage();--memory;reconnect(stages,memory);
    cout<<"Resistors are merged into RP"<<first<<".\n=> RP"<<first<<" "<<item.backname
        <<" gnd "<<format_value(item.rpsum)<<"\nThe stage is Successfully merged!\n";
}
void output(int memory,long double volt,Stage* stages,const string& filename,long double rm[]) {
    ofstream out(filename.c_str());if (!out) {cout<<"Cannot open output file.\n";return;}
    out<<"***circuits***\nstage "<<memory<<"\n\nv1 vdd gnd "<<fixed<<setprecision(0)<<volt<<"V\n\n";
    for (int x=1;x<=memory;++x) {
        for (int y=1;y<=stages[x].positionscounter;++y) out<<stages[x].positions[y]<<" "<<stages[x].nodes1[y]<<" "<<stages[x].nodes2[y]<<" "<<format_value(stages[x].rs[y])<<"\n";
        for (int y=1;y<=stages[x].positionpcounter;++y) out<<stages[x].positionp[y]<<" "<<stages[x].nodep[y]<<" "<<format_value(stages[x].rp[y])<<"\n";
    }
    out<<"end\n\n***calculation results***\n";print_results(out,memory,stages,volt,rm);
    cout<<"Exporting the file...\nThe output file successfully exported!\n";
}

int main() {
    int memory=0;long double volt=0;long double rm[16]={0};
    cout<<"*** Welcome to MySpice ***\n";
    for (;;) {
        int control=selection();
        if (control==6) {cout<<"Bye~\n";return 0;}
        if (control==1) input_file(memory,volt);
        else if (control>=2 && control<=5) {
            if (memory==0) {cout<<"There is no circuit\n";continue;}
            if (control==2) calculation(memory,myspice,volt,rm);
            else if (control==3 || control==4) {
                int first=0,second=0;
                if (!stage_pair(memory,control==3?"swap":"merge",first,second)) continue;
                if (control==3) swap_stage(myspice,first,second,memory);
                else merge_stage(myspice,first,second,memory);
            } else {
                cout<<"Please enter the name of the output file: ";
                string filename;if (!input_filename(filename)) return 0;
                if (control==5) output(memory,volt,myspice,filename,rm);
                
            }
        }
    }
}
