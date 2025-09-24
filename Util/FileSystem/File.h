#ifndef CHTL_FILE_H
#define CHTL_FILE_H

#include <string>
#include <optional>

namespace CHTL {
namespace Util {

// Reads the entire content of a file into a string.
// Returns an empty optional if the file cannot be opened.
std::optional<std::string> ReadFile(const std::string& filepath);

// Writes a string to a file.
// Returns true on success, false on failure.
bool WriteFile(const std::string& filepath, const std::string& content);

} // namespace Util
} // namespace CHTL

#endif // CHTL_FILE_H
