#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace CHTL::Util {

/**
 * @class FileSystemUtil
 * @brief Provides a collection of static utility functions for file system operations.
 */
class FileSystemUtil {
public:
    /**
     * @brief Reads the entire content of a file into a string.
     * @param path The path to the file.
     * @return The content of the file as a string.
     * @throws std::runtime_error if the file cannot be opened.
     */
    static std::string readFile(const std::filesystem::path& path);

    /**
     * @brief Writes a string to a file, overwriting the file if it exists.
     * @param path The path to the file.
     * @param content The string content to write to the file.
     * @throws std::runtime_error if the file cannot be opened for writing.
     */
    static void writeFile(const std::filesystem::path& path, const std::string& content);
};

} // namespace CHTL::Util
