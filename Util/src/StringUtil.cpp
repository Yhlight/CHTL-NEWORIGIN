#include "StringUtil.hpp"

#include <algorithm>

namespace chtl::util {

std::string trim(std::string_view input) {
    size_t start = 0;
    size_t end = input.size();
    while (start < end && std::isspace(static_cast<unsigned char>(input[start]))) ++start;
    while (end > start && std::isspace(static_cast<unsigned char>(input[end - 1]))) --end;
    return std::string(input.substr(start, end - start));
}

std::vector<std::string> split(std::string_view input, char delim) {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos <= input.size()) {
        size_t next = input.find(delim, pos);
        if (next == std::string_view::npos) {
            parts.emplace_back(input.substr(pos));
            break;
        }
        parts.emplace_back(input.substr(pos, next - pos));
        pos = next + 1;
    }
    return parts;
}

bool startsWith(std::string_view s, std::string_view prefix) {
    return s.size() >= prefix.size() && std::equal(prefix.begin(), prefix.end(), s.begin());
}

bool endsWith(std::string_view s, std::string_view suffix) {
    return s.size() >= suffix.size() && std::equal(suffix.rbegin(), suffix.rend(), s.rbegin());
}

}
