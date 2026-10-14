// Repaired reference: retain state table -> course top() -> shared SOP gates.
#include <iostream>
#include "kmap.cpp"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <set>
#include <map>
#include <stdexcept>
#include <cctype>
#include <cstdlib>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace std;

struct state {
    vector<string> variable_name;
    vector<vector<int> > information;
    bool seen = false;
} ps, input, ns, output;

string argv2;
vector<string> variables;
vector<vector<int> > binary_values;
int actual_width = 0;
int course_width = 2;

string trim(const string& text) {
    const size_t first = text.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    return text.substr(first, text.find_last_not_of(" \t\r\n") - first + 1);
}

bool valid_name(const string& name) {
    if (name.empty() || name == "CLK" || !(isalpha(static_cast<unsigned char>(name[0])) || name[0] == '_')) return false;
    for (char c : name)
        if (!(isalnum(static_cast<unsigned char>(c)) || c == '_')) return false;
    return true;
}

string absolute_path(const string& path) {
#ifdef _WIN32
    char* resolved = _fullpath(NULL, path.c_str(), 0);
    if (resolved == NULL) throw runtime_error("cannot resolve filename");
    string answer(resolved);
    free(resolved);
    for (char& c : answer) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return answer;
#else
    char* resolved = realpath(path.c_str(), NULL);
    if (resolved != NULL) { string answer(resolved); free(resolved); return answer; }
    char* current = getcwd(NULL, 0);
    if (current == NULL) throw runtime_error("cannot resolve working directory");
    string full = path.empty() || path[0] != '/' ? string(current) + "/" + path : path;
    free(current);
    vector<string> parts;
    istringstream stream(full);
    string part;
    while (getline(stream, part, '/')) {
        if (part.empty() || part == ".") continue;
        if (part == "..") { if (!parts.empty()) parts.pop_back(); }
        else parts.push_back(part);
    }
    string answer;
    for (const string& item : parts) answer += "/" + item;
    return answer.empty() ? "/" : answer;
#endif
}

