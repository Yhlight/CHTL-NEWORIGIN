#include "ModuleResolver.h"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Main resolve method
std::vector<fs::path> ModuleResolver::resolve(const std::string& rawPath, const fs::path& currentDirectory) {
    std::vector<fs::path> resolvedPaths;
    fs::path importPath(rawPath);

    // If the path is absolute, just check if it exists.
    if (importPath.is_absolute()) {
        if (fs::exists(importPath)) {
            resolvedPaths.push_back(importPath);
        }
        return resolvedPaths;
    }

    // Define search directories relative to the current file's location.
    std::vector<fs::path> searchDirs;
    searchDirs.push_back(currentDirectory);
    searchDirs.push_back(currentDirectory / "modules");
    // In the future, we can add official/global module directories here.

    for (const auto& dir : searchDirs) {
        fs::path foundPath = findInDirectory(dir, rawPath);
        if (!foundPath.empty()) {
            resolvedPaths.push_back(foundPath);
            // For non-wildcard paths, we can stop after the first match.
            break;
        }
    }

    return resolvedPaths;
}

// Helper to find a module in a specific directory, trying different extensions.
fs::path ModuleResolver::findInDirectory(const fs::path& dir, const std::string& moduleName) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return "";
    }

    // 1. Try with the exact name first.
    fs::path exactPath = dir / moduleName;
    if (fs::exists(exactPath)) {
        return fs::absolute(exactPath);
    }

    // 2. Try with extensions if no extension was provided.
    fs::path modulePath(moduleName);
    if (!modulePath.has_extension()) {
        // Prefer .cmod over .chtl
        fs::path cmodPath = dir / (moduleName + ".cmod");
        if (fs::exists(cmodPath)) {
            return fs::absolute(cmodPath);
        }

        fs::path chtlPath = dir / (moduleName + ".chtl");
        if (fs::exists(chtlPath)) {
            return fs::absolute(chtlPath);
        }
    }

    return ""; // Not found
}