#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include "../CHTLNode/ImportNode.h" // Include ImportNode for context

class ModuleResolver {
public:
    // Takes an ImportNode and the directory of the file containing the import.
    // This provides full context for path resolution (e.g., knowing to look
    // for .css files for an @Style import).
    // Returns a vector of absolute paths to the file(s) that should be loaded.
    std::vector<std::filesystem::path> resolve(const ImportNode& node, const std::filesystem::path& currentDirectory);

private:
    // Helper to search for a file in a specific directory with context-aware extension resolution.
    std::filesystem::path findInDirectory(const std::filesystem::path& dir, const ImportNode& node);
};