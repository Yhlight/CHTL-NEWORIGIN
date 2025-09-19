#include "FileSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <random>
#include <algorithm>

namespace CHTL {
namespace Util {

std::vector<std::string> FileSystem::tempFiles;
std::vector<std::string> FileSystem::tempDirectories;

bool FileSystem::exists(const std::string& path) {
    return std::filesystem::exists(path);
}

bool FileSystem::isDirectory(const std::string& path) {
    return std::filesystem::is_directory(path);
}

bool FileSystem::isFile(const std::string& path) {
    return std::filesystem::is_regular_file(path);
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directory(path);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FileSystem::createDirectories(const std::string& path) {
    try {
        return std::filesystem::create_directories(path);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FileSystem::removeFile(const std::string& path) {
    try {
        return std::filesystem::remove(path);
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FileSystem::removeDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(path) > 0;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FileSystem::copyFile(const std::string& from, const std::string& to) {
    try {
        std::filesystem::copy_file(from, to);
        return true;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

bool FileSystem::moveFile(const std::string& from, const std::string& to) {
    try {
        std::filesystem::rename(from, to);
        return true;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

size_t FileSystem::getFileSize(const std::string& path) {
    try {
        return std::filesystem::file_size(path);
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

std::string FileSystem::getFileExtension(const std::string& path) {
    std::filesystem::path p(path);
    return p.extension().string();
}

std::string FileSystem::getFileName(const std::string& path) {
    std::filesystem::path p(path);
    return p.filename().string();
}

std::string FileSystem::getFileNameWithoutExtension(const std::string& path) {
    std::filesystem::path p(path);
    return p.stem().string();
}

std::string FileSystem::getDirectoryPath(const std::string& path) {
    std::filesystem::path p(path);
    return p.parent_path().string();
}

std::string FileSystem::getAbsolutePath(const std::string& path) {
    try {
        return std::filesystem::absolute(path).string();
    } catch (const std::filesystem::filesystem_error&) {
        return path;
    }
}

std::string FileSystem::getRelativePath(const std::string& path, const std::string& base) {
    try {
        return std::filesystem::relative(path, base).string();
    } catch (const std::filesystem::filesystem_error&) {
        return path;
    }
}

std::vector<std::string> FileSystem::listDirectory(const std::string& path) {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            result.push_back(entry.path().string());
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录不存在或无法访问
    }
    return result;
}

std::vector<std::string> FileSystem::listDirectoryRecursive(const std::string& path) {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            result.push_back(entry.path().string());
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录不存在或无法访问
    }
    return result;
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> result;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(pattern) != std::string::npos) {
                    result.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error&) {
        // 目录不存在或无法访问
    }
    return result;
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::vector<uint8_t> FileSystem::readFileBinary(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0, std::ios::beg);
    
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);
    return buffer;
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool FileSystem::writeFileBinary(const std::string& path, const std::vector<uint8_t>& content) {
    std::ofstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(content.data()), content.size());
    return file.good();
}

bool FileSystem::appendFile(const std::string& path, const std::string& content) {
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

std::time_t FileSystem::getLastModifiedTime(const std::string& path) {
    try {
        auto time = std::filesystem::last_write_time(path);
        return std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count();
    } catch (const std::filesystem::filesystem_error&) {
        return 0;
    }
}

bool FileSystem::setLastModifiedTime(const std::string& path, std::time_t time) {
    try {
        // 简化实现，使用系统时间
        std::filesystem::last_write_time(path, std::filesystem::file_time_type::clock::now());
        return true;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

std::string FileSystem::getCurrentDirectory() {
    try {
        return std::filesystem::current_path().string();
    } catch (const std::filesystem::filesystem_error&) {
        return "";
    }
}

bool FileSystem::setCurrentDirectory(const std::string& path) {
    try {
        std::filesystem::current_path(path);
        return true;
    } catch (const std::filesystem::filesystem_error&) {
        return false;
    }
}

std::string FileSystem::normalizePath(const std::string& path) {
    try {
        return std::filesystem::canonical(path).string();
    } catch (const std::filesystem::filesystem_error&) {
        return path;
    }
}

std::string FileSystem::joinPath(const std::string& path1, const std::string& path2) {
    std::filesystem::path p1(path1);
    std::filesystem::path p2(path2);
    return (p1 / p2).string();
}

template<typename... Args>
std::string FileSystem::joinPath(const std::string& path, Args&&... args) {
    std::filesystem::path result(path);
    ((result /= args), ...);
    return result.string();
}

bool FileSystem::isAbsolutePath(const std::string& path) {
    std::filesystem::path p(path);
    return p.is_absolute();
}

bool FileSystem::isRelativePath(const std::string& path) {
    return !isAbsolutePath(path);
}

std::string FileSystem::getTempDirectory() {
    return std::filesystem::temp_directory_path().string();
}

std::string FileSystem::createTempFile(const std::string& prefix, const std::string& suffix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string filename = prefix;
    for (int i = 0; i < 8; ++i) {
        filename += "0123456789abcdef"[dis(gen)];
    }
    filename += suffix;
    
    std::string tempPath = joinPath(getTempDirectory(), filename);
    tempFiles.push_back(tempPath);
    return tempPath;
}

std::string FileSystem::createTempDirectory(const std::string& prefix) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    
    std::string dirname = prefix;
    for (int i = 0; i < 8; ++i) {
        dirname += "0123456789abcdef"[dis(gen)];
    }
    
    std::string tempPath = joinPath(getTempDirectory(), dirname);
    if (createDirectory(tempPath)) {
        tempDirectories.push_back(tempPath);
        return tempPath;
    }
    return "";
}

void FileSystem::cleanupTempFiles() {
    for (const auto& file : tempFiles) {
        removeFile(file);
    }
    tempFiles.clear();
    
    for (const auto& dir : tempDirectories) {
        removeDirectory(dir);
    }
    tempDirectories.clear();
}

} // namespace Util
} // namespace CHTL