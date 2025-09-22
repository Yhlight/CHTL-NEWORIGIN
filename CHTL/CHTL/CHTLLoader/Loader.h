#ifndef CHTL_LOADER_H
#define CHTL_LOADER_H

#include <string>
#include <vector>
#include <optional>

namespace CHTL {

class Loader {
public:
    // Attempts to load a file and returns its content.
    // Returns an empty optional if the file cannot be read.
    std::optional<std::string> loadFile(const std::string& path);
};

} // namespace CHTL

#endif // CHTL_LOADER_H
