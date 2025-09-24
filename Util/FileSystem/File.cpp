#include "File.h"
#include <fstream>
#include <sstream>

namespace CHTL {
namespace Util {

std::optional<std::string> ReadFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return std::nullopt;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool WriteFile(const std::string& filepath, const std::string& content) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }

    file << content;
    return true;
}

} // namespace Util
} // namespace CHTL
