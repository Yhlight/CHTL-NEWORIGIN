#include "Loader.h"
#include <fstream>
#include <sstream>

namespace CHTL {

std::optional<std::string> Loader::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return std::nullopt; // File could not be opened
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace CHTL
