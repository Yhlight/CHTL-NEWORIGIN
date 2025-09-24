#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace StringUtil {

// Removes leading and trailing whitespace from a string.
std::string trim(const std::string& str);

// Splits a string into a vector of substrings based on a delimiter.
std::vector<std::string> split(const std::string& str, char delimiter);

// Checks if a string starts with a given prefix.
bool startsWith(const std::string& str, const std::string& prefix);

// Checks if a string ends with a given suffix.
bool endsWith(const std::string& str, const std::string& suffix);

} // namespace StringUtil
} // namespace CHTL
