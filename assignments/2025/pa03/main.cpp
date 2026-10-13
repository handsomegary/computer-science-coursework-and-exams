#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>
#include "big_number.h"

bool bonus = false, plain = false;
bool next_line(std::string& line) { return static_cast<bool>(std::getline(std::cin, line)); }
void invalid() { std::cout << "Invalid input! Please try again.\n"; }
bool read_integer(const std::string& prompt, int low, int high, bool odd, int& value) {
    std::string line;
    while (true) {
        if (!plain) std::cout << prompt;
        if (!next_line(line)) return false;
        std::istringstream input(line); std::string extra;
        if ((input >> value) && !(input >> extra) && value >= low && value <= high && (!odd || value % 2 == 1)) return true;
        invalid();
    }
}
bool valid_number(const std::string& number, int base) {
    std::size_t start = !number.empty() && number[0] == '-' ? 1 : 0;
    if (number.size() <= start || number.size() - start > 100 || (start && !bonus)) return false;
    for (std::size_t i = start; i < number.size(); ++i) {
        char c = number[i]; int digit = c >= '0' && c <= '9' ? c - '0' : (c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1);
        if (digit < 0 || digit >= base) return false;
    }
    return true;
}
bool read_matrix(int size, int base, const char* name, std::vector<BigNumber>& matrix) {
    if (!plain) std::cout << "Now enter the " << size << 'x' << size << ' ' << name << " matrix (base-" << base << "), rows one by one:\n";
    for (int r = 0; r < size; ++r) {
        while (true) {
            if (!plain) std::cout << "Row " << r + 1 << " (enter " << size << " values separated by spaces, then press Enter):\n";
            std::string line; if (!next_line(line)) return false;
            std::istringstream input(line); std::vector<std::string> tokens; std::string token;
            bool correct = true;
            while (input >> token) { tokens.push_back(token); if (!valid_number(token, base)) correct = false; }
            if (!correct || static_cast<int>(tokens.size()) != size) { invalid(); continue; }
            for (const std::string& text : tokens) matrix.push_back(BigNumber(text, base));
            break;
        }
    }
    return true;
}
int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string option = argv[i];
        if (option == "--bonus") bonus = true;
        else if (option == "--plain") plain = true;
        else { std::cerr << "Unknown option\n"; return 1; }
    }
    if (!plain) std::cout << "---- Big Number Energy Distribution Calculator ----\n---------------------------------------------------\n";
    int base, n, d;
    if (!read_integer("Enter the base (2 <= k <= 16): base-k = ", 2, 16, false, base) || !read_integer("Enter N (size of energy matrix, N x N): ", 1, 100, false, n)) return 1;
    std::vector<BigNumber> energy, weights;
    if (!read_matrix(n, base, "energy", energy)) return 1;
    if (!plain) std::cout << '\n';
    if (!read_integer("Enter D (size of weight matrix, D x D): ", 1, 99, true, d) || !read_matrix(d, base, "weight", weights)) return 1;
    std::vector<BigNumber> output(n * n, BigNumber(base));
    int pad = d / 2;
    for (int r = 0; r < n; ++r) for (int c = 0; c < n; ++c)
        for (int i = 0; i < d; ++i) for (int j = 0; j < d; ++j) {
            int rr = r + i - pad, cc = c + j - pad;
            if (rr >= 0 && rr < n && cc >= 0 && cc < n) output[r * n + c] = output[r * n + c] + energy[rr * n + cc] * weights[i * d + j];
        }
    int width = 0;
    for (const BigNumber& value : output) width = std::max(width, static_cast<int>(value.str().size()) + 2);
    if (!plain) std::cout << "\n--- Computation complete: Energy distribution result (" << n << 'x' << n << ", base-" << base << ") ---\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (c == n - 1) std::cout << output[r * n + c].str();
            else std::cout << std::left << std::setw(width) << output[r * n + c].str();
        }
        std::cout << '\n';
    }
    if (bonus) {
        BigNumber sum(base); for (const BigNumber& value : output) sum = sum + value;
        if (!plain) std::cout << "\nGlobal average:\n";
        std::cout << sum.rounded_average(n * n).str() << '\n';
    }
    if (!plain) std::cout << "\nProgram finished.\n";
}
