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

    // Handle wildcard imports
    if (rawPath.back() == '*') {
        fs::path basePath(rawPath.substr(0, rawPath.length() - 1));
        if (fs::is_directory(basePath)) {
            for (const auto& entry : fs::directory_iterator(basePath)) {
                if (entry.is_regular_file() && (entry.path().extension() == ".chtl" || entry.path().extension() == ".cmod")) {
                    resolvedPaths.push_back(fs::absolute(entry.path()));
                }
            }
        }
        return resolvedPaths;
    }

    // Define search directories in order of precedence
    std::vector<fs::path> searchDirs;
    // 1. Official modules directory (placeholder)
    // searchDirs.push_back(get_official_module_dir());
    // 2. Local modules directory
    searchDirs.push_back(currentDirectory / "module");
    // 3. Current directory
    searchDirs.push_back(currentDirectory);

    for (const auto& dir : searchDirs) {
        fs::path foundPath = findInDirectory(dir, rawPath);
        if (!foundPath.empty()) {
            resolvedPaths.push_back(foundPath);
            return resolvedPaths; // Stop after the first match for non-wildcard paths
        }
    }

    return resolvedPaths; // Return empty vector if not found
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