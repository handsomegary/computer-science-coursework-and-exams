#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>

// Reconstructed teaching drivers: select the exercise with a command-line argument.
int area(int n) { return n == 1 ? 1024 : area(n - 1) / 2; }
int twice(int parameter) { return parameter * 2; }
void trace_area(int n, int depth) {
    std::cout << std::string(static_cast<std::size_t>(depth), ' ') << "enter area(" << n << ")\n";
    if (n > 1) trace_area(n - 1, depth + 2);
    std::cout << std::string(static_cast<std::size_t>(depth), ' ') << "return " << area(n) << '\n';
}
bool parse_number(const std::string& token, long long& value) {
    std::istringstream parser(token);
    char extra;
    return (parser >> value) && !(parser >> extra);
}
int main(int argc, char* argv[]) {
    if (argc != 2) { std::cerr << "Usage: midterm MODE\n"; return 1; }
    const std::string mode = argv[1];
    if (mode == "area") {
        for (int n = 1; n <= 10; ++n) std::cout << n << ' ' << area(n) << '\n';
    } else if (mode == "stack") {
        trace_area(4, 0);
    } else if (mode == "parameter") {
        int argument = 7;
        std::cout << twice(argument) << '\n';
    } else if (mode == "pairs") {
        int count;
        if (!(std::cin >> count) || count < 0 || count > 10000) return 1;
        std::vector<int> values(static_cast<std::size_t>(count));
        for (int& value : values) if (!(std::cin >> value)) return 1;
        bool found = false;
        for (int i = 0; i < count; ++i) for (int j = i + 1; j < count; ++j) {
            if (static_cast<long long>(values[static_cast<std::size_t>(i)]) + values[static_cast<std::size_t>(j)] == 12) {
                std::cout << i << ' ' << j << ": " << values[static_cast<std::size_t>(i)] << " + " << values[static_cast<std::size_t>(j)] << " = 12\n";
                found = true;
            }
        }
        if (!found) std::cout << "No pair\n";
    } else if (mode == "factor") {
        long long number = 0;
        std::string token;
        bool first = true;
        do {
            std::cout << (first ? "Enter a number: " : "Please enter a correct number: ");
            first = false;
            if (!(std::cin >> token)) return 1;
            if (!parse_number(token, number)) number = 0;
        } while (number < 2 || number > std::numeric_limits<int>::max());
        const long long original = number;
        std::vector<long long> factors;
        for (long long divisor = 2; divisor <= number / divisor; ++divisor) {
            while (number % divisor == 0) { factors.push_back(divisor); number /= divisor; }
        }
        if (number > 1) factors.push_back(number);
        std::cout << original << " = ";
        for (std::size_t i = factors.size(); i > 0; --i) {
            if (i != factors.size()) std::cout << " x ";
            std::cout << factors[i - 1];
        }
        std::cout << '\n';
    } else if (mode == "conversion") {
        int x = 21;
        double y = 6, z = 14;
        y = x / z;
        x = static_cast<int>(5.5 * y);
        std::cout << x << '\n';
    } else if (mode == "scope") {
        int value = 7;
        { int value = 3; std::cout << value << '\n'; }
        std::cout << value << '\n';
    } else if (mode == "precedence") {
        std::cout << 20 / 5 * 2 + 9 % 4 - 3 << '\n';
    } else if (mode == "array") {
        int values[] = {1, 2, 3};
        int* pointer = values;
        *(pointer + 1) = 8;
        std::cout << values[1] << '\n';
    } else if (mode == "counter") {
        int counter = 0;
        do { std::cout << counter << ' '; } while (((counter++) * 2) <= 10);
        std::cout << '\n';
    } else if (mode == "letters") {
        char c = 'A';
        while (c++ <= 'L') {
            if (c % 4 == 0) continue;
            std::cout << c << " = " << static_cast<int>(c) << '\n';
        }
    } else if (mode == "switch") {
        int choice;
        if (!(std::cin >> choice)) return 1;
        switch (choice) {
            case 1: std::cout << "one\n";
                // fall through
            case 2: std::cout << "two\n"; break;
            default: std::cout << "other\n"; break;
        }
    } else if (mode == "width") {
        std::cout << std::setfill('.') << std::setw(5) << 7 << '|' << 8 << '|' << std::setw(5) << 9 << '\n';
    } else if (mode == "grades") {
        std::string first, second, third;
        long long a, b, c;
        if (!(std::cin >> first >> second >> third) || !parse_number(first, a) ||
            !parse_number(second, b) || !parse_number(third, c)) return 1;
        if (a < 0 || a > 100 || b < 0 || b > 100 || c < 0 || c > 100) return 1;
        const double average = (static_cast<double>(a) + b + c) / 3;
        std::cout << std::fixed << std::setprecision(3) << average << '\n' << (average >= 60 ? "PASS" : "FAIL") << '\n';
    } else { std::cerr << "Unknown mode\n"; return 1; }
    return 0;
}
