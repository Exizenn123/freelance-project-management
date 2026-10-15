#include "Utils.h"
#include <cctype>
#include <algorithm>
#include <sstream>

namespace Utils {
string readLine(const string& prompt) {
    cout << prompt;
    string s;
    if (!getline(cin, s)) throw runtime_error("Input ended");
    return s;
}
int readInt(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            int v = stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (const exception&) {}
        cout << "  Invalid number. Try again.\n";
    }
}
double readDouble(const string& prompt) {
    while (true) {
        string s = readLine(prompt);
        try {
            size_t pos;
            double v = stod(s, &pos);
            if (pos == s.size()) return v;
        } catch (const exception&) {}
        cout << "  Invalid number. Try again.\n";
    }
}
bool isValidName(const string& s) {
    if (s.size() < 2 || s.size() > 40) return false;
    bool letter = false;
    for (char c : s) {
        if (isalpha(static_cast<unsigned char>(c))) letter = true;
        else if (c != ' ' && c != '.') return false;
    }
    return letter;
}
bool isValidPhone(const string& s) {
    if (s.size() != 10) return false;
    for (char c : s) if (!isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}
bool isValidText(const string& s) {
    return !s.empty() && s.find('|') == string::npos;
}
bool isValidDate(const string& s) {
    if (s.size() != 10 || s[2] != '-' || s[5] != '-') return false;
    for (int i : {0, 1, 3, 4, 6, 7, 8, 9}) if (!isdigit(static_cast<unsigned char>(s[i]))) return false;
    int d = stoi(s.substr(0, 2)), m = stoi(s.substr(3, 2)), y = stoi(s.substr(6, 4));
    if (y < 2000 || y > 2100 || m < 1 || m > 12 || d < 1) return false;
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
    if (leap) days[1] = 29;
    return d <= days[m - 1];
}
string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return tolower(c); });
    return s;
}
}
