// Repaired anonymous reference. See ../README.md for bugs and provenance.
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

using namespace std;

// Validate before converting characters to decimal digits. The latest handout
// permits positive inputs of at most 100 digits; leading zeroes are normalized.
bool valid_number(const string& value) {
    if (value.empty() || value.size() > 100) return false;
    bool nonzero = false;
    for (size_t i = 0; i < value.size(); ++i) {
        if (value[i] < '0' || value[i] > '9') return false;
        if (value[i] != '0') nonzero = true;
    }
    return nonzero;
}
string normalized(string value) {
    size_t first = value.find_first_not_of('0');
    return first == string::npos ? "0" : value.substr(first);
}
// Preserve integer menu spellings such as +1, 01 and -0 without relying on
// native integer conversion, stream failure recovery, or signed overflow.
bool selection_value(const string& value, int maximum, int& selection) {
    if (value.empty()) return false;
    size_t position = 0;
    bool negative = false;
    if (value[0] == '+' || value[0] == '-') {
        negative = value[0] == '-';
        position = 1;
    }
    if (position == value.size()) return false;
    int result = 0;
    for (; position < value.size(); ++position) {
        if (value[position] < '0' || value[position] > '9') return false;
        // result is <= maximum before multiplication; maximum is only 3/4.
        result = result * 10 + value[position] - '0';
        if (result > maximum) return false;
    }
    if (negative && result != 0) return false;
    selection = result;
    return true;
}

