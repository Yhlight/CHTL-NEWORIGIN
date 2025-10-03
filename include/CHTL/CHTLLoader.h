#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL file loader for handling file operations
 */
class CHTLLoader {
public:
    CHTLLoader();
    ~CHTLLoader() = default;

    // File operations
    std::string loadFile(const std::string& filePath) const;
    bool fileExists(const std::string& filePath) const;
    std::vector<std::string> getFileList(const std::string& directory) const;

private:
    // Implementation details
};

} // namespace CHTL