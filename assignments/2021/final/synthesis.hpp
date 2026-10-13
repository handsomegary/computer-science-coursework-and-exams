#ifndef COURSE_SYNTHESIS_HPP
#define COURSE_SYNTHESIS_HPP
#include "kmap.cpp"
#include <cctype>
#include <iostream>
#include <map>

namespace synthesis {
inline std::string trim(const std::string& s) {
    const std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
inline bool identifier(const std::string& s) {
    if (s.empty() || !(std::isalpha(static_cast<unsigned char>(s[0])) || s[0] == '_')) return false;
    for (char c : s) if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_')) return false;
    return true;
}
struct Section {
    std::vector<std::string> names;
    std::vector<std::vector<int> > rows;
};
struct Table {
    std::string ff;
    Section present, input, next, output;
    unsigned variables;
    std::vector<std::vector<int> > functions;
    std::vector<std::string> targets;
};
inline Section& section(Table& t, const std::string& label) {
    if (label == "Present") return t.present;
    if (label == "Input") return t.input;
    if (label == "Next") return t.next;
    if (label == "Output") return t.output;
    throw std::runtime_error("unknown state-table section");
}
inline Table read_table(const std::string& filename, bool extended, bool incomplete) {
    std::ifstream in(filename.c_str());
    if (!in) throw std::runtime_error("cannot open state table");
    Table t;
    Section* active = nullptr;
    std::set<std::string> seen;
    bool have_ff = false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line.compare(0, 10, "Flip-Flop:") == 0) {
            if (have_ff) throw std::runtime_error("duplicate Flip-Flop declaration");
            t.ff = trim(line.substr(10)); have_ff = true; active = nullptr;
            if (t.ff != "D" && !(extended && (t.ff == "T" || t.ff == "JK")))
                throw std::runtime_error("unsupported flip-flop type");
            continue;
        }
        if (line.compare(0, 2, "//") == 0) {
            const std::size_t left = line.find('('), right = line.find(')');
            if (left == std::string::npos || right == std::string::npos || right < left ||
                !trim(line.substr(right + 1)).empty()) throw std::runtime_error("invalid section header");
            const std::string label = trim(line.substr(2, left - 2));
            active = &section(t, label);
            if (!seen.insert(label).second) throw std::runtime_error("duplicate section");
            const std::string names = trim(line.substr(left + 1, right - left - 1));
            if (names != "0") {
                std::istringstream list(names);
                std::string name;
                std::set<std::string> unique;
                while (std::getline(list, name, ',')) {
                    name = trim(name);
                    if (!identifier(name) || !unique.insert(name).second)
                        throw std::runtime_error("invalid or duplicate node name");
                    active->names.push_back(name);
                }
                if (active->names.empty() || (!names.empty() && names.back() == ','))
                    throw std::runtime_error("empty node name");
            }
            continue;
        }
        if (!active || active->names.empty()) throw std::runtime_error("row outside nonempty section");
        std::istringstream row(line);
        std::vector<int> values;
        std::string token;
        while (row >> token) {
            if (token != "0" && token != "1") throw std::runtime_error("state-table values must be 0 or 1");
            values.push_back(token[0] - '0');
        }
        if (values.size() != active->names.size()) throw std::runtime_error("incorrect row width");
        active->rows.push_back(values);
    }
    if (!have_ff || seen.size() != 4) throw std::runtime_error("missing state-table declaration or section");
    if (std::set<std::string>(t.present.names.begin(), t.present.names.end()) !=
        std::set<std::string>(t.next.names.begin(), t.next.names.end()))
        throw std::runtime_error("Next nodes must match Present nodes");
    t.variables = static_cast<unsigned>(t.present.names.size() + t.input.names.size());
    if (t.variables > (extended ? 8U : 4U)) throw std::runtime_error("too many state/input variables");
    if (t.output.names.size() + (t.ff == "JK" ? 2 : 1) * t.present.names.size() > 10000)
        throw std::runtime_error("too many output functions");
    std::set<std::string> used;
    used.insert("CLK");
    for (const Section* s : std::vector<const Section*>{&t.present, &t.input, &t.output})
        for (const std::string& name : s->names)
            if (!used.insert(name).second) throw std::runtime_error("node names must be globally distinct and not CLK");
    for (const std::string& name : t.present.names) {
        if (t.ff == "JK") { t.targets.push_back("J" + name); t.targets.push_back("K" + name); }
        else t.targets.push_back(t.ff + name);
    }
    for (const std::string& target : t.targets)
        if (!used.insert(target).second) throw std::runtime_error("node conflicts with flip-flop excitation name");
    t.targets.insert(t.targets.end(), t.output.names.begin(), t.output.names.end());
    std::size_t row_count = 0;
    bool have_row_count = false;
    for (const Section* s : std::vector<const Section*>{&t.present, &t.input, &t.next, &t.output}) {
        if (!s->names.empty()) {
            if (!have_row_count) { row_count = s->rows.size(); have_row_count = true; }
            if (row_count != s->rows.size()) throw std::runtime_error("section row counts differ");
        }
    }
    if (!row_count && t.variables == 0 && t.targets.empty()) row_count = 1;
    if (!row_count) throw std::runtime_error("state table has no rows");
    t.functions.assign(t.targets.size(), std::vector<int>(std::size_t(1) << t.variables, 2));
    std::set<std::uint32_t> assigned;
    for (std::size_t r = 0; r < row_count; ++r) {
        std::uint32_t address = 0;
        for (int v : (t.present.names.empty() ? std::vector<int>() : t.present.rows[r])) address = address * 2 + v;
        for (int v : (t.input.names.empty() ? std::vector<int>() : t.input.rows[r])) address = address * 2 + v;
        if (!assigned.insert(address).second) throw std::runtime_error("duplicate state/input combination");
        std::size_t f = 0;
        for (unsigned i = 0; i < t.present.names.size(); ++i) {
            const auto it = std::find(t.next.names.begin(), t.next.names.end(), t.present.names[i]);
            const int q = t.present.rows[r][i], next = t.next.rows[r][it - t.next.names.begin()];
            if (t.ff == "D") t.functions[f++][address] = next;
            else if (t.ff == "T") t.functions[f++][address] = q ^ next;
            else { t.functions[f++][address] = q ? 2 : next; t.functions[f++][address] = q ? 1 - next : 2; }
        }
        for (unsigned i = 0; i < t.output.names.size(); ++i) t.functions[f++][address] = t.output.rows[r][i];
    }
    if (!incomplete && assigned.size() != (std::size_t(1) << t.variables))
        throw std::runtime_error("missing state/input combinations (bonus --allow-incomplete can mark them don't-care)");
    return t;
}
inline void write_kmap(const Table& t) {
    std::ofstream out("kmap_in.txt");
    if (!out) throw std::runtime_error("cannot write kmap_in.txt");
    out << t.functions.size() << '\n';
    for (const std::vector<int>& function : t.functions) {
        out << t.variables << '\n';
        for (std::uint32_t i = 0; i < (std::uint32_t(1) << t.variables); ++i)
            out << (i ? " " : "") << function[gray_assignment(t.variables, i)];
        out << '\n';
    }
    if (!out) throw std::runtime_error("failed writing kmap_in.txt");
}
inline std::vector<std::vector<logic::Cube> > read_minimized(const Table& t) {
    std::ifstream in("kmap_out.txt");
    if (!in) throw std::runtime_error("cannot open kmap_out.txt");
    std::vector<std::vector<logic::Cube> > functions(t.functions.size());
    std::size_t index = 0;
    bool have_function = false;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') {
            std::istringstream marker(line.substr(1));
            int number;
            std::string rest;
            if (!(marker >> number) || (marker >> rest) || number < 1 ||
                static_cast<std::size_t>(number) != index + 1 ||
                static_cast<std::size_t>(number) > functions.size())
                throw std::runtime_error("invalid minimized function label");
            ++index; have_function = true; continue;
        }
        if (!have_function) throw std::runtime_error("cube before function label");
        std::istringstream row(line);
        std::string token;
        logic::Cube c;
        unsigned width = 0;
        while (row >> token) {
            if (token != "0" && token != "1" && token != "2") throw std::runtime_error("invalid minimized cube");
            if (t.variables == 0) {
                if (token != "1" || width) throw std::runtime_error("invalid constant-one marker");
            } else {
                c.bits = (c.bits << 1) | (token == "1" ? 1 : 0);
                c.mask = (c.mask << 1) | (token != "2" ? 1 : 0);
            }
            ++width;
        }
        if (width != (t.variables ? t.variables : 1)) throw std::runtime_error("invalid minimized cube width");
        functions[index - 1].push_back(c);
    }
    if (index != functions.size()) throw std::runtime_error("missing minimized function");
    return functions;
}
inline std::string joined(const std::vector<std::string>& names) {
    std::string s;
    for (const std::string& n : names) { if (!s.empty()) s += ','; s += n; }
    return s;
}
inline void write_bench(const Table& t, const std::vector<std::vector<logic::Cube> >& functions,
                        const std::string& filename) {
    std::vector<std::string> variables = t.present.names;
    variables.insert(variables.end(), t.input.names.begin(), t.input.names.end());
    std::set<std::string> reserved(variables.begin(), variables.end());
    reserved.insert(t.targets.begin(), t.targets.end()); reserved.insert("CLK");
    std::map<std::string, std::string> products;
    std::set<std::string> inverted;
    std::vector<std::string> gates, equations;
    unsigned wire = 1, and_count = 0, or_count = 0, buffer_count = 0;
    for (unsigned f = 0; f < functions.size(); ++f) {
        std::vector<std::string> terms;
        for (const logic::Cube& c : functions[f]) {
            std::vector<std::string> literals;
            for (unsigned i = 0; i < variables.size(); ++i) {
                const std::uint32_t bit = std::uint32_t(1) << (variables.size() - 1 - i);
                if (!(c.mask & bit)) continue;
                std::string literal = variables[i];
                if (!(c.bits & bit)) { inverted.insert(literal); literal = "!" + literal; }
                literals.push_back(literal);
            }
            if (literals.empty()) terms.push_back("1");
            else if (literals.size() == 1) terms.push_back(literals[0]);
            else {
                const std::string key = joined(literals);
                if (!products.count(key)) {
                    std::string net;
                    do { net = "N" + std::to_string(wire++); } while (reserved.count(net));
                    reserved.insert(net); products[key] = net;
                    gates.push_back(net + " = AND(" + key + ")"); ++and_count;
                }
                terms.push_back(products[key]);
            }
        }
        if (terms.size() > 1) { equations.push_back(t.targets[f] + " = OR(" + joined(terms) + ")"); ++or_count; }
        else {
            equations.push_back(t.targets[f] + " = BUFF(" + (terms.empty() ? "0" : terms[0]) + ")");
            ++buffer_count;
        }
    }
    std::ofstream out(filename.c_str());
    if (!out) throw std::runtime_error("cannot write benchmark");
    out << "# " << t.input.names.size() << " inputs\n# " << t.output.names.size() << " outputs\n# "
        << inverted.size() << " inverters\n# " << t.present.names.size() << " flip-flops\n# "
        << and_count + or_count << " gates (" << and_count << " AND + " << or_count << " OR)\n\n";
    for (const std::string& name : t.input.names) out << "INPUT(" << name << ")\n";
    out << '\n';
    for (const std::string& name : t.output.names) out << "OUTPUT(" << name << ")\n";
    out << '\n';
    for (const std::string& name : inverted) out << '!' << name << " = NOT(" << name << ")\n";
    for (const std::string& gate : gates) out << gate << '\n';
    for (const std::string& eq : equations) out << eq << '\n';
    for (const std::string& name : t.present.names) {
        out << name << " = " << t.ff << '(';
        if (t.ff == "JK") out << 'J' << name << ",K" << name;
        else out << t.ff << name;
        out << ",CLK)\n";
    }
    // BUFF nodes are explicit aliases/constants, not AND/OR gates.
    out << "# " << buffer_count << " buffers\n#END\n";
    if (!out) throw std::runtime_error("failed writing benchmark");
}
inline bool intermediate_name(const std::string& path) {
    std::string name = path.substr(path.find_last_of("/\\") == std::string::npos ?
                                  0 : path.find_last_of("/\\") + 1);
    for (char& c : name) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    return name == "kmap_in.txt" || name == "kmap_out.txt";
}
inline int run(int argc, char* argv[], bool extended) {
    try {
        bool incomplete = false;
        if (extended && argc == 4 && std::string(argv[3]) == "--allow-incomplete") incomplete = true;
        else if (argc != 3) throw std::runtime_error("usage: synth state_table.txt circuit.bench [--allow-incomplete (bonus)]");
        const std::string input = argv[1], output = argv[2];
        if (input == output || intermediate_name(input) || intermediate_name(output))
            throw std::runtime_error("input/output conflict with intermediate files");
        const Table t = read_table(input, extended, incomplete);
        write_kmap(t); top();
        write_bench(t, read_minimized(t), output);
        return 0;
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << '\n'; return 1; }
}
} // namespace synthesis
#endif
