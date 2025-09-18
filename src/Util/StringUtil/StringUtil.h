#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace CHTL {

class StringUtil {
public:
    // Splits a string by a delimiter.
    static std::vector<std::string> split(const std::string& str, char delimiter);

    // Trims from start (in place)
    static void ltrim(std::string &s);

    // Trims from end (in place)
    static void rtrim(std::string &s);

    // Trims from both ends (in place)
    static void trim(std::string &s);

    // Checks if a string starts with a prefix.
    static bool startsWith(const std::string& str, const std::string& prefix);

    // Checks if a string ends with a suffix.
    static bool endsWith(const std::string& str, const std::string& suffix);

    // Replaces all occurrences of a substring with another.
    static void replaceAll(std::string& str, const std::string& from, const std::string& to);
};

} // namespace CHTL
