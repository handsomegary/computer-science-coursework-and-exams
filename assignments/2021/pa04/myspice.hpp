#ifndef MYSPICE_HPP
#define MYSPICE_HPP

#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace myspice {

struct Resistor {
    std::string name, from, to;
    long double ohms;
};

struct Stage {
    std::string node;
    std::vector<Resistor> series, shunt;
};

struct Circuit {
    std::string source_name;
    long double voltage;
    std::vector<Stage> stages;
    Circuit() : source_name("v1"), voltage(0) {}
};

struct Result {
    long double input_resistance, vth, rth;
    std::map<std::string, long double> voltage, current;
};

inline std::string trim(const std::string& value) {
    const std::string white = " \t\r\n";
    const std::size_t first = value.find_first_not_of(white);
    if (first == std::string::npos) return "";
    return value.substr(first, value.find_last_not_of(white) - first + 1);
}

inline int integer(const std::string& token) {
    std::istringstream in(token);
    long long value = 0;
    char extra = 0;
    if (!(in >> value) || (in >> extra) || value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max()) {
        throw std::runtime_error("an integer is required");
    }
    return static_cast<int>(value);
}

inline long double number(const std::string& token, bool resistance) {
    errno = 0;
    char* last = NULL;
    const long double mantissa = std::strtold(token.c_str(), &last);
    if (last == token.c_str() || errno == ERANGE || !std::isfinite(mantissa)) {
        throw std::runtime_error("invalid numeric value: " + token);
    }
    const std::string suffix(last);
    long double scale = 1;
    if (resistance) {
        if (suffix == "f") scale = 1e-15L;
        else if (suffix == "p") scale = 1e-12L;
        else if (suffix == "n") scale = 1e-9L;
        else if (suffix == "u") scale = 1e-6L;
        else if (suffix == "m") scale = 1e-3L;
        else if (suffix == "K" || suffix == "k") scale = 1e3L;
        else if (suffix == "Meg") scale = 1e6L;
        else if (suffix == "G") scale = 1e9L;
        else if (suffix == "T") scale = 1e12L;
        else if (suffix == "P") scale = 1e15L;
        else if (!suffix.empty()) throw std::runtime_error("invalid resistor prefix: " + suffix);
        if (!(mantissa > 0)) throw std::runtime_error("resistance must be positive");
    } else {
        if (!suffix.empty() && suffix != "V" && suffix != "v") {
            throw std::runtime_error("source voltage accepts only V or v");
        }
        if (std::floor(mantissa) != mantissa || mantissa < std::numeric_limits<int>::min() ||
            mantissa > std::numeric_limits<int>::max()) {
            throw std::runtime_error("source voltage must fit a signed 32-bit integer");
        }
    }
    const long double value = mantissa * scale;
    if (!std::isfinite(value) || (resistance && value == 0)) {
        throw std::runtime_error("numeric value is outside the supported range");
    }
    return value;
}

inline long double total(const std::vector<Resistor>& path) {
    long double value = 0;
    for (std::size_t i = 0; i < path.size(); ++i) {
        value += path[i].ohms;
        if (!std::isfinite(value)) throw std::runtime_error("path resistance is outside the supported range");
    }
    return value;
}

inline long double parallel(long double a, long double b) {
    if (!(a > 0) || !(b > 0) || !std::isfinite(a) || !std::isfinite(b)) {
        throw std::runtime_error("equivalent resistance is outside the supported range");
    }
    if (a > b) std::swap(a, b);
    const long double equivalent = a / (1 + a / b);
    if (equivalent == 0) throw std::runtime_error("equivalent resistance underflows the supported range");
    return equivalent;
}

