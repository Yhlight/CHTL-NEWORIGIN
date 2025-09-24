#include "StringUtil.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <vector>
#include <fstream>

namespace CHTL::Util {

std::string StringUtil::trim(const std::string& str) {
    const std::string whitespace = " \t\n\r\f\v";
    size_t first = str.find_first_not_of(whitespace);
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(whitespace);
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> StringUtil::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;

    // FIX: Handle empty string case as per code review suggestion.
    // An empty input should result in a vector containing one empty string.
    if (str.empty()) {
        tokens.push_back("");
        return tokens;
    }

    std::string::size_type start = 0;
    std::string::size_type end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }

    tokens.push_back(str.substr(start));

    return tokens;
}

} // namespace CHTL::Util
