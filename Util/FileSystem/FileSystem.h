#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace CHTL {
namespace Util {

class FileSystem {
public:
    // 检查文件是否存在
    static bool exists(const std::string& path);
    
    // 检查是否为目录
    static bool isDirectory(const std::string& path);
    
    // 检查是否为文件
    static bool isFile(const std::string& path);
    
    // 创建目录
    static bool createDirectory(const std::string& path);
    
    // 创建多级目录
    static bool createDirectories(const std::string& path);
    
    // 删除文件
    static bool removeFile(const std::string& path);
    
    // 删除目录
    static bool removeDirectory(const std::string& path);
    
    // 复制文件
    static bool copyFile(const std::string& from, const std::string& to);
    
    // 移动文件
    static bool moveFile(const std::string& from, const std::string& to);
    
    // 获取文件大小
    static size_t getFileSize(const std::string& path);
    
    // 获取文件扩展名
    static std::string getFileExtension(const std::string& path);
    
    // 获取文件名（不含路径）
    static std::string getFileName(const std::string& path);
    
    // 获取文件名（不含扩展名）
    static std::string getFileNameWithoutExtension(const std::string& path);
    
    // 获取目录路径
    static std::string getDirectoryPath(const std::string& path);
    
    // 获取绝对路径
    static std::string getAbsolutePath(const std::string& path);
    
    // 获取相对路径
    static std::string getRelativePath(const std::string& path, const std::string& base);
    
    // 列出目录内容
    static std::vector<std::string> listDirectory(const std::string& path);
    
    // 递归列出目录内容
    static std::vector<std::string> listDirectoryRecursive(const std::string& path);
    
    // 查找文件
    static std::vector<std::string> findFiles(const std::string& directory, const std::string& pattern);
    
    // 读取文件内容
    static std::string readFile(const std::string& path);
    
    // 读取文件内容（二进制）
    static std::vector<uint8_t> readFileBinary(const std::string& path);
    
    // 写入文件内容
    static bool writeFile(const std::string& path, const std::string& content);
    
    // 写入文件内容（二进制）
    static bool writeFileBinary(const std::string& path, const std::vector<uint8_t>& content);
    
    // 追加文件内容
    static bool appendFile(const std::string& path, const std::string& content);
    
    // 获取文件修改时间
    static std::time_t getLastModifiedTime(const std::string& path);
    
    // 设置文件修改时间
    static bool setLastModifiedTime(const std::string& path, std::time_t time);
    
    // 获取当前工作目录
    static std::string getCurrentDirectory();
    
    // 设置当前工作目录
    static bool setCurrentDirectory(const std::string& path);
    
    // 路径规范化
    static std::string normalizePath(const std::string& path);
    
    // 路径连接
    static std::string joinPath(const std::string& path1, const std::string& path2);
    
    // 路径连接（多个参数）
    template<typename... Args>
    static std::string joinPath(const std::string& path, Args&&... args);
    
    // 检查路径是否为绝对路径
    static bool isAbsolutePath(const std::string& path);
    
    // 检查路径是否为相对路径
    static bool isRelativePath(const std::string& path);
    
    // 获取临时目录
    static std::string getTempDirectory();
    
    // 创建临时文件
    static std::string createTempFile(const std::string& prefix = "chtl_", const std::string& suffix = ".tmp");
    
    // 创建临时目录
    static std::string createTempDirectory(const std::string& prefix = "chtl_");
    
    // 清理临时文件
    static void cleanupTempFiles();
    
private:
    static std::vector<std::string> tempFiles;
    static std::vector<std::string> tempDirectories;
};

} // namespace Util
} // namespace CHTL