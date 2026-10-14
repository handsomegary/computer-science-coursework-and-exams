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

struct resistor {
    string resistor_name;
    string resistor_beginning;
    string resistor_end;
    long double resistor_value;
    long double resistor_current;
};
struct stage_information {
    vector<resistor> rs;
    vector<resistor> rp;
};
resistor voltage={"0","0","0",0,0};
vector<stage_information> stage(16);
int stage_number=0;
vector<long double> node_voltage;

void input_file() {
    string filename;
    cout<<"Please enter the name of the input file: ";
    while (input_filename(filename)) {
        ifstream file(filename.c_str());
        if (!file) {cout<<"The file is not found.\nPlease enter the name of the input file again: ";continue;}
        try {
            // Build a fresh circuit so repeated loads do not append stale resistors.
            vector<stage_information> loaded(16);
            string word,token;
            int count=0;
            if (!(file>>word>>count) || word!="stage" || count<1 || count>15) throw runtime_error("stage range");
            resistor source;
            if (!(file>>source.resistor_name>>source.resistor_beginning>>source.resistor_end>>token)) throw runtime_error("voltage line");
            source.resistor_value=value_compute(token);
            source.resistor_current=0;
            resistor item;
            while (file>>item.resistor_name && item.resistor_name!="end") {
                if (!(file>>item.resistor_beginning>>item.resistor_end>>token)) throw runtime_error("resistor line");
                size_t underscore=item.resistor_name.find('_');
                int index=stoi(item.resistor_name.substr(2,underscore-2));
                // Decimal IDs 10..15 must not be parsed by adding their digits.
                if (index<1 || index>count || item.resistor_name.size()<3) throw runtime_error("resistor stage");
                item.resistor_value=value_compute(token);
                item.resistor_current=0;
                if (!(item.resistor_value>0)) throw runtime_error("resistance must be positive");
                vector<resistor>& path=item.resistor_name[1]=='S'?loaded[index].rs:loaded[index].rp;
                path.push_back(item);
                if (path.size()>15) throw runtime_error("path length");
            }
            for (int i=1;i<=count;++i) if (loaded[i].rs.empty() || loaded[i].rp.empty()) throw runtime_error("missing path");
            stage=loaded;stage_number=count;voltage=source;
            cout<<"Loading the file...\nThe input file successfully loaded!\n";
            return;
        } catch (const exception& error) {
            cout<<"Invalid circuit: "<<error.what()<<"\nPlease enter the name of the input file again: ";
        }
    }
}
long double path_sum(const vector<resistor>& path) {
    long double value=0;
    for (const resistor& item:path) value+=item.resistor_value;
    return value;
}
void compute() {
    // Original backward resistance reduction and forward current division.
    vector<long double> ohm(stage_number+1),load(stage_number+1);
    for (int i=stage_number;i>=1;--i) {
        long double shunt=path_sum(stage[i].rp);
        load[i]=i==stage_number?shunt:parallel_value(shunt,ohm[i+1]);
        ohm[i]=path_sum(stage[i].rs)+load[i];
    }
    node_voltage.assign(stage_number+1,0);
    node_voltage[0]=voltage.resistor_value;
    for (int i=1;i<=stage_number;++i) {
        long double series_current=node_voltage[i-1]/ohm[i];
        // Multiplication preserves tiny outputs that subtracting I*RS would erase.
        node_voltage[i]=node_voltage[i-1]*(load[i]/ohm[i]);
        long double shunt_current=node_voltage[i]/path_sum(stage[i].rp);
        for (resistor& item:stage[i].rs) item.resistor_current=series_current;
        for (resistor& item:stage[i].rp) item.resistor_current=shunt_current;
    }
}
void print_results(ostream& out) {
    compute();
    out<<"**************************************\n";
    result_row(out,"<<voltage>>","");
    result_row(out,"vdd",format_value(voltage.resistor_value,"v"));
    for (int i=1;i<=stage_number;++i) result_row(out,stage[i].rp[0].resistor_beginning,format_value(node_voltage[i],"v"));
    result_row(out,"<<current>>","");
    for (int i=1;i<=stage_number;++i) {
        for (const resistor& item:stage[i].rs) result_row(out,item.resistor_name,format_value(item.resistor_current,"A"));
        for (const resistor& item:stage[i].rp) result_row(out,item.resistor_name,format_value(item.resistor_current,"A"));
    }
    result_row(out,"","");result_row(out,"myspice2021","");
    out<<"**************************************\n";
}
void calculation() {
    cout<<"Calculating...\nThe result is successfully calculated!\n";
    print_results(cout);
}
void reconnect() {
    string beginning="vdd";
    for (int i=1;i<=stage_number;++i) {
        stage[i].rs[0].resistor_beginning=beginning;
        beginning=stage[i].rp[0].resistor_beginning;
    }
}
void swap_stage() {
    int first=0,second=0;
    if (!stage_pair(stage_number,"swap",first,second)) return;
    std::swap(stage[first],stage[second]);reconnect();
    cout<<"The stage is successfully swapped!\n";
}
void merge_stage() {
    int first=0,second=0;
    if (!stage_pair(stage_number,"merge",first,second)) return;
    long double next=path_sum(stage[second].rs)+path_sum(stage[second].rp);
    resistor merged={"RP"+to_string(first),stage[first].rp[0].resistor_beginning,"gnd",
                     parallel_value(path_sum(stage[first].rp),next),0};
    stage[first].rp.assign(1,merged);
    for (int i=second;i<stage_number;++i) stage[i]=stage[i+1];
    stage[stage_number]=stage_information();--stage_number;
    reconnect();
    cout<<"Resistors are merged into "<<merged.resistor_name<<".\n=> "<<merged.resistor_name<<" "
        <<merged.resistor_beginning<<" gnd "<<format_value(merged.resistor_value)<<"\nThe stage is Successfully merged!\n";
}
void output_file() {
    cout<<"Please enter the name of the output file: ";
    string filename;if (!input_filename(filename)) return;
    ofstream out(filename.c_str());
    if (!out) {cout<<"Cannot open output file.\n";return;}
    out<<"***circuits***\nstage "<<stage_number<<"\n\n"<<voltage.resistor_name<<" vdd gnd "
       <<fixed<<setprecision(0)<<voltage.resistor_value<<"V\n\n";
    for (int i=1;i<=stage_number;++i) {
        for (const resistor& item:stage[i].rs) out<<item.resistor_name<<" "<<item.resistor_beginning<<" "<<item.resistor_end<<" "<<format_value(item.resistor_value)<<"\n";
        for (const resistor& item:stage[i].rp) out<<item.resistor_name<<" "<<item.resistor_beginning<<" "<<item.resistor_end<<" "<<format_value(item.resistor_value)<<"\n";
    }
    out<<"end\n\n***calculation results***\n";print_results(out);
    cout<<"Exporting the file...\nThe output file successfully exported!\n";
}
int main() {
    cout<<"*** Welcome to MySpice ***\n";
    for (;;) {
        int control=selection();
        if (control==6) {cout<<"Bye~\n";return 0;}
        if (control==1) input_file();
        else if (control>=2 && control<=5) {
            if (stage_number==0) {cout<<"There is no circuit\n";continue;}
            if (control==2) calculation();
            else if (control==3) swap_stage();
            else if (control==4) merge_stage();
            else output_file();
        }
    }
}