// Path topology determines stage order, including circuits exported after swaps/merges.
inline Circuit read_circuit(std::istream& input) {
    Circuit circuit;
    int count = 0;
    bool have_count = false, have_source = false;
    std::vector<Resistor> resistors;
    std::set<std::string> names;
    std::string line;
    while (std::getline(input, line)) {
        if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        line = trim(line);
        if (line.empty() || line == "***circuits***") continue;
        if (line == "end" || line == "***calculation results***") break;
        std::istringstream fields(line);
        std::vector<std::string> words;
        std::string word;
        while (fields >> word) words.push_back(word);
        if (words[0] == "stage") {
            if (have_count || words.size() != 2) throw std::runtime_error("invalid stage header");
            count = integer(words[1]);
            if (count < 1 || count > 15) throw std::runtime_error("stage count must be 1..15");
            have_count = true;
        } else if (words[0][0] == 'v' || words[0][0] == 'V') {
            if (have_source || words.size() != 4 || words[1] != "vdd" || words[2] != "gnd") {
                throw std::runtime_error("one voltage source from vdd to gnd is required");
            }
            circuit.source_name = words[0];
            circuit.voltage = number(words[3], false);
            have_source = true;
        } else {
            if (words.size() != 4 || words[0].size() < 3 || words[0][0] != 'R' ||
                (words[0][1] != 'S' && words[0][1] != 'P') || words[1] == words[2]) {
                throw std::runtime_error("invalid resistor line");
            }
            if (!names.insert(words[0]).second) throw std::runtime_error("duplicate resistor name");
            Resistor resistor = {words[0], words[1], words[2], number(words[3], true)};
            resistors.push_back(resistor);
        }
    }
    if (!have_count || !have_source) throw std::runtime_error("stage header and voltage source are required");
    std::map<std::string, std::size_t> series_from, shunt_from;
    for (std::size_t i = 0; i < resistors.size(); ++i) {
        std::map<std::string, std::size_t>& paths =
            resistors[i].name[1] == 'S' ? series_from : shunt_from;
        if (!paths.insert(std::make_pair(resistors[i].from, i)).second) {
            throw std::runtime_error("ambiguous resistor path");
        }
    }
    std::set<std::size_t> used;
    std::set<std::string> junctions;
    std::string start = "vdd";
    for (int i = 0; i < count; ++i) {
        Stage stage;
        std::string node = start;
        do {
            const std::map<std::string, std::size_t>::const_iterator found = series_from.find(node);
            if (found == series_from.end() || !used.insert(found->second).second) {
                throw std::runtime_error("broken or cyclic series path");
            }
            const Resistor& resistor = resistors[found->second];
            stage.series.push_back(resistor);
            if (stage.series.size() > 15) throw std::runtime_error("each series path must have 1..15 resistors");
            node = resistor.to;
        } while (shunt_from.find(node) == shunt_from.end());
        stage.node = node;
        if (node == "vdd" || node == "gnd" || !junctions.insert(node).second) {
            throw std::runtime_error("invalid stage junction");
        }
        do {
            const std::map<std::string, std::size_t>::const_iterator found = shunt_from.find(node);
            if (found == shunt_from.end() || !used.insert(found->second).second) {
                throw std::runtime_error("broken or cyclic shunt path");
            }
            const Resistor& resistor = resistors[found->second];
            stage.shunt.push_back(resistor);
            if (stage.shunt.size() > 15) throw std::runtime_error("each shunt path must have 1..15 resistors");
            node = resistor.to;
        } while (node != "gnd");
        circuit.stages.push_back(stage);
        start = stage.node;
    }
    if (used.size() != resistors.size() || series_from.find(start) != series_from.end()) {
        throw std::runtime_error("stage count does not match the ladder topology");
    }
    // Internal nodes belong to exactly one resistor path. Cross-connections are not ladders.
    std::map<std::string, int> incidence;
    for (std::size_t i = 0; i < resistors.size(); ++i) {
        ++incidence[resistors[i].from];
        ++incidence[resistors[i].to];
    }
    for (std::map<std::string, int>::const_iterator it = incidence.begin(); it != incidence.end(); ++it) {
        if (it->first != "vdd" && it->first != "gnd" && junctions.count(it->first) == 0 && it->second != 2) {
            throw std::runtime_error("an internal node must connect exactly two resistors");
        }
    }
    if (incidence["vdd"] != 1 || incidence["gnd"] != count) {
        throw std::runtime_error("invalid ladder terminals");
    }
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        if (incidence[circuit.stages[i].node] != (i + 1 == circuit.stages.size() ? 2 : 3)) {
            throw std::runtime_error("invalid stage junction connections");
        }
    }
    return circuit;
}

