#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <iomanip>
#include <limits>

void by_value(int value) { ++value; }
void by_reference(int& value) { ++value; }
void by_pointer(int* value) { if (value != nullptr) ++*value; }
class Sum {
    int first_, second_;
public:
    Sum(int first, int second) : first_(first), second_(second) {}
    long long value() const { return static_cast<long long>(first_) + second_; }
};
int sign(int value) { return (value > 0) - (value < 0); }
int main(int argc, char* argv[]) {
    if (argc != 2) { std::cerr << "Usage: final MODE\n"; return 1; }
    const std::string mode = argv[1];
    if (mode == "passing") {
        int value = 5;
        by_value(value); std::cout << value << '\n';
        by_reference(value); std::cout << value << '\n';
        by_pointer(&value); std::cout << value << '\n';
    } else if (mode == "pointers") {
        char blocks[3] = {'A', 'B', 'C'};
        char* ptr = blocks;
        std::cout << blocks[0] << '\n' << *(blocks + 2) << '\n' << *(ptr + 1) << '\n' << *ptr << '\n';
        ptr = blocks + 1;
        std::cout << *ptr << '\n' << *(ptr + 1) << '\n';
        ptr = blocks;
        char temp = *++ptr; std::cout << temp << '\n';
        temp = ++*ptr; std::cout << temp << '\n';
        temp = *ptr++; std::cout << temp << '\n';
        temp = *ptr; std::cout << temp << '\n';
        temp = static_cast<char>(*blocks + 1); std::cout << temp << '\n';
    } else if (mode == "array3d") {
        int array_test[2][3][4] = {3,4,2,3,0,-3,9,11,23,12,23,2,13,4,56,3,5,9,3,5,5,1,4,9};
        long long sum = 0;
        int* arrayPtr = nullptr;
        for (int plane = 0; plane < 2; ++plane) for (int row = 0; row < 3; ++row) {
            arrayPtr = *(*(array_test + plane) + row);
            for (int col = 0; col < 4; ++col) sum += *(arrayPtr + col);
        }
        std::cout << sum << '\n';
    } else if (mode == "tokens") {
        std::string line;
        if (!std::getline(std::cin, line)) return 1;
        if (line.find('\0') != std::string::npos) return 1;
        std::vector<char> buffer(line.begin(), line.end());
        buffer.push_back('\0');
        for (char* token = std::strtok(buffer.data(), " ,.-;!?"); token != nullptr; token = std::strtok(nullptr, " ,.-;!?"))
            std::cout << token << '\n';
    } else if (mode == "class") {
        int first, second;
        if (!(std::cin >> first >> second)) return 1;
        std::cout << Sum(first, second).value() << '\n';
    } else if (mode == "length") {
        int values[] = {1,2,3,4,5};
        std::cout << sizeof(values) / sizeof(values[0]) << '\n';
    } else if (mode == "compare") {
        const char* a = "National Central University";
        const char* b = "National Tsing Hua University";
        std::cout << sign(std::strcmp(a,b)) << '\n' << sign(std::strncmp(a,b,8)) << '\n' << sign(std::strncmp(b,a,14)) << '\n';
    } else if (mode == "selection") {
        int values[] = {7,3,5,1};
        for (int i = 0; i < 3; ++i) {
            int smallest = i;
            for (int j = i + 1; j < 4; ++j) if (values[j] < values[smallest]) smallest = j;
            const int saved = values[i]; values[i] = values[smallest]; values[smallest] = saved;
            for (int j = 0; j < 4; ++j) std::cout << (j == 0 ? "" : " ") << values[j];
            std::cout << '\n';
        }
    } else if (mode == "fill") {
        std::cout << std::setfill('0') << std::setw(4) << 7 << ' ' << std::setw(4) << 8 << '\n';
    } else if (mode == "matrix") {
        int b[2][2] = {{1}, {3,4}};
        std::cout << b[1][0] << '\n';
    } else if (mode == "member") {
        struct Point { int x_center; int y_center; };
        Point point = {3,4};
        Point* pt = &point;
        std::cout << pt->x_center << ' ' << (*pt).x_center << '\n';
    } else if (mode == "constptr") {
        int x = 7;
        int* const myPtr = &x;
        *myPtr = 9;
        std::cout << x << '\n';
    } else if (mode == "file") {
        const char* filename = "integers.txt";
        { std::ofstream output(filename); if (!output) return 1; output << 123 << ' ' << 456; if (!output) return 1; }
        std::ifstream input(filename);
        char character;
        int integer;
        for (int i = 0; i < 2; ++i) {
            if (!(input >> character >> integer)) return 1;
            std::cout << "character:" << character << "\ninteger:" << integer << '\n';
        }
    } else { std::cerr << "Unknown mode\n"; return 1; }
    return 0;
}
