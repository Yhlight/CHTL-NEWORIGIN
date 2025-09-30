#include "CHTL/CHTLLoader.h"
#include <fstream>
#include <sstream>

namespace CHTL {

CHTLLoader::CHTLLoader() = default;

CHTLLoader::~CHTLLoader() = default;

void CHTLLoader::initialize() {
    // Initialize loader
}

std::string CHTLLoader::loadFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLLoader::fileExists(const std::string& filePath) const {
    std::ifstream file(filePath);
    return file.good();
}

std::vector<std::string> CHTLLoader::getDependencies(const std::string& content) {
    std::vector<std::string> dependencies;
    // Parse content for import statements
    // This is a simplified implementation
    return dependencies;
}

} // namespace CHTL