inline Circuit load(const std::string& filename) {
    std::ifstream file(filename.c_str());
    if (!file) throw std::runtime_error("The file is not found.");
    return read_circuit(file);
}

inline Result solve(const Circuit& circuit) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    const std::size_t count = circuit.stages.size();
    std::vector<long double> load_at(count);
    for (std::size_t k = count; k > 0; --k) {
        const std::size_t i = k - 1;
        const long double shunt = total(circuit.stages[i].shunt);
        load_at[i] = i + 1 == count ? shunt :
            parallel(shunt, total(circuit.stages[i + 1].series) + load_at[i + 1]);
    }
    Result result;
    result.input_resistance = total(circuit.stages[0].series) + load_at[0];
    if (!std::isfinite(result.input_resistance)) throw std::runtime_error("input resistance is outside the supported range");
    result.voltage["vdd"] = circuit.voltage;
    result.voltage["gnd"] = 0;
    long double upstream_voltage = circuit.voltage;
    long double driving_resistance = 0;
    for (std::size_t i = 0; i < count; ++i) {
        const Stage& stage = circuit.stages[i];
        const long double series = total(stage.series);
        const long double shunt = total(stage.shunt);
        const long double resistance = series + load_at[i];
        if (!std::isfinite(resistance)) throw std::runtime_error("stage resistance is outside the supported range");
        const long double feed_current = upstream_voltage / resistance;
        const long double node_voltage = upstream_voltage * (load_at[i] / resistance);
        const long double shunt_current = node_voltage / shunt;
        if (!std::isfinite(feed_current) || !std::isfinite(shunt_current) || !std::isfinite(node_voltage)) {
            throw std::runtime_error("calculation is outside the supported numeric range");
        }
        if (upstream_voltage != 0 && node_voltage == 0) {
            throw std::runtime_error("voltage underflows the supported numeric range");
        }
        // Add drops from the lower end; subtraction can erase tiny internal voltages.
        long double value = node_voltage;
        for (std::size_t j = stage.series.size(); j > 0; --j) {
            const Resistor& resistor = stage.series[j - 1];
            result.current[resistor.name] = feed_current;
            result.voltage[resistor.to] = value;
            value += feed_current * resistor.ohms;
        }
        value = 0;
        for (std::size_t j = stage.shunt.size(); j > 0; --j) {
            const Resistor& resistor = stage.shunt[j - 1];
            result.current[resistor.name] = shunt_current;
            result.voltage[resistor.to] = value;
            value += shunt_current * resistor.ohms;
        }
        upstream_voltage = node_voltage;
        // Short the ideal source, then look from each successive junction to ground.
        driving_resistance = parallel(shunt, series + driving_resistance);
    }
    result.vth = upstream_voltage;
    result.rth = driving_resistance;
    return result;
}

inline std::string engineering(long double value, const std::string& unit = "") {
    static const char* prefixes[] = {"f", "p", "n", "u", "m", "", "K", "Meg", "G", "T", "P"};
    if (value == 0) return "0.000" + unit;
    int index = 5;
    long double magnitude = std::fabs(value);
    while (magnitude < 1 && index > 0) { magnitude *= 1000; --index; }
    while (magnitude >= 1000 && index < 10) { magnitude /= 1000; ++index; }
    if (magnitude < 1 || magnitude >= 1000) {
        // Very deep ladders can attenuate below femto: preserve the nonzero value.
        std::ostringstream out;
        out << std::scientific << std::setprecision(3) << value << unit;
        return out.str();
    }
    if (std::floor(magnitude * 1000 + 0.5L) >= 1000000 && index < 10) {
        magnitude /= 1000;
        ++index;
    }
    std::ostringstream out;
    out << std::fixed << std::setprecision(3) << (value < 0 ? -magnitude : magnitude)
        << prefixes[index] << unit;
    return out.str();
}

inline void row(std::ostream& out, const std::string& left, const std::string& right = "") {
    out << "** " << std::left << std::setw(13) << left << std::setw(20) << right << "**\n";
}

