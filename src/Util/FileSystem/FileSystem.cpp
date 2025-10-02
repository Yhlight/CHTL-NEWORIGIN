#include "FileSystem.h"
#include <fstream>
#include <sstream>
#include <sys/stat.h>

namespace Util {

bool FileSystem::exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool FileSystem::isFile(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && S_ISREG(buffer.st_mode);
}

bool FileSystem::isDirectory(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0) && S_ISDIR(buffer.st_mode);
}

std::string FileSystem::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) return false;
    file << content;
    return true;
}

bool FileSystem::createDirectory(const std::string&) {
    // TODO: Implement cross-platform directory creation
    return false;
}

std::vector<std::string> FileSystem::listDirectory(const std::string&) {
    // TODO: Implement directory listing
    return {};
}

std::string FileSystem::getExtension(const std::string& path) {
    size_t pos = path.find_last_of('.');
    return (pos != std::string::npos) ? path.substr(pos) : "";
}

std::string FileSystem::getBaseName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(pos + 1) : path;
}

std::string FileSystem::getDirName(const std::string& path) {
    size_t pos = path.find_last_of("/\\");
    return (pos != std::string::npos) ? path.substr(0, pos) : ".";
}

} // namespace Util
