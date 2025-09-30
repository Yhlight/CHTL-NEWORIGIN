#include "ModuleResolver.h"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Main resolve method
std::vector<fs::path> ModuleResolver::resolve(const std::string& rawPath, const fs::path& currentDirectory) {
    std::vector<fs::path> resolvedPaths;
    std::string moduleName = rawPath;
    bool official_only = false;

    // Handle official module prefix "chtl::"
    std::string official_prefix = "chtl::";
    if (rawPath.rfind(official_prefix, 0) == 0) { // A simplified starts_with check
        moduleName = rawPath.substr(official_prefix.length());
        official_only = true;
    }

    fs::path importPath(moduleName);

    // If the path is absolute, just check if it exists.
    if (importPath.is_absolute()) {
        if (fs::exists(importPath)) {
            resolvedPaths.push_back(importPath);
        }
        return resolvedPaths;
    }

    // Handle wildcard imports
    if (moduleName.back() == '*') {
        fs::path basePath(moduleName.substr(0, moduleName.length() - 1));
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
    fs::path official_dir = "/app/official_modules";

    if (official_only) {
        searchDirs.push_back(official_dir);
    } else {
        // 1. Official modules directory (now implemented)
        searchDirs.push_back(official_dir);
        // 2. Local modules directory
        searchDirs.push_back(currentDirectory / "module");
        // 3. Current directory
        searchDirs.push_back(currentDirectory);
    }


    for (const auto& dir : searchDirs) {
        fs::path foundPath = findInDirectory(dir, moduleName);
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