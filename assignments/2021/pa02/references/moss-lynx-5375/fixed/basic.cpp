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
long long int decimaltobinary(int value) {
    if (value == 0) return 0;
    return (decimaltobinary(value / 2) * 10) | (value % 2);
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

int main() {
    cout << "*** Welcome to Positive Binary adder ***\n";
    long long int i = 1;
    long long int binary_partial = 0;
    int error = 0;
    while (1) {
        prompt(i, error);
        int input = 0;
        int status = read_number(input);
        if (!status) return 0;
        if (status == 1 && input == -1) { cout << "Bye\n"; return 0; }
        if (status != 1 || input < 0 || input > 524287) { error = 1; continue; }
        long long int binary_input = decimaltobinary(input);
        if (i == 1) { binary_partial = binary_input; ++i; error = 0; continue; }
        int overflow = 0;
        long long int binary_result = binaryadder(binary_partial, binary_input, 0, 19, overflow);
        if (overflow) { error = 2; continue; }
        cout << "Calculating... " << binary_partial << " + " << binary_input
             << " = " << binary_result << "\nPartial Sum: "
             << binarytodecimal(binary_result) << "\n\n";
        binary_partial = binary_result;
        ++i;
        error = 0;
    }
}
