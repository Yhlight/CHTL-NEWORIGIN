#pragma once

#include <string>
#include <vector>

namespace CHTL {
namespace FileSystem {

// Reads the entire content of a file into a string.
// Returns an empty string if the file cannot be read.
std::string readFileToString(const std::string& filePath);

// Writes a string to a file, overwriting the file if it exists.
// Returns true on success, false on failure.
bool writeFileFromString(const std::string& filePath, const std::string& content);

} // namespace FileSystem
} // namespace CHTL
