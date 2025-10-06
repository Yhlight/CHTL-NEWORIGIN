#ifndef CHTL_STRING_UTIL_H
#define CHTL_STRING_UTIL_H

#include "../../Common.h"

namespace CHTL {
namespace Util {

class StringUtil {
public:
    // 去除首尾空白
    static String trim(const String& str);
    static String trimLeft(const String& str);
    static String trimRight(const String& str);
    
    // 字符串分割
    static Vector<String> split(const String& str, char delimiter);
    static Vector<String> split(const String& str, const String& delimiter);
    
    // 字符串拼接
    static String join(const Vector<String>& strings, const String& separator);
    
    // 大小写转换
    static String toUpper(const String& str);
    static String toLower(const String& str);
    
    // 字符串判断
    static bool startsWith(const String& str, const String& prefix);
    static bool endsWith(const String& str, const String& suffix);
    static bool contains(const String& str, const String& substr);
    
    // 字符串替换
    static String replace(const String& str, const String& from, const String& to);
    static String replaceAll(const String& str, const String& from, const String& to);
    
    // 空白字符判断
    static bool isWhitespace(char c);
    static bool isAllWhitespace(const String& str);
    
    // 数字字面量判断
    static bool isDigit(char c);
    static bool isNumber(const String& str);
    
    // 标识符判断
    static bool isAlpha(char c);
    static bool isAlphaNumeric(char c);
    static bool isIdentifierStart(char c);
    static bool isIdentifierChar(char c);
    
    // 转义处理
    static String escape(const String& str);
    static String unescape(const String& str);
    
    // 格式化
    template<typename... Args>
    static String format(const String& fmt, Args&&... args);
};

} // namespace Util
} // namespace CHTL

#endif // CHTL_STRING_UTIL_H
