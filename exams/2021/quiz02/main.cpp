#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz02 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        std::cout << "INPUT a, b\nWHILE b is not zero\n"
                     "    replace (a, b) with (b, a modulo b)\nOUTPUT a\n";
    } else if (mode == "3") {
        int answer;
        if (!(std::cin >> answer)) return 1;
        std::cout << "A: ";
        if (answer == 7) std::cout << "correct\n";
        else std::cout << "incorrect\n";
        std::cout << "B: ill-formed in C++11\nC: "
                  << (answer == 7 ? "correct" : "incorrect") << "\nD: ";
        answer == 7 ? std::cout << "correct" : std::cout << "incorrect";
        std::cout << '\n';
    } else if (mode == "4") {
        int x = 3;
        x += 4;
        std::cout << "B: " << x << '\n';
        x = 3;
        x = +4;
        std::cout << "C: " << x << "\nA and D: ill-formed\n";
    } else if (mode == "5") {
        long long a, b;
        if (!(std::cin >> a >> b) || a < 0 || b < 0 ||
            a > 2147483647LL || b > 2147483647LL) return 1;
        while (b != 0) {
            const long long t = b;
            b = a % b;
            a = t;
        }
        std::cout << a << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
