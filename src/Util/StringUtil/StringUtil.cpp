#include "../../../include/CHTLCommon.h"
#include <algorithm>
#include <cctype>
#include <regex>

namespace CHTL {

String StringUtil::trim(const String& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == String::npos) {
        return "";
    }
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

StringVector StringUtil::split(const String& str, char delimiter) {
    StringVector result;
    String current;
    
    for (char c : str) {
        if (c == delimiter) {
            if (!current.empty()) {
                result.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        result.push_back(current);
    }
    
    return result;
}

bool StringUtil::startsWith(const String& str, const String& prefix) {
    return str.length() >= prefix.length() && 
           str.substr(0, prefix.length()) == prefix;
}

bool StringUtil::endsWith(const String& str, const String& suffix) {
    return str.length() >= suffix.length() && 
           str.substr(str.length() - suffix.length()) == suffix;
}

String StringUtil::toLowerCase(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

String StringUtil::toUpperCase(const String& str) {
    String result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool StringUtil::isIdentifier(const String& str) {
    if (str.empty() || !std::isalpha(str[0]) && str[0] != '_') {
        return false;
    }
    
    for (char c : str) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool StringUtil::isNumber(const String& str) {
    if (str.empty()) return false;
    
    size_t pos = 0;
    try {
        std::stod(str, &pos);
        return pos == str.length();
    } catch (...) {
        return false;
    }
}

String StringUtil::escapeString(const String& str) {
    String result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\'': result += "\\'"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

String StringUtil::unescapeString(const String& str) {
    String result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; break;
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += str[i + 1]; break;
            }
            ++i; // Skip next character
        } else {
            result += str[i];
        }
    }
    return result;
}

} // namespace CHTL