bool same_file(const string& first, const string& second) {
    if (absolute_path(first) == absolute_path(second)) return true;
#ifdef _WIN32
    HANDLE a = CreateFileA(first.c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    HANDLE b = CreateFileA(second.c_str(), FILE_READ_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                           NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    BY_HANDLE_FILE_INFORMATION one{}, two{};
    const bool equal = a != INVALID_HANDLE_VALUE && b != INVALID_HANDLE_VALUE &&
                       GetFileInformationByHandle(a, &one) && GetFileInformationByHandle(b, &two) &&
                       one.dwVolumeSerialNumber == two.dwVolumeSerialNumber &&
                       one.nFileIndexHigh == two.nFileIndexHigh && one.nFileIndexLow == two.nFileIndexLow;
    if (a != INVALID_HANDLE_VALUE) CloseHandle(a);
    if (b != INVALID_HANDLE_VALUE) CloseHandle(b);
    return equal;
#else
    struct stat a{}, b{};
    return stat(first.c_str(), &a) == 0 && stat(second.c_str(), &b) == 0 && a.st_dev == b.st_dev && a.st_ino == b.st_ino;
#endif
}

vector<string> names(const string& header) {
    const size_t first = header.find('('), last = header.find_last_of(')');
    if (first == string::npos || last == string::npos || last <= first || last != header.size() - 1)
        throw runtime_error("invalid section header");
    const string body = trim(header.substr(first + 1, last - first - 1));
    vector<string> answer;
    if (body == "0" || body.empty()) return answer;
    istringstream stream(body);
    string name;
    set<string> used;
    while (getline(stream, name, ',')) {
        name = trim(name);
        if (!valid_name(name) || !used.insert(name).second) throw runtime_error("invalid or duplicate name");
        answer.push_back(name);
    }
    if (body.back() == ',') throw runtime_error("empty name");
    return answer;
}

void read_state_file(const string& path) {
    ifstream file(path.c_str());
    if (!file) throw runtime_error("cannot open state table");
    state* section = NULL;
    bool dff = false;
    string line;
    while (getline(file, line)) {
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        line = trim(line);
        if (line.empty()) continue;
        if (line.compare(0, 10, "Flip-Flop:") == 0) {
            if (trim(line.substr(10)) != "D") throw runtime_error("this base version requires D flip-flops");
            dff = true;
            continue;
        }
        if (line.compare(0, 2, "//") == 0) {
            const size_t parenthesis = line.find('(');
            if (parenthesis == string::npos) continue;
            const string label = trim(line.substr(2, parenthesis - 2));
            if (label == "Present") section = &ps;
            else if (label == "Input") section = &input;
            else if (label == "Next") section = &ns;
            else if (label == "Output") section = &output;
            else throw runtime_error("unknown state table section");
            if (section->seen) throw runtime_error("duplicate section");
            section->seen = true;
            section->variable_name = names(line);
            continue;
        }
        if (section == NULL) throw runtime_error("data before a section");
        if (section->variable_name.empty()) {
            if (line != "0") throw runtime_error("unexpected data in an empty section");
            continue;
        }
        istringstream row(line);
        vector<int> bits;
        int value;
        while (row >> value) {
            if (value != 0 && value != 1) throw runtime_error("state table values must be 0 or 1");
            bits.push_back(value);
        }
        if (!row.eof() || bits.size() != section->variable_name.size()) throw runtime_error("invalid row width");
        section->information.push_back(bits);
    }
    if (!dff || !ps.seen || !input.seen || !ns.seen || !output.seen) throw runtime_error("missing required section");
    variables = ps.variable_name;
    variables.insert(variables.end(), input.variable_name.begin(), input.variable_name.end());
    if (variables.size() > 4) throw runtime_error("present state plus input exceeds four variables");
    actual_width = static_cast<int>(variables.size());
    course_width = actual_width < 2 ? 2 : actual_width;
    const size_t rows = size_t(1) << actual_width;
    state* sections[] = {&ps, &input, &ns, &output};
    for (state* item : sections)
        if (!item->variable_name.empty() && item->information.size() != rows) throw runtime_error("incomplete section");
    if (ns.variable_name.size() != ps.variable_name.size()) throw runtime_error("next-state width mismatch");
    vector<size_t> next_column;
    for (const string& name : ps.variable_name) {
        size_t column = 0;
        while (column < ns.variable_name.size() && ns.variable_name[column] != name) ++column;
        if (column == ns.variable_name.size()) throw runtime_error("next-state name mismatch");
        next_column.push_back(column);
    }
    set<string> used;
    for (const string& name : variables)
        if (!used.insert(name).second) throw runtime_error("duplicate state/input signal");
    for (const string& name : output.variable_name)
        if (!used.insert(name).second) throw runtime_error("output signal conflicts with state/input");
    binary_values.assign(ps.variable_name.size() + output.variable_name.size(), vector<int>(rows, 0));
    vector<bool> filled(rows, false);
    for (size_t row = 0; row < rows; ++row) {
        size_t assignment = 0;
        if (!ps.variable_name.empty())
            for (int bit : ps.information[row]) assignment = (assignment << 1) | static_cast<size_t>(bit);
        if (!input.variable_name.empty())
            for (int bit : input.information[row]) assignment = (assignment << 1) | static_cast<size_t>(bit);
        if (filled[assignment]) throw runtime_error("duplicate state/input row");
        filled[assignment] = true;
        for (size_t f = 0; f < ps.variable_name.size(); ++f) binary_values[f][assignment] = ns.information[row][next_column[f]];
        for (size_t f = 0; f < output.variable_name.size(); ++f)
            binary_values[ps.variable_name.size() + f][assignment] = output.information[row][f];
    }
}

void k_in_file() {
    ofstream file("kmap_in.txt");
    if (!file) throw runtime_error("cannot write kmap_in.txt");
    file << binary_values.size() << '\n';
    const int columns = (course_width + 1) / 2;
    for (const vector<int>& function : binary_values) {
        file << course_width << '\n';
        for (int position = 0; position < (1 << course_width); ++position) {
            const int row = position >> columns, column = position & ((1 << columns) - 1);
            const int gray = ((row ^ (row >> 1)) << columns) | (column ^ (column >> 1));
            const int assignment = gray >> (course_width - actual_width);
            file << (position ? " " : "") << function[static_cast<size_t>(assignment)];
        }
        file << '\n';
    }
    if (!file) throw runtime_error("failed writing K-map input");
    file.close();
    if (!file) throw runtime_error("failed closing K-map input");
}

string join(const vector<string>& values) {
    string text;
    for (const string& value : values) {
        if (!text.empty()) text += ',';
        text += value;
    }
    return text;
}

string fresh(const string& preferred, set<string>& used, int& counter) {
    string name = preferred;
    while (used.count(name)) name = "N" + to_string(++counter);
    used.insert(name);
    return name;
}

vector<string> process_bool(const vector<int>& cube, vector<string>& inverters,
                            vector<string>& gates, set<string>& used, int& counter) {
    vector<string> literals;
    for (int index = 0; index < actual_width; ++index) {
        if (cube[static_cast<size_t>(index)] == 2) continue;
        const size_t column = static_cast<size_t>(index);
        if (cube[column] == 1) literals.push_back(variables[column]);
        else {
            if (inverters[column].empty()) {
                inverters[column] = fresh("!" + variables[column], used, counter);
                gates.push_back(inverters[column] + " = NOT(" + variables[column] + ")");
            }
            literals.push_back(inverters[column]);
        }
    }
    return literals;
}

string process_bool_2(const vector<string>& literals, map<string, string>& products,
                     vector<string>& gates, set<string>& used, int& counter) {
    if (literals.empty()) return "1";
    if (literals.size() == 1) return literals[0];
    const string key = join(literals);
    const map<string, string>::const_iterator found = products.find(key);
    if (found != products.end()) return found->second;
    const string wire = fresh("N" + to_string(++counter), used, counter);
    products[key] = wire;
    gates.push_back(wire + " = AND(" + key + ")");
    return wire;
}

void k_out_file() {
    ifstream file("kmap_out.txt");
    if (!file) throw runtime_error("cannot read kmap_out.txt");
    typedef vector<vector<int> > bool_function;
    vector<bool_function> function_value(binary_values.size());
    size_t number = 0;
    string line;
    while (getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') {
            istringstream label(line.substr(1));
            size_t expected;
            if (!(label >> expected) || expected != number + 1 || expected > function_value.size())
                throw runtime_error("invalid K-map function marker");
            ++number;
            continue;
        }
        if (number == 0) throw runtime_error("K-map cube before marker");
        vector<int> cube;
        istringstream row(line);
        int value;
        while (row >> value) {
            if (value < 0 || value > 2) throw runtime_error("invalid K-map literal");
            cube.push_back(value);
        }
        if (!row.eof() || cube.size() != static_cast<size_t>(course_width)) throw runtime_error("invalid K-map cube width");
        function_value[number - 1].push_back(cube);
    }
    if (number != function_value.size()) throw runtime_error("missing K-map functions");
    set<string> used(variables.begin(), variables.end());
    used.insert(output.variable_name.begin(), output.variable_name.end());
    used.insert("CLK");
    vector<string> inverters(variables.size()), gates, emitted_functions, next_wires;
    map<string, string> products;
    int counter = 0, or_count = 0, buffer_count = 0;
    for (size_t f = 0; f < function_value.size(); ++f) {
        const bool next = f < ps.variable_name.size();
        const string target = next ? fresh("D" + ps.variable_name[f], used, counter)
                                   : output.variable_name[f - ps.variable_name.size()];
        if (next) next_wires.push_back(target);
        set<string> unique_terms;
        for (const vector<int>& cube : function_value[f])
            unique_terms.insert(process_bool_2(process_bool(cube, inverters, gates, used, counter),
                                               products, gates, used, counter));
        vector<string> terms(unique_terms.begin(), unique_terms.end());
        if (unique_terms.count("1")) terms.assign(1, "1");
        if (terms.empty()) terms.push_back("0");
        if (terms.size() == 1) {
            emitted_functions.push_back(target + " = BUFF(" + terms[0] + ")");
            ++buffer_count;
        } else {
            emitted_functions.push_back(target + " = OR(" + join(terms) + ")");
            ++or_count;
        }
    }
    ofstream result(argv2.c_str());
    if (!result) throw runtime_error("cannot write BENCH circuit");
    size_t inverter_count = 0;
    for (const string& inverter : inverters) inverter_count += !inverter.empty();
    result << "# " << input.variable_name.size() << " inputs\n# " << output.variable_name.size() << " outputs\n"
           << "# " << inverter_count << " inverters\n# " << ps.variable_name.size() << " flip-flops\n"
           << "# " << products.size() + static_cast<size_t>(or_count) << " gates (" << products.size()
           << " AND + " << or_count << " OR)\n# " << buffer_count << " buffers\n\n";
    for (const string& name : input.variable_name) result << "INPUT(" << name << ")\n";
    for (const string& name : output.variable_name) result << "OUTPUT(" << name << ")\n";
    result << '\n';
    for (const string& gate : gates) result << gate << '\n';
    for (const string& function : emitted_functions) result << function << '\n';
    for (size_t index = 0; index < ps.variable_name.size(); ++index)
        result << ps.variable_name[index] << " = D(" << next_wires[index] << ",CLK)\n";
    result << "#END\n";
    if (!result) throw runtime_error("failed writing BENCH circuit");
    result.close();
    if (!result) throw runtime_error("failed closing BENCH circuit");
}

int main(int argc, char* argv[]) {
    if (argc != 3) { cerr << "Usage: synth state_table circuit.bench\n"; return 1; }
    try {
        if (same_file(argv[1], argv[2]) || same_file(argv[1], "kmap_in.txt") || same_file(argv[1], "kmap_out.txt") ||
            same_file(argv[2], "kmap_in.txt") || same_file(argv[2], "kmap_out.txt") || same_file("kmap_in.txt", "kmap_out.txt"))
            throw runtime_error("state table, circuit and scratch files must be distinct");
        argv2 = argv[2];
        read_state_file(argv[1]);
        // The unchanged course module does not report a failed output open.
        ofstream preflight("kmap_out.txt", ios::trunc);
        if (!preflight) throw runtime_error("cannot write fresh K-map output");
        preflight.close();
        if (!preflight) throw runtime_error("cannot close K-map output");
        k_in_file();
        top();
        k_out_file();
    } catch (const exception& error) { cerr << error.what() << '\n'; return 1; }
    return 0;
}
