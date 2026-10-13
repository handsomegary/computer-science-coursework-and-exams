#include <cctype>
#include <iostream>
#include <string>

bool identifier(const std::string& value) {
    if (value.empty()) return false;
    for (std::size_t i = 0; i < value.size(); ++i) {
        const unsigned char c = static_cast<unsigned char>(value[i]);
        if (!(std::isalpha(c) || c == '_' || (i != 0 && std::isdigit(c))))
            return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz01 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        const std::string choices[] = {"my*Value", "_AAA1", "widthee", "m_x"};
        for (int i = 0; i < 4; ++i)
            std::cout << static_cast<char>('A' + i) << ": "
                      << (identifier(choices[i]) ? "valid" : "invalid") << '\n';
    } else if (mode == "3") {
        int number1 = 1, number2 = 2;
        number1 = number2;
        std::cout << "A: number2=" << number2 << '\n';
        int sum = number1 + number2;
        std::cout << "B: sum=" << sum << "; number2=" << number2 << '\n';
        if (!(std::cin >> number2)) return 1;
        std::cout << "C: number2=" << number2 << '\n';
        number2 = 2;
        std::cout << "D: " << number2 << "; number2=" << number2 << '\n';
    } else if (mode == "4") {
        int a = 4, b = 12, c = 37, d = 51;
        if (a < b) std::cout << "a < b\n";
        if (a > b) std::cout << "a > b\n";
        if (d <= c) std::cout << "d <= c\n";
        if (c != d) std::cout << "c != d\n";
    } else if (mode == "5") {
        int first, second;
        if (!(std::cin >> first >> second)) return 1;
        int number1 = first, number2 = second;
        std::cout << "Original:\n";
        if (number1 > number2)
            std::cout << number1 << " is larger than " << number2 << '\n';
        std::cout << number1 << " is bigger than " << number2 << '\n';
        if ((number1 = number2))
            std::cout << number1 << " is equal to " << number2 << '\n';
        number1 = first;
        std::cout << "Corrected:\n";
        if (number1 > number2)
            std::cout << number1 << " is larger than " << number2 << '\n';
        std::cout << number1 << " is bigger than " << number2 << '\n';
        if (number1 == number2)
            std::cout << number1 << " is equal to " << number2 << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
