#include <iostream>
#include <string>

int mystery(int number) {
    return number <= 1 ? 1 : number * mystery(number - 1);
}

template<class T>
T maximum(T value1, T value2) {
    return value1 > value2 ? value1 : value2;
}

int sumPair(int a, int b) {
    return a + b;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: quiz04 2|3|4|5\n";
        return 2;
    }
    const std::string mode = argv[1];
    if (mode == "2") {
        {
            enum person {me = 1, you = 2, them = 3};
            std::cout << "A: " << me << ' ' << you << ' ' << them << '\n';
        }
        {
            enum person {me, you, them};
            std::cout << "B: " << me << ' ' << you << ' ' << them << '\n';
        }
        {
            enum person {me = 0, you = 0, them = 0};
            std::cout << "C: " << me << ' ' << you << ' ' << them << '\n';
        }
        std::cout << "D: ill-formed (duplicate enumerator name)\n";
    } else if (mode == "3") {
        int number;
        if (!(std::cin >> number) || number < 0 || number > 12) return 1;
        std::cout << mystery(number) << '\n';
    } else if (mode == "4") {
        std::cout << "int maximum: " << maximum(2, 5)
                  << "\ndouble maximum: " << maximum(2.3, 5.2) << '\n';
    } else if (mode == "5") {
        int x, y;
        if (!(std::cin >> x >> y) || x < -1000000000 || x > 1000000000 ||
            y < -1000000000 || y > 1000000000) return 1;
        std::cout << sumPair(x, y) << '\n';
    } else {
        std::cerr << "Unknown question\n";
        return 2;
    }
    return 0;
}
