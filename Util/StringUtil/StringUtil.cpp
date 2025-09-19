#include "StringUtil.h"
#include <cctype>
#include <functional>

namespace CHTL {
namespace Util {

std::vector<std::string> StringUtil::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    if (str.empty() || delimiter.empty()) {
        result.push_back(str);
        return result;
    }
    
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

std::string StringUtil::trim(const std::string& str) {
    return ltrim(rtrim(str));
}

std::string StringUtil::ltrim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtil::rtrim(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

std::string StringUtil::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtil::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool StringUtil::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtil::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

std::string StringUtil::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = result.find(from);
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    return result;
}

std::string StringUtil::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    return result;
}

bool StringUtil::isEmptyOrWhitespace(const std::string& str) {
    return str.empty() || std::all_of(str.begin(), str.end(), ::isspace);
}

template<typename... Args>
std::string StringUtil::join(const std::string& delimiter, Args&&... args) {
    std::ostringstream oss;
    bool first = true;
    ((oss << (first ? "" : delimiter) << args, first = false), ...);
    return oss.str();
}

template<typename... Args>
std::string StringUtil::format(const std::string& format, Args&&... args) {
    // 简单的格式化实现，实际项目中可能需要更复杂的实现
    std::ostringstream oss;
    size_t pos = 0;
    size_t argIndex = 0;
    
    while (pos < format.length()) {
        size_t placeholder = format.find("{}", pos);
        if (placeholder == std::string::npos) {
            oss << format.substr(pos);
            break;
        }
        
        oss << format.substr(pos, placeholder - pos);
        // 这里需要根据参数类型进行转换，简化实现
        oss << "ARG" << argIndex;
        pos = placeholder + 2;
        argIndex++;
    }
    
    return oss.str();
}

bool StringUtil::isNumber(const std::string& str) {
    if (str.empty()) return false;
    
    size_t start = 0;
    if (str[0] == '-' || str[0] == '+') start = 1;
    
    bool hasDigit = false;
    bool hasDot = false;
    
    for (size_t i = start; i < str.length(); ++i) {
        if (std::isdigit(str[i])) {
            hasDigit = true;
        } else if (str[i] == '.' && !hasDot) {
            hasDot = true;
        } else {
            return false;
        }
    }
    
    return hasDigit;
}

bool StringUtil::isValidIdentifier(const std::string& str) {
    if (str.empty()) return false;
    
    // 第一个字符必须是字母或下划线
    if (!std::isalpha(str[0]) && str[0] != '_') return false;
    
    // 后续字符可以是字母、数字或下划线
    for (size_t i = 1; i < str.length(); ++i) {
        if (!std::isalnum(str[i]) && str[i] != '_') return false;
    }
    
    return true;
}

std::string StringUtil::escape(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2);
    
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    
    return result;
}

std::string StringUtil::unescape(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '"': result += '"'; i++; break;
                case '\\': result += '\\'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                case 't': result += '\t'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    
    return result;
}

size_t StringUtil::hash(const std::string& str) {
    std::hash<std::string> hasher;
    return hasher(str);
}

bool StringUtil::matches(const std::string& str, const std::string& pattern) {
    try {
        std::regex regex(pattern);
        return std::regex_match(str, regex);
    } catch (const std::regex_error&) {
        return false;
    }
}

std::vector<std::string> StringUtil::extractMatches(const std::string& str, const std::string& pattern) {
    std::vector<std::string> matches;
    try {
        std::regex regex(pattern);
        std::sregex_iterator iter(str.begin(), str.end(), regex);
        std::sregex_iterator end;
        
        for (; iter != end; ++iter) {
            matches.push_back(iter->str());
        }
    } catch (const std::regex_error&) {
        // 正则表达式错误，返回空结果
    }
    
    return matches;
}

} // namespace Util
} // namespace CHTL