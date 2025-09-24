#include "FileSystem.h"
#include <stdexcept>
#include <algorithm>

namespace CHTL::Util {

bool FileSystem::exists(const std::string& path) {
    try {
        return std::filesystem::exists(toStdPath(path));
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::isFile(const std::string& path) {
    try {
        return std::filesystem::is_regular_file(toStdPath(path));
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::isDirectory(const std::string& path) {
    try {
        return std::filesystem::is_directory(toStdPath(path));
    } catch (const std::exception&) {
        return false;
    }
}

std::string FileSystem::readFile(const std::string& path) {
    try {
        std::ifstream file(toStdPath(path), std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file: " + path);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        return content;
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading file " + path + ": " + e.what());
    }
}

bool FileSystem::writeFile(const std::string& path, const std::string& content) {
    try {
        std::ofstream file(toStdPath(path), std::ios::binary);
        if (!file.is_open()) {
            return false;
        }
        
        file.write(content.c_str(), content.size());
        return file.good();
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::createDirectory(const std::string& path) {
    try {
        return std::filesystem::create_directories(toStdPath(path));
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::removeFile(const std::string& path) {
    try {
        return std::filesystem::remove(toStdPath(path));
    } catch (const std::exception&) {
        return false;
    }
}

bool FileSystem::removeDirectory(const std::string& path) {
    try {
        return std::filesystem::remove_all(toStdPath(path)) > 0;
    } catch (const std::exception&) {
        return false;
    }
}

std::vector<std::string> FileSystem::listFiles(const std::string& path) {
    std::vector<std::string> files;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(toStdPath(path))) {
            if (entry.is_regular_file()) {
                files.push_back(fromStdPath(entry.path()));
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    return files;
}

std::vector<std::string> FileSystem::listDirectories(const std::string& path) {
    std::vector<std::string> dirs;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(toStdPath(path))) {
            if (entry.is_directory()) {
                dirs.push_back(fromStdPath(entry.path()));
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    return dirs;
}

std::vector<std::string> FileSystem::listAll(const std::string& path, bool recursive) {
    std::vector<std::string> all;
    try {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(toStdPath(path))) {
                all.push_back(fromStdPath(entry.path()));
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(toStdPath(path))) {
                all.push_back(fromStdPath(entry.path()));
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    return all;
}

std::string FileSystem::getCurrentDirectory() {
    try {
        return fromStdPath(std::filesystem::current_path());
    } catch (const std::exception&) {
        return ".";
    }
}

std::string FileSystem::getAbsolutePath(const std::string& path) {
    try {
        return fromStdPath(std::filesystem::absolute(toStdPath(path)));
    } catch (const std::exception&) {
        return path;
    }
}

std::string FileSystem::getDirectoryName(const std::string& path) {
    try {
        return fromStdPath(toStdPath(path).parent_path());
    } catch (const std::exception&) {
        return ".";
    }
}

std::string FileSystem::getFileName(const std::string& path) {
    try {
        return fromStdPath(toStdPath(path).filename());
    } catch (const std::exception&) {
        return path;
    }
}

std::string FileSystem::getFileExtension(const std::string& path) {
    try {
        return fromStdPath(toStdPath(path).extension());
    } catch (const std::exception&) {
        return "";
    }
}

std::string FileSystem::joinPath(const std::string& base, const std::string& relative) {
    try {
        return fromStdPath(toStdPath(base) / relative);
    } catch (const std::exception&) {
        return base + "/" + relative;
    }
}

std::vector<std::string> FileSystem::findFiles(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> matches;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(toStdPath(directory))) {
            if (entry.is_regular_file()) {
                std::string filename = fromStdPath(entry.path().filename());
                // Simple pattern matching (can be enhanced with regex)
                if (filename.find(pattern) != std::string::npos) {
                    matches.push_back(fromStdPath(entry.path()));
                }
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    return matches;
}

std::vector<std::string> FileSystem::findFilesRecursive(const std::string& directory, const std::string& pattern) {
    std::vector<std::string> matches;
    try {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(toStdPath(directory))) {
            if (entry.is_regular_file()) {
                std::string filename = fromStdPath(entry.path().filename());
                if (filename.find(pattern) != std::string::npos) {
                    matches.push_back(fromStdPath(entry.path()));
                }
            }
        }
    } catch (const std::exception&) {
        // Return empty vector on error
    }
    return matches;
}

std::string FileSystem::getModulePath(const std::string& moduleName) {
    // Implementation for module path resolution
    return "";
}

std::string FileSystem::getOfficialModulePath() {
    // Implementation for official module path
    return "";
}

std::string FileSystem::getUserModulePath(const std::string& basePath) {
    return joinPath(basePath, "module");
}

std::vector<std::string> FileSystem::searchModulePaths(const std::string& moduleName, const std::string& basePath) {
    std::vector<std::string> paths;
    
    // Search in user module directory
    std::string userModuleDir = getUserModulePath(basePath);
    if (exists(userModuleDir)) {
        auto files = findFiles(userModuleDir, moduleName);
        paths.insert(paths.end(), files.begin(), files.end());
    }
    
    // Search in official module directory
    std::string officialModuleDir = getOfficialModulePath();
    if (!officialModuleDir.empty() && exists(officialModuleDir)) {
        auto files = findFiles(officialModuleDir, moduleName);
        paths.insert(paths.end(), files.begin(), files.end());
    }
    
    return paths;
}

std::filesystem::path FileSystem::toStdPath(const std::string& path) {
    return std::filesystem::path(path);
}

std::string FileSystem::fromStdPath(const std::filesystem::path& path) {
    return path.string();
}

} // namespace CHTL::Util