#pragma once

#include "../Common.hpp"

namespace CHTL::Util {

class StringUtil {
public:
    // 去除字符串首尾空白
    static String trim(const String& str) {
        auto start = str.begin();
        while (start != str.end() && std::isspace(*start)) {
            ++start;
        }
        
        auto end = str.end();
        do {
            --end;
        } while (std::distance(start, end) > 0 && std::isspace(*end));
        
        return String(start, end + 1);
    }
    
    // 分割字符串
    static Vec<String> split(const String& str, char delimiter) {
        Vec<String> result;
        std::stringstream ss(str);
        String item;
        
        while (std::getline(ss, item, delimiter)) {
            result.push_back(item);
        }
        
        return result;
    }
    
    // 检查是否以某个前缀开头
    static bool startsWith(const String& str, const String& prefix) {
        return str.size() >= prefix.size() && 
               str.compare(0, prefix.size(), prefix) == 0;
    }
    
    // 检查是否以某个后缀结尾
    static bool endsWith(const String& str, const String& suffix) {
        return str.size() >= suffix.size() && 
               str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    // 替换所有出现的子串
    static String replaceAll(String str, const String& from, const String& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != String::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
    
    // 转换为小写
    static String toLower(String str) {
        std::transform(str.begin(), str.end(), str.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return str;
    }
    
    // 转换为大写
    static String toUpper(String str) {
        std::transform(str.begin(), str.end(), str.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return str;
    }
    
    // 检查是否为数字
    static bool isNumeric(const String& str) {
        return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
    }
    
    // 检查是否为标识符（字母开头，后跟字母数字下划线）
    static bool isIdentifier(const String& str) {
        if (str.empty() || (!std::isalpha(str[0]) && str[0] != '_')) {
            return false;
        }
        return std::all_of(str.begin() + 1, str.end(),
                          [](char c) { return std::isalnum(c) || c == '_'; });
    }
    
    // 连接字符串
    static String join(const Vec<String>& strings, const String& delimiter) {
        if (strings.empty()) return "";
        
        std::ostringstream oss;
        oss << strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            oss << delimiter << strings[i];
        }
        return oss.str();
    }
};

} // namespace CHTL::Util