inline void calculation(const Circuit& circuit, std::ostream& out, bool all_nodes = false) {
    if (circuit.stages.empty()) { out << "There is no circuit\n"; return; }
    const Result result = solve(circuit);
    out << "**************************************\n";
    row(out, "<<voltage>>");
    row(out, "vdd", engineering(circuit.voltage, "v"));
    if (all_nodes) {
        for (std::map<std::string, long double>::const_iterator it = result.voltage.begin(); it != result.voltage.end(); ++it) {
            if (it->first != "vdd" && it->first != "gnd") row(out, it->first, engineering(it->second, "v"));
        }
    } else {
        for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
            row(out, circuit.stages[i].node, engineering(result.voltage.at(circuit.stages[i].node), "v"));
        }
    }
    row(out, "<<current>>");
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        const Stage& stage = circuit.stages[i];
        for (std::size_t j = 0; j < stage.series.size(); ++j) row(out, stage.series[j].name, engineering(result.current.at(stage.series[j].name), "A"));
        for (std::size_t j = 0; j < stage.shunt.size(); ++j) row(out, stage.shunt[j].name, engineering(result.current.at(stage.shunt[j].name), "A"));
    }
    row(out, "");
    row(out, "myspice2021");
    out << "**************************************\n";
}

inline void rewire(Circuit& circuit) {
    std::string upstream = "vdd";
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        circuit.stages[i].series[0].from = upstream;
        upstream = circuit.stages[i].node;
    }
}

inline void adjacent(const Circuit& circuit, int& first, int& second) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    if (first < 1 || second < 1 || static_cast<std::size_t>(first) > circuit.stages.size() ||
        static_cast<std::size_t>(second) > circuit.stages.size()) throw std::runtime_error("The node doesn't exist.");
    if (std::abs(first - second) != 1) throw std::runtime_error("The stages must be adjacent.");
    if (first > second) std::swap(first, second);
}

inline void swap_stage(Circuit& circuit, int first, int second, std::ostream* messages = NULL) {
    adjacent(circuit, first, second);
    std::vector<Resistor> before;
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) before.push_back(circuit.stages[i].series[0]);
    std::swap(circuit.stages[first - 1], circuit.stages[second - 1]);
    rewire(circuit);
    if (messages != NULL) {
        std::map<std::string, std::string> after;
        for (std::size_t i = 0; i < circuit.stages.size(); ++i) after[circuit.stages[i].series[0].name] = circuit.stages[i].series[0].from;
        for (std::size_t i = 0; i < before.size(); ++i) {
            if (before[i].from != after[before[i].name]) *messages << before[i].name << " is changing node from " << before[i].from << " to " << after[before[i].name] << " ...\n";
        }
        *messages << "The stage is successfully swapped!\n";
    }
}

inline void merge_stage(Circuit& circuit, int first, int second, std::ostream* messages = NULL) {
    adjacent(circuit, first, second);
    Stage& keep = circuit.stages[first - 1];
    const Stage& remove = circuit.stages[second - 1];
    const long double equivalent = parallel(total(keep.shunt), total(remove.series) + total(remove.shunt));
    std::ostringstream stem;
    stem << "RP" << first;
    std::string name = stem.str();
    std::set<std::string> remaining_names;
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        if (i == static_cast<std::size_t>(first - 1) || i == static_cast<std::size_t>(second - 1)) continue;
        for (std::size_t j = 0; j < circuit.stages[i].shunt.size(); ++j) remaining_names.insert(circuit.stages[i].shunt[j].name);
    }
    unsigned int suffix = 1;
    while (remaining_names.count(name) != 0) {
        std::ostringstream unique;
        unique << stem.str() << "_m" << suffix++;
        name = unique.str();
    }
    const Resistor merged = {name, keep.node, "gnd", equivalent};
    keep.shunt.assign(1, merged);
    circuit.stages.erase(circuit.stages.begin() + second - 1);
    rewire(circuit);
    if (messages != NULL) {
        *messages << "Resistors are merged into " << name << ".\n=> " << name << ' ' << merged.from
            << " gnd " << engineering(equivalent) << "\nThe stage is successfully merged!\n";
    }
}

