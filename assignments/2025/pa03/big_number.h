#ifndef ENERGY_BIG_NUMBER_H
#define ENERGY_BIG_NUMBER_H
#include <vector>
#include <string>
#include <algorithm>

// Digits are little-endian in the selected radix, with a separate sign.
class BigNumber {
    int base;
    bool negative;
    std::vector<int> digits;
    void normalize() {
        while (digits.size() > 1 && digits.back() == 0) digits.pop_back();
        if (digits.size() == 1 && digits[0] == 0) negative = false;
    }
    int compare_absolute(const BigNumber& b) const {
        if (digits.size() != b.digits.size()) return digits.size() > b.digits.size() ? 1 : -1;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i)
            if (digits[i] != b.digits[i]) return digits[i] > b.digits[i] ? 1 : -1;
        return 0;
    }
public:
    explicit BigNumber(int radix = 10) : base(radix), negative(false), digits(1, 0) {}
    BigNumber(const std::string& text, int radix) : base(radix), negative(text[0] == '-') {
        for (int i = static_cast<int>(text.size()) - 1; i >= (negative ? 1 : 0); --i)
            digits.push_back(text[i] <= '9' ? text[i] - '0' : text[i] - 'A' + 10);
        normalize();
    }
    std::string str() const {
        std::string text = negative ? "-" : "";
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) text += "0123456789ABCDEF"[digits[i]];
        return text;
    }
    BigNumber operator+(const BigNumber& b) const {
        BigNumber result(base); result.digits.clear();
        if (negative == b.negative) {
            int carry = 0;
            for (std::size_t i = 0; i < std::max(digits.size(), b.digits.size()) || carry; ++i) {
                int value = carry + (i < digits.size() ? digits[i] : 0) + (i < b.digits.size() ? b.digits[i] : 0);
                result.digits.push_back(value % base); carry = value / base;
            }
            result.negative = negative;
        } else {
            int comparison = compare_absolute(b);
            const BigNumber& large = comparison >= 0 ? *this : b;
            const BigNumber& small = comparison >= 0 ? b : *this;
            int borrow = 0;
            for (std::size_t i = 0; i < large.digits.size(); ++i) {
                int value = large.digits[i] - borrow - (i < small.digits.size() ? small.digits[i] : 0);
                borrow = value < 0; if (borrow) value += base;
                result.digits.push_back(value);
            }
            result.negative = large.negative;
        }
        result.normalize(); return result;
    }
    BigNumber operator*(const BigNumber& b) const {
        BigNumber result(base); result.digits.assign(digits.size() + b.digits.size() + 1, 0);
        for (std::size_t i = 0; i < digits.size(); ++i) {
            int carry = 0;
            for (std::size_t j = 0; j < b.digits.size() || carry; ++j) {
                int value = result.digits[i + j] + carry + digits[i] * (j < b.digits.size() ? b.digits[j] : 0);
                result.digits[i + j] = value % base; carry = value / base;
            }
        }
        result.negative = negative != b.negative; result.normalize(); return result;
    }
    BigNumber rounded_average(int divisor) const {
        BigNumber result(base); result.digits.assign(digits.size(), 0);
        long long remainder = 0;
        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            long long value = remainder * base + digits[i];
            result.digits[i] = static_cast<int>(value / divisor); remainder = value % divisor;
        }
        // Round the magnitude first; this handles negative quotients and -0.5.
        if (remainder * 2 >= divisor) {
            int carry = 1;
            for (std::size_t i = 0; carry; ++i) {
                if (i == result.digits.size()) result.digits.push_back(0);
                int value = result.digits[i] + carry; result.digits[i] = value % base; carry = value / base;
            }
        }
        result.negative = negative; result.normalize(); return result;
    }
};
#endif
