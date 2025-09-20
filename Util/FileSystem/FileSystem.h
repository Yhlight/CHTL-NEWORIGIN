#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

class FileSystem {
public:
    // 检查文件是否存在
    static bool exists(const std::string& path);
    
    // 检查是否为目录
    static bool isDirectory(const std::string& path);
    
    // 创建目录
    static bool createDirectory(const std::string& path);
    
    // 读取文件内容
    static std::string readFile(const std::string& path);
    
    // 写入文件内容
    static bool writeFile(const std::string& path, const std::string& content);
    
    // 获取文件扩展名
    static std::string getExtension(const std::string& path);
    
    // 获取文件名（不含路径）
    static std::string getFileName(const std::string& path);
    
    // 获取目录路径
    static std::string getDirectory(const std::string& path);
};

#endif // FILE_SYSTEM_H