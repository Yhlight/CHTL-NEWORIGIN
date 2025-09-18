#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace CHTL {

std::string FileSystem::readFile(const std::filesystem::path& path) {
    if (!exists(path) || !isFile(path)) {
        throw std::runtime_error("File does not exist or is not a regular file: " + path.string());
    }

    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for reading: " + path.string());
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystem::writeFile(const std::filesystem::path& path, const std::string& content) {
    // Create parent directory if it doesn't exist
    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + path.string());
    }
    file << content;
}

bool FileSystem::exists(const std::filesystem::path& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isFile(const std::filesystem::path& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::isDirectory(const std::filesystem::path& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::createDirectory(const std::filesystem::path& path) {
    return std::filesystem::create_directories(path);
}

std::vector<std::filesystem::path> FileSystem::getFiles(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> files;
    if (!isDirectory(directory)) {
        return files;
    }
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }
    return files;
}

std::vector<std::filesystem::path> FileSystem::getDirectories(const std::filesystem::path& directory) {
    std::vector<std::filesystem::path> directories;
    if (!isDirectory(directory)) {
        return directories;
    }
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            directories.push_back(entry.path());
        }
    }
    return directories;
}

} // namespace CHTL
