#pragma once

#include <string>
#include <vector>
#include <filesystem>

class ModuleResolver {
public:
    // Takes a raw import path (e.g., "MyModule", "./components/Button") and the directory
    // of the file that contains the import statement.
    // Returns a vector of absolute paths to the file(s) that should be loaded.
    // The vector is used to support wildcard imports, which can resolve to multiple files.
    std::vector<std::filesystem::path> resolve(const std::string& rawPath, const std::filesystem::path& currentDirectory);

private:
    // Helper to search for a file in a specific directory with extension resolution.
    std::filesystem::path findInDirectory(const std::filesystem::path& dir, const std::string& moduleName);
};