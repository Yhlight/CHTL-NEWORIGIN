#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace CHTL {

class FileSystem {
public:
    // Reads the entire content of a file into a string.
    static std::string readFile(const std::filesystem::path& path);

    // Writes a string to a file, overwriting it if it exists.
    static void writeFile(const std::filesystem::path& path, const std::string& content);

    // Checks if a file or directory exists.
    static bool exists(const std::filesystem::path& path);

    // Checks if a path is a regular file.
    static bool isFile(const std::filesystem::path& path);

    // Checks if a path is a directory.
    static bool isDirectory(const std::filesystem::path& path);

    // Creates a directory, including any necessary parent directories.
    static bool createDirectory(const std::filesystem::path& path);

    // Gets a list of all files in a directory (non-recursive).
    static std::vector<std::filesystem::path> getFiles(const std::filesystem::path& directory);

    // Gets a list of all subdirectories in a directory (non-recursive).
    static std::vector<std::filesystem::path> getDirectories(const std::filesystem::path& directory);
};

} // namespace CHTL
