#ifndef ZIP_UTIL_H
#define ZIP_UTIL_H

#include <string>
#include <vector>

class ZipUtil {
public:
    // 压缩文件
    static bool compress(const std::string& sourcePath, const std::string& zipPath);
    
    // 解压文件
    static bool extract(const std::string& zipPath, const std::string& destPath);
    
    // 检查是否为ZIP文件
    static bool isZipFile(const std::string& path);
    
    // 获取ZIP文件中的文件列表
    static std::vector<std::string> listFiles(const std::string& zipPath);
};

#endif // ZIP_UTIL_H