inline void write_netlist(const Circuit& circuit, std::ostream& out) {
    out << "***circuits***\nstage " << circuit.stages.size() << "\n\n" << circuit.source_name
        << " vdd gnd " << std::fixed << std::setprecision(0) << circuit.voltage << "V\n\n";
    for (std::size_t i = 0; i < circuit.stages.size(); ++i) {
        const Stage& stage = circuit.stages[i];
        for (std::size_t k = 0; k < 2; ++k) {
            const std::vector<Resistor>& path = k == 0 ? stage.series : stage.shunt;
            for (std::size_t j = 0; j < path.size(); ++j) {
                out << path[j].name << ' ' << path[j].from << ' ' << path[j].to << ' ' << engineering(path[j].ohms) << '\n';
            }
        }
    }
    out << "end\n\n";
}

inline void raw_result(const Circuit& circuit, std::ostream& out) {
    const Result result = solve(circuit);
    out << std::scientific << std::setprecision(std::numeric_limits<long double>::max_digits10);
    out << "input_resistance " << result.input_resistance << "\nvth " << result.vth << "\nrth " << result.rth << '\n';
    for (std::map<std::string, long double>::const_iterator it = result.voltage.begin(); it != result.voltage.end(); ++it) out << "voltage " << it->first << ' ' << it->second << '\n';
    for (std::map<std::string, long double>::const_iterator it = result.current.begin(); it != result.current.end(); ++it) out << "current " << it->first << ' ' << it->second << '\n';
}

inline void thevenin(const Circuit& circuit, std::ostream& out, bool legacy, bool raw) {
    const Result result = solve(circuit);
    const long double voltage = legacy ? circuit.voltage : result.vth;
    const long double resistance = legacy ? result.input_resistance : result.rth;
    if (raw) {
        out << std::scientific << std::setprecision(std::numeric_limits<long double>::max_digits10)
            << "vth " << voltage << "\nrth " << resistance << '\n';
    } else {
        out << "***Thevenin equivalence circuit***\nRth = " << engineering(resistance)
            << "\nVth = " << engineering(voltage, "v") << '\n';
    }
}

inline void save(const Circuit& circuit, const std::string& filename, bool is_thevenin,
                 bool legacy, bool raw, bool all_nodes) {
    if (circuit.stages.empty()) throw std::runtime_error("There is no circuit");
    // Validate all calculations before opening/truncating the requested output.
    solve(circuit);
    std::ofstream output(filename.c_str());
    if (!output) throw std::runtime_error("cannot open output file: " + filename);
    if (is_thevenin) thevenin(circuit, output, legacy, raw);
    else if (raw) raw_result(circuit, output);
    else {
        write_netlist(circuit, output);
        output << "***calculation results***\n";
        calculation(circuit, output, all_nodes);
    }
    if (!output) throw std::runtime_error("cannot write output file: " + filename);
}

inline void menu(std::ostream& out, bool bonus) {
    out << "==========================================\n"
        << "Please type 1 to input file\nPlease type 2 to calculate voltage and current\n"
        << "Please type 3 to swap stage\nPlease type 4 to merge stage\n"
        << "Please type 5 to output file\nPlease type 6 to exit\n";
    if (bonus) out << "Please type 7 to output Thevenin equivalence circuit\n";
    out << "Enter your selection Here: ";
}

inline bool prompt(std::istream& in, std::ostream& out, const std::string& text, std::string& reply) {
    out << text;
    if (!std::getline(in, reply)) return false;
    reply = trim(reply);
    return true;
}

inline bool stage_prompt(std::istream& in, std::ostream& out, const Circuit& circuit,
                         const std::string& verb, bool first, int other, int& answer) {
    std::string line;
    const std::string which = first ? "first" : "second";
    while (prompt(in, out, "Please enter the " + which + " stage you want to " + verb + ": ", line)) {
        try {
            answer = integer(line);
            if (answer < 1 || static_cast<std::size_t>(answer) > circuit.stages.size()) {
                out << "The node doesn't exist. Please enter again.\n";
            } else if (!first && std::abs(answer - other) != 1) {
                out << "The stages must be adjacent. Please enter again.\n";
            } else return true;
        } catch (const std::exception&) {
            out << "The node doesn't exist. Please enter again.\n";
        }
    }
    return false;
}

