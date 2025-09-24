#include "FileSystemUtil.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL::Util {

std::string FileSystemUtil::readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for reading: " + path.string());
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    file.close();
    return contents.str();
}

void FileSystemUtil::writeFile(const std::filesystem::path& path, const std::string& content) {
    // Ensure the directory exists
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path, std::ios::out | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Cannot open file for writing: " + path.string());
    }

    file << content;
    file.close();
}

} // namespace CHTL::Util
