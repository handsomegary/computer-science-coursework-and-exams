#include <iostream>
#include <string>
#include <vector>

bool readArray(std::vector<int>& values) {
    int count;
    if (!(std::cin >> count) || count < 1 || count > 20) return false;
    values.resize(static_cast<std::size_t>(count));
    for (int i = 0; i < count; ++i)
        if (!(std::cin >> values[i]) || values[i] < -1000000 || values[i] > 1000000)
            return false;
    return true;
}

void printArray(const std::vector<int>& values) {
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0) std::cout << ' ';
        std::cout << values[i];
    }
    std::cout << '\n';
}

int arraySum(const int values[], std::size_t count) {
    int sum = 0;
    for (std::size_t i = 0; i < count; ++i) sum += values[i];
    return sum;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz05 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        const int b[5] = {7};
        const int c[] = {0, 7, 0, 3, 8, 2};
        const int d[5] = {9, 1, 9};
        std::cout << "A: ill-formed (six initializers for five elements)\nB:";
        for (int value : b) std::cout << ' ' << value;
        std::cout << "\nC:";
        for (int value : c) std::cout << ' ' << value;
        std::cout << "\nD:";
        for (int value : d) std::cout << ' ' << value;
        std::cout << '\n';
    } else if (mode == "3") {
        std::vector<int> values;
        if (!readArray(values)) return 1;
        for (std::size_t i = 1; i < values.size(); ++i) {
            const int key = values[i];
            std::size_t j = i;
            while (j > 0 && values[j - 1] > key) {
                values[j] = values[j - 1];
                --j;
            }
            values[j] = key;
            std::cout << "prefix " << i + 1 << ": ";
            printArray(values);
        }
        std::cout << "sorted: ";
        printArray(values);
    } else if (mode == "4") {
        int rows, columns;
        if (!(std::cin >> rows >> columns) || rows < 1 || rows > 10 ||
            columns < 1 || columns > 10) return 1;
        int zeros = 0;
        for (int i = 0; i < rows * columns; ++i) {
            int value;
            if (!(std::cin >> value)) return 1;
            if (value == 0) ++zeros;
        }
        const int original = zeros * 2;
        std::cout << "Original: "
                  << (original >= rows * columns / 2 ? "sparse" : "not sparse")
                  << "; reported zeros: " << original << '\n';
        std::cout << "Corrected: "
                  << (zeros > rows * columns - zeros ? "sparse" : "not sparse")
                  << "; zeros: " << zeros << '\n';
    } else if (mode == "5") {
        std::vector<int> values;
        if (!readArray(values)) return 1;
        std::cout << "explicit size: " << values.size()
                  << "\nsum: " << arraySum(values.data(), values.size()) << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
