#pragma once

#include <string>
#include <vector>

namespace CHTL {

/**
 * @brief CHTL Loader - Handles file loading and dependency management
 */
class CHTLLoader {
public:
    CHTLLoader();
    ~CHTLLoader();
    
    /**
     * @brief Initialize loader
     */
    void initialize();
    
    /**
     * @brief Load file content
     * @param filePath Path to file
     * @return File content
     */
    std::string loadFile(const std::string& filePath);
    
    /**
     * @brief Check if file exists
     * @param filePath Path to file
     * @return True if exists, false otherwise
     */
    bool fileExists(const std::string& filePath) const;
    
    /**
     * @brief Get dependencies from content
     * @param content File content
     * @return List of dependencies
     */
    std::vector<std::string> getDependencies(const std::string& content);

private:
    // Implementation details
};

} // namespace CHTL