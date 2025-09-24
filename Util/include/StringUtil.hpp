#pragma once

#include <string>
#include <string_view>
#include <vector>

namespace chtl::util {

std::string trim(std::string_view input);
std::vector<std::string> split(std::string_view input, char delim);
bool startsWith(std::string_view s, std::string_view prefix);
bool endsWith(std::string_view s, std::string_view suffix);

}