void inputvectorandadder(string b, string c) {
    int lengthb = b.length();
    int lengthc = c.length();
    int lengthsss;
    int tf = 1;
    if (lengthb > lengthc) {
        lengthsss = lengthb + 1;
    }
    else {
        lengthsss = lengthc + 1;
    }
    vector <int> s1(lengthsss);
    vector <int> s2(lengthsss);
    vector <int> s3(lengthsss);
    for (int i = 0;i <= lengthb - 1; ++i) {
        s1[i] = b[lengthb - 1 - i] - '0';
    }
    for (int i = 0;i <= lengthc - 1; ++i) {
        s2[i] = c[lengthc - 1 - i] - '0';
    }
    for (int z = 0; z < lengthsss; ++z) {
        if (s1[z] + s2[z] < 10) {
            s3[z] = s1[z] + s2[z];
        }
        else {
            s3[z] = s1[z] + s2[z] - 10;
            s1[z + 1]++;
        }
    }
    cout << "Result: ";
    for (int k = lengthsss - 1;k >= 0;k--) {
        if (tf == 1) {
            if (s3[k] > 0) {
                cout << s3[k];
                tf = 0;
            }
            if (s3[k] == 0) {
                tf = 1;
            }
            continue;
        }
        cout << s3[k];
    }
    if (b.length() == 1 && c.length() == 1 && s1[0] == 0 && s2[0] == 0) {
        cout << 0;
    }
}
void mean(string b, string c) {
    int lengthb = b.length();
    int lengthc = c.length();
    int lengthsss;
    int tf = 1;
    if (lengthb > lengthc) {
        lengthsss = lengthb + 1;
    }
    else {
        lengthsss = lengthc + 1;
    }
    vector <int> s1(lengthsss);
    vector <int> s2(lengthsss);
    vector <int> s3(lengthsss);
    vector <int> s4(lengthsss);
    for (int i = 0;i <= lengthb - 1; ++i) {
        s1[i] = b[lengthb - 1 - i] - '0';
    }
    for (int i = 0;i <= lengthc - 1; ++i) {
        s2[i] = c[lengthc - 1 - i] - '0';
    }
    for (int z = 0; z < lengthsss; ++z) {
        if (s1[z] + s2[z] < 10) {
            s3[z] = s1[z] + s2[z];
        }
        else {
            s3[z] = s1[z] + s2[z] - 10;
            s1[z + 1]++;
        }
    }
    for (int z = lengthsss - 1;z > 0;z--) {
        if (s3[z] % 2 == 0) {
            s4[z] = s3[z] / 2;
        }
        if (s3[z] % 2 != 0) {
            s4[z] = s3[z] / 2;
            s3[z - 1] += 10;
        }
    }
    s4[0] = s3[0] / 2;
    for (int k = lengthsss - 1;k >= 0;k--) {
        if (tf == 1) {
            if (s4[k] == 0) {
                tf = 1;
            }
            if (s4[k] > 0) {
                cout << s4[k];
                tf = 0;
            }
            continue;
        }
        cout << s4[k];
    }
    if (b.length() == 1 && c.length() == 1 && s4[0] == 0) {
        cout << 0;
    }
}
void inputvectorandSubtraction(string b, string c) {
    const bool negative = b.size() < c.size() ||
                          (b.size() == c.size() && b < c);
    if (negative) swap(b, c);
    int lengthb = b.length();
    int lengthc = c.length();
    int lengthsss;
    int tf = 1;
    string memory;
    if (lengthb > lengthc) {
        lengthsss = lengthb + 1;
    }
    else {
        lengthsss = lengthc + 1;
    }
    vector <int> s1(lengthsss);
    vector <int> s2(lengthsss);
    vector <int> s3(lengthsss);
    for (int i = 0;i <= lengthb - 1; ++i) {
        s1[i] = b[lengthb - 1 - i] - '0';
    }
    for (int i = 0;i <= lengthc - 1; ++i) {
        s2[i] = c[lengthc - 1 - i] - '0';
    }
    for (int z = 0; z < lengthsss; ++z) {
        if (s1[z] - s2[z] >= 0) {
            s3[z] = s1[z] - s2[z];
        }
        else {
            s3[z] = s1[z] - s2[z] + 10;
            s1[z + 1]--;
        }
    }
    cout << "Result: ";
    if (negative) cout << "-";
    for (int k = lengthsss - 1;k > 0;k--) {
        if (tf == 1) {
            if (s3[k] > 0) {
                cout << s3[k];
                tf = 0;
            }
            if (s3[k] == 0) {
                tf = 1;
            }
            continue;
        }
        cout << s3[k];
    }
    cout << s3[0];
}
void inputvectorMultiplication(string b, string c) {
    int lengthb = b.length();
    int lengthc = c.length();
    int tf = 1;
    int lengthsss = lengthb + lengthc;
    vector <int> s1(lengthsss);
    vector <int> s2(lengthsss);
    vector <int> s3(lengthsss);
    for (int i = 0;i <= lengthb - 1; ++i) {
        s1[i] = b[lengthb - 1 - i] - '0';
    }
    for (int i = 0;i <= lengthc - 1; ++i) {
        s2[i] = c[lengthc - 1 - i] - '0';
    }
    for (int i = 0;i < lengthc;++i) {
        int k = i;
        for (int j = 0; j < lengthb;++j) {
            s3[k] += s2[i] * s1[j];
            k++;
        }
    }
    for (int i = 0; i + 1 < lengthsss; ++i) {
        if (s3[i] > 9) {
            s3[i + 1] += s3[i] / 10;
            s3[i] = s3[i] % 10;
        }
    }
    cout << "Result: ";
    for (int k = lengthsss - 1;k >= 0;k--) {
        if (tf == 1) {
            if (s3[k] > 0) {
                cout << s3[k];
                tf = 0;
            }
            if (s3[k] == 0) {
                tf = 1;
            }
            continue;
        }
        cout << s3[k];
    }
    if ((b.length() == 1 || c.length() == 1) && (s1[0] == 0 || s2[0] == 0)) {
        cout << 0;
    }
}
int main() {
    while (1) {
        cout << "----Welcome to big number calculator----" << endl;
        cout << "Please choose the function" << endl;
        cout << "1. Addition" << endl;
        cout << "2. Subtraction" << endl;
        cout << "3. Multiplication" << endl;
        cout << "0. Exit" << endl << "--------------------------" << endl;
        int a;
        string selection;
        string b;
        string c;
        if (!(cin >> selection)) break;
        if (!selection_value(selection, 3, a)) {
            cout << "Error! Please try again." << endl << endl;
            continue;
        }
        if (a == 1) {
            cout << "----Now for Addition----" << endl;
            cout << "Please enter the two numbers." << endl;
            if (!(cin >> b >> c)) break;
            if (!valid_number(b) || !valid_number(c)) {
                cout << "Error! Please try again." << endl << endl;
                continue;
            }
            b = normalized(b);
            c = normalized(c);
            inputvectorandadder(b, c);
            cout << "\nMean: ";
            mean(b, c);
            cout << endl << endl;
            continue;
        }
        else if (a == 2) {
            cout << "----Now for Subtraction----" << endl;
            cout << "Please enter the two numbers." << endl;
            if (!(cin >> b >> c)) break;
            if (!valid_number(b) || !valid_number(c)) {
                cout << "Error! Please try again." << endl << endl;
                continue;
            }
            b = normalized(b);
            c = normalized(c);
            inputvectorandSubtraction(b, c);
            cout << "\nMean: ";
            mean(b, c);
            cout << endl << endl;
            continue;
        }
        else if (a == 3) {
            cout << "----Now for Multiplication----" << endl;
            cout << "Please enter the two numbers." << endl;
            if (!(cin >> b >> c)) break;
            if (!valid_number(b) || !valid_number(c)) {
                cout << "Error! Please try again." << endl << endl;
                continue;
            }
            b = normalized(b);
            c = normalized(c);
            inputvectorMultiplication(b, c);
            cout << "\nMean: ";
            mean(b, c);
            cout << endl << endl;
            continue;
        }
        else if (a == 0) {
            cout << "Good bye!";
            break;
        }
        else {
            cout << "Error! Please try again." << endl << endl;
            continue;
        }
    }
    return (0);
}
