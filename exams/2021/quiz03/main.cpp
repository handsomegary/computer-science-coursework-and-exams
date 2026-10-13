#include <iomanip>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz03 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        int floatingCount = 0;
        for (double value = 0; value < 1.0; value += 0.1) ++floatingCount;
        int integerCount = 0;
        for (int i = 0; i < 10; ++i) ++integerCount;
        std::cout << "floating iterations: " << floatingCount
                  << "\ninteger iterations: " << integerCount << '\n';
    } else if (mode == "3") {
        int counter = 1;
        do {
            std::cout << counter << ' ';
        } while (counter++ < 10);
        std::cout << '\n';
    } else if (mode == "4") {
        std::cout << "C: ";
        int i = 0;
        for (; i < 10; ++i) std::cout << i << ' ';
        std::cout << "\nD: ";
        for (int j = 0; j < 10; ++j) std::cout << j << ' ';
        std::cout << '\n';
    } else if (mode == "5") {
        std::cout << '[' << std::setw(5) << 7 << "][" << 8 << "]\n";
        std::cout << std::fixed << std::setprecision(2) << 1.25 << ' '
                  << 2.5 << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
