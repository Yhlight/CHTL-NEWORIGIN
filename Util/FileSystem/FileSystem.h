#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <memory>

namespace CHTL::Util {

class FileSystem {
public:
    // File operations
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static bool createDirectory(const std::string& path);
    static bool removeFile(const std::string& path);
    static bool removeDirectory(const std::string& path);
    
    // Directory operations
    static std::vector<std::string> listFiles(const std::string& path);
    static std::vector<std::string> listDirectories(const std::string& path);
    static std::vector<std::string> listAll(const std::string& path, bool recursive = false);
    
    // Path operations
    static std::string getCurrentDirectory();
    static std::string getAbsolutePath(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string joinPath(const std::string& base, const std::string& relative);
    
    // Search operations
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    static std::vector<std::string> findFilesRecursive(const std::string& directory, const std::string& pattern);
    
    // Module path operations
    static std::string getModulePath(const std::string& moduleName);
    static std::string getOfficialModulePath();
    static std::string getUserModulePath(const std::string& basePath);
    static std::vector<std::string> searchModulePaths(const std::string& moduleName, const std::string& basePath);
    
private:
    static std::filesystem::path toStdPath(const std::string& path);
    static std::string fromStdPath(const std::filesystem::path& path);
};

} // namespace CHTL::Util