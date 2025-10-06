#ifndef CHTL_FILESYSTEM_H
#define CHTL_FILESYSTEM_H

#include "../../Common.h"
#include <fstream>

namespace CHTL {
namespace Util {

class FileSystem {
public:
    // 文件读取
    static String readFile(const String& path);
    static Vector<String> readLines(const String& path);
    
    // 文件写入
    static bool writeFile(const String& path, const String& content);
    static bool writeLines(const String& path, const Vector<String>& lines);
    
    // 文件追加
    static bool appendFile(const String& path, const String& content);
    
    // 文件存在性检查
    static bool exists(const String& path);
    static bool isFile(const String& path);
    static bool isDirectory(const String& path);
    
    // 文件操作
    static bool createDirectory(const String& path);
    static bool createDirectories(const String& path);
    static bool removeFile(const String& path);
    static bool removeDirectory(const String& path);
    
    // 路径操作
    static String getFileName(const String& path);
    static String getFileNameWithoutExtension(const String& path);
    static String getExtension(const String& path);
    static String getDirectory(const String& path);
    static String joinPath(const String& path1, const String& path2);
    static String normalizePath(const String& path);
    static String absolutePath(const String& path);
    
    // 文件搜索
    static Vector<String> listFiles(const String& directory, bool recursive = false);
    static Vector<String> findFiles(const String& directory, const String& pattern, bool recursive = true);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_FILESYSTEM_H
