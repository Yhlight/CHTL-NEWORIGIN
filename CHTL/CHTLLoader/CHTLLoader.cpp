#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

std::string CHTLLoader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("CHTLLoader Error: Could not open file " + filepath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace CHTL
