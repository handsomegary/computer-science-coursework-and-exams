#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

struct Plain {
    int first;
    int second;
};

struct Defaulted {
    int first = 1;
    int second = 2;
};

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: quiz06 2|3|4|5 [input-file]\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode != "3" && argc != 2) return 2;
    if (mode == "2") {
        std::cout << "iostream: standard input/output\nfstream: file streams\n";
    } else if (mode == "3") {
        if (argc != 3) return 1;
        std::ifstream input(argv[2], std::ios::binary);
        if (!input) return 1;
        input.seekg(0, std::ios::end);
        if (input.tellg() != std::streampos(10)) return 1;
        input.seekg(1, std::ios::beg);
        input.seekg(1, std::ios::cur);
        std::cout << "A: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(8, std::ios::end);
        std::cout << "B: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(2, std::ios::beg);
        std::cout << "C: " << input.tellg() << '\n';
        input.seekg(1, std::ios::beg);
        input.seekg(2);
        std::cout << "D: " << input.tellg() << '\n';
    } else if (mode == "4") {
        std::string token;
        if (!(std::cin >> token)) return 1;
        if (token.size() > 32) return 1;
        int digits = 0, points = 0;
        for (std::size_t i = 0; i < token.size(); ++i) {
            if (i == 0 && (token[i] == '+' || token[i] == '-')) continue;
            if (token[i] == '.') ++points;
            else if (token[i] >= '0' && token[i] <= '9') ++digits;
            else return 1;
        }
        if (digits == 0 || points > 1) return 1;
        std::istringstream parser(token);
        double value;
        if (!(parser >> value) || !parser.eof() || !std::isfinite(value) ||
            std::fabs(value) > 1000000) return 1;
        std::cout << "atof (double): " << std::atof(token.c_str())
                  << "\natoi (int): " << std::atoi(token.c_str())
                  << "\natol (long): " << std::atol(token.c_str()) << '\n';
    } else if (mode == "5") {
        Plain a;
        a.first = 1;
        a.second = 2;
        const Plain b = {1, 2};
        const Defaulted c;
        const Plain d = b;
        std::cout << "A: " << a.first << ' ' << a.second
                  << "\nB: " << b.first << ' ' << b.second
                  << "\nC (C++11): " << c.first << ' ' << c.second
                  << "\nD: " << d.first << ' ' << d.second << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
