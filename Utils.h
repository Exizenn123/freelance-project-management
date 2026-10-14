#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <iostream>
#include <stdexcept>
using namespace std;

// Small helper functions: input reading and validation.
namespace Utils {
    string readLine(const string& prompt);          // prompt + getline (throws on EOF)
    int    readInt(const string& prompt);           // keeps asking until a valid integer
    double readDouble(const string& prompt);        // keeps asking until a valid number
    bool   isValidName(const string& s);            // letters/spaces only, 2-40 chars
    bool   isValidPhone(const string& s);           // exactly 10 digits
    bool   isValidText(const string& s);            // not empty, no '|' character
    bool   isValidDate(const string& s);            // DD-MM-YYYY with real calendar check
    string toLower(string s);
    // Grows a dynamic array to double capacity (used for all entity arrays)
    template <class T>
    void grow(T*& arr, int count, int& capacity) {
        int newCap = capacity * 2;
        T* bigger = new T[newCap];
        for (int i = 0; i < count; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
        capacity = newCap;
    }
}
#endif
