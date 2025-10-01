#include "TestUtil.h"
#include <algorithm>
#include <cctype>

std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), [](unsigned char c) { return std::isspace(c); }), str.end());
    return str;
}