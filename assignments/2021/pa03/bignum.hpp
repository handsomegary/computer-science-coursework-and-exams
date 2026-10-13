#ifndef PA03_BIGNUM_HPP
#define PA03_BIGNUM_HPP

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace pa03 {

// Canonical numbers have no leading zeroes, except for the number "0".
inline std::string normalize(const std::string& value) {
    const std::size_t first = value.find_first_not_of('0');
    return first == std::string::npos ? "0" : value.substr(first);
}

inline bool parse_positive(const std::string& token, std::string& value) {
    if (token.empty() || token.size() > 100) {
        return false;
    }
    for (std::size_t i = 0; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') {
            return false;
        }
    }
    value = normalize(token);
    return value != "0";
}

inline int compare(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) {
        return a.size() < b.size() ? -1 : 1;
    }
    return a == b ? 0 : (a < b ? -1 : 1);
}

inline std::string add(const std::string& a, const std::string& b) {
    std::string result;
    std::size_t i = a.size();
    std::size_t j = b.size();
    int carry = 0;
    while (i != 0 || j != 0 || carry != 0) {
        int digit = carry;
        if (i != 0) {
            digit += a[--i] - '0';
        }
        if (j != 0) {
            digit += b[--j] - '0';
        }
        result.push_back(static_cast<char>('0' + digit % 10));
        carry = digit / 10;
    }
    std::reverse(result.begin(), result.end());
    return result;
}

// Requires a >= b; both inputs are canonical nonnegative integers.
inline std::string subtract_magnitude(const std::string& a,
                                      const std::string& b) {
    std::string result;
    std::size_t i = a.size();
    std::size_t j = b.size();
    int borrow = 0;
    while (i != 0) {
        int digit = a[--i] - '0' - borrow;
        if (j != 0) {
            digit -= b[--j] - '0';
        }
        borrow = digit < 0 ? 1 : 0;
        if (borrow != 0) {
            digit += 10;
        }
        result.push_back(static_cast<char>('0' + digit));
    }
    std::reverse(result.begin(), result.end());
    return normalize(result);
}

inline std::string subtract(const std::string& a, const std::string& b) {
    return compare(a, b) < 0 ? "-" + subtract_magnitude(b, a)
                             : subtract_magnitude(a, b);
}

inline std::string multiply(const std::string& a, const std::string& b) {
    if (a == "0" || b == "0") {
        return "0";
    }
    std::vector<int> digits(a.size() + b.size(), 0);
    for (std::size_t i = 0; i < a.size(); ++i) {
        for (std::size_t j = 0; j < b.size(); ++j) {
            digits[i + j] += (a[a.size() - 1 - i] - '0') *
                             (b[b.size() - 1 - j] - '0');
        }
    }
    for (std::size_t i = 0; i + 1 < digits.size(); ++i) {
        digits[i + 1] += digits[i] / 10;
        digits[i] %= 10;
    }
    std::string result;
    for (std::size_t i = digits.size(); i != 0; --i) {
        result.push_back(static_cast<char>('0' + digits[i - 1]));
    }
    return normalize(result);
}

inline std::string halve(const std::string& value) {
    std::string result;
    int remainder = 0;
    for (std::size_t i = 0; i < value.size(); ++i) {
        const int digit = remainder * 10 + value[i] - '0';
        result.push_back(static_cast<char>('0' + digit / 2));
        remainder = digit % 2;
    }
    return normalize(result);
}

// Decimal long division: each quotient digit needs at most nine subtractions.
// Requires b > 0, as guaranteed by the input validation.
inline std::pair<std::string, std::string> divide(const std::string& a,
                                                const std::string& b) {
    std::string quotient;
    std::string remainder = "0";
    for (std::size_t i = 0; i < a.size(); ++i) {
        remainder = normalize(remainder + a[i]);
        int digit = 0;
        while (compare(remainder, b) >= 0) {
            remainder = subtract_magnitude(remainder, b);
            ++digit;
        }
        quotient.push_back(static_cast<char>('0' + digit));
    }
    return std::make_pair(normalize(quotient), remainder);
}

inline bool parse_choice(const std::string& token, int maximum, int& choice) {
    std::size_t i = 0;
    bool negative = false;
    if (!token.empty() && (token[0] == '+' || token[0] == '-')) {
        negative = token[0] == '-';
        ++i;
    }
    if (i == token.size()) {
        return false;
    }
    choice = 0;
    for (; i < token.size(); ++i) {
        if (token[i] < '0' || token[i] > '9') {
            return false;
        }
        choice = choice * 10 + token[i] - '0';
        if (choice > maximum) {
            return false;
        }
    }
    return !negative || choice == 0;
}

inline void print_menu(bool bonus) {
    std::cout << "----Welcome to big number calculator----\n"
              << "Please choose the function\n"
              << "1. Addition\n"
              << "2. Subtraction\n"
              << "3. Multiplication\n";
    if (bonus) {
        std::cout << "4. Division\n";
    }
    std::cout << "0. Exit\n--------------------------\n";
}

inline int run_calculator(bool bonus) {
    for (;;) {
        print_menu(bonus);
        std::string token;
        if (!(std::cin >> token)) {
            return 0;
        }
        int choice = 0;
        if (!parse_choice(token, bonus ? 4 : 3, choice)) {
            std::cout << "Error! Please try again.\n\n";
            continue;
        }
        if (choice == 0) {
            std::cout << "Good bye!\n";
            return 0;
        }
        if (choice == 1) {
            std::cout << "----Now for Addition----\n";
        } else if (choice == 2) {
            std::cout << "----Now for Subtraction ----\n";
        } else if (choice == 3) {
            std::cout << "----Now for Multiplication----\n";
        } else {
            std::cout << "----Now for Division----\n";
        }
        std::cout << "Please enter the two numbers.\n";
        std::string raw_a;
        std::string raw_b;
        if (!(std::cin >> raw_a >> raw_b)) {
            return 0;
        }
        std::string a;
        std::string b;
        if (!parse_positive(raw_a, a) || !parse_positive(raw_b, b)) {
            std::cout << "Error! Please try again.\n\n";
            continue;
        }
        if (choice == 1) {
            std::cout << "Result: " << add(a, b) << '\n';
        } else if (choice == 2) {
            std::cout << "Result: " << subtract(a, b) << '\n';
        } else if (choice == 3) {
            std::cout << "Result: " << multiply(a, b) << '\n';
        } else {
            const std::pair<std::string, std::string> result = divide(a, b);
            std::cout << "Result: " << result.first << '\n'
                      << "Remainder: " << result.second << '\n';
        }
        std::cout << "Mean: " << halve(add(a, b)) << "\n\n";
    }
}

}  // namespace pa03

#endif
