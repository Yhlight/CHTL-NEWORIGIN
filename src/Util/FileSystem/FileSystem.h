/**
 * @file FileSystem.h
 * @brief File system utilities
 */

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

namespace Util {

class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool createDirectory(const std::string& path);
    static std::vector<std::string> listDirectory(const std::string& path);
    static std::string getExtension(const std::string& path);
    static std::string getBaseName(const std::string& path);
    static std::string getDirName(const std::string& path);
};

} // namespace Util

#endif // FILESYSTEM_H
