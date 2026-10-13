#ifndef BINARY_HPP
#define BINARY_HPP

#include <iostream>

// All variables and parameters are int / long long int. There are no arrays,
// strings, static variables or global variables. Decimal digit containers are
// long long int; their digits themselves are binary 0/1 digits.
inline long long int bitwiseAdd(long long int a, long long int b) {
    if (b == 0) return a;
    return bitwiseAdd(a ^ b, (a & b) << 1);
}

inline long long int decimalToBinary(int value) {
    if (value == 0) return 0;
    return (decimalToBinary(value / 2) * 10) | (value % 2);
}

inline int binaryToDecimal(long long int value) {
    if (value == 0) return 0;
    return (binaryToDecimal(value / 10) * 2) |
           static_cast<int>(value % 10);
}

inline long long int binaryAdder(long long int a, long long int b,
                                 int carry, int width, int& finalCarry) {
    if (width == 0) {
        finalCarry = carry;
        return 0;
    }
    int left = static_cast<int>(a % 10);
    int right = static_cast<int>(b % 10);
    int digit = left ^ right ^ carry;
    int nextCarry = (left & right) | (left & carry) | (right & carry);
    --width;
    long long int higher = binaryAdder(a / 10, b / 10, nextCarry,
                                       width, finalCarry);
    return (higher * 10) | digit;
}

inline long long int inverted(long long int number, int width) {
    if (width == 0) return 0;
    int digit = static_cast<int>(number % 10) ^ 1;
    --width;
    return (inverted(number / 10, width) * 10) | digit;
}

inline long long int encodedSigned(int number, int width) {
    if (number >= 0) return decimalToBinary(number);
    int magnitude = -number; // Unary sign only; subtraction uses the full adder.
    int carry = 0;
    return binaryAdder(inverted(decimalToBinary(magnitude), width), 1,
                       0, width, carry);
}

inline int decodedSigned(long long int packed, int sign) {
    if (sign == 0) return binaryToDecimal(packed);
    int carry = 0;
    long long int magnitude = binaryAdder(inverted(packed, 19), 1, 0,
                                          19, carry);
    // A separate sign bit allows a leading zero on positive 19-digit values.
    // The low 19 bits of -2^19 are zero; do not mistake that value for zero.
    if (carry) return -524288;
    return -binaryToDecimal(magnitude);
}

inline int magnitudeBits(int value) {
    if (value == 0) return 0;
    int width = magnitudeBits(value / 2);
    return ++width;
}

inline void showSignedBinary(int number) {
    if (number >= 0) {
        std::cout << '0' << decimalToBinary(number);
    } else {
        int width = magnitudeBits(-number);
        ++width;
        std::cout << encodedSigned(number, width);
    }
}

inline int whitespace(int c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
}

// Consume exactly one whitespace-delimited token without storing characters.
// Return 1 for a signed 32-bit integer, -1 for a bad token, 0 for clean EOF.
inline int readDecimal(int& value) {
    int c = std::cin.get();
    while (c != -1 && whitespace(c)) c = std::cin.get();
    if (c == -1) return 0;
    int negative = 0;
    if (c == '-' || c == '+') {
        negative = c == '-';
        c = std::cin.get();
    }
    long long int magnitude = 0;
    int digits = 0;
    int valid = 1;
    while (c != -1 && !whitespace(c)) {
        if (c < '0' || c > '9') {
            valid = 0;
        } else {
            ++digits;
            // ASCII decimal digits have their numeric value in the low nibble.
            int digit = c & 15;
            if (magnitude > 214748364LL ||
                (magnitude == 214748364LL && digit > (negative ? 8 : 7))) {
                valid = 0;
            } else if (valid) {
                magnitude = bitwiseAdd(magnitude * 10, digit);
            }
        }
        c = std::cin.get();
    }
    if (!valid || digits == 0) return -1;
    value = static_cast<int>(negative ? -magnitude : magnitude);
    return 1;
}

inline void ordinal(long long int index) {
    std::cout << index;
    long long int lastTwo = index % 100;
    if (lastTwo >= 11 && lastTwo <= 13) std::cout << "th";
    else if (index % 10 == 1) std::cout << "st";
    else if (index % 10 == 2) std::cout << "nd";
    else if (index % 10 == 3) std::cout << "rd";
    else std::cout << "th";
}

inline void prompt(long long int index, int error) {
    if (error == 1)
        std::cout << "The decimal number you entered is out of range. ";
    if (error == 2)
        std::cout << "The partial sum is out of range. ";
    std::cout << "Please enter the ";
    ordinal(index);
    if (error) std::cout << " decimal number again: ";
    else std::cout << " number in decimal: ";
}

inline int runBinaryAdder(int bonus) {
    if (bonus)
        std::cout << "*** Welcome to Binary Adder and Subtractor ***\n";
    else
        std::cout << "*** Welcome to Positive Binary adder ***\n";
    long long int index = 1;
    long long int partial = 0;
    int partialDecimal = 0;
    int error = 0;
    while (1) {
        prompt(index, error);
        int number = 0;
        int status = readDecimal(number);
        if (status == 0) return 0;
        if (status == 1 && number == (bonus ? 0 : -1)) {
            std::cout << "Bye\n";
            return 0;
        }
        if (status != 1 || (bonus && (number < -262143 || number > 524287)) ||
            (!bonus && (number < 0 || number > 524287))) {
            error = 1;
            continue;
        }
        long long int addend = bonus ? encodedSigned(number, 19)
                                     : decimalToBinary(number);
        if (index == 1) {
            partial = addend;
            partialDecimal = number;
            ++index;
            error = 0;
            continue;
        }
        int carry = 0;
        long long int next = binaryAdder(partial, addend, 0, 19, carry);
        int nextSign = (partialDecimal < 0) ^ (number < 0) ^ carry;
        int nextDecimal = bonus ? decodedSigned(next, nextSign) : binaryToDecimal(next);
        int overflow = bonus
            ? ((partialDecimal < 0 && number < 0 && nextDecimal >= 0) ||
               (partialDecimal >= 0 && number >= 0 && nextDecimal < 0) ||
               nextDecimal < -262143 || nextDecimal > 524287)
            : carry;
        if (overflow) {
            error = 2;
            continue;
        }
        std::cout << "Calculating... ";
        if (bonus) showSignedBinary(partialDecimal);
        else std::cout << partial;
        std::cout << " + ";
        if (bonus) showSignedBinary(number);
        else std::cout << addend;
        std::cout << " = ";
        if (bonus) showSignedBinary(nextDecimal);
        else std::cout << next;
        std::cout << "\nPartial Sum: " << nextDecimal << "\n\n";
        partial = next;
        partialDecimal = nextDecimal;
        ++index;
        error = 0;
    }
}

#endif
