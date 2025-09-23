#include "CHTLLoader.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

std::filesystem::path CHTLLoader::resolvePath(const std::string& path, const std::string& currentFilePath) {
    std::filesystem::path currentDir = std::filesystem::path(currentFilePath).parent_path();
    return currentDir / path;
}

std::string CHTLLoader::loadFile(const std::string& path, const std::string& currentFilePath) {
    std::filesystem::path fullPath = resolvePath(path, currentFilePath);

    if (!std::filesystem::exists(fullPath)) {
        throw std::runtime_error("Imported file not found: " + fullPath.string());
    }

    std::ifstream file(fullPath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open imported file: " + fullPath.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
