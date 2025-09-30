#include "CHTL/CHTLLoader.h"
#include <fstream>
#include <filesystem>

namespace CHTL {

CHTLLoader::CHTLLoader() = default;

std::string CHTLLoader::loadFile(const std::string& filePath) const {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool CHTLLoader::fileExists(const std::string& filePath) const {
    return std::filesystem::exists(filePath);
}

std::vector<std::string> CHTLLoader::getFileList(const std::string& directory) const {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // Directory doesn't exist or can't be accessed
    }
    return files;
}

} // namespace CHTL