inline int interactive(bool bonus, bool legacy, std::istream& in, std::ostream& out) {
    Circuit circuit;
    std::string line;
    out << "*** Welcome to MySpice ***\n";
    for (;;) {
        menu(out, bonus);
        if (!std::getline(in, line)) return 0;
        int selection = 0;
        try { selection = integer(trim(line)); }
        catch (const std::exception&) { out << "Invalid selection.\n"; continue; }
        out << "==========================================\n";
        if (selection == 6) { out << "Bye~\n"; return 0; }
        if (selection == 1) {
            bool again = false;
            for (;;) {
                if (!prompt(in, out, again ? "Please enter the name of the input file again: " :
                    "Please enter the name of the input file: ", line)) return 0;
                std::ifstream file(line.c_str());
                if (!file) { out << "The file is not found.\n"; again = true; continue; }
                out << "Loading the file...\n";
                try {
                    Circuit loaded = read_circuit(file);
                    circuit = loaded;
                    out << "The input file successfully loaded!\n";
                    break;
                } catch (const std::exception& error) {
                    out << "Invalid circuit: " << error.what() << '\n';
                    again = true;
                }
            }
        } else if ((selection >= 2 && selection <= 5) || (bonus && selection == 7)) {
            if (circuit.stages.empty()) { out << "There is no circuit\n"; continue; }
            if (selection == 2) {
                out << "Calculating...\nThe result is successfully calculated!\n";
                calculation(circuit, out);
            } else if (selection == 3 || selection == 4) {
                if (circuit.stages.size() < 2) { out << "At least two stages are required.\n"; continue; }
                int first = 0, second = 0;
                const std::string verb = selection == 3 ? "swap" : "merge";
                if (!stage_prompt(in, out, circuit, verb, true, 0, first) ||
                    !stage_prompt(in, out, circuit, verb, false, first, second)) return 0;
                if (selection == 3) swap_stage(circuit, first, second, &out);
                else merge_stage(circuit, first, second, &out);
            } else {
                if (!prompt(in, out, "Please enter the name of the output file: ", line)) return 0;
                out << "Exporting the file...\n";
                try {
                    save(circuit, line, selection == 7, legacy, false, false);
                    out << (selection == 7 ? "The bonus file successfully exported!\n" : "The output file successfully exported!\n");
                } catch (const std::exception& error) { out << error.what() << '\n'; }
            }
        } else out << "Invalid selection.\n";
    }
}

inline int run(int argc, char* argv[], bool bonus) {
    try {
        if (argc == 1) return interactive(bonus, false, std::cin, std::cout);
        if (argc == 2 && bonus && std::string(argv[1]) == "--legacy-thevenin") {
            return interactive(bonus, true, std::cin, std::cout);
        }
        if (argc < 3) throw std::runtime_error("usage: program input.txt output.txt [--swap i j] [--merge i j] [--raw] [--all-nodes] [--circuit] [--thevenin] [--legacy-thevenin]");
        Circuit circuit = load(argv[1]);
        bool is_thevenin = bonus, legacy = false, raw = false, all_nodes = false;
        for (int i = 3; i < argc; ++i) {
            const std::string option(argv[i]);
            if (option == "--swap" || option == "--merge") {
                if (i + 2 >= argc) throw std::runtime_error("a stage operation requires two positions");
                const int first = integer(argv[++i]);
                const int second = integer(argv[++i]);
                if (option == "--swap") swap_stage(circuit, first, second);
                else merge_stage(circuit, first, second);
            } else if (option == "--raw") raw = true;
            else if (option == "--all-nodes") all_nodes = true;
            else if (option == "--circuit") is_thevenin = false;
            else if (option == "--thevenin" && bonus) is_thevenin = true;
            else if (option == "--legacy-thevenin" && bonus) { legacy = true; is_thevenin = true; }
            else throw std::runtime_error("unknown option: " + option);
        }
        save(circuit, argv[2], is_thevenin, legacy, raw, all_nodes);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

}  // namespace myspice
#endif
