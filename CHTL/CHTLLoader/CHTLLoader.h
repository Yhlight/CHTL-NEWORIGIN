#pragma once

#include <string>
#include <vector>
#include <filesystem>

class CHTLLoader {
public:
    // Given a path and the current file's path, resolves and reads the file.
    static std::string loadFile(const std::string& path, const std::string& currentFilePath);

private:
    // Helper to resolve the full path
    static std::filesystem::path resolvePath(const std::string& path, const std::string& currentFilePath);
};
