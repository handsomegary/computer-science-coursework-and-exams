#include "logic.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>

namespace {
std::string trim(const std::string& s) {
    const std::size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
std::string uncomment(const std::string& s) { return s.substr(0, s.find("//")); }
std::vector<std::string> header(const std::string& text, const std::string& label) {
    const std::string s = trim(uncomment(text));
    const std::size_t eq = s.find('='), semi = s.find(';');
    if (eq == std::string::npos || semi == std::string::npos || semi < eq ||
        trim(s.substr(0, eq)) != label || !trim(s.substr(semi + 1)).empty())
        throw std::runtime_error("invalid " + label + " header");
    std::istringstream in(s.substr(eq + 1, semi - eq - 1));
    std::vector<std::string> names;
    std::string name;
    while (in >> name) names.push_back(name);
    return names;
}
class Parser {
    std::string text;
    std::size_t pos;
    const std::vector<std::string>& names;
    void space() { while (pos < text.size() && std::isspace(static_cast<unsigned char>(text[pos]))) ++pos; }
    bool take(char c) { space(); if (pos < text.size() && text[pos] == c) { ++pos; return true; } return false; }
    std::vector<logic::Cube> atom() {
        if (take('(')) {
            std::vector<logic::Cube> r = sum();
            if (!take(')')) throw std::runtime_error("missing ')' in expression");
            return r;
        }
        const bool neg = take('!');
        space();
        if (pos >= text.size()) throw std::runtime_error("missing literal");
        const char c = text[pos++];
        if (c == '0' || c == '1') {
            const bool value = (c == '1') != neg;
            return value ? std::vector<logic::Cube>(1, logic::Cube()) : std::vector<logic::Cube>();
        }
        const auto it = std::find(names.begin(), names.end(), std::string(1, c));
        if (it == names.end()) throw std::runtime_error("unknown variable or invalid literal");
        const std::uint32_t bit = std::uint32_t(1) << (names.size() - 1 - (it - names.begin()));
        return std::vector<logic::Cube>(1, logic::Cube(neg ? 0 : bit, bit));
    }
    std::vector<logic::Cube> product() {
        std::vector<logic::Cube> result = atom();
        while (take('*')) {
            const std::vector<logic::Cube> right = atom();
            std::set<logic::Cube> combined;
            for (const logic::Cube& a : result) for (const logic::Cube& b : right) {
                if (!((a.bits ^ b.bits) & a.mask & b.mask))
                    combined.insert(logic::Cube(a.bits | b.bits, a.mask | b.mask));
            }
            result.assign(combined.begin(), combined.end());
        }
        return result;
    }
    std::vector<logic::Cube> sum() {
        std::vector<logic::Cube> result = product();
        while (take('+')) {
            const std::vector<logic::Cube> right = product();
            result.insert(result.end(), right.begin(), right.end());
        }
        return result;
    }
public:
    Parser(const std::string& s, const std::vector<std::string>& ns) : text(s), pos(0), names(ns) {}
    std::vector<logic::Cube> run() {
        std::vector<logic::Cube> r = sum();
        take(';'); space();
        if (pos != text.size()) throw std::runtime_error("unexpected expression suffix");
        std::sort(r.begin(), r.end()); r.erase(std::unique(r.begin(), r.end()), r.end());
        return r;
    }
};
void solve(const std::string& input, const std::string& output) {
    std::ifstream in(input.c_str());
    if (!in) throw std::runtime_error("cannot open input file");
    std::string h1, h2;
    if (!std::getline(in, h1) || !std::getline(in, h2)) throw std::runtime_error("missing headers");
    if (!h1.empty() && h1.back() == '\r') h1.pop_back();
    if (!h2.empty() && h2.back() == '\r') h2.pop_back();
    const std::vector<std::string> vars = header(h1, "INORDER");
    const std::vector<std::string> outs = header(h2, "OUTORDER");
    if (vars.empty() || vars.size() > 26 || outs.size() != 1)
        throw std::runtime_error("requires 1..26 input variables and one output");
    std::set<std::string> unique;
    for (const std::string& v : vars) {
        if (v.size() != 1 || v[0] < 'a' || v[0] > 'z' || !unique.insert(v).second)
            throw std::runtime_error("inputs must be distinct lowercase letters");
    }
    if (outs[0].empty() || !(std::isalpha(static_cast<unsigned char>(outs[0][0])) || outs[0][0] == '_'))
        throw std::runtime_error("invalid output identifier");
    for (char c : outs[0]) if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_'))
        throw std::runtime_error("invalid output identifier");
    std::string expression, line;
    while (std::getline(in, line)) expression += uncomment(line) + " ";
    const std::size_t eq = expression.find('=');
    if (eq == std::string::npos || trim(expression.substr(0, eq)) != outs[0])
        throw std::runtime_error("output assignment does not match OUTORDER");
    const std::vector<logic::Cube> cubes = Parser(expression.substr(eq + 1), vars).run();
    const std::uint32_t full = (std::uint32_t(1) << vars.size()) - 1;
    std::vector<std::uint32_t> on;
    bool constant_one = false;
    for (const logic::Cube& c : cubes) if (c.mask == 0) constant_one = true;
    std::vector<logic::Cube> answer;
    if (constant_one) answer.push_back(logic::Cube());
    else {
        for (const logic::Cube& c : cubes) {
            if (c.mask != full && vars.size() > 16)
                throw std::runtime_error("general SOP extension is limited to 16 variables; use full minterms");
            const std::uint32_t free = full & ~c.mask;
            std::uint32_t part = free;
            do { on.push_back(c.bits | part); if (part == 0) break; part = (part - 1) & free; } while (true);
        }
        answer = logic::minimize(static_cast<unsigned>(vars.size()), on);
    }
    std::ofstream out(output.c_str());
    if (!out) throw std::runtime_error("cannot open output file");
    out << h1 << '\n' << h2 << '\n' << outs[0] << " = " << logic::sop(answer, vars) << '\n';
    if (!out) throw std::runtime_error("failed writing output file");
}
}
int main(int argc, char* argv[]) {
    try {
        std::string input, output;
        if (argc == 1) {
            std::cout << "Input .eqn file: ";
            if (!std::getline(std::cin, input)) throw std::runtime_error("missing input filename");
            std::cout << "Output .eqn file: ";
            if (!std::getline(std::cin, output)) throw std::runtime_error("missing output filename");
        } else if (argc == 3) { input = argv[1]; output = argv[2]; }
        else throw std::runtime_error("usage: logic_min input.eqn output.eqn");
        if (input == output) throw std::runtime_error("input and output filenames must differ");
        solve(input, output);
        return 0;
    } catch (const std::exception& e) { std::cerr << "Error: " << e.what() << '\n'; return 1; }
}
