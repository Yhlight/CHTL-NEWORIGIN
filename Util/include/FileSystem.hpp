#pragma once

#include <string>
#include <string_view>

namespace chtl::util {

std::string readFileToString(const std::string& path);
bool writeStringToFile(const std::string& path, std::string_view content);

}
