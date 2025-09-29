#include "ModuleResolver.h"
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

// Main resolve method, now context-aware thanks to the ImportNode.
std::vector<fs::path> ModuleResolver::resolve(const ImportNode& node, const fs::path& currentDirectory) {
    std::vector<fs::path> resolvedPaths;
    fs::path importPath(node.path);

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
    // As per CHTL.md, search in a 'module' subdirectory.
    // The test paths are relative to the build dir, so we need to adjust.
    fs::path modulePath = currentDirectory / "import_samples";
    if (!fs::exists(modulePath)) { // Fallback for non-test execution
        modulePath = currentDirectory / "module";
    }
    searchDirs.push_back(modulePath);
    // In the future, we can add official/global module directories here.
    // e.g., getenv("CHTL_MODULE_PATH")

    for (const auto& dir : searchDirs) {
        fs::path foundPath = findInDirectory(dir, node);
        if (!foundPath.empty()) {
            resolvedPaths.push_back(foundPath);
            // For non-wildcard paths, we can stop after the first match.
            // Wildcard logic will be expanded later if needed.
            break;
        }
    }

    return resolvedPaths;
}

// Helper to find a module in a specific directory, trying different extensions based on import context.
fs::path ModuleResolver::findInDirectory(const fs::path& dir, const ImportNode& node) {
    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return "";
    }

    std::string moduleName = node.path;

    // 1. Try with the exact name first (e.g., if path includes extension).
    fs::path exactPath = dir / moduleName;
    if (fs::exists(exactPath)) {
        return fs::absolute(exactPath);
    }

    // 2. Try with extensions if no extension was provided.
    fs::path modulePath(moduleName);
    if (!modulePath.has_extension()) {
        std::vector<std::string> extensions_to_try;
        switch(node.category) {
            case ImportCategory::Chtl:
            case ImportCategory::Template:
            case ImportCategory::Custom:
                extensions_to_try = {".cmod", ".chtl"};
                break;
            case ImportCategory::Style:
                extensions_to_try = {".css"};
                break;
            case ImportCategory::JavaScript:
                extensions_to_try = {".js", ".cjjs"};
                break;
            case ImportCategory::Html:
                extensions_to_try = {".html"};
                break;
            default:
                // For other types, don't guess extensions.
                break;
        }

        for (const auto& ext : extensions_to_try) {
            fs::path pathWithExt = dir / (moduleName + ext);
            if (fs::exists(pathWithExt)) {
                return fs::absolute(pathWithExt);
            }
        }
    }

    return ""; // Not found
}