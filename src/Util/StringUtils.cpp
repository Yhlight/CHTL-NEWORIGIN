#include "StringUtils.h"

size_t findMatchingBrace(const std::string& s, size_t start_pos) {
    if (s[start_pos] != '{') {
        return std::string::npos;
    }
    int balance = 1;
    for (size_t i = start_pos + 1; i < s.length(); ++i) {
        if (s[i] == '{') {
            balance++;
        } else if (s[i] == '}') {
            balance--;
        }
        if (balance == 0) {
            return i;
        }
    }
    return std::string::npos;
}