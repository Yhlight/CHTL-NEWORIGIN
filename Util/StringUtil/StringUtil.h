#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Util {

class StringUtil {
public:
    // 字符串分割
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    
    // 去除前后空白字符
    static std::string trim(const std::string& str);
    
    // 去除左侧空白字符
    static std::string ltrim(const std::string& str);
    
    // 去除右侧空白字符
    static std::string rtrim(const std::string& str);
    
    // 转换为小写
    static std::string toLower(const std::string& str);
    
    // 转换为大写
    static std::string toUpper(const std::string& str);
    
    // 检查字符串是否以指定前缀开始
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // 检查字符串是否以指定后缀结束
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // 替换字符串
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    
    // 替换所有匹配的字符串
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // 检查字符串是否为空或只包含空白字符
    static bool isEmptyOrWhitespace(const std::string& str);
    
    // 字符串连接
    template<typename... Args>
    static std::string join(const std::string& delimiter, Args&&... args);
    
    // 字符串格式化
    template<typename... Args>
    static std::string format(const std::string& format, Args&&... args);
    
    // 检查是否为数字
    static bool isNumber(const std::string& str);
    
    // 检查是否为有效的标识符
    static bool isValidIdentifier(const std::string& str);
    
    // 转义特殊字符
    static std::string escape(const std::string& str);
    
    // 反转义特殊字符
    static std::string unescape(const std::string& str);
    
    // 计算字符串哈希值
    static size_t hash(const std::string& str);
    
    // 检查字符串是否匹配正则表达式
    static bool matches(const std::string& str, const std::string& pattern);
    
    // 提取匹配的子字符串
    static std::vector<std::string> extractMatches(const std::string& str, const std::string& pattern);
};

} // namespace Util
} // namespace CHTL