#include "../../../include/CHTLCommon.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <dirent.h>
#include <sys/stat.h>

namespace CHTL {

String FileSystem::readFile(const String& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw CHTLException("Cannot open file: " + path);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void FileSystem::writeFile(const String& path, const String& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw CHTLException("Cannot write to file: " + path);
    }
    
    file << content;
}

bool FileSystem::fileExists(const String& path) {
    struct stat buffer;
    return stat(path.c_str(), &buffer) == 0;
}

bool FileSystem::isDirectory(const String& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return S_ISDIR(buffer.st_mode);
}

StringVector FileSystem::listFiles(const String& path) {
    StringVector files;
    
    DIR* dir = opendir(path.c_str());
    if (dir == nullptr) {
        throw CHTLException("Cannot open directory: " + path);
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_name[0] != '.') { // Skip hidden files
            files.push_back(entry->d_name);
        }
    }
    
    closedir(dir);
    return files;
}

String FileSystem::getExtension(const String& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == String::npos) {
        return "";
    }
    return path.substr(dotPos);
}

String FileSystem::getBasename(const String& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == String::npos) {
        return path;
    }
    return path.substr(slashPos + 1);
}

String FileSystem::getDirname(const String& path) {
    size_t slashPos = path.find_last_of("/\\");
    if (slashPos == String::npos) {
        return ".";
    }
    return path.substr(0, slashPos);
}

String FileSystem::normalizePath(const String& path) {
    String result = path;
    
    // Replace backslashes with forward slashes
    std::replace(result.begin(), result.end(), '\\', '/');
    
    // Remove double slashes
    size_t pos = 0;
    while ((pos = result.find("//", pos)) != String::npos) {
        result.replace(pos, 2, "/");
    }
    
    // Handle . and .. segments
    StringVector segments = StringUtil::split(result, '/');
    StringVector normalized;
    
    for (const String& segment : segments) {
        if (segment == "." || segment.empty()) {
            continue;
        } else if (segment == "..") {
            if (!normalized.empty() && normalized.back() != "..") {
                normalized.pop_back();
            } else {
                normalized.push_back(segment);
            }
        } else {
            normalized.push_back(segment);
        }
    }
    
    // Reconstruct path
    if (normalized.empty()) {
        return ".";
    }
    
    result = normalized[0];
    for (size_t i = 1; i < normalized.size(); ++i) {
        result += "/" + normalized[i];
    }
    
    return result;
}

} // namespace CHTL