// Repaired anonymous reference; original recursion/packed-bit design retained.
#include <iostream>
#include <climits>
using namespace std;

// Decimal-digit containers hold binary digits, as in the original reference.
// All user values and all arithmetic state are local integer variables.
long long int binary_sum_bits(long long int a, long long int b) {
    if (b == 0) return a;
    return binary_sum_bits(a ^ b, (a & b) << 1);
}
long long int d2bpositive(int value) {
    if (value == 0) return 0;
    return (d2bpositive(value / 2) * 10) | (value % 2);
}
int binarytodecimal(long long int value) {
    if (value == 0) return 0;
    return (binarytodecimal(value / 10) * 2) |
           static_cast<int>(value % 10);
}
// Full-adder truth table; fixed width avoids constructing a 20-digit number.
long long int binaryadder(long long int a, long long int b,
                          int carry, int bits, int& carry_out) {
    if (bits == 0) { carry_out = carry; return 0; }
    int left = static_cast<int>(a % 10);
    int right = static_cast<int>(b % 10);
    int bit = left ^ right ^ carry;
    carry = (left & right) | (carry & (left ^ right));
    --bits;
    long long int higher = binaryadder(a / 10, b / 10, carry, bits, carry_out);
    return (higher * 10) | bit;
}
int space(int c) {
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' ||
           c == '\f' || c == '\v';
}
// Parse a complete token directly into int; no array/string/char local needed.
int read_number(int& value) {
    int c = cin.get();
    while (c != -1 && space(c)) c = cin.get();
    if (c == -1) return 0;
    int negative = c == '-';
    if (negative || c == '+') c = cin.get();
    int digit_count = 0;
    int valid = 1;
    long long int magnitude = 0;
    while (c != -1 && !space(c)) {
        if (c < '0' || c > '9') valid = 0;
        else {
            ++digit_count;
            int digit = c & 15;
            if (magnitude > 214748364LL ||
                (magnitude == 214748364LL && digit > (negative ? 8 : 7)))
                valid = 0;
            if (valid) magnitude = binary_sum_bits(magnitude * 10, digit);
        }
        c = cin.get();
    }
    if (!valid || !digit_count) return -1;
    value = static_cast<int>(negative ? -magnitude : magnitude);
    return 1;
}
void timee(long long int i) {
    cout << i;
    if (i % 100 >= 11 && i % 100 <= 13) cout << "th";
    else if (i % 10 == 1) cout << "st";
    else if (i % 10 == 2) cout << "nd";
    else if (i % 10 == 3) cout << "rd";
    else cout << "th";
}
void prompt(long long int i, int error) {
    if (error == 1) cout << "The decimal number you entered is out of range. ";
    else if (error == 2) cout << "The partial sum is out of range. ";
    cout << "Please enter the ";
    timee(i);
    cout << (error ? " decimal number again: " : " number in decimal: ");
}

long long int inverted(long long int binary, int width) {
    if (width == 0) return 0;
    int bit = static_cast<int>(binary % 10) ^ 1;
    --width;
    return (inverted(binary / 10, width) * 10) | bit;
}
long long int d2bnegative(int magnitude, int width) {
    int ignored = 0;
    return binaryadder(inverted(d2bpositive(magnitude), width), 1, 0, width, ignored);
}
int bit_count(int n) {
    if (n == 0) return 0;
    int count = bit_count(n / 2);
    return ++count;
}
int negative_width(int magnitude) {
    int width = bit_count(magnitude);
#ifdef COURSE_WIDTH
    ++width;
#else
    // Powers of two already have a sign bit in their shortest negative form.
    // Removing the lowest set bit with XOR distinguishes powers without '-'.
    int low_bit = magnitude & -magnitude;
    if ((magnitude ^ low_bit) != 0) ++width;
#endif
    return width;
}
int negative_limit() {
#ifdef COURSE_WIDTH
    return -262143;
#else
    return -262144;
#endif
}
long long int encode19(int value) {
    return value < 0 ? d2bnegative(-value, 19) : d2bpositive(value);
}
int decode19(long long int binary, int sign) {
    if (!sign) return binarytodecimal(binary);
    int carry = 0;
    long long int magnitude = binaryadder(inverted(binary, 19), 1, 0, 19, carry);
    if (carry) return -524288;
    return -binarytodecimal(magnitude);
}
void show_signed(int value) {
    if (value >= 0) cout << '0' << d2bpositive(value);
    else cout << d2bnegative(-value, negative_width(-value));
}
int main() {
    cout << "*** Welcome to Binary Adder and Subtractor ***\n";
    long long int N = 1;
    long long int binary_partial = 0;
    int partial = 0;
    int error = 0;
    while (1) {
        prompt(N, error);
        int input = 0;
        int status = read_number(input);
        if (!status) return 0;
        if (status == 1 && input == 0) { cout << "Bye\n"; return 0; }
        if (status != 1 || input < negative_limit() || input > 524287) {
            error = 1; continue;
        }
        long long int binary_input = encode19(input);
        if (N == 1) {
            binary_partial = binary_input; partial = input;
            ++N; error = 0; continue;
        }
        // The nineteenth low bit container plus a separate sign bit implements
        // sign extension without ever storing 20 decimal digits in long long.
        int carry = 0;
        long long int binary_result = binaryadder(binary_partial, binary_input, 0, 19, carry);
        int sign = (partial < 0) ^ (input < 0) ^ carry;
        int result = decode19(binary_result, sign);
        if ((partial >= 0 && input >= 0 && sign) ||
            (partial < 0 && input < 0 && !sign) || result < negative_limit()) {
            error = 2; continue;
        }
        cout << "Calculating... "; show_signed(partial);
        cout << " + "; show_signed(input);
        cout << " = "; show_signed(result);
        cout << "\nPartial Sum: " << result << "\n\n";
        partial = result;
        binary_partial = binary_result;
        ++N; error = 0;
    }
}
