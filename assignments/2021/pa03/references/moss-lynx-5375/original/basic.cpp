#include <iostream>
#include <cmath>
#include <vector>
#include <string>

using namespace std;

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
    for (int i = 0; i < lengthsss; ++i) {
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
        int a;//輸入
        string b;
        string c;
        cin >> a;
        if (a == 1) {
            cout << "----Now for Addition----" << endl;
            cout << "Please enter the two numbers." << endl;
            cin >> b;
            cin >> c;
            inputvectorandadder(b, c);
            cout << "\nMean: ";
            mean(b, c);
            cout << endl << endl;
            continue;
        }
        else if (a == 2) {
            cout << "----Now for Subtraction----" << endl;
            cout << "Please enter the two numbers." << endl;
            cin >> b;
            cin >> c;
            inputvectorandSubtraction(b, c);
            cout << "\nMean: ";
            mean(b, c);
            cout << endl << endl;
            continue;
        }
        else if (a == 3) {
            cout << "----Now for Multiplication----" << endl;
            cout << "Please enter the two numbers." << endl;
            cin >> b;
            cin >> c;
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
