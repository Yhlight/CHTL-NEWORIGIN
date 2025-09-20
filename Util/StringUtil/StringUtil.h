#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include <string>
#include <vector>

class StringUtil {
public:
    // 字符串分割
    static std::vector<std::string> split(const std::string& str, char delimiter);
    
    // 去除前后空格
    static std::string trim(const std::string& str);
    
    // 字符串替换
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    
    // 检查是否以指定字符串开头
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // 检查是否以指定字符串结尾
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // 转换为小写
    static std::string toLower(const std::string& str);
    
    // 转换为大写
    static std::string toUpper(const std::string& str);
};

#endif // STRING_UTIL_H