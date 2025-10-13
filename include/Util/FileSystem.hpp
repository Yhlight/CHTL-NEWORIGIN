#pragma once

#include "../Common.hpp"

namespace CHTL::Util {

class FileSystem {
public:
    // 读取文件内容
    static Opt<String> readFile(const Path& path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
    
    // 写入文件内容
    static bool writeFile(const Path& path, const String& content) {
        // 确保父目录存在
        std::filesystem::create_directories(path.parent_path());
        
        std::ofstream file(path);
        if (!file.is_open()) {
            return false;
        }
        
        file << content;
        return true;
    }
    
    // 检查文件是否存在
    static bool exists(const Path& path) {
        return std::filesystem::exists(path);
    }
    
    // 检查是否为目录
    static bool isDirectory(const Path& path) {
        return std::filesystem::is_directory(path);
    }
    
    // 检查是否为文件
    static bool isFile(const Path& path) {
        return std::filesystem::is_regular_file(path);
    }
    
    // 获取文件扩展名
    static String getExtension(const Path& path) {
        return path.extension().string();
    }
    
    // 获取文件名（不含扩展名）
    static String getStem(const Path& path) {
        return path.stem().string();
    }
    
    // 获取文件名（含扩展名）
    static String getFilename(const Path& path) {
        return path.filename().string();
    }
    
    // 列出目录中的所有文件
    static Vec<Path> listFiles(const Path& directory, bool recursive = false) {
        Vec<Path> files;
        
        if (!std::filesystem::exists(directory)) {
            return files;
        }
        
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path());
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(directory)) {
                if (entry.is_regular_file()) {
                    files.push_back(entry.path());
                }
            }
        }
        
        return files;
    }
    
    // 创建目录
    static bool createDirectories(const Path& path) {
        return std::filesystem::create_directories(path);
    }
    
    // 获取绝对路径
    static Path getAbsolutePath(const Path& path) {
        return std::filesystem::absolute(path);
    }
    
    // 路径拼接
    static Path joinPath(const Path& base, const String& relative) {
        return base / relative;
    }
};

} // namespace CHTL::Util
