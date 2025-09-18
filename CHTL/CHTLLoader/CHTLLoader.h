#pragma once

#include <string>
#include <vector>

namespace CHTL {

// The CHTLLoader is responsible for handling file I/O for imports.
// It resolves paths and reads file contents.
class CHTLLoader {
public:
    // Reads the entire content of a file into a string.
    // Throws a runtime_error if the file cannot be opened.
    static std::string readFile(const std::string& filepath);
};

} // namespace CHTL
