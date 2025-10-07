#ifndef CHTL_STRING_UTIL_H
#define CHTL_STRING_UTIL_H

#include "../../Common.h"
#include <algorithm>
#include <sstream>

namespace CHTL {
namespace Util {

class StringUtil {
public:
    // 去除字符串前后空白
    static String trim(const String& str);
    static String trimLeft(const String& str);
    static String trimRight(const String& str);
    
    // 分割字符串
    static Vector<String> split(const String& str, char delimiter);
    static Vector<String> split(const String& str, const String& delimiter);
    
    // 替换字符串
    static String replace(const String& str, const String& from, const String& to);
    static String replaceAll(const String& str, const String& from, const String& to);
    
    // 转大写
    static String toUpper(const String& str);
    
    // 转小写
    static String toLower(const String& str);
    
    // C++17兼容的 starts_with
    static bool startsWith(const String& str, const String& prefix);
    
    // C++17兼容的 ends_with
    static bool endsWith(const String& str, const String& suffix);
    
    // 检查是否包含子串
    static bool contains(const String& str, const String& substr);
    
    // 转义HTML字符
    static String escapeHtml(const String& str);
    
    // 移除字符串中的所有空白
    static String removeWhitespace(const String& str);
    
    // 连接字符串数组
    static String join(const Vector<String>& strings, const String& delimiter);
    
    // 字符检查函数
    static bool isWhitespace(char c);
    static bool isAllWhitespace(const String& str);
    static bool isDigit(char c);
    static bool isNumber(const String& str);
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);
    static bool isIdentifierStart(char c);
    static bool isIdentifierChar(char c);
    
    // 转义和反转义
    static String escape(const String& str);
    static String unescape(const String& str);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_STRING_UTIL_H
