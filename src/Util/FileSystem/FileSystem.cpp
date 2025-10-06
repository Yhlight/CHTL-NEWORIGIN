#include "FileSystem.h"
#include "../StringUtil/StringUtil.h"
#include <filesystem>
#include <fstream>
#include <sstream>

namespace fs = std::filesystem;

namespace CHTL {
namespace Util {

String FileSystem::readFile(const String& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Vector<String> FileSystem::readLines(const String& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    
    Vector<String> lines;
    String line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    return lines;
}

bool FileSystem::writeFile(const String& path, const String& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool FileSystem::writeLines(const String& path, const Vector<String>& lines) {
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& line : lines) {
        file << line << '\n';
    }
    return file.good();
}

bool FileSystem::appendFile(const String& path, const String& content) {
    std::ofstream file(path, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return file.good();
}

bool FileSystem::exists(const String& path) {
    return fs::exists(path);
}

bool FileSystem::isFile(const String& path) {
    return fs::is_regular_file(path);
}

bool FileSystem::isDirectory(const String& path) {
    return fs::is_directory(path);
}

bool FileSystem::createDirectory(const String& path) {
    return fs::create_directory(path);
}

bool FileSystem::createDirectories(const String& path) {
    return fs::create_directories(path);
}

bool FileSystem::removeFile(const String& path) {
    return fs::remove(path);
}

bool FileSystem::removeDirectory(const String& path) {
    return fs::remove_all(path) > 0;
}

String FileSystem::getFileName(const String& path) {
    return fs::path(path).filename().string();
}

String FileSystem::getFileNameWithoutExtension(const String& path) {
    return fs::path(path).stem().string();
}

String FileSystem::getExtension(const String& path) {
    auto ext = fs::path(path).extension().string();
    if (!ext.empty() && ext[0] == '.') {
        return ext.substr(1);
    }
    return ext;
}

String FileSystem::getDirectory(const String& path) {
    return fs::path(path).parent_path().string();
}

String FileSystem::joinPath(const String& path1, const String& path2) {
    return (fs::path(path1) / path2).string();
}

String FileSystem::normalizePath(const String& path) {
    return fs::path(path).lexically_normal().string();
}

String FileSystem::absolutePath(const String& path) {
    return fs::absolute(path).string();
}

Vector<String> FileSystem::listFiles(const String& directory, bool recursive) {
    Vector<String> files;
    
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return files;
    }
    
    if (recursive) {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    } else {
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                files.push_back(entry.path().string());
            }
        }
    }
    
    return files;
}

Vector<String> FileSystem::findFiles(const String& directory, const String& pattern, bool recursive) {
    Vector<String> files;
    auto allFiles = listFiles(directory, recursive);
    
    for (const auto& file : allFiles) {
        String fileName = getFileName(file);
        // 简单的通配符匹配
        if (pattern == "*" || pattern == "*.*") {
            files.push_back(file);
        } else if (StringUtil::contains(pattern, "*")) {
            // 处理通配符
            String prefix = pattern.substr(0, pattern.find('*'));
            String suffix = pattern.substr(pattern.find('*') + 1);
            if (StringUtil::startsWith(fileName, prefix) && StringUtil::endsWith(fileName, suffix)) {
                files.push_back(file);
            }
        } else if (fileName == pattern || getExtension(file) == pattern) {
            files.push_back(file);
        }
    }
    
    return files;
}

} // namespace Util
} // namespace CHTL
